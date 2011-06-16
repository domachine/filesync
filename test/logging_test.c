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

/**
   \file This file implements tests for the logging-module.
*/

#include "test_utils.h"
#include "logging.h"

int main()
{
    TEST_MSG("** starting logging test **");
    init_log(stderr, TRUE, INFO | DEBUG | ERROR);

    log_msg(INFO, "Test info channel");
    log_msg(DEBUG, "Test debug channel");
    log_msg(ERROR, "Test error channel");
    log_msg(WARN, "If you see this message something went wrong!!");

    return EXIT_SUCCESS;
}
