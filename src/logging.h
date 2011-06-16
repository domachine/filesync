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

/**
   \file This file declares the logging interface used in all parts
   of the program.
*/

#ifndef _FILESYNC_LOGGING_H_
#define _FILESYNC_LOGGING_H_

#include <stdio.h>

/* For the BOOL type */
#include "utils.h"


/**
   \brief The log-level datatype that tells the logging
   interface which channel to use.
*/
enum
{
    INFO = 1,
    WARN = 2,
    ERROR = 4,
    DEBUG = 8
};

/** \brief Constant to select all channels. */
#define ALL_CHANNELS (INFO | WARN | ERROR | DEBUG)

/**
   \brief The initialization function.
   \param c The file-descriptor to print the log messages to.
   \param nl TRUE if a newline should be appended to all messages.
   \param filter This field specifies which levels should be logged.
*/
void init_log(FILE *c, BOOL nl, int filter);

/**
   \brief Log a message on the logging channel.
   \param l The level this message has.
   \param fmt A printf like format string.
   \param ... The format arguments.
*/
void log_msg(int l, const char *fmt, ...);

/**
   \brief Closes the logging descriptor if != stdout and != stderr.
   and cleans up.
*/
void close_log();

#endif  /* _FILESYNC_LOGGING_H_ */
