
// Controller classes with extended features such as exporting a bypass parameter by default
// or setting the component state through a ParamStateClass (see below).
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

#include <public.sdk/source/vst/vsteditcontroller.h>
#include <pluginterfaces/base/ustring.h>
#include <array>


namespace Uberton {
using namespace Steinberg;
using namespace Steinberg::Vst;

/*
 * - can automatically add a bypass parameter(set implementBypass to false in constructor to avoid doing so)
 */
class ControllerBase : public EditControllerEx1
{
public:
	ControllerBase();

	tresult PLUGIN_API initialize(FUnknown* context) SMTG_OVERRIDE;

	bool implementBypass = true;
};

/*
 * Controller base class that uses some kind of global parameter state with specifications
 * as described in parameters.h. The bypass parameter is exported automatically if enabled.
 * The component state is set through the member function of the ParamStateClass class.
 */
template<class ParamStateClass, bool hasBypass>
class ControllerBaseP : public EditControllerEx1
{
public:
	ControllerBaseP() {}

	tresult PLUGIN_API initialize(FUnknown* context) SMTG_OVERRIDE {
		tresult result = EditControllerEx1::initialize(context);
		if (result != kResultTrue) return result;

		if (hasBypass)
			parameters.addParameter(Steinberg::UString256("Bypass"), nullptr, 1, 0, ParameterInfo::kIsBypass | ParameterInfo::kCanAutomate, bypassId);

		return kResultTrue;
	}

	// Called when a preset is loaded
	tresult PLUGIN_API setComponentState(IBStream* state) SMTG_OVERRIDE {
		ParamStateClass paramState;
		tresult result = paramState.setComponentState(state, *this);
		if (hasBypass)
			setParamNormalized(bypassId, paramState.isBypassed());
		return result;
	}

	void addRangeParam(Vst::ParamID id, UString256 name, UString256 units, std::array<ParamValue, 3> minMaxDefault, bool readonly = false) {
		int flags = 0;
		if (readonly) {
			flags |= ParameterInfo::kIsReadOnly;
		}
		else {
			flags |= ParameterInfo::kCanAutomate;
		}
		auto p = parameters.addParameter(new RangeParameter(name, id, units, minMaxDefault[0], minMaxDefault[1], minMaxDefault[2], 0, flags));
		p->setUnitID(currentUnitID);
	}

	// all parameters created with addRangeParam() after setting current unit id will get this id
	void setCurrentUnitID(UnitID id) {
		currentUnitID = id;
	}

	UnitID currentUnitID = kRootUnitId;
	const bool ReadOnly = true;
};


}