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

#include <ProcessorBase.h>
#include "ids.h"
#include "processorImpl.h"

namespace Uberton {
namespace TesseractFx {


class Processor : public ProcessorBase<ParamState, ImplementBypass>
{
public:
	Processor();

	tresult PLUGIN_API initialize(FUnknown* context) SMTG_OVERRIDE;
	tresult PLUGIN_API setActive(TBool state) SMTG_OVERRIDE;
	tresult PLUGIN_API setBusArrangements(SpeakerArrangement* inputs, int32 numIns, SpeakerArrangement* outputs, int32 numOuts) SMTG_OVERRIDE;
	tresult PLUGIN_API canProcessSampleSize(int32 symbolicSampleSize) SMTG_OVERRIDE;

	void processAudio(ProcessData& data) override;
	void processParameterChanges(IParameterChanges* parameterChanges) override;

	//static FUnknown* createInstance(void*) { return (Vst::IAudioProcessor*)new Processor(); }

private:
	// Inexpensive parameter udpates
	void recomputeInexpensiveParameters();

	// Expensive parameter udpates
	void updateResonatorDimension();

	// Update all parameters (expensive and inexpensive)
	void recomputeParameters() override;



	std::unique_ptr<ProcessorImplBase> processorImpl;

	float volume{ 0 };
	float mix{ 1 };
	int resonatorDim = maxDimension;
	int resonatorOrder = maxOrder;
	float resonatorFreq{ 0 };
	float resonatorDamp{ 0 };
	float resonatorVel{ 0 };
	bool limiterOn{ false };

	double vuPPM = 0;
	double vuPPMOld = 0;
};

}
}