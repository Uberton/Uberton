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
#include <resonator.h>
#include <chrono>

namespace Uberton {
namespace ResonatorPlugin {
namespace Hypersphere {

Processor::Processor() {
	setControllerClass(ControllerUID);

	auto initValue = [&](const auto& p) {
		paramState[p.id] = p.toNormalized(p.initialValue);
	};

	paramState.version = stateVersion;

	initValue(ParamSpecs::resonatorDim);
	initValue(ParamSpecs::resonatorOrder);
	for (int i = 0; i < maxDimension; i++) {
		paramState[Params::kParamInL0 + i] = 0;
		paramState[Params::kParamInR0 + i] = 0;
		paramState[Params::kParamOutL0 + i] = 0;
		paramState[Params::kParamOutR0 + i] = 0;
	}
}

tresult PLUGIN_API Processor::setActive(TBool state) {
	if (state) {
		if (processSetup.symbolicSampleSize == kSample32) {
			processorImpl = std::make_unique<SphereProcessorImpl<Math::NSphereResonator<float, maxDimension, maxOrder, 2>, float>>();
			Math::PreComputedCubeResonator<float, maxDimension, maxOrder, 2> a;
		}
		else {
			processorImpl = std::make_unique<SphereProcessorImpl<Math::NSphereResonator<double, maxDimension, maxOrder, 2>, double>>();
		}
		processorImpl->init(processSetup.sampleRate);
		recomputeParameters();
	}
	else {
		processorImpl.reset();
		sendMessageID(processorDeactivatedMsgID);
	}
	return kResultTrue;
}

void Processor::recomputeInexpensiveParameters() {
	resonatorOrder = toDiscrete(ParamSpecs::resonatorOrder);
	ResonatorProcessorBase::recomputeInexpensiveParameters();
}

void Processor::updateResonatorDimension() {
	resonatorDim = toDiscrete(ParamSpecs::resonatorDim);
	ResonatorProcessorBase::updateResonatorDimension();
}

FUnknown* createProcessorInstance(void*) {
	return static_cast<IAudioProcessor*>(new Processor);
}
} // namespace Hypersphere
} // namespace ResonatorPlugin
} // namespace Uberton