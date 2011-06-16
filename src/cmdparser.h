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
   \file Declares the command line parser which parses the commandline
   arguments given by the user.
*/

#ifndef _FILESYNC_CMDPARSER_H_
#define _FILESYNC_CMDPARSER_H_

struct watch_session;

/**
   \brief Parses the command line and writes the
   extracted information to the given watch_session.

   Errors and warnings are logged through the logging interface.
   A return-value != 0 indicates an error while parsing.

   \param ws A pointer to the watch_session to write information to.
   \param argc The number of command-line arguments (always > 1).
   \param argv The command line argument vector (should include the program name).
   \return 0 on success. -1 on failure.
*/
int parse_cmd_line(struct watch_session *ws, int argc, char *const *argv);

#endif  /* _FILESYNC_CMDPARSER_H_ */
