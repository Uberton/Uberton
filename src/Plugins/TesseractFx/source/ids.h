
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
constexpr int maxOrder = 200;


enum Params : Steinberg::Vst::ParamID {
	kParamVol,
	kParamMix,

	kParamResonatorType,
	kParamResonatorDim,
	kParamResonatorOrder,
	kParamResonatorFreq,
	kParamResonatorDamp,
	kParamResonatorVel,

	kParamLCFreq,
	kParamLCQ,
	kParamHCFreq,
	kParamHCQ,

	kParamInL0,
	kParamInLN = kParamInL0 + maxDimension - 1,
	kParamInR0,
	kParamInRN = kParamInR0 + maxDimension - 1,
	kParamOutL0,
	kParamOutLN = kParamOutL0 + maxDimension - 1,
	kParamOutR0,
	kParamOutRN = kParamOutR0 + maxDimension - 1,

	kParamInPosCurveL,
	kParamInPosCurveR,
	kParamOutPosCurveL,
	kParamOutPosCurveR,
	kParamLinkInPosCurves,
	kParamLinkOutPosCurves,

	kParamVUPPM, // OUT
	kNumGlobalParameters
};

namespace ParamSpecs {

// id, min, max, default, initial (all not normalized!)
constexpr ParamSpec vol{ kParamVol, { 0.0, 1.0, 0.8 }, 0.8 };
constexpr ParamSpec mix{ kParamMix, { 0.0, 1.0, 1.0 }, 1.0 };
constexpr ParamSpec resonatorType{ kParamResonatorType, { 1, 2, 1 }, 1 };
constexpr ParamSpec resonatorDim{ kParamResonatorDim, { 1, maxDimension, 5 }, maxDimension };
constexpr ParamSpec resonatorOrder{ kParamResonatorOrder, { 1, maxOrder, 5 }, 1 };
constexpr ParamSpec resonatorFreq{ kParamResonatorFreq, { 100, 2000, 500 }, 200 };
constexpr ParamSpec resonatorDamp{ kParamResonatorDamp, { 0, 10, 1 }, 1 };
constexpr ParamSpec resonatorVel{ kParamResonatorVel, { 0.1, 1000.0, 10.0 }, 10.0 };

constexpr ParamSpec lcFreq{ kParamLCFreq, { 20, 10000, 20 }, 20 };
constexpr ParamSpec lcQ{ kParamLCQ, { 1, 8, 1 }, 10 };
constexpr ParamSpec hcFreq{ kParamHCFreq, { 100, 20000, 20000 }, 20000 };
constexpr ParamSpec hcQ{ kParamHCQ, { 1, 8, 1 }, 10 };

constexpr ParamSpec inPosCurveL{ kParamInPosCurveL, { 0.0, 1.0, 0.5 }, 0.5 };
constexpr ParamSpec inPosCurveR{ kParamInPosCurveR, { 0.0, 1.0, 0.5 }, 0.5 };
constexpr ParamSpec outPosCurveL{ kParamOutPosCurveL, { 0.0, 1.0, 0.5 }, 0.5 };
constexpr ParamSpec outPosCurveR{ kParamOutPosCurveR, { 0.0, 1.0, 0.5 }, 0.5 };
constexpr ParamSpec linkInPosCurves{ kParamLinkInPosCurves, { 0, 1, 0 }, 0 };
constexpr ParamSpec linkOutPosCurves{ kParamLinkOutPosCurves, { 0, 1, 0 }, 0 };


constexpr ParamSpec vuPPM{ kParamVUPPM, { 0.0, 1.0, 0.0 }, 0.0 };

}

using ParamState = UniformParamState<kNumGlobalParameters>;

static const Steinberg::FUID ProcessorUID(0x81b3f1ba, 0x02024999, 0x837809e4, 0xa0be69d0);
static const Steinberg::FUID ControllerUID(0x10f2c668, 0x566d47e6, 0xbd8950d1, 0x9aed5c60);

}
}