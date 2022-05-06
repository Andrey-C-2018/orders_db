cmake_minimum_required(VERSION 3.15)

project(matching3 VERSION 0.5 LANGUAGES CXX)
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

function (find_n_add_library LIBNAME1)

	unset(Curr_lib_full_path CACHE)
	find_library(Curr_lib_full_path NAMES ${LIBNAME1} PATHS ${Lib_Path1} ${Lib_Path2})
	if(NOT CURR_LIB_FULL_PATH)
		set(Lib_Paths_Msg ${Lib_Path1})
		if(Lib_Path2)
			string(APPEND Lib_Paths_Msg " or " ${Lib_Path2})
		endif()

		message(FATAL_ERROR "the library ${LIBNAME1} is not found at ${Lib_Paths_Msg}")
		return()
	endif()

	add_library(${LIBNAME1} STATIC IMPORTED)

	if(MSVC_TOOLSET_VERSION GREATER 80)
		string(REPLACE "Debug" "$(Configuration)" Curr_lib_full_path ${Curr_lib_full_path})
		string(REPLACE "Release" "$(Configuration)" Curr_lib_full_path ${Curr_lib_full_path})
	endif()

	message(STATUS "${LIBNAME1} library path: ${Curr_lib_full_path}")
	set_property(TARGET ${LIBNAME1} PROPERTY IMPORTED_LOCATION ${Curr_lib_full_path})
endfunction()

######################################

find_n_add_library(basic)
find_n_add_library(date)
find_n_add_library(csv_file)
find_n_add_library(db)

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
target_include_directories(matching3 PRIVATE ${CMAKE_SOURCE_DIR}/../..)
target_include_directories(matching3 PUBLIC ${MARIADB_INCLUDE_DIRS})

target_link_libraries(matching3 date csv_file db basic mariadb)