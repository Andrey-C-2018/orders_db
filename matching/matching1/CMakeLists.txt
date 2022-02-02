cmake_minimum_required(VERSION 3.15)

project(matching1 VERSION 0.4 LANGUAGES CXX)
if(NOT CMAKE_BUILD_TYPE)
       set(CMAKE_BUILD_TYPE Release)
endif()

if(MSVC_TOOLSET_VERSION GREATER 80)
	set(Lib_Path1 ${CMAKE_SOURCE_DIR}/../../0libs/Debug)
	set(Lib_Path2 ${CMAKE_SOURCE_DIR}/../../0libs/Release)
else()
	set(Lib_Path1 ${CMAKE_SOURCE_DIR}/../../0libs/${CMAKE_BUILD_TYPE})
	set(Lib_Path2 "")
endif()
set(CMAKE_SUPPRESS_REGENERATION true)

#######################################

find_library(BASIC_LIBRARY NAMES basic PATHS ${Lib_Path1} ${Lib_Path2})
if(NOT BASIC_LIBRARY)
	set(Basic_Lib_Path ${Lib_Path1})
	if(Lib_Path2)
		string(APPEND Basic_Lib_Path " or " ${Lib_Path2})
	endif()

	message(FATAL_ERROR "the library basic is not found at ${Basic_Lib_Path}")
	return()
endif()

add_library(basic UNKNOWN IMPORTED)

if(MSVC_TOOLSET_VERSION GREATER 80)
	string(REPLACE "Debug" "$(Configuration)" BASIC_LIBRARY ${BASIC_LIBRARY})
	string(REPLACE "Release" "$(Configuration)" BASIC_LIBRARY ${BASIC_LIBRARY})
endif()
message(STATUS "basic library path: ${BASIC_LIBRARY}")
set_property(TARGET basic PROPERTY IMPORTED_LOCATION ${BASIC_LIBRARY})

#######################################

find_library(DATE_LIBRARY NAMES date PATHS ${Lib_Path1} ${Lib_Path2})
if(NOT DATE_LIBRARY)
	set(Date_Lib_Path ${Lib_Path1})
	if(Lib_Path2)
		string(APPEND Date_Lib_Path " or " ${Lib_Path2})
	endif()

	message(FATAL_ERROR "the library date is not found at ${Date_Lib_Path}")
	return()
endif()

add_library(date UNKNOWN IMPORTED)

if(MSVC_TOOLSET_VERSION GREATER 80)
	string(REPLACE "Debug" "$(Configuration)" DATE_LIBRARY ${DATE_LIBRARY})
	string(REPLACE "Release" "$(Configuration)" DATE_LIBRARY ${DATE_LIBRARY})
endif()
message(STATUS "date library path: ${DATE_LIBRARY}")
set_property(TARGET date PROPERTY IMPORTED_LOCATION ${DATE_LIBRARY})

#######################################

find_library(DB_LIBRARY NAMES db PATHS ${Lib_Path1} ${Lib_Path2})
if(NOT DB_LIBRARY)
	set(Db_Lib_Path ${Lib_Path1})
	if(Lib_Path2)
		string(APPEND Db_Lib_Path " or " ${Lib_Path2})
	endif()

	message(FATAL_ERROR "the library db is not found at ${Db_Lib_Path}")
	return()
endif()

add_library(db UNKNOWN IMPORTED)

if(MSVC_TOOLSET_VERSION GREATER 80)
	string(REPLACE "Debug" "$(Configuration)" DB_LIBRARY ${DB_LIBRARY})
	string(REPLACE "Release" "$(Configuration)" DB_LIBRARY ${DB_LIBRARY})
endif()
message(STATUS "db library path: ${DB_LIBRARY}")
set_property(TARGET db PROPERTY IMPORTED_LOCATION ${DB_LIBRARY})

#######################################

find_library(DBF3_LIBRARY NAMES dbf3 PATHS ${Lib_Path1} ${Lib_Path2})
if(NOT DBF3_LIBRARY)
	set(Dbf3_Lib_Path ${Lib_Path1})
	if(Lib_Path2)
		string(APPEND Dbf3_Lib_Path " or " ${Lib_Path2})
	endif()

	message(FATAL_ERROR "the library dbf3 is not found at ${Dbf3_Lib_Path}")
	return()
endif()

add_library(dbf3 UNKNOWN IMPORTED)

if(MSVC_TOOLSET_VERSION GREATER 80)
	string(REPLACE "Debug" "$(Configuration)" DBF3_LIBRARY ${DBF3_LIBRARY})
	string(REPLACE "Release" "$(Configuration)" DBF3_LIBRARY ${DBF3_LIBRARY})
endif()
message(STATUS "dbf3 library path: ${DBF3_LIBRARY}")
set_property(TARGET dbf3 PROPERTY IMPORTED_LOCATION ${DBF3_LIBRARY})

#######################################

find_package(SQLite3 REQUIRED)
if(NOT SQLite3_FOUND)
	message(FATAL_ERROR "the Sqlite3 library is not found on the system")
	return()
endif()
message(STATUS "libSqlite3 include path: ${SQLite3_INCLUDE_DIRS}")

#######################################

if(NOT MSVC OR MSVC_TOOLSET_VERSION LESS_EQUAL 80)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/${CMAKE_BUILD_TYPE})
endif()

add_executable(matching1 main.cpp DbfTableAdapter.h DbfTableAdapter.cpp Convert.h main.cpp)
set_target_properties(matching1 PROPERTIES C_STANDARD 14 C_STANDARD_REQUIRED ON)
target_include_directories(matching1 PRIVATE ${CMAKE_SOURCE_DIR}/../..)
target_include_directories(matching1 PUBLIC ${SQLite3_INCLUDE_DIRS})
target_sources(matching1 PRIVATE DbfTableAdapter.cpp)

target_link_libraries(matching1 basic date db dbf3 sqlite3)