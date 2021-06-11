
// Plugin ids (for processor, controller and parameters) for TesseractFx
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
namespace TesseractFx {


constexpr int maxDimension = 10;
constexpr int maxOrder = 10;

enum Params : Steinberg::Vst::ParamID {
	kParamVol,
	kParamMix,

	kParamResonatorType,
	kParamResonatorDim,
	kParamResonatorOrder,
	kParamResonatorFreq,
	kParamResonatorDamp,
	kParamResonatorVel,

	kParamFilterCutoff,

	kParamIn0,
	kParamXL = kParamIn0 + maxDimension - 1,
	kParamOut0,
	kParamYL = kParamOut0 + maxDimension - 1,

	kParamVUPPM, // OUT
	kNumGlobalParameters
};

using ParamState = UniformParamState<kNumGlobalParameters>;

static const Steinberg::FUID ProcessorUID(0x81b3f1ba, 0x02024999, 0x837809e4, 0xa0be69d0);
static const Steinberg::FUID ControllerUID(0x10f2c668, 0x566d47e6, 0xbd8950d1, 0x9aed5c60);

}
}