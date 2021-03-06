/* This program listens on a directory for changes and applies them
 * to another location.
 * Copyright (C) 2010, 2011  Dominik Burgdörfer <dominik.burgdoerfer@googlemail.com>

 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
   \file Here is the watch-session structure defined
   which is used to store information about one session.
*/

#ifndef _FILESYNC_WATCH_SESSION_H_
#define _FILESYNC_WATCH_SESSION_H_

#include <sys/inotify.h>
#include <regex.h>

/* Because of BOOL type. */
#include "utils.h"


/**
   \brief Stores information about a session.

   In general this is filled by the command line parser.
*/
struct watch_session
{
    /** \brief The src directory which is watched. */
    struct str_buf src;

    /** \brief The target which will be synchronized. */
    char *target;

    /** \brief The path to the rsync-executable. */
    char *rsync_path;

    /** \brief The depth to watch the source directory. */
    int depth;

    /** \brief Inotify dscriptor. */
    int notify_descr;

    /** \brief The watch-table. */
    struct dir_watch *watch_table;

    /** \brief The watch filter mask. */
    uint32_t watch_mask;

    /** \brief Internal field to cache src length. */
    /*int src_len;*/

    /**
       \brief Regular expression that specifies files that should
       be excluded from synchronization.
    */
    regex_t *excl;

    /** \brief Shows if the program is intended to run as daemon. */
    BOOL daemon;

    char *pid_file;
    char *log_file;
};

/**
   \brief Initializes new watch_session structure.

   The structure itself is allocated via malloc.
   It should be freed using destroy_watch_session().

   \return A pointer to the newly allocated structure.
*/
struct watch_session *new_watch_session();

/**
   \brief Compiles the given regex and sets the exclude field.

   On error a log message is send to WARN.

   \param ws The affected watch session.
   \param regex The regex to compile.
   \return 0 on success and -1 on failure.
*/
int watch_session_set_excl(struct watch_session *ws, const char *regex, int flags);

/**
   \brief Sets the src field in the watch_session.

   Calculates correct size.

   \param ws The affected watch session.
   \param src The value to set.
*/
int watch_session_set_src(struct watch_session *ws, const char *src);

/* Property setter macros. */
#define __set_str_prop(ws, key, value) (clone_str(&ws->key, value))
#define __set_int_prop(ws, key, value) (ws->key = value)
#define watch_session_set_target(ws, value) __set_str_prop(ws, target, value)
#define watch_session_set_rsync_path(ws, value) __set_str_prop(ws, rsync_path, value)
#define watch_session_set_pid_file(ws, value) __set_str_prop(ws, pid_file, value)
#define watch_session_set_log_file(ws, value) __set_str_prop(ws, log_file, value)

#define watch_session_set_depth(ws, value) __set_int_prop(ws, depth, value)
#define watch_session_set_daemon(ws, value) __set_int_prop(ws, daemon, value)
#define watch_session_set_ext_excl(ws, value) watch_session_set_excl(ws, value, \
                                                                     REG_EXTENDED)

/**
   \brief Destroys all resources allocated by new_watch_session().

   Destroys the structure itself, too.

   \param ws A pointer to the watch_session structure to destroy.
*/
void destroy_watch_session(struct watch_session *ws);

#endif  /* _FILESYNC_WATCH_SESSION_H_ */
