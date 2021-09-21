
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
 * T will probably be float or double. Vector uses no dynamic data storage and is thus
 * save for audio programming. 
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




// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------- Factorials -----------------------------
// ----------------------------------------------------------------


template<class T = double>
inline T factorial(int n) {
	return n <= 1 ? 1 : factorial<T>(n - 1) * n;
}


//
// fast lookup version for factorial function (0 < n < 89).
//
inline double lookupFactorial(int n) {
	static double table[90] = {
		1, 1, 2, 6, 24, 120, 720, 5040, 40320, 362880, 3628800, 39916800,
		479001600, 6227020800, 87178291200, 1307674368000,
		20922789888000, 355687428096000, 6402373705728000,
		1.21645100408832e+17, 2.43290200817664e+18, 5.109094217170944e+19,
		1.124000727777608e+21, 2.585201673888498e+22, 6.204484017332394e+23,
		1.551121004333099e+25, 4.032914611266057e+26, 1.088886945041835e+28,
		3.048883446117138e+29, 8.841761993739701e+30, 2.65252859812191e+32,
		8.222838654177922e+33, 2.631308369336935e+35, 8.683317618811886e+36,
		2.952327990396041e+38, 1.033314796638614e+40, 3.719933267899012e+41,
		1.376375309122634e+43, 5.23022617466601e+44, 2.039788208119744e+46,
		8.159152832478977e+47, 3.34525266131638e+49, 1.40500611775288e+51,
		6.041526306337383e+52, 2.658271574788449e+54, 1.196222208654802e+56,
		5.502622159812088e+57, 2.586232415111682e+59, 1.241391559253607e+61,
		6.082818640342675e+62, 3.041409320171338e+64, 1.551118753287382e+66,
		8.065817517094388e+67, 4.274883284060025e+69, 2.308436973392414e+71,
		1.269640335365828e+73, 7.109985878048635e+74, 4.052691950487722e+76,
		2.350561331282879e+78, 1.386831185456899e+80, 8.320987112741392e+81,
		5.075802138772248e+83, 3.146997326038794e+85, 1.98260831540444e+87,
		1.268869321858842e+89, 8.247650592082472e+90, 5.443449390774431e+92,
		3.647111091818868e+94, 2.480035542436831e+96, 1.711224524281413e+98,
		1.197857166996989e+100, 8.504785885678622e+101, 6.123445837688608e+103,
		4.470115461512683e+105, 3.307885441519386e+107, 2.480914081139539e+109,
		1.88549470166605e+111, 1.451830920282858e+113, 1.132428117820629e+115,
		8.946182130782973e+116, 7.156945704626378e+118, 5.797126020747366e+120,
		4.75364333701284e+122, 3.945523969720657e+124, 3.314240134565352e+126,
		2.817104114380549e+128, 2.422709538367272e+130, 2.107757298379527e+132,
		1.854826422573984e+134, 1.650795516090845e+136
	};
#ifdef _DEBUG
	if (n < 0) throw std::exception("invalid number (negative) for lookupFactorial()");
	if (n > 89) throw std::exception("invalid number (n > 89) for lookupFactorial()");
#endif // DEBUG
	return table[n];
}

// generate factorial lookup table
//inline void printFactorials() {
//	std::cout << std::setprecision(16);
//	for (int i = 1; i < 90; i++) {
//		std::cout << factorial<double>(i) << ",";
//		if (i % 3 == 0) {
//			std::cout << '\n';
//		}
//	}
//}


// Product of all integers from 1 to n with same parity (odd/even) as n
template<class T = double>
inline T doublefactorial(int n) {
	if (n == 0 || n == 1) return 1;
	return n * doublefactorial<T>(n - 2);
}




// ----------------------------------------------------------------
// ----------------------------------------------------------------
// --------------------- Hypergeometric ---------------------------
// ----------------------------------------------------------------


//
// Gaussian hypergeometric function 2F1(a, b, c, x)
// Restrictions: |x| < 1, c no negative integer and not zero
//
template<class T>
inline T hypergeometric(T a, T b, T c, T x, double tolerance = 1e-7) {
#ifdef _DEBUG
	if (x == T(1)) {
		std::cout << "x==1 in hypergeometrical\n";
		return 0;
	}
	if (c <= 0 && std::ceil(c) - c < 0.001) {
		std::cout << "c==0 or negative integer in hypergeometrical\n";
		return 0;
	}
#endif
	double term = a * b * x / c;
	double value = 1.0 + term;
	int n = 1;

	while (abs(term) > tolerance) {
		a++, b++, c++, n++;
		term *= a * b * x / c / n;
		value += term;
	}
	//std::cout << "(n=" << n << ")";

	return value;
}

// Time constant version with fixed number of iterations.
// This algorithm may produce wrong values if the convergence is
// not fast enough and also be slower for fast converging parameters.
template<class T>
inline T hypergeometricFixedIterations(T a, T b, T c, T x) {
	// http://www.cplusplus.com/forum/general/255896/
	int numIterations = 20;
	T term = a * b * x / c;
	T value = 1.0 + term;
	for (int i = 0; i < numIterations; i++) {
		a++, b++, c++;
		term *= a * b * x / c / (i + 2);
		value += term;
	}
	return value;
}



// ----------------------------------------------------------------
// ----------------------------------------------------------------
// -------------------------- Binomial ----------------------------
// ----------------------------------------------------------------


inline int binomialCoeff(int n, int k) {
	int result = 1;

	// C(n, k) = C(n, n-k)
	if (k > n - k) {
		k = n - k;
	}
	for (int i = 0; i < k; ++i) {
		result *= (n - i);
		result /= (i + 1);
	}
	return result;
}



// ----------------------------------------------------------------
// ----------------------------------------------------------------
// -------------------------- Power 4 -----------------------------
// ----------------------------------------------------------------


//
// fastest possible 4 to the power of n (for 0 <= n < 32) using long long
//
inline long long power4LL(int n) {
	if (n == 0) return 1;
	return (4ll << 2 * (n - 1));
}

//
// fast 4 to the power of n (fastest for 0 <= n < 32 and pretty fast for n >= 32)
//
inline double power4(int n) {
	if (n < 32) return power4LL(n);
	double result = 1;
	int i = 0;
	while (i < n) {
		result *= 4;
		i++;
	}
	return result;
}



// ----------------------------------------------------------------
// ----------------------------------------------------------------
// --------------------------- Gamma ------------------------------
// ----------------------------------------------------------------


//
// fast version of gamma function for values (n + 0.5) with 0 ≤ n ≤ 45
// (upper limit because of lookupFactorial) using the identity
// Γ(n + 0.5) = √π · (2n)! / (n! · 4^n)
//
template<class T>
inline T plusOneHalfGamma(int n) {
	constexpr T sqrtPi{ 1.772453851 };
	return sqrtPi * lookupFactorial(2 * n) / (lookupFactorial(n) * power4(n));
}

//template<class T>
//T plusOneHalfGamma1(int n) {
//	constexpr T sqrtPi{ 1.772453850905516 };
//	return factorial(2 * n) / (factorial(n) * std::pow(4, n)) * sqrtPi;
//}
//
//
//
//template<class T>
//T plusOneHalfGamma2(int n) {
//	constexpr T sqrtPi{ 1.772453851 };
//	return lookupFactorial(2 * n) / (lookupFactorial(n) * std::pow(4, n)) * sqrtPi;
//}
//
//template<class T>
//T plusOneHalfGamma3(int n) {
//	constexpr T sqrtPi{ 1.772453851 };
//	return lookupFactorial(2 * n) / (lookupFactorial(n) * powerLL(n)) * sqrtPi;
//}

// more than 100 times faster than std::tgamma() for this special case
inline double lookupPlusOneHalfGamma(int n) {
	static const double table[] = {
		1.772453850905516, 0.8862269254527581,
		1.329340388179137, 3.323350970447843,
		11.63172839656745, 52.34277778455352,
		287.8852778150443, 1871.254305797788,
		14034.40729348341, 119292.461994609,
		1133278.388948786, 11899423.08396225,
		136843365.4655659, 1710542068.319574,
		23092317922.31425, 334838609873.5565,
		5189998453040.126, 85634974475162.06,
		1498612053315336, 2.772432298633371e+16,
		5.406242982335076e+17, 1.108279811378691e+19,
		2.382801594464185e+20, 5.361303587544416e+21,
		1.259906343072937e+23, 3.086770540528698e+24,
		7.871264878348179e+25, 2.085885192762267e+27,
		5.736184280096235e+28, 1.634812519827426e+30,
		4.82269693349091e+31, 1.470922564714727e+33,
		4.63340607885139e+34, 1.505856975626701e+36,
		5.04462086834945e+37, 1.740394199580561e+39,
		6.178399408510991e+40, 2.255115784106512e+42,
		8.456684190399418e+43, 3.255823413303776e+45,
		1.286050248254992e+47, 5.208503505432715e+48,
		2.161528954754577e+50, 9.186498057706955e+51,
		3.996126655102523e+53, 1.778276361520623e+55,
		8.091157444918838e+56, 3.762388211887259e+58,
		1.787134400646448e+60, 8.667601843135271e+61,
		4.290462912351957e+63, 2.166683770737739e+65,
		1.115842141929936e+67, 5.858171245132165e+68,
		3.134121616145708e+70, 1.70809628079941e+72,
		9.479934358436727e+73, 5.356162912516751e+75,
		3.079793674697132e+77, 1.801679299697822e+79,
		1.071999183320204e+81, 6.485595059087234e+82,
		3.988640961338649e+84, 2.492900600836656e+86,
		1.582991881531277e+88, 1.021029763587674e+90,
		6.687744951499261e+91, 4.447350392747009e+93,
		3.001961515104231e+95, 2.056343637846398e+97,
		1.429158828303246e+99, 1.007556973953789e+101,
		7.204032363769591e+102, 5.222923463732953e+104,
		3.838848745843721e+106, 2.859942315653573e+108,
		2.159256448318447e+110, 1.651831182963612e+112,
		1.280169166796799e+114, 1.004932795935487e+116,
		7.989215727687122e+117, 6.431318660788134e+119,
		5.24152470854233e+121, 4.324257884547423e+123,
		3.610755333597097e+125, 3.051088256889547e+127,
		2.608680459640563e+129, 2.256508597589087e+131,
		1.974445022890451e+133, 1.747383845258049e+135
	};
#ifdef _DEBUG
	if (n < 0) throw std::exception("invalid number (negative) for lookupPlusOneHalfGamma()");
	if (n > 89) throw std::exception("invalid number (n > 89) for lookupPlusOneHalfGamma()");
#endif // DEBUG
	return table[n];
}



// ----------------------------------------------------------------
// ----------------------------------------------------------------
// -------------------- Associated Legendre -----------------------
// ----------------------------------------------------------------


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

// Associated Legendre polynom (including Condon-Shortley phase (-1)^m)
template<class T>
inline auto assoc_legendre(int l, int m, T x) noexcept {
	auto d = static_cast<T>(pow(T(1) - x * x, T(abs(m)) * T(0.5)));
	return assoc_legendre_impl(l, m, x, d);
}


// Generalized legendre function for real parameters.
// Absolute error is at most 2e-9. Relative error at most 1e-6 but only for high l,m and very small results (<1e-8)
// Accuracy could be improved by increasing tolerance in hypergeometric
template<class T>
inline T generalized_assoc_legendre(T lambda, T mu, T x) {
	// https://en.wikipedia.org/wiki/Associated_Legendre_polynomials#Generalization_via_hypergeometric_functions
	return T(1) / std::tgamma(T(1) - mu) * std::pow((T(1) + x) / (T(1) - x), mu * T(0.5)) * hypergeometric<T>(-lambda, lambda + T(1), T(1) - mu, (T(1) - x) * T(0.5));
}

// evaluates generalized associated legendre function for (lambda = l + 0.5) and (mu = m + 0.5)
// Restrictions: m < 2 (due to lookupPlusOneHalfGamma()) and |x| < 1
//
// The hypergeometric function should converge for all l, m (so long as |x| < 1), because
// the third parameter is never an integer.
template<class T>
inline T generalized_assoc_legendre_plus_onehalf(T l, T m, T x) {
	T lambda = l + 0.5;
	T mu = m + 0.5;
	// https://en.wikipedia.org/wiki/Associated_Legendre_polynomials#Generalization_via_hypergeometric_functions
	return std::pow((T(1) + x) / (T(1) - x), mu * T(0.5)) * hypergeometric<T>(-lambda, lambda + T(1), T(1) - mu, (T(1) - x) * T(0.5)) / lookupPlusOneHalfGamma(2 - m);
}
// lambda = n +.5
// 1 - lambda = 1 - n - .5 = 2 - n + 0.5;
//


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