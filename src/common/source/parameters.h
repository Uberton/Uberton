
// Organisation class for VST parameters. Implements the setState() and getState() functions.
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
#include <base/source/fstreamer.h>
#include <array>

namespace Uberton {
using namespace Steinberg;
using namespace Steinberg::Vst;

/*
 * Wrapper for a global parameter state with N parameters of type ParamValue.
 * Provides member functions for loading and storing the entire state through
 * an IBStream and setting an EditController's component state.
 *
 * A bypass is always implemented and stored/loaded as well as a version number
 * because adding a bypass parameter in a later version of a plugin would be hard
 * otherwise.
 */
template<uint32 N>
struct UniformParamState
{
	std::array<ParamValue, N> params{};
	bool bypass{ false };
	uint64 version = 0;

	bool isBypassed() const { return bypass; }
	void setBypass(bool state) { bypass = state; }

	tresult getState(IBStream* stream) {
		IBStreamer s(stream, kLittleEndian);
		if (!s.writeInt64u(version)) return kResultFalse;
		if (!s.writeBool(bypass)) return kResultFalse;

		for (uint32 id = 0; id < N; id++) {
			if (!s.writeDouble(params[id])) return kResultFalse;
		}
		return kResultOk;
	}

	tresult setState(IBStream* stream) {
		IBStreamer s(stream, kLittleEndian);
		uint64 version = 0;

		if (!s.readInt64u(version)) return kResultFalse;
		if (!s.readBool(bypass)) return kResultFalse;

		for (uint32 id = 0; id < N; id++) {
			if (!s.readDouble(params[id])) return kResultFalse;
		}
		return kResultOk;
	}

	tresult setComponentState(IBStream* stream, EditController& controller) {
		if (setState(stream) != kResultOk) return kResultFalse;

		for (uint32 id = 0; id < N; id++) {
			controller.setParamNormalized(id, params[id]);
		}
		return kResultOk;
	}

	ParamValue& operator[](int32 id) {
#ifdef _DEBUG
		if (id < 0 || id >= N) throw std::exception("Invalid param id");
#endif
		return params[id];
	}

	const ParamValue& operator[](int32 id) const {
#ifdef _DEBUG
		if (id < 0 || id >= N) throw std::exception("Invalid param id");
#endif
		return params[id];
	}
};


constexpr bool ImplementBypass = true;
constexpr bool NoBypass = false;
constexpr ParamID bypassId = 1000001;

/*
 * Concept for a parameter state wrapper.
 * Can be used with C++20
 */
//template<class T> concept ParamStateClass = requires
//(T p, IBStream* stream, EditController& controller, int32 id) {
//	{ p.getState(stream) } -> std::convertible_to<tresult>;
//	{ p.setState(stream) } -> std::convertible_to<tresult>;
//	{ p.setComponentState(stream, controller) } -> std::convertible_to<tresult>;
//	{ p[id] } -> std::convertible_to<ParamValue>;
//};


/// Turn discrete value between min and max to normalized ParamValue from 0 to 1
inline ParamValue discreteToNormalized(int value, int min, int max) {
	return (value - min) / static_cast<ParamValue>(max - min);
}

/// Turn normalized ParamValue from 0 to 1 tp discrete value between min and max
inline int normalizedToDiscrete(ParamValue value, int min, int max) {
	return std::min<int>(max - min, static_cast<int>(value * (max - min + 1))) + min;
}




} // namespace Uberton