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
   \file This file declares convenience macros and functions
   used in various ways.
*/

#ifndef _FILESYNC_UTILS_H_
#define _FILESYNC_UTILS_H_

/* Check memory to be != NULL before freeing it. */
#define FREE_MEM(x) if(x) free(x)

/**
   \brief Simple datatype that implements the boolean type.
*/
typedef enum
{
    FALSE,
    TRUE
} BOOL;

int clone_str(char **dest, const char *src);

#endif  /* _FILESYNC_UTILS_H_ */
