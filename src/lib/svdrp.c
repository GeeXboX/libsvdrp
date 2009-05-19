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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "svdrp.h"
#include "svdrp_internals.h"
#include "logs.h"
 
svdrp_t *svdrp_open (char* host, int port, int timeout, svdrp_verbosity_level_t verbosity)
{
    svdrp_t *svdrp = NULL;
    
    svdrp = calloc (1, sizeof (svdrp_t));
    if (!svdrp)
        return NULL;
        
    svdrp->host = strdup (host);
    svdrp->port = port ? port : SVDRP_DEFAULT_PORT;
    svdrp->timeout = timeout ? timeout : SVDRP_DEFAULT_TIMEOUT;
    svdrp->verbosity = verbosity;

    svdrp_log (svdrp, SVDRP_MSG_VERBOSE, __FUNCTION__);
    
    if (!svdrp_open_conn(svdrp))
        svdrp->is_connected = 0;
    
    return svdrp;
}

void svdrp_close (svdrp_t *svdrp)
{
    svdrp_log (svdrp, SVDRP_MSG_VERBOSE, __FUNCTION__);
    
    if (!svdrp)
        return;
        
    if (svdrp->conn)
        svdrp_close_conn (svdrp);

    if (svdrp->host)
        free (svdrp->host);
    
    /* FIXME make sure to properly free all members */             
    free (svdrp);
}

static int svdrp_simple_cmd (svdrp_t *svdrp, const char *cmd)
{
    svdrp_log (svdrp, SVDRP_MSG_VERBOSE, __FUNCTION__);
    
    svdrp_send(svdrp, cmd);
    
    if (svdrp_read_reply(svdrp) == SVDRP_REPLY_OK)
        return SVDRP_OK;
    else
        return SVDRP_ERROR;   
}

int svdrp_epg_clear (svdrp_t *svdrp, int channel_id)
{
    char *cmd;
    int ret;

    svdrp_log (svdrp, SVDRP_MSG_VERBOSE, __FUNCTION__);
    
    if (channel_id) {
        const size_t len = 15;
        cmd = malloc (len);
        
        svdrp_log (svdrp, SVDRP_MSG_INFO, "Clear EPG for channel '%i'", channel_id);
        snprintf(cmd, len, "CLRE %i\n", channel_id);
    } else {
        svdrp_log (svdrp, SVDRP_MSG_INFO, "Clear EPG list");
        cmd = "CLRE\n";
    }
    
    ret = svdrp_simple_cmd (svdrp, cmd);
    free (cmd);
    
    return ret;    
}

int svdrp_epg_scan (svdrp_t *svdrp)
{
    svdrp_log (svdrp, SVDRP_MSG_VERBOSE, __FUNCTION__);
    svdrp_log (svdrp, SVDRP_MSG_INFO, "Begin EPG scan");
    
    return svdrp_simple_cmd (svdrp, "SCAN\n");
}

int svdrp_next_timer_event (svdrp_t *svdrp, int *timer_id, time_t *time)
{
    svdrp_log (svdrp, SVDRP_MSG_VERBOSE, __FUNCTION__);
    
    svdrp_send(svdrp, "NEXT abs\n");
    
    if (svdrp_read_reply(svdrp) == SVDRP_REPLY_OK) {
        int mytimer;
        struct tm tm;
        char *time_str = strstr(svdrp->last_reply, " ") + 1;
        char full_time_str[256];
        
        mytimer = atoi (svdrp->last_reply);
        strptime(time_str, "%s", &tm);
        strftime(full_time_str, 256, "%Y-%m-%d %H:%M:%S %z(%Z) (stamp: %s)", &tm);
        
        svdrp_log (svdrp, SVDRP_MSG_INFO, "Next timer %i at %s", mytimer,
                   full_time_str);
        
        if (timer_id)
            *timer_id = mytimer;
            
        if (time)
            *time = mktime(&tm);

        return SVDRP_OK;
    } else { /* usually 550 No active timers */
        return SVDRP_ERROR;
    }
}

int svdrp_osd_message (svdrp_t *svdrp, const char *msg)
{
    const size_t len = strlen(msg) + 7;
    char *cmd;
    int ret;
    
    svdrp_log (svdrp, SVDRP_MSG_VERBOSE, __FUNCTION__);
    
    if (!msg) {
        svdrp_log (svdrp, SVDRP_MSG_WARNING, "Cannot send empty OSD message");
        return SVDRP_ERROR;
    }
    
    cmd = malloc (len);
    svdrp_log (svdrp, SVDRP_MSG_INFO, "OSD message '%s'", msg);
    snprintf (cmd, len, "MESG %s\n", msg);    
    ret = svdrp_simple_cmd (svdrp, cmd);
    free(cmd);
    
    return ret;
}

int svdrp_hit_key(svdrp_t *svdrp, svdrp_key_t key)
{
    static const char const *keys[] = {
        [SVDRP_KEY_UP]           = "Up",
        [SVDRP_KEY_DOWN]         = "Down",
        [SVDRP_KEY_MENU]         = "Menu",
        [SVDRP_KEY_OK]           = "Ok",
        [SVDRP_KEY_BACK]         = "Back",
        [SVDRP_KEY_LEFT]         = "Left",
        [SVDRP_KEY_RIGHT]        = "Right",
        [SVDRP_KEY_RED]          = "Red",
        [SVDRP_KEY_GREEN]        = "Green",
        [SVDRP_KEY_YELLOW]       = "Yellow",
        [SVDRP_KEY_BLUE]         = "Blue",
        [SVDRP_KEY_0]            = "0",
        [SVDRP_KEY_1]            = "1",
        [SVDRP_KEY_2]            = "2",
        [SVDRP_KEY_3]            = "3",
        [SVDRP_KEY_4]            = "4",
        [SVDRP_KEY_5]            = "5",
        [SVDRP_KEY_6]            = "6",
        [SVDRP_KEY_7]            = "7",
        [SVDRP_KEY_8]            = "8",
        [SVDRP_KEY_9]            = "9",
        [SVDRP_KEY_INFO]         = "Info",
        [SVDRP_KEY_PLAY]         = "Play",
        [SVDRP_KEY_PAUSE]        = "Pause",
        [SVDRP_KEY_STOP]         = "Stop",
        [SVDRP_KEY_RECORD]       = "Record",
        [SVDRP_KEY_FASTFWD]      = "FastFwd",
        [SVDRP_KEY_FASTREW]      = "FastRew",
        [SVDRP_KEY_NEXT]         = "Next",
        [SVDRP_KEY_PREV]         = "Prev",
        [SVDRP_KEY_POWER]        = "Power",
        [SVDRP_KEY_CHANNELPLUS]  = "Channel+",
        [SVDRP_KEY_CHANNELMINUS] = "Channel-",
        [SVDRP_KEY_PREVCHANNEL]  = "PrevChannel",
        [SVDRP_KEY_VOLUMEPLUS]   = "Volume+",
        [SVDRP_KEY_VOLUMEMINUS]  = "Volume-",
        [SVDRP_KEY_MUTE]         = "Mute",
        [SVDRP_KEY_AUDIO]        = "Audio",
        [SVDRP_KEY_SUBTITLES]    = "Subtitles",
        [SVDRP_KEY_SCHEDULE]     = "Schedule",
        [SVDRP_KEY_CHANNELS]     = "Channels",
        [SVDRP_KEY_TIMERS]       = "Timers",
        [SVDRP_KEY_RECORDINGS]   = "Recordings",
        [SVDRP_KEY_SETUP]        = "Setup",
        [SVDRP_KEY_COMMANDS]     = "Commands",
        [SVDRP_KEY_USER1]        = "User1",
        [SVDRP_KEY_USER2]        = "User2",
        [SVDRP_KEY_USER3]        = "User3",
        [SVDRP_KEY_USER4]        = "User4",
        [SVDRP_KEY_USER5]        = "User5",
        [SVDRP_KEY_USER6]        = "User6",
        [SVDRP_KEY_USER7]        = "User7",
        [SVDRP_KEY_USER8]        = "User8",
        [SVDRP_KEY_USER9]        = "User9",
    };
    const size_t len = strlen(keys[key]) + 7;
    char *cmd = malloc(len);
    int ret;
    
    svdrp_log (svdrp, SVDRP_MSG_VERBOSE, __FUNCTION__);
    svdrp_log (svdrp, SVDRP_MSG_INFO, "Hit key '%s'", keys[key]);
    
    snprintf (cmd, len, "HITK %s\n", keys[key]);
    ret = svdrp_simple_cmd(svdrp, cmd);
    free(cmd);
    
    return ret;
}

int svdrp_volume_mute (svdrp_t *svdrp)
{
    svdrp_log (svdrp, SVDRP_MSG_VERBOSE, __FUNCTION__);
    
    return svdrp_simple_cmd(svdrp, "VOLU mute\n");
}

int svdrp_volume_up (svdrp_t *svdrp)
{
    svdrp_log (svdrp, SVDRP_MSG_VERBOSE, __FUNCTION__);

    return svdrp_simple_cmd(svdrp, "VOLU +\n");
}

int svdrp_volume_down (svdrp_t *svdrp)
{
    svdrp_log (svdrp, SVDRP_MSG_VERBOSE, __FUNCTION__);
    
    return svdrp_simple_cmd(svdrp, "VOLU -\n");   
}

int svdrp_volume_set (svdrp_t *svdrp, int volume)
{
    const int len = 10;
    char cmd[len];
    int ret;
    
    svdrp_log (svdrp, SVDRP_MSG_VERBOSE, __FUNCTION__);
    
    if (volume < 0 || volume > 255) {
        svdrp_log (svdrp, SVDRP_MSG_WARNING, "Illegal volume: %i", volume);
        
        return SVDRP_ERROR;
    }
    
    svdrp_log (svdrp, SVDRP_MSG_INFO, "Set volume to %i", volume);
    
    snprintf(cmd, len, "VOLU %i\n", volume);
    ret = svdrp_simple_cmd(svdrp, cmd);
    
    return ret;   
}

int svdrp_set_remote(svdrp_t *svdrp, int state)
{
    char *cmd;
    
    svdrp_log (svdrp, SVDRP_MSG_VERBOSE, __FUNCTION__);
    
    if (state)
        cmd = "REMO on\n";
    else
        cmd = "REMO off\n";
    
    return svdrp_simple_cmd(svdrp, cmd);
}

int svdrp_try_connect(svdrp_t *svdrp)
{
    return svdrp->is_connected ? 1 : svdrp_open_conn(svdrp);
}

int svdrp_is_connected(svdrp_t *svdrp)
{
    return svdrp->is_connected;
}

const char *svdrp_get_property(svdrp_t *svdrp, svdrp_property_t property)
{
    svdrp_log (svdrp, SVDRP_MSG_VERBOSE, __FUNCTION__);

    switch (property)
    {
    case SVDRP_PROPERTY_NAME:    return svdrp->name;
    case SVDRP_PROPERTY_VERSION: return svdrp->version;
    case SVDRP_PROPERTY_CHARSET: return svdrp->charset;
    case SVDRP_PROPERTY_HOSTNAME: return svdrp->host;
    }
    
    return NULL;
}

int svdrp_get_timer(svdrp_t *svdrp, int timer_id, svdrp_timer_t *timer)
{
    const int len = 10;
    char cmd[len];

    svdrp_log (svdrp, SVDRP_MSG_VERBOSE, __FUNCTION__);
    
    snprintf(cmd, len, "LSTT %i\n", timer_id);
    
    svdrp_send(svdrp, cmd);
    
    if (svdrp_read_reply(svdrp) == SVDRP_REPLY_OK) {
        unsigned char flags;
        char day[256], start[256], stop[256], file[256], data[256];

        if (!timer)
            return SVDRP_ERROR;

        sscanf(svdrp->last_reply + 2, 
               "%i:%hhi:%[^:]:%[^:]:%[^:]:%i:%i:%[^:]:%[^:]",
               &(timer->channel),
               &flags,
               day,
               start,
               stop,
               &(timer->priority),
               &(timer->lifetime),
               file,
               data);

        timer->id = timer_id;
        timer->start = strdup (start);
        timer->stop = strdup (stop);
        timer->file = strdup (file);
        timer->data = strdup (data);
        timer->is_active = ((flags & SVDRP_TIMER_ACTIVE_FLAG) != 0);
        timer->is_recording = ((flags & SVDRP_TIMER_RECORDING_FLAG) != 0);
        timer->is_instant = ((flags & SVDRP_TIMER_INSTANT_FLAG) != 0);
        timer->use_vps = ((flags & SVDRP_TIMER_VPS_FLAG) != 0);

        if (day[0] == 'M' || day[0] == '-') /* repeating timer */
        { 
            int i;
            
            for (i = 0; i < 7; i++)
                if (day[i] != '-')
                    timer->repeating |= ((unsigned char) (1 << i));
                    
            if (strlen(day) > 7 && day[7] == '@')
                timer->first_date = strdup (day + 8);
            else
                timer->first_date = NULL;
        }
        else /* one shot timer */
        {
            timer->first_date = strdup (day);
            timer->repeating = 0;
        }

        return SVDRP_OK;
    } else { /* usually 501 Timer not defined */
        return SVDRP_ERROR;
    }
}
