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

#include <resonator.h>
#include <public.sdk/samples/vst/note_expression_synth/source/filter.h>
#include "common_param_specs.h"


namespace Uberton {
namespace ResonatorPlugin {

class ProcessorImplBase
{
public:
	virtual void init(float sampleRate) = 0;
	virtual float processAll(ProcessData& data, float mix, float volume, bool limit) = 0;
	virtual void setResonatorDim(int resonatorDim) = 0;
	virtual void setResonatorOrder(int resonatorOrder) = 0;
	virtual void setResonatorFreq(float freq, float damp, float vel) = 0;
	virtual void setLCFilterFreqAndQ(double freq, double q) = 0;
	virtual void setHCFilterFreqAndQ(double freq, double q) = 0;
	virtual void updateResonatorInputPosition(const ParamState& paramState) = 0;
	virtual void updateResonatorOutputPosition(const ParamState& paramState) = 0;
	virtual ~ProcessorImplBase() {}
};

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
	using InputVecArr = std::array<SpaceVec, numChannels>;
	//using Resonator = Math::PreComputedCubeResonator<SampleType, maxDimension, maxOrder, numChannels>;
	using Filter = Steinberg::Vst::NoteExpressionSynth::Filter;


	static_assert(numChannels == Resonator::numChannels());
	static_assert(numChannels == Resonator::numChannels());

	void init(float sampleRate) override {
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
		}
	}

	void setResonatorOrder(int resonatorOrder) override {
		resonator.setOrder(resonatorOrder);
		currentResonatorOrder = resonatorOrder;
		updateCompensation();
	}

	virtual void updateCompensation() {
		compensation = 0.03f / std::sqrt(currentResonatorOrder);
	}

	void setResonatorFreq(float freq, float damp, float vel) override {
		if (freq != currentResFreq || damp != currentResDamp || vel != currentResVel) {
			resonator.setFreqDampeningAndVelocity(freq, damp, vel);
			currentResFreq = freq;
			currentResDamp = damp;
			currentResVel = vel;
		}
	}

	void setLCFilterFreqAndQ(double freq, double q) override {
		for (auto& filter : lcFilters) {
			filter.setFreqAndQ(freq, q);
		}
	}

	void setHCFilterFreqAndQ(double freq, double q) override {
		for (auto& filter : hcFilters) {
			filter.setFreqAndQ(freq, q);
		}
	}

	template<typename T>
	inline T tanh_approx(T x) {
		T sq = x * x;
		return x * (27 + sq) / (27 + 9 * sq);
	}

	// returns the max sample of the output buffer
	float processAll(ProcessData& data, float mix, float volume, bool limit) final {
		int32 numSamples = data.numSamples;

		SampleType** in = (SampleType**)data.inputs[0].channelBuffers32;
		SampleType** out = (SampleType**)data.outputs[0].channelBuffers32;


		float wet = mix;
		float dry = 1 - wet;
		std::array<SampleType, numChannels> input;
		SampleVec tmp;
		SampleType maxSampleLSq = 0;
		SampleType maxSampleRSq = 0;
		// higher resonator orders result in considerably higher volumes
		//float compensation = 1.f;//		/ std::sqrt(currentResonatorOrder);
		//compensation = 10;

		for (int32 i = 0; i < numSamples; i++) {
			for (int ch = 0; ch < numChannels; ch++) {
				input[ch] = *(in[ch] + i);
			}
			resonator.delta(input);
			tmp = resonator.next();
			for (int ch = 0; ch < numChannels; ch++) {
				tmp[ch] = lcFilters[ch].process(tmp[ch]);
				tmp[ch] = hcFilters[ch].process(tmp[ch]);
				tmp[ch] = volume * (tmp[ch] * wet * compensation + dry * (*(in[ch] + i)));
				if (limit) {
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
		}
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
		InputVecArr inputPositions;
		int d = static_cast<int>(inputPositions[0].size());
		for (int i = 0; i < d; i++) {
			inputPositions[0][i] = paramState[Params::kParamOutL0 + i];
			if constexpr (numChannels > 1)
				inputPositions[1][i] = paramState[Params::kParamOutR0 + i];
		}
		inputPositions[0] += inputPosSpaceCurve(paramState[Params::kParamInPosCurveL]);
		if constexpr (numChannels > 1)
			inputPositions[1] += inputPosSpaceCurve(paramState[Params::kParamInPosCurveR]);
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
		outputPositions[0] += outputPosSpaceCurve(paramState[Params::kParamOutPosCurveL]);
		if constexpr (numChannels > 1)
			outputPositions[1] += outputPosSpaceCurve(paramState[Params::kParamOutPosCurveR]);
		resonator.setOutputPositions(outputPositions);
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
			otherCoords,
			otherCoords,
			otherCoords,
			otherCoords,
			otherCoords,
			otherCoords,
			otherCoords
		};
	}

	// t in [0,1]; returns 0 vector for t = 0.5
	SpaceVec outputPosSpaceCurve(ParamValue t) const {
		return inputPosSpaceCurve(t);
	}


	Resonator resonator;
	std::array<Filter, numChannels> lcFilters{ Filter::Type::kHighpass, Filter::Type::kHighpass };
	std::array<Filter, numChannels> hcFilters{ Filter::Type::kLowpass, Filter::Type::kLowpass };

	SampleType currentResFreq = 1, currentResDamp = 1, currentResVel = 1;
	int currentResonatorOrder = 1;
	float compensation = 0.03f / std::sqrt(currentResonatorOrder);

	//SampleVec output
	SampleType vuPPMLSq{ 0 };
	SampleType vuPPMRSq{ 0 };
};

}
}
