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

add_executable(cmdparser_test cmdparser_test.c)
target_link_libraries(cmdparser_test fs ${LUA_LIBRARIES})

add_executable(notify_test notify_test.c)
target_link_libraries(notify_test fs)

add_executable(confparser_test confparser_test.c)
target_link_libraries(confparser_test fs ${LUA_LIBRARIES})

add_executable(rsync_proxy_test rsync_proxy_test.c)
target_link_libraries(rsync_proxy_test fs ${LUA_LIBRARIES})

add_executable(logging_test logging_test.c)
target_link_libraries(logging_test fs ${LUA_LIBRARIES})
