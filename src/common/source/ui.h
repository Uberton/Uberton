
// Additional UI classes to extend the VSTGUI controls.
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

#include <vstgui/plugin-bindings/vst3editor.h>


namespace Uberton {

using namespace VSTGUI;

class VST3EditorEx1;

/*
 * Animation that gradually sets the frame color opacity to 0. 
 * The target type needs to be a CView and also implement the functions setFrameColor() and 
 * getFrameColor(). 
 */
template<class Target>
//requires std::derived_from<Target, CView> && requires(Target t, CColor c) {
//	t.setFrameColor(c);
//	{ t.getFrameColor() } -> std::convertible_to<CColor>;
//}
class FadingFrameAnimation : public VSTGUI::Animation::IAnimationTarget, public NonAtomicReferenceCounted
{
public:
	FadingFrameAnimation(float endValue, bool forceEndValueOnFinish = false)
		: startValue(0.f), endValue(endValue), forceEndValueOnFinish(forceEndValueOnFinish) {}

	void animationStart(CView* view, IdStringPtr name) override {
		auto* control = dynamic_cast<Target*>(view);
		if (control) {
			clr = control->getFrameColor();
			startValue = clr.alpha;
		}
	}

	void animationTick(CView* view, IdStringPtr name, float pos) override {
		auto* control = dynamic_cast<Target*>(view);
		if (control) {
			float value = startValue + (endValue - startValue) * pos;
			clr.alpha = value;
			control->setFrameColor(clr);
			if (control->isDirty())
				control->invalid();
		}
	}

	void animationFinished(CView* view, IdStringPtr name, bool wasCanceled) override {
		auto* control = dynamic_cast<Target*>(view);
		if (control) {
			if (!wasCanceled || forceEndValueOnFinish) {
				clr.alpha = endValue;
				control->setFrameColor(clr);
			}
		}
	}

private:
	float startValue;
	float endValue;
	bool forceEndValueOnFinish;
	CColor clr;
};


/* 
 * Button that changes frame color when pressed down and fades the frame color
 * to transparent when released. 
 */
class FadingFrameAnimationButton : public VSTGUI::CKickButton
{
public:
	FadingFrameAnimationButton(const CRect& size);

	void draw(CDrawContext* context) override;

	CMouseEventResult onMouseDown(CPoint& where, const CButtonState& buttons) override;
	CMouseEventResult onMouseUp(CPoint& where, const CButtonState& buttons) override;

	void setFrameColor(const CColor& clr);
	CColor getFrameColor() const;

	void setPressedFrameColor(const CColor& clr);
	CColor getPressedFrameColor() const;

	void setHorizontalBitmapFlip(bool flip);
	bool isHorizontalBitmapFlip() const;

private:
	CColor pressedFrameColor = kWhiteCColor; // Frame color for when control is pressed down
	CColor frameColor = kTransparentCColor;	 // Current frame color which will be pressedFrameColor but with animated opacity

	bool bitmapHFlip{ false };
	const int animationTimeMilliseconds = 400;
};


/*
 * Special control for undo and redo buttons that fires a callback when pressed and features
 * a type field for distinguishing undo and redo buttons. 
 */
class HistoryButton : public FadingFrameAnimationButton
{
public:
	HistoryButton(const CRect&);

	CMouseEventResult onMouseUp(CPoint& where, const CButtonState& buttons) override;

	enum class Type {
		Undo,
		Redo
	};

	void setType(Type type);
	Type getType() const;

	void setCallback(const std::function<void(Type)>& callback);

	CLASS_METHODS(HistoryButton, FadingFrameAnimationButton);

private:
	Type type{ Type::Undo };
	std::function<void(Type)> callback;
};


/*
 * Context menu specifically for Uberton. Allows setting zoom factors that can be controlled through the menu 
 * or alternatively through the right click option menu. 
 */
class UbertonContextMenu : public COptionMenu
{

	enum class MenuItemID : int32_t {
		userGuide,
		zoom,
		url,
		legal
	};

	using TheEditor = VST3EditorEx1;

public:
	UbertonContextMenu();

	using ZoomFactors = std::vector<double>;

	CMouseEventResult onMouseDown(CPoint& where, const CButtonState& buttons) override;
	CMouseEventResult onMouseUp(CPoint& where, const CButtonState& buttons) override;

	void draw(CDrawContext* context) override;

	void setSymbolColor(const CColor& clr);
	CColor getSymbolColor() const;

	void setZoomFactors(const ZoomFactors& zoomFactors);
	ZoomFactors getZoomFactors() const;

	void setPressedFrameColor(const CColor& clr);
	CColor getPressedFrameColor() const;

	CLASS_METHODS(UbertonContextMenu, COptionMenu);

protected:
	void initialize();
	void initMenu();
	void startAnimation();
	void itemSelected(CCommandMenuItem* item);

	bool openUserguide();

private:
	TheEditor* editor{ nullptr };
	SharedPointer<COptionMenu> zoomMenu{ nullptr };
	SharedPointer<COptionMenu> legalMenu{ nullptr };

	ZoomFactors zoomFactors = { .8, .9, 1, 1.1, 1.2, 1.5 };
	CColor symbolColor = kWhiteCColor;
	CColor pressedFrameColor = kWhiteCColor; // Frame color for when control is pressed down
	bool initialized{ false };
	const int animationTimeMilliseconds = 400;
};


/*
 * Special slider that places a handle bitmap along a path through two specified points inside the control. 
 */
class DiagonalSlider : public CSliderBase
{
public:
	DiagonalSlider(const CRect& r);

	void draw(CDrawContext* context) override;

	void setHandleBitmap(CBitmap* handle);
	CBitmap* getHandleBitmap() const;

	void setPathStartPoint(const CPoint& p);
	void setPathEndPoint(const CPoint& p);

	CPoint getPathStartPoint() const;
	CPoint getPathEndPoint() const;

	void setHorizontalBitmapFlip(bool flip);
	bool isHorizontalBitmapFlip() const;
	void setVerticalBitmapFlip(bool flip);
	bool isVerticalBitmapFlip() const;

	CMouseEventResult onMouseDown(CPoint& where, const CButtonState& buttons) override;
	CMouseEventResult onMouseUp(CPoint& where, const CButtonState& buttons) override;
	CMouseEventResult onMouseMoved(CPoint& where, const CButtonState& buttons) override;
	CMouseEventResult onMouseCancel() override;
	bool onWheel(const CPoint& where, const CMouseWheelAxis& axis, const float& distance,
		const CButtonState& buttons) override {
		return CSliderBase::onWheel(where, axis, distance, buttons);
	}

	CLASS_METHODS(DiagonalSlider, CControl);

protected:
	CRect getHandleRect() const;

private:
	// mouse event values
	CPoint startPoint{ 0, 0 };
	float startValue{ 0 };

	SharedPointer<CBitmap> handleBitmap{ nullptr };

	CPoint p1;
	CPoint p2;
	bool bitmapHFlip{ false };
	bool bitmapVFlip{ false };
};


/*
 * A parameter label display for discrete paremeters that shows not the parameter itself 
 * but a corresponding string that is mapped to the value. The list of strings can be 
 * specified using setNames(). 
 */
class StringMapLabel : public CParamDisplay
{
public:
	StringMapLabel(const CRect&);

	void draw(CDrawContext* context) override;

	void setNames(const std::vector<std::string>& names);
	std::vector<std::string> getNames() const;

	CLASS_METHODS(StringMapLabel, CTextLabel);

protected:
	void updateText();

private:
	std::vector<std::string> names;
	bool initialized{ false };
};


/*
 * A TextEdit that also displays units for the values of the associated parameter. 
 */
class TextEditUnits : public CTextEdit
{
public:
	TextEditUnits(const CRect&);

	void draw(CDrawContext* context) override;

	void setUnits(std::string units);
	std::string getUnits() const;

	CLASS_METHODS(TextEditUnits, CTextEdit);

private:
	std::string units = "";
};


/*
 * VU meter control that scales volume logarithmically (therefore the dB scale is linear).
 */
class LogVUMeter : public CVuMeter
{
public:
	LogVUMeter(const CRect& size);
	void draw(CDrawContext* context) override;

	// set max dB value for scale (i.e. 0 or 6)
	void setDezibelMin(double dezibelMin);
	double getDezibelMin() const;
	// set min dB value for scale (i.e. -40 or -60)
	void setDezibelMax(double dezibelMax);
	double getDezibelMax() const;


private:
	double minDb = -40;
	double maxDb = 0;
};


/*
 * On-Off-Button designed for use with ParamLinkSubcontroller subcontroller. 
 * LinkButton calls valueChanged() also when the value changed externally. 
 * 
 */
class LinkButton : public COnOffButton
{
public:
	LinkButton(const CRect& size);
	void draw(CDrawContext* context) override;


private:
	float oldValue{ 0 };
};


/*
 * Extension for VST3Editor that allows to set an additional "pre"-scaling factor. 
 * The idea is to provide all bitmaps at 2x the size that they are displayed at 
 * 100% zoom. Also, the entire UI is built twice as large. The default size, which 
 * is then displayed as "100%" is actually 50%. 
 *
 * This allows the end user to enlarge the UI up to 2x without getting blurry. 
 */
class VST3EditorEx1 : public VST3Editor
{
public:
	VST3EditorEx1(Steinberg::Vst::EditController* controller, UTF8StringPtr templateName, UTF8StringPtr xmlFile);

	// Two ways to do it: 
	// - Change getAbsScaleFactor() which is not virtual in the VSTGUI library, so the "virtual" would need to be added here. 
	// - Or override the virtual setContentScaleFactor() member function and slide in the additional scaling factor there. 

	//double getAbsScaleFactor() const override;
	Steinberg::tresult PLUGIN_API setContentScaleFactor(Steinberg::IPlugViewContentScaleSupport::ScaleFactor factor) override;

	void setPrescaleFactor(double f);
	double getPrescaleFactor();

	bool openUserguide();

	void setUserguidePath(const std::string& userGuidePath);

	static std::string getUbertonLocation();

private:
	double prescaleFactor = 1;
	std::string userGuidePath;
	double actualContentScaleFactor = 1;
};
}