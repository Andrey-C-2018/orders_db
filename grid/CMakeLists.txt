cmake_minimum_required(VERSION 3.15)

project(grid VERSION 0.9 LANGUAGES CXX)
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

add_library(grid STATIC CellConfigurator.h CellsLayout.h FieldComposer.h FieldsProperties.h GridCommonDeclarations.h Grid.h GridLine.h GridTableProxy.h IComposedDrawable.h IConfigurator.h IDrawable.h IFieldsProperties.h IGCParamsList.h IGridCellsContainer.h IInteractiveObject.h IReloadable.h ISharedComposer.h ISizes.h RecordComposer.h RecordEvaluator.h TextCell.h)
target_sources(grid PRIVATE CellConfigurator.cpp Grid.cpp GridTableProxy.cpp)

target_include_directories(grid PRIVATE ${XLIBS_ROOT_FOLDER})
set_target_properties(grid PROPERTIES C_STANDARD 14 C_STANDARD_REQUIRED ON)