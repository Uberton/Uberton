
// Basic oscillator classes to generate sine/rect/trig and more wave shapes
// The current phase is handled internally and updated with each call to get().
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

#include "vstmath.h"

namespace Uberton {

class Oscillator
{
protected:
	double freq{ 0 };
	double phase{ 0 };
	double phaseInc{ 0 };
	double sampleRate{ 44100 };

	void updatePhaseInc() {
		phaseInc = 2 * Math::pi<double>() * freq / sampleRate;
	}

public:
	Oscillator() {
	}

	void setSampleRate(double sampleRate) {
		this->sampleRate = sampleRate;
		updatePhaseInc();
	}

	void setFrequency(double frequency) {
		freq = frequency;
		updatePhaseInc();
	}

	void setPhase(double phase) {
		this->phase = phase;
	}

	virtual double get() = 0;
};

class SineOscillator : public Oscillator
{
public:
	double get() final {
		double sample = std::sin(phase);
		phase += phaseInc;
		return sample;
	}
};

class SineOscillatorPow5 : public Oscillator
{
public:
	double get() final {
		//double sample = std::pow(std::sin(phase), 5);
		double sample = std::pow(std::sin(phase), 5);
		phase += phaseInc;
		return sample;
	}
};

class RectOscillator : public Oscillator
{
public:
	double get() final {
		double sample = (phase > Math::pi<double>()) ? 1 : -1;
		;
		phase += phaseInc;
		if (phase >= Math::twopi<double>())
			phase -= Math::twopi<double>();
		return sample;
	}
};

class TriangleOscillator : public Oscillator
{
public:
	double get() final {
		double sample = -1.0 + (2.0 * phase * Math::r_twopi<double>());
		phase += phaseInc;
		if (phase >= Math::twopi<double>())
			phase -= Math::twopi<double>();
		return 2.0 * (abs(sample) - 0.5);
	}
};

} // namespace Uberton