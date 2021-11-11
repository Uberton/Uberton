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
#include "ui.h"
#include "subcontrollers.h"
#include <sstream>
#include <public.sdk/source/vst/vsthelpers.h>
#include <optional>


namespace Uberton {
using namespace VSTGUI;

IController* PLUGIN_API HistoryControllerBase::createSubController(VSTGUI::UTF8StringPtr name, const VSTGUI::IUIDescription* description, VSTGUI::VST3Editor* editor) {
	if (UTF8StringView(name) == "HistorySubcontroller") {
		HistorySubcontroller* historyController = new HistorySubcontroller(this, editor, history.canUndo(), history.canRedo());
		hcm.insert({ editor, historyController });
		return historyController;
	}
	if (UTF8StringView(name) == "ParamLinkSubcontroller") {
		ParamLinkSubcontroller* linkController = new ParamLinkSubcontroller(editor);
		return linkController;
	}
	return nullptr;
}

void PLUGIN_API HistoryControllerBase::editorAttached(EditorView* editor) {
	auto p = dynamic_cast<TheEditor*>(editor);
	if (p) {
		updateCurrentZoomFactor();
		editors.push_back(p);
		p->setZoomFactor(currentZoomFactor);
	}
}

void PLUGIN_API HistoryControllerBase::editorRemoved(EditorView* editor) {
	if (auto p = dynamic_cast<VST3Editor*>(editor)) {
		currentZoomFactor = p->getZoomFactor();
	}
	editors.erase(std::remove_if(editors.begin(), editors.end(), [&](EditorView* v) { return editor == v; }));
	hcm.erase(editor);
}

tresult PLUGIN_API HistoryControllerBase::getState(IBStream* stream) {

	IBStreamer s(stream, kLittleEndian);
	if (!s.writeInt64u(stateVersion)) return kResultFalse;

	updateCurrentZoomFactor();
	if (!s.writeDouble(currentZoomFactor)) return kResultFalse;
	return kResultOk;
}

tresult PLUGIN_API HistoryControllerBase::setState(IBStream* stream) {
	IBStreamer s(stream, kLittleEndian);
	uint64 version = 0;

	if (!s.readInt64u(version)) return kResultFalse;
	if (!s.readDouble(currentZoomFactor)) return kResultFalse;
	if (Helpers::isProjectState(stream) == kResultTrue) {
		for (auto editor : editors) {
			editor->setZoomFactor(currentZoomFactor);
		}
	}

	history.clear();
	updateHistoryButtons();
	return kResultOk;
}

tresult HistoryControllerBase::beginEdit(Vst::ParamID id) {
	//FDebugPrint("beginEdit(%i)", id);
	if (id != invalidParamID) {
		currentlyEditedParam = id;
		startValue = getParamNormalized(id);
		//FDebugPrint("beginEdit(%i)", id);
	}
	return Parent::beginEdit(id);
}

tresult HistoryControllerBase::endEdit(Vst::ParamID id) {
	//FDebugPrint("endEdit(%i)", id);
	if (id != invalidParamID && id == currentlyEditedParam && startValue != getParamNormalized(id)) {

		Action action{ id, startValue, getParamNormalized(id) };

		history.execute(action);
		updateHistoryButtons();

		const std::wstring widestring = actionToString(action);
		std::string narrowstring;
		for (const auto c : widestring) {
			narrowstring += (char)c;
		}
		//FDebugPrint(narrowstring.c_str());
	}
	currentlyEditedParam = invalidParamID;
	return Parent::endEdit(id);
}

void HistoryControllerBase::undo() {
	if (auto a = history.undo()) {
		Action action = *a;
		applyAction(action.id, action.oldValue);
		updateHistoryButtons();
	}
}

void HistoryControllerBase::redo() {
	if (auto a = history.redo()) {
		Action action = *a;
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

std::wstring HistoryControllerBase::actionToString(const Action& action) {
	const Parameter* p = getParameterObject(action.id);
	const ParameterInfo& info = p->getInfo();
	const std::wstring title; // = info.shortTitle ? info.shortTitle : info.title;

	std::wstringstream sstream;
	sstream << title << ": ";
	String128 buffer;
	p->toString(startValue, buffer);
	sstream << buffer << info.units << " -> ";
	p->toString(getParamNormalized(action.id), buffer);
	sstream << buffer << info.units << "\n";

	const std::wstring widestring = sstream.str();
	return sstream.str();
}

void HistoryControllerBase::updateHistoryButtons() {
	for (auto& controller : hcm) {
		controller.second->updateButtonState(history.canUndo(), history.canRedo());
	}
}

void HistoryControllerBase::updateCurrentZoomFactor() {
	if (editors.size() > 0) {
		currentZoomFactor = editors[0]->getZoomFactor();
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
