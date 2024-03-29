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

#include "ids.h"
#include <ResonatorProcessor.h>

namespace Uberton {
namespace ResonatorPlugin {
namespace Tesseract {


class Processor : public ResonatorProcessorBase
{
public:
	Processor();

	tresult PLUGIN_API setActive(TBool state) SMTG_OVERRIDE;


private:
	void updateResonatorDimension() override;
	void recomputeInexpensiveParameters() override;
};

}
}
}