cmake_minimum_required(VERSION 3.10)

project(db_ext VERSION 0.7 LANGUAGES CXX)

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

add_library(db_ext STATIC DbTable.cpp
        DbInsertHelper.cpp
        DependentTableUpdater.cpp
        FilteringManager.cpp
        MetaInfoBasic.cpp
        MetaInfo.cpp
        ScalarQueryCache.cpp
        QueryModifier.cpp
        RevDbResultSet.cpp
        SortingManager.cpp
        PaginalRS.cpp
        PaginalRS.h
        PaginalStatement.cpp
        PaginalStatement.h
        UniqueRowIdRS.cpp
        UniqueRowIdRS.h)

target_include_directories(db_ext PRIVATE ${XLIBS_ROOT_FOLDER})

set_target_properties(db_ext PROPERTIES C_STANDARD 14 C_STANDARD_REQUIRED ON)