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
#include <getopt.h>

#define DEFAULT_WAKEUP_MARGIN 10

int main (int argc, char **argv)
{
    char *hostname = "localhost";
    int port = 2001;
    int timeout = 10;
    svdrp_verbosity_level_t verbosity = SVDRP_MSG_ERROR;
    svdrp_t *svdrp;
    int convert_time = 0;
    struct tm tm = {0};
    time_t time = 0;
    int timer_id = -1;
    int ret = 0;
    char time_str[256];
    int option = -1;
    int wakeup_margin = DEFAULT_WAKEUP_MARGIN;

    const char *const short_options = "v:l:m:h";
    const struct option long_options [] = {
        {"verbose", required_argument, NULL, 'v'},
        {"localtime", no_argument, NULL, 'l'},
        {"help", no_argument, NULL, 'h'},
        {"wakeup-margin", required_argument, NULL, 'm'},
        {0, 0, 0, 0}
    };

    while ((option=getopt_long(argc, argv, short_options, long_options, NULL))>0) {
        switch(option)
        {
            case 'v':
                if (!strcmp(optarg, "none")) verbosity=SVDRP_MSG_NONE;
                else if (!strcmp(optarg, "verbose")) verbosity=SVDRP_MSG_VERBOSE;
                else if (!strcmp(optarg, "info")) verbosity=SVDRP_MSG_INFO;
                else if (!strcmp(optarg, "warning")) verbosity=SVDRP_MSG_WARNING;
                else if (!strcmp(optarg, "error")) verbosity=SVDRP_MSG_ERROR;
                else if (!strcmp(optarg, "critical")) verbosity=SVDRP_MSG_CRITICAL;
                else { fprintf(stderr, "invalid verbosity level: %s\n", optarg); return -1; }
                break;
            case 'l':
                convert_time=1;
                break;
            case 'm':
		fprintf(stderr, "optarg value: %s", optarg);
                //wakeup_margin=atoi(optarg);
                break;
            case 'h':
            default:
                fprintf(stderr, "usage: %s [-h|--help] [-l|--localtime] [-m|--wakeup-margin <minutes>] [-v|--verbose] [none|verbose|info|warning|error|critical]]\n" \
                        "   note: if your hardware clock runs on localtime, use -l for being able to use the output as bios wakeup time.\n", argv[0]);
                return -1;
        }
    }

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
        tm.tm_min -= wakeup_margin;
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
