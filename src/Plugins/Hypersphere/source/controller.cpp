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
namespace Hypersphere {


tresult PLUGIN_API Controller::initialize(FUnknown* context) {

	tresult result = ResonatorController::initialize(context);
	if (result != kResultTrue) return result;


	UnitID rootUnitId = 1;

	setCurrentUnitID(rootUnitId);
	{
		addParam<DiscreteParameter>(ParamSpecs::resonatorDim, "Resonator Dimension", "Res Dim", "D", Precision(0))->getInfo().stepCount = maxDimension - 2;
		addParam<DiscreteParameter>(ParamSpecs::resonatorOrder, "Resonator Order", "Res Order", "", Precision(0))->getInfo().stepCount = maxOrder - 1;
	}
	auto add = [&](const LinearParamSpec& paramSpec, ParamID id, const wchar_t* name, int dimension, const wchar_t* units = L"") {
		LinearParamSpec spec = paramSpec;
		spec.id = id;
		UString256 namestr(name);
		if (dimension >= 0) {
			UString256 a("", 10);
			a.printInt(dimension);
			namestr.append(a);
		}
		addParam<LinearParameter>(spec, namestr, namestr, units, Precision(4));
	};

	setCurrentUnitID(inputPositionUnitId);
	{
		add(ParamSpecs::resonatorInputRCoordinate, Params::kParamInL0, L"Left r", -1);
		add(ParamSpecs::resonatorInputPhiCoordinate, Params::kParamInL0 + 1, L"Left φ", -1);
		for (int i = 2; i < maxDimension; i++) {
			add(ParamSpecs::resonatorInputThetaCoordinate, Params::kParamInL0 + i, L"Left ϑ", i - 1, L"π");
		}
		add(ParamSpecs::resonatorInputRCoordinate, Params::kParamInR0, L"Right r", -1);
		add(ParamSpecs::resonatorInputPhiCoordinate, Params::kParamInR0 + 1, L"Right φ", -1);
		for (int i = 2; i < maxDimension; i++) {
			add(ParamSpecs::resonatorInputThetaCoordinate, Params::kParamInR0 + i, L"Right ϑ", i - 1, L"π");
		}
	}
	setCurrentUnitID(outputPositionUnitId);
	{
		add(ParamSpecs::resonatorOutputRCoordinate, Params::kParamOutL0, L"Left r", -1);
		add(ParamSpecs::resonatorOutputPhiCoordinate, Params::kParamOutL0 + 1, L"Left φ", -1);
		for (int i = 2; i < maxDimension; i++) {
			add(ParamSpecs::resonatorOutputThetaCoordinate, Params::kParamOutL0 + i, L"Left ϑ", i - 1, L"π");
		}
		add(ParamSpecs::resonatorOutputRCoordinate, Params::kParamOutR0, L"Right r", -1);
		add(ParamSpecs::resonatorOutputPhiCoordinate, Params::kParamOutR0 + 1, L"Right φ", -1);
		for (int i = 2; i < maxDimension; i++) {
			add(ParamSpecs::resonatorOutputThetaCoordinate, Params::kParamOutR0 + i, L"Right ϑ", i - 1, L"π");
		}
	}
	return kResultTrue;
}

IPlugView* PLUGIN_API Controller::createView(FIDString name) {
	if (ConstString(name) == ViewType::kEditor) {
		auto editor = new VST3EditorEx1(this, "Editor", "editor.uidesc");
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
	double radius = velocity * std::sqrt((1.0 + dim - 2.0) / (w * w + b * b));
	radius = 1.0 / std::sqrt((w * w + b * b) / (velocity * velocity * (dim - 1)));
	double radiusNormalized = ParamSpecs::resonatorLength.toNormalized(radius);
	if (getParamNormalized(Params::kParamResonatorLength) != radiusNormalized) {
		setParamNormalized(Params::kParamResonatorLength, ParamSpecs::resonatorLength.toNormalized(radius));
	}
}

FUnknown* createControllerInstance(void*) {
	return static_cast<IEditController*>(new Controller);
}

} // namespace Hypersphere
} // namespace ResonatorPlugin
} // namespace Uberton
