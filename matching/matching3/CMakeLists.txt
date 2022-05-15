cmake_minimum_required(VERSION 3.15)

project(matching3 VERSION 0.5 LANGUAGES CXX)
if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Release)
endif()
set(CMAKE_SUPPRESS_REGENERATION true)

if(NOT XLIBS_ROOT_FOLDER)
	set(XLIBS_ROOT_FOLDER ${CMAKE_SOURCE_DIR}/../..)
endif()

if(NOT XLIBS_OUTPUT_FOLDER)
	if(MSVC_TOOLSET_VERSION GREATER 80)
		set(XLIBS_OUTPUT_FOLDER ${XLIBS_ROOT_FOLDER}/0libs)
	else()
		set(XLIBS_OUTPUT_FOLDER ${XLIBS_ROOT_FOLDER}/0libs/${CMAKE_BUILD_TYPE})
	endif()
endif()

add_subdirectory(${XLIBS_ROOT_FOLDER}/basic basic_build)
add_subdirectory(${XLIBS_ROOT_FOLDER}/date date_build)
add_subdirectory(${XLIBS_ROOT_FOLDER}/csv_file csv_file_build)
add_subdirectory(${XLIBS_ROOT_FOLDER}/db db_build)

######################################

find_package(PkgConfig REQUIRED)
if(NOT PKG_CONFIG_FOUND)
	message(FATAL_ERROR "no Pkg-Config on the system")
	return()
endif()

pkg_check_modules(MARIADB QUIET libmariadb)
if(NOT ${MARIADB_FOUND})
	message(FATAL_ERROR "the Mariadb library is not found on the system")
	return()
endif()

if(NOT MARIADB_INCLUDE_DIR)
	message(STATUS "libmariadb is at the default include path")
else()
	message(STATUS "libmariadb include path: ${MARIADB_INCLUDE_DIR}")
endif()

#######################################

if(NOT MSVC OR MSVC_TOOLSET_VERSION LESS_EQUAL 80)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/${CMAKE_BUILD_TYPE})
endif()

add_executable(matching3 main.cpp)
set_target_properties(matching3 PROPERTIES C_STANDARD 14 C_STANDARD_REQUIRED ON)
target_include_directories(matching3 PRIVATE ${XLIBS_ROOT_FOLDER})
target_include_directories(matching3 PUBLIC ${MARIADB_INCLUDE_DIRS})

target_link_libraries(matching3 date csv_file db basic mariadb)