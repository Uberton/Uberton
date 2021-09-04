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
#include <oscillators.h>
#include <resonator.h>
#include "ids.h"


namespace Uberton {
namespace BasicInstrument {

class Processor : public ProcessorBase<ParamState, ImplementBypass>
{
public:
	Processor();

	tresult PLUGIN_API initialize(FUnknown* context) SMTG_OVERRIDE;
	tresult PLUGIN_API setBusArrangements(SpeakerArrangement* inputs, int32 numIns, SpeakerArrangement* outputs, int32 numOuts) SMTG_OVERRIDE;
	tresult PLUGIN_API setupProcessing(ProcessSetup& setup) SMTG_OVERRIDE;

	void processAudio(ProcessData& data) override;
	void processEvents(IEventList* eventList) override;
	void processParameterChanges(IParameterChanges* parameterChanges) override;

	static FUnknown* createInstance(void*) { return (Vst::IAudioProcessor*)new Processor(); }


protected:
	using ParamState1 = UniformParamState<1>;
	ParamState1 paramState;

	bool playing{ false };
	SineOscillatorPow5 osc;
	Math::CubeResonator<float, 1, 5, 1> resonator;
};

}
}