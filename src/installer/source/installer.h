// Installer window controller for creating plugin installers.
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

#include <vstgui/standalone/include/helpers/appdelegate.h>
#include <vstgui/standalone/include/helpers/windowlistener.h>
#include <vstgui/standalone/include/helpers/windowcontroller.h>
#include <vstgui/lib/controls/cbuttons.h>

#include "wincontroller.h"

using namespace VSTGUI::Standalone;
using namespace VSTGUI::Standalone::Application;
namespace Uberton {
namespace Installer {


// Text button that invokes a callback when clicked.
class TextButton : public CTextButton
{
public:
	using Callback1 = std::function<void(void)>;
	using Callback2 = std::function<void(CControl*)>;
	TextButton(const CRect& size, UTF8StringPtr text);
	TextButton(const CRect& size, UTF8StringPtr text, Callback1 callback1);
	TextButton(const CRect& size, UTF8StringPtr text, Callback2 callback2);

	CMouseEventResult onMouseUp(CPoint& where, const CButtonState& buttons);

	Callback1 callback1;
	Callback2 callback2;
};


class InstallerWindowController : public GeneralWindowController
{
public:
	bool initialize(const WindowConfiguration& config, WindowPtr& window) override;
	CFrame* getFrame() const;
	const IMenuBuilder* getWindowMenuBuilder(const IWindow& window) const override;

private:
	IWindow* window{ nullptr };
	SharedPointer<CFrame> frame;
	EmptyMenuBuilder emptyMenuBuilder;
};


class Installer : public DelegateAdapter, public WindowListenerAdapter
{
public:
	Installer(UTF8String pluginName, UTF8String version) : DelegateAdapter({ pluginName, version, "com.uberton." + pluginName }) {
	}

	void finishLaunching() override;
	void onClosed(const IWindow& window) override;

private:
	void generatePages();

	void createFonts();
	void createBitmaps();

	void firstPage();
	bool nextPage();
	bool previousPage();
	bool changePage(CViewContainer* newPage);

	void finish();
	void cancel();


	using Page = CViewContainer;
	std::vector<SharedPointer<Page>> pages;
	Page* currentPage{ nullptr };
	int pageIndex{ 0 };
	SharedPointer<TextButton> button;

	InstallerWindowController* controller{ nullptr };
	WindowPtr window{ nullptr };
	CFrame* frame{ nullptr };

	SharedPointer<CFontDesc> headerFont;
	SharedPointer<CFontDesc> mainFont;
	SharedPointer<CFontDesc> buttonFont;
	SharedPointer<CGradient> buttonGradient;
	SharedPointer<CGradient> buttonGradientHighlighted;
};

}
}
