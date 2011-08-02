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

#include "logging.h"

#include <stdarg.h>
#include <assert.h>


/* Static fields to store all configuration data. */
static FILE *_log_descr;
static int _log_filter;


void init_log(FILE *c, BOOL nl, int filter)
{
    assert(c);

    _log_descr = c;
    log_nl = nl;
    _log_filter = filter;
}

void log_msg(int l, const char *fmt, ...)
{
    assert(_log_descr);

    if((_log_filter & l) == l) {
        va_list ap;

        va_start(ap, fmt);
        vfprintf(_log_descr, fmt, ap);
        va_end(ap);

        if(log_nl)
            fputc('\n', _log_descr);
        fflush(_log_descr);
    }
}

void close_log()
{
    /* Clean up */
    if(_log_descr != stderr && _log_descr != stdout)
        fclose(_log_descr);
}
