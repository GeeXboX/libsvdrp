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

#ifndef SVDRP_LOGS_H
#define SVDRP_LOGS_H

/**
 * \file logs.h
 *
 * libsvdrp internal logging functions.
 */

/**
 * \brief Log a message.
 *
 * \param[in] svdrp        an SVDRP connection object
 * \param[in] level        level of verbosity of the message
 * \param[in] format       format string of the message (like printf)
 * \param[in] ...          items of the format string
 *
 * Records a message in the log. If the log verbosity level is higher than the
 * message level the message will not be displayed.
 */
void svdrp_log (svdrp_t *svdrp, svdrp_verbosity_level_t level, 
           const char *format, ...);

#endif /* SVDRP_LOGS_H */
