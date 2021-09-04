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
#include <ui.h>

namespace Uberton {
namespace BasicFx {

tresult PLUGIN_API Controller::initialize(FUnknown* context) {
	tresult result = ControllerBase::initialize(context);
	if (result != kResultTrue) return result;


	UnitID rootUnitId = 1;
	addUnit(new Unit(USTRING("Root"), rootUnitId));

	setCurrentUnitID(rootUnitId);
	addParam<LinearParameter>(ParamSpecs::vol, "Volumea", "Vol", "%", 0);
	addParam<DiscreteParameter>(ParamSpecs::list, "List", "List")->getInfo().stepCount = 5 - 1;

	return kResultTrue;
}

IPlugView* PLUGIN_API Controller::createView(FIDString name) {
	if (ConstString(name) == ViewType::kEditor) {
		auto editor = new VST3EditorEx1(this, "Editor", "editor.uidesc");
		editor->setPrescaleFactor(1);
		return editor;
	}
	return nullptr;
}

} // namespace BasicFx
} // namespace Uberton