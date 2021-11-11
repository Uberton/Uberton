// -----------------------------------------------------------------------------------------------------------------------------
// This file is part of the Überton project. Copyright (C) 2021 Überton
//
// Überton is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// Überton is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
// of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details. You should
// have received a copy of the GNU General Public License along with Überton. If not, see http://www.gnu.org/licenses/.
// -----------------------------------------------------------------------------------------------------------------------------


#include "controller.h"
#include <ui.h>
#include <subcontrollers.h>

namespace Uberton {
namespace TesseractFx {

tresult PLUGIN_API Controller::initialize(FUnknown* context) {

	tresult result = ControllerBase::initialize(context);
	if (result != kResultTrue) return result;


	// Set up units
	UnitID rootUnitId = 1;
	addUnit(new Unit(USTRING("Root"), rootUnitId));
	setCurrentUnitID(rootUnitId);
	UnitID inputPositionUnitId = 2;
	addUnit(new Unit(USTRING("Input Position"), inputPositionUnitId, rootUnitId));
	UnitID outputPositionUnitId = 3;
	addUnit(new Unit(USTRING("Output Position"), outputPositionUnitId, rootUnitId));
	UnitID postSectionUnitId = 4;
	addUnit(new Unit(USTRING("Post Filter"), postSectionUnitId, rootUnitId));



	setCurrentUnitID(rootUnitId);
	{
		addParam<LogParameter>(ParamSpecs::vol, "Master Volume", "MVol", "", Precision(2));
		addParam<LinearParameter>(ParamSpecs::mix, "Mix", "Mix", "%", Precision(0));
		addParam<DiscreteParameter>(ParamSpecs::resonatorType, "Resonator Type", "Res Type", "")->getInfo().stepCount = 1;
		addParam<DiscreteParameter>(ParamSpecs::resonatorDim, "Resonator Dimension", "Res Dim", "D", Precision(0))->getInfo().stepCount = maxDimension - 1;
		addParam<DiscreteParameter>(ParamSpecs::resonatorOrder, "Resonator Order", "Res Order", "", Precision(0))->getInfo().stepCount = maxOrder - 1;

		addParam<LogParameter>(ParamSpecs::resonatorDamp, "Resonator Dampening", "Res Damp", "", Precision(2));
		addParam<LogParameter>(ParamSpecs::resonatorFreq, "Resonator Frequency", "Res Freq", "Hz");

		addParam<LinearParameter>(ParamSpecs::resonatorVel, "Sonic Velocity", "Sonic Vel", "m/s");

		addParam<LinearParameter>(ParamSpecs::inPosCurveL, "Input Pos Curve L", "InPos L", "", Precision(3));
		addParam<LinearParameter>(ParamSpecs::inPosCurveR, "Input Pos Curve R", "InPos R", "", Precision(3));
		addParam<LinearParameter>(ParamSpecs::outPosCurveL, "Output Pos Curve L", "OutPos L", "", Precision(3));
		addParam<LinearParameter>(ParamSpecs::outPosCurveR, "Output Pos Curve R", "OutPos R", "", Precision(3));
		addStringListParam(ParamSpecs::linkInPosCurves, "Link In Pos Curves", "Link In C", { "Not Linked", "Linked" }, ParameterInfo::kNoFlags);
		addStringListParam(ParamSpecs::linkOutPosCurves, "Link Out Pos Curves", "Link Out C", { "Not Linked", "Linked" }, ParameterInfo::kNoFlags);

		parameters.addParameter(new GainParameter(L"Output Level L", ParamSpecs::vuPPML.id, L"dB", 0, ParameterInfo::kIsReadOnly, rootUnitId, L"Level", vuPPMOverheadDB));
		parameters.addParameter(new GainParameter(L"Output Level R", ParamSpecs::vuPPMR.id, L"dB", 0, ParameterInfo::kIsReadOnly, rootUnitId, L"Level", vuPPMOverheadDB));
		addParam<LinearParameter>(ParamSpecs::processTime, "Process Time", "T", "", Precision(6), ParameterInfo::kIsReadOnly);

		addStringListParam(ParamSpecs::limiterOn, "Output Limiter", "Out Lim", { "Off", "On" });
		addParam<LinearParameter>(ParamSpecs::resonatorLength, "Resonator Length", "Res Len", "m", Precision(3), ParameterInfo::kIsReadOnly);
	}

	setCurrentUnitID(postSectionUnitId);
	{
		addParam<LogParameter>(ParamSpecs::lcFreq, "Low Cut Frequency", "LC Freq", "Hz", Precision(0));
		addParam<LinearParameter>(ParamSpecs::lcQ, "Low Cut Q", "LC Q", "");
		addParam<LogParameter>(ParamSpecs::hcFreq, "High Cut Frequency", "HC Freq", "Hz", Precision(0));
		addParam<LinearParameter>(ParamSpecs::hcQ, "High Cut Q", "HC Q", "");
	}
	setCurrentUnitID(inputPositionUnitId);
	{
		UString256 a("", 10);
		for (int i = 0; i < maxDimension; i++) {
			a.printInt(i);
			UString256 name("Left X");
			name.append(a);
			addRangeParam(Params::kParamInL0 + i, name, "", { 0, 1, .5 });
		}
		for (int i = 0; i < maxDimension; i++) {
			a.printInt(i);
			UString256 name("Right X");
			name.append(a);
			addRangeParam(Params::kParamInR0 + i, name, "", { 0, 1, .5 });
		}
		setCurrentUnitID(outputPositionUnitId);
		for (int i = 0; i < maxDimension; i++) {
			a.printInt(i);
			UString256 name("Left Y");
			name.append(a);
			addRangeParam(Params::kParamOutL0 + i, name, "", { 0, 1, .5 });
		}
		for (int i = 0; i < maxDimension; i++) {
			a.printInt(i);
			UString256 name("Right Y");
			name.append(a);
			addRangeParam(Params::kParamOutR0 + i, name, "", { 0, 1, .5 });
		}
	}

	return kResultTrue;
}

IPlugView* PLUGIN_API Controller::createView(FIDString name) {
	if (ConstString(name) == ViewType::kEditor) {
		auto editor = new VST3EditorEx1(this, "Editor", "editor.uidesc");
		editor->setPrescaleFactor(.5);
#ifdef TesseractFx_USERGUIDE_PATH
		editor->setUserguidePath(TesseractFx_USERGUIDE_PATH);
#endif
		return editor;
	}
	return nullptr;
}

tresult PLUGIN_API Controller::notify(IMessage* message) {
	if (FIDStringsEqual(message->getMessageID(), processorDeactivatedMsgID)) {
		parameters.getParameter(Params::kParamVUPPM_L)->setNormalized(0);
		parameters.getParameter(Params::kParamVUPPM_R)->setNormalized(0);
		return kResultTrue;
	}
	return ControllerBase<ParamState, ImplementBypass>::notify(message);
}

tresult PLUGIN_API Controller::setParamNormalized(ParamID tag, ParamValue value) {
	auto result = ControllerBase<ParamState, ImplementBypass>::setParamNormalized(tag, value);
	switch (tag) {
	case Params::kParamResonatorFreq:
	case Params::kParamResonatorDim:
	case Params::kParamResonatorDamp:
		updateResonatorLength();
	}
	return result;
}

tresult PLUGIN_API Controller::setComponentState(IBStream* state) {
	auto result = ControllerBase<ParamState, ImplementBypass>::setComponentState(state);
	updateResonatorLength();
	return result;
}

void Controller::updateResonatorLength() {
	const double pi = 3.14159265358;
	const double velocity = 343;
	int dim = ParamSpecs::resonatorDim.toDiscrete(getParamNormalized(kParamResonatorDim));
	double w = ParamSpecs::resonatorFreq.toScaled(getParamNormalized(kParamResonatorFreq)) * 2 * pi;
	double b = ParamSpecs::resonatorDamp.toScaled(getParamNormalized(kParamResonatorDamp));

	double length = pi * velocity * std::sqrt(dim / (w * w + b * b));
	double lengthNormalized = ParamSpecs::resonatorLength.toNormalized(length);
	if (getParamNormalized(Params::kParamResonatorLength) != lengthNormalized) {
		setParamNormalized(Params::kParamResonatorLength, ParamSpecs::resonatorLength.toNormalized(length));
	}
}

FUnknown* createControllerInstance(void*) {
	return static_cast<IEditController*>(new Controller);
}

} // namespace TesseractFx
} // namespace Uberton