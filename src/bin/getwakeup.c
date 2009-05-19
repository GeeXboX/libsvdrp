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

#define _GNU_SOURCE
#include <time.h>
#include <stdio.h>
#include <svdrp.h>
#include <string.h>

#define WAKEUP_MARGIN 10

int main (int argc, char **argv)
{
    char *hostname = "localhost";
    int port = 2001;
    int timeout = 10;
    svdrp_verbosity_level_t verbosity = SVDRP_MSG_ERROR;
    svdrp_t *svdrp;
    int convert_time = 0;
    struct tm tm;
    time_t time;
    int timer_id;
    int ret;
    char time_str[256];
    
    svdrp = svdrp_open(hostname, port, timeout, verbosity);
    
    if(!svdrp_is_connected(svdrp)) {
        fprintf(stderr, "Connection failed\n");
        return 2;
    }

    ret = svdrp_next_timer_event(svdrp, &timer_id, &time);
    if (ret == SVDRP_OK) {
        localtime_r(&time, &tm);
        if (convert_time)
            tm.tm_sec += tm.tm_gmtoff;
        tm.tm_min -= WAKEUP_MARGIN;
        mktime(&tm);
        strftime(time_str, 256, "%s", &tm);
        printf("%s\n", time_str);
        svdrp_close(svdrp);
        return 0;
    } else {
        svdrp_close(svdrp);
        return 1;
    }
}
