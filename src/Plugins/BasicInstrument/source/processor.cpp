// Plugin name and version of BasicInstrument
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


#include "processor.h"
#include "ids.h"

#include <public.sdk/source/vst/vstaudioprocessoralgo.h>
#include <vstmath.h>

namespace Uberton {
namespace BasicInstrument {

Processor::Processor() {
	setControllerClass(ControllerUID);
	paramState[Params::kParamVolId] = .8;
}

tresult PLUGIN_API Processor::initialize(FUnknown* context) {
	tresult result = AudioEffect::initialize(context);
	if (result != kResultTrue)
		return kResultFalse;

	addEventInput(STR16("Event Input"), 1);
	addAudioOutput(STR16("Audio Output"), SpeakerArr::kStereo);

	return kResultTrue;
}

tresult PLUGIN_API Processor::setupProcessing(ProcessSetup& setup) {
	osc.setSampleRate(setup.sampleRate);
	resonator.setSampleRate(setup.sampleRate);
	resonator.setInputPositions({ .3 });
	resonator.setOutputPositions({ .7 });
	return ProcessorBase::setupProcessing(setup);
}

void Processor::processAudio(ProcessData& data) {
	int32 numChannels = data.outputs[0].numChannels;
	int32 numSamples = data.numSamples;

	Sample32** out = data.outputs[0].channelBuffers32;
	float volume = paramState.params[kParamVolId];

	if (playing) {
		for (int32 i = 0; i < numSamples; i++) {
			float sample = osc.get();
			sample = resonator.next()[0];

			for (int32 channel = 0; channel < numChannels; channel++) {
				out[channel][i] = sample * volume;
			}
		}
	}
	else {
		Algo::clear32(data.outputs, numSamples);
	}
}

void Processor::processParameterChanges(IParameterChanges* inputParameterChanges) {
	Algo::foreach (inputParameterChanges, [&](IParamValueQueue& paramQueue) {
		Algo::foreachLast(paramQueue, [&](int32 id, int32 sampleOffset, ParamValue value) {
			if (id == bypassId) {
				setBypassed(value > 0.5);
				paramState.bypass = value > 0.5f;
			}
			else
				paramState.params[id] = value;
		}

		);
	});
}

void Processor::processEvents(IEventList* eventList) {
	Algo::foreach (eventList, [&](const Event& event) {
		switch (event.type) {
		case Event::kNoteOnEvent:
			playing = true;
			osc.setPhase(0.0);
			osc.setFrequency(Math::frequencyTable[event.noteOn.pitch]);
			resonator.setFreqDampeningAndVelocity(Math::frequencyTable[event.noteOn.pitch], .1, 10);
			resonator.delta({ .5 });
			break;
		case Event::kNoteOffEvent:
			//playing = false;
			
			break;
		case Event::kNoteExpressionValueEvent:
			break;
		}
	});
}

tresult PLUGIN_API Processor::setBusArrangements(SpeakerArrangement* inputs, int32 numIns, SpeakerArrangement* outputs, int32 numOuts) {
	// one stereo output bus
	if (numIns == 0 && numOuts == 1 && outputs[0] == SpeakerArr::kStereo) {
		return AudioEffect::setBusArrangements(inputs, numIns, outputs, numOuts);
	}
	return kResultFalse;
}

/*tresult PLUGIN_API Processor::setState(IBStream* state) {
	return paramState.setState(state);
}

tresult PLUGIN_API Processor::getState(IBStream* state) {
	return paramState.getState(state);
}*/

} // namespace BasicInstrument
} // namespace Uberton
