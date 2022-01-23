
// Utilities for processing code or Processor classes
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

namespace Uberton {
namespace ProcessorUtilities {


// Get a linear ramp from currentValue to newValue using the inverse of the ramp time in samples
// Usage example:
// 
//	const double rampTimeInv = 1.0 / numSamples;
//	double currentVolume = ... ;
//	const double newVolume = ... ;
//	const double volumeRamp = getRamp(currentVolume, newVolume, rampTimeInv);
//	
//	for (int i = 0; i < numSamples; i++) {
//	    ...
//	    currentVolume += volumeRamp;
//	}
//
template<class T>
T getRamp(T currentValue, T newValue, T rampTimeInv) {
	return currentValue == newValue ? 0 : (newValue - currentValue) * rampTimeInv;
}

}
}