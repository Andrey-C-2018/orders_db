cmake_minimum_required(VERSION 3.0)

project(db_test VERSION 0.2 LANGUAGES CXX)
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
add_subdirectory(${XLIBS_ROOT_FOLDER}/db db_build)

#######################################

find_package(SQLite3 REQUIRED)
if(NOT SQLite3_FOUND)
	message(FATAL_ERROR "the Sqlite3 library is not found on the system")
	return()
endif()
message(STATUS "libSqlite3 include path: ${SQLite3_INCLUDE_DIRS}")

#######################################

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
message(STATUS "libmariadb include path: ${MARIADB_INCLUDE_DIR}")

#######################################

if(NOT MSVC OR MSVC_TOOLSET_VERSION LESS_EQUAL 80)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/${CMAKE_BUILD_TYPE})
endif()

add_executable(db_test main.cpp)
set_target_properties(db_test PROPERTIES C_STANDARD 14 C_STANDARD_REQUIRED ON)
target_include_directories(db_test PRIVATE ${XLIBS_ROOT_FOLDER})
target_include_directories(db_test PUBLIC ${SQLite3_INCLUDE_DIRS})
target_include_directories(db_test PUBLIC ${MARIADB_INCLUDE_DIRS})
target_include_directories(db_test PUBLIC ${UTPP_INCLUDE_DIRS})
target_sources(db_test PRIVATE MySQLTest.cpp)

target_link_libraries(db_test db basic date mariadb UnitTest++)
