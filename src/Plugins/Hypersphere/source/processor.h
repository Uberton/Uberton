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

#include "ids.h"
#include <ResonatorProcessor.h>

namespace Uberton {
namespace ResonatorPlugin {
namespace Hypersphere {


class Processor : public ResonatorProcessorBase
{
public:
	Processor();

	tresult PLUGIN_API setActive(TBool state) SMTG_OVERRIDE;


private:
	void updateResonatorDimension() override;
	void recomputeInexpensiveParameters() override;
};

template<class Resonator, typename SampleType, int numChannels = 2>
class SphereProcessorImpl : public ProcessorImpl<Resonator, SampleType, numChannels>
{
	void updateResonatorInputPosition(const ParamState& paramState) override {
		InputVecArr inputPositions;
		int d = static_cast<int>(inputPositions[0].size());
		for (int i = 0; i < d; i++) {
			inputPositions[0][i] = paramState[Params::kParamOutL0 + i];
			if constexpr (numChannels > 1)
				inputPositions[1][i] = paramState[Params::kParamOutR0 + i];
		}
		inputPositions[0] += inputPosSpaceCurveSphere(paramState[Params::kParamInPosCurveL]);
		if constexpr (numChannels > 1)
			inputPositions[1] += inputPosSpaceCurveSphere(paramState[Params::kParamInPosCurveR]);
		resonator.setInputPositions(inputPositions);
	}

	void updateResonatorOutputPosition(const ParamState& paramState) override {
		InputVecArr outputPositions;
		int d = static_cast<int>(outputPositions[0].size());
		for (int i = 0; i < d; i++) {
			outputPositions[0][i] = paramState[Params::kParamOutL0 + i];
			if constexpr (numChannels > 1)
				outputPositions[1][i] = paramState[Params::kParamOutR0 + i];
		}
		outputPositions[0] += outputPosSpaceCurveSphere(paramState[Params::kParamOutPosCurveL]);
		if constexpr (numChannels > 1)
			outputPositions[1] += outputPosSpaceCurveSphere(paramState[Params::kParamOutPosCurveR]);
		resonator.setOutputPositions(outputPositions);
	}

protected:
	// t in [0,1]; returns [1, π, π/2, π/2, π/2... ] for t = 0.5
	SpaceVec inputPosSpaceCurveSphere(ParamValue t) const {
		constexpr float pi = Math::pi<SampleType>();
		const SampleType r = (t * 2) * (t * 2);
		const SampleType phi = 2 * pi * t;
		const SampleType eps = 0.01;
		const SampleType theta = t * (pi - 2 * eps) + eps; // the theta angles may not be 0 or zero!
		// for t == 0.5 this function returns the 0 vector.
		return SpaceVec{
			r,	   // [0,4]
			phi,   // [0,2π]
			theta, // [ε,π-ε]
			theta, // [ε,π-ε]
			theta, // [ε,π-ε]
			theta, // [ε,π-ε]
			theta, // [ε,π-ε]
			theta, // [ε,π-ε]
			theta, // [ε,π-ε]
			theta  // [ε,π-ε]
		};
	}

	// t in [0,1]; returns [1, π, π/2, π/2, π/2... ] for t = 0.5
	SpaceVec outputPosSpaceCurveSphere(ParamValue t) const {
		return inputPosSpaceCurve(t);
	}

};

}
}
}