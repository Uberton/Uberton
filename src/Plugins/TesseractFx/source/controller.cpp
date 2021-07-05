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
#include "ids.h"
#include <vstmath.h>

namespace Uberton {
namespace TesseractFx {

tresult PLUGIN_API Controller::initialize(FUnknown* context) {

	tresult result = ControllerBaseP::initialize(context);
	if (result != kResultTrue) return result;

	// Set up units
	UnitID rootUnitId = 1;
	addUnit(new Unit(USTRING("Root"), rootUnitId));
	setCurrentUnitID(rootUnitId);
	UnitID inputPositionUnitId = 2;
	addUnit(new Unit(USTRING("Input Position"), inputPositionUnitId, rootUnitId));
	UnitID outputPositionUnitId = 3;
	addUnit(new Unit(USTRING("Output Position"), outputPositionUnitId, rootUnitId));
	UnitID postSection = 4;
	addUnit(new Unit(USTRING("Post Filter"), postSection, rootUnitId));


	setCurrentUnitID(rootUnitId);
	{
		addRangeParam(ParamSpecs::vol, "Master Volume", "MVol", "%");
		addRangeParam(ParamSpecs::mix, "Mix", "Mix", "%");
		addRangeParam(ParamSpecs::resonatorType, "Resonator Type", "Res Type", "")->getInfo().stepCount = 1;
		addRangeParam(ParamSpecs::resonatorDim, "Resonator Dimension", "Res Dim", "D")->getInfo().stepCount = maxDimension - 1;
		addRangeParam(ParamSpecs::resonatorOrder, "Resonator Order", "Res Order", "")->getInfo().stepCount = maxOrder - 1;
		addRangeParam(ParamSpecs::resonatorFreq, "Resonator Frequency", "Res Freq", "Hz");
		addRangeParam(ParamSpecs::resonatorDamp, "Resonator Dampening", "Res Damp", "%");
		addRangeParam(ParamSpecs::resonatorVel, "Sonic Velocity", "Sonic Vel", "m/s");

		addRangeParam(ParamSpecs::inPosCurveL, "Input Pos Curve L", "InPos L", "%");
		addRangeParam(ParamSpecs::inPosCurveR, "Input Pos Curve R", "InPos R", "%");
		addRangeParam(ParamSpecs::outPosCurveL, "Output Pos Curve L", "OutPos L", "%");
		addRangeParam(ParamSpecs::outPosCurveR, "Output Pos Curve R", "OutPos R", "%");
		addStringListParam(ParamSpecs::linkInPosCurves, "Link In Pos Curves", "Link In C", { "Not Linked", "linked" });
		addStringListParam(ParamSpecs::linkOutPosCurves, "Link Out Pos Curves", "Link Out C", { "Not Linked", "linked" });

		addRangeParam(ParamSpecs::vuPPM, "Output Level", "Level", "dB", ParameterInfo::kIsReadOnly);
	}

	setCurrentUnitID(postSection);
	{
		addRangeParam(ParamSpecs::lcFreq, "Low Cut Frequency", "LC Freq", "Hz");
		addRangeParam(ParamSpecs::lcQ, "Low Cut Q", "LC Q", "");
		addRangeParam(ParamSpecs::hcFreq, "High Cut Frequency", "HC Freq", "Hz");
		addRangeParam(ParamSpecs::hcQ, "High Cut Q", "HC Q", "");
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
	//if (ConstString(name) == ViewType::kEditor) return new VSTGUI::VST3Editor(this, "Editor", "editor.uidesc");
	return nullptr;
}

} // namespace TesseractFx
} // namespace Uberton