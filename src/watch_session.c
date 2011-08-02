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
#include <string.h>

#include <assert.h>
#include <sys/inotify.h>

#include "utils.h"
#include "logging.h"


struct watch_session *new_watch_session()
{
    struct watch_session *ws;
    ws = (struct watch_session *)f_malloc(sizeof(struct watch_session));

    if(!ws)
        return NULL;

    /* Normalize all values to clear defaults. */
    ws->src.str = NULL;
    ws->target = NULL;
    ws->rsync_path = NULL;

    clone_str(&ws->rsync_path, "rsync");

    ws->depth = -1;
    ws->src.len = 0;
    ws->notify_descr = inotify_init();

    assert(ws->notify_descr);

    ws->watch_table = NULL;

    ws->watch_mask = IN_CREATE |
        IN_MOVE |
        IN_MODIFY;

    ws->excl = NULL;

    ws->daemon = FALSE;

    ws->pid_file = NULL;
    ws->log_file = NULL;

    return ws;
}

int watch_session_set_excl(struct watch_session *ws, const char *regex, int flags)
{
    int err_code;

    ws->excl = (regex_t *)f_malloc(sizeof(regex_t));

    if((err_code = regcomp(ws->excl, regex, flags)) != 0) {
        size_t err_len = regerror(err_code, ws->excl, NULL, 0);
        char *err_buf = (char *)f_malloc(err_len);

        regerror(err_code, ws->excl, err_buf, err_len);
        log_msg(WARN, "Failed to compile regex: %s", err_buf);

        return -1;
    }

    return 0;
}

int watch_session_set_src(struct watch_session *ws, const char *src)
{
    ws->src.len = strlen(src);
    return clone_str(&ws->src.str, src);
}

void destroy_watch_session(struct watch_session *ws)
{
    assert(ws);

    /* Do the garbage collection. */
    FREE_MEM(ws->src.str);
    FREE_MEM(ws->target);
    FREE_MEM(ws->rsync_path);

    if(ws->excl) {
        regfree(ws->excl);
        free(ws->excl);
    }

    FREE_MEM(ws->pid_file);
    FREE_MEM(ws->log_file);

    /* TODO: Free the hash structure. */
    /* Free the structure itself. */
    free(ws);
}
