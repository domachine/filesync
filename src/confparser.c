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
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <lua.h>
#include <lauxlib.h>

#include "watch_session.h"
#include "logging.h"


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

/* Length of chunks to read. */
#define BUF_SIZE 256

/* Struct to save reader status. */
struct buffer
{
    int fd;
    char buf[BUF_SIZE];
    char end;
};

static const char *file_reader(lua_State *lua, void *data, size_t *size)
{
    struct buffer *bf = (struct buffer *)data;

    if(bf->end)
        return NULL;

    *size = read(bf->fd, bf->buf, BUF_SIZE);
    if(*size <= 0)
        /* End reached. */
        return NULL;

    if(*size < BUF_SIZE)
        bf->end = 1;

    return bf->buf;
}

int parse_conf_fd(struct watch_session *ws, int cfile, const char *fname)
{
    lua_State *lua = luaL_newstate();
    struct buffer bf;

    (void)memset(&bf, 0, sizeof(struct buffer));
    bf.fd = cfile;
    bf.end = 0;

    /* Load config file onto stack */
    if(lua_load(lua, file_reader, &bf, fname) != 0) {
        log_msg(WARN, "error while loading %s: %s\n",
                cfile, lua_tostring(lua, -1));
        return -1;
    }

    if(lua_pcall(lua, 0, 0, 0) != 0) {
        log_msg(WARN, "error while calling %s: %s\n",
                cfile, lua_tostring(lua, -1));
        return -1;
    }

    /* Expose the watch_session properties to the lua side. */
    SET_STR_PROP("src", watch_session_set_src);
    SET_STR_PROP("target", watch_session_set_target);
    SET_STR_PROP("exclude", watch_session_set_ext_excl);
    SET_STR_PROP("pid_file", watch_session_set_pid_file);
    SET_STR_PROP("log_file", watch_session_set_log_file);

    SET_NUM_PROP("depth", watch_session_set_depth);
    SET_NUM_PROP("watch_mask", watch_session_set_depth);
    SET_NUM_PROP("daemon", watch_session_set_daemon);

    lua_close(lua);

    return 0;
}

int parse_conf(struct watch_session *ws, const char *cfile)
{
    int fd = open(cfile, O_RDONLY);
    int ret;

    if(fd == -1) {
        log_msg(WARN, "Failed to open %s: %s",
                cfile, strerror(errno));
        return -1;
    }

    ret = parse_conf_fd(ws, fd, cfile);
    close(fd);

    return ret;
}
