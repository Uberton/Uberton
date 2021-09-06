
// Plugin ids and parameter specifications for common resonator based plugins
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
namespace ResonatorPlugin {

constexpr int maxDimension = 10;

constexpr double vuPPMOverheadDB = 2;
const double vuPPMNormalizedMultiplicator = std::pow(10, vuPPMOverheadDB / 20.0);
const double vuPPMNormalizedMultiplicatorInv = 1.0 / vuPPMNormalizedMultiplicator;

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

	kParamVUPPM_L,		   // OUT
	kParamVUPPM_R,		   // OUT
	kParamProcessTime,	   // OUT
	kParamResonatorLength, // OUT

	kParamLimiterOn,
	kNumGlobalParameters
};


namespace ParamSpecs {

// id, min, max, default, initial (all scaled not normalized!)
static const LogParamSpec vol{ kParamVol, 0.0, 1.0, 0.8, 0.8 };
static const LinearParamSpec mix{ kParamMix, 0.0, 100, 100, 100 };

static const DiscreteParamSpec resonatorType{ kParamResonatorType, 1, 2, 1, 1 };
//static const DiscreteParamSpec resonatorDim{ kParamResonatorDim, 1, maxDimension, 4, 3 };
//static const DiscreteParamSpec resonatorOrder{ kParamResonatorOrder, 1, maxOrder, 5, 128 };

static const LogParamSpec resonatorFreq{ kParamResonatorFreq, 20, 15000, 500, 200 };
static const LogParamSpec resonatorDamp{ kParamResonatorDamp, 0, 10, 1, 2.3 };
static const LinearParamSpec resonatorVel{ kParamResonatorVel, 0.1, 1000.0, 343, 343 };
static const LinearParamSpec resonatorLength{ kParamResonatorLength, 0, 100, 1, 1 }; // just for reading

static const LogParamSpec lcFreq{ kParamLCFreq, 20, 5000, 200, 650 };
static const LinearParamSpec lcQ{ kParamLCQ, 1, 8, 1, 1 };
static const LogParamSpec hcFreq{ kParamHCFreq, 100, 18000, 18000, 18000 };
static const LinearParamSpec hcQ{ kParamHCQ, 1, 8, 1, 1 };

static const LinearParamSpec inPosCurveL{ kParamInPosCurveL, -1.0, 1.0, 0.0, 0.0 };
static const LinearParamSpec inPosCurveR{ kParamInPosCurveR, -1.0, 1.0, 0.0, 0.0 };
static const LinearParamSpec outPosCurveL{ kParamOutPosCurveL, -1.0, 1.0, 0.0, 0.0 };
static const LinearParamSpec outPosCurveR{ kParamOutPosCurveR, -1.0, 1.0, 0.0, 0.0 };
static const ParamSpec linkInPosCurves{ kParamLinkInPosCurves, 0, 1, 0, 0 };
static const ParamSpec linkOutPosCurves{ kParamLinkOutPosCurves, 0, 1, 0, 0 };

static const LinearParamSpec vuPPML{ kParamVUPPM_L, 0.0, 1.0, 0.0, 0.0 };
static const LinearParamSpec vuPPMR{ kParamVUPPM_R, 0.0, 1.0, 0.0, 0.0 };
static const LinearParamSpec processTime{ kParamProcessTime, 0, 10, 0.0, 0.0 };

static const ParamSpec limiterOn{ kParamLimiterOn, 0, 1, 1, 1 };
}

using ParamState = UniformParamState<kNumGlobalParameters>;

static const Steinberg::FIDString processorDeactivatedMsgID = "pDeactivated";

}
}