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

#include "cmdparser.h"

#include <limits.h>
#include <stdlib.h>
#include <getopt.h>
#include <assert.h>

#include "watch_session.h"
#include "logging.h"


int parse_cmd_line(struct watch_session *ws, int argc, char *const *argv)
{
    struct option long_options[] =
        {
            {"rsync", required_argument, NULL, 'r'},
            {"help",  no_argument,       NULL, 'h'},
            {"depth", required_argument, NULL, 'd'},
            {NULL,    0,                 NULL, 0}
        };

    const char *short_options = "r:hd:";

    /* Loop through command line arguments
     * and interprete them. */
    while(1) {
        int option_index = 0;
        int c = -1;

        /* Retrieve next option */
        c = getopt_long(argc, argv, short_options,
                        long_options, &option_index);

        if(c == -1)
            break;

        switch(c) {
        case 0:
            break;
        case 'r':
        {
            int r = clone_str(&ws->rsync_path, optarg);

            /* Check that memory was successfully allocated. */
            assert(r == 0);
            break;
        }
        case 'd':
            ws->depth = atoi(optarg);
            if(ws->depth == INT_MAX) {
                log_msg(WARN,
                        "error while parsing depth argument, "
                        "settings depth to -1");
                ws->depth = -1;
            }
            break;
        case '?':
            return -1;
        default:
            /* This case should be impossible to reach */
            printf("val: %d\n", c);
            abort();
        }
    }

    int add_args = argc - optind;
    int args = optind;

    /* Parse remaining arguments */
    if(add_args >= 1) {

        if(clone_str(&ws->src, argv[args]) < 0)
            return -1;

        if(--add_args >= 1) {
            if(clone_str(&ws->target, argv[++args]) < 0)
                return -1;
        }
    }

    /* TODO: Build rsync command ... */
    return 0;
}