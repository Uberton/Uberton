
set(TesseractFx_sources
    source/controller.h
    source/ids.h
    source/processor.h
    source/version.h
    source/factory.cpp
    source/controller.cpp
    source/processor.cpp
    source/processorImpl.h
    resource/editor.uidesc
)


set(target TesseractFx)

smtg_add_vst3plugin(${target} ${TesseractFx_sources})
set_target_properties(${target} PROPERTIES ${UBERTON_FOLDER})

target_link_libraries(${target} PRIVATE uberton_common)
target_include_directories(${target} PUBLIC "${UBERTON_SRC_PATH}/src/common/source")

smtg_add_vst3_resource(${target} "resource/editor.uidesc")
smtg_add_vst3_resource(${target} "resource/background.png")
smtg_add_vst3_resource(${target} "resource/about.png")
smtg_add_vst3_resource(${target} "resource/menu_button.png")
smtg_add_vst3_resource(${target} "resource/undo_button.png")
smtg_add_vst3_resource(${target} "resource/vumeter_on_L.png")
smtg_add_vst3_resource(${target} "resource/vumeter_on_R.png")
smtg_add_vst3_resource(${target} "resource/vumeter_off.png")
smtg_add_vst3_resource(${target} "resource/link_button.png")
smtg_add_vst3_resource(${target} "resource/slider_handle.png")
smtg_add_vst3_resource(${target} "resource/slider_background.png")
smtg_add_vst3_resource(${target} "resource/Knob10.png")
smtg_add_vst3_resource(${target} "resource/Knob61.png")
smtg_add_vst3_resource(${target} "resource/Knob91.png")
smtg_add_vst3_resource(${target} "resource/ncube_projections.png")
smtg_add_vst3_resource(${target} "resource/led.png")

smtg_add_vst3_snapshot(${target} "resource/81B3f1BA02024999837809E4A0BE69D0_snapshot.png")
smtg_add_vst3_snapshot(${target} "resource/81B3f1BA02024999837809E4A0BE69D0_snapshot_2.0x.png")

if(SMTG_MAC)
    smtg_set_bundle(${target} INFOPLIST "${CMAKE_CURRENT_LIST_DIR}/resource/Info.plist" PREPROCESS)
elseif(SMTG_WIN)
    target_sources(${target} PRIVATE resource/plugin.rc)
endif()


if(UBERTON_BUILD_INSTALLERS)
    uberton_set_userguide_pdf(TARGET ${target} USERGUIDE_PDF "User_Guide/Tesseract User Guide.pdf")
    uberton_set_factory_presets(TARGET ${target} FACTORY_PRESETS "Factory_Presets")
    uberton_set_plugin_installer(NAME ${target} VERSION "1.0.1")
endif()