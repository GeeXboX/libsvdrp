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
#include <unistd.h>

#include "utils.h"

#define MAXLINE 256

static ssize_t read_wrapper(int fd, char *buf)
{
    static int count = 0;
    static char *p;
    static char read_buf[MAXLINE];

    if (count <= 0) {
        int keep_reading = 0;
        do {
            count = read(fd, read_buf, sizeof(read_buf));
            if (count < 0) {
                if (errno == EINTR)
                    keep_reading = 1;
                else
                    return -1;
            } else if (count == 0) {
                return 0;
            }
            p = read_buf;
        } while (keep_reading);
    }

    count--;
    *buf = *p++;
    return 1;
}

int readline(int fd, void *buf, int maxlen)
{
    int n, count;
    char c;
    char *p = buf;

    for (n = 1; n < maxlen; n++) {
        count = read_wrapper(fd, &c);
        if (count == 1) {
            *p++ = c;
            if (c == '\n')
                break;
        } else if (count == 0) {
            if (n == 1)
                return 0;
            else
                break;
        } else {
            return -1;
        }
    }

    *p = 0;
    return n;
}

