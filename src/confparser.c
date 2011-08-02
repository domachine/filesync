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


#include "confparser.h"

#include <errno.h>
#include <string.h>
#include <dirent.h>

#include <lua.h>
#include <lauxlib.h>

#include "watch_session.h"


int lua_scandir(lua_State *lua)
{
    /* First argument. */
    const char *dir_path = luaL_checkstring(lua, -1);
    struct dirent **dirs;  /* The selected directories. */
    int dir_num;  /* The number of read directories. */
    int i;  /* Runtime variable. */

    lua_newtable(lua);

    if((dir_num = scandir(dir_path, &dirs, NULL, alphasort)) == -1)
        luaL_error(lua, "scandir: %s", strerror(errno));

    for(i = 0; dir_num > 0; --dir_num, ++i) {
        lua_pushstring(lua, dirs[i]->d_name);
        lua_pushnumber(lua, i + 1);

        lua_settable(lua, -2);
    }

    /* Array still resides on stack. */
    return 1;
}

#define SET_PROP(name, set_func, conv_func) do {  \
        lua_getglobal(lua, name);                       \
        if(!lua_isnil(lua, -1))                         \
            set_func(ws, conv_func(lua, -1));           \
        lua_pop(lua, -1);                               \
    } while(0)

#define SET_STR_PROP(name, set_func) SET_PROP(name, set_func, lua_tostring)
#define SET_NUM_PROP(name, set_func) SET_PROP(name, set_func, lua_tonumber)

int parse_conf(struct watch_session *ws, const char *cfile)
{
    lua_State *lua = luaL_newstate();

    lua_register(lua, "listdir", lua_scandir);

    /* Expose the watch_session properties to the lua side. */
    SET_STR_PROP("src", watch_session_set_src);
    SET_STR_PROP("target", watch_session_set_target);
    SET_STR_PROP("exclude", watch_session_set_ext_excl);
    SET_STR_PROP("pid_file", watch_session_set_pid_file);
    SET_STR_PROP("log_file", watch_session_set_log_file);

    SET_NUM_PROP("depth", watch_session_set_depth);
    SET_NUM_PROP("watch_mask", watch_session_set_depth);
    SET_NUM_PROP("daemon", watch_session_set_daemon);

    return 0;
}
