
cmake_minimum_required(VERSION 3.4.3)

project(common_lib)

set(target uberton_common)

# --- common ------
add_library(${target} 
    STATIC
        source/uberton.h
        source/ControllerBase.cpp
        source/ProcessorBase.cpp
        source/ControllerBase.h
        source/ProcessorBase.h
        source/vstmath.h
        source/vstmath.cpp
        source/oscillators.h
        source/resonator.h
        source/parameters.h
        source/filter.h
        source/cube_ewp_n=50.h
        source/cube_ewp_n=200.h
        source/ActionHistory.h
        source/ActionHistory.cpp
        source/ui.h
        source/ui.cpp
        source/viewcreator.h
        source/viewcreator.cpp
        source/viewcreator_globals.h
        source/subcontrollers.h
        source/subcontrollers.cpp
        source/adsr.h
)


# add the dependencies (are they all needed?)
target_link_libraries(${target} 
    PUBLIC
        base
        sdk
        vstgui
        vstgui_uidescription
        sdk_common
        pluginterfaces
        vstgui_support
)

target_include_directories(${target} 
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/..
)

set_target_properties(${target} PROPERTIES ${UBERTON_FOLDER})
target_compile_features(${target} PUBLIC cxx_std_17)

smtg_setup_universal_binary(${target})