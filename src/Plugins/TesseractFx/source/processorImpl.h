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

#include <public.sdk/samples/vst/common/logscale.h>
#include <resonator.h>
#include <filter.h>
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
	//using Resonator = Math::CubeResonator<SampleType, dim, order, numChannels>;
	using Resonator = Math::PreComputedCubeResonator<SampleType, maxDimension, maxOrder, numChannels>;


	void init(float sampleRate) override {
		for (auto& filter : filters) {
			filter.setCutoff(.2);
			filter.setMode(Filter::Mode::Highpass);
		}

		resonator.setSampleRate(sampleRate);
	}

	void setResonatorDim(int dim) {
		if (dim != resonator.getDim()) {
			resonator.setDim(dim);
			resonator.setFreqDampeningAndVelocity(currentResFreq, currentResDamp, currentResVel); // need to update this when dim changed
		}
	}

	void setResonatorOrder(int order) {
		resonator.setOrder(order);
	}

	void setResonatorFreq(SampleType freq, SampleType damp, SampleType vel) {
		if (freq != currentResFreq || damp != currentResDamp || vel != currentResVel) {
			resonator.setFreqDampeningAndVelocity(freq, damp, vel);
			currentResFreq = freq;
			currentResDamp = damp;
			currentResVel = vel;
		}
	}

	void setFilterCutoff(double value) {
		for (auto& filter : filters) {
			filter.setCutoff(value);
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


	Resonator resonator;
	std::array<Filter, numChannels> filters;
	LogScale<ParamValue> freqLogScale{ 0., 1., 80., 18000., 0.5, 1800. };

	//Filter f1{ Filter::Mode::Highpass };
	//Filter f2{ Filter::Mode::Highpass };

	SampleType currentResFreq = 1, currentResDamp = 1, currentResVel = 1;
};

}
}
