
// Processor classes that enhance the implementation of the class AudioEffect
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


#pragma once

#include <public.sdk/source/vst/vstaudioeffect.h>
#include <public.sdk/source/vst/vsteditcontroller.h>
#include <public.sdk/source/vst/vstaudioprocessoralgo.h>
#include "parameters.h"


namespace Uberton {
using namespace Steinberg;
using namespace Steinberg::Vst;

/*
 * Base Processor
 *
 * - Only one bus (with any number of channels) and equal input / output numbers are allowed.
 * - Implements bypass ramping(ramp over one sample buffer)
 */
class ProcessorBaseA : public AudioEffect
{
public:
	ProcessorBaseA();

	tresult PLUGIN_API process(ProcessData& data) SMTG_OVERRIDE;
	tresult PLUGIN_API setActive(TBool state) SMTG_OVERRIDE;
	tresult PLUGIN_API setBusArrangements(SpeakerArrangement* inputs, int32 numIns, SpeakerArrangement* outputs, int32 numOuts) SMTG_OVERRIDE;

	tresult PLUGIN_API setState(IBStream* state) SMTG_OVERRIDE;
	tresult PLUGIN_API getState(IBStream* state) SMTG_OVERRIDE;

	virtual void processAudio(ProcessData& data) = 0;
	virtual void processParameterChanges(IParameterChanges* parameterChanges) = 0;
	virtual void processEvents(IEventList* eventList) {}
	bool bypassProcessing(ProcessData& data);
	void checkSilence(ProcessData& data);

	bool isBypassed() const { return bypass; }
	void setBypassed(bool bypass);

private:
	bool bypass{ false };

	enum class BypassingState {
		None,
		RampToOff,
		RampToOn
	};
	BypassingState bypassingState{ BypassingState::None };
};




/*
 * Processor base class that uses some kind of global parameter state with specifications
 * as described in parameters.h.
 * The plugins setState/getState functions uses the corresponding functions of ParamState.
 * A ramped bypass can be implemented but is only advised for Fx plugins with one bus and
 * as many inputs as outputs. The ParamState class has to provide the actual bypass parameter
 * state.
 */
template<class ParamState, bool hasBypass = ImplementBypass>
// requires requires (ParamState p, IBStream* stream, EditController& controller, bool b, int i) {
//	{ p.getState(stream) } -> std::convertible_to<tresult>;
//	{ p.setState(stream) } -> std::convertible_to<tresult>;
//	{ p.setComponentState(stream, controller) } -> std::convertible_to<tresult>;
//	{ p[i] } -> std::convertible_to<double>;
//	{ p.isBypassed() } -> std::convertible_to<bool>;
//	p.setBypass(b);
// }
class ProcessorBase;



/*
 * Parent for this Processor Base class - contains common parts for ProcessorBase with and without bypass 
 */
template<class ParamState>
class ProcessorBaseCommon : public AudioEffect
{
public:
	tresult PLUGIN_API getState(IBStream* state) SMTG_OVERRIDE { return paramState.getState(state); }

	virtual void processAudio(ProcessData& data) = 0;
	virtual void processParameterChanges(IParameterChanges* parameterChanges) = 0;
	virtual void processEvents(IEventList* eventList) {}

	void checkSilence(ProcessData& data) {
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

	ParamValue toScaled(const ParamSpec& p) {
		return p.toScaled(paramState[p.id]);
	}

	int toDiscrete(const ParamSpec& p) {
		return p.toDiscrete(paramState[p.id]);
	}

protected:
	ParamState paramState;
};




//
// Implementation with bypass
// --------------------------
// Additional reqirements for ParamState (ParamState needs to store bypass state):
//
// requires requires (ParamState p, bool b) {
//	{ p.isBypassed() } -> std::convertible_to<bool>;
//	p.setBypass(b);
// }
//
template<class ParamState>
class ProcessorBase<ParamState, ImplementBypass> : public ProcessorBaseCommon<ParamState>
{
public:
	tresult PLUGIN_API process(ProcessData& data) SMTG_OVERRIDE {
		processParameterChanges(data.inputParameterChanges);
		processEvents(data.inputEvents);

		if (data.numSamples > 0 && !bypassProcessing(data)) {
			processAudio(data);
			checkSilence(data);
		}
		return kResultTrue;
	}

	tresult PLUGIN_API setState(IBStream* state) SMTG_OVERRIDE {
		tresult r = paramState.setState(state);
		recomputeParameters();
		return r;
	}

	bool bypassProcessing(ProcessData& data) {
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
			return true;
		}
		return false;
	}


	bool isBypassed() const {
		return paramState.isBypassed();
	}

	void setBypassed(bool state) {
		if (state == paramState.isBypassed()) return;
		paramState.setBypass(state);
		if (state)
			bypassingState = BypassingState::RampToOff;
		else
			bypassingState = BypassingState::RampToOn;
	}

	virtual void recomputeParameters() {}

private:
	enum class BypassingState {
		None,
		RampToOff,
		RampToOn
	};
	BypassingState bypassingState{ BypassingState::None };
};


//
// Implementation without bypass
//
template<class ParamState>
class ProcessorBase<ParamState, NoBypass> : public ProcessorBaseCommon<ParamState>
{
public:
	tresult PLUGIN_API process(ProcessData& data) SMTG_OVERRIDE {
		processParameterChanges(data.inputParameterChanges);
		processEvents(data.inputEvents);

		if (data.numSamples > 0) {
			processAudio(data);
			checkSilence(data);
		}
		return kResultTrue;
	}

	tresult PLUGIN_API setState(IBStream* state) SMTG_OVERRIDE {
		tresult r = paramState.setState(state);
		recomputeParameters();
		return r;
	}
};


} // namespace Uberton