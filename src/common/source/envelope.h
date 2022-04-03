
// Envelope generators.
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

namespace Uberton {
// ADSR evenlope with quadratic attack, decay and release curves:
//     .
//    /|
//   /  \________
//  |            |
//  |             \_____
//   A  D   S     R
//
// Technically (numerically), A/D/R times up to 10'000'000 samples should be no problem
// (100 seconds order of magnitude at 44100 kHz sampling rate). Probably even more.
//
//            .·
//         .˙  |·
//       ·     | ˙
//     ·       |   ˙·.
//   .         |       ˙ · · · · · · · · ·
//  ·          |         |              | ˙.
// ·      A    |    D    |      S       | R  ·
// .           |         |              |      ˙· .  .  .  .  .
//
class QuadraticADSREnvelope
{
	using Int = long long;

public:
	// setParams should better not be called betwee start() and release()
	void setParams(Int attackSamples, Int decaySamples, double sustainLevel, Int releaseSamples) noexcept {
		ca = 2 * 1.0 / (attackSamples * (attackSamples - 1.0));
		cd = -2 * (1.0 - sustainLevel) / (decaySamples * (decaySamples - 1.0));
		attackEnd = attackSamples;
		decayEnd = attackEnd + decaySamples;
		releaseTime = releaseSamples;
	}

	void reset() noexcept {
		index = 0;
		value = 0;
		constant = false;
		c0 = ca;
		a0 = attackEnd;
		nextBreak = attackEnd;
	}

	void release() noexcept {
		c0 = -2.0 * value / (releaseTime * (releaseTime - 1.0));
		a0 = nextBreak = releaseEnd = index + releaseTime;
		constant = false;
	}

	double next() noexcept {
		if (constant) return value;
		value += c0 * (a0 - 1.0 - index);
		if (++index == nextBreak) {
			if (nextBreak == attackEnd) { // finished attack period
				c0 = cd;
				a0 = decayEnd;
				nextBreak = decayEnd;
			} else if (nextBreak == decayEnd) { // finished decay period
				constant = true;
			} else if (nextBreak == releaseEnd) { // finished release period
				value = 0;
				constant = true;
			}
		}
		return value;
	}


	bool hasReachedSustain() const noexcept {
		return index == decayEnd;
	}

	bool isFinished() const noexcept {
		return index == releaseEnd;
	}


private:
	Int attackEnd{ 1 };
	Int decayEnd{ 1 };
	Int releaseEnd{ 1 };
	Int releaseTime{ 1 };

	Int a0{};
	double c0{};

	bool constant{ false }; // true during sustain and after release
	Int nextBreak{};		// index of next period switch

	double ca{}, cd{}; // time coefficients for attack and release period
	Int index{ 0 };	   // sample count
	double value{ 0 }; // current velocity
};




//
// Attack, Decay and Release can be changed during their phases and the ramp will be adjusted.
// Decay and Release can also be changed before their phases (unnecessarily obvious for Attack)
// Sustain can only be changed before the sustain phase has started.
// Other changes are allowed but have no effect.
//
//       /\
//      /  \
//     /    \__________
//    /                \
//   /                  \___
//
class LinearADSREnvelope
{
	// using a sample rate of 192000
	// we can use this for up to
	//  - (2^31-1)/192000 ≈ 3 hours with 32 bit int
	//  -  (2^63-1)/192000 ≈ 13344 million hours with 32 bit int
	using Int = long long;

public:
	void set(Int attackSamples, Int decaySamples, double sustainLevel, Int releaseSamples) noexcept {
		a = attackSamples;
		d = decaySamples;
		s = sustainLevel;
		r = releaseSamples;

		if (index < attackEnd) { // if still in attack phase, update attack ramp, otherwise ignore new attack value
			currentRamp = 1. / a;
			attackEnd = static_cast<Int>((1. - value) * a) + index;
			nextBreak = attackEnd;
		} else if (index < decayEnd) { // if still in decay phase, update decay ramp, otherwise ignore new attack value
			currentRamp = (s - 1.) / d;
			decayEnd = static_cast<Int>(-(value - s) / currentRamp) + index;
			nextBreak = decayEnd;
		} else if (index < releaseEnd) {
			currentRamp = -releaseValue / r;
			releaseEnd = static_cast<Int>(-value / currentRamp) + index;
			nextBreak = releaseEnd;
		}
	}

	void reset() noexcept {
		index = 0;
		value = 0;
		constant = false;
		currentRamp = 1. / a;
		attackEnd = a;
		nextBreak = attackEnd;
		releaseEnd = -1;
	}

	void release() noexcept {
		releaseValue = value;
		currentRamp = -value / r;
		releaseEnd = index + r;
		nextBreak = releaseEnd;
		attackEnd = 0; // important in case release() is called during attack phase
		decayEnd = 0;  // important in case release() is called during decay phase
		constant = false;
	}

	double next() noexcept {
		if (!constant) {
			value += currentRamp;
			if (++index == nextBreak) {
				if (nextBreak == attackEnd) {
					currentRamp = (s - 1.) / d;
					decayEnd = attackEnd + d;
					nextBreak = decayEnd;
				} else if (nextBreak == decayEnd) {
					constant = true;
				} else if (nextBreak == releaseEnd) {
					value = 0;
					constant = true;
				}
			}
		}
		return value;
	}

	bool hasReachedSustain() const noexcept {
		return index == decayEnd;
	}

	bool isFinished() const noexcept {
		return index == releaseEnd;
	}

	bool isInRelease() const noexcept {
		return nextBreak == releaseEnd;
	}


private:
	double currentRamp{};

	Int a{}, d{}, r{};
	double s{};

	Int attackEnd{};
	Int decayEnd{};
	Int releaseEnd{ -1 };

	bool constant{ false }; // true during sustain and after release
	int nextBreak{};		// index of next period switch

	Int index{};	// sample count
	double value{}; // current velocity

	double releaseValue{}; // value at time of release
};


// 
// Same specification as LinearADSREnvelope but with power functions as curves which can be 
// linear (1), quadratic (2), cubic (3) or quartic (4). 
// 
// The parameter can be chosen to resemble an exponential curve. This way, when applied to a level,
// the change in perceived loudness is approximately linear while for a linear envelope it would be 
// logarithmic. All curves are concave. 
// 
//     .
//     |
//    | \________
//   /           |
//  /             \_____
//

template<int n>
class NaturalPowerADSREnvelope
{
	using Int = long long;

public:
	static_assert(n >= 1 && n <= 4, "Template parameter n can only take values 1,2,3,4");


	void set(Int attackSamples, Int decaySamples, double sustainLevel, Int releaseSamples) noexcept {
		a = attackSamples;
		d = decaySamples;
		if constexpr (n == 1)
			s = sustainLevel;
		else if constexpr (n == 2)
			s = std::sqrt(sustainLevel);
		else if constexpr (n == 3)
			s = std::pow(sustainLevel, 1.0 / 3.0);
		else if constexpr (n == 4)
			s = std::pow(sustainLevel, 1.0 / 4.0);
		r = releaseSamples;

		if (index < attackEnd) { // if still in attack phase, update attack ramp, otherwise ignore new attack value
			currentRamp = 1. / a;
			attackEnd = static_cast<Int>((1. - value) * a) + index;
			nextBreak = attackEnd;
		} else if (index < decayEnd) { // if still in decay phase, update decay ramp, otherwise ignore new attack value
			currentRamp = (s - 1.) / d;
			decayEnd = static_cast<Int>(-(value - s) / currentRamp) + index;
			nextBreak = decayEnd;
		} else if (index < releaseEnd) {
			currentRamp = -releaseValue / r;
			releaseEnd = static_cast<Int>(-value / currentRamp) + index;
			nextBreak = releaseEnd;
		}
	}

	void reset() noexcept {
		index = 0;
		value = 0;
		constant = false;
		currentRamp = 1. / a;
		attackEnd = a;
		nextBreak = attackEnd;
		releaseEnd = -1;
	}

	void release() noexcept {
		releaseValue = value;
		currentRamp = -value / r;
		releaseEnd = index + r;
		nextBreak = releaseEnd;
		attackEnd = 0; // important in case release() is called during attack phase
		decayEnd = 0;  // important in case release() is called during decay phase
		constant = false;
	}

	double next() noexcept {
		if (!constant) {
			value += currentRamp;
			if (++index == nextBreak) {
				if (nextBreak == attackEnd) {
					currentRamp = (s - 1.) / d;
					decayEnd = attackEnd + d;
					nextBreak = decayEnd;
				} else if (nextBreak == decayEnd) {
					constant = true;
				} else if (nextBreak == releaseEnd) {
					value = 0;
					constant = true;
				}
			}
		}
		if constexpr (n == 1) {
			return value;
		} else if constexpr (n == 2) {
			return value * value;
		} else if constexpr (n == 3) {
			auto sq = value * value;
			return value * sq;
		} else if constexpr (n == 4) {
			auto sq = value * value;
			return sq * sq;
		}
	}

	bool hasReachedSustain() const noexcept {
		return index == decayEnd;
	}

	bool isFinished() const noexcept {
		return index == releaseEnd;
	}

	bool isInRelease() const noexcept {
		return nextBreak == releaseEnd;
	}


private:
	double currentRamp{};

	Int a{}, d{}, r{};
	double s{};

	Int attackEnd{};
	Int decayEnd{};
	Int releaseEnd{ -1 };

	bool constant{ false }; // true during sustain and after release
	int nextBreak{};		// index of next period switch

	Int index{};	// sample count
	double value{}; // current velocity

	double releaseValue{}; // value at time of release
};


}