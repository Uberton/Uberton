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
#include <ResonatorProcessorImpl.h>

namespace Uberton {
namespace Math {
template<class T, int maxDim, int maxOrder>
CubeEWPStorage<T, maxDim> getCubeEWPStorage();
}
}
namespace Uberton {
namespace ResonatorPlugin {
namespace Tesseract {


template<class SampleType, int numChannels>
using CubeResonator = Math::PreComputedCubeResonator<SampleType, maxDimension, maxOrder, numChannels>;





template<typename SampleType, int numChannels = 2>
class ProcessorImplCube : public ProcessorImpl<CubeResonator<SampleType, numChannels>, SampleType, numChannels>
{
public:
	using Resonator = CubeResonator<SampleType, numChannels>;

	ProcessorImplCube() {
		this->resonator.setStorage(Math::getCubeEWPStorage<SampleType, Resonator::maxDimension(), Resonator::maxOrder()>());
	}

protected:
	void updateCompensation() override {
		// higher resonator orders result in considerably higher volumes
		this->volumeCompensation = 0.03f / std::sqrt(this->currentResonatorOrder);
	}
};





Processor::Processor() {
	setControllerClass(ControllerUID);

	auto initValue = [&](const auto& p, ParamID id = -1) {
		paramState[id == -1 ? p.id : id] = p.toNormalized(p.initialValue);
	};

	paramState.version = stateVersion;

	initValue(ParamSpecs::resonatorDim);
	initValue(ParamSpecs::resonatorOrder);

	for (int i = 0; i < maxDimension; i++) {
		initValue(ParamSpecs::resonatorInputCoordinate, Params::kParamInL0 + i);
		initValue(ParamSpecs::resonatorInputCoordinate, Params::kParamInR0 + i);
		initValue(ParamSpecs::resonatorOutputCoordinate, Params::kParamOutL0 + i);
		initValue(ParamSpecs::resonatorOutputCoordinate, Params::kParamOutR0 + i);
	}
}

tresult PLUGIN_API Processor::setActive(TBool state) {
	if (state) {
		constexpr int stereo = 2;
		if (processSetup.symbolicSampleSize == kSample32) {
			//using Resonator = Math::PreComputedCubeResonator<float, maxDimension, maxOrder, stereo>;
			processorImpl = std::make_unique<ProcessorImplCube<float, stereo>>();
		} else {
			//using Resonator = Math::PreComputedCubeResonator<double, maxDimension, maxOrder, stereo>;
			processorImpl = std::make_unique<ProcessorImplCube<double, stereo>>();
		}
		processorImpl->init(processSetup.sampleRate);
		recomputeParameters();
	} else {
		processorImpl.reset();
		sendMessageID(processorDeactivatedMsgID);
	}
	return kResultTrue;
}

void Processor::recomputeInexpensiveParameters() {
	state.resonatorOrder = toDiscrete(ParamSpecs::resonatorOrder);
	ResonatorProcessorBase::recomputeInexpensiveParameters();
}

void Processor::updateResonatorDimension() {
	state.resonatorDim = toDiscrete(ParamSpecs::resonatorDim);
	ResonatorProcessorBase::updateResonatorDimension();
}

FUnknown* createProcessorInstance(void*) {
	return static_cast<IAudioProcessor*>(new Processor);
}
} // namespace Tesseract
} // namespace ResonatorPlugin
} // namespace Uberton
