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
#include <chrono>

namespace Uberton {
namespace TesseractFx {

Processor::Processor() {
	setControllerClass(ControllerUID);

	auto initValue = [&](const auto& p) {
		paramState[p.id] = p.toNormalized(p.initialValue);
	};

	paramState.version = stateVersion;

	initValue(ParamSpecs::vol);
	initValue(ParamSpecs::mix);
	initValue(ParamSpecs::resonatorType);
	initValue(ParamSpecs::resonatorDim);
	initValue(ParamSpecs::resonatorOrder);
	initValue(ParamSpecs::freq);
	initValue(ParamSpecs::damp);
	initValue(ParamSpecs::resonatorVel);

	initValue(ParamSpecs::inPosCurveL);
	initValue(ParamSpecs::inPosCurveR);
	initValue(ParamSpecs::outPosCurveL);
	initValue(ParamSpecs::outPosCurveR);

	initValue(ParamSpecs::lcFreq);
	initValue(ParamSpecs::lcQ);
	initValue(ParamSpecs::hcFreq);
	initValue(ParamSpecs::hcQ);

	for (int i = 0; i < maxDimension; i++) {
		paramState[Params::kParamInL0 + i] = .5;
		paramState[Params::kParamInR0 + i] = .5;
		paramState[Params::kParamOutL0 + i] = .5;
		paramState[Params::kParamOutR0 + i] = .5;
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
		if (processSetup.symbolicSampleSize == kSample32) {
			processorImpl = std::make_unique<ProcessorImpl<float>>();
		}
		else {
			processorImpl = std::make_unique<ProcessorImpl<double>>();
		}
		processorImpl->init(processSetup.sampleRate);
		recomputeParameters();
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

tresult PLUGIN_API Processor::canProcessSampleSize(int32 symbolicSampleSize) {
	if (symbolicSampleSize == kSample32) return kResultTrue;
	if (symbolicSampleSize == kSample64) return kResultTrue;
	return kResultFalse;
}

void Processor::processAudio(ProcessData& data) {
	using std::chrono::steady_clock;
	auto t0 = steady_clock::now();



	// Handle silence flags
	{
		if (data.inputs[0].silenceFlags != 0 && vuPPM < 0.0001) {
			data.outputs[0].silenceFlags = data.inputs[0].silenceFlags;

			uint32 sampleFramesSize = getSampleFramesSizeInBytes(processSetup, data.numSamples);
			int32 numChannels = data.inputs[0].numChannels;
			void** in = getChannelBuffersPointer(processSetup, data.inputs[0]);
			void** out = getChannelBuffersPointer(processSetup, data.outputs[0]);
			for (int32 i = 0; i < numChannels; ++i) {
				if (in[i] != out[i]) {
					memset(out[i], 0, sampleFramesSize);
				}
			}
			return;
		}
		data.outputs[0].silenceFlags = 0;
	}
	vuPPMOld = vuPPM;
	vuPPM = processorImpl->processAll(data, mix, volume);
	if (isBypassed()) { // add "last" point, before this function is not called anymore
		Processor::addOutputPoint(data, kParamVUPPM_L, 0);
		Processor::addOutputPoint(data, kParamVUPPM_R, 0);
	}
	//if (vuPPM != vuPPMOld) {
	//	addOutputPoint(data, kParamVUPPM, vuPPM);
	//	if (isBypassed()) { // add "last" point, before this function is not called anymore
	//		addOutputPoint(data, kParamVUPPM, 0);
	//	}
	//}
	std::chrono::duration<double> duration = steady_clock::now() - t0;

	addOutputPoint(data, kParamProcessTime, (duration.count() / data.numSamples) * 1000.0 / 10.0);
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
			if (id == Params::kParamInPosCurveL || id == Params::kParamInPosCurveR || (id >= Params::kParamInL0 && id <= Params::kParamInRN)) {
				processorImpl->updateResonatorInputPosition(paramState);
			}
			if (id == Params::kParamOutPosCurveL || id == Params::kParamOutPosCurveR || (id >= Params::kParamOutL0 && id <= Params::kParamOutRN)) {
				processorImpl->updateResonatorOutputPosition(paramState);
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
	recomputeInexpensiveParameters();
	updateResonatorDimension(); // also updates positions!
}

void Processor::recomputeInexpensiveParameters() {
	volume = toScaled(ParamSpecs::vol);
	mix = paramState[Params::kParamMix];
	resonatorOrder = toDiscrete(ParamSpecs::resonatorOrder);
	resonatorFreq = toScaled(ParamSpecs::freq);
	resonatorDamp = toScaled(ParamSpecs::damp);
	resonatorVel = toScaled(ParamSpecs::resonatorVel);

	// auto& f = processorImpl->resonator.timeFunctions;
	// FDebugPrint("Out EF %i %i: %f, %f, %f, %f,%f, %f, %f, %f, %f, %f\n", resonatorDim, resonatorOrder, f[0].real(), f[1].real(), f[2].real(), f[3].real(), f[4].real(), f[5].real(), f[6].real(), f[7].real(), f[8].real(), f[9].real());

	if (processorImpl) {
		processorImpl->setResonatorFreq(resonatorFreq, resonatorDamp, resonatorVel);
		processorImpl->setResonatorOrder(resonatorOrder);
		processorImpl->setLCFilterFreqAndQ(toScaled(ParamSpecs::lcFreq), toScaled(ParamSpecs::lcQ));
		processorImpl->setHCFilterFreqAndQ(toScaled(ParamSpecs::hcFreq), toScaled(ParamSpecs::hcQ));
	}
}

void Processor::updateResonatorDimension() {
	resonatorDim = toDiscrete(ParamSpecs::resonatorDim);
	if (processorImpl) {
		processorImpl->setResonatorDim(resonatorDim);
		// Need to reevaluate all eigenfunctions
		processorImpl->updateResonatorInputPosition(paramState);
		processorImpl->updateResonatorOutputPosition(paramState);
	}
	//auto& f = processorImpl->resonator.inputPosEF[0];
	//FDebugPrint("Out EF %i: %f, %f, %f, %f,%f, %f, %f, %f, %f, %f\n", resonatorDim, f[0].real(), f[1].real(), f[2].real(), f[3].real(), f[4].real(), f[5].real(), f[6].real(), f[7].real(), f[8].real(), f[9].real());
}
FUnknown* createProcessorInstance(void*) {
	return static_cast<IAudioProcessor*>(new Processor);
}
} // namespace TesseractFx
} // namespace Uberton
