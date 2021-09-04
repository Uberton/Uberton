// View creators for the custom UI classes for using them in the uidescription editor.
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

#include <vstgui/uidescription/detail/uiviewcreatorattributes.h>
#include <vstgui/vstgui_uidescription.h>
#include <vstgui/uidescription/viewcreator/sliderviewcreator.h>


namespace Uberton {

using namespace VSTGUI;
class FadingFrameAnimationButton;
class HistoryButton;
class UbertonContextMenu;
class DiagonalSlider;
class StringMapLabel;
class TextEditUnits;
class LogVUMeter;
class LinkButton;


class FadingFrameAnimationButtonFactory : public ViewCreatorAdapter
{
	using Control = FadingFrameAnimationButton;

	static const std::string pressedFrameColor;
	static const std::string horizontalBitmapFlip;

public:
	FadingFrameAnimationButtonFactory();

	IdStringPtr getViewName() const override;
	IdStringPtr getBaseViewName() const override;

	CView* create(const UIAttributes& attributes, const IUIDescription* description) const override;
	bool apply(CView* view, const UIAttributes& attributes, const IUIDescription* description) const override;

	bool getAttributeNames(StringList& attributeNames) const override;
	AttrType getAttributeType(const std::string& attributeName) const override;
	bool getAttributeValue(CView* view, const string& attributeName, string& stringValue, const IUIDescription* desc) const override;
};



class HistoryButtonFactory : public FadingFrameAnimationButtonFactory
{
	using Control = HistoryButton;
	static const std::string type;

public:
	HistoryButtonFactory();

	IdStringPtr getViewName() const override;
	IdStringPtr getBaseViewName() const override;

	CView* create(const UIAttributes& attributes, const IUIDescription* description) const override;
	bool apply(CView* view, const UIAttributes& attributes, const IUIDescription* description) const override;

	bool getAttributeNames(StringList& attributeNames) const override;
	AttrType getAttributeType(const std::string& attributeName) const override;
	static std::array<std::string, 2>& typeStrings();
	bool getPossibleListValues(const string& attributeName, ConstStringPtrList& list) const override;
	bool getAttributeValue(CView* view, const string& attributeName, string& stringValue, const IUIDescription* desc) const override;
};



class UbertonContextMenuFactory : public ViewCreatorAdapter
{
	using Control = UbertonContextMenu;

	static const std::string zoomFactors;
	static const std::string symbolColor;
	static const std::string pressedFrameColor;

public:
	UbertonContextMenuFactory();

	IdStringPtr getViewName() const override;
	IdStringPtr getBaseViewName() const override;

	CView* create(const UIAttributes& attributes, const IUIDescription* description) const override;
	bool apply(CView* view, const UIAttributes& attributes, const IUIDescription* description) const override;

	bool getAttributeNames(StringList& attributeNames) const override;
	AttrType getAttributeType(const std::string& attributeName) const override;
	bool getAttributeValue(CView* view, const string& attributeName, string& stringValue, const IUIDescription* desc) const override;
};


class DiagonalSliderFactory : public UIViewCreator::SliderBaseCreator
{
	using Control = DiagonalSlider;

	static const std::string handleBitmap;
	static const std::string pathStartPoint;
	static const std::string pathEndPoint;
	static const std::string horizontalBitmapFlip;
	static const std::string verticalBitmapFlip;

public:
	DiagonalSliderFactory();

	IdStringPtr getViewName() const override;
	IdStringPtr getBaseViewName() const override;

	CView* create(const UIAttributes& attributes, const IUIDescription* description) const override;
	bool apply(CView* view, const UIAttributes& attributes, const IUIDescription* description) const override;

	bool getAttributeNames(StringList& attributeNames) const override;
	AttrType getAttributeType(const std::string& attributeName) const override;
	bool getAttributeValue(CView* view, const string& attributeName, string& stringValue, const IUIDescription* desc) const override;
};


class StringMapLabelFactory : public ViewCreatorAdapter
{
	using Control = StringMapLabel;

	static const std::string names;

public:
	StringMapLabelFactory();

	IdStringPtr getViewName() const override;
	IdStringPtr getBaseViewName() const override;

	CView* create(const UIAttributes& attributes, const IUIDescription* description) const override;
	bool apply(CView* view, const UIAttributes& attributes, const IUIDescription* description) const override;

	bool getAttributeNames(StringList& attributeNames) const override;
	AttrType getAttributeType(const std::string& attributeName) const override;
	bool getAttributeValue(CView* view, const string& attributeName, string& stringValue, const IUIDescription* desc) const override;
};

class TextEditUnitsFactory : public ViewCreatorAdapter
{
	using Control = TextEditUnits;

	static const std::string units;

public:
	TextEditUnitsFactory();

	IdStringPtr getViewName() const override;
	IdStringPtr getBaseViewName() const override;

	CView* create(const UIAttributes& attributes, const IUIDescription* description) const override;
	bool apply(CView* view, const UIAttributes& attributes, const IUIDescription* description) const override;

	bool getAttributeNames(StringList& attributeNames) const override;
	AttrType getAttributeType(const std::string& attributeName) const override;
	bool getAttributeValue(CView* view, const string& attributeName, string& stringValue, const IUIDescription* desc) const override;
};

class LogVUMeterFactory : public ViewCreatorAdapter
{
	using Control = LogVUMeter;

	static const std::string dezibelMin;
	static const std::string dezibelMax;

public:
	LogVUMeterFactory();

	IdStringPtr getViewName() const override;
	IdStringPtr getBaseViewName() const override;

	CView* create(const UIAttributes& attributes, const IUIDescription* description) const override;
	bool apply(CView* view, const UIAttributes& attributes, const IUIDescription* description) const override;

	bool getAttributeNames(StringList& attributeNames) const override;
	AttrType getAttributeType(const std::string& attributeName) const override;
	bool getAttributeValue(CView* view, const string& attributeName, string& stringValue, const IUIDescription* desc) const override;
};

class LinkButtonFactory : public ViewCreatorAdapter
{
	using Control = LinkButton;

public:
	LinkButtonFactory();

	IdStringPtr getViewName() const override;
	IdStringPtr getBaseViewName() const override;

	CView* create(const UIAttributes& attributes, const IUIDescription* description) const override;
};



namespace ViewCreator {

static const IdStringPtr kFadingFrameAnimationButton = "Fading Frame Animation Button";
static const IdStringPtr kHistoryButton = "History Button";
static const IdStringPtr kUbertonContextMenu = "Uberton Context Menu";
static const IdStringPtr kDiagonalSlider = "Diagonal Slider";
static const IdStringPtr kStringMapLabel = "String Map Label";
static const IdStringPtr kTextEditUnits = "Text Edit with Units";
static const IdStringPtr kLogVUMeter = "Logarithmic VU Meter";
static const IdStringPtr kLinkButton = "Link Button";

}

}