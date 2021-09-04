
// Plugin ids (for processor, controller and parameters) for BasicInstrument
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
namespace BasicInstrument {


enum Params : Steinberg::Vst::ParamID {
	kParamVol,
	kNumGlobalParameters,
};

namespace ParamSpecs {
static const LinearParamSpec vol{ kParamVol, 0, 1, 0.8, 0.8 };
}
using ParamState = UniformParamState<kNumGlobalParameters>;

static const Steinberg::FUID ProcessorUID(0xed4cc4ff, 0x6a0846c5, 0xb07de226, 0xb01098c4);
static const Steinberg::FUID ControllerUID(0x624ad3a6, 0x20894bc1, 0xb6de1482, 0xc1cf0900);

}
}