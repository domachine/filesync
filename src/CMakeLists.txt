# This program listens on a directory for changes and applies them
# to another location.
# Copyright (C) 2010, 2011  Dominik Burgdörfer <dominik.burgdoerfer@googlemail.com>

# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

set(LIB_SRC watch_session.c
  rsync_proxy.c logging.c
  cmdparser.c utils.c
  notify.c watcher.c
  confparser.c)

set(EXE_SRC main.c)

add_library(fs ${LIB_SRC})
add_executable(filesync ${EXE_SRC})

message(${LUA_LIBRARIES})
target_link_libraries(filesync fs ${LUA_LIBRARIES})
