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
#include <stdio.h>
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
    char name[256], version[256], charset[256];

    svdrp_log (svdrp, SVDRP_MSG_VERBOSE, __FUNCTION__);

    sscanf(banner, "%s SVDRP VideoDiskRecorder %[^;]; %*[^;]; %s",
           name, version, charset);

    svdrp->name = strdup (name);
    svdrp->version = strdup (version);
    svdrp->charset = strdup (charset);
}

svdrp_reply_code_t svdrp_read_reply(svdrp_t *svdrp)
{
    char *line;
    char strcode[3];
    svdrp_reply_code_t code;
    int read_next;

    if (!svdrp)
        return SVDRP_ERROR;

    svdrp_log (svdrp, SVDRP_MSG_VERBOSE, __FUNCTION__);

    if (!(svdrp->is_connected))
        return SVDRP_ERROR;

    do {
        line = svdrp_read(svdrp);
        strncpy(strcode, line, 3);
        code = atoi (strcode);

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

    if (!host) {
        svdrp_log (svdrp, SVDRP_MSG_ERROR, "Connection failed (gethostbyname)");
        return 0;
    }

    memcpy (&addr.sin_addr.s_addr, host->h_addr, host->h_length);
    addr.sin_family = AF_INET;
    addr.sin_port = htons (svdrp->port);

    svdrp_log (svdrp, SVDRP_MSG_INFO, "Opening connection to %s:%i", svdrp->host, svdrp->port);

    if (connect (s, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        svdrp_log (svdrp, SVDRP_MSG_ERROR, "Connection failed with error %i", errno);
        return 0;
    }

    svdrp->conn = s;
    svdrp->is_connected = 1;

    svdrp_read_reply(svdrp);

    return 1;
}

void svdrp_close_conn (svdrp_t *svdrp)
{
    svdrp_log (svdrp, SVDRP_MSG_VERBOSE, __FUNCTION__);
    svdrp_log (svdrp, SVDRP_MSG_INFO, "Closing connection");

    close (svdrp->conn);

    svdrp->is_connected = 0;
}

int svdrp_send (svdrp_t *svdrp, const char* cmd)
{
    int ret;
    int tries = 0;

    svdrp_log (svdrp, SVDRP_MSG_VERBOSE, __FUNCTION__);

    if (!svdrp)
        return -1;

    if (!(svdrp->is_connected))
        svdrp_open_conn (svdrp);

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
