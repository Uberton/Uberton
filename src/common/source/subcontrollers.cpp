// -----------------------------------------------------------------------------------------------------------------------------
// This file is part of the Überton project. Copyright (C) 2021 Überton
//
// Überton is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// Überton is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
// of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details. You should
// have received a copy of the GNU General Public License along with Überton. If not, see http://www.gnu.org/licenses/.
// -----------------------------------------------------------------------------------------------------------------------------


#include "subcontrollers.h"
#include "ControllerBase.h"
#include "ui.h"

using namespace Uberton;


HistorySubcontroller::HistorySubcontroller(HistoryControllerBase* hController, IController* parentController, bool initialCanUndo, bool initialCanRedo)
	: DelegationController(parentController), hController(hController), initialCanUndo(initialCanUndo), initialCanRedo(initialCanRedo) {}

CView* HistorySubcontroller::verifyView(CView* view, const UIAttributes& attributes, const IUIDescription* description) {
	HistoryButton* button = dynamic_cast<HistoryButton*>(view);
	if (button) {
		if (button->getType() == HistoryButton::Type::Undo) {
			undoButton = button;
			undoButton->setCallback([this](HistoryButton::Type type) { undo(); });
			undoButton->setMouseEnabled(initialCanUndo);
		}
		else if (button->getType() == HistoryButton::Type::Redo) {
			redoButton = button;
			redoButton->setCallback([this](HistoryButton::Type type) { redo(); });
			redoButton->setMouseEnabled(initialCanRedo);
		}
	}
	return view;
}

void HistorySubcontroller::undo() {
	hController->undo();
}

void HistorySubcontroller::redo() {
	hController->redo();
}

void HistorySubcontroller::updateButtonState(bool canUndo, bool canRedo) {
	if (undoButton && canUndo != undoButton->getMouseEnabled()) {
		undoButton->setMouseEnabled(canUndo);
		undoButton->setDirty();
	}
	if (redoButton && canRedo != redoButton->getMouseEnabled()) {
		redoButton->setMouseEnabled(canRedo);
		redoButton->setDirty();
	}
}


ParamLinkSubcontroller::ParamLinkSubcontroller(IController* parentController) : DelegationController(parentController) {
	if (auto editor = dynamic_cast<VST3Editor*>(parentController)) {
		this->editor = editor;
	}
}

CView* ParamLinkSubcontroller::verifyView(CView* view, const UIAttributes& attributes, const IUIDescription* description) {
	LinkedControlType* linkedControl = dynamic_cast<LinkedControlType*>(view);
	LinkerType* linker = dynamic_cast<LinkerType*>(view);
	if (linker) {
		linkerControl = linker;
		linkerControl->registerControlListener(this);
		updateLinkState();
	}
	else if (linkedControl) {
		linkedControls.push_back(linkedControl);
		linkedControl->registerControlListener(this);
	}
	return view;
}

void ParamLinkSubcontroller::valueChanged(CControl* control) {
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

void ParamLinkSubcontroller::updateLinkState() {
	if (linkerControl->getValueNormalized() != 0)
		link = true;
	else
		link = false;
}
