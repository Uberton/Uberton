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
//#include "gui.h"
#include <vstmath.h>
//#include <viewcreator.h>

#include <vstgui/uidescription/delegationcontroller.h>

namespace Uberton {
namespace ManualGui {


class LinkController : public DelegationController
{
public:
	using LinkedControlType = CSlider;
	using LinkerType = COnOffButton;
	LinkController(IController* parentController) : DelegationController(parentController) {
	}
	CView* verifyView(CView* view, const UIAttributes& attributes, const IUIDescription* description) override {
		LinkedControlType* linkedControl = dynamic_cast<LinkedControlType*>(view);
		LinkerType* linker = dynamic_cast<LinkerType*>(view);
		if (linkedControl) {
			linkedControls.push_back(linkedControl);
			linkedControl->registerControlListener(this);
		}
		else if (linker) {
			linkerControl = linker;
			linkerControl->registerControlListener(this);
			updateLinkState();
		}
		return view;
	}

	void valueChanged(CControl* control) override {
		if (control == linkerControl) {
			updateLinkState();
			DelegationController::valueChanged(control);
			return;
		}
		if (link) {
			for (const auto& c : linkedControls) {
				if (c != control) {
					auto z = control->getValueNormalized();
					c->setValueNormalized(z);
					DelegationController::valueChanged(c);
				}
			}
		}
		DelegationController::valueChanged(control);
	}

	void updateLinkState() {
		if (linkerControl->getValueNormalized() != 0)
			link = true;
		else
			link = false;
	}

protected:
	LinkerType* linkerControl{ nullptr };
	std::vector<LinkedControlType*> linkedControls;

	bool link{ false };
};



tresult PLUGIN_API Controller::initialize(FUnknown* context) {
	tresult result = ControllerBase::initialize(context);
	if (result != kResultTrue) return result;

	addRangeParam(Params::kParamVolId, "Gain", "dB", { -9, 11, 4 });
	addRangeParam(Params::kParamSlider1, "Slider1", "Hz", { 0, 200, 11 });
	addRangeParam(Params::kParamSlider2, "Slider2", "", { 50, 100, 52 });

	addStringListParam({ Params::kParamLink, 0, 1, 1, 1 }, "Link", "", { "Link", "Unlinked" });
	
	//addRangeParam(45, "Zoom", "%", {})
	//addStringListParam(ParamSpec(45, 0, 2, 0, 1), "Zoom", "Zoom", { "50%", "100%", "200%" });
	return kResultTrue;
}

IPlugView* PLUGIN_API Controller::createView(FIDString name) {
	if (ConstString(name) == ViewType::kEditor) {
		//auto editor = new Editor(this);
		auto editor = new VSTGUI::VST3Editor(this, "Editor", "editor.uidesc");

		//editor->setZoomFactor(2);
		//editor->setAllowedZoomFactors({ .5, 1, 2, 4 });



		return editor;
	}
	return nullptr;
}


/*void PLUGIN_API Controller::editorAttached(EditorView* editor) {
	auto p = dynamic_cast<TheEditor*>(editor);
	if (p) {
		editors.push_back(p);
	}
}
void PLUGIN_API Controller::editorRemoved(EditorView* editor) {
	editors.erase(std::remove_if(editors.begin(), editors.end(), [&](EditorView* v) { return editor == v; }));
	hcm.erase(editor);
}
void Controller::updateHistoryButtons() {
	for (auto& controller : hcm) {
		controller.second->updateButtonState(history.canUndo(), history.canRedo());
	}
}*/
VSTGUI::IController* PLUGIN_API Controller::createSubController(VSTGUI::UTF8StringPtr name, const VSTGUI::IUIDescription* description, VSTGUI::VST3Editor* editor) {

	if (UTF8StringView(name) == "LinkController") {
		return new LinkController(editor);
	}
	/*else if (UTF8StringView(name) == "HistoryController") {
		HistoryController* historyController = new HistoryController(this, editor, history.canUndo(), history.canRedo());
		hcm.insert({ editor, historyController });
		return historyController;
	}*/
	return ControllerBase::createSubController(name, description, editor);
}
} // namespace ManualGui
} // namespace Uberton