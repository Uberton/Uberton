//// -----------------------------------------------------------------------------------------------------------------------------
//// This file is part of the Überton project. Copyright (C) 2021 Überton
////
//// Überton is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License
//// as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
//// Überton is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
//// of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details. You should
//// have received a copy of the GNU General Public License along with Überton. If not, see http://www.gnu.org/licenses/.
//// -----------------------------------------------------------------------------------------------------------------------------
//
//
//#pragma once
//#include "gui.h"
//#include <vstgui/vstgui_uidescription.h>
//#include <vstgui/uidescription/detail/uiviewcreatorattributes.h>
//#include <vstgui/uidescription/uiviewcreator.h>
//#include <vstgui/uidescription/viewcreator/sliderviewcreator.h>
//#include <array>
//#include <ui.h>
////#include <viewcreator.h>
//
//namespace Uberton {
//namespace ManualGui {
//
//
//
////FadingFrameAnimationButtonFactory __gFadingFrameAnimationButtonFactory;
//
//class UbertonContextMenuFactory : public ViewCreatorAdapter
//{
//	using Control = UbertonContextMenu;
//
//	static const std::string zoomFactors;
//	static const std::string symbolColor;
//	static const std::string pressedFrameColor;
//
//public:
//	UbertonContextMenuFactory() {
//		UIViewFactory::registerViewCreator(*this);
//	}
//
//	IdStringPtr getViewName() const override { return "Uberton Context Menu"; }
//	IdStringPtr getBaseViewName() const override { return UIViewCreator::kCOptionMenu; }
//
//	CView* create(const UIAttributes& attributes, const IUIDescription* description) const override {
//		return new Control();
//	}
//
//	bool apply(CView* view, const UIAttributes& attributes, const IUIDescription* description) const override {
//		Control* control = dynamic_cast<Control*>(view);
//		if (!control) return false;
//		UIAttributes::StringArray value;
//		if (attributes.getStringArrayAttribute(zoomFactors, value)) {
//			Control::ZoomFactors zoomFactors;
//			for (const auto& string : value) {
//				zoomFactors.push_back(std::atof(string.c_str()) / 100.0);
//			}
//			control->setZoomFactors(zoomFactors);
//		}
//		CColor color;
//		if (UIViewCreator::stringToColor(attributes.getAttributeValue(symbolColor), color, description))
//			control->setSymbolColor(color);
//
//		if (UIViewCreator::stringToColor(attributes.getAttributeValue(pressedFrameColor), color, description))
//			control->setPressedFrameColor(color);
//
//		return true;
//	}
//
//	bool getAttributeNames(StringList& attributeNames) const override {
//		attributeNames.emplace_back(zoomFactors);
//		attributeNames.emplace_back(symbolColor);
//		attributeNames.emplace_back(pressedFrameColor);
//		return true;
//	}
//
//	AttrType getAttributeType(const std::string& attributeName) const override {
//		if (attributeName == zoomFactors) return kStringType;
//		if (attributeName == symbolColor) return kColorType;
//		if (attributeName == pressedFrameColor) return kColorType;
//		return kUnknownType;
//	}
//
//	bool getAttributeValue(CView* view, const string& attributeName, string& stringValue, const IUIDescription* desc) const override {
//		Control* control = dynamic_cast<Control*>(view);
//		if (control == 0) return false;
//		if (attributeName == zoomFactors) {
//			const auto zoomFactorsList = control->getZoomFactors();
//			if (zoomFactorsList.size() == 0) {
//				stringValue = "";
//				return true;
//			}
//			UIAttributes::StringArray stringArray;
//
//			char zoomFactorString[128];
//			for (auto factor : zoomFactorsList) {
//				sprintf(zoomFactorString, "%d", static_cast<int>(factor * 100));
//				stringArray.emplace_back(zoomFactorString);
//			}
//
//			stringValue = UIAttributes::stringArrayToString(stringArray);
//			return true;
//		}
//		if (attributeName == symbolColor) {
//			UIViewCreator::colorToString(control->getSymbolColor(), stringValue, desc);
//			return true;
//		}
//		if (attributeName == pressedFrameColor) {
//			UIViewCreator::colorToString(control->getPressedFrameColor(), stringValue, desc);
//			return true;
//		}
//		return false;
//	}
//};
//
//UbertonContextMenuFactory __gUbertonContextMenuFactory;
//const std::string UbertonContextMenuFactory::zoomFactors = "zoom-factors";
//const std::string UbertonContextMenuFactory::symbolColor = "symbol-color";
//const std::string UbertonContextMenuFactory::pressedFrameColor = "pressed-frame-color";
//
//
//class UbertonButtonFactory : public ViewCreatorAdapter
//{
//	using Control = UbertonButton;
//
//	static const std::string pressedFrameColor;
//	static const std::string horizontalBitmapFlip;
//
//public:
//	UbertonButtonFactory() {
//		UIViewFactory::registerViewCreator(*this);
//	}
//
//	IdStringPtr getViewName() const override { return "Uberton Button"; }
//	IdStringPtr getBaseViewName() const override { return UIViewCreator::kCKickButton; }
//
//	CView* create(const UIAttributes& attributes, const IUIDescription* description) const override {
//		return new Control({ 0, 0, 50, 30 });
//	}
//
//	bool apply(CView* view, const UIAttributes& attributes, const IUIDescription* description) const override {
//		Control* control = dynamic_cast<Control*>(view);
//		if (!control) return false;
//		CColor color;
//		if (UIViewCreator::stringToColor(attributes.getAttributeValue(pressedFrameColor), color, description))
//			control->setPressedFrameColor(color);
//
//		bool flip{ false };
//		if (attributes.getBooleanAttribute(horizontalBitmapFlip, flip)) {
//			control->setHorizontalBitmapFlip(flip);
//		}
//		return true;
//	}
//
//	bool getAttributeNames(StringList& attributeNames) const override {
//		attributeNames.emplace_back(pressedFrameColor);
//		attributeNames.emplace_back(horizontalBitmapFlip);
//		return true;
//	}
//
//	AttrType getAttributeType(const std::string& attributeName) const override {
//		if (attributeName == pressedFrameColor) return kColorType;
//		if (attributeName == horizontalBitmapFlip) return kBooleanType;
//		return kUnknownType;
//	}
//
//	bool getAttributeValue(CView* view, const string& attributeName, string& stringValue, const IUIDescription* desc) const override {
//		Control* control = dynamic_cast<Control*>(view);
//		if (control == 0) return false;
//		if (attributeName == pressedFrameColor) {
//			UIViewCreator::colorToString(control->getPressedFrameColor(), stringValue, desc);
//			return true;
//		}
//		if (attributeName == horizontalBitmapFlip) {
//			stringValue = UIAttributes::boolToString(control->isHorizontalBitmapFlip());
//			return true;
//		}
//		return false;
//	}
//};
//
//UbertonButtonFactory __gUbertonButtonFactory;
//const std::string UbertonButtonFactory::pressedFrameColor = "pressed-frame-color";
//const std::string UbertonButtonFactory::horizontalBitmapFlip = "horizontal-bitmap-flip";
//
//
//
//class HistoryButtonFactory : public UbertonButtonFactory
//{
//	using Control = HistoryButton;
//	static const std::string type;
//
//public:
//	HistoryButtonFactory() {
//		UIViewFactory::registerViewCreator(*this);
//	}
//
//	IdStringPtr getViewName() const override { return "History Button"; }
//	IdStringPtr getBaseViewName() const override { return UIViewCreator::kCKickButton; }
//
//	CView* create(const UIAttributes& attributes, const IUIDescription* description) const override {
//		return new Control({ 0, 0, 50, 30 });
//	}
//
//	bool apply(CView* view, const UIAttributes& attributes, const IUIDescription* description) const override {
//		Control* control = dynamic_cast<Control*>(view);
//		if (!control) return false;
//		CColor color;
//		if (auto str = attributes.getAttributeValue(type)) {
//			if (*str == "Redo") {
//				control->setType(HistoryButton::Type::Redo);
//			}
//			else if (*str == "Undo") {
//				control->setType(HistoryButton::Type::Undo);
//			}
//		}
//		return UbertonButtonFactory::apply(view, attributes, description);
//	}
//
//	bool getAttributeNames(StringList& attributeNames) const override {
//		attributeNames.emplace_back(type);
//		return UbertonButtonFactory::getAttributeNames(attributeNames);
//	}
//
//	AttrType getAttributeType(const std::string& attributeName) const override {
//		if (attributeName == type) return kListType;
//		return UbertonButtonFactory::getAttributeType(attributeName);
//	}
//
//	static std::array<std::string, 2>& typeStrings() {
//		static std::array<std::string, 2> strings = { "Redo", "Undo" };
//		return strings;
//	}
//
//	bool getPossibleListValues(const string& attributeName, ConstStringPtrList& list) const override {
//		if (attributeName == type) {
//			for (auto& t : typeStrings()) {
//				list.emplace_back(&t);
//			}
//			return true;
//		}
//		return false;
//	}
//
//	bool getAttributeValue(CView* view, const string& attributeName, string& stringValue, const IUIDescription* desc) const override {
//		Control* control = dynamic_cast<Control*>(view);
//		if (control == 0) return false;
//		if (attributeName == type) {
//			switch (control->getType()) {
//			case HistoryButton::Type::Redo:
//				stringValue = "Redo";
//				return true;
//			case HistoryButton::Type::Undo:
//				stringValue = "Undo";
//				return true;
//			}
//		}
//		return UbertonButtonFactory::getAttributeValue(view, attributeName, stringValue, desc);
//	}
//};
//
//HistoryButtonFactory __gHistoryButtonFactory;
//const std::string HistoryButtonFactory::type = "type";
//
//
//
//class TesseractSliderFactory : public UIViewCreator::SliderBaseCreator
//{
//	using Control = TesseractSlider;
//
//	static const std::string handleBitmap;
//	static const std::string pathStartPoint;
//	static const std::string pathEndPoint;
//
//public:
//	TesseractSliderFactory() {
//		UIViewFactory::registerViewCreator(*this);
//	}
//
//	IdStringPtr getViewName() const override { return "Tesseract Slider"; }
//	IdStringPtr getBaseViewName() const override { return UIViewCreator::kCControl; }
//
//	CView* create(const UIAttributes& attributes, const IUIDescription* description) const override {
//		auto slider = new TesseractSlider({ 0, 0, 50, 30 });
//		slider->setPathStartPoint({ 10, 10 });
//		slider->setPathEndPoint({ 40, 20 });
//		return slider;
//	}
//
//	bool apply(CView* view, const UIAttributes& attributes, const IUIDescription* description) const override {
//		Control* control = dynamic_cast<Control*>(view);
//		if (!control) return false;
//		CBitmap* bmp;
//		if (auto* value = attributes.getAttributeValue(handleBitmap)) {
//			if (UIViewCreator::stringToBitmap(value, bmp, description))
//				control->setHandleBitmap(bmp);
//		}
//
//		CPoint p;
//		if (attributes.getPointAttribute(pathStartPoint, p)) {
//			control->setPathStartPoint(p);
//		}
//		if (attributes.getPointAttribute(pathEndPoint, p)) {
//			control->setPathEndPoint(p);
//		}
//		return SliderBaseCreator::apply(view, attributes, description);
//	}
//
//	bool getAttributeNames(StringList& attributeNames) const override {
//		attributeNames.emplace_back(handleBitmap);
//		attributeNames.emplace_back(pathStartPoint);
//		attributeNames.emplace_back(pathEndPoint);
//		return SliderBaseCreator::getAttributeNames(attributeNames);
//	}
//
//	AttrType getAttributeType(const std::string& attributeName) const override {
//		if (attributeName == handleBitmap) return kBitmapType;
//		if (attributeName == pathStartPoint) return kPointType;
//		if (attributeName == pathEndPoint) return kPointType;
//		return SliderBaseCreator::getAttributeType(attributeName);
//	}
//
//	bool getAttributeValue(CView* view, const string& attributeName, string& stringValue, const IUIDescription* desc) const override {
//		Control* control = dynamic_cast<Control*>(view);
//		if (control == 0) return false;
//		if (attributeName == handleBitmap) {
//			if (auto bmp = control->getHandleBitmap()) {
//				UIViewCreator::bitmapToString(bmp, stringValue, desc);
//			}
//			return true;
//		}
//
//		if (attributeName == pathStartPoint) {
//			stringValue = UIAttributes::pointToString(control->getPathStartPoint());
//			return true;
//		}
//		if (attributeName == pathEndPoint) {
//			stringValue = UIAttributes::pointToString(control->getPathEndPoint());
//			return true;
//		}
//		return SliderBaseCreator::getAttributeValue(view, attributeName, stringValue, desc);
//	}
//};
//
//TesseractSliderFactory __gTesseractSliderFactory;
//
//const std::string TesseractSliderFactory::handleBitmap = "handle-bitmap";
//const std::string TesseractSliderFactory::pathStartPoint = "path-start-point";
//const std::string TesseractSliderFactory::pathEndPoint = "path-end-point";
//
//
//
////
////OneDirKnob::OneDirKnob(const CRect& size) : CKnob(size) {
////}
////struct OneDirKnob::MouseEditingState
////{
////	CPoint firstPoint;
////	CPoint lastPoint;
////	float startValue;
////	float entryState;
////	float range;
////	float coef;
////	CButtonState oldButton;
////	bool modeLinear;
////};
////CMouseEventResult OneDirKnob::onMouseMoved(CPoint& where, const CButtonState& buttons) {
////	if (buttons.isLeftButton() && isEditing()) {
////		MouseEditingState* mouseState = nullptr;
////		if (!getAttribute(kCKnobMouseStateAttribute, mouseState)) {
////			state = new MouseEditingState;
////			setAttribute(kCKnobMouseStateAttribute, mouseState);
////		}
////		return *state;
////		auto& mouseState = getMouseEditingState();
////
////		float middle = (getMax() - getMin()) * 0.5f;
////
////		if (where != mouseState.lastPoint) {
////			mouseState.lastPoint = where;
////			if (mouseState.modeLinear) {
////				CCoord diff = (mouseState.firstPoint.y - where.y) + (where.x - mouseState.firstPoint.x);
////				if (buttons != mouseState.oldButton) {
////					mouseState.range = kCKnobRange;
////					if (buttons & kZoomModifier)
////						mouseState.range *= zoomFactor;
////
////					float coef2 = (getMax() - getMin()) / mouseState.range;
////					mouseState.entryState += (float)(diff * (mouseState.coef - coef2));
////					mouseState.coef = coef2;
////					mouseState.oldButton = buttons;
////				}
////				value = (float)(mouseState.entryState + diff * mouseState.coef);
////				bounceValue();
////			}
////			else {
////				where.offset(-getViewSize().left, -getViewSize().top);
////				value = valueFromPoint(where);
////				if (mouseState.startValue - value > middle)
////					value = getMax();
////				else if (value - mouseState.startValue > middle)
////					value = getMin();
////				else
////					mouseState.startValue = value;
////			}
////			if (value != getOldValue())
////				valueChanged();
////			if (isDirty())
////				invalid();
////		}
////		return kMouseEventHandled;
////	}
////	return kMouseEventNotHandled;
////}
//
//}
//}