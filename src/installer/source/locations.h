// OS specific locations for vst plugins, factory presets and user guide
//
// -----------------------------------------------------------------------------------------------------------------------------
// This file is part of the �berton project. Copyright (C) 2021 �berton
//
// �berton is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// �berton is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
// of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details. You should
// have received a copy of the GNU General Public License along with �berton. If not, see http://www.gnu.org/licenses/.
// -----------------------------------------------------------------------------------------------------------------------------

#include <string>

#pragma once

namespace Uberton {
namespace Installer {


std::string getVSTLocation();
std::string getFactoryPresetLocation();
std::string getUserguideLocation();
}
}
