# Copyright 2021-2023 Nikita Fediuchin. All rights reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

cmake_minimum_required(VERSION 3.16)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
	message(FATAL_ERROR "Prevented in-tree build")
endif()

if(TARGET conf-static)
	return()
endif()

project(conf VERSION 1.4.0 LANGUAGES C
	DESCRIPTION "Simple and fast multi-platform \
 		configuration file library (similar to .ini) "
	HOMEPAGE_URL "https://github.com/cfnptr/conf")

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED TRUE)

option(CONF_BUILD_SHARED "Build Conf shared library" ON)
option(CONF_BUILD_TESTS "Build Conf library tests" ON)

set(MPIO_BUILD_SHARED OFF CACHE BOOL "" FORCE)
add_subdirectory(libraries/mpio)

configure_file(cmake/defines.h.in include/conf/defines.h)

set(CONF_LINK_LIBS mpio-static)
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
	list(APPEND CONF_LINK_LIBS m)
endif()

set(CONF_SOURCES source/reader.c source/writer.c)
set(CONF_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/include
	${PROJECT_SOURCE_DIR}/include ${PROJECT_SOURCE_DIR}/wrappers/cpp)

add_library(conf-static STATIC ${CONF_SOURCES})
target_link_libraries(conf-static PUBLIC ${CONF_LINK_LIBS})
target_include_directories(conf-static PUBLIC ${CONF_INCLUDE_DIRS})

if(CONF_BUILD_SHARED)
	add_library(conf-shared SHARED ${CONF_SOURCES})
	set_target_properties(conf-shared PROPERTIES
		OUTPUT_NAME "conf" WINDOWS_EXPORT_ALL_SYMBOLS ON)
	target_link_libraries(conf-shared PUBLIC ${CONF_LINK_LIBS})
	target_include_directories(conf-shared PUBLIC ${CONF_INCLUDE_DIRS})
endif()

if(CONF_BUILD_TESTS)
	enable_testing()

	add_executable(TestConfReader tests/test_reader.c)
	target_link_libraries(TestConfReader PUBLIC conf-static)
	add_test(NAME TestConfReader COMMAND TestConfReader)

	add_executable(TestConfWriter tests/test_writer.c)
	target_link_libraries(TestConfWriter PUBLIC conf-static)
	add_test(NAME TestConfWriter COMMAND TestConfWriter)
endif()
