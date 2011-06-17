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

#include "rsync_proxy.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <unistd.h>
#include <sys/wait.h>

#include "watch_session.h"
#include "utils.h"
#include "logging.h"


/* Preserve robustness. */
#define CHECK_PTR(x) if(!(x)) return -1


static int spawn_rsync(const char *rsync_path, char *const *argv)
{
    int pid;
    int fd[2];
    if(pipe(fd) < 0) return -1;

    if((pid = fork()) == -1)
        return -1;
    else if(pid == 0) {
        /* Child process. */
        close(fd[0]);

        log_msg(DEBUG, "Spawned child");
        /* Redirect standard error. */
        dup2(fd[1], 2);
        //close(2);

        /* Simply spawn the process. */
        execvp(rsync_path, argv);

        return -1;
    }

    close(fd[1]);

    char buf;

    log_msg(DEBUG, "Back to parent");
    /* FIXME: Send this through the logging interface. */
    while(read(fd[0], (void *)&buf, 1))
        putc(buf, stderr);

    int ret_code = -1;
    wait(&ret_code);

    return ret_code;
}


int sync_file(struct watch_session *ws, const char *dir, const char *file)
{
    char **argv;

    argv = (char **)malloc(5 * sizeof(char *));

    argv[0] = ws->rsync_path;

    /* TODO: Cache ws->src length. */
    /* Create source argument. */
    int src_len = ws->src_len + strlen(dir) + strlen(file) + 3; /* +3 because of slashes and \0 */
    argv[1] = (char *)malloc(src_len);

    CHECK_PTR(argv[1]);

    sprintf(argv[1], "%s/%s/%s", ws->src, dir, file);
    log_msg(DEBUG, "src: %s", argv[1]);

    /* Create target argument. */
    int target_len = strlen(ws->target) + strlen(dir) + 2; /* +2 because of slashes and \0 */
    argv[2] = (char *)malloc(target_len);

    CHECK_PTR(argv[2]);

    sprintf(argv[2], "%s/%s", ws->target, dir);
    log_msg(DEBUG, "target: %s", argv[2]);

    argv[3] = "-d";
    argv[4] = NULL;

    int ret = spawn_rsync(ws->rsync_path, argv);

    /* Clean up ... */
    FREE_MEM(argv[1]);
    FREE_MEM(argv[2]);

    return ret;
}
