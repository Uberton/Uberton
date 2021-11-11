define_property(TARGET PROPERTY UBERTON_USERGUIDE_PDF
	BRIEF_DOCS "Add user guide pdf to vst plugin target" 
	FULL_DOCS "Add user guide pdf to vst plugin target"
)

define_property(TARGET PROPERTY UBERTON_FACTORY_PRESETS
	BRIEF_DOCS "Add factory presets folder to plugin target" 
	FULL_DOCS "Add factory presets folder to plugin target"
)

define_property(TARGET PROPERTY UBERTON_PLUGIN_VERSION
	BRIEF_DOCS "Add a plugin version" 
	FULL_DOCS "Add a commonly styled plugin version to a plugin: MAJOR.MINOR.PATCH"
)


# Specify a pdf manual that is included in an installer
function(uberton_set_userguide_pdf)
	set(args TARGET USERGUIDE_PDF)
    cmake_parse_arguments(PARSE_ARGV 0 ARG "${options}" "${args}" "${list_args}")
	get_filename_component(absolute_userguide_path ${ARG_USERGUIDE_PDF} ABSOLUTE)
	set_property(TARGET ${ARG_TARGET} PROPERTY UBERTON_USERGUIDE_PDF ${absolute_userguide_path})
	string(TOUPPER ${target} plugin_name_caps)
	target_compile_definitions(${ARG_TARGET} PRIVATE "${target}_USERGUIDE_PATH=\"${target}/${ARG_USERGUIDE_PDF}\"")
    smtg_add_plugin_resource(${ARG_TARGET} ${ARG_USERGUIDE_PDF} "Documentation") 
endfunction()


# Specify a factory presets folder for plugin project target. Factory presets are included in installer
function(uberton_set_factory_presets)
	set(args TARGET FACTORY_PRESETS)
    cmake_parse_arguments(PARSE_ARGV 0 ARG "${options}" "${args}" "${list_args}")
	get_filename_component(absolute_factory_presets_path ${ARG_FACTORY_PRESETS} ABSOLUTE)
	set_property(TARGET ${ARG_TARGET} PROPERTY UBERTON_FACTORY_PRESETS ${absolute_factory_presets_path})
endfunction()


# Set the version of a plugin project target. Optionally a build number can be specified. 
# - A plugin_version.h header is created in the build folder which contains all version ints/strings
#  as well as the plugin name etc to be included in the factory and resource file. 
# - The version is also applied to the installer if one is created for the target plugin
function(uberton_set_plugin_version)
	set(args TARGET VERSION BUILD)
    cmake_parse_arguments(PARSE_ARGV 0 ARG "${options}" "${args}" "${list_args}")

	# Set target plugin version
	set_property(TARGET ${ARG_TARGET} PROPERTY UBERTON_PLUGIN_VERSION ${ARG_VERSION})

	# Configure plugin_version.h file for factory and resources. 
	string(REPLACE "." ";" VERSION_LIST "${ARG_VERSION}")
	list(LENGTH VERSION_LIST len)
	if(${len} LESS 3)
		message(SEND_ERROR "Version needs to have 3 numbers separated by dots")
	endif()
	list(GET VERSION_LIST 0 MAJOR_VERSION)
	list(GET VERSION_LIST 1 MINOR_VERSION)
	list(GET VERSION_LIST 2 PATCH_NUMBER)
	if(${ARG_BUILD})
		set(BUILD_NUMBER ${ARG_BUILD})
	else()
		set(BUILD_NUMBER 1)
	endif()
	set(FULL_VERSION_STRING "${ARG_VERSION}.${BUILD_NUMBER}")

	set(PLUGIN_NAME ${ARG_TARGET})
	string(TOUPPER ${ARG_TARGET} PLUGIN_NAME_UPPER)
	configure_file("${UBERTON_SRC_PATH}/src/common/source/plugin_version.h.in" "plugin_version.h")
	configure_file("${UBERTON_SRC_PATH}/src/common/source/version_release.in" "version.txt")
	target_include_directories(${ARG_TARGET} PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
    smtg_add_plugin_resource(${ARG_TARGET} "${CMAKE_CURRENT_BINARY_DIR}/version.txt" "Documentation") 
endfunction()