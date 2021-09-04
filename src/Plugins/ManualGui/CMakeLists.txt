
set(ManualGui_sources
    source/controller.h
    source/ids.h
    source/processor.h
    source/version.h
    source/factory.cpp
    source/controller.cpp
    source/processor.cpp
    source/gui.h
    source/gui.cpp
    resource/editor.uidesc
)


set(target ManualGui)

smtg_add_vst3plugin(${target} ${ManualGui_sources})
set_target_properties(${target} PROPERTIES ${UBERTON_FOLDER})

target_link_libraries(${target} PRIVATE sdk)
target_link_libraries(${target} PRIVATE sdk vstgui_support)
target_link_libraries(${target} PRIVATE uberton_common)
target_include_directories(${target} PUBLIC "${UBERTON_SRC_PATH}/src/common/source")

smtg_add_vst3_resource(${target} "resource/editor.uidesc")
smtg_add_vst3_resource(${target} "resource/background.png")
smtg_add_vst3_resource(${target} "resource/knobbg.png")


uberton_add_userguide_pdf(TARGET ${target} USERGUIDE_PDF "User_Guide/Manual.pdf")
uberton_add_factory_presets(TARGET ${target} FACTORY_PRESETS "Factory_Presets")

if(SMTG_MAC)
    smtg_set_bundle(${target} INFOPLIST "${CMAKE_CURRENT_LIST_DIR}/resource/Info.plist" PREPROCESS)
elseif(SMTG_WIN)
    target_sources(${target} PRIVATE resource/plugin.rc)
endif()

if(UBERTON_BUILD_INSTALLERS)
    uberton_add_plugin_installer(NAME ${target})
endif()
