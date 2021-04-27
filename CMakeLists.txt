set(SDK_IDE_UBERTON_FOLDER FOLDER "Uberton")
add_subdirectory("${SMTG_UBERTON_SRC_PATH}/src/common" "${SMTG_UBERTON_SRC_PATH}/build/common")


set_target_properties(uberton_common PROPERTIES ${SDK_IDE_UBERTON_FOLDER})

add_subdirectory(src/Plugins)