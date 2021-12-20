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

#include "ResonatorProcessorImplBase.h"
#include <processor_utilities.h>


namespace Uberton {
namespace ResonatorPlugin {

enum class Configuration {
	Mono,
	Stereo
};

template<class Resonator, typename SampleType, int numChannels = 2>
class ProcessorImpl : public ProcessorImplBase
{
public:
	using Type = SampleType;

	using SpaceVec = Math::Vector<SampleType, maxDimension>;
	using SampleVec = Math::Vector<SampleType, numChannels>;
	using PositionVecArr = std::array<SpaceVec, numChannels>;

	using Filter = Steinberg::Vst::NoteExpressionSynth::Filter;


	static_assert(numChannels == Resonator::numChannels());
	static_assert(numChannels == Resonator::numChannels());

	void init(float sampleRate) override {
		this->sampleRate = sampleRate;
		for (auto& filter : lcFilters) {
			filter.setSampleRate(sampleRate);
		}
		for (auto& filter : hcFilters) {
			filter.setSampleRate(sampleRate);
		}

		resonator.setSampleRate(sampleRate);
	}

	void setResonatorDim(int resonatorDim) override {
		if (resonatorDim != resonator.getDim()) {
			resonator.setDim(resonatorDim);
			resonator.setFreqDampeningAndVelocity(currentResFreq, currentResDamp, currentResVel); // need to update this when resonatorDim changed
			updateCompensation();
		}
	}

	void setResonatorOrder(int resonatorOrder) override {
		resonator.setOrder(resonatorOrder);
		currentResonatorOrder = resonatorOrder;
		updateCompensation();
	}

	void setResonatorFreq(float freq, float damp, float vel) override {
		if (freq != currentResFreq || damp != currentResDamp || vel != currentResVel) {
			resonator.setFreqDampeningAndVelocity(freq, damp, vel);
			currentResFreq = freq;
			currentResDamp = damp;
			currentResVel = vel;
		}
	}

	// Volume compensation for less extremes when changing resonator order or dimension
	virtual void updateCompensation() = 0;

	//void setLCFilterFreqAndQ(double freq, double q) override {
	//	for (auto& filter : lcFilters) filter.setFreqAndQ(freq, q);
	//}

	//void setHCFilterFreqAndQ(double freq, double q) override {
	//	for (auto& filter : hcFilters) filter.setFreqAndQ(freq, q);
	//}

	//template<typename T> T tanh_approx(T x) {
	//	T sq = x * x;
	//	return x * (27 + sq) / (27 + 9 * sq);
	//}

	// returns the max sample of the output buffer
	float processAll(ProcessData& data, const State& state) final {
		const int32 numSamples = data.numSamples;
		const bool limiterOn = state.limiterOn;

		SampleType** in = (SampleType**)data.inputs[0].channelBuffers32;
		SampleType** out = (SampleType**)data.outputs[0].channelBuffers32;


		// Ramping
		using ProcessorUtilities::getRamp;
		const SampleType rampTime_inv = SampleType{ 1 } / numSamples;
		const SampleType positionParamRamp = rampTime_inv * 8; // yes this is hardcoded and may not be changed

		const PositionVecArr inputDiff =
			(numChannels == 1) ? PositionVecArr{ (newInputPositions[0] - currentInputPositions[0]) * positionParamRamp }
							   : PositionVecArr{
									 (newInputPositions[0] - currentInputPositions[0]) * positionParamRamp,
									 (newInputPositions[1] - currentInputPositions[1]) * positionParamRamp
								 };
		const PositionVecArr outputDiff =
			(numChannels == 1) ? PositionVecArr{ (newOutputPositions[0] - currentOutputPositions[0]) * positionParamRamp }
							   : PositionVecArr{
									 (newOutputPositions[0] - currentOutputPositions[0]) * positionParamRamp,
									 (newOutputPositions[1] - currentOutputPositions[1]) * positionParamRamp
								 };

		SampleType volumeRamp = getRamp(currentVolume, SampleType(state.volume), rampTime_inv);
		SampleType wetRamp = getRamp(currentWet, SampleType(state.mix), rampTime_inv);
		SampleType lcRamp = getRamp(currentLCFreqNormalized, SampleType(state.lcFreqNormalized), rampTime_inv);
		SampleType hcRamp = getRamp(currentHCFreqNormalized, SampleType(state.hcFreqNormalized), rampTime_inv);

		// Temporaries
		std::array<SampleType, numChannels> input;
		SampleVec tmp;
		SampleType maxSampleLSq = 0;
		SampleType maxSampleRSq = 0;

		for (int32 i = 0; i < numSamples; i++) {
			const SampleType dry = 1. - currentWet;

			if ((i & 0b00000111) == 0) { // divisible by 8
				// updating every sample is too slow and produces audio cracks
				// we only update the positions every few samples
				if (inCurveChanged) {
					currentInputPositions[0] += inputDiff[0];
					if constexpr (numChannels > 1) {
						currentInputPositions[1] += inputDiff[1];
					}
					resonator.setInputPositions(currentInputPositions);
				}
				if (outCurveChanged) {
					currentOutputPositions[0] += outputDiff[0];
					if constexpr (numChannels > 1) {
						currentOutputPositions[1] += outputDiff[1];
					}
					resonator.setOutputPositions(currentOutputPositions);
				}
			}
			for (int ch = 0; ch < numChannels; ch++) {
				input[ch] = *(in[ch] + i);
			}
			resonator.delta(input);
			tmp = resonator.next();
			for (int ch = 0; ch < numChannels; ch++) {
				tmp[ch] = lcFilters[ch].process(tmp[ch]);
				tmp[ch] = hcFilters[ch].process(tmp[ch]);
				tmp[ch] = currentVolume * (tmp[ch] * currentWet * volumeCompensation + dry * (*(in[ch] + i)));
				if (limiterOn) {
					tmp[ch] = std::tanh(tmp[ch]);
					// the tanh approximation is a few times faster but already for higher than the lowest few
					// resonator orders the actual processing takes much more time than the limiting.
					// And the approximation is softer / can exceed 1.
					//tmp[ch] = tanh_approx(tmp[ch]);
				}
				*(out[ch] + i) = tmp[ch];
			}

			maxSampleLSq = std::max(maxSampleLSq, tmp[0] * tmp[0]);
			if constexpr (numChannels > 1) {
				maxSampleRSq = std::max(maxSampleRSq, tmp[1] * tmp[1]);
			}

			currentVolume += volumeRamp;
			currentWet += wetRamp;

			if (lcRamp) {
				currentLCFreqNormalized += lcRamp;
				double freq = ParamSpecs::lcFreq.toScaled(currentLCFreqNormalized);
				for (auto& filter : lcFilters) {
					filter.setFreqAndQ(freq, state.lcQ);
				}
			}
			if (hcRamp) {
				currentHCFreqNormalized += hcRamp;
				double freq = ParamSpecs::hcFreq.toScaled(currentHCFreqNormalized);
				for (auto& filter : hcFilters) {
					filter.setFreqAndQ(freq, state.hcQ);
				}
			}
		}
		if (inCurveChanged) {
			inCurveChanged = false;
			currentInputPositions = newInputPositions;
			resonator.setInputPositions(currentInputPositions);
		}
		if (outCurveChanged) {
			outCurveChanged = false;
			currentOutputPositions = newOutputPositions;
			resonator.setOutputPositions(currentOutputPositions);
		}
		currentVolume = state.volume;
		currentWet = state.mix;
		currentLCFreqNormalized = state.lcFreqNormalized;
		currentHCFreqNormalized = state.hcFreqNormalized;

		if (vuPPMLSq != maxSampleLSq || vuPPMRSq != maxSampleRSq) {
			addOutputPoint(data, kParamVUPPM_L, std::sqrt(maxSampleLSq) * vuPPMNormalizedMultiplicatorInv);
			addOutputPoint(data, kParamVUPPM_R, std::sqrt(maxSampleRSq) * vuPPMNormalizedMultiplicatorInv);
			vuPPMLSq = maxSampleLSq;
			vuPPMRSq = maxSampleRSq;
		}
		if constexpr (numChannels == 1) {
			return maxSampleLSq;
		} else {
			return std::max(maxSampleLSq, maxSampleRSq);
		}
	}

	static void addOutputPoint(ProcessData& data, ParamID id, ParamValue value) {
		if (data.outputParameterChanges) {
			int32 index;
			IParamValueQueue* queue = data.outputParameterChanges->addParameterData(id, index);
			if (queue) {
				queue->addPoint(0, value, index);
			}
		}
	}

	void updateResonatorInputPosition(const ParamState& paramState) override {
		inCurveChanged = true;
		for (int i = 0; i < maxDimension; i++) {
			newInputPositions[0][i] = paramState[Params::kParamInL0 + i];
			if constexpr (numChannels > 1) {
				newInputPositions[1][i] = paramState[Params::kParamInR0 + i];
			}
		}
		newInputPositions[0] += inputPosSpaceCurve(paramState[Params::kParamInPosCurveL]);
		if constexpr (numChannels > 1) {
			newInputPositions[1] += inputPosSpaceCurve(paramState[Params::kParamInPosCurveR]);
		}
	}

	void updateResonatorOutputPosition(const ParamState& paramState) override {
		outCurveChanged = true;
		for (int i = 0; i < maxDimension; i++) {
			newOutputPositions[0][i] = paramState[Params::kParamOutL0 + i];
			if constexpr (numChannels > 1) {
				newOutputPositions[1][i] = paramState[Params::kParamOutR0 + i];
			}
		}
		newOutputPositions[0] += outputPosSpaceCurve(paramState[Params::kParamOutPosCurveL]);
		if constexpr (numChannels > 1) {
			newOutputPositions[1] += outputPosSpaceCurve(paramState[Params::kParamOutPosCurveR]);
		}
	}



protected:
	// t in [0,1]; returns 0 vector for t = 0.5
	SpaceVec inputPosSpaceCurve(ParamValue t) const {
		constexpr SampleType pi = Math::pi<SampleType>();
		const SampleType t_ = t - .5;
		const SampleType phi = t_ * 1.5 * pi;

		const SampleType otherCoords = t_ * 0.5;
		// for t == 0.5 this function returns the 0 vector.
		return SpaceVec{
			otherCoords,
			phi / (2 * pi) * std::sinf(phi),
			phi / (2 * pi) * std::cosf(phi + pi * 0.5f),
			otherCoords, otherCoords, otherCoords, otherCoords,
			otherCoords, otherCoords, otherCoords
		};
	}

	// t in [0,1]; returns 0 vector for t = 0.5
	SpaceVec outputPosSpaceCurve(ParamValue t) const {
		return inputPosSpaceCurve(t);
	}


	Resonator resonator;
	std::array<Filter, numChannels> lcFilters{ Filter::Type::kHighpass, Filter::Type::kHighpass };
	std::array<Filter, numChannels> hcFilters{ Filter::Type::kLowpass, Filter::Type::kLowpass };

	double sampleRate{ 0 };

	bool inCurveChanged{ true };
	bool outCurveChanged{ true };

	// Input positions as currently played and new positions which differ when parameters
	// (curve or manual position) are changed.
	PositionVecArr currentInputPositions;
	PositionVecArr newInputPositions;
	PositionVecArr currentOutputPositions;
	PositionVecArr newOutputPositions;

	// Output values (L/R)
	SampleType vuPPMLSq{ 0 };
	SampleType vuPPMRSq{ 0 };

	// Current values (as played)
	SampleType currentVolume = 0;
	SampleType currentWet = 1;
	SampleType currentLCFreqNormalized = 0;
	SampleType currentHCFreqNormalized = 1;
	SampleType currentResFreq = 1, currentResDamp = 1, currentResVel = 1;
	int currentResonatorOrder = 1;

	// Volume compensation for higher volumes with higher resonator orders or lower dimensions
	SampleType volumeCompensation = 0.03f / std::sqrt(currentResonatorOrder);
};



}
}
