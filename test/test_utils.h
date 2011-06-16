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
   \file This provides some test utilities which can
   be used in module tests.
*/

#ifndef _FILESYNC_TEST_UTILS_H_
#define _FILESYNC_TEST_UTILS_H_

#include <stdio.h>
#include <stdlib.h>

#define TEST_MSG_V(x, args ...) (fprintf(stderr, "[INFO]  %s:%i: ", \
                                         __FILE__, __LINE__),       \
                                 fprintf(stderr, x "\n", args))

#define TEST_MSG(x) (fprintf(stderr, "[INFO]  %s:%i: ", \
                             __FILE__, __LINE__),       \
                     fprintf(stderr, x "\n"))

#define TEST_WARN_V(x, args ...) (fprintf(stderr, "[WARN]  %s:%i: ", \
                                          __FILE__, __LINE__),       \
                                  fprintf(stderr, x "\n", args))

#define TEST_WARN(x) (fprintf(stderr, "[WARN]  %s:%i: ", \
                              __FILE__, __LINE__),       \
                      fprintf(stderr, x "\n"))

#define TEST_ERROR_V(x, args ...) (fprintf(stderr, "[ERROR] %s:%i: ", \
                                           __FILE__, __LINE__),       \
                                   fprintf(stderr, x "\n", args))

#define TEST_ERROR(x) (fprintf(stderr, "[ERROR] %s:%i: ", \
                               __FILE__, __LINE__),       \
                       fprintf(stderr, x "\n"))

#define TEST_CHECK(x) if(!(x)) {                                    \
        TEST_ERROR_V("Test failed: %s", #x);                          \
        return EXIT_FAILURE;                                        \
    }

#endif  /* _FILESYNC_TEST_UTILS_H_ */
