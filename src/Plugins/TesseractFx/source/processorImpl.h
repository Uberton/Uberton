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
#include "filter.h"


namespace Uberton {


class ProcessorImplBase
{
public:
	virtual void init(float sampleRate) = 0;
};


template<typename SampleType>
class ProcessorImpl : public ProcessorImplBase
{
public:
	constexpr static int dim = 3;
	constexpr static int order = 10;
	constexpr static int numChannels = 2;
	using Type = SampleType;
	using Vec = Math::Vector<SampleType, dim>;
	using SampleVec = Math::Vector<SampleType, numChannels>;


	void init(float sampleRate) override {
		// resonator.setSampleRate(sampleRate);
		f1.setCutoff(.2);
		f2.setCutoff(.2);

		resonator.setSampleRate(sampleRate);
		resonator.setInputPositions({ Vec{ .3, .5, .2 }, Vec{ .4, .5, .6 } });
		resonator.setOutputPositions({ Vec{ .6, .5, .2 }, Vec{ .4, .5, .6 } });
	}

	void setFreq(SampleType f, SampleType b, SampleType c) {
		if (f != f_ || b != b_ || c != c_) {
			resonator.setFreqDampeningAndVelocity(f, b, c);
			f_ = f;
			c_ = c;
			b_ = b;
		}
	}

	void setCutoff(double value) {
		f1.setCutoff(value);
		f2.setCutoff(value);
	}
	
	SampleVec process(SampleType l, SampleType r) {
		resonator.delta({ l, r });
		SampleVec tmp = resonator.next();

		//tmp[0] = f1.process(tmp[0]);
		//tmp[1] = f2.process(tmp[1]);
		return { static_cast<float>(f1.process(tmp[0])), static_cast<float>(f2.process(tmp[1])) };
	}
	//void setVelocity(std::complex<SampleType> vel) {
	//	resonator.setVelocity_sq(vel);
	//}

	Filter f1{ Filter::Mode::Highpass };
	Filter f2{ Filter::Mode::Highpass };
	LogScale<ParamValue> freqLogScale{ 0., 1., 80., 18000., 0.5, 1800. };
	SampleType f_ = 1, b_ = 1, c_ = 1;

	//Math::CubeEigenvalueProblem<SampleType, dim, order, numChannels> resonator;


	using Resonator = Math::CubeResonator<SampleType, dim, 10, 2>;
	Resonator resonator;
	//using VecArray = std::array<Resonator::Vec, numChannels>;
};


}
