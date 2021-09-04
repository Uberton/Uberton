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
#include <optional>

namespace Uberton {
namespace ManualGui {
using namespace Steinberg;
using namespace Steinberg::Vst;
using namespace VSTGUI;

class Editor;


struct Action
{
	ParamID id;
	ParamValue oldValue;
	ParamValue newValue;
};
class ActionHistory
{
public:
	bool canUndo() const {
		return undoStack.size() > 0;
	}
	bool canRedo() const {
		return redoStack.size() > 0;
	}
	std::optional<Action> undo() {
		if (canUndo()) {
			Action action = undoStack.top();
			undoStack.pop();
			redoStack.push(action);
			return action;
		}
		else {
			return std::nullopt;
		}
	}
	std::optional<Action> redo() {
		if (canRedo()) {
			Action action = redoStack.top();
			redoStack.pop();
			undoStack.push(action);
			return action;
		}
		else {
			return std::nullopt;
		}
	}
	void clear() {
		while (!undoStack.empty()) {
			undoStack.pop();
		}
		while (!redoStack.empty()) {
			redoStack.pop();
		}
	}

	void execute(ParamID id, ParamValue oldValue, ParamValue newValue) {

		while (!redoStack.empty()) {
			redoStack.pop();
		}
		undoStack.push({ id, oldValue, newValue });
	}


private:
	std::stack<Action> undoStack;
	std::stack<Action> redoStack;
};


class HistoryController;

class Controller : public ControllerBase<ParamState, ImplementBypass>//, public VSTGUI::VST3EditorDelegate
{
public:
	tresult PLUGIN_API initialize(FUnknown* context) SMTG_OVERRIDE;
	IPlugView* PLUGIN_API createView(FIDString name) SMTG_OVERRIDE;

	//void PLUGIN_API editorAttached(EditorView* editor) SMTG_OVERRIDE;
	//void PLUGIN_API editorRemoved(EditorView* editor) SMTG_OVERRIDE;

	static FUnknown* createInstance(void*) { return (Vst::IEditController*)new Controller(); }

	IController* PLUGIN_API createSubController(UTF8StringPtr name, const IUIDescription* description, VST3Editor* editor) SMTG_OVERRIDE;
	// Undo
	/*tresult beginEdit(ParamID id) override {
		if (id != -1) {
			currentlyEditedParam = id;
			startValue = getParamNormalized(id);
		}
		return ControllerBase<ParamState, ImplementBypass>::beginEdit(id);
	}

	tresult endEdit(ParamID id) override {
		if (id != -1 && id == currentlyEditedParam) {
			FDebugPrint("Changed param %i from %f to %f\n", id, startValue, getParamNormalized(id));
			history.execute(id, startValue, getParamNormalized(id));
			updateHistoryButtons();
			currentlyEditedParam = -1;
		}
		return ControllerBase<ParamState, ImplementBypass>::endEdit(id);
	}

	void undo() {
		if (auto a = history.undo()) {
			Action action = a.value();
			applyAction(action.id, action.oldValue);
			updateHistoryButtons();
		}
	}
	void redo() {
		if (auto a = history.redo()) {
			Action action = a.value();
			applyAction(action.id, action.newValue);
			updateHistoryButtons();
		}
	}
	void applyAction(ParamID id, ParamValue value) {
		ControllerBase<ParamState, ImplementBypass>::beginEdit(id);
		setParamNormalized(id, value);
		ControllerBase<ParamState, ImplementBypass>::performEdit(id, value);
		ControllerBase<ParamState, ImplementBypass>::endEdit(id);
	}

	void updateHistoryButtons();

	ActionHistory history;
	ParamValue startValue{ 0 };
	ParamID currentlyEditedParam = -1;

	using TheEditor = VSTGUI::VST3Editor;
	std::vector<TheEditor*> editors;
	//std::vector<int> hcm{};
	std::map<EditorView*, HistoryController*> hcm;*/
};

}
}