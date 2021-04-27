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


#include "controller.h"
#include "ids.h"

#include <base/source/fstreamer.h>
#include <pluginterfaces/base/ibstream.h>

namespace Uberton {
namespace BasicInstrument {

tresult PLUGIN_API Controller::initialize(FUnknown* context) {
	implementBypass = false;

	tresult result = ControllerBase::initialize(context);
	if (result != kResultTrue) return result;

	parameters.addParameter(STR16("Gain 1"), STR16("dB"), 0, .5, ParameterInfo::kCanAutomate, Params::kParamVolId);

	return kResultTrue;
}

tresult PLUGIN_API Controller::setComponentState(IBStream* state) {
	ParamState1 paramState;
	return paramState.setComponentState(state, *this);
}

IPlugView* PLUGIN_API Controller::createView(FIDString name) {
	ConstString name_(name);
	if (name_ == ViewType::kEditor)
		return new VSTGUI::VST3Editor(this, "Editor", "editor.uidesc");
	return nullptr;
}

} // namespace BasicInstrument
} // namespace Uberton