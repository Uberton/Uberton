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

// Param state (mostly scaled parameters).
struct State
{
	double volume{ 0 };
	double mix{ 1 };
	int resonatorDim = maxDimension;
	int resonatorOrder = 1;
	double resonatorFreq{ 0 };
	double resonatorDamp{ 0 };
	double resonatorVel{ 0 };
	double lcFreqNormalized{ 0 };
	double hcFreqNormalized{ 0 };
	double lcQ{ 1 };
	double hcQ{ 1 };
	bool limiterOn{ false };
};

class ProcessorImplBase
{
public:
	virtual void init(float sampleRate) = 0;
	virtual float processAll(ProcessData& data, const State& state) = 0;
	virtual void setResonatorDim(int resonatorDim) = 0;
	virtual void setResonatorOrder(int resonatorOrder) = 0;
	virtual void setResonatorFreq(float freq, float damp, float vel) = 0;
	//virtual void setLCFilterFreqAndQ(double freq, double q) = 0;
	//virtual void setHCFilterFreqAndQ(double freq, double q) = 0;
	virtual void updateResonatorInputPosition(const ParamState& paramState) = 0;
	virtual void updateResonatorOutputPosition(const ParamState& paramState) = 0;
	virtual ~ProcessorImplBase() = default;
};

}
}
