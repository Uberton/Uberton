// -----------------------------------------------------------------------------------------------------------------------------
// This file is part of the Überton project. Copyright (C) 2021 Überton
//
// Überton is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// Überton is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
// of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details. You should
// have received a copy of the GNU General Public License along with Überton. If not, see http://www.gnu.org/licenses/.
// -----------------------------------------------------------------------------------------------------------------------------

#include <public.sdk/source/main/pluginfactory.h>
#include <pluginterfaces/vst/ivstaudioprocessor.h>
#include <viewcreator_globals.h>

#include "ids.h"
#include <plugin_version.h>

#define stringSubCategory "Fx|Multidimensional" // Subcategory for this plug-in (to be changed if needed, see PlugType in ivstaudioprocessor.h)

BEGIN_FACTORY_DEF(stringCompanyName, stringCompanyWeb, stringCompanyEmail)

DEF_CLASS2(INLINE_UID_FROM_FUID(Uberton::ResonatorPlugin::Tesseract::ProcessorUID), PClassInfo::kManyInstances, kVstAudioEffectClass,
	TESSERACT_stringPluginName, Vst::kDistributable, stringSubCategory, TESSERACT_FULL_VERSION_STR, kVstVersionString,
	Uberton::ResonatorPlugin::Tesseract::createProcessorInstance);

DEF_CLASS2(INLINE_UID_FROM_FUID(Uberton::ResonatorPlugin::Tesseract::ControllerUID), PClassInfo::kManyInstances, kVstComponentControllerClass,
	TESSERACT_stringPluginName "Controller", 0, "", TESSERACT_FULL_VERSION_STR, kVstVersionString,
	Uberton::ResonatorPlugin::Tesseract::createControllerInstance);

END_FACTORY;


#if VST_VERSION <= 0x030701 // VST_VERSION <= VST_3_7_1_VERSION
bool InitModule() { return true; }
bool DeinitModule() { return true; }
#endif