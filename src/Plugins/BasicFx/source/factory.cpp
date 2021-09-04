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
#include <viewcreator_globals.h>

#include "ids.h"
#include "version.h"
#include "controller.h"
#include "processor.h"

#define stringSubCategory "Fx" // Subcategory for this plug-in (to be changed if needed, see PlugType in ivstaudioprocessor.h)

BEGIN_FACTORY_DEF(stringCompanyName, stringCompanyWeb, stringCompanyEmail)

DEF_CLASS2(INLINE_UID_FROM_FUID(Uberton::BasicFx::ProcessorUID),
	PClassInfo::kManyInstances,					  // cardinality
	kVstAudioEffectClass,						  // the component category (do not changed this)
	stringPluginName,							  // here the plug-in name (to be changed)
	Vst::kDistributable,						  // means that component and controller could be distributed on different computers
	stringSubCategory,							  // Subcategory for this plug-in (to be changed)
	FULL_VERSION_STR,							  // Plug-in version (to be changed)
	kVstVersionString,							  // the VST 3 SDK version (do not changed this, use always this define)
	Uberton::BasicFx::Processor::createInstance);	  // function pointer called when this component should be instantiated

DEF_CLASS2(INLINE_UID_FROM_FUID(Uberton::BasicFx::ControllerUID),
	PClassInfo::kManyInstances,					  // cardinality
	kVstComponentControllerClass,					  // the Controller category (do not changed this)
	stringPluginName "Controller",				  // controller name (could be the same than component name)
	0,										  // not used here
	"",										  // not used here
	FULL_VERSION_STR,							  // Plug-in version (to be changed)
	kVstVersionString,							  // the VST 3 SDK version (do not changed this, use always this define)
	Uberton::BasicFx::Controller::createInstance);	  // function pointer called when this component should be instantiated

END_FACTORY;


#if VST_VERSION <= 0x030701 // VST_VERSION <= VST_3_7_1_VERSION
bool InitModule() { return true; }
bool DeinitModule() { return true; }
#endif