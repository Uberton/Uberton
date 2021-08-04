// -----------------------------------------------------------------------------------------------------------------------------
// This file is part of the �berton project. Copyright (C) 2021 �berton
//
// �berton is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// �berton is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
// of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details. You should
// have received a copy of the GNU General Public License along with �berton. If not, see http://www.gnu.org/licenses/.
// -----------------------------------------------------------------------------------------------------------------------------


#pragma once

#include <ControllerBase.h>
#include "ids.h"

namespace Uberton {
namespace BasicFx {
using namespace Steinberg;
using namespace Steinberg::Vst;

class Controller : public ControllerBase<ParamState, ImplementBypass>
{
public:
	tresult PLUGIN_API initialize(FUnknown* context) SMTG_OVERRIDE;
	IPlugView* PLUGIN_API createView(FIDString name) SMTG_OVERRIDE;

	static FUnknown* createInstance(void*) { return (Vst::IEditController*)new Controller(); }
};

}
}