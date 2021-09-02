define_property(TARGET PROPERTY UBERTON_USERGUIDE_PDF
	BRIEF_DOCS "Add user guide pdf to vst plugin target" 
	FULL_DOCS "Add user guide pdf to vst plugin target"
)

define_property(TARGET PROPERTY UBERTON_FACTORY_PRESETS
	BRIEF_DOCS "Add factory presets folder to plugin target" 
	FULL_DOCS "Add factory presets folder to plugin target"
)

function(uberton_add_userguide_pdf)
	set(args TARGET USERGUIDE_PDF)
    cmake_parse_arguments(PARSE_ARGV 0 ARG "${options}" "${args}" "${list_args}")
	get_filename_component(absolute_userguide_path ${ARG_USERGUIDE_PDF} ABSOLUTE)
	set_property(TARGET ${ARG_TARGET} PROPERTY UBERTON_USERGUIDE_PDF ${absolute_userguide_path})
endfunction()

function(uberton_add_factory_presets)
	set(args TARGET FACTORY_PRESETS)
    cmake_parse_arguments(PARSE_ARGV 0 ARG "${options}" "${args}" "${list_args}")
	get_filename_component(absolute_factory_presets_path ${ARG_FACTORY_PRESETS} ABSOLUTE)
	set_property(TARGET ${ARG_TARGET} PROPERTY UBERTON_FACTORY_PRESETS ${absolute_factory_presets_path})
endfunction()
