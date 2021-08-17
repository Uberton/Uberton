// Action history for recording parameter changes through the gui and implement an undo mechanism.
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

#include <optional>
#include <vector>

namespace Uberton {


struct Action
{
	int id;
	double oldValue;
	double newValue;
};


class ActionHistory
{
public:
	bool canUndo() const;
	bool canRedo() const;

	std::optional<Action> undo();
	std::optional<Action> redo();

	void execute(int id, double oldValue, double newValue);

	void clear();


private:
	std::vector<Action> undoStack;
	std::vector<Action> redoStack;
};
}