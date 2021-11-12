
// Plugin ids and additional parameters for Tesseract
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

static const Steinberg::FUID ProcessorUID(0x81b3f1ba, 0x02024999, 0x837809e4, 0xa0be69d0);
static const Steinberg::FUID ControllerUID(0x10f2c668, 0x566d47e6, 0xbd8950d1, 0x9aed5c60);


FUnknown* createProcessorInstance(void*);
FUnknown* createControllerInstance(void*);
}

namespace ParamSpecs {
static const DiscreteParamSpec resonatorDim{ kParamResonatorDim, 1, maxDimension, 4, 3 };
static const DiscreteParamSpec resonatorOrder{ kParamResonatorOrder, 1, Tesseract::maxOrder, 5, 128 };

static const LinearParamSpec resonatorInputCoordinate{ -1, 0, 1, .5, .5 };
static const LinearParamSpec resonatorOutputCoordinate{ resonatorInputCoordinate };

}

}
}