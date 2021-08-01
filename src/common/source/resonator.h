
// Selection of several resonators as solutions eigenvalue problems
// - String
// - 3D Sphere
// - ND Cube
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
#include <vector>

namespace Uberton {
namespace Math {

// ----      ----------------------------------------------------
// ---- Base ----------------------------------------------------
// ----      ----------------------------------------------------
// This class implements a base resonator for a discrete eigenvalue problem. The
// sample rate needs to be set before using an instance of this class. The system
// can be excited through delta peaks with variabel height and needs to be evolved
// each sample by calling next(). A number of input and output positions can be set
// for exciting / listening back on the system.
//
// Template Parameters:
//   Parent:	CRTP-style parent class that implements the specific eigenvalue problem
//	T:		float type (float/double)
//	d:		dimension (1, 2, ...)
//	N:		(maximum) number of eigenvalues taken into acount
//	channels: number of input/output "channels" or positions
//
// The parent class needs to implement the functions
//   - scalar eigenValueSqrt(int i);
//   - scalar eigenFunction(int i, const SpaceVec& x); and
//   - void setDesiredBaseFrequency(real freq, real dampening, real velocity);
//
// All positions should be normalized to [0, 1]. The same applies to the eigenFunction()
// function that the parent class needs to implement. If it features properties like a
// size or length that is internally adjusted to match the desired base frequency, this
// should not affect the output of eigenFunction() for constant x.
//
// The differential equation that is implemented in this model has the form
//         ⎛1  d²    2b d     ⎞
//     0 = ⎜−− −−  + −− −− − Δ⎟ψ(x,t)
//         ⎝c² dt²   c² dt    ⎠
// with sonic velocity c, dampening coefficient b in s⁻¹ and Δ=∇².
// The used solutions are linear combinations of
//
//     ψ(x,t) = φ(x)·exp[-bt]·exp[i√(k²c²-b²)]
//
// with spatial eigenfunctions φ(x) and corresponding eigenvalues k².
//

template<class Parent, class T, int d, int N, int channels>
class ResonatorBase : public Parent
{
	static_assert(d > 0, "template parameter d needs to be greater than 0");
	static_assert(N > 0, "template parameter N needs to be greater than 0");
	static_assert(channels > 0, "template parameter channels needs to be greater than 0");

public:
	using real = T;
	using scalar = std::complex<real>;
	using SpaceVec = Uberton::Math::Vector<T, d>;

	template<class T, int n>
	using array = std::array<T, n>;

	/// Initialize resonator with sample rate in Hz (i.e. 44100)
	void setSampleRate(T sampleRate) {
		this->deltaT = T{ 1. } / sampleRate;
		update();
	}

	/// The actual order to which the system response will be computed as well as excited
	/// can be set lower than N (the max order)
	void setOrder(int order) {
		this->order = std::max(1, std::min(N, order));
	}


	/// Excite the system at current input positions with a peak of given amounts
	void delta(const array<real, channels>& amount) {
		for (int ch = 0; ch < channels; ++ch) {
			for (int i = 0; i < order; ++i) {
				amplitudes[i] += amount[ch] * inputPosEF[ch][i];
			}
		}
	}

	/// Compute next time step and get the evaluations at the output positions
	array<real, channels> next() {
		evolve();
		array<real, channels> results{ 0 };
		for (int ch = 0; ch < channels; ++ch) {
			for (int i = 0; i < order; ++i) {
				results[ch] += (amplitudes[i] * outputPosEF[ch][i]).real();
			}
		}
		return results;
	}

	/// Set the "listening" positions (normalized to [0,1])
	void setOutputPositions(const array<SpaceVec, channels>& outPositions) {
		for (int ch = 0; ch < channels; ++ch) {
			for (int i = 0; i < N; ++i) {
				outputPosEF[ch][i] = this->eigenFunction(i, outPositions[ch]);
			}
		}
	}

	/// Set the "playing" or exciting position (normalized to [0,1])
	void setInputPositions(const array<SpaceVec, channels>& inPositions) {
		for (int ch = 0; ch < channels; ++ch) {
			for (int i = 0; i < N; ++i) {
				inputPosEF[ch][i] = this->eigenFunction(i, inPositions[ch]);
			}
		}
	}

	/// Set the base frequency (redirect to adjust i.e. the system size), dampening coefficient
	/// and (sonic) velocity
	void setFreqDampeningAndVelocity(real freq, real dampening, real velocity) {
		this->b = dampening; // lowest frequency
		this->c = velocity;
		this->setDesiredBaseFrequency(freq, dampening, velocity);
		update();
	}

	T time() const { return time; }
	constexpr int dimension() const { return d; }
	constexpr int maxOrder() const { return N; }
	constexpr int numChannels() const { return channels; }

protected:
	void update() {
		constexpr scalar imagUnit = scalar(0, 1);
		for (int i = 0; i < N; i++) {
			timeFunctions[i] = std::exp(imagUnit * this->frequency(i) * deltaT);
		}
	}

	void evolve() {
		absoluteTime += deltaT;
		for (int i = 0; i < order; i++) {
			constexpr scalar imagUnit = scalar(0, 1);
			amplitudes[i] *= timeFunctions[i]; // precomputing these is up to 20 times faster
		}
	}

	scalar frequency(int i) {
		constexpr scalar imagUnit = scalar(0, 1);
		scalar k = this->eigenValueSqrt(i);
		return (imagUnit * b + std::sqrt(k * k * c * c - b * b)); // ib + √(k²c²-b²)
	}

private:
public:
	T absoluteTime{ 0 };			  // not really needed
	T deltaT{ 0 };					  // 1 / sample rate
	real c{ 10 };					  // (sonic) velocity c
	real b{ .1f };					  // dampening factor
	array<scalar, N> amplitudes{};	  // current weights for frequency component
	array<scalar, N> timeFunctions{}; // precomputed exponential time functions

	// eigenfunction evaluations at input/output positions
	array<array<scalar, N>, channels> outputPosEF{};
	array<array<scalar, N>, channels> inputPosEF{};

	int order{ N };
};


// ----        ----------------------------------------------------
// ---- String ----------------------------------------------------
// ----        ----------------------------------------------------

template<class T>
class StringEigenValues
{
public:
	using real = T;
	using scalar = std::complex<real>;
	using SpaceVec = Uberton::Math::Vector<T, 1>;

	scalar eigenValueSqrt(int i) const {
		return (i + 1) * pi<real>() / length;
	}

	scalar eigenFunction(int i, const SpaceVec& x) const {
		return std::sin((i + 1) * pi<real>() * x[0]); // no division by length as x is normalized
	}

	void setDesiredBaseFrequency(real f, real b, real c) {
		const real w = 2 * pi<real>() * f;
		length = pi<real>() * c / std::sqrt(w * w + b * b);
	}

private:
	real length{ 1 };
};


template<class T, int N, int channels>
class StringResonator : public ResonatorBase<StringEigenValues<T>, T, 1, N, channels>
{
};


// ----      ----------------------------------------------------
// ---- Cube ----------------------------------------------------
// ----      ----------------------------------------------------

template<class T, int d, int N>
class CubeEigenValues
{
public:
	using real = T;
	using scalar = std::complex<real>;
	using KVec = Uberton::Math::Vector<real, d + 1>;
	using SpaceVec = Uberton::Math::Vector<real, d>;

	CubeEigenValues() {
		computeFirstEigenvalues();
		for (auto& a : ksAndEV) {
			for (int i = 0; i < a.size(); i++) {
				//FDebugPrint("%f, ", a[i]);
			}
			//FDebugPrint("\n");
			//std::cout << a << "\n";
		}
	}

protected:
	scalar eigenValueSqrt(int i) const {
		return ksAndEV[i][d] * pi / length;
	}

	scalar eigenFunction(int i, const SpaceVec& x) const {
		real result{ 1 };
		constexpr real pi = Uberton::Math::pi<real>();
		for (int j = 0; j < dim; ++j) {
			result *= std::sin(ksAndEV[i][j] * pi * x[j]); // no division by length as x is normalized
		}
		return result;
	}

	void setDesiredBaseFrequency(real f, real b, real c) {
		const real w = 2 * pi * f;
		length = pi * c * std::sqrt(d / (w * w + b * b));
	}


private:
	void computeFirstEigenvalues() {
		// V_sphere = N·2ᵈ
		real radius = radiusOfNSphere(numEigenvalues * std::pow(2, dim));
		int maxRadius = static_cast<int>(std::ceil(radius)) + 1;
		int maxNumEV = std::pow(maxRadius + 1, dim);
		//std::cout << "maxNumEV " << maxNumEV << "\n";

		if (maxNumEV < numEigenvalues) {
			maxNumEV = numEigenvalues;
		}
		std::vector<KVec> kvecs(maxNumEV);

		for (int i = 0; i < maxNumEV; ++i) {
			KVec kvec; // last entry sums up the squares of the other entries
			int kindex = i;
			for (int j = 0; j < dim; j++) {
				kvec[j] = kindex % (maxRadius + 1) + 1;
				kindex /= (maxRadius + 1);
				kvec[dim] += kvec[j] * kvec[j];
			}
			kvec[dim] = static_cast<real>(std::sqrt(kvec[dim]));

			kvecs[i] = kvec;
		}

		std::sort(kvecs.begin(), kvecs.end(), [](auto& a, auto& b) { return a[d] < b[d]; });
		std::copy(kvecs.begin(), kvecs.begin() + N, ksAndEV.begin());
	}

	real radiusOfNSphere(real volume) const {
		constexpr real pi = Uberton::Math::pi<real>();
		if (dim % 2 == 0) { // V = π^(½d)·r^d/(½d)!  ⇔  r = ᵈ√[V·(½d)! / π^(½d)]
			return static_cast<real>(std::pow(volume * Uberton::Math::factorial(dim / 2) / std::pow(pi, dim / 2), real{ 1 } / dim));
		}
		else { // V = 2[½(d-1)]!·(4π)^[½(d-1)]·rᵈ/d!  ⇔  r = ᵈ√[V·d! / { 2[½(d-1)]!·(4π)^[½(d-1)] }]
			return static_cast<real>(std::pow(numEigenvalues * Uberton::Math::factorial(dim) / (2 * Uberton::Math::factorial((dim - 1) / 2) * std::pow(4 * pi, (dim - 1) / 2)), real{ 1 } / dim));
		}
	}

	std::array<KVec, N> ksAndEV;
	int numEigenvalues{ N };
	real length{ 1 };
	int dim{ d };
	static constexpr real pi = Uberton::Math::pi<real>();
};


template<class T, int d, int N, int channels>
class CubeResonator : public ResonatorBase<CubeEigenValues<T, d, N>, T, d, N, channels>
{
};


// ----                                                         ---
// ---- Cube (Precomputed and stored eigenvalues + computation) ---
// ----                                                         ---

template<class T>
std::vector<std::vector<T>> computeFirstEigenvalues(int dim, int numEigenvalues) {
	// V_sphere = N·2ᵈ
	//std::cout << dim << "\n";
	using real = T;
	using scalar = std::complex<real>;
	using KVec = std::vector<real>;

	auto radiusOfNSphere = [&](real volume) {
		constexpr real pi = Uberton::Math::pi<real>();
		if (dim % 2 == 0) { // V = π^(½d)·r^d/(½d)!  ⇔  r = ᵈ√[V·(½d)! / π^(½d)]
			return std::pow(volume * Uberton::Math::factorial(dim / 2) / std::pow(pi, dim / 2), real{ 1 } / dim);
		}
		else { // V = 2[½(d-1)]!·(4π)^[½(d-1)]·rᵈ/d!  ⇔  r = ᵈ√[V·d! / { 2[½(d-1)]!·(4π)^[½(d-1)] }]
			return std::pow(numEigenvalues * Uberton::Math::factorial(dim) / (2 * Uberton::Math::factorial((dim - 1) / 2) * std::pow(4 * pi, (dim - 1) / 2)), real{ 1 } / dim);
		}
	};

	real radius = radiusOfNSphere(numEigenvalues * std::pow(2, dim));
	int add = 0;
	if (dim == 1) {
		add = numEigenvalues;
	}
	else if (dim <= 3) {
		//add = 3;
	}
	//else if (dim <= 5) {
	//	add = 10;
	//}
	int maxRadius = static_cast<int>(std::ceil(radius)) + add;
	int maxNumEV = std::pow(maxRadius + 1, dim);

	std::vector<KVec> kvecs;

	for (int i = 0; i < maxNumEV; ++i) {
		std::vector<real> kvec(dim + 1); // last entry sums up the squares of the other entries
		int kindex = i;
		for (int j = 0; j < dim; j++) {
			kvec[j] = kindex % (maxRadius + 1) + 1;
			kindex /= (maxRadius + 1);
			kvec[dim] += kvec[j] * kvec[j];
		}
		kvec[dim] = static_cast<real>(std::sqrt(kvec[dim]));
		kvecs.push_back(kvec);
	}

	std::sort(kvecs.begin(), kvecs.end(), [dim](auto& a, auto& b) { return a[dim] < b[dim]; });
	kvecs.resize(numEigenvalues);
	return kvecs;
}
//
// Compute and (de)serialize the first n eigenvectors and -values of a d-dimensional
// cube. The file is stored in ASCII format, which saves spaces as most values are integers
//
template<class T>
struct CubeEWPCalculator
{

	CubeEWPCalculator() {}

	void compute(int d, int n) {
		if (d < 1 || n < 1) throw std::exception("dim and n need to be greater than 0");
		//data = computeFirstEigenvalues<T>(d, n);
		initialized = true;
	}

	using CoeffType = short; // short is more than enough. char is just uncomfortable to read from ascii file
	struct Row
	{
		std::vector<CoeffType> coeffs;
		T eigenvalue{ 0 };
	};
	std::vector<Row> data;


	template<class U>
	friend std::ostream& operator<<(std::ostream& os, const CubeEWPCalculator<U>& a) {
		if (!a.initialized) return os;
		int dim = a.data[0].coeffs.size();
		os << dim << " " << a.data.size() << "\n";
		for (const auto& row : a.data) {
			for (const auto& entry : row.coeffs) {
				os << entry << " ";
			}
			os << row.eigenvalue << " ";
			os << "\n";
		}
		os << "-\n";
		return os;
	}
	template<class U>
	friend std::istream& operator>>(std::istream& is, CubeEWPCalculator<U>& a) {
		a.data.clear();
		int dim;
		int size;
		is >> dim >> size;
		int rowLength = dim;
		for (int i = 0; i < size; i++) {
			Row row;
			row.coeffs = std::vector<CoeffType>(rowLength);
			for (int j = 0; j < rowLength; j++) {
				is >> row.coeffs[j];
			}
			is >> row.eigenvalue;
			a.data.push_back(row);
		}
		char endChar;
		is >> endChar;
		if (endChar != '-') {
			throw std::exception("cube ewp file is damaged");
		}
		a.initialized = true;
		return is;
	}

private:
	bool initialized{ false };
};

template<class T, int maxDim>
struct CubeEWPStorage
{
	std::vector<CubeEWPCalculator<T>> matrices;

	CubeEWPStorage() {
	}

	void load(const std::string& filename) {
		std::ifstream file(filename);
		if (!file) {
			//FDebugPrint("could not read file");
			return;
		}
		for (int i = 1; i <= maxDim; i++) {
			CubeEWPCalculator<T> ewp;
			file >> ewp;
			matrices.push_back(ewp);
		}
	}

	void compute(int n) {
		matrices.clear();
		for (int i = 1; i <= maxDim; i++) {
			CubeEWPCalculator<float> ewp;
			ewp.compute(i, n);
			matrices.push_back(ewp);
		}
	}

	void save(const std::string& filename) {
		std::ofstream file(filename);
		for (const auto& ewp : matrices) {
			file << ewp;
		}
	}
};

// c++ file generated by CubeEWPStorage for n=200 that contains all eigenvalues/-vecs
// defines function getCubeEWPStorage<T, maxDim>()
#include "cube_ewp_n=200.h"
//#include "cube_ewp_n=50.h"


template<class T, int maxDim, int N>
class PreComputedCubeEigenValues
{

public:
	using real = T;
	using scalar = std::complex<real>;
	using SpaceVec = Uberton::Math::Vector<real, maxDim>;

	PreComputedCubeEigenValues() {
		storage = getCubeEWPStorage<T, maxDim, N>();
	}

	void setDim(int newDim) {
		if (newDim < 1)
			dim = 1;
		else if (newDim > maxDim)
			dim = maxDim;
		else
			dim = newDim;
	}

	int getDim() { return dim; }

protected:
	scalar eigenValueSqrt(int i) const {
		return storage.matrices[dim - 1].data[i].eigenvalue * pi<real>() / length;
	}

	scalar eigenFunction(int i, const SpaceVec& x) const {
		real result{ 1 };
		constexpr real pi = Uberton::Math::pi<real>();
		for (int j = 0; j < dim; ++j) {
			result *= std::sin(storage.matrices[dim - 1].data[i].coeffs[j] * pi * x[j]); // no division by length as x is normalized
		}
		return result;
	}

	void setDesiredBaseFrequency(real f, real b, real c) {
		constexpr real pi = Uberton::Math::pi<real>();
		const real w = 2 * pi * f;
		length = pi * c * std::sqrt(dim / (w * w + b * b));
	}

private:
	CubeEWPStorage<T, maxDim> storage;
	real length{ 1 };
	int dim{ maxDim };
};

template<class T, int maxDim, int N, int channels>
class PreComputedCubeResonator : public ResonatorBase<PreComputedCubeEigenValues<T, maxDim, N>, T, maxDim, N, channels>
{
};



// ----        ----------------------------------------------------
// ---- Sphere ----------------------------------------------------
// ----        ----------------------------------------------------

template<class T, int N>
class SphereEigenValues
{
public:
	using real = T;
	using scalar = std::complex<real>;
	using SpaceVec = Uberton::Math::Vector<real, 3>;

protected:
	scalar eigenValueSqrt(int i) const {
		int l = linearIndex(i).first;
		return std::sqrt(l * (l + 1)) * baseFreqCoeff;
	}


	scalar eigenFunction(int i, const SpaceVec& x) const {
		auto lm = linearIndex(i);
		int l = lm.first;
		int m = lm.second;

		// spherical coordinates
		real r = x[0];
		real theta = pi * x[1];
		real phi = 2 * pi * x[2];

		real legend = static_cast<T>(Uberton::Math::assoc_legendre(l, m, std::cos(theta)));

		// return only real part
		return std::pow(r, l) / Uberton::Math::r_twopi<real>() * normalizer(l, m) * legend * std::cos(m * phi);
	}

	void setDesiredBaseFrequency(real f, real b, real c) {
		baseFreqCoeff = f / std::sqrt(2);
	}

private:
	// Get m and l numbers from linear index i∈[0, n)
	std::pair<int, int> linearIndex(int i) const {
		// i+1 = l²+l+1+m and m from -l to l
		// solve for l: √(i+1) -1 <= l <= √i
		// Then, calc m from l and i
		int l = static_cast<int>(std::floor(std::sqrt(i)));
		int m = i - (l * l + l);
		return { l, m };
	}

	real normalizer(int l, int m) const {
		return (std::sqrt((2 * l + 1) / 2 * Uberton::Math::factorial(l - m) / Uberton::Math::factorial(l + m)));
	}

	real baseFreqCoeff{ 1 };
	static constexpr real pi = Uberton::Math::pi<real>();
};


template<class T, int N, int channels>
class SphereResonator : public ResonatorBase<SphereEigenValues<T, N>, T, 3, N, channels>
{
};




template<class SampleType>
class BaseAudioComponent
{
public:
	void setSampleRate(SampleType sampleRate) { timeStep = SampleType{ 1. } / sampleRate; }
	SampleType getSampleRate() { return SampleType{ 1. } / timeStep; }
	SampleType getTimeStep() { return timeStep; }

private:
	SampleType timeStep{};
};


/*
 * (Abstract) eigenvalue problem base class that implements the main procedure with eigenfunctions and -values
 * and declares methods to evaluate the eigenfunctions, -values and weights for the i-th eigenvalue.
 */
template<class T, int d, int N>
class EigenvalueProblem
{
public:
	// Evaluate for next time step at spatial position xOut.
	T next(const Vector<T, d> xOut) {
		evolve(deltaT);
		return evaluate(time, xOut);
	}

	// Evaluate for next time frame and give audio input at position xIn
	T next(const Vector<T, d> xOut, const Vector<T, d> xIn, T amplitudeIn) {
		pinchDelta(xIn, amplitudeIn);
		return next(xOut);
	}

	// Evaluate for next time step at [channels] spatial positions for Stereo or multichannel processing.
	template<int channels>
	array<T, channels> next(const array<Vector<T, d>, channels>& xOuts) {
		evolve(deltaT);
		array<T, channels> out;
		for (int i = 0; i < channels; i++)
			out[i] = evaluate(time, xOuts[i]);
		return out;
	}

	// Same but with external audio input
	template<int channels>
	array<T, channels> next(const array<Vector<T, d>, channels>& xOuts, const Vector<T, d> xIn, T amplitudeIn) {
		pinchDelta(xIn, amplitudeIn);
		return next(xOuts);
	}

	// "Pinch" at the system with delta peak.
	void pinchDelta(const Vector<T, d> x, T amount) {
		for (int i = 0; i < N; i++) {
			setAmplitude(i, amplitude(i) + eigenFunction(i, x) * amount);
		}
	}

	// "Pinch" at the system by adding to all amplitudes.
	void pinch(const array<complex<T>, N>& values) {
		for (int i = 0; i < N; i++) {
			setAmplitude(i, amplitude(i) + values[i]);
		}
	}

	// TODO: pinch with spatial function defined over interval -> needs to be decomposed
	// TODO?: pinch with temporal function -> instead pass audio signal to next()

	// Set all amplitudes to zero
	void silence() {
		for (int i = 0; i < N; i++) {
			setAmplitude(i, T{ 0 });
		}
	}
	// Call silence() and set time to 0
	void reset() {
		silence();
		resetTime();
	}
	// Call silence() and set time to 0
	void resetTime() {
		time = 0;
	}

	// Get current time
	T getTime() const { return time; }
	// Set step time interval according to sampling rate
	void setSampleRate(T sampleRate) { this->deltaT = T{ 1. } / sampleRate; }


	void setVelocity_sq(complex<T> v_sq) { velocity_sq = v_sq; }
	complex<T> getVelocity_sq() { return velocity_sq; }

protected:
	// Evolve time and amplitudes
	void evolve(T deltaTime) {
		time += deltaTime;
		for (int i = 0; i < N; i++) {
			setAmplitude(i, amplitude(i) * std::exp(complex<T>(0, 1) * /*ω=*/velocity_sq * eigenValue_sqrt(i) * deltaTime));
		}
	}

	T evaluate(T t, const Vector<T, d> x) {
		complex<T> result{ 0 };
		for (int i = 0; i < N; i++) {
			result += amplitude(i) * eigenFunction(i, x);
		}
		return result.real();
	}

	virtual T eigenFunction(int i, const Vector<T, d> x) const = 0;
	virtual T eigenValue_sqrt(int i) const = 0; // Using squareroots of eigenvalues for better performance
	virtual complex<T> amplitude(int i) const = 0;
	virtual void setAmplitude(int i, complex<T> value) = 0;

	T deltaT{ 0 }; // this needs to be set to 1/(sampling rate)

private:
	T time{ 0 }; // current Time

	complex<T> velocity_sq = 1;
};

/*
 * Optimized version for cases where the listening positions don't change every step
 *
 * Eigenfunctions are evaluated in setListeningPositions() at every listening position and stored.
 * When asking for the next sample, the cached values are used to compute the current deflection.
 */

template<class T, int d, int N, int numChannels = 1>
class FixedListenerEigenvalueProblem : public EigenvalueProblem<T, d, N>
{
public:
	void setListeningPositions(const array<Vector<T, d>, numChannels>& listeningPositions) {
		for (int i = 0; i < numChannels; ++i) {
			for (int j = 0; j < N; ++j) {
				eigenFunctionEvaluations[i][j] = this->eigenFunction(j, listeningPositions[i]);
			}
		}
	}
	void setFirstListeningPosition(const Vector<T, d>& listeningPosition) {
		for (int j = 0; j < N; ++j) {
			eigenFunctionEvaluations[0][j] = this->eigenFunction(j, listeningPosition[0]);
		}
	}

	void setStrikingPosition(const Vector<T, d> strikingPosition) {
		for (int j = 0; j < N; ++j) {
			eigenFunctionEvaluation_strike[j] = this->eigenFunction(j, strikingPosition);
		}
	}

	// "Pinch" at the system with delta peak.
	void pinchDelta(T amount) {
		for (int i = 0; i < N; i++) {
			this->setAmplitude(i, this->amplitude(i) + eigenFunctionEvaluation_strike[i] * amount);
		}
	}

	array<T, numChannels> next() {
		this->evolve(this->deltaT);
		return evaluate(this->getTime());
	}

	// Same but with external audio input
	array<T, numChannels> next(T amplitudeIn) {
		this->pinchDelta(amplitudeIn);
		return next();
	}

	T nextFirstChannel() {
		this->evolve(this->deltaT);
		return evaluateFirstChannel(this->getTime());
	}

protected:
	array<T, numChannels> evaluate(T t) {
		array<T, numChannels> results{ 0 };
		for (int i = 0; i < numChannels; ++i) {
			for (int j = 0; j < N; ++j) {
				results[i] += (this->amplitude(j) * eigenFunctionEvaluations[i][j]).real();
			}
		}
		return results;
	}
	T evaluateFirstChannel(T t) {
		T result{ 0 };
		for (int j = 0; j < N; ++j) {
			result += (this->amplitude(j) * eigenFunctionEvaluations[0][j]).real();
		}
		return result;
	}

private:
	// Eigenfunctions evaluated at the listening positions last set through setListeningPositions()
	array<array<complex<T>, N>, numChannels> eigenFunctionEvaluations;
	array<complex<T>, N> eigenFunctionEvaluation_strike;
};


/*
 * As all implementation probably keep a list of complex amplitudes, this (abstract) class implements
 * this feature for actual implementations to derive from.
 */
template<class T, int d, int N, int numChannels>
class EigenvalueProblemAmplitudeBase : public FixedListenerEigenvalueProblem<T, d, N, numChannels>
{
public:
	complex<T> amplitude(int i) const override {
		return amplitudes[i];
	}
	void setAmplitude(int i, complex<T> value) override {
		amplitudes[i] = value;
	};

private:
	array<complex<T>, N> amplitudes{}; // all default initialized with 0
};


/*
 * Implementation of the eigenvalue problem of a 1D string with fixed length. The eigenfunctions and
 * -values are similar and need not be declared separately. The weights are initialized with zero.
 */
template<class T, int N, int numChannels>
class StringEigenvalueProblem : public EigenvalueProblemAmplitudeBase<T, 1, N, numChannels>
{
public:
	StringEigenvalueProblem(T length = 1) : length(length) {}

	T eigenFunction(int i, const Vector<T, 1> x) const override {
		return std::sin((i + 1) * pi<T>() * x[0] / length); // sin(2π·x/2L)
	}
	T eigenValue_sqrt(int i) const override {
		return (i + 1) * pi<T>() / length;
	}
	T getLength() const { return length; }

	void setLength(T length) {
		this->length = length;
	}

private:
	T length; // String length
};


/*
 * Implementation of the eigenvalue problem of a sphere. The eigenfunctions and
 * -values are similar and need not be declared separately. The weights are initialized with zero.
 *
 * Warning: This is currently a 3D sphere. The Parameter d is just a dummy to make it compatible with
 * an n-dimensional cube etc.
 */
template<class T, int d, int N, int numChannels>
class SphereEigenvalueProblem : public EigenvalueProblemAmplitudeBase<T, d, N, numChannels>
{
public:
	SphereEigenvalueProblem() {}

	static Vector<T, 3> toCartesian(T theta, T phi) {
		return { std::sin(theta) * std::cos(phi), std::sin(theta) * std::sin(phi), std::cos(theta) };
	}

	// Get m and l numbers from linear index i∈[0, n)
	std::pair<int, int> linearIndex(int i) const {
		// i+1 = l²+l+1+m and m from -l to l
		// solve for l: √(i+1) -1 <= l <= √i
		// Then, calc m from l and i
		int l = static_cast<int>(std::floor(std::sqrt(i)));
		int m = i - (l * l + l);
		return { l, m };
	}

	float normalizer(int l, int m) const {
		return (std::sqrt((2 * l + 1) / 2 * factorial(l - m) / factorial(l + m)));
	}

	T eigenFunction(int i, const Vector<T, d> x) const override {
		// indices:
		auto lm = linearIndex(i);
		int l = lm.first;
		int m = lm.second;

		// spherical coordinates
		T r = x[0];
		T theta = x[1];
		T phi = x[2];

		T legend = static_cast<T>(Uberton::Math::assoc_legendre(l, m, std::cos(theta)));

		// return only real part
		return std::pow(r, l) / rsrqt2pi * normalizer(l, m) * legend * std::cos(m * phi);
	}

	const T rsrqt2pi = std::sqrt(twopi<T>());

	// k = ω/c
	// k = 2π/λ    ω=2πf=2π/T

	T eigenValue_sqrt(int i) const override {
		int l = linearIndex(i).first;
		return l * (l + 1);
		//return std::sqrt(l * (l + 1));
	}
};



template<class T, int d, int N, int numChannels>
class CubeEigenvalueProblem : public EigenvalueProblemAmplitudeBase<T, d, N, numChannels>
{
public:
	CubeEigenvalueProblem(int defaultActualDims = d) {
		actualDim = defaultActualDims;
		computeEigenvalues_and_ks();
	}

	void setDimension(int dimension) {
		if (dimension > 10 || dimension < 1) return;
		if (actualDim != dimension) {
			actualDim = dimension;
			computeEigenvalues_and_ks();
		}
	}

protected:
	/*

	 d
	___
	111 = λ₁ |
	211 = λ₂ |
	212 = λ₃ | N
			 |  ...

			 N Eigenwerte, aber N·d Quantenzahlen/Wellenzahlen

	*/
	void computeEigenvalues_and_ks() {
		int r = getRApprox();

		int maxNumEigenvalues = std::pow(r + 1, actualDim);
		//if (maxNumEigenvalues < N) throw std::exception("r<N");

		using SpaceVec = Vector<T, d + 1>;

		std::vector<SpaceVec> kvecs(maxNumEigenvalues);

		for (int i = 0; i < maxNumEigenvalues; ++i) {
			SpaceVec kvec{}; // last entry sums up the squares of the other entries
			int kindex = i;
			for (int j = 0; j < actualDim; j++) {
				kvec[j] = kindex % (r + 1) + 1;
				kindex /= (r + 1);
				kvec[d] += kvec[j] * kvec[j];
			}
			kvec[d] = static_cast<T>(std::sqrt(kvec[d]));

			kvecs[i] = kvec;
		}

		std::sort(kvecs.begin(), kvecs.end(), [](SpaceVec& a, SpaceVec& b) { return a[d] < b[d]; });
		std::copy(kvecs.begin(), kvecs.begin() + N, ks_and_eigenvalues.begin());
	}
	/*
	0000..
	1000..
	0100..
	0010..
	 .
	 .
	1100
	0110
	 .
	 .
	1110
	 .
	 .
	1111

	*/
	int getRApprox() {
		// N =!  V_sphere / 2^d
		// with
		//       | π^(d/2) r^d /(d/2)!               even d
		// V =   |
		//       | 2·(.5(d-1))!·(4π)^.5(d-1)r^d/d!   odd d
		T r = 0;
		if (!(actualDim % 2)) { // even d
			r = 2 * std::pow(N / std::pow(pi<T>(), actualDim / 2) * factorial(actualDim / 2), T{ 1 } / actualDim);
		} // odd d
		else {
			r = 2 * std::pow(N / std::pow(4 * pi<T>(), (actualDim - 1) / 2) * factorial(actualDim) / factorial((actualDim - 1) / 2) / T{ 2 }, T{ 1 } / actualDim);
		}
		// r = largest k we will get
		return static_cast<int>(std::ceil(r));
	}


protected:
	T eigenFunction(int i, const Vector<T, d> x) const override {
		T result{ 1 };
		for (int j = 0; j < actualDim; ++j) {
			result *= std::sin(ks_and_eigenvalues[i][j] * pi<T>() * x[j]);
		}
		return result;
	}

	T eigenValue_sqrt(int i) const override {
		return ks_and_eigenvalues[i][d];
	}

private:
	array<Vector<T, d + 1>, N> ks_and_eigenvalues{};
	int actualDim = d;
};

/*
 * Implementation that allows to set specific eigenfunctions and values.
 */
template<class T, int d, int N, int numChannels, class F>
class IndividualFunctionEigenvalueProblem : public EigenvalueProblemAmplitudeBase<T, d, N, numChannels>
{
public:
	IndividualFunctionEigenvalueProblem() {}

	T eigenFunction(int i, const Vector<T, 1> x) override {
		return eigenFunctions[i](x);
	}

	T eigenValue_sqrt(int i) const override {
		return eigenValues_sq[i];
	}

	array<F, N> eigenFunctions{};
	array<T, N> eigenValues_sq{};
};


} // namespace Math
} // namespace Uberton