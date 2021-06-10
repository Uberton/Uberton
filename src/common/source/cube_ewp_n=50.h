
// Precomputed eigenvalues and -vectors for a d-dimensional cube with maximum dimension 10
// and 50 eigenvalues per dimension.
//
//
// -----------------------------------------------------------------------------------------------------------------------------
// This file is part of the �berton project. Copyright (C) 2021 �berton
//
// �berton is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// �berton is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
// of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details. You should
// have received a copy of the GNU General Public License along with �berton. If not, see http://www.gnu.org/licenses/.
// -----------------------------------------------------------------------------------------------------------------------------

 
template<class T, int maxDim, int maxOrder>
CubeEWPStorage<T, maxDim> getCubeEWPStorage() {
	static_assert(maxOrder >= 1 && maxOrder <= 50, "template parameter maxOrder needs to be between 1 and 50");
	static_assert(maxDim >= 1 && maxDim <= 10, "template parameter maxDim needs to be between 1 and 10");

	using CoeffType = short; // short is more than enough. char is just uncomfortable to read from ascii file
	struct Row { std::vector<CoeffType> coeffs; T eigenvalue{ 0 }; };

	CubeEWPStorage<T, maxDim> ewp;
	CubeEWPCalculator<T> c;
	c.data = {
	{ { 1 }, 1 },
	{ { 2 }, 2 },
	{ { 3 }, 3 },
	{ { 4 }, 4 },
	{ { 5 }, 5 },
	{ { 6 }, 6 },
	{ { 7 }, 7 },
	{ { 8 }, 8 },
	{ { 9 }, 9 },
	{ { 10 }, 10 },
	{ { 11 }, 11 },
	{ { 12 }, 12 },
	{ { 13 }, 13 },
	{ { 14 }, 14 },
	{ { 15 }, 15 },
	{ { 16 }, 16 },
	{ { 17 }, 17 },
	{ { 18 }, 18 },
	{ { 19 }, 19 },
	{ { 20 }, 20 },
	{ { 21 }, 21 },
	{ { 22 }, 22 },
	{ { 23 }, 23 },
	{ { 24 }, 24 },
	{ { 25 }, 25 },
	{ { 26 }, 26 },
	{ { 27 }, 27 },
	{ { 28 }, 28 },
	{ { 29 }, 29 },
	{ { 30 }, 30 },
	{ { 31 }, 31 },
	{ { 32 }, 32 },
	{ { 33 }, 33 },
	{ { 34 }, 34 },
	{ { 35 }, 35 },
	{ { 36 }, 36 },
	{ { 37 }, 37 },
	{ { 38 }, 38 },
	{ { 39 }, 39 },
	{ { 40 }, 40 },
	{ { 41 }, 41 },
	{ { 42 }, 42 },
	{ { 43 }, 43 },
	{ { 44 }, 44 },
	{ { 45 }, 45 },
	{ { 46 }, 46 },
	{ { 47 }, 47 },
	{ { 48 }, 48 },
	{ { 49 }, 49 },
	{ { 50 }, 50 }

	};
	ewp.matrices.push_back(c);
	c.data = {
	{ { 1,1 }, 1.41421 },
	{ { 2,1 }, 2.23607 },
	{ { 1,2 }, 2.23607 },
	{ { 2,2 }, 2.82843 },
	{ { 3,1 }, 3.16228 },
	{ { 1,3 }, 3.16228 },
	{ { 3,2 }, 3.60555 },
	{ { 2,3 }, 3.60555 },
	{ { 4,1 }, 4.12311 },
	{ { 1,4 }, 4.12311 },
	{ { 3,3 }, 4.24264 },
	{ { 4,2 }, 4.47214 },
	{ { 2,4 }, 4.47214 },
	{ { 4,3 }, 5 },
	{ { 3,4 }, 5 },
	{ { 5,1 }, 5.09902 },
	{ { 1,5 }, 5.09902 },
	{ { 5,2 }, 5.38516 },
	{ { 2,5 }, 5.38516 },
	{ { 4,4 }, 5.65685 },
	{ { 5,3 }, 5.83095 },
	{ { 3,5 }, 5.83095 },
	{ { 6,1 }, 6.08276 },
	{ { 1,6 }, 6.08276 },
	{ { 6,2 }, 6.32456 },
	{ { 2,6 }, 6.32456 },
	{ { 5,4 }, 6.40312 },
	{ { 4,5 }, 6.40312 },
	{ { 3,6 }, 6.7082 },
	{ { 6,3 }, 6.7082 },
	{ { 7,1 }, 7.07107 },
	{ { 1,7 }, 7.07107 },
	{ { 5,5 }, 7.07107 },
	{ { 4,6 }, 7.2111 },
	{ { 6,4 }, 7.2111 },
	{ { 7,2 }, 7.28011 },
	{ { 2,7 }, 7.28011 },
	{ { 7,3 }, 7.61577 },
	{ { 3,7 }, 7.61577 },
	{ { 5,6 }, 7.81025 },
	{ { 6,5 }, 7.81025 },
	{ { 8,1 }, 8.06226 },
	{ { 1,8 }, 8.06226 },
	{ { 4,7 }, 8.06226 },
	{ { 7,4 }, 8.06226 },
	{ { 8,2 }, 8.24621 },
	{ { 2,8 }, 8.24621 },
	{ { 6,6 }, 8.48528 },
	{ { 3,8 }, 8.544 },
	{ { 8,3 }, 8.544 }

	};
	ewp.matrices.push_back(c);
	c.data = {
	{ { 1,1,1 }, 1.73205 },
	{ { 2,1,1 }, 2.44949 },
	{ { 1,2,1 }, 2.44949 },
	{ { 1,1,2 }, 2.44949 },
	{ { 2,2,1 }, 3 },
	{ { 2,1,2 }, 3 },
	{ { 1,2,2 }, 3 },
	{ { 3,1,1 }, 3.31662 },
	{ { 1,3,1 }, 3.31662 },
	{ { 1,1,3 }, 3.31662 },
	{ { 2,2,2 }, 3.4641 },
	{ { 3,1,2 }, 3.74166 },
	{ { 3,2,1 }, 3.74166 },
	{ { 1,2,3 }, 3.74166 },
	{ { 2,3,1 }, 3.74166 },
	{ { 1,3,2 }, 3.74166 },
	{ { 2,1,3 }, 3.74166 },
	{ { 3,2,2 }, 4.12311 },
	{ { 2,3,2 }, 4.12311 },
	{ { 2,2,3 }, 4.12311 },
	{ { 4,1,1 }, 4.24264 },
	{ { 1,4,1 }, 4.24264 },
	{ { 1,1,4 }, 4.24264 },
	{ { 3,3,1 }, 4.3589 },
	{ { 1,3,3 }, 4.3589 },
	{ { 3,1,3 }, 4.3589 },
	{ { 4,2,1 }, 4.58258 },
	{ { 2,4,1 }, 4.58258 },
	{ { 4,1,2 }, 4.58258 },
	{ { 1,2,4 }, 4.58258 },
	{ { 1,4,2 }, 4.58258 },
	{ { 2,1,4 }, 4.58258 },
	{ { 3,2,3 }, 4.69042 },
	{ { 2,3,3 }, 4.69042 },
	{ { 3,3,2 }, 4.69042 },
	{ { 2,2,4 }, 4.89898 },
	{ { 4,2,2 }, 4.89898 },
	{ { 2,4,2 }, 4.89898 },
	{ { 4,3,1 }, 5.09902 },
	{ { 3,4,1 }, 5.09902 },
	{ { 4,1,3 }, 5.09902 },
	{ { 1,4,3 }, 5.09902 },
	{ { 3,1,4 }, 5.09902 },
	{ { 1,3,4 }, 5.09902 },
	{ { 3,3,3 }, 5.19615 },
	{ { 4,2,3 }, 5.38516 },
	{ { 2,4,3 }, 5.38516 },
	{ { 3,4,2 }, 5.38516 },
	{ { 4,3,2 }, 5.38516 },
	{ { 3,2,4 }, 5.38516 }

	};
	ewp.matrices.push_back(c);
	c.data = {
	{ { 1,1,1,1 }, 2 },
	{ { 2,1,1,1 }, 2.64575 },
	{ { 1,2,1,1 }, 2.64575 },
	{ { 1,1,1,2 }, 2.64575 },
	{ { 1,1,2,1 }, 2.64575 },
	{ { 2,2,1,1 }, 3.16228 },
	{ { 2,1,1,2 }, 3.16228 },
	{ { 1,1,2,2 }, 3.16228 },
	{ { 2,1,2,1 }, 3.16228 },
	{ { 1,2,1,2 }, 3.16228 },
	{ { 1,2,2,1 }, 3.16228 },
	{ { 3,1,1,1 }, 3.4641 },
	{ { 1,3,1,1 }, 3.4641 },
	{ { 1,1,1,3 }, 3.4641 },
	{ { 1,1,3,1 }, 3.4641 },
	{ { 1,2,2,2 }, 3.60555 },
	{ { 2,1,2,2 }, 3.60555 },
	{ { 2,2,1,2 }, 3.60555 },
	{ { 2,2,2,1 }, 3.60555 },
	{ { 2,1,3,1 }, 3.87298 },
	{ { 3,1,1,2 }, 3.87298 },
	{ { 3,2,1,1 }, 3.87298 },
	{ { 1,3,2,1 }, 3.87298 },
	{ { 2,3,1,1 }, 3.87298 },
	{ { 1,2,3,1 }, 3.87298 },
	{ { 1,2,1,3 }, 3.87298 },
	{ { 2,1,1,3 }, 3.87298 },
	{ { 1,1,3,2 }, 3.87298 },
	{ { 1,1,2,3 }, 3.87298 },
	{ { 1,3,1,2 }, 3.87298 },
	{ { 3,1,2,1 }, 3.87298 },
	{ { 2,2,2,2 }, 4 },
	{ { 1,2,3,2 }, 4.24264 },
	{ { 2,2,1,3 }, 4.24264 },
	{ { 3,2,1,2 }, 4.24264 },
	{ { 2,3,1,2 }, 4.24264 },
	{ { 3,1,2,2 }, 4.24264 },
	{ { 2,2,3,1 }, 4.24264 },
	{ { 1,3,2,2 }, 4.24264 },
	{ { 3,2,2,1 }, 4.24264 },
	{ { 2,1,3,2 }, 4.24264 },
	{ { 2,1,2,3 }, 4.24264 },
	{ { 2,3,2,1 }, 4.24264 },
	{ { 1,2,2,3 }, 4.24264 },
	{ { 4,1,1,1 }, 4.3589 },
	{ { 1,4,1,1 }, 4.3589 },
	{ { 1,1,4,1 }, 4.3589 },
	{ { 1,1,1,4 }, 4.3589 },
	{ { 3,3,1,1 }, 4.47214 },
	{ { 3,1,1,3 }, 4.47214 }

	};
	ewp.matrices.push_back(c);
	c.data = {
	{ { 1,1,1,1,1 }, 2.23607 },
	{ { 2,1,1,1,1 }, 2.82843 },
	{ { 1,2,1,1,1 }, 2.82843 },
	{ { 1,1,2,1,1 }, 2.82843 },
	{ { 1,1,1,2,1 }, 2.82843 },
	{ { 1,1,1,1,2 }, 2.82843 },
	{ { 2,2,1,1,1 }, 3.31662 },
	{ { 1,2,1,1,2 }, 3.31662 },
	{ { 2,1,1,1,2 }, 3.31662 },
	{ { 2,1,2,1,1 }, 3.31662 },
	{ { 1,1,2,1,2 }, 3.31662 },
	{ { 1,2,2,1,1 }, 3.31662 },
	{ { 1,1,1,2,2 }, 3.31662 },
	{ { 1,1,2,2,1 }, 3.31662 },
	{ { 2,1,1,2,1 }, 3.31662 },
	{ { 1,2,1,2,1 }, 3.31662 },
	{ { 3,1,1,1,1 }, 3.60555 },
	{ { 1,1,1,3,1 }, 3.60555 },
	{ { 1,3,1,1,1 }, 3.60555 },
	{ { 1,1,1,1,3 }, 3.60555 },
	{ { 1,1,3,1,1 }, 3.60555 },
	{ { 1,2,2,1,2 }, 3.74166 },
	{ { 2,1,2,2,1 }, 3.74166 },
	{ { 1,2,2,2,1 }, 3.74166 },
	{ { 1,1,2,2,2 }, 3.74166 },
	{ { 2,2,2,1,1 }, 3.74166 },
	{ { 2,1,2,1,2 }, 3.74166 },
	{ { 2,1,1,2,2 }, 3.74166 },
	{ { 1,2,1,2,2 }, 3.74166 },
	{ { 2,2,1,2,1 }, 3.74166 },
	{ { 2,2,1,1,2 }, 3.74166 },
	{ { 3,2,1,1,1 }, 4 },
	{ { 2,3,1,1,1 }, 4 },
	{ { 1,1,2,1,3 }, 4 },
	{ { 3,1,2,1,1 }, 4 },
	{ { 2,1,1,3,1 }, 4 },
	{ { 1,3,2,1,1 }, 4 },
	{ { 3,1,1,2,1 }, 4 },
	{ { 1,3,1,1,2 }, 4 },
	{ { 2,1,3,1,1 }, 4 },
	{ { 1,1,3,1,2 }, 4 },
	{ { 1,2,3,1,1 }, 4 },
	{ { 1,2,1,3,1 }, 4 },
	{ { 1,2,1,1,3 }, 4 },
	{ { 2,1,1,1,3 }, 4 },
	{ { 1,1,2,3,1 }, 4 },
	{ { 1,1,3,2,1 }, 4 },
	{ { 1,1,1,3,2 }, 4 },
	{ { 1,1,1,2,3 }, 4 },
	{ { 3,1,1,1,2 }, 4 }

	};
	ewp.matrices.push_back(c);
	c.data = {
	{ { 1,1,1,1,1,1 }, 2.44949 },
	{ { 2,1,1,1,1,1 }, 3 },
	{ { 1,2,1,1,1,1 }, 3 },
	{ { 1,1,1,1,1,2 }, 3 },
	{ { 1,1,2,1,1,1 }, 3 },
	{ { 1,1,1,2,1,1 }, 3 },
	{ { 1,1,1,1,2,1 }, 3 },
	{ { 2,1,2,1,1,1 }, 3.4641 },
	{ { 2,2,1,1,1,1 }, 3.4641 },
	{ { 1,1,2,1,1,2 }, 3.4641 },
	{ { 2,1,1,1,1,2 }, 3.4641 },
	{ { 1,2,1,2,1,1 }, 3.4641 },
	{ { 1,1,2,1,2,1 }, 3.4641 },
	{ { 1,2,2,1,1,1 }, 3.4641 },
	{ { 1,2,1,1,1,2 }, 3.4641 },
	{ { 1,1,1,2,2,1 }, 3.4641 },
	{ { 2,1,1,2,1,1 }, 3.4641 },
	{ { 2,1,1,1,2,1 }, 3.4641 },
	{ { 1,1,1,2,1,2 }, 3.4641 },
	{ { 1,1,1,1,2,2 }, 3.4641 },
	{ { 1,1,2,2,1,1 }, 3.4641 },
	{ { 1,2,1,1,2,1 }, 3.4641 },
	{ { 3,1,1,1,1,1 }, 3.74166 },
	{ { 1,3,1,1,1,1 }, 3.74166 },
	{ { 1,1,1,1,3,1 }, 3.74166 },
	{ { 1,1,1,3,1,1 }, 3.74166 },
	{ { 1,1,3,1,1,1 }, 3.74166 },
	{ { 1,1,1,1,1,3 }, 3.74166 },
	{ { 1,2,2,1,2,1 }, 3.87298 },
	{ { 2,1,1,2,2,1 }, 3.87298 },
	{ { 1,2,2,1,1,2 }, 3.87298 },
	{ { 2,2,1,2,1,1 }, 3.87298 },
	{ { 2,1,1,1,2,2 }, 3.87298 },
	{ { 1,1,2,1,2,2 }, 3.87298 },
	{ { 1,2,1,1,2,2 }, 3.87298 },
	{ { 2,2,1,1,1,2 }, 3.87298 },
	{ { 1,1,2,2,1,2 }, 3.87298 },
	{ { 2,2,2,1,1,1 }, 3.87298 },
	{ { 2,1,1,2,1,2 }, 3.87298 },
	{ { 2,1,2,1,2,1 }, 3.87298 },
	{ { 1,1,1,2,2,2 }, 3.87298 },
	{ { 1,2,1,2,2,1 }, 3.87298 },
	{ { 2,2,1,1,2,1 }, 3.87298 },
	{ { 2,1,2,1,1,2 }, 3.87298 },
	{ { 2,1,2,2,1,1 }, 3.87298 },
	{ { 1,2,1,2,1,2 }, 3.87298 },
	{ { 1,1,2,2,2,1 }, 3.87298 },
	{ { 1,2,2,2,1,1 }, 3.87298 },
	{ { 1,1,2,1,1,3 }, 4.12311 },
	{ { 3,2,1,1,1,1 }, 4.12311 }

	};
	ewp.matrices.push_back(c);
	c.data = {
	{ { 1,1,1,1,1,1,1 }, 2.64575 },
	{ { 2,1,1,1,1,1,1 }, 3.16228 },
	{ { 1,1,1,1,1,1,2 }, 3.16228 },
	{ { 1,2,1,1,1,1,1 }, 3.16228 },
	{ { 1,1,2,1,1,1,1 }, 3.16228 },
	{ { 1,1,1,1,2,1,1 }, 3.16228 },
	{ { 1,1,1,2,1,1,1 }, 3.16228 },
	{ { 1,1,1,1,1,2,1 }, 3.16228 },
	{ { 1,1,1,2,2,1,1 }, 3.60555 },
	{ { 2,2,1,1,1,1,1 }, 3.60555 },
	{ { 1,2,1,1,1,2,1 }, 3.60555 },
	{ { 1,1,2,1,2,1,1 }, 3.60555 },
	{ { 2,1,2,1,1,1,1 }, 3.60555 },
	{ { 1,2,1,1,2,1,1 }, 3.60555 },
	{ { 1,2,2,1,1,1,1 }, 3.60555 },
	{ { 2,1,1,1,2,1,1 }, 3.60555 },
	{ { 1,1,2,1,1,1,2 }, 3.60555 },
	{ { 1,1,1,2,1,2,1 }, 3.60555 },
	{ { 1,1,2,2,1,1,1 }, 3.60555 },
	{ { 1,2,1,1,1,1,2 }, 3.60555 },
	{ { 2,1,1,1,1,1,2 }, 3.60555 },
	{ { 1,1,1,2,1,1,2 }, 3.60555 },
	{ { 1,1,1,1,1,2,2 }, 3.60555 },
	{ { 2,1,1,1,1,2,1 }, 3.60555 },
	{ { 1,1,2,1,1,2,1 }, 3.60555 },
	{ { 2,1,1,2,1,1,1 }, 3.60555 },
	{ { 1,1,1,1,2,1,2 }, 3.60555 },
	{ { 1,2,1,2,1,1,1 }, 3.60555 },
	{ { 1,1,1,1,2,2,1 }, 3.60555 },
	{ { 3,1,1,1,1,1,1 }, 3.87298 },
	{ { 1,1,3,1,1,1,1 }, 3.87298 },
	{ { 1,1,1,1,3,1,1 }, 3.87298 },
	{ { 1,1,1,1,1,1,3 }, 3.87298 },
	{ { 1,1,1,1,1,3,1 }, 3.87298 },
	{ { 1,3,1,1,1,1,1 }, 3.87298 },
	{ { 1,1,1,3,1,1,1 }, 3.87298 },
	{ { 2,2,1,1,1,1,2 }, 4 },
	{ { 1,2,1,1,2,2,1 }, 4 },
	{ { 1,2,1,2,1,1,2 }, 4 },
	{ { 1,1,2,2,1,2,1 }, 4 },
	{ { 2,2,2,1,1,1,1 }, 4 },
	{ { 2,1,1,1,1,2,2 }, 4 },
	{ { 2,1,1,1,2,2,1 }, 4 },
	{ { 2,1,2,1,1,1,2 }, 4 },
	{ { 1,2,1,2,1,2,1 }, 4 },
	{ { 2,1,1,2,1,1,2 }, 4 },
	{ { 2,2,1,1,2,1,1 }, 4 },
	{ { 1,2,2,1,1,1,2 }, 4 },
	{ { 2,1,2,1,2,1,1 }, 4 },
	{ { 1,1,2,1,1,2,2 }, 4 }

	};
	ewp.matrices.push_back(c);
	c.data = {
	{ { 1,1,1,1,1,1,1,1 }, 2.82843 },
	{ { 2,1,1,1,1,1,1,1 }, 3.31662 },
	{ { 1,1,1,2,1,1,1,1 }, 3.31662 },
	{ { 1,1,2,1,1,1,1,1 }, 3.31662 },
	{ { 1,2,1,1,1,1,1,1 }, 3.31662 },
	{ { 1,1,1,1,1,2,1,1 }, 3.31662 },
	{ { 1,1,1,1,1,1,1,2 }, 3.31662 },
	{ { 1,1,1,1,1,1,2,1 }, 3.31662 },
	{ { 1,1,1,1,2,1,1,1 }, 3.31662 },
	{ { 1,1,1,2,2,1,1,1 }, 3.74166 },
	{ { 1,2,1,1,1,2,1,1 }, 3.74166 },
	{ { 2,2,1,1,1,1,1,1 }, 3.74166 },
	{ { 2,1,1,1,1,1,1,2 }, 3.74166 },
	{ { 1,1,1,2,1,1,1,2 }, 3.74166 },
	{ { 1,1,2,1,1,1,1,2 }, 3.74166 },
	{ { 1,1,2,2,1,1,1,1 }, 3.74166 },
	{ { 1,1,1,1,2,2,1,1 }, 3.74166 },
	{ { 1,1,2,1,1,2,1,1 }, 3.74166 },
	{ { 1,1,2,1,2,1,1,1 }, 3.74166 },
	{ { 2,1,1,1,2,1,1,1 }, 3.74166 },
	{ { 1,1,1,2,1,1,2,1 }, 3.74166 },
	{ { 1,2,1,1,2,1,1,1 }, 3.74166 },
	{ { 2,1,2,1,1,1,1,1 }, 3.74166 },
	{ { 1,1,1,1,2,1,2,1 }, 3.74166 },
	{ { 1,1,2,1,1,1,2,1 }, 3.74166 },
	{ { 1,2,2,1,1,1,1,1 }, 3.74166 },
	{ { 1,2,1,1,1,1,1,2 }, 3.74166 },
	{ { 1,1,1,1,1,2,1,2 }, 3.74166 },
	{ { 2,1,1,1,1,1,2,1 }, 3.74166 },
	{ { 1,1,1,1,1,2,2,1 }, 3.74166 },
	{ { 1,1,1,2,1,2,1,1 }, 3.74166 },
	{ { 1,2,1,1,1,1,2,1 }, 3.74166 },
	{ { 2,1,1,2,1,1,1,1 }, 3.74166 },
	{ { 2,1,1,1,1,2,1,1 }, 3.74166 },
	{ { 1,1,1,1,1,1,2,2 }, 3.74166 },
	{ { 1,2,1,2,1,1,1,1 }, 3.74166 },
	{ { 1,1,1,1,2,1,1,2 }, 3.74166 },
	{ { 3,1,1,1,1,1,1,1 }, 4 },
	{ { 1,1,1,3,1,1,1,1 }, 4 },
	{ { 1,1,3,1,1,1,1,1 }, 4 },
	{ { 1,1,1,1,3,1,1,1 }, 4 },
	{ { 1,3,1,1,1,1,1,1 }, 4 },
	{ { 1,1,1,1,1,3,1,1 }, 4 },
	{ { 1,1,1,1,1,1,3,1 }, 4 },
	{ { 1,1,1,1,1,1,1,3 }, 4 },
	{ { 2,2,1,1,1,1,1,2 }, 4.12311 },
	{ { 1,2,2,1,1,1,2,1 }, 4.12311 },
	{ { 1,1,1,1,2,2,1,2 }, 4.12311 },
	{ { 2,1,2,1,2,1,1,1 }, 4.12311 },
	{ { 1,2,1,2,1,1,2,1 }, 4.12311 }

	};
	ewp.matrices.push_back(c);
	c.data = {
	{ { 1,1,1,1,1,1,1,1,1 }, 3 },
	{ { 2,1,1,1,1,1,1,1,1 }, 3.4641 },
	{ { 1,1,1,2,1,1,1,1,1 }, 3.4641 },
	{ { 1,2,1,1,1,1,1,1,1 }, 3.4641 },
	{ { 1,1,1,1,1,1,1,2,1 }, 3.4641 },
	{ { 1,1,1,1,1,2,1,1,1 }, 3.4641 },
	{ { 1,1,1,1,2,1,1,1,1 }, 3.4641 },
	{ { 1,1,1,1,1,1,1,1,2 }, 3.4641 },
	{ { 1,1,1,1,1,1,2,1,1 }, 3.4641 },
	{ { 1,1,2,1,1,1,1,1,1 }, 3.4641 },
	{ { 1,1,1,1,2,1,2,1,1 }, 3.87298 },
	{ { 2,2,1,1,1,1,1,1,1 }, 3.87298 },
	{ { 1,1,1,1,2,1,1,2,1 }, 3.87298 },
	{ { 1,2,2,1,1,1,1,1,1 }, 3.87298 },
	{ { 1,1,2,1,1,2,1,1,1 }, 3.87298 },
	{ { 1,2,1,1,1,1,1,1,2 }, 3.87298 },
	{ { 2,1,2,1,1,1,1,1,1 }, 3.87298 },
	{ { 1,1,1,1,2,2,1,1,1 }, 3.87298 },
	{ { 1,1,1,1,1,2,2,1,1 }, 3.87298 },
	{ { 2,1,1,1,1,1,1,1,2 }, 3.87298 },
	{ { 1,1,2,1,2,1,1,1,1 }, 3.87298 },
	{ { 1,1,1,1,1,1,2,2,1 }, 3.87298 },
	{ { 2,1,1,1,1,1,1,2,1 }, 3.87298 },
	{ { 1,1,1,2,1,1,1,1,2 }, 3.87298 },
	{ { 1,2,1,1,1,1,1,2,1 }, 3.87298 },
	{ { 1,1,1,1,1,1,2,1,2 }, 3.87298 },
	{ { 2,1,1,1,1,1,2,1,1 }, 3.87298 },
	{ { 1,1,1,2,1,2,1,1,1 }, 3.87298 },
	{ { 2,1,1,2,1,1,1,1,1 }, 3.87298 },
	{ { 1,2,1,1,1,1,2,1,1 }, 3.87298 },
	{ { 1,2,1,2,1,1,1,1,1 }, 3.87298 },
	{ { 2,1,1,1,1,2,1,1,1 }, 3.87298 },
	{ { 1,1,1,1,1,1,1,2,2 }, 3.87298 },
	{ { 1,1,1,1,1,2,1,1,2 }, 3.87298 },
	{ { 1,2,1,1,1,2,1,1,1 }, 3.87298 },
	{ { 1,1,2,2,1,1,1,1,1 }, 3.87298 },
	{ { 1,1,1,2,2,1,1,1,1 }, 3.87298 },
	{ { 1,1,2,1,1,1,2,1,1 }, 3.87298 },
	{ { 1,1,1,2,1,1,1,2,1 }, 3.87298 },
	{ { 1,1,1,1,2,1,1,1,2 }, 3.87298 },
	{ { 1,1,1,2,1,1,2,1,1 }, 3.87298 },
	{ { 1,1,2,1,1,1,1,1,2 }, 3.87298 },
	{ { 2,1,1,1,2,1,1,1,1 }, 3.87298 },
	{ { 1,2,1,1,2,1,1,1,1 }, 3.87298 },
	{ { 1,1,2,1,1,1,1,2,1 }, 3.87298 },
	{ { 1,1,1,1,1,2,1,2,1 }, 3.87298 },
	{ { 3,1,1,1,1,1,1,1,1 }, 4.12311 },
	{ { 1,1,1,1,1,1,1,1,3 }, 4.12311 },
	{ { 1,3,1,1,1,1,1,1,1 }, 4.12311 },
	{ { 1,1,3,1,1,1,1,1,1 }, 4.12311 }

	};
	ewp.matrices.push_back(c);
	c.data = {
	{ { 1,1,1,1,1,1,1,1,1,1 }, 3.16228 },
	{ { 2,1,1,1,1,1,1,1,1,1 }, 3.60555 },
	{ { 1,1,1,1,1,1,1,1,2,1 }, 3.60555 },
	{ { 1,1,1,2,1,1,1,1,1,1 }, 3.60555 },
	{ { 1,2,1,1,1,1,1,1,1,1 }, 3.60555 },
	{ { 1,1,1,1,1,1,1,1,1,2 }, 3.60555 },
	{ { 1,1,1,1,1,1,1,2,1,1 }, 3.60555 },
	{ { 1,1,1,1,2,1,1,1,1,1 }, 3.60555 },
	{ { 1,1,2,1,1,1,1,1,1,1 }, 3.60555 },
	{ { 1,1,1,1,1,1,2,1,1,1 }, 3.60555 },
	{ { 1,1,1,1,1,2,1,1,1,1 }, 3.60555 },
	{ { 1,1,1,1,1,2,1,1,2,1 }, 4 },
	{ { 2,2,1,1,1,1,1,1,1,1 }, 4 },
	{ { 1,1,1,2,1,1,1,1,2,1 }, 4 },
	{ { 2,1,1,2,1,1,1,1,1,1 }, 4 },
	{ { 2,1,1,1,1,1,1,1,1,2 }, 4 },
	{ { 1,2,1,1,2,1,1,1,1,1 }, 4 },
	{ { 1,2,1,1,1,2,1,1,1,1 }, 4 },
	{ { 1,1,1,1,2,2,1,1,1,1 }, 4 },
	{ { 1,2,1,2,1,1,1,1,1,1 }, 4 },
	{ { 1,2,1,1,1,1,2,1,1,1 }, 4 },
	{ { 2,1,2,1,1,1,1,1,1,1 }, 4 },
	{ { 2,1,1,1,1,1,1,2,1,1 }, 4 },
	{ { 1,2,2,1,1,1,1,1,1,1 }, 4 },
	{ { 1,1,2,2,1,1,1,1,1,1 }, 4 },
	{ { 1,1,1,2,1,1,1,1,1,2 }, 4 },
	{ { 1,2,1,1,1,1,1,1,1,2 }, 4 },
	{ { 1,2,1,1,1,1,1,2,1,1 }, 4 },
	{ { 1,1,1,1,1,1,1,2,1,2 }, 4 },
	{ { 1,1,1,1,1,2,1,2,1,1 }, 4 },
	{ { 1,1,1,1,1,1,2,2,1,1 }, 4 },
	{ { 2,1,1,1,2,1,1,1,1,1 }, 4 },
	{ { 1,1,2,1,1,2,1,1,1,1 }, 4 },
	{ { 1,1,1,1,2,1,2,1,1,1 }, 4 },
	{ { 1,1,1,1,1,2,1,1,1,2 }, 4 },
	{ { 1,1,2,1,1,1,1,1,1,2 }, 4 },
	{ { 2,1,1,1,1,1,2,1,1,1 }, 4 },
	{ { 2,1,1,1,1,2,1,1,1,1 }, 4 },
	{ { 1,1,2,1,1,1,1,2,1,1 }, 4 },
	{ { 1,1,1,1,1,1,2,1,1,2 }, 4 },
	{ { 1,1,1,1,1,1,1,2,2,1 }, 4 },
	{ { 1,1,2,1,1,1,2,1,1,1 }, 4 },
	{ { 1,1,2,1,2,1,1,1,1,1 }, 4 },
	{ { 2,1,1,1,1,1,1,1,2,1 }, 4 },
	{ { 1,1,1,1,1,1,2,1,2,1 }, 4 },
	{ { 1,2,1,1,1,1,1,1,2,1 }, 4 },
	{ { 1,1,1,1,1,2,2,1,1,1 }, 4 },
	{ { 1,1,1,1,2,1,1,1,1,2 }, 4 },
	{ { 1,1,1,1,2,1,1,2,1,1 }, 4 },
	{ { 1,1,2,1,1,1,1,1,2,1 }, 4 }

	};
	ewp.matrices.push_back(c);
	return ewp;
}