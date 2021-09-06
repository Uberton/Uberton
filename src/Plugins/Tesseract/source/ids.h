
// Plugin ids (for processor, controller and parameters) for Tesseract
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

namespace Tesseract {
constexpr uint64 stateVersion = 0;
constexpr int maxOrder = 200;

static const Steinberg::FUID ProcessorUID(0x7aa35f53, 0x994c4f8c, 0x808ee8db, 0x5b254201);
static const Steinberg::FUID ControllerUID(0x59de5f5d, 0xc58c4195, 0x96c9bb42, 0xdb03afeb);


FUnknown* createProcessorInstance(void*);
FUnknown* createControllerInstance(void*);
}

namespace ParamSpecs {
static const DiscreteParamSpec resonatorDim{ kParamResonatorDim, 1, maxDimension, 4, 3 };
static const DiscreteParamSpec resonatorOrder{ kParamResonatorOrder, 1, Tesseract::maxOrder, 5, 128 };
}

}
}