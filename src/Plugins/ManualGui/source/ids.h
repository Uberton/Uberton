
// Plugin ids (for processor, controller and parameters) for ManualGui
//
// -----------------------------------------------------------------------------------------------------------------------------
// This file is part of the Überton project. Copyright (C) 2021 Überton
//
// Überton is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// Überton is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
// of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details. You should
// have received a copy of the GNU General Public License along with Überton. If not, see http://www.gnu.org/licenses/.
// -----------------------------------------------------------------------------------------------------------------------------


#pragma once
#include <pluginterfaces/vst/vsttypes.h>
#include <parameters.h>

namespace Uberton {
namespace ManualGui {


enum Params : Steinberg::Vst::ParamID {
	kParamVolId,
	kParamSlider1,
	kParamSlider2,
	kParamLink,
	kNumGlobalParameters
};

using ParamState = UniformParamState<kNumGlobalParameters>;

static const Steinberg::FUID ProcessorUID(0x7dacc5d7, 0x715d4863, 0x85b997d6, 0x7efed6b4);
static const Steinberg::FUID ControllerUID(0x848e66f5, 0xc0254b8c, 0xabfcb0f2, 0x06b2940c);

}
}