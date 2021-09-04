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

/// Basic implementation of simple filters
namespace Uberton {

class Filter
{
public:
	enum class Mode {
		Lowpass,
		Highpass,
		Bandpass
	};

	Filter(Mode mode = Mode::Highpass) : mode(mode) {
		calculateFeedbackAmount();
	}

	double process(double input) {
		buf0 += cutoff * (input - buf0);
		buf1 += cutoff * (buf0 - buf1);
		switch (mode) {
		case Mode::Lowpass: return buf1;
		case Mode::Highpass: return input - buf0;
		case Mode::Bandpass: return buf0 - buf1;
		default: return 0.0;
		}
	}

	void setCutoff(double value) {
		cutoff = value;
		if (value > .999) // prevent divisions through zero
			cutoff = .999;
		calculateFeedbackAmount();
	}

	void setResonance(double value) {
		resonance = value;
		calculateFeedbackAmount();
	}

	void setMode(Mode newmode) {
		mode = newmode;
	}

private:
	double cutoff{ .99 };
	double resonance{ 0.0 };
	Mode mode = Mode::Lowpass;
	double feedbackAmount{ 0 };
	double buf0{ 0 };
	double buf1{ 0 };

	void calculateFeedbackAmount() {
		feedbackAmount = resonance + resonance / (1.0 - cutoff);
	}
};

}
