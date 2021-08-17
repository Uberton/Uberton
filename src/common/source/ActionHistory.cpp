// -----------------------------------------------------------------------------------------------------------------------------
// This file is part of the Überton project. Copyright (C) 2021 Überton
//
// Überton is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// Überton is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
// of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details. You should
// have received a copy of the GNU General Public License along with Überton. If not, see http://www.gnu.org/licenses/.
// -----------------------------------------------------------------------------------------------------------------------------

#include "ActionHistory.h"

using namespace Uberton;

bool ActionHistory::canUndo() const {
	return undoStack.size() > 0;
}

bool ActionHistory::canRedo() const {
	return redoStack.size() > 0;
}

std::optional<Action> ActionHistory::undo() {
	if (canUndo()) {
		Action action = undoStack.back();
		undoStack.pop_back();
		redoStack.push_back(action);
		return action;
	}
	else {
		return std::nullopt;
	}
}

std::optional<Action> ActionHistory::redo() {
	if (canRedo()) {
		Action action = redoStack.back();
		redoStack.pop_back();
		undoStack.push_back(action);
		return action;
	}
	else {
		return std::nullopt;
	}
}

void ActionHistory::execute(int id, double oldValue, double newValue) {
	execute({ id, oldValue, newValue });
}

void Uberton::ActionHistory::execute(const Action& action) {
	redoStack.clear();
	undoStack.push_back(action);
	keepBelowMaxUndoSteps();
}

void ActionHistory::clear() {
	undoStack.clear();
	redoStack.clear();
}

void ActionHistory::setMaxUndoSteps(int maxUndoSteps) {
	this->maxUndoSteps = maxUndoSteps;
	keepBelowMaxUndoSteps();
}

int ActionHistory::getMaxUndoSteps() const {
	return maxUndoSteps;
}

void ActionHistory::keepBelowMaxUndoSteps() {
	if (undoStack.size() > maxUndoSteps) {
		undoStack.erase(undoStack.begin(), undoStack.begin() + (undoStack.size() - maxUndoSteps));
	}
}
