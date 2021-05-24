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

	void processAudio(ProcessData& data) override;
	void processParameterChanges(IParameterChanges* parameterChanges) override;

	static FUnknown* createInstance(void*) { return (Vst::IAudioProcessor*)new Processor(); }

private:
	std::unique_ptr<ProcessorImpl<float>> processorImpl;
	double vuPPM = 0;
	double vuPPMOld = 0;

	void recomputeParameters() override;
	void recomputeInexpensiveParameters();
	void updateResonatorInputPosition();
	void updateResonatorOutputPosition();
	void addOutputPoint(ProcessData& data, ParamID id, ParamValue value);

	float gain{ 0 };
	float dampening{ 0 };
	float sonicVel{ 0 };
	float resFreq{ 0 };
	float wet{ 1 };

	bool needToUpdatePositions{ false };


	using Vec = ProcessorImpl<float>::Vec;
};

}
}