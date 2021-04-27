// -----------------------------------------------------------------------------------------------------------------------------
// This file is part of the Überton project. Copyright (C) 2021 Überton
//
// Überton is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// Überton is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
// of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details. You should
// have received a copy of the GNU General Public License along with Überton. If not, see http://www.gnu.org/licenses/.
// -----------------------------------------------------------------------------------------------------------------------------


#include "vstmath.h"

namespace Uberton {
namespace Math {


//double sin_lookup[sin_lookup_size + 1];
double cos_lookup[trig_lookup_size + 1];

void init_trig_lookup() {
	for (int i = 0; i < trig_lookup_size + 1; i++) {
		cos_lookup[i] = std::cos(i * twopi<double>() / trig_lookup_size);
	}
}

// cos lookup table. Takes optimized about 2/3 time of std::cos() but twice as long in Debug mode
double cos_lut(double x) {
	constexpr double factor = trig_lookup_size * r_twopi<double>();
	return cos_lookup[std::abs((int)(x * factor) % trig_lookup_size)];
}

double sin_lut(double x) {
	constexpr double pi_halfs = pi<double>() / 2;
	return cos_lut(x - pi_halfs);
}
// Really good and smooth approximation. About twice to three times as fast as std::cos when optimized but
// 3-5 times slower in Debug mode
template<typename T>
inline T cos_approx(T x) noexcept {
	constexpr T tp = 1. / (2. * 3.14159265358);
	x *= tp;
	x -= T(.25) + std::floor(x + T(.25));
	x *= T(16.) * (std::abs(x) - T(.5));
	x += T(.225) * x * (std::abs(x) - T(1.));

	return x;
}

// constexpr cos, best for interval [-2π,2π]
template<typename T>
constexpr T ccos(T x) {
	auto z = x * x;
	return 1 - z * (1.0 / 2.0 - z * (1.0 / 24.0 - z * (1.0 / 720.0 - z * (1.0 / 40320.0 - z * (1.0 / 3628800.0 - z * (1.0 / 479001600.0))))));
}

template<typename T>
constexpr T csin(T x) {
	auto z = x * x;
	return x * (1 - z * (1 / 6.0 - z * (1 / 120.0 - z * (1 / 5040.0 - z * (1 / 362880.0)))));
}


} // namespace Math
} // namespace Uberton