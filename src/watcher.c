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

#include "watcher.h"

#include <dirent.h>
#include <errno.h>
#include <sys/inotify.h>
#include <sys/stat.h>
#include <unistd.h>

#include "watch_session.h"
#include "notify.h"
#include "rsync_proxy.h"
#include "logging.h"


static int reg_dir(struct watch_session *ws, int cur_depth, const char *path)
{
    /* Check if depth limit was reached. */
    if(ws->depth > -1 && cur_depth > ws->depth)
        return 0;

    int compl_path_len = ws->src_len + 1 + strlen(path) + 1;
    char *compl_path;
    AUTO_SNPRINTF(compl_path, compl_path_len, "%s/%s", ws->src, path);

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
            /* Skip current and parent-directory links. */
            if(strcmp(d->d_name, ".") == 0 || strcmp(d->d_name, "..") == 0)
                continue;

            /* Turn relative path into complete path. */
            int full_path_len = strlen(path) + 1 + strlen(d->d_name) + 1;
            char *full_path;

            AUTO_SNPRINTF(full_path, full_path_len, "%s/%s", path, d->d_name);

            log_msg(DEBUG, "Checking `%s'.", full_path);
            if(!ws->excl || regexec(ws->excl, full_path, 0, NULL, 0) != 0)
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

int daemonize(FILE *pid_file)
{
    pid_t pid, sid;

    pid = fork();
    if(pid < 0) {
        log_msg(ERROR, "Failed to fork daemon process.");
        return -1;
    }
    else if(pid > 0) {
        fprintf(pid_file, "%d", pid);

        exit(EXIT_SUCCESS);
    }

    /* Make sure file access is not necessarily inherited. */
    umask(0);

    sid = setsid();
    if(sid < 0) {
        log_msg(ERROR, "Failed to get session id.");
        return -1;
    }

    return pid;
}

void run_main_loop(struct watch_session *ws)
{
    struct inotify_event *event_buf = NULL;

    while((event_buf = read_event(ws->notify_descr)) != NULL) {
        log_msg(DEBUG, "Event occured ...");

        struct dir_watch *dw = get_dir_watch(ws, event_buf->wd);
        assert(dw);

        struct stat affec_file;

        int compl_path_len = ws->src_len + 1 +
            strlen(dw->path) + 1 +
            strlen(event_buf->name) + 1;

        char *compl_path;
        AUTO_SNPRINTF(compl_path, compl_path_len, "%s/%s/%s", ws->src, dw->path, event_buf->name);

        /* Check the file-type of the file reported by inotify. */
        if(stat(compl_path, &affec_file) < 0)
            log_msg(WARN, "Stat failed for `%s': %s", compl_path, strerror(errno));
        else {
            int rel_path_len = strlen(dw->path) + 1 + strlen(event_buf->name) + 1;
            char *rel_path;
            AUTO_SNPRINTF(rel_path, rel_path_len, "%s/%s", dw->path, event_buf->name);

            if(S_ISDIR(affec_file.st_mode) && (event_buf->mask & IN_CREATE) == IN_CREATE) {
                /* The file is a directory (newly created). So add it to the
                   watch-table. */
                log_msg(DEBUG, "Adding newly created directory: %s", rel_path);

                reg_dir(ws, dw->depth_level + 1, rel_path);
            }

            if(!ws->excl || regexec(ws->excl, rel_path, 0, NULL, 0) != 0) {
                log_msg(DEBUG, "Synchronizing file: %s", rel_path);
                sync_file(ws, dw->path, event_buf->name);
            }

            free(rel_path);
        }

        free(compl_path);
        free(event_buf);
    }

    /*FREE_MEM(event_buf);*/
}

int run_watcher(struct watch_session *ws)
{
    if(ws->daemon) {
        /* Prepare everything for daemon spawning. */
        FILE *pid_file;

        if(!ws->pid_file) {
            log_msg(WARN, "Daemon mode but no pid-file given (using stdout).");
            pid_file = stdout;
        }
        else {
            pid_file = fopen(ws->pid_file, "w");
            if(!pid_file) {
                log_msg(ERROR, "Failed to open pid-file: %s", strerror(errno));
                return EXIT_FAILURE;
            }
        }

        int ret = daemonize(pid_file);
        if(ret < 0)
            return EXIT_FAILURE;
    }

    FILE *log_file_descr = stderr;

    if(ws->log_file) {
        log_file_descr = fopen(ws->log_file, "w");

        if(!log_file_descr) {
            log_msg(WARN, "Failed to open log-file (using stderr).");

            log_file_descr = stderr;
        }
    }

    init_log(log_file_descr, TRUE, ALL_CHANNELS);

    reg_dir(ws, 0, ".");

    /*
      The main watch-loop.
    */
    run_main_loop(ws);

    return EXIT_SUCCESS;
}
