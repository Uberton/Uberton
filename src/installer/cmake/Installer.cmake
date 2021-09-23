include(cmake/CMakeRC.cmake)

# Add a single resource with the cmrc library
function(uberton_add_cmrc_resource)
	set(args NAME FILE WHENCE)
	cmake_parse_arguments(PARSE_ARGV 0 ARG "${options}" "${args}" "${list_args}")
	message(STATUS "adding file to resources from folder \"${ARG_FOLDER}\": ${ARG_FILE}")
	
	if(DEFINED ARG_WHENCE)
		set(whence_arg WHENCE "${ARG_WHENCE}")
	endif()

	cmrc_add_resource_library(rc_${ARG_NAME} ALIAS ${ARG_NAME} NAMESPACE ${ARG_NAME} ${whence_arg} ${ARG_FILE})
	target_link_libraries(${target} PUBLIC ${ARG_NAME})
	set_target_properties(rc_${ARG_NAME} PROPERTIES ${UBERTON_INSTALLER_RESOURCE_FOLDER})
endfunction()


# Add a folder recursively with the cmrc library
function(uberton_add_cmrc_resource_folder)
	set(args NAME FOLDER WHENCE)
	cmake_parse_arguments(PARSE_ARGV 0 ARG "${options}" "${args}" "${list_args}")
	file(GLOB_RECURSE file_list "${ARG_FOLDER}/*")
	message(STATUS "adding files to resources from folder \"${ARG_FOLDER}\":")
	
	foreach (file ${file_list})
		message(STATUS "${file}")
	endforeach()

	if(DEFINED ARG_WHENCE)
		set(whence_arg WHENCE "${ARG_WHENCE}")
	endif()

	cmrc_add_resource_library(rc_${ARG_NAME} ALIAS ${ARG_NAME} NAMESPACE ${ARG_NAME} ${whence_arg} ${file_list})
	target_link_libraries(${target} PUBLIC ${ARG_NAME})
	set_target_properties(rc_${ARG_NAME} PROPERTIES ${UBERTON_INSTALLER_RESOURCE_FOLDER})
endfunction()


# Add an installer project to the specified plugin. 
# - The version is optional and only necessary if the plugin target has not version set up via 
#   uberton_set_plugin_version().
# - If the factory presets folder and a user guide pdf is set for the target via 
#	uberton_set_factory_presets() and uberton_set_userguide_pdf(), then they will be added to the 
#   binary executable with the CMRC library which creates extra projects in the Installers/Resource Projects
#   folder. 
function (uberton_set_plugin_installer)
	set(args NAME VERSION)
	cmake_parse_arguments(PARSE_ARGV 0 ARG "${options}" "${args}" "${list_args}")

	
	get_target_property(PLUGIN_BINARY_DIR   ${ARG_NAME} SMTG_PLUGIN_BINARY_DIR)
	get_target_property(PLUGIN_PACKAGE_NAME ${ARG_NAME} SMTG_PLUGIN_PACKAGE_NAME)
	get_target_property(TARGET_SOURCE       ${ARG_NAME} SMTG_PLUGIN_PACKAGE_PATH)
	get_property(UBERTON_PLUGIN_VERSION_SET TARGET ${ARG_NAME} PROPERTY UBERTON_PLUGIN_VERSION SET)

	set(UBERTON_PLUGIN_NAME ${ARG_NAME})
	if(DEFINED ARG_VERSION)
		set(UBERTON_PLUGIN_VERSION ${ARG_VERSION})
	elseif(UBERTON_PLUGIN_VERSION_SET)
		get_target_property(PLUGIN_VERSION ${ARG_NAME} UBERTON_PLUGIN_VERSION)
		set(UBERTON_PLUGIN_VERSION ${PLUGIN_VERSION})
	else()
		set(UBERTON_PLUGIN_VERSION "1.0.0")
	endif()
	set(UBERTON_PLUGIN_RELEASE_DIR "${PLUGIN_BINARY_DIR}/Release/${PLUGIN_PACKAGE_NAME}")


	project("${ARG_NAME}_Installer")
	set(target "${ARG_NAME}_Installer_${UBERTON_PLUGIN_VERSION}")
	
	set(${target}_sources
		"${ABSOLUTE_INSTALLER_PATH}/source/main.cpp"
	)
	vstgui_add_executable(${target} "${${target}_sources}")
	if(SMTG_WIN)
		set_target_properties(${target} PROPERTIES LINK_FLAGS "/INCLUDE:wWinMain /MANIFESTUAC:\"level='requireAdministrator' uiAccess='false'\" ")
	endif()


	
	get_property(UBERTON_PLUGIN_HAS_USERGUIDE_PDF TARGET ${ARG_NAME} PROPERTY UBERTON_USERGUIDE_PDF SET)
	if(UBERTON_PLUGIN_HAS_USERGUIDE_PDF)
		get_target_property(UBERTON_PLUGIN_USERGUIDE_PDF ${ARG_NAME} UBERTON_USERGUIDE_PDF)
		set(userguide_pdf "userguide_pdf")
		uberton_add_cmrc_resource(NAME "${ARG_NAME}_${userguide_pdf}" FILE ${UBERTON_PLUGIN_USERGUIDE_PDF})
	endif()

	get_property(UBERTON_PLUGIN_HAS_FACTORY_PRESETS TARGET ${ARG_NAME} PROPERTY UBERTON_FACTORY_PRESETS SET)
	if(UBERTON_PLUGIN_HAS_FACTORY_PRESETS)
		get_target_property(UBERTON_PLUGIN_FACTORY_PRESETS_PATH ${ARG_NAME} UBERTON_FACTORY_PRESETS)
		set(factory_presets "factory_presets")
		uberton_add_cmrc_resource_folder(NAME "${ARG_NAME}_${factory_presets}" FOLDER ${UBERTON_PLUGIN_FACTORY_PRESETS_PATH})
	endif()
	
	set(vst_plugin "vst_plugin")
	uberton_add_cmrc_resource_folder(NAME "${ARG_NAME}_${vst_plugin}" WHENCE "${PLUGIN_BINARY_DIR}/Release/" FOLDER ${UBERTON_PLUGIN_RELEASE_DIR})


	configure_file("${ABSOLUTE_INSTALLER_PATH}/source/plugin_defs.h.in" plugin_defs.h)

	target_link_libraries(${target} PUBLIC installer_lib)
	add_dependencies(${target} ${ARG_NAME})
	target_compile_features(${target} PUBLIC cxx_std_17)
	target_include_directories(${target} PUBLIC ${PROJECT_BINARY_DIR})
	target_include_directories(${target} PUBLIC "${UBERTON_SRC_PATH}/src/installer/source")
	set_target_properties(${target} PROPERTIES ${UBERTON_INSTALLER_FOLDER})

endfunction()
