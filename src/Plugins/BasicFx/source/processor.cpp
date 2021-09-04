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
#include <public.sdk/source/vst/vstaudioprocessoralgo.h>

namespace Uberton {
namespace BasicFx {

Processor::Processor() {
	setControllerClass(ControllerUID);

	paramState.version = 0;

	auto initValue = [&](const auto& p) {
		paramState[p.id] = p.toNormalized(p.initialValue);
	};

	initValue(ParamSpecs::vol);
}

tresult PLUGIN_API Processor::initialize(FUnknown* context) {
	//---always initialize the parent-------
	tresult result = AudioEffect::initialize(context);
	if (result != kResultTrue)
		return kResultFalse;

	//---create Audio In/Out busses------
	// we want a stereo Input and a Stereo Output
	addAudioInput(STR16("AudioInput"), SpeakerArr::kStereo);
	addAudioOutput(STR16("AudioOutput"), SpeakerArr::kStereo);

	return kResultTrue;
}

void Processor::processAudio(ProcessData& data) {
	//int32 numChannels = data.inputs[0].numChannels;
	int32 numSamples = data.numSamples;

	//Sample32** in = data.inputs[0].channelBuffers32;
	//Sample32** out = data.outputs[0].channelBuffers32;

	float volume = paramState[Params::kParamVol];

	Algo::multiply32(data.inputs[0], data.outputs[0], numSamples, volume);
}

void Processor::processParameterChanges(IParameterChanges* inputParameterChanges) {
	Algo::foreach (inputParameterChanges, [&](IParamValueQueue& paramQueue) {
		// Just process the latest parameter change and apply it immediately (ignoring sampleOffset)
		// For sample-accurate automation this needs to be more precise.
		Algo::foreachLast(paramQueue, [&](int32 id, int32 sampleOffset, ParamValue value) {
			if (id == bypassId) {
				setBypassed(value > 0.5);
			}
			else
				paramState.params[id] = value;
		}

		);
	});
	//if (inputParameterChanges) {
	//	int32 numParamsChanged = inputParameterChanges->getParameterCount();
	//	for (int32 index = 0; index < numParamsChanged; index++) {
	//		IParamValueQueue* paramQueue = inputParameterChanges->getParameterData(index);

	//		if (paramQueue) {
	//			ParamValue value;
	//			int32 sampleOffset;
	//			int32 numPoints = paramQueue->getPointCount();

	//			// Just process the latest parameter change and apply it immediately (ignoring sampleOffset)
	//			// For sample-accurate automation this needs to be more precise.
	//			int32 id = paramQueue->getParameterId();
	//			if (paramQueue->getPoint(numPoints - 1, sampleOffset, value) == kResultTrue) {
	//				if (id == bypassId) {
	//					setBypassed(value > 0.5);
	//				}
	//				else {
	//					paramState.params[id] = value;
	//				}
	//			}
	//		}
	//	}
	//}
}

} // namespace BasicFx
} // namespace Uberton
