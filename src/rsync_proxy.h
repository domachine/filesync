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

#ifndef _FILESYNC_RSYNC_PROXY_H_
#define _FILESYNC_RSYNC_PROXY_H_

/* Forward declaration. */
struct watch_session;

int sync_file(struct watch_session *ws, const char *dir, const char *file);

#endif  /* _FILESYNC_RSYNC_PROXY_H_ */
