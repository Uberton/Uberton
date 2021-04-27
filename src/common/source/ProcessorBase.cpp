// -----------------------------------------------------------------------------------------------------------------------------
// This file is part of the Überton project. Copyright (C) 2021 Überton
//
// Überton is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// Überton is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
// of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details. You should
// have received a copy of the GNU General Public License along with Überton. If not, see http://www.gnu.org/licenses/.
// -----------------------------------------------------------------------------------------------------------------------------


#include "ProcessorBase.h"
#include <public.sdk/source/vst/vstaudioprocessoralgo.h>
#include <cmath>

namespace Uberton {

ProcessorBase::ProcessorBase() {}

tresult PLUGIN_API ProcessorBase::process(ProcessData& data) {
	processParameterChanges(data.inputParameterChanges);
	processEvents(data.inputEvents);

	if (data.numSamples > 0 && !bypassProcessing(data)) {
		processAudio(data);
		checkSilence(data);
	}
	return kResultTrue;
}

bool ProcessorBase::bypassProcessing(ProcessData& data) {
	if (data.numSamples == 0) return true;

	if (bypassingState != BypassingState::None) {
		// Bypass ramping (only first bus)
		processAudio(data);

		float dry = 0;
		float wet = 0;
		float f = 1.0f / data.numSamples;

		for (int channel = 0; channel < data.numInputs; channel++) {
			Sample32* in = data.inputs[0].channelBuffers32[channel];
			Sample32* out = data.outputs[0].channelBuffers32[channel];
			if (in == out) continue;

			if (bypassingState == BypassingState::RampToOff) {
				for (int i = 0; i < data.numSamples; i++) {
					dry = i * f;
					wet = (data.numSamples - i) * f;
					out[i] = wet * out[i] + dry * in[i];
				}
			}
			else { // bypassingState == BypassingState::RampToOn
				for (int i = 0; i < data.numSamples; i++) {
					dry = (data.numSamples - i) * f;
					wet = i * f;
					out[i] = wet * out[i] + dry * in[i];
				}
			}
		}
		data.outputs[0].silenceFlags = 0;
		bypassingState = BypassingState::None;
		return true;
	}
	else if (isBypassed()) {

		// Bypass (first in/out bus pair is copied, all other output busses are cleared)
		AudioBusBuffers& inBus = data.inputs[0];
		AudioBusBuffers& outBus = data.outputs[0];
		Algo::copy32(&inBus, &outBus, data.numSamples, 0);

		for (int32 bus = 1; bus < data.numOutputs; bus++) {
			Algo::clear32(&data.outputs[bus], data.numSamples);
		}

		// Implement true bypass (any number of busses)
		/*for (int32 bus = 0; bus < data.numInputs && bus < data.numOutputs; bus++) {
			Bus* outputBus = getAudioOutput(bus);
			if (!outputBus || !outputBus->isActive()) {
				continue;
			}
			//if(data.inputs[bus] != data.outputs[bus])
			Algo::copy32(&data.inputs[bus], &data.outputs[bus], data.numSamples, 0);

			
			for (int32 channel = 0; channel < data.outputs[bus].numChannels; ++channel) {
				Sample32* in = data.inputs[bus].channelBuffers32[channel];
				Sample32* out = data.outputs[bus].channelBuffers32[channel];
				if (in != out) {
					memcpy(out, in, sizeof(float) * data.numSamples);
				}
				data.outputs[bus].silenceFlags |= data.inputs[bus].silenceFlags & ((uint64)1 << channel);
			}
		}*/
		return true;
	}
	return false;
}

void ProcessorBase::checkSilence(ProcessData& data) {
	for (int32 i = 0; i < data.numOutputs; i++) {
		auto& bus = data.outputs[i];
		bus.silenceFlags = 0;
		if (!getAudioOutput(i)->isActive()) continue;
		for (int32 ch = 0; ch < bus.numChannels; ch++) {
			bool isSilent = true;
			for (int32 sample = 0; sample < data.numSamples; sample += 20) {
				if (std::abs(bus.channelBuffers32[ch][sample]) > 0.0001) {
					isSilent = false;
					break;
				}
			}
			if (isSilent) {
				bus.silenceFlags |= (uint64)1 << ch;
			}
		}
	}
}

void ProcessorBase::processParameterChanges(IParameterChanges* parameterChanges) {}

void ProcessorBase::setBypassed(bool bypass) {
	if (bypass != this->bypass) {
		this->bypass = bypass;
		if (bypass)
			bypassingState = BypassingState::RampToOff;
		else
			bypassingState = BypassingState::RampToOn;
	}
}

tresult PLUGIN_API ProcessorBase::setActive(TBool state) {
	return kResultOk;
}

tresult PLUGIN_API ProcessorBase::setBusArrangements(SpeakerArrangement* inputs, int32 numIns, SpeakerArrangement* outputs, int32 numOuts) {
	if (numIns == 1 && numOuts == 1 && inputs[0] == outputs[0]) {
		return AudioEffect::setBusArrangements(inputs, numIns, outputs, numOuts);
	}
	return kResultOk;
}

tresult PLUGIN_API ProcessorBase::setState(IBStream* state) {
	return kResultOk;
}

tresult PLUGIN_API ProcessorBase::getState(IBStream* state) {
	return kResultOk;
}

}