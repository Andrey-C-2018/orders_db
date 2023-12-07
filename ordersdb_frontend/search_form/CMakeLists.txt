cmake_minimum_required(VERSION 3.10)

project(search_form VERSION 1.1 LANGUAGES CXX)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif ()

if (NOT XLIBS_ROOT_FOLDER)
    set(XLIBS_ROOT_FOLDER ${CMAKE_SOURCE_DIR}/../..)
endif ()

if (NOT XLIBS_OUTPUT_FOLDER)
    if (MSVC_TOOLSET_VERSION GREATER 80)
        set(XLIBS_OUTPUT_FOLDER ${XLIBS_ROOT_FOLDER}/0libs)
    else ()
        set(XLIBS_OUTPUT_FOLDER ${XLIBS_ROOT_FOLDER}/0libs/${CMAKE_BUILD_TYPE})
    endif ()
endif ()

#######################################

if (NOT ${MARIADB_FOUND})
    find_package(PkgConfig REQUIRED)
    if(NOT PKG_CONFIG_FOUND)
        message(FATAL_ERROR "no Pkg-Config on the system")
        return()
    endif()

    pkg_check_modules(MARIADB QUIET libmariadb)
    if (NOT ${MARIADB_FOUND})
        message(FATAL_ERROR "the Mariadb library is not found on the system")
        return()
    endif()
    message(STATUS "libmariadb include path: ${MARIADB_INCLUDE_DIR}")
endif()

#######################################

if(NOT MSVC OR MSVC_TOOLSET_VERSION LESS_EQUAL 80)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/${CMAKE_BUILD_TYPE})
endif()

add_executable(search_form MyApp.cpp MyApp.h
        MyFrame.cpp MyFrame.h
        DoubleBndTarget.cpp DoubleBndTarget.h
        FieldsList.cpp FieldsList.h
        Inserter.cpp Inserter.h
        IUploadProgress.h
        OrderingComboBox.cpp OrderingComboBox.h
        OrdersInsertHelper.cpp OrdersInsertHelper.h
        PaidFilter.cpp PaidFilter.h
        PaymentsInsertHelper.cpp PaymentsInsertHelper.h
        CenterBinder.cpp CenterBinder.h
        resource.h
        SearchForm.cpp SearchForm.h
        SearchFormGridEvtHandler.cpp SearchFormGridEvtHandler.h
        UploadDialog.cpp UploadDialog.h
        Uploader.cpp Uploader.h
        ZoneFilter.cpp ZoneFilter.h)

set_target_properties(search_form PROPERTIES C_STANDARD 14 C_STANDARD_REQUIRED ON)
target_include_directories(search_form PRIVATE ${XLIBS_ROOT_FOLDER})
target_include_directories(search_form PRIVATE ${XLIBS_ROOT_FOLDER}/ordersdb_frontend)
target_include_directories(search_form PUBLIC ${MARIADB_INCLUDE_DIRS})