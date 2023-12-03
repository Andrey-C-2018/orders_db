cmake_minimum_required(VERSION 3.10)

project(forms_common VERSION 1.1 LANGUAGES CXX)

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

add_library(forms_common STATIC ActDateCellWidget.cpp ActDateCellWidget.h
            ActNameCellWidget.cpp ActNameCellWidget.h
            ActNoFilter.cpp ActNoFilter.h
            AutorizationManager.cpp AutorizationManager.h
            CommonRoutines.cpp CommonRoutines.h
            Constants.h
            Constraints.h
            FilteringComboBox.cpp FilteringComboBox.h
            FlexiblePropepties.cpp FlexiblePropepties.h
            FormsGridEvtHandler.cpp FormsGridEvtHandler.h
            FormsInsertionBinders.h
            Messages.h
            ParametersManager.cpp ParametersManager.h
            PermissionsManager.cpp PermissionsManager.h
            PaymentsDbTableEvtHandler.cpp PaymentsDbTableEvtHandler.h
            PermissionsManager.cpp PermissionsManager.h
            Validators.h
            VernamOneTimePad.h)

target_include_directories(forms_common PRIVATE ${XLIBS_ROOT_FOLDER})
set_target_properties(forms_common PROPERTIES C_STANDARD 14 C_STANDARD_REQUIRED ON)