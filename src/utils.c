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

#include "utils.h"

#include <stdlib.h>
#include <string.h>

#include <assert.h>

#include "logging.h"


int clone_str(char **dest, const char *src)
{
    int src_size = strlen(src) + 1;  /* +1 because of null-byte. */

    assert(dest);
    *dest = (char *)realloc(*dest, src_size);
    if(!dest)
        return -1;

    strncpy(*dest, src, src_size);

    return 0;
}


void *f_malloc(size_t s)
{
    void *mem = malloc(s);
    if(!mem) {
        log_msg(ERROR, "Failed to allocate memory.");
        abort();
    }

    return mem;
}
