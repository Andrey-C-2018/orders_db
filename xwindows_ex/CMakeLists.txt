cmake_minimum_required(VERSION 3.15)

project(xwindows_ex VERSION 0.8 LANGUAGES CXX)

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

add_library(xwindows_ex STATIC HorizontalSizer.h ITabStopAction.h ITabStopManager.h Sizer.h VerticalSizer.h XCtrlInputFilter.h XCurrencyField.h XDateField.h XDynamicSizer.h XNullWidget.h XTabStopEdit.h XTabStopFrame.h XTabStopManager.h XTabStopPanel.h XTabStopWidget.h)
target_sources(xwindows_ex PRIVATE HorizontalSizer.cpp Sizer.cpp VerticalSizer.cpp XCurrencyField.cpp XDateField.cpp XDynamicSizer.cpp XNullWidget.cpp XTabStopEdit.cpp XTabStopManager.cpp)

target_include_directories(xwindows_ex PRIVATE ${XLIBS_ROOT_FOLDER})
set_target_properties(xwindows_ex PROPERTIES C_STANDARD 14 C_STANDARD_REQUIRED ON)