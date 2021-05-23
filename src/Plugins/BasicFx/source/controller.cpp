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
#include <vstmath.h>

namespace Uberton {
namespace BasicFx {

tresult PLUGIN_API Controller::initialize(FUnknown* context) {
	tresult result = ControllerBaseP::initialize(context);
	if (result != kResultTrue) return result;

	addRangeParam(Params::kParamVolId, "Gain", "dB", { 0, 1, .5 });
	return kResultTrue;
}

IPlugView* PLUGIN_API Controller::createView(FIDString name) {
	if (ConstString(name) == ViewType::kEditor)
		return new VSTGUI::VST3Editor(this, "Editor", "editor.uidesc");
	return nullptr;
}

} // namespace BasicFx
} // namespace Uberton