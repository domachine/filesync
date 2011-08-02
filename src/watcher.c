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
    int path_len;
    char *cpath;  /* Complete path. */
    DIR *dirhandle;  /* Directory handle for directory to register. */
    struct dirent *dir_entry;  /* Iterator variable. */


    /* Cache path length. */
    path_len = strlen(path);

    /* Check if depth limit was reached. */
    if(ws->depth > -1 && cur_depth > ws->depth)
        return 0;

    cpath = (char *)f_malloc(ws->src.len + 1 + strlen(path) + 1);
    sprintf(cpath, "%s/%s", ws->src.str, path);

    dirhandle = opendir(cpath);
    free(cpath);

    if(!dirhandle) {
        log_msg(WARN, "Failed to open `%s': %s", path, strerror(errno));
        return -1;
    }

    install_dir_watch(ws, path, cur_depth);

    while((dir_entry = readdir(dirhandle)) != NULL) {
        if(dir_entry->d_type == DT_DIR) {
            /* Skip current and parent-directory links. */
            if(strcmp(dir_entry->d_name, ".") == 0 ||
               strcmp(dir_entry->d_name, "..") == 0)
                continue;

            /* Turn relative path into complete path. */
            cpath = (char *)f_malloc(path_len + 1 + strlen(dir_entry->d_name) + 1);
            sprintf(cpath, "%s/%s", path, dir_entry->d_name);

            log_msg(DEBUG, "Checking `%s'.", cpath);
            if(!ws->excl || regexec(ws->excl, cpath, 0, NULL, 0) != 0)
                /* Watch subdirectory. */
                reg_dir(ws, cur_depth + 1, cpath);

            free(cpath);
        }
    }

    return -1;
}

static const size_t EVENT_SIZE = (sizeof(struct inotify_event) + 16)*1024;


static struct inotify_event* read_event(int fd)
{
    struct inotify_event* buf;
    ssize_t nbytes;

    /* Allocate memory. */
    buf = (struct inotify_event*)f_malloc(EVENT_SIZE);
    nbytes = read(fd, buf, EVENT_SIZE);

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
    struct inotify_event *ev_buf;


    while((ev_buf = read_event(ws->notify_descr)) != NULL) {
        struct dir_watch *ev_src;  /* The source directory of and event. */
        struct stat s_ev_src;  /* Stat entry for the source directory. */
        char *cpath;  /* Complete path. */


        log_msg(DEBUG, "Event occured ...");

        /* Get the entry from the watch table. */
        ev_src = get_dir_watch(ws, ev_buf->wd);
        assert(ev_src);

        cpath = (char *)f_malloc(ws->src.len + 1 +
                                 strlen(ev_src->path) + 1 +
                                 strlen(ev_buf->name) + 1);
        sprintf(cpath, "%s/%s/%s", ws->src.str, ev_src->path, ev_buf->name);

        /* Check the file-type of the file reported by inotify. */
        if(stat(cpath, &s_ev_src) < 0)
            log_msg(WARN, "Stat failed for `%s': %s", cpath, strerror(errno));
        else {
            char *rel_path;  /* Relative path of the source directory. */

            rel_path = (char *)f_malloc(strlen(ev_src->path) + 1 +
                                        strlen(ev_buf->name) + 1);
            sprintf(rel_path, "%s/%s", ev_src->path, ev_buf->name);

            if(S_ISDIR(s_ev_src.st_mode) &&
               (ev_buf->mask & IN_CREATE) == IN_CREATE) {
                /* The file is a directory (newly created). So add it to the
                   watch-table. */
                log_msg(DEBUG, "Adding newly created directory: %s", rel_path);

                reg_dir(ws, ev_src->depth_level + 1, rel_path);
            }

            if(!ws->excl || regexec(ws->excl, rel_path, 0, NULL, 0) != 0) {
                log_msg(DEBUG, "Synchronizing file: %s", rel_path);
                sync_file(ws, ev_src->path, ev_buf->name);
            }

            free(rel_path);
        }

        free(cpath);
        free(ev_buf);
    }
}

int run_watcher(struct watch_session *ws)
{
    FILE *log_file_descr;  /* Holds log file descriptor. */


    if(ws->daemon) {
        FILE *pid_file;  /* File descriptor for pid file. */

        /* Prepare everything for daemon spawning. */
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

        if(daemonize(pid_file) < 0)
            return EXIT_FAILURE;
    }

    /* Stderr is fallback log descriptor. */
    log_file_descr = stderr;

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
