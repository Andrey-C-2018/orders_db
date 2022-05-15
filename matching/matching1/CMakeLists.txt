cmake_minimum_required(VERSION 3.15)

project(matching1 VERSION 0.4 LANGUAGES CXX)
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

#add_subdirectory(${XLIBS_ROOT_FOLDER}/basic basic_build)
add_subdirectory(${XLIBS_ROOT_FOLDER}/date date_build)
add_subdirectory(${XLIBS_ROOT_FOLDER}/db db_build)
add_subdirectory(${XLIBS_ROOT_FOLDER}/dbf3 dbf3_build)

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
target_include_directories(matching1 PRIVATE ${XLIBS_ROOT_FOLDER})
target_include_directories(matching1 PUBLIC ${SQLite3_INCLUDE_DIRS})
target_sources(matching1 PRIVATE DbfTableAdapter.cpp)

target_link_libraries(matching1 basic date db dbf3 sqlite3)