// -----------------------------------------------------------------------------------------------------------------------------
// This file is part of the Überton project. Copyright (C) 2021 Überton
//
// Überton is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// Überton is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
// of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details. You should
// have received a copy of the GNU General Public License along with Überton. If not, see http://www.gnu.org/licenses/.
// -----------------------------------------------------------------------------------------------------------------------------


#include "ControllerBase.h"
#include "parameters.h"
#include <pluginterfaces/base/ustring.h>
#include <base/source/fstreamer.h>

namespace Uberton {

ControllerBase::ControllerBase() {}

tresult PLUGIN_API ControllerBase::initialize(FUnknown* context) {
	tresult result = EditControllerEx1::initialize(context);
	if (result != kResultOk) return result;

	//addUnit(new Unit(USTRING("Unit1"), 1));

	if (implementBypass)
		parameters.addParameter(USTRING("Bypass"), nullptr, 1, 0, ParameterInfo::kIsBypass | ParameterInfo::kCanAutomate, bypassId);

	return kResultOk;
}

}