/*
 * GeeXboX libsvdrp: an interface to VDR via SVDRP.
 * Copyright (C) 2009 Davide Cavalca <davide@geexbox.org>
 *
 * This file is part of libsvdrp.
 *
 * libsvdrp is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * libsvdrp is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with libsvdrp; if not, write to the Free Software
 * Foundation, Inc, 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <errno.h>
#include <netdb.h>
//#include <stdio.h> //printf
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "svdrp.h"
#include "svdrp_internals.h"
#include "logs.h"
#include "utils.h"

#define SVDRP_MAX_TRIES 10
#define MAXLINE 1024
 
static char* svdrp_read(svdrp_t *svdrp)
{
    char* buf;
    char line[MAXLINE];
    int len;
    
    svdrp_log (svdrp, SVDRP_MSG_VERBOSE, __FUNCTION__);
    
    len = readline (svdrp->conn, &line, MAXLINE);
    
    buf = malloc(len);
    strncpy(buf, line, len);
    
    return buf;
}

static void svdrp_parse_banner(svdrp_t *svdrp, const char *banner)
{
    /* tardis SVDRP VideoDiskRecorder 1.6.0-2; Tue Apr 21 11:50:43 2009; UTF-8 */
    char *mybanner = strdup(banner);
    char *p, *q;
    size_t len;
    
    svdrp_log (svdrp, SVDRP_MSG_VERBOSE, __FUNCTION__);
    
    /* grab VDR name */
    p = mybanner;
    q = strstr(mybanner, " ");
    len = q - p + 1;
    *q = '\0';
    svdrp->name = malloc(len);
    strncpy(svdrp->name, p, len);
    
    /* grab VDR version number */
    p = strstr(q + 1, "VideoDiskRecorder") + strlen("VideoDiskRecorder") + 1;
    q = strstr(q + 1, ";");
    len = q - p + 1;
    *q = '\0';
    svdrp->version = malloc (len);
    strncpy(svdrp->version, p, len);
    
    /* grab VDR charset */
    p = strstr(q + 1, ";") + 2;
    q = p + strlen(p) - 2;
    len = q - p + 1;
    *q = '\0';
    svdrp->charset = malloc(len);
    strncpy(svdrp->charset, p, len);    
    
    free(mybanner);
}

svdrp_reply_code_t svdrp_read_reply(svdrp_t *svdrp)
{
    char *line;
    char strcode[3];
    svdrp_reply_code_t code;
    int read_next;
    
    svdrp_log (svdrp, SVDRP_MSG_VERBOSE, __FUNCTION__);
    
    do {
        line = svdrp_read(svdrp);
        strncpy(strcode, line, 3);
        code = atoi (line);
        
        switch (code)
        {
        case SVDRP_REPLY_HELP:
        case SVDRP_REPLY_EPG_DATA:
        case SVDRP_REPLY_QUIT:
        case SVDRP_REPLY_EPG_START:
        case SVDRP_REPLY_ABORT:
        case SVDRP_REPLY_UNKNOWN_CMD:
        case SVDRP_REPLY_UNKNOWN_PARAM:
        case SVDRP_REPLY_UNIMPEMENTED_CMD:
        case SVDRP_REPLY_UNIMPEMENTED_PARAM:
        case SVDRP_REPLY_ACTION_NOT_TAKEN:
        case SVDRP_REPLY_TRANSACTION_FAILED:
        case SVDRP_REPLY_GRAB_DATA:
        case SVDRP_REPLY_PLUGIN:
            /* not implemented */
            break;
        case SVDRP_REPLY_READY:
            svdrp_parse_banner(svdrp, line + 4);
            svdrp_log (svdrp, SVDRP_MSG_INFO, "Connected to VDR %s on %s (%s)", svdrp->version, svdrp->name, svdrp->charset);
            break;
        case SVDRP_REPLY_OK:
            svdrp_log (svdrp, SVDRP_MSG_INFO, "Operation successfully completed");
            break;
        }
        
        if (line[3] == '-') {
            read_next = 1;
            free(line);
        } else {
            read_next = 0;
        }
    } while (read_next);
    
    svdrp->last_reply_code = code;
    svdrp->last_reply = line + 4;
    
    return code;
}

int svdrp_open_conn (svdrp_t *svdrp)
{
    struct sockaddr_in addr;
    struct hostent *host;
    int s;
    
    svdrp_log (svdrp, SVDRP_MSG_VERBOSE, __FUNCTION__);
    
    if (!svdrp)
        return 0;
    
    s = socket (AF_INET, SOCK_STREAM, 0);
    if (s < 0)
        return 0;
        
    /*
      TODO set socket timeout
      man setsockopt
      man 7 socket
    */
    host = gethostbyname (svdrp->host);
    memcpy (&addr.sin_addr.s_addr, host->h_addr, host->h_length);
    addr.sin_family = AF_INET;
    addr.sin_port = htons (svdrp->port);
    
    svdrp_log (svdrp, SVDRP_MSG_INFO, "Opening connection to %s:%i", svdrp->host, svdrp->port);
    
    if (connect (s, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        svdrp_log (svdrp, SVDRP_MSG_ERROR, "Connection failed with error %i", errno);
        return 0;
    }
    
    svdrp->conn = s;
    
    svdrp_read_reply(svdrp);
    
    return s;
}

void svdrp_close_conn (svdrp_t *svdrp)
{
    svdrp_log (svdrp, SVDRP_MSG_VERBOSE, __FUNCTION__);
    svdrp_log (svdrp, SVDRP_MSG_INFO, "Closing connection");
    
    close (svdrp->conn);
}

int svdrp_send (svdrp_t *svdrp, const char* cmd)
{
    int ret; 
    int tries = 0;  

    svdrp_log (svdrp, SVDRP_MSG_VERBOSE, __FUNCTION__);
    
    if (!svdrp)
        return -1;
        
    if (!(svdrp->conn)) {
        svdrp_open_conn (svdrp);
    }
    
    do {
        char *logcmd = strdup (cmd);
        logcmd[strlen(logcmd) - 1] = '\0'; /* strip newline from logged cmd */
        svdrp_log (svdrp, SVDRP_MSG_INFO, "Sending command: '%s'", logcmd);
        free (logcmd);
        tries++;
        ret = write (svdrp->conn, cmd, strlen (cmd));
    
        if (ret == -1) {
            svdrp_log (svdrp, SVDRP_MSG_WARNING, "Write failed");
            svdrp_close_conn (svdrp);
            svdrp_open_conn (svdrp);
        }
    } while (ret == -1 && tries < SVDRP_MAX_TRIES);
    
    return ret;
}
