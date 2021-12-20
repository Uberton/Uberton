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
#include <ResonatorProcessorImpl.h>

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
	void updateCompensation() override {
		volumeCompensation = 1.0 / resonator.getDim();
	}

	void updateResonatorInputPosition(const ParamState& paramState) override {
		PositionVecArr inputPositions;
		auto d = inputPositions[0].size();

		auto resultVec = [&](ParamID firstId, SpaceVec& output) {
			constexpr auto pi = Math::pi<SampleType>();
			output[0] = ParamSpecs::resonatorInputRCoordinate.toScaled(paramState[firstId]);
			output[1] = ParamSpecs::resonatorInputPhiCoordinate.toScaled(paramState[firstId + 1]) * pi;
			for (size_t i = 2; i < d; i++) {
				output[i] = ParamSpecs::resonatorInputThetaCoordinate.toScaled(paramState[firstId + i]) * pi;
			}
		};

		resultVec(Params::kParamInL0, inputPositions[0]);
		if constexpr (numChannels > 1)
			resultVec(Params::kParamInR0, inputPositions[1]);

		//for (int i = 0; i < d; i++) {
		//	inputPositions[0][i] = paramState[Params::kParamOutL0 + i];
		//	if constexpr (numChannels > 1)
		//		inputPositions[1][i] = paramState[Params::kParamOutR0 + i];
		//}
		//inputPositions[0] += inputPosSpaceCurveSphere(paramState[Params::kParamInPosCurveL]);
		//if constexpr (numChannels > 1)
		//	inputPositions[1] += inputPosSpaceCurveSphere(paramState[Params::kParamInPosCurveR]);

		//clamping
		constexpr SampleType eps = 1e-5;
		constexpr SampleType piMinusEps = Math::pi<SampleType>() - eps;
		for (size_t i = 2; i < d; i++) {
			inputPositions[0][i] = std::max(eps, std::min(piMinusEps, inputPositions[0][i]));
			if constexpr (numChannels > 1)
				inputPositions[1][i] = std::max(eps, std::min(piMinusEps, inputPositions[1][i]));
		}
		resonator.setInputPositions(inputPositions);
	}

	void updateResonatorOutputPosition(const ParamState& paramState) override {
		PositionVecArr outputPositions;
		auto d = outputPositions[0].size();

		auto resultVec = [&](ParamID firstId, SpaceVec& output) {
			constexpr auto pi = Math::pi<SampleType>();
			output[0] = ParamSpecs::resonatorInputRCoordinate.toScaled(paramState[firstId]);
			output[1] = ParamSpecs::resonatorInputPhiCoordinate.toScaled(paramState[firstId + 1]) * pi;
			for (size_t i = 2; i < d; i++) {
				output[i] = ParamSpecs::resonatorInputThetaCoordinate.toScaled(paramState[firstId + i]) * pi;
			}
		};

		resultVec(Params::kParamOutL0, outputPositions[0]);
		if constexpr (numChannels > 1)
			resultVec(Params::kParamOutR0, outputPositions[1]);

		//for (int i = 0; i < d; i++) {
		//	outputPositions[0][i] = paramState[Params::kParamOutL0 + i];
		//	if constexpr (numChannels > 1)
		//		outputPositions[1][i] = paramState[Params::kParamOutR0 + i];
		//}
		//outputPositions[0] += outputPosSpaceCurveSphere(paramState[Params::kParamOutPosCurveL]);
		//if constexpr (numChannels > 1)
		//	outputPositions[1] += outputPosSpaceCurveSphere(paramState[Params::kParamOutPosCurveR]);

		//clamping
		constexpr SampleType eps = 1e-5;
		constexpr SampleType piMinusEps = Math::pi<SampleType>() - eps;
		for (size_t i = 2; i < d; i++) {
			outputPositions[0][i] = std::max(eps, std::min(piMinusEps, outputPositions[0][i]));
			if constexpr (numChannels > 1)
				outputPositions[1][i] = std::max(eps, std::min(piMinusEps, outputPositions[1][i]));
		}
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
		return SpaceVec(0);
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