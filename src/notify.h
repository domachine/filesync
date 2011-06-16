/* This program listens on a directory for changes and applies them
 * to another location, too.
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
 * along with this program.  If not, see <http://www.gnu.org/licenses/>. */

/**
   \file Declares some helper functions to interact with rsync.
*/

#ifndef _FILESYNC_NOTIFY_PROXY_H_
#define _FILESYNC_NOTIFY_PROXY_H_

#include "uthash.h"


/**
   \brief A structure that holds information about
   a watched directory.
*/
struct dir_watch
{
    int wd;

    /** \brief The path of the watched directory (relative to the top level). */
    char *path;

    /** \brief The depth-level of the watched directory. */
    int depth_level;

    /** \brief Needed to make this hashable. */
    UT_hash_handle hh;
};

/* Forward declaration. */
struct watch_session;


/**
   \brief Installs the given directory in the watch table.
   \param path The path of the directory to watch
   \param depth The depth-level of the directory.
   \return 0 on success, -1 on memory failure.
*/
int install_dir_watch(struct watch_session *ws, const char *path, int depth);

/**
   \brief Returns the watch-entry for the given watch-descriptor.
   \param ws The top level watch-session.
   \param wd The watch-descriptor of the entry to fetch.
*/
struct dir_watch *get_dir_watch(struct watch_session *ws, int wd);

#endif  /* _FILESYNC_NOTIFY_PROXY_H_ */
