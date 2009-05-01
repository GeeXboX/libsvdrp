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

#include "config.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "svdrp.h"
#include "svdrp_internals.h"

#ifdef USE_LOGCOLOR
#define NORMAL   "\033[0m"
#define COLOR(x) "\033[" #x ";1m"
#define BOLD     COLOR(1)
#define F_RED    COLOR(31)
#define F_GREEN  COLOR(32)
#define F_YELLOW COLOR(33)
#define F_BLUE   COLOR(34)
#define B_RED    COLOR(41)
#endif /* USE_LOGCOLOR */

static int
svdrp_log_test (svdrp_t *svdrp, svdrp_verbosity_level_t level)
{
    int verbosity;

    if (!svdrp)
        return 0;

    verbosity = svdrp->verbosity;

    /* do we really want logging ? */
    if (verbosity == SVDRP_MSG_NONE)
        return 0;

    if (level < verbosity)
        return 0;

    return 1;
}

void
svdrp_log (svdrp_t *svdrp, svdrp_verbosity_level_t level, const char *format, ...)
{
#ifdef USE_LOGCOLOR
    static const char const *c[] = {
        [SVDRP_MSG_VERBOSE]  = F_BLUE,
        [SVDRP_MSG_INFO]     = F_GREEN,
        [SVDRP_MSG_WARNING]  = F_YELLOW,
        [SVDRP_MSG_ERROR]    = F_RED,
        [SVDRP_MSG_CRITICAL] = B_RED,
  };
#endif /* USE_LOGCOLOR */
    static const char const *l[] = {
        [SVDRP_MSG_VERBOSE]  = "Verb",
        [SVDRP_MSG_INFO]     = "Info",
        [SVDRP_MSG_WARNING]  = "Warn",
        [SVDRP_MSG_ERROR]    = "Err",
        [SVDRP_MSG_CRITICAL] = "Crit",
    };
    va_list va;

    if (!svdrp || !format)
        return;

    if (!svdrp_log_test (svdrp, level))
        return;

    va_start (va, format);

#ifdef USE_LOGCOLOR
    fprintf (stderr, "[" BOLD "libsvdrp" NORMAL "] %s%s" NORMAL ": ", 
        c[level], l[level]);
#else
    fprintf (stderr, "[libsvdrp] %s: ", l[level]);
#endif /* USE_LOGCOLOR */

    vfprintf (stderr, format, va);
    fprintf (stderr, "\n");
    va_end (va);
}
