// WindowController for programmatically creating standalone applications with VSTGUI without UIDescriptions.  
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

#include "vstgui/standalone/include/iwindow.h"
#include <vstgui/standalone/include/helpers/windowcontroller.h>
#include <vstgui/standalone/include/iapplication.h>
#include <vstgui/standalone/include/imenubuilder.h>
#include <vstgui/lib/cframe.h>


namespace Uberton {
namespace Installer {

using namespace VSTGUI;
using namespace VSTGUI::Standalone;


class EmptyMenuBuilder : public IMenuBuilder
{
	bool showCommandGroupInMenu(const Interface& context, const UTF8String& group) const override { return false; }
	bool showCommandInMenu(const Interface& context, const Command& cmd) const override { return false; }
	SortFunction getCommandGroupSortFunction(const Interface& context, const UTF8String& group) const override { return nullptr; }
	bool prependMenuSeparator(const Interface& context, const Command& cmd) const override { return false; }
};


class GeneralWindowController : public WindowControllerAdapter
{
public:
	virtual bool initialize(const WindowConfiguration& config, WindowPtr& window) { return true; }
};


template<class WindowController>
//requires std::derived_from<WindowController, GeneralWindowController>
std::pair<WindowPtr, WindowController*> makeWindow(const WindowConfiguration& config) {
	auto controller = std::make_shared<WindowController>();

	WindowPtr window = IApplication::instance().createWindow(config, controller);

	if (!window) return { nullptr, nullptr };
	if (!controller->initialize(config, window)) return { nullptr, nullptr };

	return { window, controller.get() };
}

}
}