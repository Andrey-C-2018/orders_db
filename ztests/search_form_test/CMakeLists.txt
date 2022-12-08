cmake_minimum_required(VERSION 3.10)
project(search_form_test VERSION 1.0 LANGUAGES CXX)

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

#######################################

find_package(UnitTest++ REQUIRED)
if(NOT UTPP_INCLUDE_DIRS)
	message(FATAL_ERROR "the toolkit UnitTest++ is not found on the system")
	return()
endif()
message(STATUS "UnitTest++ include path: ${UTPP_INCLUDE_DIRS}")

#######################################

if(NOT MSVC OR MSVC_TOOLSET_VERSION LESS_EQUAL 80)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/${CMAKE_BUILD_TYPE})
endif()

add_executable(search_form_test main.cpp)
target_include_directories(search_form_test PRIVATE ${XLIBS_ROOT_FOLDER})
target_include_directories(search_form_test PUBLIC ${UTPP_INCLUDE_DIRS})
target_sources(search_form_test PRIVATE FieldsListTest.cpp
		${XLIBS_ROOT_FOLDER}/ordersdb_frontend/search_form/FieldsList.h
		${XLIBS_ROOT_FOLDER}/ordersdb_frontend/search_form/FieldsList.cpp)

target_link_libraries(search_form_test basic UnitTest++)