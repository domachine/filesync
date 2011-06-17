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

#include "watch_session.h"

#include <stdlib.h>

#include <assert.h>
#include <sys/inotify.h>

#include "utils.h"


struct watch_session *new_watch_session()
{
    struct watch_session *ws;
    ws = (struct watch_session *)malloc(sizeof(struct watch_session));

    if(!ws)
        return NULL;

    /* Normalize all values to clear defaults. */
    ws->src = NULL;
    ws->target = NULL;
    ws->rsync_path = NULL;

    clone_str(&ws->rsync_path, "rsync");

    ws->depth = -1;
    ws->src_len = 0;
    ws->notify_descr = inotify_init();

    assert(ws->notify_descr);

    ws->watch_table = NULL;

    ws->watch_mask = IN_CREATE |
        IN_MOVE |
        IN_MODIFY;

    ws->excl = NULL;

    return ws;
}

void destroy_watch_session(struct watch_session *ws)
{
    assert(ws);

    /* Do the garbage collection. */
    FREE_STR(ws->src);
    FREE_STR(ws->target);
    FREE_STR(ws->rsync_path);

    /* TODO: Free the hash structure. */
    /* Free the structure itself. */
    free(ws);
}
