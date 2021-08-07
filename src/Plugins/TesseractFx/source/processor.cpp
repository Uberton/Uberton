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

	auto initValue = [&](const auto& p) {
		paramState[p.id] = p.toNormalized(p.initialValue);
	};

	paramState.version = 0;

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
	//int32 numSamples = data.numSamples;


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
	vuPPMOld = vuPPM;
	vuPPM = processorImpl->processAll(data, mix, volume);
	if (vuPPM != vuPPMOld) {
		//float db = 20 * std::log10(vuPPM);
		addOutputPoint(data, kParamVUPPM, vuPPM);
	}
	/*Sample32* sInL;
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
	}*/
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
				updateResonatorInputPosition();
			}
			if (id == Params::kParamOutPosCurveL || id == Params::kParamOutPosCurveR || (id >= Params::kParamOutL0 && id <= Params::kParamOutRN)) {
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
	}
}

void Processor::updateResonatorInputPosition() {
	SpaceVec inputPosL, inputPosR;
	size_t d = inputPosL.size();
	for (int i = 0; i < d; i++) {
		inputPosL[i] = paramState[Params::kParamInL0 + i];
		inputPosR[i] = paramState[Params::kParamInR0 + i];
	}
	inputPosL += inputPosSpaceCurve(paramState[Params::kParamInPosCurveL]);
	inputPosR += inputPosSpaceCurve(paramState[Params::kParamInPosCurveR]);
	processorImpl->resonator.setInputPositions({ inputPosL, inputPosR });
	// FDebugPrint("Outputpos %f, %f, %f, %f,%f, %f, %f, %f, %f, %f\n", inputPos[0], inputPos[1], inputPos[2], inputPos[3], inputPos[4], inputPos[5], inputPos[6], inputPos[7], inputPos[8], inputPos[9]);
}

void Processor::updateResonatorOutputPosition() {
	SpaceVec outputPosL, outputPosR;
	size_t d = outputPosL.size();
	for (int i = 0; i < d; i++) {
		outputPosL[i] = paramState[Params::kParamOutL0 + i];
		outputPosR[i] = paramState[Params::kParamOutR0 + i];
	}
	outputPosL += outputPosSpaceCurve(paramState[Params::kParamOutPosCurveL]);
	outputPosR += outputPosSpaceCurve(paramState[Params::kParamOutPosCurveR]);
	processorImpl->resonator.setOutputPositions({ outputPosL, outputPosR });
	// FDebugPrint("Outputpos %f, %f, %f, %f,%f, %f, %f, %f, %f, %f\n", outputPos[0], outputPos[1], outputPos[2], outputPos[3], outputPos[4], outputPos[5], outputPos[6], outputPos[7], outputPos[8], outputPos[9]);
}

void Processor::updateResonatorDimension() {
	resonatorDim = toDiscrete(ParamSpecs::resonatorDim);
	processorImpl->setResonatorDim(resonatorDim);
	// Need to reevaluate all eigenfunctions
	updateResonatorInputPosition();
	updateResonatorOutputPosition();
	//auto& f = processorImpl->resonator.inputPosEF[0];
	//FDebugPrint("Out EF %i: %f, %f, %f, %f,%f, %f, %f, %f, %f, %f\n", resonatorDim, f[0].real(), f[1].real(), f[2].real(), f[3].real(), f[4].real(), f[5].real(), f[6].real(), f[7].real(), f[8].real(), f[9].real());
}

Processor::SpaceVec Processor::inputPosSpaceCurve(ParamValue t) {
	constexpr float pi = Math::pi<float>();
	const float t_ = t - .5;
	const float phi = t_ * 1.5 * pi;
	// for t == 0.5 this function returns the 0 vector.
	return SpaceVec{
		t_ * 0.5f,
		phi / (2 * pi) * std::sinf(phi),
		phi / (2 * pi) * std::cosf(phi + pi * 0.5f),
		t_ * 0.5f,
		t_ * 0.5f,
		t_ * 0.5f,
		t_ * 0.5f,
		t_ * 0.5f,
		t_ * 0.5f,
		t_ * 0.5f
	};
}

Processor::SpaceVec Processor::outputPosSpaceCurve(ParamValue t) {
	constexpr float pi = Math::pi<float>();
	const float t_ = t - .5;
	const float phi = t_ * 1.5 * pi;
	// for t == 0.5 this function returns the 0 vector.
	return SpaceVec{
		t_ * 0.5f,
		phi / (2 * pi) * std::sinf(phi),
		phi / (2 * pi) * std::cosf(phi + pi * 0.5f),
		t_ * 0.5f,
		t_ * 0.5f,
		t_ * 0.5f,
		t_ * 0.5f,
		t_ * 0.5f,
		t_ * 0.5f,
		t_ * 0.5f
	};
}

} // namespace TesseractFx
} // namespace Uberton
