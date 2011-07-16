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

#include <assert.h>

#include "test_utils.h"
#include "watch_session.h"
#include "utils.h"
#include "logging.h"


int main()
{
    init_log(stderr, TRUE, ALL_CHANNELS);
    TEST_MSG("** starting notify test **");

    struct watch_session *ws = new_watch_session();
    assert(ws);

    clone_str(&ws->src.str, "test-src");
    ws->src.len = strlen(ws->src.str);

    clone_str(&ws->target, "test-target");

    if(install_dir_watch(ws, "dir", 1) < 0)
        perror("Failed to add watch");

    close_log();

    return 0;
}
