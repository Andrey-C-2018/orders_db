cmake_minimum_required(VERSION 3.10)

project(db VERSION 0.3.2 LANGUAGES CXX)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif ()
set(CMAKE_SUPPRESS_REGENERATION true)

if (NOT XLIBS_ROOT_FOLDER)
    set(XLIBS_ROOT_FOLDER ${CMAKE_SOURCE_DIR}/..)
endif ()

if (NOT XLIBS_OUTPUT_FOLDER)
    if (MSVC_TOOLSET_VERSION GREATER 80)
        set(XLIBS_OUTPUT_FOLDER ${XLIBS_ROOT_FOLDER}/0libs)
    else ()
        set(XLIBS_OUTPUT_FOLDER ${XLIBS_ROOT_FOLDER}/0libs/${CMAKE_BUILD_TYPE})
    endif ()
endif ()

#######################################

find_package(PkgConfig REQUIRED)
if (NOT PKG_CONFIG_FOUND)
    message(FATAL_ERROR "no Pkg-Config on the system")
    return()
endif ()

pkg_check_modules(MARIADB QUIET libmariadb)
if (NOT ${MARIADB_FOUND})
    message(FATAL_ERROR "the Mariadb library is not found on the system")
    return()
endif ()
message(STATUS "libmariadb include path: ${MARIADB_INCLUDE_DIR}")

#######################################

add_library(db STATIC DbException.cpp
        SQLite/SQLiteCommon.cpp
        SQLite/SQLiteConnection.cpp
        SQLite/SQLiteField.cpp
        SQLite/SQLiteResultSet.cpp
        SQLite/SQLiteResultSetMetadata.cpp
        SQLite/SQLiteStatement.cpp
        MySQL/MySQLConnection.cpp
        MySQL/MySQLConnectionFactory.cpp
        MySQL/MySQLException.cpp
        MySQL/MySQLField.cpp
        MySQL/MySQLResultSet.cpp
        MySQL/MySQLResultSetMetadata.cpp
        MySQL/MySQLStatement.cpp
        MySQL/MySQLStatementException.cpp
        MySQL/MySQLStmtData.cpp
        MySQL/MySQLStmtDataEx.cpp
        MySQL/MySQLVariant.cpp IDbStaticResultSet.h)

target_include_directories(db PRIVATE ${XLIBS_ROOT_FOLDER})
target_include_directories(db PUBLIC ${MARIADB_INCLUDE_DIRS})

set_target_properties(db PROPERTIES C_STANDARD 14 C_STANDARD_REQUIRED ON)