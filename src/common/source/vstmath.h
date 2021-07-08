
// Math facilities featuring
//  - Vector class
//  - Factorial
//  - Legendre polynomials (and associated legendre)
//  - Sine/cosine lookup and approximations
//  - Midi note to frequency mapping
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

#include <algorithm>
#include <array>
#include <numeric>
#include <functional>
#include <complex>

namespace Uberton {
namespace Math {

#ifdef _DEBUG
#define UBERTON_DEBUG_ASSERT(expr, msg) \
	if (!(expr)) throw std::exception(msg);
#else
#define UBERTON_DEBUG_ASSERT
#endif


template<class T>
using complex = std::complex<T>;
template<class T, int N>
using array = std::array<T, N>;

template<typename T>
constexpr T pi() { return static_cast<T>(3.1415926535897932384626); }
template<typename T>
constexpr T twopi() { return 2 * pi<T>(); }
template<typename T>
constexpr T r_twopi() { return T{ 1 } / twopi<T>(); }

/*
 * Basic class for representing a fixed length vector of dimension d and of type T.
 * T will probably be float or double
 */
template<class T, int d>
class Vector
{
	array<T, d> values;

public:
	using Index = ptrdiff_t;

	Vector(const T (&elems)[d]) {
		std::copy(std::begin(elems), std::end(elems), values.begin());
	}
	Vector(const std::initializer_list<T> elems) {
		size_t h = elems.size() < d ? elems.size() : d;
		std::copy(std::begin(elems), std::begin(elems) + h, values.begin());
		std::fill(values.begin() + h, values.end(), T{});
	}
	Vector(const array<T, d>& elems) {
		std::copy(std::begin(elems), std::end(elems), values.begin());
	}

	template<class U>
	Vector(const array<U, d>& elems) {
		for (int i = 0; i < d; i++) {
			values[i] = static_cast<T>(elems[i]);
		}
	}

	Vector(const T& value = 0) {
		std::fill(values.begin(), values.end(), value);
	}

	// Unchecked access (faster)
	const T& operator[](Index i) const { return values[i]; }
	T& operator[](Index i) { return values[i]; }
	// Runtime checked access (saver)
	const T& at(Index i) const { values.at(i); }
	T& at(Index i) { return values.at(i); }

	Vector& operator+=(const T& c) { return apply(std::plus<T>(), c); }
	Vector& operator-=(const T& c) { return apply(std::minus<T>(), c); }
	Vector& operator*=(const T& c) { return apply(std::multiplies<T>(), c); }
	Vector& operator/=(const T& c) { return apply(std::divides<T>(), c); }
	Vector& operator+=(const Vector& v) { return apply(std::plus<T>(), v); }
	Vector& operator-=(const Vector& v) { return apply(std::minus<T>(), v); }

	Vector operator+(const T& c) const { return Vector(*this) += c; }
	Vector operator-(const T& c) const { return Vector(*this) -= c; }
	Vector operator*(const T& c) const { return Vector(*this) *= c; }
	Vector operator/(const T& c) const { return Vector(*this) /= c; }
	Vector operator+(const Vector& v) const { return Vector(*this) += v; }
	Vector operator-(const Vector& v) const { return Vector(*this) -= v; }

	template<class F>
	Vector& apply(F f) {
		std::for_each(values.begin(), values.end(), f);
		return *this;
	}
	template<class F>
	Vector& apply(F f, const T& c) {
		std::for_each(values.begin(), values.end(), [&](T& v) { v = f(v, c); });
		return *this;
	}
	template<class F>
	Vector& apply(F f, const Vector& v) {
		std::transform(values.begin(), values.end(), v.values.begin(), values.begin(), f);
		return *this;
	}

	// Inner product of two vectors
	T operator*(const Vector& v) const {
		return std::inner_product(values.begin(), values.end(), v.values.begin(), T{ 0 });
	}

	friend std::ostream& operator<<(std::ostream& os, const Vector<T, d>& v) {
		os << "(";
		for (Index i = 0; i < d - 1; i++)
			os << v.values[i] << ",";
		return os << v.values[d - 1] << ")";
	}

	size_t size() const { return values.size(); }
};

template<class T, int d>
bool operator==(const Vector<T, d>& a, const Vector<T, d>& b) {
	using Index = typename Vector<T, d>::Index;
	for (Index i = 0; i < d; i++)
		if (a[i] != b[i]) return false;
	return true;
}
template<class T, int d>
bool operator!=(const Vector<T, d>& a, const Vector<T, d>& b) { return !(a == b); }

template<class T, int d>
Vector<T, d> operator-(const Vector<T, d>& v) { return v * -1; }
template<class T, int d>
Vector<T, d> operator+(T c, const Vector<T, d>& v) { return v + c; }
template<class T, int d>
Vector<T, d> operator-(T c, const Vector<T, d>& v) { return v - c; }
template<class T, int d>
Vector<T, d> operator*(T c, const Vector<T, d>& v) { return v * c; }




template<class T = double>
inline T factorial(int n) {
	return n <= 1 ? 1 : factorial<T>(n - 1) * n;
}

// Product of all integers from 1 to n with same parity (odd/even) as n
template<class T = double>
inline T doublefactorial(int n) {
	if (n == 0 || n == 1) return 1;
	return n * doublefactorial<T>(n - 2);
}



template<class T>
inline T legendre_p0(const T& x) {
	return static_cast<T>(1);
}

template<class T>
inline T legendre_p1(const T& x) {
	return x;
}

template<class T>
inline T legendre_p2(const T& x) {
	return ((static_cast<T>(3) * x * x) - static_cast<T>(1)) / static_cast<T>(2);
}

// Legendre polynom
template<class T>
inline T legendre(unsigned int n, const T& x) {
	switch (n) {
	case 0:
		return legendre_p0<T>(x);
	case 1:
		return legendre_p1<T>(x);
	case 2:
		return legendre_p2<T>(x);
	default:
		break;
	}

	auto pnm1(legendre_p2<T>(x));
	auto pnm2(legendre_p1<T>(x));
	T pn;

	for (auto m = 3u; m <= n; ++m) {
		pn = ((static_cast<T>((2 * m) - 1)) * x * pnm1 - (static_cast<T>(m - 1) * pnm2)) / static_cast<T>(m);
		pnm2 = pnm1;
		pnm1 = pn;
	}

	return pn;
}


// Recursive property of associated legendre polynoms
template<class T>
inline T assoc_legendre_next(int l, int m, T x, T Pl, T Plm1) {
	return ((2 * l + 1) * (x) * (Pl) - (l + m) * (Plm1)) / static_cast<T>(l + 1 - m);
}

template<class T>
inline T assoc_legendre_impl(int l, int m, T x, T sin_theta_power) {
	if (l < 0)
		return assoc_legendre_impl(-l - 1, m, x, sin_theta_power);

	if (m < 0) {
		int sign = (m & 1) ? -1 : 1;
		return sign * std::tgamma(static_cast<T>(l + m + 1)) / std::tgamma(static_cast<T>(l + 1 - m)) * assoc_legendre_impl(l, -m, x, sin_theta_power);
	}
	if (m > l) {
		return T{ 0 };
	}
	if (m == 0) {
		return legendre(l, x);
	}

	T p0 = static_cast<T>(doublefactorial(2 * m - 1)) * sin_theta_power;

	if (m & 1)
		p0 *= -1;
	if (m == l)
		return p0;
	T p1 = x * (2 * m + 1) * p0;
	int n = m + 1;

	while (n < l) {
		std::swap(p0, p1);
		p1 = assoc_legendre_next(n, m, x, p0, p1);
		++n;
	}

	//if ((m & 1u) != 0) {
	//	p1 = -p1;
	//}
	return p1;
}

// Associated Legendre polynom
template<class T>
inline auto assoc_legendre(int l, int m, T x) noexcept {
	auto d = static_cast<T>(pow(1 - x * x, T(abs(m)) / 2));
	return assoc_legendre_impl(l, m, x, d);
}




//template<typename T, int size>
//struct CosTable {
//	T values[size];
//
//	constexpr CosTable() : values() {
//		for (int i = 0; i < size + 1; i++) {
//			values[i] = ccos(i * twopi<double>() / size);
//		}
//	}
//	T cos(T x) const {
//		constexpr T factor = size * r_twopi<double>();
//		return values[std::abs((int)(x * factor) % size)];
//	}
//
//	T sin(T x) const {
//		constexpr double pi_halfs = pi<double>() / 2;
//		return cos(x - pi_halfs);
//	}
//};



constexpr int trig_lookup_size = 2000;
//constexpr CosTable<float, trig_lookup_size> cosTable;

void init_trig_lookup();

// cos lookup table. Takes optimized about 2/3 time of std::cos() but twice as long in Debug mode
double cos_lut(double x);

double sin_lut(double x);
// Really good and smooth approximation. About twice to three times as fast as std::cos when optimized but
// 3 times slower in Debug mode
template<typename T>
T cos_approx(T x) noexcept;


/*
 * Map midi pitches to the corresponding frequencies for 440Hz tuning.
 */
template<typename T, int size>
struct FrequencyTable
{
	T frequencies[size];

	constexpr FrequencyTable() : frequencies() {
		double k = 1.059463094359; // ¹²√2
		double noteA = 6.875;
		double freq = noteA * k * k * k; // c
		for (int i = 0; i < size; i++) {
			frequencies[i] = static_cast<T>(freq);
			freq *= k;
		}
	}

	T operator[](const int index) const {
		UBERTON_DEBUG_ASSERT(index >= 0 && index < size, "Invalid access to frequency table");
		return frequencies[index];
	}
};

constexpr FrequencyTable<float, 128> frequencyTable;

} // namespace Math
} // namespace Uberton