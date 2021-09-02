include(cmake/CMakeRC.cmake)


function(uberton_add_cmrc_resource)
	set(args NAME FILE WHENCE)
	cmake_parse_arguments(PARSE_ARGV 0 ARG "${options}" "${args}" "${list_args}")
	message(STATUS "adding file to resources from folder \"${ARG_FOLDER}\": ${ARG_FILE}")
	
	if(DEFINED ARG_WHENCE)
		set(whence_arg WHENCE "${ARG_WHENCE}")
	endif()

	cmrc_add_resource_library(rc_${ARG_NAME} ALIAS ${ARG_NAME} NAMESPACE ${ARG_NAME} ${ARG_FILE})
	target_link_libraries(${target} PUBLIC ${ARG_NAME})
	set_target_properties(rc_${ARG_NAME} PROPERTIES ${UBERTON_INSTALLER_RESOURCE_FOLDER})
endfunction()


function(uberton_add_cmrc_resource)
	set(args NAME FILE WHENCE)
	cmake_parse_arguments(PARSE_ARGV 0 ARG "${options}" "${args}" "${list_args}")
	message(STATUS "adding file to resources from folder \"${ARG_FOLDER}\": ${ARG_FILE}")
	
	if(DEFINED ARG_WHENCE)
		set(whence_arg WHENCE "${ARG_WHENCE}")
	endif()

	cmrc_add_resource_library(rc_${ARG_NAME} ALIAS ${ARG_NAME} NAMESPACE ${ARG_NAME} ${ARG_FILE})
	target_link_libraries(${target} PUBLIC ${ARG_NAME})
	set_target_properties(rc_${ARG_NAME} PROPERTIES ${UBERTON_INSTALLER_RESOURCE_FOLDER})
endfunction()

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

	cmrc_add_resource_library(rc_${ARG_NAME} ALIAS ${ARG_NAME} NAMESPACE ${ARG_NAME} ${file_list})
	target_link_libraries(${target} PUBLIC ${ARG_NAME})
	set_target_properties(rc_${ARG_NAME} PROPERTIES ${UBERTON_INSTALLER_RESOURCE_FOLDER})
endfunction()


function (uberton_add_plugin_installer)
	set(args NAME)
	cmake_parse_arguments(PARSE_ARGV 0 ARG "${options}" "${args}" "${list_args}")


	project("${ARG_NAME}_Installer")
	set(target "${ARG_NAME}_Installer")
	
	set(${target}_sources
		"${ABSOLUTE_INSTALLER_PATH}/source/main.cpp"
	)
	vstgui_add_executable(${target} "${${target}_sources}")
	if(SMTG_WIN)
		set_target_properties(${target} PROPERTIES LINK_FLAGS "/INCLUDE:wWinMain /MANIFESTUAC:\"level='requireAdministrator' uiAccess='false'\" ")
		#target_link_options(${target} PUBLIC LINK_OPTIONS "MANIFESTUAC:\"level='requireAdministrator' uiAccess='false'\" ")
		#add_link_options("/MANIFESTUAC:\"level='requireAdministrator' uiAccess='false'\" ")
	endif()


	get_target_property(PLUGIN_BINARY_DIR   ${ARG_NAME} SMTG_PLUGIN_BINARY_DIR)
	get_target_property(PLUGIN_PACKAGE_NAME ${ARG_NAME} SMTG_PLUGIN_PACKAGE_NAME)
	get_target_property(TARGET_SOURCE       ${ARG_NAME} SMTG_PLUGIN_PACKAGE_PATH)

	set(UBERTON_PLUGIN_NAME "${ARG_NAME}")
	set(UBERTON_PLUGIN_VERSION "1.0.0")
	set(UBERTON_PLUGIN_RELEASE_DIR "${PLUGIN_BINARY_DIR}/Release/${PLUGIN_PACKAGE_NAME}")


	
	get_property(UBERTON_PLUGIN_HAS_USERGUIDE_PDF TARGET ${ARG_NAME} PROPERTY UBERTON_USERGUIDE_PDF SET)
	if(UBERTON_PLUGIN_HAS_USERGUIDE_PDF)
		get_target_property(UBERTON_PLUGIN_USERGUIDE_PDF ${ARG_NAME} UBERTON_USERGUIDE_PDF)
		set(userguide_pdf "userguide_pdf")
		uberton_add_cmrc_resource(NAME "${ARG_NAME}_${userguide_pdf}" FILE ${UBERTON_PLUGIN_USERGUIDE_PDF})
		#cmrc_add_resource_library(rc_${userguide_pdf} ALIAS ${userguide_pdf} NAMESPACE ${userguide_pdf} ${UBERTON_PLUGIN_USERGUIDE_PDF})
		#set_target_properties(rc_${userguide_pdf} PROPERTIES "Installer/Resource_Projects")
		#target_link_libraries(${target} PUBLIC ${userguide_pdf})
	endif()

	get_property(UBERTON_PLUGIN_HAS_FACTORY_PRESETS TARGET ${ARG_NAME} PROPERTY UBERTON_FACTORY_PRESETS SET)
	if(UBERTON_PLUGIN_HAS_FACTORY_PRESETS)
		get_target_property(UBERTON_PLUGIN_FACTORY_PRESETS_PATH ${ARG_NAME} UBERTON_FACTORY_PRESETS)
		set(factory_presets "factory_presets")
		uberton_add_cmrc_resource_folder(NAME "${ARG_NAME}_${factory_presets}" FOLDER ${UBERTON_PLUGIN_FACTORY_PRESETS_PATH})
	endif()
	
	configure_file("${ABSOLUTE_INSTALLER_PATH}/source/plugin_defs.h.in" plugin_defs.h)
	
	#uberton_add_cmrc_resource_folder(NAME ${vst} FOLDER ${UBERTON_PLUGIN_RELEASE_DIR})

	target_link_libraries(${target} PUBLIC installer_lib)
	add_dependencies(${target} ${ARG_NAME})
	target_compile_features(${target} PUBLIC cxx_std_17)
	target_include_directories(${target} PUBLIC ${PROJECT_BINARY_DIR})
	target_include_directories(${target} PUBLIC "${UBERTON_SRC_PATH}/src/installer/source")
	set_target_properties(${target} PROPERTIES ${UBERTON_INSTALLER_FOLDER})

endfunction()
