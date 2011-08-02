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


#include <signal.h>

#include "logging.h"
#include "watch_session.h"
#include "cmdparser.h"
#include "watcher.h"


/* Holds information about the current session. */
static struct watch_session *ws;

static void clean_up(int sig)
{
    log_msg(INFO, "Shutting down (Signal %d occured) ...", sig);
    destroy_watch_session(ws);
    close_log();

    exit(EXIT_SUCCESS);
}

int main(int argc, char **argv)
{
    /* Print all log messages to stderr per default. */
    FILE *log_file_descr = stderr;
    int ret;  /* Return code of the watcher. */

    /* TODO: make the verbosity level cutomizable through command line. */
    init_log(log_file_descr, TRUE, ALL_CHANNELS);

    signal(SIGINT, clean_up);
    signal(SIGTERM, clean_up);

    /* Build new watch_session. */
    ws = new_watch_session();

    if(parse_cmd_line(ws, argc, argv) < 0)
        return EXIT_FAILURE;

    if(!ws) {
        log_msg(ERROR, "Failed to allocate memory "
                "for watch-session structure.");
        return EXIT_FAILURE;
    }

    if(!ws->src.str) {
        log_msg(ERROR, "No source given.");
        return EXIT_FAILURE;
    }

    if(!ws->target) {
        log_msg(ERROR, "No target given.");
        return EXIT_FAILURE;
    }

    if(ws->src.str[ws->src.len - 1] == '/') {
        ws->src.str[ws->src.len - 1] = '\0';
        --ws->src.len;
    }

    ret = run_watcher(ws);

    clean_up(SIGTERM);
    return ret;
}
