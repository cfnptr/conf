cmake_minimum_required(VERSION 3.10)

if (${CMAKE_SOURCE_DIR} MATCHES ${CMAKE_BINARY_DIR})
	message(FATAL_ERROR "Prevented in-tree build")
endif ()

if (TARGET conf)
	return()
endif ()

project(conf
	VERSION 1.0.0
	DESCRIPTION "Simple and fast configuration file library"
	HOMEPAGE_URL "https://github.com/cfnptr/json"
	LANGUAGES C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED TRUE)

add_subdirectory(libraries/mpio)

configure_file(
	cmake/defines.h.in
	conf/defines.h)

add_library(conf STATIC
	source/reader.c)
target_link_libraries(conf PUBLIC
	mpio)
target_include_directories(conf PUBLIC
	${PROJECT_BINARY_DIR}
	${PROJECT_SOURCE_DIR}/include)
