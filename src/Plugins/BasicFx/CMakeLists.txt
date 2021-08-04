
set(BasicFx_sources
    source/controller.h
    source/ids.h
    source/processor.h
    source/version.h
    source/factory.cpp
    source/controller.cpp
    source/processor.cpp
    resource/editor.uidesc
)


set(target BasicFx)

smtg_add_vst3plugin(${target} ${BasicFx_sources})
set_target_properties(${target} PROPERTIES ${SDK_IDE_UBERTON_FOLDER})

target_link_libraries(${target} PRIVATE sdk)
target_link_libraries(${target} PRIVATE sdk vstgui_support)
target_link_libraries(${target} PRIVATE uberton_common)
target_include_directories(${target} PUBLIC "${SMTG_UBERTON_SRC_PATH}/src/common/source")

smtg_add_vst3_resource(${target} "resource/editor.uidesc")
smtg_add_vst3_resource(${target} "resource/menu_button.png")
smtg_add_vst3_resource(${target} "resource/undo_button.png")

if(SMTG_MAC)
    smtg_set_bundle(${target} INFOPLIST "${CMAKE_CURRENT_LIST_DIR}/resource/Info.plist" PREPROCESS)
elseif(SMTG_WIN)
    target_sources(${target} PRIVATE resource/plugin.rc)
endif()
