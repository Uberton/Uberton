// -----------------------------------------------------------------------------------------------------------------------------
// This file is part of the Überton project. Copyright (C) 2021 Überton
//
// Überton is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// Überton is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
// of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details. You should
// have received a copy of the GNU General Public License along with Überton. If not, see http://www.gnu.org/licenses/.
// -----------------------------------------------------------------------------------------------------------------------------

#include "ui.h"
#include <public.sdk/source/common/openurl.h>
#include "ControllerBase.h"

using namespace Uberton;


Uberton::FadingFrameAnimationButton::FadingFrameAnimationButton(const CRect& size) : CKickButton(size, nullptr, -1, nullptr) {}

void Uberton::FadingFrameAnimationButton::draw(CDrawContext* context) {
	CGraphicsTransform transform;

	if (bitmapHFlip) {
		transform.translate(-getViewSize().getTopRight());
		transform.scale(-1, 1);
		transform.translate(getViewSize().getTopRight());
		transform.translate(-getViewSize().getWidth(), 0);
	}

	CDrawContext::Transform ctxTransform(*context, transform);

	if (getDrawBackground()) {
		getDrawBackground()->draw(context, getViewSize(), { offset.x, offset.y });
	}
	context->setFrameColor(frameColor);
	context->setLineWidth(1);
	context->drawRect(getViewSize(), kDrawStroked);
	if (!getMouseEnabled()) {
		context->setFillColor({ 0, 0, 0, 100 });
		context->drawRect(getViewSize(), kDrawFilled);
	}
	setDirty(false);
}

CMouseEventResult Uberton::FadingFrameAnimationButton::onMouseDown(CPoint& where, const CButtonState& buttons) {
	setFrameColor(pressedFrameColor);
	removeAnimation("FrameOpacityAnimation"); // in case it is still running
	CKickButton::onMouseDown(where, buttons);
	return kMouseEventHandled;
}

CMouseEventResult Uberton::FadingFrameAnimationButton::onMouseUp(CPoint& where, const CButtonState& buttons) {
	addAnimation("FrameOpacityAnimation", new FadingFrameAnimation<FadingFrameAnimationButton>(0),
		new VSTGUI::Animation::CubicBezierTimingFunction(animationTimeMilliseconds, CPoint(0.4, 0), CPoint(0.6, 1)));
	return CKickButton::onMouseUp(where, buttons);
}

void Uberton::FadingFrameAnimationButton::setFrameColor(const CColor& clr) {
	if (frameColor != clr) {
		frameColor = clr;
		setDirty();
	}
}

CColor Uberton::FadingFrameAnimationButton::getFrameColor() const {
	return frameColor;
}

void Uberton::FadingFrameAnimationButton::setPressedFrameColor(const CColor& clr) {
	if (pressedFrameColor != clr) {
		pressedFrameColor = clr;
		setDirty();
	}
}

CColor Uberton::FadingFrameAnimationButton::getPressedFrameColor() const {
	return pressedFrameColor;
}

void Uberton::FadingFrameAnimationButton::setHorizontalBitmapFlip(bool flip) {
	bitmapHFlip = flip;
	setDirty();
}

bool Uberton::FadingFrameAnimationButton::isHorizontalBitmapFlip() const {
	return bitmapHFlip;
}





Uberton::HistoryButton::HistoryButton(const CRect& size) : FadingFrameAnimationButton(size) {
}

CMouseEventResult Uberton::HistoryButton::onMouseUp(CPoint& where, const CButtonState& buttons) {
	if (hitTest(where)) {
		if (callback) callback(type);
	}
	return FadingFrameAnimationButton::onMouseUp(where, buttons);
}

void Uberton::HistoryButton::setType(Type type) {
	this->type = type;
}

HistoryButton::Type Uberton::HistoryButton::getType() const {
	return type;
}

void Uberton::HistoryButton::setCallback(const std::function<void(Type)>& callback) {
	this->callback = callback;
}


Uberton::HistoryController::HistoryController(HistoryControllerBase* hController, IController* parentController, bool initialCanUndo, bool initialCanRedo)
	: DelegationController(parentController), hController(hController), initialCanUndo(initialCanUndo), initialCanRedo(initialCanRedo) {}

CView* Uberton::HistoryController::verifyView(CView* view, const UIAttributes& attributes, const IUIDescription* description) {
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

void Uberton::HistoryController::undo() {
	hController->undo();
}

void Uberton::HistoryController::redo() {
	hController->redo();
}

void Uberton::HistoryController::updateButtonState(bool canUndo, bool canRedo) {
	if (undoButton && canUndo != undoButton->getMouseEnabled()) {
		undoButton->setMouseEnabled(canUndo);
		undoButton->setDirty();
	}
	if (redoButton && canRedo != redoButton->getMouseEnabled()) {
		redoButton->setMouseEnabled(canRedo);
		redoButton->setDirty();
	}
}



Uberton::UbertonContextMenu::UbertonContextMenu() : COptionMenu(CRect(0, 0, 20, 20), nullptr, -1) {
	zoomMenu = makeOwned<COptionMenu>();
	zoomMenu->setStyle(kMultipleCheckStyle);
	initMenu();
	setFrameColor(kTransparentCColor);
	setBackColor(kBlackCColor);
	setSymbolColor(kWhiteCColor);
}

CMouseEventResult Uberton::UbertonContextMenu::onMouseDown(CPoint& where, const CButtonState& buttons) {
	setFrameColor(pressedFrameColor);
	removeAnimation("FrameOpacityAnimation"); // in case it is still running
	if (editor) {
		double zoomFactor = editor->getZoomFactor();
		auto it = std::find(zoomFactors.begin(), zoomFactors.end(), zoomFactor);
		if (it != zoomFactors.end()) {
			zoomMenu->checkEntryAlone(std::distance(zoomFactors.begin(), it));
		}
	}

	if (buttons & (kLButton | kRButton | kApple)) {
		startAnimation();
	}

	auto result = COptionMenu::onMouseDown(where, buttons);

	if (result == kMouseDownEventHandledButDontNeedMovedOrUpEvents) {
		return result;
	}
	return kMouseEventHandled; // UbertonContextMenu handled it, even if COptionMenu didn't
}

CMouseEventResult Uberton::UbertonContextMenu::onMouseUp(CPoint& where, const CButtonState& buttons) {
	startAnimation();
	return COptionMenu::onMouseUp(where, buttons);
}

void Uberton::UbertonContextMenu::draw(CDrawContext* context) {
	if (!initialized) {
		initialize();
	}

	drawBack(context);

	//auto p = getViewSize().getTopLeft();
	//auto q = getViewSize().getTopRight();
	//context->setFrameColor(symbolColor);
	//context->setLineWidth(1);
	//double dy = getHeight() / 6.0;
	//double x0 = getWidth() / 8;
	//double x1 = getWidth() - x0 - 2;
	//for (int i = 0; i < 3; i++) {
	//	context->drawLine(p + CPoint{ x0, dy * (i + 2) }, p + CPoint{ x1, dy * (i + 2) });
	//}
	setDirty(false);
}

void Uberton::UbertonContextMenu::setSymbolColor(const CColor& clr) {
	symbolColor = clr;
	setDirty();
}

CColor Uberton::UbertonContextMenu::getSymbolColor() const { return symbolColor; }

void Uberton::UbertonContextMenu::setZoomFactors(const ZoomFactors& zoomFactors) {
	this->zoomFactors = zoomFactors;
	editor = dynamic_cast<VST3Editor*>(getEditor());
	if (editor) {
		editor->setAllowedZoomFactors(zoomFactors);
	}
	initMenu();
}

UbertonContextMenu::ZoomFactors Uberton::UbertonContextMenu::getZoomFactors() const { return zoomFactors; }

void Uberton::UbertonContextMenu::setPressedFrameColor(const CColor& clr) {
	if (pressedFrameColor != clr) {
		pressedFrameColor = clr;
		setDirty();
	}
}

CColor Uberton::UbertonContextMenu::getPressedFrameColor() const {
	return pressedFrameColor;
}

void Uberton::UbertonContextMenu::initialize() {
	editor = dynamic_cast<VST3Editor*>(getEditor());
	if (editor) {
		editor->setAllowedZoomFactors(zoomFactors);
	}
	initialized = true;
}

void Uberton::UbertonContextMenu::initMenu() {
	this->removeAllEntry();
	auto addItem = [&](const UTF8String& text, int id, const UTF8String& category, COptionMenu* menu) {
		auto c = makeOwned<CCommandMenuItem>(CCommandMenuItem::Desc{ text, id, nullptr, category, text });
		c->remember();
		c->setActions([&](CCommandMenuItem* item) { itemSelected(item); }, nullptr);
		auto item = menu->addEntry(c);
		item->setTag(id);
		return item;
	};
	zoomMenu->removeAllEntry();

	addItem("User Guide", static_cast<int32_t>(MenuItemID::userGuide), "Base", this);
	auto zoomItem = addItem("Zoom", static_cast<int32_t>(MenuItemID::zoom), "Base", this);
	zoomItem->setSubmenu(zoomMenu);
	addItem("Uberton.org", static_cast<int32_t>(MenuItemID::url), "Base", this);


	char zoomFactorString[128];
	int32_t zoomFactorTag = 0;
	for (auto factor : zoomFactors) {
		sprintf(zoomFactorString, "%d%%", static_cast<int>(factor * 100));
		addItem(zoomFactorString, zoomFactorTag++, "Zoom", zoomMenu);
	}
}

void Uberton::UbertonContextMenu::startAnimation() {
	addAnimation("FrameOpacityAnimation", new FadingFrameAnimation<UbertonContextMenu>(0), new VSTGUI::Animation::CubicBezierTimingFunction(animationTimeMilliseconds, CPoint(0.4, 0), CPoint(0.6, 1)));
}

void Uberton::UbertonContextMenu::itemSelected(CCommandMenuItem* item) {
	if (item->getCommandCategory() == "Base") {
		switch (static_cast<MenuItemID>(item->getTag())) {
		case MenuItemID::userGuide:
			return;
		case MenuItemID::url:
			openURLInDefaultApplication("https://uberton.org");
			return;
		}
	}
	else if (item->getCommandCategory() == "Zoom") {
		if (item->getTag() < 0 || item->getTag() >= static_cast<ptrdiff_t>(zoomFactors.size())) return;
		if (editor) {
			editor->setZoomFactor(zoomFactors[item->getTag()]);
		}
		return;
	}
}



Uberton::DiagonalSlider::DiagonalSlider(const CRect& r) : CSliderBase(r, nullptr, -1) {
	setWantsFocus(true);
	//kAlwaysUseZoomFactor = true;
}

void Uberton::DiagonalSlider::draw(CDrawContext* context) {
	if (getDrawBackground()) {
		CRect rect(0, 0, getControlSizePrivate().x, getControlSizePrivate().y);
		rect.offset(getViewSize().left, getViewSize().top);
		getDrawBackground()->draw(context, rect);
	}
	if (handleBitmap) {
		handleBitmap->draw(context, getHandleRect());
	}
	ParamValue value = getValueNormalized();
	double x0 = p1.x + (p2.x - p1.x) * value + getViewSize().left;
	double y0 = p1.y + (p2.y - p1.y) * value + getViewSize().top;
	context->drawPoint({ x0, y0 }, kBlackCColor);
	setDirty(false);
}

CRect Uberton::DiagonalSlider::getHandleRect() const {
	CRect r;
	ParamValue value = getValueNormalized();
	double x0 = p1.x + (p2.x - p1.x) * value;
	double y0 = p1.y + (p2.y - p1.y) * value;

	r.left = x0 - handleBitmap->getWidth() / 2.0 + getOffsetHandle().x;
	r.right = x0 + handleBitmap->getWidth() / 2.0 + getOffsetHandle().x;
	r.top = y0 - handleBitmap->getHeight() / 2.0 + getOffsetHandle().y;
	r.bottom = y0 + handleBitmap->getHeight() / 2.0 + getOffsetHandle().y;
	r.offset(getViewSize().getTopLeft());
	return r;
}

void Uberton::DiagonalSlider::setHandleBitmap(CBitmap* handle) {
	handleBitmap = handle;
	setDirty();
}

CBitmap* Uberton::DiagonalSlider::getHandleBitmap() const {
	return handleBitmap;
}

void Uberton::DiagonalSlider::setPathStartPoint(const CPoint& p) {
	p1 = p;
	setDirty();
}

void Uberton::DiagonalSlider::setPathEndPoint(const CPoint& p) {
	p2 = p;
	setDirty();
}

CPoint Uberton::DiagonalSlider::getPathStartPoint() const { return p1; }

CPoint Uberton::DiagonalSlider::getPathEndPoint() const { return p2; }

CMouseEventResult Uberton::DiagonalSlider::onMouseDown(CPoint& where, const CButtonState& buttons) {
	if (!(buttons & kLButton)) return kMouseEventNotHandled;

	invalidMouseWheelEditTimer(this);
	beginEdit();
	startValue = getValue();
	startPoint = where;
	return kMouseEventHandled;
}

CMouseEventResult Uberton::DiagonalSlider::onMouseUp(CPoint& where, const CButtonState& buttons) {
	if (isEditing()) {
		endEdit();
	}
	return kMouseEventHandled;
}

CMouseEventResult Uberton::DiagonalSlider::onMouseMoved(CPoint& where, const CButtonState& buttons) {
	if (buttons.isLeftButton() && isEditing()) {

		float horizontalDistance = p2.x - p1.x;
		float factor = 5.f * horizontalDistance;

		if (buttons & kAlt) {
			factor *= getZoomFactor();
		}
		float value = startValue + (where.x - startPoint.x) / factor;
		setValueNormalized(value);

		if (isDirty()) {
			valueChanged();
			invalid();
		}
		return kMouseEventHandled;
	}
	return kMouseEventNotHandled;
}

CMouseEventResult Uberton::DiagonalSlider::onMouseCancel() {
	if (isEditing()) {
		value = startValue;
		if (isDirty()) {
			valueChanged();
			invalid();
		}
		endEdit();
	}
	return kMouseEventHandled;
}
