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

#ifndef SVDRP_UTILS_H
#define SVDRP_UTILS_H

/**
 * \file utils.h
 *
 * libsvdrp internal utility functions.
 */

/**
 * \brief Reads a line from a file.
 *
 * \param[in]  fd          the file descriptor to read from
 * \param[out] buf         buffer where to place the data read
 * \param[in]  maxlen      maximum number of characters to read
 *
 * Use to read a full line (up to EOL) from a file.
 */
int readline(int fd, void *buf, int maxlen);

#endif /* SVDRP_UTILS_H */
