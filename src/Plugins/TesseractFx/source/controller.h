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

#include "ids.h"
#include <ControllerBase.h>
#include <vstgui/plugin-bindings/vst3editor.h>

namespace Uberton {
namespace TesseractFx {
using namespace Steinberg;
using namespace Steinberg::Vst;

class Controller : public ControllerBase<ParamState, ImplementBypass>
{
public:
	tresult PLUGIN_API initialize(FUnknown* context) SMTG_OVERRIDE;
	IPlugView* PLUGIN_API createView(FIDString name) SMTG_OVERRIDE;

	static FUnknown* createInstance(void*) { return (Vst::IEditController*)new Controller(); }

	tresult PLUGIN_API getState(IBStream* stream) SMTG_OVERRIDE {

		IBStreamer s(stream, kLittleEndian);
		if (!s.writeInt64u(stateVersion)) return kResultFalse;

		double zoomFactor = 0.5;
		if (editors.size() > 1) {
			zoomFactor = editors[0]->getZoomFactor();
		}
		if (!s.writeDouble(zoomFactor)) return kResultFalse;
		return kResultOk;
	}
	tresult PLUGIN_API setState(IBStream* stream) SMTG_OVERRIDE {

		IBStreamer s(stream, kLittleEndian);
		uint64 version = 0;

		if (!s.readInt64u(version)) return kResultFalse;
		if (!s.readDouble(initialZoomFactor)) return kResultFalse;
		return kResultOk;
	}

private:
	double initialZoomFactor{ 0.5 };
};

}
}