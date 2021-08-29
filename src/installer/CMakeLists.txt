
cmake_minimum_required(VERSION 3.4.3)


project(installer_lib)

set(target installer_lib)

add_library(${target}
	STATIC
		source/defs.h
		source/terms_and_conditions.h
		source/wincontroller.h
		source/wincontroller.cpp
		source/installer.h
		source/installer.cpp
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



project(installer_example)
set(target TheExampleInstaller)

set(${target}_sources
	source/main.cpp
)



vstgui_add_executable(${target} "${${target}_sources}")

set(UBERTON_PLUGIN_NAME "\"TesseractFFX\"")
set(UBERTON_PLUGIN_VERSION "\"1.0.0\"")
configure_file(source/plugin_defs.h.in plugin_defs.h)
target_include_directories(${target} PUBLIC ${PROJECT_BINARY_DIR})

target_compile_features(${target} PUBLIC cxx_std_17)
set_target_properties(${target} PROPERTIES ${UBERTON_INSTALLER_FOLDER})

target_link_libraries(${target} PUBLIC ${INSTALLER_LIB})

target_include_directories(${target} PUBLIC "${UBERTON_SRC_PATH}/src/installer/source")
target_include_directories(${target} PUBLIC "${SMTG_VSTGUI_ROOT}/vstgui4")

function (addPluginInstaller)
    set(args NAME)
    cmake_parse_arguments(PARSE_ARGV 0 ARG "${options}" "${args}" "${list_args}")


	project("${ARG_NAME}_Installer")
	
	set(target "${ARG_NAME}_Installer")
	
	set(${target}_sources
		"${ABSOLUTE_INSTALLER_PATH}/source/main.cpp"
	)
	vstgui_add_executable(${target} "${${target}_sources}")
	
	set(UBERTON_PLUGIN_NAME "\"${ARG_NAME}\"")
	set(UBERTON_PLUGIN_VERSION "\"1.0.0\"")

	configure_file("${ABSOLUTE_INSTALLER_PATH}/source/plugin_defs.h.in" plugin_defs.h)

	message(STATUS ${target})
	target_link_libraries(${target} PUBLIC installer_lib)
	target_compile_features(${target} PUBLIC cxx_std_17)
	target_include_directories(${target} PUBLIC ${PROJECT_BINARY_DIR})
	target_include_directories(${target} PUBLIC "${UBERTON_SRC_PATH}/src/installer/source")
	set_target_properties(${target} PROPERTIES ${UBERTON_INSTALLER_FOLDER})

	#get_property(${target}_vstlocation TARGET ${ARG_NAME} LOCATION_RELEASE)
	#message(STATUS ${target}_vstlocation)
	#set(${target}_vstlocation $<$<TARGET_FILE_DIR:${ARG_NAME}>>)
	#message(STATUS "${${target}_vstlocation}")


endfunction()