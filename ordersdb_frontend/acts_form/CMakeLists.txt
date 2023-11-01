cmake_minimum_required(VERSION 3.10)

project(acts_form VERSION 1.1 LANGUAGES CXX)

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

#add_subdirectory(${XLIBS_ROOT_FOLDER}/basic basic_build)
#add_subdirectory(${XLIBS_ROOT_FOLDER}/date date_build)
#add_subdirectory(${XLIBS_ROOT_FOLDER}/db db_build)
#add_subdirectory(${XLIBS_ROOT_FOLDER}/db_ext db_ext_build)
#add_subdirectory(${CMAKE_SOURCE_DIR}/forms_common forms_common_build)

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

add_executable(acts_form MyApp.cpp
                MyApp.h
                MyFrame.cpp MyFrame.h
                ActsForm.cpp ActsForm.h
                DefenderDbTableEvtHandler.cpp DefenderDbTableEvtHandler.h
                DefendersList.cpp DefendersList.h
                DefendersListEvtHandler.cpp DefendersListEvtHandler.h
                OrdersDbTableEventsHandler.cpp OrdersDbTableEventsHandler.h
                OrdersGridEvtHandler.cpp OrdersGridEvtHandler.h
                OrdersList.cpp OrdersList.h
                PaymentsGridEvtHandler.cpp PaymentsGridEvtHandler.h
                PaymentsInserter.cpp PaymentsInserter.h
                PaymentsList.cpp PaymentsList.h
                PaymentsNavPanel.cpp PaymentsNavPanel.h
                resource.h)

set_target_properties(acts_form PROPERTIES C_STANDARD 14 C_STANDARD_REQUIRED ON)
target_include_directories(acts_form PRIVATE ${XLIBS_ROOT_FOLDER})
target_include_directories(acts_form PUBLIC ${MARIADB_INCLUDE_DIRS})