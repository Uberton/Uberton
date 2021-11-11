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

namespace Uberton {
namespace ResonatorPlugin {
namespace Tesseract {


tresult PLUGIN_API Controller::initialize(FUnknown* context) {

	tresult result = ResonatorController::initialize(context);
	if (result != kResultTrue) return result;


	UnitID rootUnitId = 1;

	setCurrentUnitID(rootUnitId);
	{
		addParam<DiscreteParameter>(ParamSpecs::resonatorDim, "Resonator Dimension", "Res Dim", "D", Precision(0))->getInfo().stepCount = maxDimension - 1;
		addParam<DiscreteParameter>(ParamSpecs::resonatorOrder, "Resonator Order", "Res Order", "", Precision(0))->getInfo().stepCount = maxOrder - 1;
	}

	return kResultTrue;
}

IPlugView* PLUGIN_API Controller::createView(FIDString name) {
	if (ConstString(name) == ViewType::kEditor) {
		auto editor = new VST3EditorEx1(this, "Editor", "editor.uidesc", "Tesseract");
		editor->setPrescaleFactor(.5);
#ifdef Tesseract_USERGUIDE_PATH
		editor->setUserguidePath(Tesseract_USERGUIDE_PATH);
#endif
		return editor;
	}
	return nullptr;
}

void Controller::updateResonatorSizeDisplay() {
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

} // namespace Tesseract
} // namespace ResonatorPlugin
} // namespace Uberton
