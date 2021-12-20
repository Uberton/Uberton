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
#include "common_param_specs.h"
#include "ResonatorProcessorImplBase.h"

namespace Uberton {
namespace ResonatorPlugin {


class ResonatorProcessorBase : public ProcessorBase<ParamState, ImplementBypass>
{
public:
	ResonatorProcessorBase();

	tresult PLUGIN_API initialize(FUnknown* context) SMTG_OVERRIDE;
	tresult PLUGIN_API setBusArrangements(SpeakerArrangement* inputs, int32 numIns, SpeakerArrangement* outputs, int32 numOuts) SMTG_OVERRIDE;
	tresult PLUGIN_API canProcessSampleSize(int32 symbolicSampleSize) SMTG_OVERRIDE;

	void processAudio(ProcessData& data) override;
	void processParameterChanges(IParameterChanges* parameterChanges) override;
	void beforeBypass(ProcessData& data) override;


protected:
	// Inexpensive parameter udpates
	virtual void recomputeInexpensiveParameters();

	// Expensive parameter udpates
	virtual void updateResonatorDimension();

	// Update all parameters (expensive and inexpensive)
	void recomputeParameters() override;



	std::unique_ptr<ProcessorImplBase> processorImpl;

	State state; // Scaled parameters stored here

	double vuPPM = 0; // contains max of left and right channel from last buffer to check if there is silence
};

}
}