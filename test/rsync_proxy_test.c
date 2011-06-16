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

#include <unistd.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>

#include <assert.h>
#include <errno.h>

#include "watch_session.h"
#include "rsync_proxy.h"
#include "utils.h"
#include "logging.h"


int main()
{
    init_log(stderr, TRUE, DEBUG);

    if(mkdir("test-src", S_IRWXU | S_IRWXO) != 0 || mkdir("test-target", S_IRWXU | S_IRWXO) != 0)
        perror("Mkdir failed");

    FILE *file = fopen("test-src/file", "w");
    if(!file)
        perror("Failed to write test file");
    else
        fclose(file);

    struct watch_session *ws = new_watch_session();

    clone_str(&ws->rsync_path, "rsync");
    clone_str(&ws->src, "test-src");
    clone_str(&ws->target, "test-target");

    int ret;
    if((ret = sync_file(ws, ".", "file")) != 0)
        log_msg(DEBUG, "rsync-ret: %i", ret);

    destroy_watch_session(ws);

    remove("test-src/file");
    remove("test-target/file");

    rmdir("test-src");
    rmdir("test-target");

    close_log();

    return EXIT_SUCCESS;
}
