cmake_minimum_required(VERSION 3.15)

project(xwindows VERSION 0.8 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 14)

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

if(NOT MSVC OR MSVC_TOOLSET_VERSION LESS_EQUAL 80)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/${CMAKE_BUILD_TYPE})
endif()

add_library(xwindows STATIC XApp.h XButton.h XLabel.h XPanel.h XProgressBar.h XComboBox.h XEvent.h XEventHandler.h XGC.h XFrame.h XWidget.h XWindow.h platform_specific.h)
target_sources(xwindows PRIVATE XApp.cpp XButton.cpp XLabel.cpp XComboBox.cpp XEvent.cpp XEventHandler.cpp)
target_sources(xwindows PRIVATE XPanel.cpp XProgressBar.cpp XGC.cpp XWindow.cpp XWidget.cpp XFrame.cpp platform_specific.cpp)

target_include_directories(xwindows PRIVATE ${XLIBS_ROOT_FOLDER})
set_target_properties(xwindows PROPERTIES C_STANDARD 14 C_STANDARD_REQUIRED ON)