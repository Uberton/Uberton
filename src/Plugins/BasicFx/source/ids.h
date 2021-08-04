
// Plugin ids (for processor, controller and parameters) for BasicFx
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
namespace BasicFx {


enum Params : Steinberg::Vst::ParamID {
	kParamVol,
	kNumGlobalParameters
};

namespace ParamSpecs {

// id, min, max, default, initial (all scaled not normalized!)
constexpr ParamSpec vol{ kParamVol, { 0, 100, 80 }, 80 };

}

using ParamState = UniformParamState<kNumGlobalParameters>;

static const Steinberg::FUID ProcessorUID(0xBD58B550, 0xF9E5634E, 0x9D2EFF39, 0xEA0927B1);
static const Steinberg::FUID ControllerUID(0xA0B1A6F4, 0x005D9B47, 0x967177E3, 0x7A671891);

}
}