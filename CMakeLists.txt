
set(UBERTON_FOLDER FOLDER "Uberton")
set(UBERTON_INSTALLER_FOLDER FOLDER "Uberton/Installers")
set(UBERTON_INSTALLER_RESOURCE_FOLDER FOLDER "Uberton/Installers/Resource_Projects")

option(UBERTON_BUILD_INSTALLERS OFF)

get_filename_component(ABSOLUTE_INSTALLER_PATH "./src/installer" ABSOLUTE)
include(cmake/Properties.cmake)

add_subdirectory(src/common)

if(UBERTON_BUILD_INSTALLERS)
	add_subdirectory(src/installer)
endif()

add_subdirectory(src/Plugins)