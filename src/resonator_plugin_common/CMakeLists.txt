
cmake_minimum_required(VERSION 3.4.3)

project(resonator_plugin_common)

set(target resonator_plugin_common)

# --- common ------
add_library(${target} 
    STATIC
        source/common_param_specs.h
        source/ResonatorController.h
        source/ResonatorController.cpp
        source/ResonatorProcessor.h
        source/ResonatorProcessor.cpp
        source/ResonatorProcessorImplBase.h
        source/ResonatorProcessorImpl.h
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
        uberton_common
)

target_include_directories(${target} 
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/..
        ${UBERTON_SRC_PATH}/src/common/source
)

set_target_properties(${target} PROPERTIES ${UBERTON_FOLDER})
target_compile_features(${target} PUBLIC cxx_std_17)

smtg_setup_universal_binary(${target})