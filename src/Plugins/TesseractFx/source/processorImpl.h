
// Acutal processing implementation for the plugin processor that does the heavy audio work. 
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

#include <resonator.h>
//#include <filter.h>
#include <public.sdk/samples/vst/note_expression_synth/source/filter.h>
#include "ids.h"


namespace Uberton {
namespace TesseractFx {

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
	virtual double getResonatorLength() const = 0;
	virtual ~ProcessorImplBase() {}
};


template<typename SampleType>
class ProcessorImpl : public ProcessorImplBase
{
public:
	constexpr static int maxOrder = maxOrder;
	constexpr static int numChannels = 2;
	using Type = SampleType;
	using SpaceVec = Math::Vector<SampleType, maxDimension>;
	using SampleVec = Math::Vector<SampleType, numChannels>;
	using Resonator = Math::PreComputedCubeResonator<SampleType, maxDimension, maxOrder, numChannels>;
	using Filter = Steinberg::Vst::NoteExpressionSynth::Filter;


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
		static_assert(numChannels >= 2);
		// higher resonator orders result in considerably higher volumes
		float volumeCompensation = 0.03f / std::sqrt(currentResonatorOrder);

		for (int32 i = 0; i < numSamples; i++) {
			for (int ch = 0; ch < numChannels; ch++) {
				input[ch] = *(in[ch] + i);
			}
			resonator.delta(input);
			tmp = resonator.next();
			for (int ch = 0; ch < numChannels; ch++) {
				tmp[ch] = lcFilters[ch].process(tmp[ch]); 
				tmp[ch] = hcFilters[ch].process(tmp[ch]);
				tmp[ch] = volume * (tmp[ch] * wet * volumeCompensation + dry * (*(in[ch] + i)));
				if (limit) {
					tmp[ch] = std::tanh(tmp[ch]);
					// the tanh approximation is a few times faster but already for higher than the lowest few
					// resonator orders the actual processing takes much more time than the limiting. 
					// And the approximation is softer / can exceed 1. 
					//tmp[ch] = tanh_approx(tmp[ch]); 
				}
				*(out[ch] + i) = tmp[ch];
			}

			//float k = 0.5 * (tmp[0] + tmp[1]);
			//maxSample = std::max(maxSample, k * k);
			maxSampleLSq = std::max(maxSampleLSq, tmp[0] * tmp[0]);
			maxSampleRSq = std::max(maxSampleRSq, tmp[1] * tmp[1]);
		}
		if (vuPPMLSq != maxSampleLSq || vuPPMRSq != maxSampleRSq) {
			Processor::addOutputPoint(data, kParamVUPPM_L, std::sqrt(maxSampleLSq) * vuPPMNormalizedMultiplicatorInv);
			Processor::addOutputPoint(data, kParamVUPPM_R, std::sqrt(maxSampleRSq) * vuPPMNormalizedMultiplicatorInv);
			vuPPMLSq = maxSampleLSq;
			vuPPMRSq = maxSampleRSq;
		}
		return std::max(maxSampleLSq, maxSampleRSq);
	}

	//SampleVec output
	SampleType vuPPMLSq{ 0 };
	SampleType vuPPMRSq{ 0 };

	void updateResonatorInputPosition(const ParamState& paramState) override {
		SpaceVec inputPosL, inputPosR;
		int d = static_cast<int>(inputPosL.size());
		for (int i = 0; i < d; i++) {
			inputPosL[i] = paramState[Params::kParamInL0 + i];
			inputPosR[i] = paramState[Params::kParamInR0 + i];
		}
		inputPosL += inputPosSpaceCurve(paramState[Params::kParamInPosCurveL]);
		inputPosR += inputPosSpaceCurve(paramState[Params::kParamInPosCurveR]);
		resonator.setInputPositions({ inputPosL, inputPosR });
	}

	void updateResonatorOutputPosition(const ParamState& paramState) override {
		SpaceVec outputPosL, outputPosR;
		int d = static_cast<int>(outputPosL.size());
		for (int i = 0; i < d; i++) {
			outputPosL[i] = paramState[Params::kParamOutL0 + i];
			outputPosR[i] = paramState[Params::kParamOutR0 + i];
		}
		outputPosL += outputPosSpaceCurve(paramState[Params::kParamOutPosCurveL]);
		outputPosR += outputPosSpaceCurve(paramState[Params::kParamOutPosCurveR]);
		resonator.setOutputPositions({ outputPosL, outputPosR });
	}


	double getResonatorLength() const override {
		return resonator.getLength();
	}

protected:
	// t in [0,1]; returns 0 vector for t = 0.5
	SpaceVec inputPosSpaceCurve(ParamValue t) {
		constexpr float pi = Math::pi<float>();
		const float t_ = t - .5;
		const float phi = t_ * 1.5 * pi;
		// for t == 0.5 this function returns the 0 vector.
		return SpaceVec{
			t_ * 0.5f,
			phi / (2 * pi) * std::sinf(phi),
			phi / (2 * pi) * std::cosf(phi + pi * 0.5f),
			t_ * 0.5f,
			t_ * 0.5f,
			t_ * 0.5f,
			t_ * 0.5f,
			t_ * 0.5f,
			t_ * 0.5f,
			t_ * 0.5f
		};
	}

	// t in [0,1]; returns 0 vector for t = 0.5
	SpaceVec outputPosSpaceCurve(ParamValue t) {
		constexpr float pi = Math::pi<float>();
		const float t_ = t - .5;
		const float phi = t_ * 1.5 * pi;
		// for t == 0.5 this function returns the 0 vector.
		return SpaceVec{
			t_ * 0.5f,
			phi / (2 * pi) * std::sinf(phi),
			phi / (2 * pi) * std::cosf(phi + pi * 0.5f),
			t_ * 0.5f,
			t_ * 0.5f,
			t_ * 0.5f,
			t_ * 0.5f,
			t_ * 0.5f,
			t_ * 0.5f,
			t_ * 0.5f
		};
	}
	//void updateFilter(double freq, double q) {
	//	lcFreq.set(freq);
	//	lcQ.set(q);
	//}

	//RampedParameter<float> lcFreq{ 0, 1 };
	//RampedParameter<float> lcQ{ 0, 1 };
private:
	Resonator resonator;
	std::array<Filter, numChannels> lcFilters{ Filter::Type::kHighpass, Filter::Type::kHighpass };
	std::array<Filter, numChannels> hcFilters{ Filter::Type::kLowpass, Filter::Type::kLowpass };

	SampleType currentResFreq = 1, currentResDamp = 1, currentResVel = 1;
	int currentResonatorOrder = 1;
};

}
}
