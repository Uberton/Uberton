// -----------------------------------------------------------------------------------------------------------------------------
// This file is part of the Überton project. Copyright (C) 2021 Überton
//
// Überton is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// Überton is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
// of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details. You should
// have received a copy of the GNU General Public License along with Überton. If not, see http://www.gnu.org/licenses/.
// -----------------------------------------------------------------------------------------------------------------------------


#include "installer.h"
#include "terms_and_conditions.h"
#include "locations.h"
#include <vstgui/lib/controls/cscrollbar.h>
#include <vstgui/lib/cscrollview.h>
#include <public.sdk/source/common/openurl.h>

namespace Uberton {
namespace Installer {

CMouseEventResult TextButton::onMouseUp(CPoint& where, const CButtonState& buttons) {
	if (hitTest(where)) {
		if (callback1) callback1();
		if (callback2) callback2(this);
	}
	return CTextButton::onMouseUp(where, buttons);
}

TextButton::TextButton(const CRect& size, UTF8StringPtr text) : CTextButton(size, nullptr, -1, title) {
}

TextButton::TextButton(const CRect& size, UTF8StringPtr text, Callback1 callback1) : CTextButton(size, nullptr, -1, text) {
	this->callback1 = callback1;
}

TextButton::TextButton(const CRect& size, UTF8StringPtr text, Callback2 callback2) : CTextButton(size, nullptr, -1, text) {
	this->callback2 = callback2;
}

bool Uberton::Installer::InstallerWindowController::initialize(const WindowConfiguration& config, WindowPtr& window) {
	this->window = window.get();
	frame = makeOwned<CFrame>(CRect(), nullptr);
	frame->setTransparency(true);

	CPoint size{ 200, 300 };
	frame->setSize(size.x, size.y);

	window->setContentView(frame);
	window->setSize(size);
	return true;
}

CFrame* Uberton::Installer::InstallerWindowController::getFrame() const {
	return frame.get();
}

const IMenuBuilder* Uberton::Installer::InstallerWindowController::getWindowMenuBuilder(const IWindow& window) const {
	return &emptyMenuBuilder;
}


void Installer::finishLaunching() {
	WindowConfiguration config;
	config.title = getInfo().name;
	config.style.border().close().centered();
	config.size = { 600, 400 };

	auto [window, controller] = makeWindow<InstallerWindowController>(config);
	if (window) {
		window->show();
		window->setSize(config.size);
		window->registerWindowListener(this);
		this->window = window;
		this->controller = controller;

		frame = controller->getFrame();
		frame->setBackgroundColorDrawStyle(kDrawFilled);
		frame->setBackgroundColor(kWhiteCColor);

		generatePages();
		firstPage();
		//nextPage();
		//doInstall();
	}
	else {
		IApplication::instance().quit();
	}
}

void Installer::generatePages() {
	createFonts();
	createBitmaps();
	auto addPage = [&]() {
		auto page = new CViewContainer(frame->getViewSize());
		pages.push_back(page);
		frame->addView(page);
		page->setVisible(false);
		page->setBackgroundColor(CColor(24, 24, 24));
		return page;
	};
	CPoint buttonSize{ 80, 32 };
	CCoord marginBetween = 10;
	CCoord rightMargin = 8;
	CCoord bottomMargin = 5;


	buttonGradient = owned(CGradient::create(0, 1, CColor(251, 251, 251), CColor(231, 231, 240)));
	buttonGradientHighlighted = owned(CGradient::create(0, 1, CColor(85, 85, 104), CColor(85, 85, 85)));

	std::string a;
	a = getVSTLocation();
	a = getFactoryPresetLocation();
	a = getUserguideLocation();

	CPoint button1Position{ frame->getWidth() - buttonSize.x - rightMargin, frame->getHeight() - buttonSize.y - bottomMargin };
	CRect button1Rect{ button1Position, buttonSize };

	CPoint button2Position{ button1Position.x - buttonSize.x - marginBetween, button1Position.y };
	CRect button2Rect{ button2Position, buttonSize };

	CRect headerTextRect{ 50, 20, frame->getWidth() - 20, 100 };
	CRect mainTextRect{ headerTextRect.left, headerTextRect.bottom, headerTextRect.right, headerTextRect.bottom + 200 };


	auto makeButton = [&](CViewContainer* parent, const CRect& rect, UTF8StringPtr text, const TextButton::Callback2& callback) {
		auto button = new TextButton(rect, text, callback);
		parent->addView(button);
		button->setRoundRadius(2);
		button->setFont(buttonFont);
		button->setGradient(buttonGradient);
		button->setGradientHighlighted(buttonGradientHighlighted);
		return button;
	};

	auto makeHeader = [&](CViewContainer* parent, UTF8StringPtr text) {
		auto label = new CMultiLineTextLabel{ headerTextRect };
		parent->addView(label);
		label->setText(text);
		label->setFont(headerFont);
		label->setBackColor(kTransparentCColor);
		label->setFrameColor(kTransparentCColor);
		return label;
	};
	auto makeMainText = [&](CViewContainer* parent, UTF8StringPtr text, const CRect& rect) {
		auto label = new CMultiLineTextLabel{ rect };
		parent->addView(label);
		label->setText(text);
		label->setFont(mainFont);
		label->setBackColor(kTransparentCColor);
		label->setFrameColor(kTransparentCColor);
		label->setLineLayout(CMultiLineTextLabel::LineLayout::wrap);
		label->setHoriAlign(CHoriTxtAlign::kLeftText);
		return label;
	};
	auto makeMainTextA = [&](CViewContainer* parent, UTF8StringPtr text) {
		return makeMainText(parent, text, mainTextRect);
	};

	auto page = addPage();
	makeButton(page, button1Rect, "Next", [&](CControl* p) { nextPage(); });
	makeButton(page, button2Rect, "Cancel", [&](CControl* p) { cancel(); });
	makeHeader(page, "Welcome to the " + getInfo().name + "\nSetup Wizard");
	makeMainTextA(page, "This wizard will now install the Tesseract plugin as well as the factory presets. \nPress NEXT to proceed installing the plugin or CANCEL to abort the setup. \n\nWARNING: Tesseract is based on unrestrained strong resonances and can produce high volumes or distortions. \n\nPlease read through the manual (accessible from the plugins menu) before using this plugin. Always use a limiter after Tesseract (either the built-in one or a third-party plugin). \n\nYou can visit the webpage uberton.org for further reference or contact us via mail@uberton.org if you encounter any issues, want to give feedback, or get in touch with the developers.");



	auto licensePage = addPage();
	makeButton(licensePage, button1Rect, "Accept", [&](CControl* p) { nextPage(); });
	makeButton(licensePage, button2Rect, "Back", [&](CControl* p) { previousPage(); });
	makeHeader(licensePage, "License Agreement");
	CScrollView* scrollView = new CScrollView(mainTextRect.extend(-20, 50), { 0, 0, 500, 500 }, CScrollView::CScrollViewStyle::kVerticalScrollbar, 12);
	licensePage->addView(scrollView);
	scrollView->setBackgroundColor(CColor(40, 40, 40));
	scrollView->getVerticalScrollbar()->setWheelInc(.02);
	scrollView->getVerticalScrollbar()->setScrollerColor(CColor(130, 130, 130));
	scrollView->getVerticalScrollbar()->setScrollerColor(CColor(130, 130, 130));

	float viewPadding = 7;
	auto licenseText = makeMainText(scrollView, TERMS_AND_CONDITIONS, { viewPadding, viewPadding, mainTextRect.getWidth() - 2 * viewPadding - 5, mainTextRect.getHeight() - 2 * viewPadding });
	licenseText->setAutoHeight(true);

	scrollView->setContainerSize({ viewPadding, viewPadding, viewPadding + scrollView->getWidth(), licenseText->getHeight() - viewPadding });

	page = addPage();
	makeButton(page, button1Rect, "Install", [&](CControl* p) { doInstall(); });
	makeButton(page, button2Rect, "Back", [&](CControl* p) { previousPage(); });
	makeHeader(page, "Setup Prepared");
	makeMainTextA(page, "Click Install to proceed. ");


	page = addPage();
	makeButton(page, button1Rect, "Finish", [&](CControl* p) { finish(); });
	makeHeader(page, "Installation finished");
	resultLabel = makeMainTextA(page, "Installation results");
	//makeMainText(page, "Do you want to open the user guide now?", { mainTextRect.left, mainTextRect.bottom, mainTextRect.right, mainTextRect.bottom + 30 });
	//makeButton(page, {}, "Open Manual", [&](CControl* p) { Steinberg::openURLInDefaultApplication(""); });
}

void Installer::createFonts() {
	headerFont = makeOwned<CFontDesc>("Arial", 16, CTxtFace::kBoldFace);
	mainFont = makeOwned<CFontDesc>("Arial", 12);
	buttonFont = makeOwned<CFontDesc>("Arial", 14, CTxtFace::kBoldFace);
}

void Installer::createBitmaps() {
}

void Installer::firstPage() {
	if (pages.size() == 0) return;
	changePage(pages[0]);
}

bool Installer::nextPage() {
	if (pageIndex < pages.size() - 1) {
		return changePage(pages[++pageIndex]);
	}
	return false;
}

bool Installer::previousPage() {
	if (pageIndex > 0) {
		return changePage(pages[--pageIndex]);
	}
	return false;
}

bool Installer::changePage(CViewContainer* newPage) {
	if (!newPage) return false;

	//if (currentPage != nullptr)
	//frame->removeView(currentPage);

	//frame->addView(newPage);
	if (currentPage != nullptr)
		currentPage->setVisible(false);
	currentPage = newPage;
	currentPage->setVisible(true);

	return true;
}

void Installer::doInstall() {
	nextPage();
	std::string result;
	if (installCallback) installCallback(result);
	resultLabel->setText(UTF8String(result));
}

void Installer::finish() {
	IApplication::instance().quit();
}

void Installer::cancel() {
	IApplication::instance().quit();
}

void Installer::onClosed(const IWindow& window) {
	IApplication::instance().quit();
}


}
}