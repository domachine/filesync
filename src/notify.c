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
    char *cpath;  /* Complete path. */
    struct dir_watch *pwatch;  /* The watch associated with the given path. */


    assert(ws);

    /* Turn relative path into full path. */
    cpath = (char *)f_malloc(ws->src.len + 1 +
                             strlen(path) + 1);

    sprintf(cpath, "%s/%s", ws->src.str, path);

    /* Build watch-table entry. */
    pwatch =
        (struct dir_watch *)f_malloc(sizeof(struct dir_watch));

    pwatch->path = NULL;

    clone_str(&pwatch->path, path);
    pwatch->depth_level = depth;

    if(!pwatch)
        return -2;

    log_msg(DEBUG, "cpath: %s", cpath);

    if((pwatch->wd = inotify_add_watch(ws->notify_descr,
                                       cpath, ws->watch_mask)) < 0) {

        log_msg(DEBUG, "Failed to add watch for `%s': %s", cpath, strerror(errno));
        /* Failed to add watch.
           The caller can retrieve information using errno. */
        free(cpath);
        free(pwatch);
        return -1;
    }
    else
        log_msg(DEBUG, "Added watch for `%s'", cpath);


    free(cpath);

    HASH_ADD_INT(ws->watch_table, wd, pwatch);
    return pwatch->wd;
}

struct dir_watch *get_dir_watch(struct watch_session *ws, int wd)
{
    struct dir_watch *out = NULL;

    HASH_FIND_INT(ws->watch_table, &wd, out);

    return out;
}
