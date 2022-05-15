cmake_minimum_required(VERSION 3.10)

project(basic VERSION 0.8.0 LANGUAGES CXX)

if(NOT CMAKE_BUILD_TYPE)
       set(CMAKE_BUILD_TYPE Release)
endif()
set(CMAKE_SUPPRESS_REGENERATION true)

if(NOT XLIBS_ROOT_FOLDER)
	set(XLIBS_ROOT_FOLDER ${CMAKE_SOURCE_DIR}/..)
endif()

if(NOT XLIBS_OUTPUT_FOLDER)
	if(MSVC_TOOLSET_VERSION GREATER 80)
		set(XLIBS_OUTPUT_FOLDER ${XLIBS_ROOT_FOLDER}/0libs)
	else()
		set(XLIBS_OUTPUT_FOLDER ${XLIBS_ROOT_FOLDER}/0libs/${CMAKE_BUILD_TYPE})
	endif()
endif()

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${XLIBS_OUTPUT_FOLDER})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${XLIBS_OUTPUT_FOLDER})

add_library(basic STATIC Cryptor.h Cryptor.cpp Exception.cpp Exception.h chars.h Delegate.h dll_definitions.h FileSystem.h ImmutableString.h locale_init.h IProperties.h PropertiesFile.cpp PropertiesFile.h Process.h TextConv.h tstring.h tstream.h XConv.h XString.h)
target_sources(basic PRIVATE unix/DirectoryImpl.cpp unix/FileSystemImpl.cpp)

set_target_properties(basic PROPERTIES C_STANDARD 14 C_STANDARD_REQUIRED ON)
