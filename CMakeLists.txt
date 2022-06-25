# Copyright 2021-2022 Nikita Fediuchin. All rights reserved.
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

if (${CMAKE_SOURCE_DIR} MATCHES ${CMAKE_BINARY_DIR})
	message(FATAL_ERROR "Prevented in-tree build")
endif ()

if (TARGET conf-static)
	return()
endif ()

project(conf
	VERSION 1.3.1
	DESCRIPTION "Simple and fast configuration file library"
	HOMEPAGE_URL "https://github.com/cfnptr/conf"
	LANGUAGES C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED TRUE)

option(CONF_BUILD_SHARED "Build CONF shared library" ON)
option(CONF_BUILD_TESTS "Build CONF library tests" ON)

add_subdirectory(libraries/mpio)

configure_file(
	cmake/defines.h.in
	include/conf/defines.h)

set(CONF_LINK_LIBRARIES mpio-static)

if (CMAKE_SYSTEM_NAME MATCHES "Linux")
	list(APPEND CONF_LINK_LIBRARIES m)
endif ()

set(CONF_SOURCES
	source/reader.c
	source/writer.c)

add_library(conf-static STATIC
	${CONF_SOURCES})
set_target_properties(conf-static PROPERTIES
	OUTPUT_NAME "conf")
target_link_libraries(conf-static PUBLIC
	${CONF_LINK_LIBRARIES})
target_include_directories(conf-static PUBLIC
	${PROJECT_BINARY_DIR}/include
	${PROJECT_SOURCE_DIR}/include)

if (CONF_BUILD_SHARED)
	add_library(conf-shared SHARED
		${CONF_SOURCES})
	set_target_properties(conf-shared PROPERTIES
		OUTPUT_NAME "conf"
		WINDOWS_EXPORT_ALL_SYMBOLS ON)
	target_link_libraries(conf-shared PUBLIC
		${CONF_LINK_LIBRARIES})
	target_include_directories(conf-shared PUBLIC
		${PROJECT_BINARY_DIR}/include
		${PROJECT_SOURCE_DIR}/include)
endif ()

if (CONF_BUILD_TESTS)
	enable_testing()

	add_executable(TestConfReader tests/test_reader.c)
	target_link_libraries(TestConfReader PUBLIC conf-static)
	add_test(NAME TestConfReader COMMAND TestConfReader)

	add_executable(TestConfWrite tests/test_writer.c)
	target_link_libraries(TestConfWrite PUBLIC conf-static)
	add_test(NAME TestConfWrite COMMAND TestConfWrite)
endif ()
