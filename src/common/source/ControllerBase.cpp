// -----------------------------------------------------------------------------------------------------------------------------
// This file is part of the Überton project. Copyright (C) 2021 Überton
//
// Überton is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// Überton is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
// of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details. You should
// have received a copy of the GNU General Public License along with Überton. If not, see http://www.gnu.org/licenses/.
// -----------------------------------------------------------------------------------------------------------------------------


#include "ControllerBase.h"
#include "parameters.h"
#include <pluginterfaces/base/ustring.h>
#include <base/source/fstreamer.h>
#include "ui.h"


namespace Uberton {
using namespace VSTGUI;

IController* PLUGIN_API HistoryControllerBase::createSubController(VSTGUI::UTF8StringPtr name, const VSTGUI::IUIDescription* description, VSTGUI::VST3Editor* editor) {
	if (UTF8StringView(name) == "HistoryController") {
		HistoryController* historyController = new HistoryController(this, editor, history.canUndo(), history.canRedo());
		hcm.insert({ editor, historyController });
		return historyController;
	}
	return nullptr;
}

void PLUGIN_API HistoryControllerBase::editorAttached(EditorView* editor) {
	auto p = dynamic_cast<TheEditor*>(editor);
	if (p) {
		editors.push_back(p);
	}
}

void PLUGIN_API HistoryControllerBase::editorRemoved(EditorView* editor) {
	editors.erase(std::remove_if(editors.begin(), editors.end(), [&](EditorView* v) { return editor == v; }));
	hcm.erase(editor);
}

tresult HistoryControllerBase::beginEdit(Vst::ParamID id) {
	if (id != -1) {
		currentlyEditedParam = id;
		startValue = getParamNormalized(id);
	}
	return Parent::beginEdit(id);
}

tresult HistoryControllerBase::endEdit(Vst::ParamID id) {
	if (id != -1 && id == currentlyEditedParam) {
		Parameter* p = getParameterObject(id);

		std::wstring s = p->getInfo().title;	std::wstringstream sstream;
		sstream << "Changed parameter " << p->getInfo().title << " from ";
		String128 buffer;
		p->toString(startValue, buffer);
		sstream << buffer << p->getInfo().units << " to ";
		p->toString(getParamNormalized(id), buffer);
		sstream << buffer << p->getInfo().units<< "\n";

		std::wstring widestring = sstream.str();
		std::string narrowstring;
		for (const auto c : widestring) {
			narrowstring += (char)c;
		}

		FDebugPrint(narrowstring.c_str());
		history.execute(id, startValue, getParamNormalized(id));
		updateHistoryButtons();
		currentlyEditedParam = -1;
	}
	return Parent::endEdit(id);
}

void HistoryControllerBase::undo() {
	if (auto a = history.undo()) {
		Action action = a.value();
		applyAction(action.id, action.oldValue);
		updateHistoryButtons();
	}
}

void HistoryControllerBase::redo() {
	if (auto a = history.redo()) {
		Action action = a.value();
		applyAction(action.id, action.newValue);
		updateHistoryButtons();
	}
}

void HistoryControllerBase::applyAction(ParamID id, ParamValue value) {
	Parent::beginEdit(id);
	setParamNormalized(id, value);
	Parent::performEdit(id, value);
	Parent::endEdit(id);
}

void HistoryControllerBase::updateHistoryButtons() {
	for (auto& controller : hcm) {
		controller.second->updateButtonState(history.canUndo(), history.canRedo());
	}
}




ControllerBase1::ControllerBase1() {}

tresult PLUGIN_API ControllerBase1::initialize(FUnknown* context) {
	tresult result = EditControllerEx1::initialize(context);
	if (result != kResultOk) return result;

	if (implementBypass)
		parameters.addParameter(USTRING("Bypass"), nullptr, 1, 0, ParameterInfo::kIsBypass | ParameterInfo::kCanAutomate, bypassId);

	return kResultOk;
}

} // namespace Uberton