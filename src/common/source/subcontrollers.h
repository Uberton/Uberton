
// Special subcontrollers for vstgui ui elements
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


#pragma once

#include <vstgui/uidescription/delegationcontroller.h>


namespace VSTGUI {
class VST3Editor;
}

namespace Uberton {

using namespace VSTGUI;

class HistoryControllerBase;
class HistoryButton;
class LinkButton;


/*
 * Subcontroller for two history buttons (undo and redo). 
 * Set this controller as subcontroller of a view container and place two HistoryButtons inside 
 * (one of type HistoryButton::Type::Undo and one of HistoryButton::Type::Redo). The press events 
 * are forwarded to the controller which needs to be derived from HistoryControllerBase. 
 * 
 * The controller is responsible for telling the HistorySubcontroller if undo/redo is available at the 
 * moment (once upon creation time and then throu updateButtonState()) and the controller sets the 
 * mouse enabled state of the buttons accordingly. 
 * 
 */
class HistorySubcontroller : public DelegationController
{
public:
	HistorySubcontroller(HistoryControllerBase* hController, IController* parentController, bool initialCanUndo, bool initialCanRedo);

	CView* verifyView(CView* view, const UIAttributes& attributes, const IUIDescription* description) override;
	void updateButtonState(bool canUndo, bool canRedo);


protected:
	void undo();
	void redo();


private:
	HistoryButton* undoButton{ nullptr };
	HistoryButton* redoButton{ nullptr };
	HistoryControllerBase* hController;
	bool initialCanUndo;
	bool initialCanRedo;
};


/*
 * Subcontroller for a container that links the states of all child CControl instances except a
 * LinkButton that serves as a switch to enable/disable linking. 
 */
class ParamLinkSubcontroller : public DelegationController
{
public:
	using LinkedControlType = CControl;
	using LinkerType = LinkButton;

	ParamLinkSubcontroller(IController* parentController);

	CView* verifyView(CView* view, const UIAttributes& attributes, const IUIDescription* description) override;
	void valueChanged(CControl* control) override;
	void controlBeginEdit(CControl* control) override;
	//void controlEndEdit(CControl* control) override;
	void updateLinkState();


protected:
	LinkerType* linkerControl{ nullptr };
	std::vector<std::pair<LinkedControlType*, float>> linkedControls;
	float draggedControlStartValue{ 0 };

	bool link{ false };
	VST3Editor* editor{ nullptr };
};

}