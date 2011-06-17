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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <dirent.h>
#include <errno.h>
#include <sys/inotify.h>
#include <sys/stat.h>
#include <assert.h>

#include "logging.h"
#include "watch_session.h"
#include "cmdparser.h"
#include "notify.h"
#include "rsync_proxy.h"


static int reg_dir(struct watch_session *ws, int cur_depth, const char *path)
{
    /* Check if depth limit was reached. */
    if(ws->depth > -1 && cur_depth > ws->depth)
        return 0;

    int compl_path_len = strlen(ws->src) + 1 + strlen(path) + 1;
    char *compl_path = (char *)malloc(compl_path_len);
    assert(compl_path);
    sprintf(compl_path, "%s/%s", ws->src, path);
    DIR *cur_dir = opendir(compl_path);
    free(compl_path);

    if(!cur_dir) {
        log_msg(WARN, "Failed to open `%s': %s", path, strerror(errno));
        return -1;
    }

    install_dir_watch(ws, path, cur_depth);

    struct dirent *d;
    while((d = readdir(cur_dir)) != NULL) {
        if(d->d_type == DT_DIR) {
            if(strcmp(d->d_name, ".") == 0 || strcmp(d->d_name, "..") == 0)
                continue;

            /* Turn relative path into complete path. */
            int full_path_len = strlen(path) + 1 + strlen(d->d_name) + 1;
            char *full_path = (char *)malloc(full_path_len);

            if(!full_path) {
                log_msg(WARN, "Failed to allocate memory for full_path.");
                return -2;
            }

            sprintf(full_path, "%s/%s", path, d->d_name);

            /* Watch subdirectory. */
            reg_dir(ws, cur_depth + 1, full_path);

            free(full_path);
        }
    }

    return -1;
}

static const size_t EVENT_SIZE = (sizeof(struct inotify_event) + 16)*1024;


static struct inotify_event* read_event(int fd)
{
    struct inotify_event* buf = NULL;

    /* Allocate memory. */
    buf = (struct inotify_event*)malloc(EVENT_SIZE);

    assert(buf);

    ssize_t nbytes = read(fd, buf, EVENT_SIZE);

    if(nbytes < 0) {
        /* Failed to read from file-descriptor. */
        free(buf);
        return NULL;
    }

    return buf;
}

int main(int argc, char **argv)
{
    /* TODO: make the verbosity level cutomizable through command line. */
    init_log(stderr, TRUE, ALL_CHANNELS);

    /* Build new watch_session. */
    struct watch_session *ws = new_watch_session();

    if(!ws) {
        log_msg(ERROR, "Failed to allocate memory "
                "for watch-session structure.");
        return EXIT_FAILURE;
    }

    parse_cmd_line(ws, argc, argv);

    if(!ws->src) {
        log_msg(ERROR, "No source given.");
        return EXIT_FAILURE;
    }

    if(!ws->target) {
        log_msg(ERROR, "No target given.");
        return EXIT_FAILURE;
    }

    int src_len = strlen(ws->src);
    if(ws->src[src_len - 1] == '/')
        ws->src[src_len - 1] = '\0';

    reg_dir(ws, 0, ".");
    struct inotify_event *event_buf = NULL;

    while((event_buf = read_event(ws->notify_descr)) != NULL) {
        log_msg(DEBUG, "Event occured ...");

        struct dir_watch *dw = get_dir_watch(ws, event_buf->wd);
        assert(dw);

        struct stat affec_file;

        char *compl_path = (char *)malloc(strlen(ws->src) + 1 +
                                          strlen(dw->path) + 1 +
                                          strlen(event_buf->name) + 1);

        assert(compl_path);

        sprintf(compl_path, "%s/%s/%s", ws->src, dw->path, event_buf->name);

        /* Check the file-type of the file reported by inotify. */
        if(stat(compl_path, &affec_file) < 0)
            log_msg(WARN, "Stat failed for `%s': %s", compl_path, strerror(errno));
        else {
            if(S_ISDIR(affec_file.st_mode) && (event_buf->mask & IN_CREATE) == IN_CREATE) {

                /* The file is a directory (newly created). So add it to the
                   watch-table. */
                char *rel_path = (char *)malloc(strlen(dw->path) + 1 +
                                                strlen(event_buf->name) + 1);

                assert(rel_path);

                sprintf(rel_path, "%s/%s", dw->path, event_buf->name);
                log_msg(DEBUG, "Adding newly created directory: %s", rel_path);

                reg_dir(ws, dw->depth_level + 1, rel_path);

                free(rel_path);
            }

            log_msg(DEBUG, "Synchronizing file: %s/%s", dw->path, event_buf->name);
            sync_file(ws, dw->path, event_buf->name);
        }

        free(compl_path);
    }

    destroy_watch_session(ws);
    close_log();

    return EXIT_SUCCESS;
}