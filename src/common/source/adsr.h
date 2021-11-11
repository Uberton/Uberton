#pragma once

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
public:
	// setParams should better not be called betwee start() and release()
	void setParams(int attackSamples, int decaySamples, double sustainLevel, int releaseSamples) noexcept {
		ca = 2 * 1.f / (attackSamples * (attackSamples - 1.0));
		cd = -2 * (1.f - sustainLevel) / (decaySamples * (decaySamples - 1.0));
		attackEnd = attackSamples;
		decayEnd = attackEnd + decaySamples;
		releaseTime = releaseSamples;
	}

	void start() noexcept {
		index = 0;
		value = 0;
		constant = false;
		c0 = ca;
		a0 = attackEnd;
		nextBreak = attackEnd;
	}

	void release() noexcept {
		c0 = -2.f * value / (releaseTime * (releaseTime - 1.0));
		a0 = nextBreak = releaseEnd = index + releaseTime;
		constant = false;
	}

	double next() noexcept {
		if (constant) return value;
		value += c0 * (a0 - index - 1);
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


	bool hasReachedSustain() const {
		return index == decayEnd;
	}

	bool isFinished() const {
		return index == releaseEnd;
	}


private:
	int attackEnd{ 1 };
	int decayEnd{ 1 };
	int releaseEnd{ 1 };
	int releaseTime{ 1 };

	int a0;
	double c0;

	bool constant{ false }; // true during sustain and after release
	int nextBreak;			// index of next period switch

	double ca, cd;	   // time coefficients for attack and release period
	int index{ 0 };	   // sample count
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
public:
	void set(int attackSamples, int decaySamples, double sustainLevel, int releaseSamples) noexcept {
		a = attackSamples;
		d = decaySamples;
		s = sustainLevel;
		r = releaseSamples;

		if (index < attackEnd) { // if still in attack phase, update attack ramp, otherwise ignore new attack value
			currentRamp = 1. / a;
			attackEnd = static_cast<int>((1. - value) * a) + index;
			nextBreak = attackEnd;
		} else if (index < decayEnd) { // if still in decay phase, update decay ramp, otherwise ignore new attack value
			currentRamp = (s - 1.) / d;
			decayEnd = static_cast<int>(-(value - s) / currentRamp) + index;
			nextBreak = decayEnd;
		} else if (index < releaseEnd) {
			currentRamp = -releaseValue / r;
			releaseEnd = static_cast<int>(-value / currentRamp) + index;
			nextBreak = releaseEnd;
		}
	}

	void start() noexcept {
		index = 0;
		value = 0;
		constant = false;
		currentRamp = 1. / a;
		attackEnd = a;
		nextBreak = attackEnd;
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
		if (constant) return value;
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
		return value;
	}

	bool hasReachedSustain() const {
		return index == decayEnd;
	}

	bool isFinished() const {
		return index == releaseEnd;
	}


private:
	double currentRamp;

	int a, d, r;
	double s;

	int attackEnd{ 1 };
	int decayEnd{ 1 };
	int releaseEnd{ 1 };

	bool constant{ false }; // true during sustain and after release
	int nextBreak;			// index of next period switch

	int index{ 0 };	   // sample count
	double value{ 0 }; // current velocity

	double releaseValue{ 0 }; // value at time of release
};