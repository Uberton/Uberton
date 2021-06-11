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
namespace TesseractFx {

Processor::Processor() {
	setControllerClass(ControllerUID);
	paramState[Params::kParamVol] = .8;
	paramState[Params::kParamMix] = .8;
	paramState[Params::kParamResonatorDim] = 1;
	paramState[Params::kParamResonatorOrder] = 1;
	paramState[Params::kParamResonatorFreq] = .5;
	paramState[Params::kParamResonatorDamp] = .1;
	paramState[Params::kParamResonatorVel] = 5;

	for (int i = 0; i < maxDimension; i++) {
		paramState[Params::kParamIn0 + i] = .5;
		paramState[Params::kParamOut0 + i] = .5;
	}
}

tresult PLUGIN_API Processor::initialize(FUnknown* context) {
	tresult result = ProcessorBase::initialize(context);
	if (result != kResultTrue)
		return kResultFalse;

	addAudioInput(STR16("AudioInput"), SpeakerArr::kStereo);
	addAudioOutput(STR16("AudioOutput"), SpeakerArr::kStereo);

	return kResultTrue;
}

tresult PLUGIN_API Processor::setActive(TBool state) {
	if (state) {
		processorImpl = std::make_unique<ProcessorImpl<float>>();
		processorImpl->init(processSetup.sampleRate);
		recomputeParameters();

		/*if (processSetup.symbolicSampleSize == kSample32) {
			processorImpl = std::make_unique<ProcessorImpl<float>>();
		} else {
			processorImpl = std::make_unique<ProcessorImpl<double>>();
		}*/
	}
	return kResultTrue;
}

tresult PLUGIN_API Processor::setBusArrangements(SpeakerArrangement* inputs, int32 numIns, SpeakerArrangement* outputs, int32 numOuts) {
	// Only support stereo in/stereo out
	if (numIns == 1 && numOuts == 1 && inputs[0] == SpeakerArr::kStereo && inputs[0] == outputs[0]) {
		return ProcessorBase::setBusArrangements(inputs, numIns, outputs, numOuts);
	}
	return kResultFalse;
}

void Processor::processAudio(ProcessData& data) {
	int32 numChannels = data.inputs[0].numChannels;
	int32 numSamples = data.numSamples;


	//if (processSetup.symbolicSampleSize == kSample32)
	void** in = getChannelBuffersPointer(processSetup, data.inputs[0]);
	void** out = getChannelBuffersPointer(processSetup, data.outputs[0]);

	// Handle silence flags
	{
		if (data.inputs[0].silenceFlags != 0 && vuPPM < 0.0001) {
			data.outputs[0].silenceFlags = data.inputs[0].silenceFlags;

			uint32 sampleFramesSize = getSampleFramesSizeInBytes(processSetup, data.numSamples);
			for (int32 i = 0; i < numChannels; ++i) {
				if (in[i] != out[i]) {
					memset(out[i], 0, sampleFramesSize);
				}
			}
			return;
		}
		data.outputs[0].silenceFlags = 0;
	}

	Sample32* sInL;
	Sample32* sInR;
	float wet = mix;
	float dry = 1 - wet;
	Math::Vector<float, ProcessorImpl<float>::numChannels> tmp;

	vuPPMOld = vuPPM;

	for (int32 i = 0; i < numSamples; i++) {
		sInL = (Sample32*)in[0] + i;
		sInR = (Sample32*)in[1] + i;

		tmp = processorImpl->process(*sInL, *sInR);

		*((Sample32*)out[0] + i) = volume * tmp[0] * wet + dry * (*sInL);
		*((Sample32*)out[1] + i) = volume * tmp[1] * wet + dry * (*sInR);
		vuPPM += std::abs(tmp[0] + tmp[1]);
	}
	vuPPM /= 2 * numSamples;
	if (vuPPM != vuPPMOld) {
		//float db = 20 * std::log10(vuPPM);
		addOutputPoint(data, kParamVUPPM, vuPPM);
	}
}

void Processor::processParameterChanges(IParameterChanges* inputParameterChanges) {
	Algo::foreach (inputParameterChanges, [&](IParamValueQueue& paramQueue) {
		// Just process the latest parameter change and apply it immediately (ignoring sampleOffset)
		// For sample-accurate automation this needs to be more precise.
		Algo::foreachLast(paramQueue, [&](int32 id, int32 sampleOffset, ParamValue value) {
			if (id == bypassId) {
				setBypassed(value > 0.5);
			}
			else {
				paramState.params[id] = value;
			}
			if (id >= Params::kParamIn0 && id < Params::kParamIn0 + maxDimension) {
				updateResonatorInputPosition();
			}
			if (id >= Params::kParamOut0 && id < Params::kParamOut0 + maxDimension) {
				updateResonatorOutputPosition();
			}
			if (id == Params::kParamResonatorDim) {
				updateResonatorDimension();
			}
		}

		);
	});
	recomputeInexpensiveParameters();
}

void Processor::recomputeParameters() {
	//updateResonatorInputPosition();
	//updateResonatorOutputPosition();
	recomputeInexpensiveParameters();
	updateResonatorDimension();
}

void Processor::recomputeInexpensiveParameters() {
	volume = paramState[Params::kParamVol];
	mix = paramState[Params::kParamMix];
	resonatorOrder = normalizedToDiscrete(paramState[Params::kParamResonatorOrder], 1, maxOrder);
	resonatorFreq = paramState[Params::kParamResonatorFreq] * 1900 + 100;
	resonatorDamp = paramState[Params::kParamResonatorDamp] * 100;
	resonatorVel = paramState[Params::kParamResonatorVel] * (1000 - .1) + .1;

	// auto& f = processorImpl->resonator.timeFunctions;
	// FDebugPrint("Out EF %i %i: %f, %f, %f, %f,%f, %f, %f, %f, %f, %f\n", resonatorDim, resonatorOrder, f[0].real(), f[1].real(), f[2].real(), f[3].real(), f[4].real(), f[5].real(), f[6].real(), f[7].real(), f[8].real(), f[9].real());

	if (processorImpl) {
		processorImpl->setResonatorFreq(resonatorFreq, resonatorDamp, resonatorVel);
		processorImpl->setFilterCutoff(paramState[Params::kParamFilterCutoff]);
		processorImpl->setResonatorOrder(resonatorOrder);
	}
}

void Processor::updateResonatorInputPosition() {
	Vec inputPos;
	size_t d = inputPos.size();
	for (int i = 0; i < d; i++) {
		inputPos[i] = paramState[Params::kParamIn0 + i];
	}
	processorImpl->resonator.setInputPositions({ inputPos, inputPos });
	// FDebugPrint("Outputpos %f, %f, %f, %f,%f, %f, %f, %f, %f, %f\n", inputPos[0], inputPos[1], inputPos[2], inputPos[3], inputPos[4], inputPos[5], inputPos[6], inputPos[7], inputPos[8], inputPos[9]);
}

void Processor::updateResonatorOutputPosition() {
	Vec outputPos;
	size_t d = outputPos.size();
	for (int i = 0; i < d; i++) {
		outputPos[i] = paramState[Params::kParamOut0 + i];
	}
	processorImpl->resonator.setOutputPositions({ outputPos, outputPos });
	// FDebugPrint("Outputpos %f, %f, %f, %f,%f, %f, %f, %f, %f, %f\n", outputPos[0], outputPos[1], outputPos[2], outputPos[3], outputPos[4], outputPos[5], outputPos[6], outputPos[7], outputPos[8], outputPos[9]);
}

void Processor::updateResonatorDimension() {
	//resonatorDim = std::min<int>(maxDimension - 1, (int)(paramState[Params::kParamResonatorDim] * maxDimension)) + 1;
	resonatorDim = normalizedToDiscrete(paramState[Params::kParamResonatorDim], 1, maxDimension);
	processorImpl->setResonatorDim(resonatorDim);
	// Need to reevaluate all eigenfunctions
	updateResonatorInputPosition();
	updateResonatorOutputPosition();
	//auto& f = processorImpl->resonator.inputPosEF[0];
	//FDebugPrint("Out EF %i: %f, %f, %f, %f,%f, %f, %f, %f, %f, %f\n", resonatorDim, f[0].real(), f[1].real(), f[2].real(), f[3].real(), f[4].real(), f[5].real(), f[6].real(), f[7].real(), f[8].real(), f[9].real());
}

void Processor::addOutputPoint(ProcessData& data, ParamID id, ParamValue value) {
	if (data.outputParameterChanges) {
		int32 index;
		IParamValueQueue* queue = data.outputParameterChanges->addParameterData(id, index);
		if (queue) {
			queue->addPoint(0, value, index);
		}
	}
}

} // namespace TesseractFx
} // namespace Uberton
