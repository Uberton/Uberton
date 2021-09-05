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
#include <filesystem>

#ifdef _WIN32
#include <ShlObj.h>
#else
#include <cstdlib>
#endif

using namespace Uberton;


FadingFrameAnimationButton::FadingFrameAnimationButton(const CRect& size) : CKickButton(size, nullptr, -1, nullptr) {}

void FadingFrameAnimationButton::draw(CDrawContext* context) {
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
	context->setLineWidth(2);
	CRect c = getViewSize();
	c.left++;
	c.right--;
	c.top++;
	c.bottom--;
	context->drawRect(c, kDrawStroked);
	if (!getMouseEnabled()) {
		context->setFillColor({ 0, 0, 0, 100 });
		context->drawRect(getViewSize(), kDrawFilled);
	}
	setDirty(false);
}

CMouseEventResult FadingFrameAnimationButton::onMouseDown(CPoint& where, const CButtonState& buttons) {
	setFrameColor(pressedFrameColor);
	removeAnimation("FrameOpacityAnimation"); // in case it is still running
	CKickButton::onMouseDown(where, buttons);
	return kMouseEventHandled;
}

CMouseEventResult FadingFrameAnimationButton::onMouseUp(CPoint& where, const CButtonState& buttons) {
	addAnimation("FrameOpacityAnimation", new FadingFrameAnimation<FadingFrameAnimationButton>(0),
		new VSTGUI::Animation::CubicBezierTimingFunction(animationTimeMilliseconds, CPoint(0.4, 0), CPoint(0.6, 1)));
	return CKickButton::onMouseUp(where, buttons);
}

void FadingFrameAnimationButton::setFrameColor(const CColor& clr) {
	if (frameColor != clr) {
		frameColor = clr;
		setDirty();
	}
}

CColor FadingFrameAnimationButton::getFrameColor() const {
	return frameColor;
}

void FadingFrameAnimationButton::setPressedFrameColor(const CColor& clr) {
	if (pressedFrameColor != clr) {
		pressedFrameColor = clr;
		setDirty();
	}
}

CColor FadingFrameAnimationButton::getPressedFrameColor() const {
	return pressedFrameColor;
}

void FadingFrameAnimationButton::setHorizontalBitmapFlip(bool flip) {
	bitmapHFlip = flip;
	setDirty();
}

bool FadingFrameAnimationButton::isHorizontalBitmapFlip() const {
	return bitmapHFlip;
}





HistoryButton::HistoryButton(const CRect& size) : FadingFrameAnimationButton(size) {
}

CMouseEventResult HistoryButton::onMouseUp(CPoint& where, const CButtonState& buttons) {
	if (hitTest(where)) {
		if (callback) callback(type);
	}
	return FadingFrameAnimationButton::onMouseUp(where, buttons);
}

void HistoryButton::setType(Type type) {
	this->type = type;
}

HistoryButton::Type HistoryButton::getType() const {
	return type;
}

void HistoryButton::setCallback(const std::function<void(Type)>& callback) {
	this->callback = callback;
}



UbertonContextMenu::UbertonContextMenu() : COptionMenu(CRect(0, 0, 20, 20), nullptr, -1) {
	zoomMenu = makeOwned<COptionMenu>();
	zoomMenu->setStyle(kMultipleCheckStyle);
	initMenu();
	setFrameColor(kTransparentCColor);
	setBackColor(kBlackCColor);
	setSymbolColor(kWhiteCColor);
}

CMouseEventResult UbertonContextMenu::onMouseDown(CPoint& where, const CButtonState& buttons) {
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

CMouseEventResult UbertonContextMenu::onMouseUp(CPoint& where, const CButtonState& buttons) {
	startAnimation();
	return COptionMenu::onMouseUp(where, buttons);
}

void UbertonContextMenu::draw(CDrawContext* context) {
	if (!initialized) {
		initialize();
	}

	drawBack(context);

	context->setFrameColor(frameColor);
	context->setLineWidth(2);
	CRect c = getViewSize();
	c.left++;
	c.right--;
	c.top++;
	c.bottom--;
	context->drawRect(c, kDrawStroked);
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

void UbertonContextMenu::setSymbolColor(const CColor& clr) {
	symbolColor = clr;
	setDirty();
}

CColor UbertonContextMenu::getSymbolColor() const { return symbolColor; }

void UbertonContextMenu::setZoomFactors(const ZoomFactors& zoomFactors) {
	this->zoomFactors = zoomFactors;
	editor = dynamic_cast<TheEditor*>(getEditor());
	if (editor) {
		editor->setAllowedZoomFactors(zoomFactors);
	}
	initMenu();
}

UbertonContextMenu::ZoomFactors UbertonContextMenu::getZoomFactors() const { return zoomFactors; }

void UbertonContextMenu::setPressedFrameColor(const CColor& clr) {
	if (pressedFrameColor != clr) {
		pressedFrameColor = clr;
		setDirty();
	}
}

CColor UbertonContextMenu::getPressedFrameColor() const {
	return pressedFrameColor;
}

void UbertonContextMenu::initialize() {
	editor = dynamic_cast<TheEditor*>(getEditor());
	if (editor) {
		ZoomFactors scaledZoomFactors;
		for (const auto& zoomFactor : zoomFactors) {
			scaledZoomFactors.push_back(zoomFactor);
		}
		editor->setAllowedZoomFactors(zoomFactors);
	}
	initialized = true;
}

void UbertonContextMenu::initMenu() {
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

void UbertonContextMenu::startAnimation() {
	addAnimation("FrameOpacityAnimation", new FadingFrameAnimation<UbertonContextMenu>(0), new VSTGUI::Animation::CubicBezierTimingFunction(animationTimeMilliseconds, CPoint(0.4, 0), CPoint(0.6, 1)));
}

void UbertonContextMenu::itemSelected(CCommandMenuItem* item) {
	if (item->getCommandCategory() == "Base") {
		switch (static_cast<MenuItemID>(item->getTag())) {
		case MenuItemID::userGuide:
			openUserguide();
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

bool UbertonContextMenu::openUserguide() {
	if (!editor) return false;
	return editor->openUserguide();
}

DiagonalSlider::DiagonalSlider(const CRect& r) : CSliderBase(r, nullptr, -1) {
	setWantsFocus(true);
}

void DiagonalSlider::draw(CDrawContext* context) {
	{
		CGraphicsTransform transform;

		if (bitmapHFlip) {
			transform.translate(-getViewSize().getTopRight());
			transform.scale(-1, 1);
			transform.translate(getViewSize().getTopRight());
			transform.translate(-getViewSize().getWidth(), 0);
		}
		if (bitmapVFlip) {
			transform.translate(-getViewSize().getTopRight());
			transform.scale(1, -1);
			transform.translate(getViewSize().getTopRight());
			transform.translate(0, getViewSize().getHeight());
		}

		CDrawContext::Transform ctxTransform(*context, transform);

		if (getDrawBackground()) {
			CRect rect(0, 0, getControlSizePrivate().x, getControlSizePrivate().y);
			rect.offset(getViewSize().left, getViewSize().top);
			getDrawBackground()->draw(context, rect);
		}
	}
	if (handleBitmap) {
		handleBitmap->draw(context, getHandleRect());
	}
	setDirty(false);
}

CRect DiagonalSlider::getHandleRect() const {
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

void DiagonalSlider::setHandleBitmap(CBitmap* handle) {
	handleBitmap = handle;
	setDirty();
}

CBitmap* DiagonalSlider::getHandleBitmap() const {
	return handleBitmap;
}

void DiagonalSlider::setPathStartPoint(const CPoint& p) {
	p1 = p;
	setDirty();
}

void DiagonalSlider::setPathEndPoint(const CPoint& p) {
	p2 = p;
	setDirty();
}

CPoint DiagonalSlider::getPathStartPoint() const { return p1; }

CPoint DiagonalSlider::getPathEndPoint() const { return p2; }

void DiagonalSlider::setHorizontalBitmapFlip(bool flip) {
	if (flip == bitmapHFlip) return;
	bitmapHFlip = flip;
	setDirty();
}

bool DiagonalSlider::isHorizontalBitmapFlip() const {
	return bitmapHFlip;
}

void DiagonalSlider::setVerticalBitmapFlip(bool flip) {
	if (flip == bitmapVFlip) return;
	bitmapVFlip = flip;
	setDirty();
}

bool DiagonalSlider::isVerticalBitmapFlip() const {
	return bitmapVFlip;
}

CMouseEventResult DiagonalSlider::onMouseDown(CPoint& where, const CButtonState& buttons) {
	if (!(buttons & kLButton)) return kMouseEventNotHandled;

	invalidMouseWheelEditTimer(this);


	if (checkDefaultValue(buttons)) {
		endEdit();
		return kMouseDownEventHandledButDontNeedMovedOrUpEvents;
	}
	beginEdit();
	startValue = getValue();
	startPoint = where;
	return kMouseEventHandled;
}

CMouseEventResult DiagonalSlider::onMouseUp(CPoint& where, const CButtonState& buttons) {
	if (isEditing()) {
		endEdit();
	}
	return kMouseEventHandled;
}

CMouseEventResult DiagonalSlider::onMouseMoved(CPoint& where, const CButtonState& buttons) {
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

CMouseEventResult DiagonalSlider::onMouseCancel() {
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

StringMapLabel::StringMapLabel(const CRect& size) : CParamDisplay(size) {
	setValueToStringFunction2([&](float value, std::string& result, CParamDisplay* display) {
		int discreteValue = std::floor<int>(value);
		if (discreteValue < 0 || discreteValue >= static_cast<ptrdiff_t>(names.size())) return false;
		result = names[discreteValue];
		return true;
	});
}

void StringMapLabel::setNames(const std::vector<std::string>& names) {
	this->names = names;
	setDirty();
}

std::vector<std::string> StringMapLabel::getNames() const {
	return names;
}

void StringMapLabel::draw(CDrawContext* context) {
	if (!initialized) {
		setValueToStringFunction2([&](float value, std::string& result, CParamDisplay* display) {
			int discreteValue = std::floor<int>(value) - getMin();
			if (discreteValue < 0 || discreteValue >= static_cast<ptrdiff_t>(names.size())) return false;
			result = names[discreteValue];
			return true;
		});
		initialized = true;
	}
	CParamDisplay::draw(context);
}



TextEditUnits::TextEditUnits(const CRect& size) : CTextEdit(size, nullptr, -1) {
}

void TextEditUnits::draw(CDrawContext* context) {
	if (!text.empty() && tag != -1 && !units.empty()) {
		std::string tmp = text;
		text += "" + units;
		CTextEdit::draw(context);
		text = tmp;
	}
	else {
		CTextEdit::draw(context);
	}
}

void TextEditUnits::setUnits(std::string units) {
	this->units = units;
	setDirty();
}

std::string TextEditUnits::getUnits() const {
	return units;
}

LogVUMeter::LogVUMeter(const CRect& size) : CVuMeter(size, nullptr, nullptr, 100) {
}

void LogVUMeter::draw(CDrawContext* context) {
	if (!getOnBitmap())
		return;

	CRect _rectOn(rectOn);
	CRect _rectOff(rectOff);
	CPoint pointOn;
	CPoint pointOff;

	bounceValue();

	float newValue = getOldValue() / std::pow(10, decreaseValue);
	if (newValue < value)
		newValue = value;
	setOldValue(newValue);

	newValue = (newValue - getMin()) / getRange(); // normalize

	double overhead = maxDb;
	double normalizedMax = std::pow(10, overhead / 20.0);
	double logValue = 20 * std::log10(newValue * normalizedMax);
	const double range = maxDb - minDb;
	double nomalizedLogValue = (std::max(logValue, minDb) - minDb) / range;

	if (style & kHorizontal) {
		auto tmp = (CCoord)(((int32_t)(nbLed * nomalizedLogValue + 0.5f) / (float)nbLed) * getOnBitmap()->getWidth());
		pointOff(tmp, 0);

		_rectOff.left += tmp;
		_rectOn.right = tmp + rectOn.left;
	}
	else {
		auto tmp = (CCoord)(((int32_t)(nbLed * (1.f - nomalizedLogValue) + 0.5f) / (float)nbLed) * getOnBitmap()->getHeight());
		pointOn(0, tmp);

		_rectOff.bottom = tmp + rectOff.top;
		_rectOn.top += tmp;
	}

	if (getOffBitmap()) {
		getOffBitmap()->draw(context, _rectOff, pointOff);
	}

	getOnBitmap()->draw(context, _rectOn, pointOn);

	setDirty(false);
}

void LogVUMeter::setDezibelMin(double dezibelMin) {
	this->minDb = dezibelMin;
}

double LogVUMeter::getDezibelMin() const {
	return minDb;
}

void LogVUMeter::setDezibelMax(double dezibelMax) {
	this->maxDb = dezibelMax;
}

double Uberton::LogVUMeter::getDezibelMax() const {
	return maxDb;
}

LinkButton::LinkButton(const CRect& size) : COnOffButton(size, nullptr, -1) {
}

void LinkButton::draw(CDrawContext* context) {
	if (value != oldValue) {
		valueChanged();
		oldValue = value;
	}
	COnOffButton::draw(context);
}

VST3EditorEx1::VST3EditorEx1(Steinberg::Vst::EditController* controller, UTF8StringPtr templateName, UTF8StringPtr xmlFile)
	: VST3Editor(controller, templateName, xmlFile) {
}

Steinberg::tresult PLUGIN_API VST3EditorEx1::setContentScaleFactor(Steinberg::IPlugViewContentScaleSupport::ScaleFactor factor) {
	actualContentScaleFactor = factor;
	return VST3Editor::setContentScaleFactor(factor *prescaleFactor);
}

void VST3EditorEx1::setPrescaleFactor(double f) {
	if (prescaleFactor == f) return;
	prescaleFactor = f;

	setContentScaleFactor(actualContentScaleFactor);
	if (getFrame()) {
		getFrame()->setZoom(getAbsScaleFactor());
	}
}

double VST3EditorEx1::getPrescaleFactor() { return prescaleFactor; }

//double Uberton::VST3EditorEx1::getAbsScaleFactor() const {
//	return zoomFactor * contentScaleFactor * prescaleFactor;
//}


bool VST3EditorEx1::openUserguide() {
	if (userGuidePath.empty()) return false;

	std::string path = getUbertonLocation() + userGuidePath;
	if (std::filesystem::exists(path)) {
		return openURLInDefaultApplication(path.c_str());
	}
	else {
		return openURLInDefaultApplication(("https://uberton.org/plugins/" + userGuidePath).c_str());
	}
}

void Uberton::VST3EditorEx1::setUserguidePath(const std::string& userGuidePath) {
	this->userGuidePath = userGuidePath;
}

std::string Uberton::VST3EditorEx1::getUbertonLocation() {
#ifdef _WIN32
	char path[MAX_PATH];
	SHGetSpecialFolderPathA(0, path, CSIDL_PROGRAM_FILES, FALSE);
	std::string pathS{ path };
	return pathS + "/Uberton/";
#elif __APPLE__
	return "Applications/Uberton/";
#endif
}
