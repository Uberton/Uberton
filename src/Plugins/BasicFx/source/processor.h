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

#include <ProcessorBase.h>
#include "ids.h"

namespace Uberton {
namespace BasicFx {

class Processor : public ProcessorBaseP<UniformParamState<1>, ImplementBypass>
{
public:
	Processor();

	tresult PLUGIN_API initialize(FUnknown* context) SMTG_OVERRIDE;

	void processAudio(ProcessData& data) override;
	void processParameterChanges(IParameterChanges* parameterChanges) override;

	static FUnknown* createInstance(void*) { return (Vst::IAudioProcessor*)new Processor(); }
};

}
}