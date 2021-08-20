
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
#include "pluginterfaces/base/ustring.h"
#include <array>
#include <cmath>

namespace Uberton {
using namespace Steinberg;
using namespace Steinberg::Vst;

/*
 * Wrapper for a global parameter state with N parameters of type ParamValue (double).
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



/*
 * Helper functions for converting between normalized discrete and scaled values
 */

/// Turn discrete value between min and max to normalized value from 0 to 1
inline double discreteToNormalized(int value, int min, int max) {
	return (value - min) / static_cast<double>(max - min);
}

/// Turn normalized value from 0 to 1 to discrete value between min and max
inline int normalizedToDiscrete(double value, int min, int max) {
	return std::min<int>(max - min, static_cast<int>(value * (max - min + 1))) + min;
}

/// Linearily convert normalized value in [1, 1] to value in [min, max]
inline double normalizedToScaled(double value, double min, double max) {
	return value * (max - min) + min;
}

/// Linearily cConvert value in [min, max] to normalized value in [0, 1]
inline double scaledToNormalized(double value, double min, double max) {
	return (value - min) / (max - min);
}


/* 
 * Linear ramped parameter. 
 * step() needs to be called each sample
 */
template<class FloatType>
class RampedValue
{
public:
	RampedValue(FloatType value, FloatType numRampSamples)
		: value(value), targetValue(value), numRampSamples(numRampSamples) {}

	void set(FloatType newValue) noexcept {
		if (newValue == targetValue) return;
		targetValue = newValue;
		countdown = numRampSamples;
		delta = (targetValue - value) / numRampSamples;
	}

	FloatType get() const noexcept {
		return value;
	}

	// returns true when finished
	bool step() noexcept {
		if (countdown <= 0) return true;
		countdown--;
		value = targetValue + countdown * delta;
		//value += ramp; // might over-/undershoot a bit because of fp precision
		return false;

		delta *= countdown;
		value += delta;
	}

private:
	FloatType value;
	FloatType targetValue;
	FloatType delta{ 0 };
	int countdown{ 0 };
	const FloatType numRampSamples;
};


/*
 * Convert dezibels to normalized volume 
 */
template<class T>
T dBtoVolume(T dB) {
	return static_cast<T>(std::pow(10.0, 0.05f * dB));
}

/*
 * Convert normalized volume to dezibels
 */
template<class T>
T volumeTodB(T volume) {
	return static_cast<T>(20.0 * std::log10(volume));
}


/*
 * @Deprecated
 * Parameter specification with id, minimum, maximum, default as well as initial value.
 * 
 * Provides helper functions for converting between normalized and scaled/discrete using the
 * stored info about min and max. 
 */
struct ParamSpec
{
	constexpr ParamSpec(int32_t id, double min, double max, double defaultValue, double initialValue)
		: id(id), minValue(min), maxValue(max),
		  defaultValue(defaultValue), initialValue(initialValue) {}

	constexpr ParamSpec(int32_t id, const double (&minMaxDefault)[3], double initialValue)
		: id(id), minValue(minMaxDefault[0]), maxValue(minMaxDefault[1]),
		  defaultValue(minMaxDefault[2]), initialValue(initialValue) {}

	const int32_t id;
	const double initialValue;
	const double defaultValue;
	const double minValue;
	const double maxValue;

	double fromDiscrete(int value) const {
		return discreteToNormalized(value, minValue, maxValue);
	}

	int toDiscrete(double value) const {
		return normalizedToDiscrete(value, minValue, maxValue);
	}

	double toNormalized(double value) const {
		return scaledToNormalized(value, minValue, maxValue);
	}

	double toScaled(double value) const {
		return normalizedToScaled(value, minValue, maxValue);
	}
};


/*
 * Scales for converting normalized value to plain ones. 
 * -----------------------------------------------------
 */

/*
 * Linear scales that linearily maps from [0, 1] to [min, max]
 */
class LinearScale
{
public:
	LinearScale(double min = 0, double max = 1) : min(min), max(max) {}

	double toScaled(double value) const {
		return value * (max - min) + min;
	}

	double toNormalized(double value) const {
		return (value - min) / (max - min);
	}

	double getMin() const { return min; };
	double getMax() const { return max; };

private:
	double min;
	double max;
};

/*
 * Discrete scales that linearily maps from [0, 1] to [min, max] while min and max being integers. 
 */
class DiscreteScale
{
public:
	DiscreteScale(int min = 0, int max = 1) : min(min), max(max) {}

	double toScaled(double value) const {
		return normalizedToDiscrete(value, min, max);
	}

	double toNormalized(double value) const {
		return (value - min) / static_cast<double>(max - min);
	}

	int toDiscrete(double value) const {
		return normalizedToDiscrete(value, min, max);
	}

	double fromDiscrete(int value) const {
		return discreteToNormalized(value, min, max);
	}

	int getMin() const { return min; };
	int getMax() const { return max; };

private:
	int min;
	int max;
};

/*
 * Logarithmic function of the form
 *     f(x) = a·bˣ + c
 * that maps [0, 1] to [min, max].
 */
class LogarithmicScale
{
public:
	LogarithmicScale(double min = 0, double max = 1, double base = 10.0) {
		b = base;
		logb_inv = 1 / std::log(b);
		a = (min - max) / (1 - b);
		c = min - a;
	}

	double toScaled(double value) const {
		return a * std::pow(b, value) + c;
	}

	double toNormalized(double value) const {
		return std::log((value - c) / a) * logb_inv;
	}

private:
	double a;
	double b;
	double logb_inv;
	double c;
};

/*
 * Power function of the form
 *     f(x) = a·xᵇ + c
 * that maps [0, 1] to [min, max] and additionally fulfills the equation
 *     f(x₁) = y₁.
 * 
 * This can be used as a good approximation of a logarithmic scale such as a frequency parameter. 
 */
class PowerScale
{
public:
	PowerScale(double min = 0, double max = 1, const double (&pair)[2] = { .5, .5 }) : min(min) {
		a = max - min;
		b = std::log((pair[1] - min) / a) / std::log(pair[0]);
	}

	double toScaled(double value) const {
		return a * std::pow(value, b) + min;
	}

	double toNormalized(double value) const {
		return std::pow((value - min) / a, 1.0 / b);
	}

private:
	double a;
	double b;
	double min;
};



/*
 * ParamSpec classes to store a parameters id, default and initial value 
 * as well as a scale for converting to and from normalized values. 
 * ---------------------------------------------------------------------
 */

template<class Scale>
struct ParamSpecBase
{
	ParamSpecBase(int32_t id, double defaultValue, double initialValue) : id(id), defaultValue(defaultValue), initialValue(initialValue) {}
	double toNormalized(double value) const { return scale.toNormalized(value); }
	double toScaled(double value) const { return scale.toScaled(value); }

	using ScaleType = Scale;
	Scale scale;
	int32_t id;
	double defaultValue;
	double initialValue;
};

struct LinearParamSpec : public ParamSpecBase<LinearScale>
{
	LinearParamSpec(int32_t id, double min, double max, double defaultValue, double initialValue)
		: ParamSpecBase<LinearScale>(id, defaultValue, initialValue) {
		scale = { min, max };
	}
};

struct DiscreteParamSpec : public ParamSpecBase<DiscreteScale>
{
	DiscreteParamSpec(int32_t id, int min, int max, int defaultValue, int initialValue)
		: ParamSpecBase<DiscreteScale>(id, defaultValue, initialValue) {
		scale = { min, max };
	}

	int toDiscrete(double value) const {
		return scale.toDiscrete(value);
	}
};

struct LogParamSpec : public ParamSpecBase<LogarithmicScale>
{
	LogParamSpec(int32_t id, double min, double max, double defaultValue, double initialValue, double base = 10.0)
		: ParamSpecBase<LogarithmicScale>(id, defaultValue, initialValue) {
		scale = { min, max, base };
	}
};

struct PowerParamSpec : public ParamSpecBase<PowerScale>
{
	PowerParamSpec(int32_t id, double min, double max, double defaultValue, double initialValue, const double (&pair)[2])
		: ParamSpecBase<PowerScale>(id, defaultValue, initialValue) {
		scale = { min, max, pair };
	}
};

/*
 * Vst Parameter classes that convert to and from normalized values through a scale. 
 * The constructor takes a corresponding ParamSpec (i.e. linear or logarithmic). 
 * ---------------------------------------------------------------------------------
 */

template<class ParamSpec>
class ScaledParameter : public Vst::Parameter
{
public:
	ScaledParameter(const ParamSpec& paramSpec, const TChar* title, const TChar* shortTitle = nullptr, const TChar* units = nullptr, int32 flags = ParameterInfo::kCanAutomate, UnitID unitID = 0)
		: Parameter(title, paramSpec.id, units, paramSpec.toNormalized(paramSpec.defaultValue), 0, flags, unitID, shortTitle), scale(paramSpec.scale) {
		setPrecision(1);
	}

	void toString(ParamValue value, String128 string) const SMTG_OVERRIDE {
		UString128 wrapper;
		wrapper.printFloat(toPlain(value), precision);
		wrapper.copyTo(string, 128);
	}

	bool fromString(const TChar* string, ParamValue& value) const SMTG_OVERRIDE {
		UString wrapper((TChar*)string, strlen16(string));
		if (wrapper.scanFloat(value)) {
			value = toNormalized(value);
			return true;
		}
		return false;
	}

	ParamValue toPlain(ParamValue value) const SMTG_OVERRIDE {
		return scale.toScaled(value);
	}

	ParamValue toNormalized(ParamValue value) const SMTG_OVERRIDE {
		return scale.toNormalized(value);
	}

	using ParamSpecType = ParamSpec;

	const typename ParamSpec::ScaleType& scale;
};


class LinearParameter : public ScaledParameter<LinearParamSpec>
{
public:
	using ScaledParameter<ScaledParameter::ParamSpecType>::ScaledParameter;
};

class DiscreteParameter : public ScaledParameter<DiscreteParamSpec>
{
public:
	using ScaledParameter<ScaledParameter::ParamSpecType>::ScaledParameter;
};

class LogParameter : public ScaledParameter<LogParamSpec>
{
public:
	using ScaledParameter<ScaledParameter::ParamSpecType>::ScaledParameter;
};

class PowerParameter : public ScaledParameter<PowerParamSpec>
{
public:
	using ScaledParameter<ScaledParameter::ParamSpecType>::ScaledParameter;
};


/*
 * A special gain VST Parameter 
 */
class GainParameter : public Vst::Parameter
{
public:
	GainParameter(const TChar* title, ParamID tag, const TChar* units, ParamValue defaultValue = 0.0, int32 flags = ParameterInfo::kCanAutomate, UnitID unitID = 0, const TChar* shortTitle = nullptr)
		: Parameter(title, tag, units, defaultValue, 0, flags, unitID, shortTitle) {
	}

	void toString(ParamValue value, String128 string) const SMTG_OVERRIDE {
		UString128 wrapper;
		double dB = volumeTodB(value);
		if (value > 0.0001) {
			wrapper.printFloat(dB, precision);
		}
		else {
			wrapper.append(L"-oo");
		}
		wrapper.copyTo(string, 128);
	}

	bool fromString(const TChar* string, ParamValue& value) const SMTG_OVERRIDE {
		UString wrapper((TChar*)string, strlen16(string));

		if (wrapper.scanFloat(value)) {
			if (value > 0.0) value = 0.0;
			value = dBtoVolume(value);
			return true;
		}
		return false;
	}
};

} // namespace Uberton