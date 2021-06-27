// -----------------------------------------------------------------------------------------------------------------------------
// This file is part of the �berton project. Copyright (C) 2021 �berton
//
// �berton is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// �berton is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
// of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details. You should
// have received a copy of the GNU General Public License along with �berton. If not, see http://www.gnu.org/licenses/.
// -----------------------------------------------------------------------------------------------------------------------------


#include "controller.h"
#include "ids.h"
#include <vstmath.h>

namespace Uberton {
namespace TesseractFx {

tresult PLUGIN_API Controller::initialize(FUnknown* context) {

	tresult result = ControllerBaseP::initialize(context);
	if (result != kResultTrue) return result;

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
	addRangeParam(Params::kParamVol, "Master Volume", "%", { 0, 1, .8 });
	addRangeParam(Params::kParamMix, "Mix", "%", { 0, 1, 1 });
	addRangeParam(Params::kParamInPosCurveL, "Input Pos Curve L", "%", { 0, 1, .5 });
	addRangeParam(Params::kParamInPosCurveR, "Input Pos curve R", "%", { 0, 1, .5 });
	addRangeParam(Params::kParamOutPosCurveL, "Output Pos curve L", "%", { 0, 1, .5 });
	addRangeParam(Params::kParamOutPosCurveR, "Output Pos curve R", "%", { 0, 1, .5 });

	addRangeParam(Params::kParamResonatorDim, "Dimension", "D", { 1, maxDimension, 5 })->getInfo().stepCount = maxDimension - 1;
	addRangeParam(Params::kParamResonatorOrder, "Order", "", { 1, maxOrder, maxOrder })->getInfo().stepCount = maxOrder - 1;
	addRangeParam(Params::kParamResonatorFreq, "Freq", "Hz", { 100, 2000, 500 });
	addRangeParam(Params::kParamResonatorDamp, "Dampening", "%", { 0, 10, 1 });
	addRangeParam(Params::kParamResonatorVel, "Sonic Vel", "m/s", { .1, 1000, 10 });
	addRangeParam(Params::kParamVUPPM, "Output Level", "dB", { 0, 1, 0 }, ReadOnly);


	setCurrentUnitID(postSection);
	addRangeParam(Params::kParamLCFreq, "Low Cut Freq", "Hz", { 20, 2000, 500 });
	addRangeParam(Params::kParamLCQ, "Low Cut Q", "", { 1, 8, 1 });
	addRangeParam(Params::kParamHCFreq, "High Cut Freq", "Hz", { 100, 2000, 500 });

	setCurrentUnitID(inputPositionUnitId);
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


	return kResultTrue;
}

IPlugView* PLUGIN_API Controller::createView(FIDString name) {
	//if (ConstString(name) == ViewType::kEditor) return new VSTGUI::VST3Editor(this, "Editor", "editor.uidesc");
	return nullptr;
}

} // namespace TesseractFx
} // namespace Uberton