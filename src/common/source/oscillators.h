
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


// Performance-optimized triangle oscillator with adjustable duty cycle to morph continously
// between symmetric triangle and sawtooth.
//
// Output values are between -1 and 1.
//
// The frequency should be less than half the sample rate (else the output may be greater than 1
// not to speak of aliasing issues).
//
// The duty cycle needs to be in the interval 0 to (excluding) 1. It is mathematically possible to 
// insert values between -1 and 0 to get an inverse sawtooth but this creates non-desired results 
// because of an optimization used internally. Setting the duty cycle to exactly 1 offsets the 
// frequency a bit, better use something like 0.999. 
//
// Changes to the osciallator parameters (frequency and duty) are updated immediately after the
// current (up- or downward) phase has finished.
//
struct TriangleDutyOscillator
{
	// duty ∈ [0,1] from triangle (=0) with equal angles to sawtooth (=1)
	TriangleDutyOscillator() {}

	void setSampleRate(double sampleRate) {
		this->sampleRate = sampleRate;
		update();
		reset();
	}

	void setParameters(double freq, double duty) {
		freq_inv = 1.0 / freq;
		this->duty = duty;
		update();
	}

	void reset() {
		phase = 0.0; // start at zero to get rid of cracks
		phaseInc = upPhaseInc;
	}

	double next() {
		double value = phase;
		phase += phaseInc;
		if (phase >= 1.0) {
			double overshoot = (phase - 1.0) * upPhaseInc_inv;
			phase += (downPhaseInc - upPhaseInc) * overshoot;
			phaseInc = downPhaseInc;

			// the down phase may be shorter if the duty cycle is very low for it
			// thus the overshoot correction already leads to values < -1.0
			// In this case we need to correct again for the other direction.
			//   There is no correction necessary for the other case because
			//    a) this oscillator is only made for f < .5*Fs
			//    b) this oscillator allows only duty cycles with the up phase longer or equal the down phase.
			if (phase <= -1.0) {
				double overshoot = (phase + 1.0) * downPhaseInc_inv;
				phase -= (downPhaseInc - upPhaseInc) * overshoot;
				phaseInc = upPhaseInc;
			}
		} else if (phase <= -1.0) {
			double overshoot = (phase + 1.0) * downPhaseInc_inv;
			phase -= (downPhaseInc - upPhaseInc) * overshoot;
			phaseInc = upPhaseInc;
		}
		return value;
	}

private:
	void update() {
		/*
				 p = steepness/2 + 1/2 ∈ [0.5, 1.0]
				 q = 1 - p ∈ [0.0, 0.5]

			  ∧                 _
			1 |       /\         |
			  |      / .\        |
			  |     /  . \       |
			  |    /   .  \      | Δy
			  |   /    .   \     |
			  |  /     .    \    |
			  | /      .     \   |
			  |/       .      \  |
			  |−−−−−−−−.−−−−−−−\−−−−−−−−−−−−−−−−/−−−−−> t
			0 |   Δx1     Δx2   \              /
			  |                  \            /
			  |                   \          /
			  |                    \        /
			  |                     \      /
			  |                      \    /
			  |                       \  /
		    -1 |                        \/


			  |__________________________________|
						 Δx = 1/f

						 Δy = 1

				Δx1 = Δx·p/2      Δx2 = Δx·(1-p)/2
				dy1 = Δy/Δx1·dx   dy2 = Δy/Δx2·dx

			 */
		double p = 0.5 * duty + 0.5; // ∈ [0.5, 1.0]
		double Dx1 = freq_inv * p * 0.5;
		double Dx2 = freq_inv * (1.0 - p) * 0.5;
		upPhaseInc_inv = Dx1 * sampleRate;

		if (duty < 1.0)
			downPhaseInc_inv = -Dx2 * sampleRate;
		else
			downPhaseInc_inv = -1.0;

		upPhaseInc = 1.0 / upPhaseInc_inv;
		downPhaseInc = 1.0 / downPhaseInc_inv;
	}

	double duty{};
	double freq_inv{};
	double sampleRate{};
	double phase{};
	double phaseInc{};
	double upPhaseInc{};
	double upPhaseInc_inv{};
	double downPhaseInc{};
	double downPhaseInc_inv{};
};

} // namespace Uberton