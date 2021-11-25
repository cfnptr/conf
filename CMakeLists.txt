cmake_minimum_required(VERSION 3.10)

if (${CMAKE_SOURCE_DIR} MATCHES ${CMAKE_BINARY_DIR})
	message(FATAL_ERROR "Prevented in-tree build")
endif ()

if (TARGET conf)
	return()
endif ()

project(conf
	VERSION 0.0.0
	DESCRIPTION "Simple and fast configuration file library"
	HOMEPAGE_URL "https://github.com/cfnptr/json"
	LANGUAGES C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED TRUE)

option(CONF_BUILD_SHARED "Build CONF shared library" ON)
option(CONF_ENABLE_TESTING "Enable CONF tests building" ON)

add_subdirectory(libraries/mpio)

configure_file(
	cmake/defines.h.in
	conf/defines.h)

set(CONF_LINK_LIBRARIES mpio)

if(CMAKE_SYSTEM_NAME MATCHES "Linux")
	list(APPEND CONF_LINK_LIBRARIES m)
endif()

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
	${PROJECT_BINARY_DIR}
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
		${PROJECT_BINARY_DIR}
		${PROJECT_SOURCE_DIR}/include)
endif ()

if (CONF_ENABLE_TESTING)
	enable_testing()

	add_executable(TestConfReader tests/test_reader.c)
	target_link_libraries(TestConfReader PUBLIC conf-static)
	add_test(NAME TestConfReader COMMAND TestConfReader)

	add_executable(TestConfWrite tests/test_writer.c)
	target_link_libraries(TestConfWrite PUBLIC conf-static)
	add_test(NAME TestConfWrite COMMAND TestConfWrite)
endif ()
