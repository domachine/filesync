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

//#error Not finished yet!

#include "notify.h"

#include <stdlib.h>
#include <stdio.h>

#include <assert.h>
#include <errno.h>

#include "watch_session.h"
#include "utils.h"
#include "logging.h"


int install_dir_watch(struct watch_session *ws, const char *path, int depth)
{
    assert(ws);

    /* Turn relative path into full path. */
    int path_len = ws->src_len + 1 + strlen(path) + 1;
    char *full_path = (char *)malloc(path_len);

    if(!full_path)
        return -2;

    snprintf(full_path, path_len, "%s/%s", ws->src, path);

    /* Build watch-table entry. */
    struct dir_watch *w =
        (struct dir_watch *)malloc(sizeof(struct dir_watch));

    assert(w);

    w->path = NULL;

    clone_str(&w->path, path);
    w->depth_level = depth;

    if(!w)
        return -2;

    log_msg(DEBUG, "full_path: %s", full_path);

    if((w->wd = inotify_add_watch(ws->notify_descr,
                                  full_path, ws->watch_mask)) < 0) {

        log_msg(DEBUG, "Failed to add watch for `%s': %s", full_path, strerror(errno));
        /* Failed to add watch.
           The caller can retrieve information using errno. */
        free(full_path);
        free(w);
        return -1;
    }
    else
        log_msg(DEBUG, "Added watch for `%s'", full_path);


    free(full_path);

    HASH_ADD_INT(ws->watch_table, wd, w);
    return w->wd;
}

struct dir_watch *get_dir_watch(struct watch_session *ws, int wd)
{
    struct dir_watch *out = NULL;

    HASH_FIND_INT(ws->watch_table, &wd, out);

    return out;
}
