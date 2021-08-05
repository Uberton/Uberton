// -----------------------------------------------------------------------------------------------------------------------------
// This file is part of the Überton project. Copyright (C) 2021 Überton
//
// Überton is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// Überton is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
// of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details. You should
// have received a copy of the GNU General Public License along with Überton. If not, see http://www.gnu.org/licenses/.
// -----------------------------------------------------------------------------------------------------------------------------

#include "viewcreator.h"
#include <array>

#include "vstgui/vstgui.h"
#include "vstgui/vstgui_uidescription.h"
#include "vstgui/uidescription/detail/uiviewcreatorattributes.h"


namespace Uberton {


FadingFrameAnimationButtonFactory::FadingFrameAnimationButtonFactory() {
	UIViewFactory::registerViewCreator(*this);
}

IdStringPtr FadingFrameAnimationButtonFactory::getViewName() const { return Uberton::ViewCreator::kFadingFrameAnimationButton; }

IdStringPtr FadingFrameAnimationButtonFactory::getBaseViewName() const { return UIViewCreator::kCKickButton; }

CView* FadingFrameAnimationButtonFactory::create(const UIAttributes& attributes, const IUIDescription* description) const {
	return new Control({ 0, 0, 50, 30 });
}

bool FadingFrameAnimationButtonFactory::apply(CView* view, const UIAttributes& attributes, const IUIDescription* description) const {
	Control* control = dynamic_cast<Control*>(view);
	if (!control) return false;
	CColor color;
	if (UIViewCreator::stringToColor(attributes.getAttributeValue(pressedFrameColor), color, description))
		control->setPressedFrameColor(color);

	bool flip{ false };
	if (attributes.getBooleanAttribute(horizontalBitmapFlip, flip)) {
		control->setHorizontalBitmapFlip(flip);
	}
	return true;
}

bool FadingFrameAnimationButtonFactory::getAttributeNames(StringList& attributeNames) const {
	attributeNames.emplace_back(pressedFrameColor);
	attributeNames.emplace_back(horizontalBitmapFlip);
	return true;
}

IViewCreator::AttrType FadingFrameAnimationButtonFactory::getAttributeType(const std::string& attributeName) const {
	if (attributeName == pressedFrameColor) return kColorType;
	if (attributeName == horizontalBitmapFlip) return kBooleanType;
	return kUnknownType;
}

bool FadingFrameAnimationButtonFactory::getAttributeValue(CView* view, const string& attributeName, string& stringValue, const IUIDescription* desc) const {
	Control* control = dynamic_cast<Control*>(view);
	if (control == 0) return false;
	if (attributeName == pressedFrameColor) {
		UIViewCreator::colorToString(control->getPressedFrameColor(), stringValue, desc);
		return true;
	}
	if (attributeName == horizontalBitmapFlip) {
		stringValue = UIAttributes::boolToString(control->isHorizontalBitmapFlip());
		return true;
	}
	return false;
}

const std::string FadingFrameAnimationButtonFactory::pressedFrameColor = "pressed-frame-color";
const std::string FadingFrameAnimationButtonFactory::horizontalBitmapFlip = "horizontal-bitmap-flip";



Uberton::HistoryButtonFactory::HistoryButtonFactory() {
	UIViewFactory::registerViewCreator(*this);
}

IdStringPtr Uberton::HistoryButtonFactory::getViewName() const { return Uberton::ViewCreator::kHistoryButton; }

IdStringPtr Uberton::HistoryButtonFactory::getBaseViewName() const { return UIViewCreator::kCKickButton; }

CView* HistoryButtonFactory::create(const UIAttributes& attributes, const IUIDescription* description) const {
	return new Control({ 0, 0, 50, 30 });
}

bool HistoryButtonFactory::apply(CView* view, const UIAttributes& attributes, const IUIDescription* description) const {
	Control* control = dynamic_cast<Control*>(view);
	if (!control) return false;
	CColor color;
	if (auto str = attributes.getAttributeValue(type)) {
		if (*str == "Redo") {
			control->setType(HistoryButton::Type::Redo);
		}
		else if (*str == "Undo") {
			control->setType(HistoryButton::Type::Undo);
		}
	}
	return FadingFrameAnimationButtonFactory::apply(view, attributes, description);
}

bool HistoryButtonFactory::getAttributeNames(StringList& attributeNames) const {
	attributeNames.emplace_back(type);
	return FadingFrameAnimationButtonFactory::getAttributeNames(attributeNames);
}

IViewCreator::AttrType HistoryButtonFactory::getAttributeType(const std::string& attributeName) const {
	if (attributeName == type) return kListType;
	return FadingFrameAnimationButtonFactory::getAttributeType(attributeName);
}

std::array<std::string, 2>& HistoryButtonFactory::typeStrings() {
	static std::array<std::string, 2> strings = { "Redo", "Undo" };
	return strings;
}

bool HistoryButtonFactory::getPossibleListValues(const string& attributeName, ConstStringPtrList& list) const {
	if (attributeName == type) {
		for (auto& t : typeStrings()) {
			list.emplace_back(&t);
		}
		return true;
	}
	return false;
}

bool HistoryButtonFactory::getAttributeValue(CView* view, const string& attributeName, string& stringValue, const IUIDescription* desc) const {
	Control* control = dynamic_cast<Control*>(view);
	if (control == 0) return false;
	if (attributeName == type) {
		switch (control->getType()) {
		case HistoryButton::Type::Redo:
			stringValue = "Redo";
			return true;
		case HistoryButton::Type::Undo:
			stringValue = "Undo";
			return true;
		}
	}
	return FadingFrameAnimationButtonFactory::getAttributeValue(view, attributeName, stringValue, desc);
}

const std::string HistoryButtonFactory::type = "type";



UbertonContextMenuFactory::UbertonContextMenuFactory() {
	UIViewFactory::registerViewCreator(*this);
}

IdStringPtr UbertonContextMenuFactory::getViewName() const { return Uberton::ViewCreator::kUbertonContextMenu; }

IdStringPtr UbertonContextMenuFactory::getBaseViewName() const { return UIViewCreator::kCOptionMenu; }

CView* UbertonContextMenuFactory::create(const UIAttributes& attributes, const IUIDescription* description) const {
	return new Control();
}

bool UbertonContextMenuFactory::apply(CView* view, const UIAttributes& attributes, const IUIDescription* description) const {
	Control* control = dynamic_cast<Control*>(view);
	if (!control) return false;
	UIAttributes::StringArray value;
	if (attributes.getStringArrayAttribute(zoomFactors, value)) {
		Control::ZoomFactors zoomFactors;
		for (const auto& string : value) {
			zoomFactors.push_back(std::atof(string.c_str()) / 100.0);
		}
		control->setZoomFactors(zoomFactors);
	}
	CColor color;
	if (UIViewCreator::stringToColor(attributes.getAttributeValue(symbolColor), color, description))
		control->setSymbolColor(color);

	if (UIViewCreator::stringToColor(attributes.getAttributeValue(pressedFrameColor), color, description))
		control->setPressedFrameColor(color);

	return true;
}

bool UbertonContextMenuFactory::getAttributeNames(StringList& attributeNames) const {
	attributeNames.emplace_back(zoomFactors);
	attributeNames.emplace_back(symbolColor);
	attributeNames.emplace_back(pressedFrameColor);
	return true;
}

IViewCreator::AttrType UbertonContextMenuFactory::getAttributeType(const std::string& attributeName) const {
	if (attributeName == zoomFactors) return kStringType;
	if (attributeName == symbolColor) return kColorType;
	if (attributeName == pressedFrameColor) return kColorType;
	return kUnknownType;
}

bool UbertonContextMenuFactory::getAttributeValue(CView* view, const string& attributeName, string& stringValue, const IUIDescription* desc) const {
	Control* control = dynamic_cast<Control*>(view);
	if (control == 0) return false;
	if (attributeName == zoomFactors) {
		const auto zoomFactorsList = control->getZoomFactors();
		if (zoomFactorsList.size() == 0) {
			stringValue = "";
			return true;
		}
		UIAttributes::StringArray stringArray;

		char zoomFactorString[128];
		for (auto factor : zoomFactorsList) {
			sprintf(zoomFactorString, "%d", static_cast<int>(factor * 100));
			stringArray.emplace_back(zoomFactorString);
		}

		stringValue = UIAttributes::stringArrayToString(stringArray);
		return true;
	}
	if (attributeName == symbolColor) {
		UIViewCreator::colorToString(control->getSymbolColor(), stringValue, desc);
		return true;
	}
	if (attributeName == pressedFrameColor) {
		UIViewCreator::colorToString(control->getPressedFrameColor(), stringValue, desc);
		return true;
	}
	return false;
}

const std::string UbertonContextMenuFactory::zoomFactors = "zoom-factors";
const std::string UbertonContextMenuFactory::symbolColor = "symbol-color";
const std::string UbertonContextMenuFactory::pressedFrameColor = "pressed-frame-color";



DiagonalSliderFactory::DiagonalSliderFactory() {
	UIViewFactory::registerViewCreator(*this);
}

IdStringPtr DiagonalSliderFactory::getViewName() const { return Uberton::ViewCreator::kDiagonalSlider; }

IdStringPtr DiagonalSliderFactory::getBaseViewName() const { return UIViewCreator::kCControl; }

CView* DiagonalSliderFactory::create(const UIAttributes& attributes, const IUIDescription* description) const {
	auto slider = new DiagonalSlider({ 0, 0, 50, 30 });
	slider->setPathStartPoint({ 10, 10 });
	slider->setPathEndPoint({ 40, 20 });
	return slider;
}

bool DiagonalSliderFactory::apply(CView* view, const UIAttributes& attributes, const IUIDescription* description) const {
	Control* control = dynamic_cast<Control*>(view);
	if (!control) return false;
	CBitmap* bmp;
	if (auto* value = attributes.getAttributeValue(handleBitmap)) {
		if (UIViewCreator::stringToBitmap(value, bmp, description))
			control->setHandleBitmap(bmp);
	}

	CPoint p;
	if (attributes.getPointAttribute(pathStartPoint, p)) {
		control->setPathStartPoint(p);
	}
	if (attributes.getPointAttribute(pathEndPoint, p)) {
		control->setPathEndPoint(p);
	}
	return SliderBaseCreator::apply(view, attributes, description);
}

bool DiagonalSliderFactory::getAttributeNames(StringList& attributeNames) const {
	attributeNames.emplace_back(handleBitmap);
	attributeNames.emplace_back(pathStartPoint);
	attributeNames.emplace_back(pathEndPoint);
	return SliderBaseCreator::getAttributeNames(attributeNames);
}

IViewCreator::AttrType DiagonalSliderFactory::getAttributeType(const std::string& attributeName) const {
	if (attributeName == handleBitmap) return kBitmapType;
	if (attributeName == pathStartPoint) return kPointType;
	if (attributeName == pathEndPoint) return kPointType;
	return SliderBaseCreator::getAttributeType(attributeName);
}

bool DiagonalSliderFactory::getAttributeValue(CView* view, const string& attributeName, string& stringValue, const IUIDescription* desc) const {
	Control* control = dynamic_cast<Control*>(view);
	if (control == 0) return false;
	if (attributeName == handleBitmap) {
		if (auto bmp = control->getHandleBitmap()) {
			UIViewCreator::bitmapToString(bmp, stringValue, desc);
		}
		return true;
	}

	if (attributeName == pathStartPoint) {
		stringValue = UIAttributes::pointToString(control->getPathStartPoint());
		return true;
	}
	if (attributeName == pathEndPoint) {
		stringValue = UIAttributes::pointToString(control->getPathEndPoint());
		return true;
	}
	return SliderBaseCreator::getAttributeValue(view, attributeName, stringValue, desc);
}

const std::string DiagonalSliderFactory::handleBitmap = "handle-bitmap";
const std::string DiagonalSliderFactory::pathStartPoint = "path-start-point";
const std::string DiagonalSliderFactory::pathEndPoint = "path-end-point";



StringMapLabelFactory::StringMapLabelFactory() {
	UIViewFactory::registerViewCreator(*this);
}

IdStringPtr StringMapLabelFactory::getViewName() const { return Uberton::ViewCreator::kStringMapLabel; }

IdStringPtr StringMapLabelFactory::getBaseViewName() const { return UIViewCreator::kCParamDisplay; }

CView* StringMapLabelFactory::create(const UIAttributes& attributes, const IUIDescription* description) const {
	return new Control({ 0, 0, 50, 30 });
}

bool StringMapLabelFactory::apply(CView* view, const UIAttributes& attributes, const IUIDescription* description) const {
	Control* control = dynamic_cast<Control*>(view);
	if (!control) return false;
	UIAttributes::StringArray value;
	if (attributes.getStringArrayAttribute(names, value)) {
		std::vector<std::string> receivedNames;
		for (const auto& string : value) {
			receivedNames.push_back(string);
		}
		control->setNames(receivedNames);
	}
	return true;
}

bool StringMapLabelFactory::getAttributeNames(StringList& attributeNames) const {
	attributeNames.emplace_back(names);
	return true;
}

IViewCreator::AttrType StringMapLabelFactory::getAttributeType(const std::string& attributeName) const {
	if (attributeName == names) return kStringType;
	return kUnknownType;
}

bool StringMapLabelFactory::getAttributeValue(CView* view, const string& attributeName, string& stringValue, const IUIDescription* desc) const {
	Control* control = dynamic_cast<Control*>(view);
	if (control == 0) return false;
	if (attributeName == names) {
		const auto nameList = control->getNames();
		if (nameList.size() == 0) {
			stringValue = "";
			return true;
		}
		UIAttributes::StringArray stringArray;

		for (auto name : nameList) {
			stringArray.emplace_back(name);
		}

		stringValue = UIAttributes::stringArrayToString(stringArray);
		return true;
	}
	return false;
}

const std::string StringMapLabelFactory::names = "names";



TextEditUnitsFactory::TextEditUnitsFactory() {
	UIViewFactory::registerViewCreator(*this);
}

IdStringPtr TextEditUnitsFactory::getViewName() const { return Uberton::ViewCreator::kTextEditUnits; }

IdStringPtr TextEditUnitsFactory::getBaseViewName() const { return UIViewCreator::kCTextEdit; }

CView* TextEditUnitsFactory::create(const UIAttributes& attributes, const IUIDescription* description) const {
	return new Control({ 0, 0, 50, 30 });
}

bool TextEditUnitsFactory::apply(CView* view, const UIAttributes& attributes, const IUIDescription* description) const {
	Control* control = dynamic_cast<Control*>(view);
	if (!control) return false;
	if (auto* s = attributes.getAttributeValue(units)) {
		control->setUnits(*s);
	}
	return true;
}

bool TextEditUnitsFactory::getAttributeNames(StringList& attributeNames) const {
	attributeNames.emplace_back(units);
	return true;
}

IViewCreator::AttrType TextEditUnitsFactory::getAttributeType(const std::string& attributeName) const {
	if (attributeName == units) return kStringType;
	return kUnknownType;
}

bool TextEditUnitsFactory::getAttributeValue(CView* view, const string& attributeName, string& stringValue, const IUIDescription* desc) const {
	Control* control = dynamic_cast<Control*>(view);
	if (control == 0) return false;
	if (attributeName == units) {
		stringValue = control->getUnits();
		return true;
	}
	return false;
}

const std::string TextEditUnitsFactory::units = "units";


}
