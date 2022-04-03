
// Low frequency oscillators
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


class LFO
{
public:
	enum class Type {
		Sine,
		SlopedTriangle,
		Rectangle
	};

	void setSampleRate(double sampleRate) { this->sampleRate = sampleRate; }
	double getSampleRate() const { return sampleRate; }

	void setFrequency(double frequency) { freq = frequency; }
	double getFrequency() const { return freq; }

	void setStartingPhase(double startingPhase) { this->startingPhase = startingPhase; }
	double getStartingPhase() const { return startingPhase; }

	void setType(Type type) { this->type = type; }
	Type getType() const { return type; }

	// E.g. slope for triangle or duty cycle for rectangle
	void setShapeParameter(double shapeParameter) { this->shapeParameter = shapeParameter; }
	double getShapeParameter() const { return shapeParameter; };

	double next() {
		switch (type) {
		case Type::Sine: return 0;
		case Type::SlopedTriangle: return 0;
		case Type::Rectangle: return 0;
		}
	}

	void reset() {
		phase = startingPhase;
	}

private:
	Type type;
	double phase{};
	double startingPhase{};
	double freq{};
	double sampleRate{};

	double shapeParameter{};
};

}