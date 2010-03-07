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

#ifndef SVDRP_INTERNALS_H
#define SVDRP_INTERNALS_H

/**
 * \file svdrp_internals.h
 *
 * libsvdrp private API functions.
 */

struct svdrp_s {
    svdrp_verbosity_level_t verbosity;
    char *host;
    int port;
    int timeout;
    int is_connected;
    int conn;
    int last_reply_code;
    char *last_reply;
    char *name;
    char *version;
    char *charset;
};

/* SVDRP Reply Codes
Reply codes are in the format
<Reply code><-|Space><Text><Newline>
In the last line the "-" is replace be a space.

901..999 Plugin specific reply codes
*/

/** \brief SVDRP Reply Codes */
typedef enum svdrp_reply_code {
    SVDRP_REPLY_HELP               = 214, /**< Help message */
    SVDRP_REPLY_EPG_DATA           = 215, /**< EPG or recording data record */
    SVDRP_REPLY_GRAB_DATA          = 216, /**< Image grab data (base 64) */
    SVDRP_REPLY_READY              = 220, /**< VDR service ready */
    SVDRP_REPLY_QUIT               = 221, /**< VDR service closing transmission
                                           *   channel */
    SVDRP_REPLY_OK                 = 250, /**< Requested VDR action okay,
                                           *   completed */
    SVDRP_REPLY_EPG_START          = 354, /**< Start sending EPG data */
    SVDRP_REPLY_ABORT              = 451, /**< Requested action aborted: local
                                           *   error in processing */
    SVDRP_REPLY_UNKNOWN_CMD        = 500, /**< Syntax error, command
                                           *   unrecognized */
    SVDRP_REPLY_UNKNOWN_PARAM      = 501, /**< Syntax error in parameters or
                                           *   arguments */
    SVDRP_REPLY_UNIMPEMENTED_CMD   = 502, /**< Command not implemented */
    SVDRP_REPLY_UNIMPEMENTED_PARAM = 504, /**< Command parameter not
                                           *   implemented */
    SVDRP_REPLY_ACTION_NOT_TAKEN   = 550, /**< Requested action not taken */
    SVDRP_REPLY_TRANSACTION_FAILED = 554, /**< Transaction failed */
    SVDRP_REPLY_PLUGIN             = 900, /**< Default plugin reply code */
} svdrp_reply_code_t;

svdrp_reply_code_t svdrp_read_reply(svdrp_t *svdrp);
int svdrp_open_conn (svdrp_t *svdrp);
void svdrp_close_conn (svdrp_t *svdrp);
int svdrp_send (svdrp_t *svdrp, const char* cmd);

#endif /* SVDRP_INTERNALS_H */
