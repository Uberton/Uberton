
// Plugin ids and additional parameters for Hypersphere
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
#include <common_param_specs.h>

namespace Uberton {
namespace ResonatorPlugin {

namespace Hypersphere {
constexpr uint64 stateVersion = 0;
constexpr int maxOrder = 200;
static const Steinberg::FUID ProcessorUID(0xF6EB58AE, 0x1C944641, 0xA50C3B89, 0x33ED9E29);
static const Steinberg::FUID ControllerUID(0x32735D45, 0x89DC48F7, 0xB93B4756, 0x71743B0A);

FUnknown* createProcessorInstance(void*);
FUnknown* createControllerInstance(void*);
}

namespace ParamSpecs {
static const DiscreteParamSpec resonatorDim{ kParamResonatorDim, 2, maxDimension, 4, 3 };
static const DiscreteParamSpec resonatorOrder{ kParamResonatorOrder, 1, Hypersphere::maxOrder, 5, 128 };
}

}
}