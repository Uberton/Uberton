
cmake_minimum_required(VERSION 3.4.3)


project(installer_lib)

set(target installer_lib)

add_library(${target}
	STATIC
		source/terms_and_conditions.h
		source/wincontroller.h
		source/installer.h
		source/installer.cpp
		source/locations.h
		source/locations.cpp
)



target_link_libraries(${target}
	PUBLIC
		vstgui_standalone
)


target_include_directories(${target}
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/..
)

set_target_properties(${target} PROPERTIES ${UBERTON_INSTALLER_FOLDER})
target_compile_features(${target} PUBLIC cxx_std_17)
target_include_directories(${target} PUBLIC "${SMTG_VSTGUI_ROOT}/vstgui4")

set(INSTALLER_LIB installer_lib)

smtg_setup_universal_binary(${target})


include(cmake/Installer.cmake)
