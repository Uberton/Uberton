//
//// 
////
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
//#include <pluginterfaces/vst/vsttypes.h>
////#include <parameters.h>
//#include <vstgui/vstgui.h>
//#include <vstgui/lib/animation/animations.h>
//#include <vstgui/uidescription/delegationcontroller.h>
//#include <vstgui/plugin-bindings/vst3editor.h>
//#include <vstgui/lib/controls/icommandmenuitemtarget.h>
//#include <public.sdk/source/common/openurl.h>
//#include "pluginterfaces/vst/ivstcontextmenu.h"
//#include "ids.h"
//#include "controller.h"
//
//namespace Uberton {
//namespace ManualGui {
//
//using namespace VSTGUI;
//
//
//
//class TesseractSlider : public CSliderBase
//{
//public:
//	TesseractSlider(const CRect& r) : CSliderBase(r, nullptr, -1) {
//		setWantsFocus(true);
//		kAlwaysUseZoomFactor = true;
//	}
//
//	void draw(CDrawContext* context) override {
//		if (getDrawBackground()) {
//			CRect rect(0, 0, getControlSizePrivate().x, getControlSizePrivate().y);
//			rect.offset(getViewSize().left, getViewSize().top);
//			getDrawBackground()->draw(context, rect);
//		}
//		if (handleBitmap) {
//			handleBitmap->draw(context, getHandleRect());
//		}
//		ParamValue value = getValueNormalized();
//		double x0 = p1.x + (p2.x - p1.x) * value + getViewSize().left;
//		double y0 = p1.y + (p2.y - p1.y) * value + getViewSize().top;
//		context->drawPoint({ x0, y0 }, kBlackCColor);
//		setDirty(false);
//	}
//
//	CRect getHandleRect() const {
//		CRect r;
//		ParamValue value = getValueNormalized();
//		double x0 = p1.x + (p2.x - p1.x) * value;
//		double y0 = p1.y + (p2.y - p1.y) * value;
//
//		r.left = x0 - handleBitmap->getWidth() / 2.0 + getOffsetHandle().x;
//		r.right = x0 + handleBitmap->getWidth() / 2.0 + getOffsetHandle().x;
//		r.top = y0 - handleBitmap->getHeight() / 2.0 + getOffsetHandle().y;
//		r.bottom = y0 + handleBitmap->getHeight() / 2.0 + getOffsetHandle().y;
//		r.offset(getViewSize().getTopLeft());
//		return r;
//	}
//
//	void setHandleBitmap(CBitmap* handle) {
//		handleBitmap = handle;
//		setDirty();
//	}
//
//	CBitmap* getHandleBitmap() const {
//		return handleBitmap;
//	}
//
//	void setPathStartPoint(const CPoint& p) {
//		p1 = p;
//		setDirty();
//	}
//	void setPathEndPoint(const CPoint& p) {
//		p2 = p;
//		setDirty();
//	}
//
//	CPoint getPathStartPoint() const { return p1; }
//	CPoint getPathEndPoint() const { return p2; }
//
//	CMouseEventResult onMouseDown(CPoint& where, const CButtonState& buttons) override {
//		if (!(buttons & kLButton)) return kMouseEventNotHandled;
//
//		invalidMouseWheelEditTimer(this);
//		beginEdit();
//		startValue = getValue();
//		startPoint = where;
//		return kMouseEventHandled;
//	}
//	CMouseEventResult onMouseUp(CPoint& where, const CButtonState& buttons) override {
//		if (isEditing()) {
//			endEdit();
//		}
//		return kMouseEventHandled;
//	}
//	CMouseEventResult onMouseMoved(CPoint& where, const CButtonState& buttons) override {
//		if (buttons.isLeftButton() && isEditing()) {
//
//			float horizontalDistance = p2.x - p1.x;
//			float factor = 5.f * horizontalDistance;
//
//			if (buttons & kAlt) {
//				factor *= getZoomFactor();
//			}
//			float value = startValue + (where.x - startPoint.x) / factor;
//			setValueNormalized(value);
//
//			if (isDirty()) {
//				valueChanged();
//				invalid();
//			}
//			return kMouseEventHandled;
//		}
//		return kMouseEventNotHandled;
//	}
//	CMouseEventResult onMouseCancel() override {
//		if (isEditing()) {
//			value = startValue;
//			if (isDirty()) {
//				valueChanged();
//				invalid();
//			}
//			endEdit();
//		}
//		return kMouseEventHandled;
//	}
//
//
//	CLASS_METHODS(TesseractSlider, CControl);
//
//private:
//	// mouse event values
//	CPoint startPoint{ 0, 0 };
//	float startValue{ 0 };
//
//	SharedPointer<CBitmap> handleBitmap{ nullptr };
//
//	CPoint p1;
//	CPoint p2;
//};
//
//
//
//
//template<class Target>
////requires std::derived_from<Target, CView> && requires(Target t, CColor c) {
////	t.setFrameColor(c);
////	{ t.getFrameColor() } -> std::convertible_to<CColor>;
////}
//class FrameOpacityAnimation : public VSTGUI::Animation::IAnimationTarget, public NonAtomicReferenceCounted
//{
//public:
//	FrameOpacityAnimation(float endValue, bool forceEndValueOnFinish = false)
//		: startValue(0.f), endValue(endValue), forceEndValueOnFinish(forceEndValueOnFinish) {}
//
//	void animationStart(CView* view, IdStringPtr name) override {
//		auto* control = dynamic_cast<Target*>(view);
//		if (control) {
//			clr = control->getFrameColor();
//			startValue = clr.alpha;
//		}
//	}
//
//	void animationTick(CView* view, IdStringPtr name, float pos) override {
//		auto* control = dynamic_cast<Target*>(view);
//		if (control) {
//			float value = startValue + (endValue - startValue) * pos;
//			clr.alpha = value;
//			control->setFrameColor(clr);
//			if (control->isDirty())
//				control->invalid();
//		}
//	}
//
//	void animationFinished(CView* view, IdStringPtr name, bool wasCanceled) override {
//		auto* control = dynamic_cast<Target*>(view);
//		if (control) {
//			if (!wasCanceled || forceEndValueOnFinish) {
//				clr.alpha = endValue;
//				control->setFrameColor(clr);
//			}
//		}
//	}
//
//private:
//	float startValue;
//	float endValue;
//	bool forceEndValueOnFinish;
//	CColor clr;
//};
//
//
//class UbertonButton : public VSTGUI::CKickButton
//{
//public:
//	UbertonButton(const CRect& size) : CKickButton(size, nullptr, -1, nullptr) {}
//
//	void draw(CDrawContext* context) override {
//		CGraphicsTransform transform;
//
//		if (bitmapHFlip) {
//			transform.translate(-getViewSize().getTopRight());
//			transform.scale(-1, 1);
//			transform.translate(getViewSize().getTopRight());
//			transform.translate(-getViewSize().getWidth(), 0);
//		}
//
//		CDrawContext::Transform ctxTransform(*context, transform);
//
//		if (getDrawBackground()) {
//			getDrawBackground()->draw(context, getViewSize(), { offset.x, offset.y });
//		}
//
//		context->setFrameColor(frameColor);
//		context->setLineWidth(1);
//		context->drawRect(getViewSize(), kDrawStroked);
//		if (!getMouseEnabled()) {
//			context->setFillColor({ 0, 0, 0, 100 });
//			context->drawRect(getViewSize(), kDrawFilled);
//		}
//		setDirty(false);
//	}
//
//	CMouseEventResult onMouseDown(CPoint& where, const CButtonState& buttons) override {
//		setFrameColor(pressedFrameColor);
//		removeAnimation("FrameOpacityAnimation"); // in case it is still running
//		CKickButton::onMouseDown(where, buttons);
//		return kMouseEventHandled;
//	}
//
//	CMouseEventResult onMouseUp(CPoint& where, const CButtonState& buttons) override {
//		addAnimation("FrameOpacityAnimation", new FrameOpacityAnimation<UbertonButton>(0), new VSTGUI::Animation::CubicBezierTimingFunction(400, CPoint(0.42, 0.), CPoint(0.58, 1.)));
//		return CKickButton::onMouseUp(where, buttons);
//	}
//
//	void setFrameColor(const CColor& clr) {
//		if (frameColor != clr) {
//			frameColor = clr;
//			setDirty();
//		}
//	}
//
//	CColor getFrameColor() const {
//		return frameColor;
//	}
//
//	void setPressedFrameColor(const CColor& clr) {
//		if (pressedFrameColor != clr) {
//			pressedFrameColor = clr;
//			setDirty();
//		}
//	}
//
//	CColor getPressedFrameColor() const {
//		return pressedFrameColor;
//	}
//
//	void setHorizontalBitmapFlip(bool flip) {
//		bitmapHFlip = flip;
//		setDirty();
//	}
//
//	bool isHorizontalBitmapFlip() const {
//		return bitmapHFlip;
//	}
//
//private:
//	CColor pressedFrameColor = kWhiteCColor; // Frame color for when control is pressed down
//	CColor frameColor = kTransparentCColor;	 // Current frame color which will be pressedFrameColor but with animated opacity
//
//	bool bitmapHFlip{ false };
//};
//
//
////
////class HistoryButton : public UbertonButton
////{
////public:
////	HistoryButton(const CRect& size) : UbertonButton(size) {}
////
////
////	CMouseEventResult onMouseUp(CPoint& where, const CButtonState& buttons) override {
////		if (hitTest(where)) {
////			if (callback) callback(type);
////		}
////		return UbertonButton::onMouseUp(where, buttons);
////	}
////	enum class Type {
////		Undo,
////		Redo
////	};
////
////	void setType(Type type) {
////		this->type = type;
////	}
////	Type getType() const {
////		return type;
////	}
////
////	void setCallback(const std::function<void(Type)>& callback) {
////		this->callback = callback;
////	}
////
////private:
////	Type type{ Type::Undo };
////	std::function<void(Type)> callback;
////};
//
//
//
//class UbertonContextMenu : public COptionMenu
//{
//
//	enum class MenuItemID : int32_t {
//		userGuide,
//		zoom,
//		url
//	};
//
//public:
//	UbertonContextMenu() : COptionMenu(CRect(0, 0, 20, 20), nullptr, -1) {
//		zoomMenu = makeOwned<COptionMenu>();
//		zoomMenu->setStyle(kMultipleCheckStyle);
//		initMenu();
//		setFrameColor(kTransparentCColor);
//		setBackColor(kBlackCColor);
//		setSymbolColor(kWhiteCColor);
//	}
//
//	CMouseEventResult onMouseDown(CPoint& where, const CButtonState& buttons) override {
//		setFrameColor(pressedFrameColor);
//		removeAnimation("FrameOpacityAnimation"); // in case it is still running
//		if (editor) {
//			double zoomFactor = editor->getZoomFactor();
//			auto it = std::find(zoomFactors.begin(), zoomFactors.end(), zoomFactor);
//			if (it != zoomFactors.end()) {
//				zoomMenu->checkEntryAlone(std::distance(zoomFactors.begin(), it));
//			}
//		}
//
//		if (buttons & (kLButton | kRButton | kApple)) {
//			startAnimation();
//		}
//
//		auto result = COptionMenu::onMouseDown(where, buttons);
//
//		if (result == kMouseDownEventHandledButDontNeedMovedOrUpEvents) {
//			return kMouseDownEventHandledButDontNeedMovedOrUpEvents;
//		}
//		return kMouseEventHandled;
//	}
//
//	CMouseEventResult onMouseUp(CPoint& where, const CButtonState& buttons) override {
//		startAnimation();
//		return COptionMenu::onMouseUp(where, buttons);
//	}
//
//	void startAnimation() {
//		addAnimation("FrameOpacityAnimation", new FrameOpacityAnimation<UbertonContextMenu>(0), new VSTGUI::Animation::CubicBezierTimingFunction(400, CPoint(0.42, 0.), CPoint(0.58, 1.)));
//	}
//	using ZoomFactors = std::vector<double>;
//
//	bool initialized{ false };
//
//	void initialize() {
//		editor = dynamic_cast<VST3Editor*>(getEditor());
//		if (editor) {
//			editor->setAllowedZoomFactors(zoomFactors);
//		}
//		initialized = true;
//	}
//	void draw(CDrawContext* context) override {
//		if (!initialized) {
//			initialize();
//		}
//
//		drawBack(context);
//
//
//		auto p = getViewSize().getTopLeft();
//		auto q = getViewSize().getTopRight();
//		context->setFrameColor(symbolColor);
//		context->setLineWidth(1);
//		double dy = getHeight() / 6.0;
//		double x0 = getWidth() / 8;
//		double x1 = getWidth() - x0 - 2;
//		for (int i = 0; i < 3; i++) {
//			context->drawLine(p + CPoint{ x0, dy * (i + 2) }, p + CPoint{ x1, dy * (i + 2) });
//		}
//		setDirty(false);
//	}
//
//
//	void setSymbolColor(const CColor& clr) {
//		symbolColor = clr;
//		setDirty(true);
//	}
//
//	CColor getSymbolColor() const { return symbolColor; }
//
//	void setZoomFactors(const ZoomFactors& zoomFactors) {
//		this->zoomFactors = zoomFactors;
//		editor = dynamic_cast<VST3Editor*>(getEditor());
//		if (editor) {
//			editor->setAllowedZoomFactors(zoomFactors);
//		}
//		initMenu();
//	}
//
//	ZoomFactors getZoomFactors() const { return zoomFactors; }
//
//	void setPressedFrameColor(const CColor& clr) {
//		if (pressedFrameColor != clr) {
//			pressedFrameColor = clr;
//			setDirty();
//		}
//	}
//
//	CColor getPressedFrameColor() const {
//		return pressedFrameColor;
//	}
//
//protected:
//	void initMenu() {
//		this->removeAllEntry();
//		auto addItem = [&](const UTF8String& text, int id, const UTF8String& category, COptionMenu* menu) {
//			auto c = makeOwned<CCommandMenuItem>(CCommandMenuItem::Desc{ text, id, nullptr, category, text });
//			c->remember();
//			c->setActions([&](CCommandMenuItem* item) { itemSelected(item); }, nullptr);
//			auto item = menu->addEntry(c);
//			item->setTag(id);
//			return item;
//		};
//		zoomMenu->removeAllEntry();
//
//		addItem("User Guide", static_cast<int32_t>(MenuItemID::userGuide), "Base", this);
//		auto zoomItem = addItem("Zoom", static_cast<int32_t>(MenuItemID::zoom), "Base", this);
//		zoomItem->setSubmenu(zoomMenu);
//		addItem("Uberton.org", static_cast<int32_t>(MenuItemID::url), "Base", this);
//
//
//		char zoomFactorString[128];
//		int32_t zoomFactorTag = 0;
//		for (auto factor : zoomFactors) {
//			sprintf(zoomFactorString, "%d%%", static_cast<int>(factor * 100));
//			addItem(zoomFactorString, zoomFactorTag++, "Zoom", zoomMenu);
//		}
//	}
//
//	void itemSelected(CCommandMenuItem* item) {
//		if (item->getCommandCategory() == "Base") {
//			switch (static_cast<MenuItemID>(item->getTag())) {
//			case MenuItemID::userGuide:
//				return;
//			case MenuItemID::url:
//				openURLInDefaultApplication("https://uberton.org");
//				return;
//			}
//		}
//		else if (item->getCommandCategory() == "Zoom") {
//			if (item->getTag() < 0 || item->getTag() >= zoomFactors.size()) return;
//			if (editor) {
//				editor->setZoomFactor(zoomFactors[item->getTag()]);
//			}
//			return;
//		}
//	}
//
//
//private:
//	VST3Editor* editor{ nullptr };
//	SharedPointer<COptionMenu> zoomMenu{ nullptr };
//
//	ZoomFactors zoomFactors = { .8, .9, 1, 1.1, 1.2, 1.5 };
//	CColor symbolColor = kWhiteCColor;
//	CColor pressedFrameColor = kWhiteCColor; // Frame color for when control is pressed down
//};
//
//
//class MenuButton : public CView, public CommandMenuItemTargetAdapter
//{
//public:
//	MenuButton(const CRect& size, std::function<void(double)> zoomFactorCallback)
//		: CView(size), zoomFactorCallback(zoomFactorCallback) {}
//
//	void addEntry(const UTF8String& text, int id, UTF8String& category) {
//	}
//	struct Entry
//	{
//	};
//	std::vector<Entry> entries;
//
//	std::vector<double> zoomFactors = { .5, 1, 2 };
//
//	CMouseEventResult onMouseDown(CPoint& where, const CButtonState& buttons) override {
//		FDebugPrint("pressed");
//		CMouseEventResult result = kMouseEventNotHandled;
//		COptionMenu* baseMenu = new COptionMenu();
//		COptionMenu* zoomMenu = new COptionMenu();
//
//		auto addItem = [&](const UTF8String& text, int id, const UTF8String& category, COptionMenu* menu = nullptr) {
//			if (!menu)
//				menu = baseMenu;
//			auto item = menu->addEntry(new CCommandMenuItem(CCommandMenuItem::Desc{ text, id, this, category, text }));
//			item->setTag(id);
//			return item;
//		};
//
//		addItem("Item 1", 0, "Base");
//		addItem("Item 2", 1, "Base");
//
//		char zoomFactorString[128];
//		int32_t zoomFactorTag = 0;
//		for (auto factor : zoomFactors) {
//			sprintf(zoomFactorString, "%d%%", static_cast<int>(factor * 100));
//			addItem(zoomFactorString, zoomFactorTag++, "Zoom", zoomMenu);
//		}
//
//		addItem("Zoom", 3, "Base")->setSubmenu(zoomMenu);
//
//
//		CPoint point = getViewSize().getBottomLeft();
//		if (baseMenu) {
//			baseMenu->remember();
//			SharedPointer<CFrame> blockFrame = getFrame();
//			auto self = VSTGUI::shared(baseMenu);
//			//getFrame()->doAfterEventProcessing([self, blockFrame, point]() {
//			//	//self->setStyle(COptionMenu::kPopupStyle | COptionMenu::kMultipleCheckStyle);
//			//	self->popup(blockFrame, point);
//			//});
//			getFrame()->doAfterEventProcessing([=]() {
//				baseMenu->setStyle(COptionMenu::kPopupStyle | COptionMenu::kMultipleCheckStyle);
//				baseMenu->popup(blockFrame, point);
//				baseMenu->forget();
//			});
//			result = kMouseEventHandled;
//		}
//		auto a = dynamic_cast<EditorView*>(getEditor());
//		if (!a) return result;
//		a->getController()->getComponentHandler();
//
//
//		/*Steinberg::FUnknownPtr<Steinberg::Vst::IComponentHandler3> handler(a->getController()->getComponentHandler());
//		Steinberg::Vst::ParamID paramID;
//		if (handler) {
//			CPoint where2(where);
//			getFrame()->getTransform().transform(where2);
//			//bool paramFound = findParameter((Steinberg::int32)where2.x, (Steinberg::int32)where2.y, paramID) == Steinberg::kResultTrue;
//			Steinberg::Vst::IContextMenu* contextMenu = handler->createContextMenu(a, nullptr);
//			if (contextMenu) {
//				if (baseMenu)
//					VST3EditorInternal::addCOptionMenuEntriesToIContextMenu(this, baseMenu,
//						contextMenu);
//				getFrame()->doAfterEventProcessing([=]() {
//					contextMenu->popup(static_cast<Steinberg::UCoord>(where2.x),
//						static_cast<Steinberg::UCoord>(where2.y));
//					contextMenu->release();
//				});
//				result = kMouseEventHandled;
//			}
//		}*/
//		if (baseMenu)
//			baseMenu->forget();
//		if (zoomMenu) // submenus are not deleted!
//			zoomMenu->forget();
//		return result;
//	}
//	void draw(CDrawContext* context) override {
//		context->setLineWidth(1);
//		context->setFillColor(CColor(255, 255, 255, 255));
//
//		context->setFrameColor(CColor(0, 0, 0, 255));
//		context->drawRect(getViewSize(), kDrawFilledAndStroked);
//		setDirty(false);
//	}
//	bool onCommandMenuItemSelected(CCommandMenuItem* item) override {
//		if (item->getCommandCategory() == "Base") {
//			if (item->getTag() == 0) {
//				FDebugPrint("Item 1");
//			}
//			else if (item->getTag() == 1) {
//
//				FDebugPrint("Item 2");
//			}
//		}
//		else if (item->getCommandCategory() == "Zoom") {
//			if (item->getTag() < 0 || item->getTag() >= zoomFactors.size()) return false;
//			FDebugPrint("Set Zoom to %f", zoomFactors[item->getTag()]);
//			//getFrame()->setZoom(zoomFactors[item->getTag()]);
//			if (zoomFactorCallback) {
//				zoomFactorCallback(zoomFactors[item->getTag()]);
//			}
//			return true;
//		}
//		return false;
//	}
//
//	std::function<void(double)> zoomFactorCallback{ nullptr };
//};
//
//
//class ButtonController : public FObject, public VSTGUI::IController
//{
//public:
//	ButtonController(EditController* editController);
//	void PLUGIN_API update(FUnknown* changedUnknown, int32 message) override;
//	VSTGUI::CView* verifyView(VSTGUI::CView* view, const VSTGUI::UIAttributes& attributes,
//		const VSTGUI::IUIDescription* description) override;
//	void valueChanged(VSTGUI::CControl* pControl) override;
//	void controlBeginEdit(VSTGUI::CControl* pControl) override;
//	void controlEndEdit(VSTGUI::CControl* pControl) override;
//};
//
//
//
//
//
////class HistoryController : public DelegationController
////{
////public:
////	using HistoryButton = HistoryButton;
////
////	HistoryController(Uberton::ManualGui::Controller* c, IController* parentController, bool canUndo, bool canRedo) : 
////		DelegationController(parentController), c(c) ,canUndo(canUndo),canRedo(canRedo){
////	}
////	CView* verifyView(CView* view, const UIAttributes& attributes, const IUIDescription* description) override {
////		HistoryButton* button = dynamic_cast<HistoryButton*>(view);
////		if (button) {
////			if (button->getType() == HistoryButton::Type::Undo) {
////				undoButton = button;
////				undoButton->setCallback([this](HistoryButton::Type type) { undo(); });
////				undoButton->setMouseEnabled(canUndo);
////			}
////			else if (button->getType() == HistoryButton::Type::Redo) {
////				redoButton = button;
////				redoButton->setCallback([this](HistoryButton::Type type) { redo(); });
////				redoButton->setMouseEnabled(canRedo);
////			}
////		}
////		return view;
////	}
////	void undo() {
////		c->undo();
////	}
////	void redo() {
////		c->redo();
////	}
////
////	void updateButtonState(bool canUndo, bool canRedo) {
////		if (undoButton && canUndo != undoButton->getMouseEnabled()) {
////			undoButton->setMouseEnabled(canUndo);
////			undoButton->setDirty();
////		}
////		if (redoButton && canRedo != redoButton->getMouseEnabled()) {
////			redoButton->setMouseEnabled(canRedo);
////			redoButton->setDirty();
////		}
////	}
////
////private:
////	HistoryButton* undoButton{ nullptr };
////	HistoryButton* redoButton{ nullptr };
////	Uberton::ManualGui::Controller* c;
////	bool canUndo;
////	bool canRedo;
////};
//
//class LinkController : public DelegationController
//{
//public:
//	using LinkedControlType = CSlider;
//	using LinkerType = COnOffButton;
//	LinkController(IController* parentController) : DelegationController(parentController) {
//	}
//	CView* verifyView(CView* view, const UIAttributes& attributes, const IUIDescription* description) override {
//		LinkedControlType* linkedControl = dynamic_cast<LinkedControlType*>(view);
//		LinkerType* linker = dynamic_cast<LinkerType*>(view);
//		if (linkedControl) {
//			linkedControls.push_back(linkedControl);
//			linkedControl->registerControlListener(this);
//		}
//		else if (linker) {
//			linkerControl = linker;
//			linkerControl->registerControlListener(this);
//			updateLinkState();
//		}
//		return view;
//	}
//
//	void valueChanged(CControl* control) override {
//		if (control == linkerControl) {
//			updateLinkState();
//			DelegationController::valueChanged(control);
//			return;
//		}
//		if (link) {
//			for (const auto& c : linkedControls) {
//				if (c != control) {
//					auto z = control->getValueNormalized();
//					c->setValueNormalized(z);
//					DelegationController::valueChanged(c);
//				}
//			}
//		}
//		DelegationController::valueChanged(control);
//	}
//
//	void updateLinkState() {
//		if (linkerControl->getValueNormalized() != 0)
//			link = true;
//		else
//			link = false;
//	}
//
//protected:
//	LinkerType* linkerControl{ nullptr };
//	std::vector<LinkedControlType*> linkedControls;
//
//	bool link{ false };
//};
//
//
//
//
//
//
//
//
//class Editor : public VSTGUIEditor, public CommandMenuItemTargetAdapter, public IMouseObserver
//{
//public:
//	Editor(void* controller) : VSTGUIEditor(controller) {
//		setRect({ 0, 0, 700, 200 });
//	}
//
//	bool PLUGIN_API open(void* parent, const VSTGUI::PlatformType& platformType) override {
//		if (frame) return false;
//		return false;
//		CRect size(0, 0, 700, 200);
//		frame = new CFrame(size, this);
//		frame->setBackgroundColor(kBlueCColor);
//
//		c = new CViewContainer(size);
//
//
//		CBitmap* bmp = new CBitmap("background.png");
//		CBitmap* bmp2 = new CBitmap("knobbg.png");
//		if (!bmp2->isLoaded())
//			FDebugPrint("err with bmp");
//		c->setBackground(bmp);
//		//frame.setba
//		CRect knobRect(20, 20, 50, 50);
//		knob = new CKnob(knobRect, nullptr, Params::kParamVolId, bmp2, nullptr, { 30, 10 });
//		knob->setMin(0);
//		knob->setMax(1);
//		//this->controller->liste
//		//k//nob->setva
//		//knob->setBackground(bmp2);
//		knob->setCoronaColor(kRedCColor);
//		c->addView(knob);
//		getFrame()->setZoom(zoomFactor);
//		getFrame()->open(parent, platformType, nullptr);
//
//		menuButton = new MenuButton({ 0, 0, 20, 20 }, [&](float f) { setZoomFactor(f); });
//		c->addView(menuButton);
//
//		//menu = new COptionMenu({ 0, 0, 30, 30 }, nullptr, 3);
//		//menu->setBackColor(kRedCColor);
//		//menu->addEntry(new CCommandMenuItem({ "Item1", 1, this, "Zooms", "Item1" }));
//		//menu->addSeparator();
//		//menu->addEntry("dsdfsdf");
//		//frame->addView(menu);
//
//		frame->addView(c);
//		bmp->forget();
//		bmp2->forget();
//
//		//menu->setFontColor(kTransparentCColor);
//
//		return true;
//	}
//
//	bool requestResize(const CPoint& newSize) {
//		if (!plugFrame)
//			return false;
//		CCoord width = newSize.x;
//		CCoord height = newSize.y;
//		double scaleFactor = zoomFactor;
//		//if ((width >= std::round(minSize.x * scaleFactor) && width <= std::round(maxSize.x * scaleFactor) && height >= std::round(minSize.y * scaleFactor) && height <= std::round(maxSize.y * scaleFactor))) {
//		Steinberg::ViewRect vr;
//		vr.right = static_cast<Steinberg::int32>(width);
//		vr.bottom = static_cast<Steinberg::int32>(height);
//		return plugFrame->resizeView(this, &vr) == Steinberg::kResultTrue ? true : false;
//		//}
//		return false;
//	}
//	void PLUGIN_API close() override {
//		if (frame) {
//			frame->close();
//			frame = nullptr;
//		}
//		knob = nullptr;
//	}
//
//	bool validateCommandMenuItem(CCommandMenuItem* item) override { return false; }
//	bool onCommandMenuItemSelected(CCommandMenuItem* item) override {
//		if (item->getCommandCategory() == "Zooms") {
//			setZoomFactor(2);
//			//size_t index = static_cast<size_t>(item->getTag());
//			//if (index < allowedZoomFactors.size()) {
//			//	setZoomFactor(allowedZoomFactors[index]);
//			//}
//			return true;
//		}
//		return false;
//	}
//	virtual void onMouseEntered(CView* view, CFrame* frame) override {
//	}
//	virtual void onMouseExited(CView* view, CFrame* frame) override {}
//	CMouseEventResult onMouseDown(CFrame* frame, const CPoint& where, const CButtonState& buttons) override {
//		CMouseEventResult result = kMouseEventNotHandled;
//		return result;
//	}
//	void setZoomFactor(double factor) {
//		if (zoomFactor == factor) return;
//		zoomFactor = factor;
//		if (getFrame() == nullptr) return;
//		getFrame()->setZoom(zoomFactor);
//		requestResize({ 700 * zoomFactor, 200 * zoomFactor });
//	}
//	double getZoomFactor() const { return zoomFactor; }
//	void setAllowedZoomFactors(std::vector<double> zoomFactors) { allowedZoomFactors = zoomFactors; }
//	tresult PLUGIN_API canResize() {
//		// always return true as this can change dynamicaly
//		return kResultTrue;
//	}
//
//private:
//	CViewContainer* c;
//	CKnob* knob{ nullptr };
//	COptionMenu* menu;
//	MenuButton* menuButton;
//	double zoomFactor{ 1 };
//	std::vector<double> allowedZoomFactors{ 1, 2 };
//	VST3EditorDelegate* delegate{ nullptr };
//};
//
//
//
//
//class OneDirKnob : public CKnob
//{
//public:
//	OneDirKnob(const CRect& size);
//	CMouseEventResult onMouseMoved(CPoint& where, const CButtonState& buttons) override;
//
//	struct MouseEditingState;
//	CLASS_METHODS(OneDirKnob, CKnob);
//
//private:
//	std::string text;
//};
//}
//}