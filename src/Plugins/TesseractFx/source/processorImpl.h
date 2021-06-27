// -----------------------------------------------------------------------------------------------------------------------------
// This file is part of the �berton project. Copyright (C) 2021 �berton
//
// �berton is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// �berton is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
// of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details. You should
// have received a copy of the GNU General Public License along with �berton. If not, see http://www.gnu.org/licenses/.
// -----------------------------------------------------------------------------------------------------------------------------


#pragma once

#include <public.sdk/samples/vst/common/logscale.h>
#include <resonator.h>
//#include <filter.h>
#include <../../Plugins/Synth1/source/filter.h>
#include "ids.h"


namespace Uberton {
namespace TesseractFx {

class ProcessorImplBase
{
public:
	virtual void init(float sampleRate) = 0;
};


template<typename SampleType>
class ProcessorImpl : public ProcessorImplBase
{
public:
	constexpr static int maxOrder = maxOrder;
	constexpr static int numChannels = 2;
	using Type = SampleType;
	using Vec = Math::Vector<SampleType, maxDimension>;
	using SampleVec = Math::Vector<SampleType, numChannels>;
	//using Resonator = Math::CubeResonator<SampleType, resonatorDim, resonatorOrder, numChannels>;
	using Resonator = Math::PreComputedCubeResonator<SampleType, maxDimension, maxOrder, numChannels>;
	using Filter = Steinberg::Vst::NoteExpressionSynth::Filter;


	void init(float sampleRate) override {
		for (auto& filter : filters) {
			//filter.setCutoff(.2);
			//filter.setMode(Filter::Mode::Highpass);
			filter.setSampleRate(sampleRate);
			filter.setFreqAndQ(220, 10);
			filter.setType(Filter::Type::kHighpass);
		}

		resonator.setSampleRate(sampleRate);
	}

	void setResonatorDim(int resonatorDim) {
		if (resonatorDim != resonator.getDim()) {
			resonator.setDim(resonatorDim);
			resonator.setFreqDampeningAndVelocity(currentResFreq, currentResDamp, currentResVel); // need to update this when resonatorDim changed
		}
	}

	void setResonatorOrder(int resonatorOrder) {
		resonator.setOrder(resonatorOrder);
	}

	void setResonatorFreq(SampleType freq, SampleType damp, SampleType vel) {
		if (freq != currentResFreq || damp != currentResDamp || vel != currentResVel) {
			resonator.setFreqDampeningAndVelocity(freq, damp, vel);
			currentResFreq = freq;
			currentResDamp = damp;
			currentResVel = vel;
		}
	}

	void setFilterCutoff(double freq, double q) {
		for (auto& filter : filters) {
			filter.setFreqAndQ(freq, q);
			//filter.setCutoff(value);
		}
	}

	SampleVec process(SampleType l, SampleType r) {
		resonator.delta({ l, r });
		SampleVec tmp = resonator.next();
		for (int i = 0; i < numChannels; i++) {
			tmp[i] = filters[i].process(tmp[i]);
		}
		return tmp;
		//return { static_cast<SampleType>(f1.process(tmp[0])), static_cast<SampleType>(f2.process(tmp[1])) };
	}

	void processAll(ProcessData& data, float mix, float volume) {
		int32 numSamples = data.numSamples;
		
		SampleType** in = (SampleType**)data.inputs[0].channelBuffers32;
		SampleType** out = (SampleType**)data.outputs[0].channelBuffers32;


		SampleType* sInL;
		SampleType* sInR;
		float wet = mix;
		float dry = 1 - wet;
		std::array<SampleType, numChannels> input;
		SampleVec tmp;

		for (int32 i = 0; i < numSamples; i++) {
			//sInL = in[0] + i;
			//sInR = in[1] + i;

			//resonator.delta({ *sInL, *sInR });

			for (int ch = 0; ch < numChannels; ch++) {
				input[ch] = *(in[ch] + i);
			}
			resonator.delta(input);
			tmp = resonator.next();
			for (int ch = 0; ch < numChannels; ch++) {
				tmp[ch] = filters[ch].process(tmp[ch]) * .01; // its tooooo loud!!
				*(out[ch] + i) = volume * tmp[ch] * wet + dry * (*(in[ch] + i));
			}

			//*(out[0] + i) = volume * tmp[0] * wet + dry * (*sInL);
			//*(out[1] + i) = volume * tmp[1] * wet + dry * (*sInR);
		}
	}


	Resonator resonator;
	std::array<Filter, numChannels> filters{ Filter::Type::kHighpass, Filter::Type::kHighpass };
	LogScale<ParamValue> freqLogScale{ 0., 1., 80., 18000., 0.5, 1800. };

	//Filter f1{ Filter::Mode::Highpass };
	//Filter f2{ Filter::Mode::Highpass };

	SampleType currentResFreq = 1, currentResDamp = 1, currentResVel = 1;
};

}
}
