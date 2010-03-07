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

#ifndef SVDRP_H
#define SVDRP_H

/**
 * \file svdrp.h
 *
 * GeeXboX libsvdrp public API header.
 */

/** \brief libsvdrp version */
#define LIBSVDRP_VERSION "0.0.1"

/** \brief Default timeout for SVDRP connections */
#define SVDRP_DEFAULT_TIMEOUT 30

/** \brief Default port for SVDRP connections */
#define SVDRP_DEFAULT_PORT 2001

/** \brief SVDRP return code for successful operations */
#define SVDRP_OK    1

/** \brief SVDRP return code for failed operations */
#define SVDRP_ERROR 0

/**
 * \brief SVDRP connection object.
 *
 * This manages an SVDRP connection to VDR.
 */
typedef struct svdrp_s svdrp_t;

/** \brief SVDRP verbosity. */
typedef enum {
    SVDRP_MSG_NONE,          /**< no error messages */
    SVDRP_MSG_VERBOSE,       /**< super-verbose mode: mostly for debugging */
    SVDRP_MSG_INFO,          /**< working operations */
    SVDRP_MSG_WARNING,       /**< harmless failures */
    SVDRP_MSG_ERROR,         /**< may result in hazardous behavior */
    SVDRP_MSG_CRITICAL,      /**< prevents lib from working */
} svdrp_verbosity_level_t;

/**
 * \brief Keys accepted by VDR.
 *
 * The list of keys understood by VDR, which can be given to svdrp_hit_key.
 */
typedef enum {
    SVDRP_KEY_UP,
    SVDRP_KEY_DOWN,
    SVDRP_KEY_MENU,
    SVDRP_KEY_OK,
    SVDRP_KEY_BACK,
    SVDRP_KEY_LEFT,
    SVDRP_KEY_RIGHT,
    SVDRP_KEY_RED,
    SVDRP_KEY_GREEN,
    SVDRP_KEY_YELLOW,
    SVDRP_KEY_BLUE,
    SVDRP_KEY_0,
    SVDRP_KEY_1,
    SVDRP_KEY_2,
    SVDRP_KEY_3,
    SVDRP_KEY_4,
    SVDRP_KEY_5,
    SVDRP_KEY_6,
    SVDRP_KEY_7,
    SVDRP_KEY_8,
    SVDRP_KEY_9,
    SVDRP_KEY_INFO,
    SVDRP_KEY_PLAY,
    SVDRP_KEY_PAUSE,
    SVDRP_KEY_STOP,
    SVDRP_KEY_RECORD,
    SVDRP_KEY_FASTFWD,
    SVDRP_KEY_FASTREW,
    SVDRP_KEY_NEXT,
    SVDRP_KEY_PREV,
    SVDRP_KEY_POWER,
    SVDRP_KEY_CHANNELPLUS,
    SVDRP_KEY_CHANNELMINUS,
    SVDRP_KEY_PREVCHANNEL,
    SVDRP_KEY_VOLUMEPLUS,
    SVDRP_KEY_VOLUMEMINUS,
    SVDRP_KEY_MUTE,
    SVDRP_KEY_AUDIO,
    SVDRP_KEY_SUBTITLES,
    SVDRP_KEY_SCHEDULE,
    SVDRP_KEY_CHANNELS,
    SVDRP_KEY_TIMERS,
    SVDRP_KEY_RECORDINGS,
    SVDRP_KEY_SETUP,
    SVDRP_KEY_COMMANDS,
    SVDRP_KEY_USER1,
    SVDRP_KEY_USER2,
    SVDRP_KEY_USER3,
    SVDRP_KEY_USER4,
    SVDRP_KEY_USER5,
    SVDRP_KEY_USER6,
    SVDRP_KEY_USER7,
    SVDRP_KEY_USER8,
    SVDRP_KEY_USER9,
} svdrp_key_t;

/** \brief SVDRP property. */
typedef enum {
    SVDRP_PROPERTY_NAME,          /**< VDR server name */
    SVDRP_PROPERTY_VERSION,       /**< VDR version */
    SVDRP_PROPERTY_CHARSET,       /**< VDR server charset */
    SVDRP_PROPERTY_HOSTNAME,      /**< VDR server hostname */
} svdrp_property_t;

#define SVDRP_MONDAY    ((unsigned char) (1 << 0))
#define SVDRP_TUESDAY   ((unsigned char) (1 << 1))
#define SVDRP_WEDNESDAY ((unsigned char) (1 << 2))
#define SVDRP_THURSDAY  ((unsigned char) (1 << 3))
#define SVDRP_FRIDAY    ((unsigned char) (1 << 4))
#define SVDRP_SATURDAY  ((unsigned char) (1 << 5))
#define SVDRP_SUNDAY    ((unsigned char) (1 << 6))

#define SVDRP_TIMER_ACTIVE_FLAG     1
#define SVDRP_TIMER_INSTANT_FLAG    2
#define SVDRP_TIMER_VPS_FLAG        4
#define SVDRP_TIMER_RECORDING_FLAG  8

typedef struct svdrp_timer_s {
    int id;
    int channel;
    char* first_date;
    char* start;
    char* stop;
    unsigned char repeating;      /**< Days of the week the timer repeats (bitfield) */
    int is_active;                /**< Whether the timer is active or not */
    int is_recording;             /**< Whether the timer is currently recording or not */
    int is_instant;               /**< Whether this is an instant recording timer */
    int use_vps;                  /**< Whether the timer uses VPS */
    int priority;                 /**< Timer priority (0-99), highest wins */
    int lifetime;                 /**< Recording lifetime (0-99) */
    char *file;                   /**< File name */
    char *data;                   /**< Auxiliary data */
} svdrp_timer_t;

/**
 * \name SVDRP (Un)Initialization.
 * @{
 */

/**
 * \brief Initialize a new SVDRP connection.
 *
 * \param[in] host         host name of target VDR.
 * \param[in] port         SVDRP port.
 * \param[in] timeout      connection timeout.
 * \param[in] verbosity    level of verbosity to set.
 * \return SVDRP connection object or NULL.
 *
 * Creates a new SVDRP connection object and returns it. In case of errors, NULL is returned.
 */
svdrp_t *svdrp_open(char* host, int port, int timeout, svdrp_verbosity_level_t verbosity);

/**
 * \brief Close an SVDRP connection.
 *
 * \param[in] svdrp        an SVDRP connection object
 *
 * Safetly destroy an SVDRP connection object.
 */
void svdrp_close(svdrp_t *svdrp);

/**
 * \brief Check if there's an active SVDRP connection and try to re-establish
 *        it if necessary.
 *
 * \param[in] svdrp        an SVDRP connection object
 * \return                 whether the connection attempt has been successful or not
 *
 * Checks whether there's an active SVDRP connection; if not, tries to open a
 * new connection. Returns the SVDRP connection status.
 */
int svdrp_try_connect(svdrp_t *svdrp);

/**
 * \brief Check if there's an active SVDRP connection.
 *
 * \param[in] svdrp        an SVDRP connection object
 * \return                 whether the SVDRP connection is active or not
 *
 * Returns the current SVDRP connection status.
 */
int svdrp_is_connected(svdrp_t *svdrp);

/**
 * \brief Get a property of the VDR server.
 *
 * \param[in] svdrp        an SVDRP connection object
 * \param[in] property     the property to get
 * \return                 the property value
 *
 * Returns the value of a property of the VDR server.
 */
const char *svdrp_get_property(svdrp_t *svdrp, svdrp_property_t property);

/**
 * @}
 */

/**
 * \name VDR commands.
 * @{
 */

/**
 * \brief Clear the EPG list.
 *
 * \param[in] svdrp        an SVDRP connection object
 * \param[in] channel_id   number of the channel whose EPG should be cleared
 * \return                 SVDRP_OK on success, SVDRP_ERROR otherwise.
 *
 * Clears the EPG list of the given channel. If channel_id is NULL, it clears the entire EPG list.
 */
int svdrp_epg_clear (svdrp_t *svdrp, int channel_id);

/**
 * \brief Force an EPG scan.
 *
 * \param[in] svdrp        an SVDRP connection object
 * \return                 SVDRP_OK on success, SVDRP_ERROR otherwise.
 *
 * Forces an EPG scan. If VDR is running on a single DVB device system, the scan will be done on the primary device unless it is currently recording.
 */
int svdrp_epg_scan (svdrp_t *svdrp);

/**
 * \brief Displays the given message on the OSD.
 *
 * \param[in] svdrp        an SVDRP connection object
 * \param[in] msg          the message to display
 * \return                 SVDRP_OK on success, SVDRP_ERROR otherwise.
 *
 * The message will be displayed for a few seconds as soon as the OSD has
 * become idle. If a new MESG command is entered while the previous message
 * has not yet been displayed, the old message will be overwritten.
 */
int svdrp_osd_message (svdrp_t *svdrp, const char *msg);

/**
 * \brief Get the next timer event.
 *
 * \param[in] svdrp        an SVDRP connection object
 * \param[out] timer_id    the number of the next timer
 * \param[out] time        the time when the next timer will fire
 * \return                 SVDRP_OK on success, SVDRP_ERROR otherwise.
 *
 * Shows the next timer event as the number of seconds since the epoch. If
 * smaller than the current time, the timer is currently recording and has
 * started at the given time. If timer_id or time are NULL the respective
 * information will not be returned.
 */
int svdrp_next_timer_event(svdrp_t *svdrp, int *timer_id, time_t *time);

int svdrp_get_timer(svdrp_t *svdrp, int timer_id, svdrp_timer_t *timer);

int svdrp_volume_mute (svdrp_t *svdrp);
int svdrp_volume_up (svdrp_t *svdrp);
int svdrp_volume_down (svdrp_t *svdrp);
int svdrp_volume_set (svdrp_t *svdrp, int volume);
int svdrp_hit_key(svdrp_t *svdrp, svdrp_key_t key);
int svdrp_set_remote(svdrp_t *svdrp, int state);

/**
 * @}
 */

#endif /* SVDRP_H */
