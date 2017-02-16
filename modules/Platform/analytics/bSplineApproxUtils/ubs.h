// ==========================================================================
// Dipl.-Informatiker Matthias Borner
// 'Interaktive Engineering Technologien'
//
// Fraunhofer-Institut für Graphische Datenverarbeitung IGD
// Fraunhoferstr. 5 | 64283 Darmstadt | Germany
// Telefon +49 6151 155-663 | Fax +49 6151 155-139
// matthias.borner@igd.fraunhofer.de | http://www.igd.fraunhofer.de/iet
//
// Creation Date: 2015_06_02
// Content: mathematics for uniform cubic b splines
// ==========================================================================

// Explanation:
//   Only uniform cubic b splines are implemented.
//   Each control point can be flagged smooth (sharpFlags[i] = 0) or sharp (sharpFlags[i] = 1).
//   The base functions are hard coded and therefore evaluated fast.
//   The b spline is clamped at start and end, thus interpolating the first and last control point.
// Definitions:
//   The i-th base function has its peak at parameter i.
//   The non-zero range of the i-th base function is (i-2, i+2).
//   Each uniform interval has the length of 1.
//   For clamped base functions, special cases are implemented.

#ifndef UBS_H
#define UBS_H

#include <vector>
#include <memory>

#include <Eigen/Core>

template <size_t Dim>
class UBS
{
private:

	// ===== MEMBERS =====
	std::vector<Eigen::Matrix<double, Dim, 1>>	controlPoints;	// set of 3D control points
	std::vector<bool>				sharpFlags;		// flag for each control point, if it is considered smooth or sharp

public:

	enum EvaluationOptions
	{
		uniform = 1,
		calculateDerivative = 2,
		quadratic = 4,
		cubic = 8
	};

	// creates a Uniform B Spline with given control points. If sharpFlags_ is not the size of controlPoints_, the default sharpFlags will be set (all inner control points are smooth).
	UBS(const std::vector<Eigen::Matrix<double, Dim, 1>>& controlPoints_, const std::vector<bool>& sharpFlags_ = std::vector<bool>())
		: controlPoints(controlPoints_)
		, sharpFlags(sharpFlags_)
	{
		if (sharpFlags.size() != controlPoints.size()) sharpFlags = std::vector<bool>(controlPoints.size(), false);
		if (!sharpFlags.empty())
		{
			sharpFlags.front() = true;
			sharpFlags.back() = true;
		}
		if (!isValid())
		{
			controlPoints.clear();
			sharpFlags.clear();
		}
	}

	~UBS() {}

	// ===== GETTERS =====

	const std::vector<Eigen::Matrix<double, Dim, 1>>& getControlPoints() const { return controlPoints; }
	const size_t getNumberControlPoints() const { return controlPoints.size(); }
	const std::vector<bool>& getSharpFlags() const { return sharpFlags; }

	// ===== EVALUATION =====

	// returns true, if the data of this UBS is valid.
	bool isValid() const
	{
		if (controlPoints.size() < 2) return false;										// at least two control point
		if (controlPoints.size() != sharpFlags.size()) return false;					// sharp flag for every control point
		if (sharpFlags.front() == false || sharpFlags.back() == false) return false;	// has to be clamped at start and end
		return true;
	}

	// evaluates this UBS at one parameter t in [0, #controlPoints - 1].
	Eigen::Matrix<double, Dim, 1> evaluate(const double t) const
	{
		assert(isValid());
		assert(parameterIsInIntervalRange(t));
		// cap at start and end
		if (t <= 0.0) return controlPoints.front();
		if (t >= (double)(controlPoints.size() - 1)) return controlPoints.back();
		// find sharp knots around t => clamped intervall [s, e]
		std::pair<size_t, size_t> interval = extractSharpInterval(t);
		// basis functions [floor(t - 1), floor(t + 2)] are non zero at t. clamping has to be taken into account too.
		Eigen::Matrix<double, Dim, 1> evaluatedPoint;
		evaluatedPoint.setZero();

		size_t iStart = interval.first;
		if (t > 1.0) iStart = std::max(interval.first, static_cast<size_t>(t) - 1);
		for (size_t i = iStart; i <= std::min(interval.second, static_cast<size_t>(t) + 2); ++i)
			evaluatedPoint += N3citse(i, t, interval.first, interval.second) * controlPoints[i];
		return evaluatedPoint;
	}

	// evaluates this UBS at several parameters T in [0, #controlPoints - 1]. It is faster then single evaluations, if T is sorted so that parameters belonging to one sharp interval are grouped together.
	std::vector<Eigen::Matrix<double, Dim, 1>> evaluate(const std::vector<double>& T) const
	{
		assert(isValid());
		std::vector<Eigen::Matrix<double, Dim, 1>> evaluatedPoints;
		if (T.empty()) return evaluatedPoints;
		evaluatedPoints.reserve(T.size());
		// find sharp knots around t => clamped intervall [s, e] and iterate over T
		std::pair<size_t, size_t> interval = extractSharpInterval(T[0]);
		for (auto& t : T)
		{
			assert(parameterIsInIntervalRange(t));
			// go to next sharp interval, if T[iPara] is outside the current interval
			if (!parameterIsInIntervalRange(t, interval.first, interval.second)) interval = extractSharpInterval(t);
			// basis functions [floor(t - 1), floor(t + 2)] are non zero at t. clamping has to be taken into account too.
			Eigen::Matrix<double, Dim, 1> evaluatedPoint = Eigen::Matrix<double, Dim, 1>::Zero();

			size_t iStart = interval.first;
			if (t > 1.0) iStart = std::max(interval.first, static_cast<size_t>(t) - 1);
			for (size_t i = iStart; i <= std::min(interval.second, static_cast<size_t>(t) + 2); ++i)
				evaluatedPoint += N3citse(i, t, interval.first, interval.second) * controlPoints[i];
			evaluatedPoints.push_back(evaluatedPoint);
		}
		return evaluatedPoints;
	}

	static std::vector<Eigen::VectorXd> evaluateBasisFunctions(const std::vector<double>& T, const size_t n)
	{
		std::vector<Eigen::VectorXd> bfs(T.size(), Eigen::VectorXd::Zero(n));

		for (size_t t = 0; t < T.size(); ++t)
			for (size_t i = 0; i < n; ++i)
				bfs[t](i) = N3cite(i, T[t], 3);

		return bfs;
	}

	// ===== BASE FUNCTIONS =====
#define sechstel    1.0 / 6.0
#define drittel     1.0 / 3.0
#define zweidrittel 2.0 / 3.0

	// returns the value of i-th uniform cubic b spline basis function at parameter t (not clamped).
	static double N3it(const size_t i, const double t)
	{
		// shift parameter t [i-2, i+2] => [0,4]
		double t2 = t - static_cast<double>(i) + 2.0;
		// parameter t2 not in [0,4] => return 0
		if (t2 <= 0.0 || t2 >= 4.0) return 0.0;
		// [0,1)  first curve: 1/6 t^3
		if (t2 < 1.0) return sechstel * t2 * t2 * t2;
		// [0,2) second curve: 1/2 s^3 + 1/2 s^2 + 1/2 s + 1/6 (with s = t-1) => 1/6(-3 t^3 + 12 t^2 - 12 t + 4)
		if (t2 < 2.0) return sechstel * (((-3.0 * t2 + 12.0) * t2 - 12.0) * t2 + 4.0);
		// [0,3)  third curve: (1/2 s - 1)s^2 + 2/3 (with s = t-2)
		if (t2 < 3.0)
		{
			t2 -= 2.0;
			return (0.5f * t2 - 1.0) * t2 * t2 + zweidrittel;
		}
		// [0,4) fourth curve: -1/6 s^3 (with s = t-4)
		if (t2 < 4)
		{
			t2 -= 4.0;
			return -sechstel * t2 * t2 * t2;
		}
		return 0.0;
	}

	// returns the value of i-th cubic uniform b spline basis function at parameter t, clamped at 0 and e.
	static double N3cite(const size_t i, const double t, const size_t e)
	{
		assert(e > 0);
		assert(i <= e);
		assert(t >= 0);
		assert(t <= static_cast<double>(e));
		// i not in [0, e] or t not in (i-2, i+2) => return 0
		if (t <= i - 2.0 || t >= i + 2.0 || t > e || t < 0 || i > e || e == 0) return 0.0;
		// common case: four or more basis functions
		if (e > 2)
		{
			// first curve
			if (i == 0)
			{
				// not in non-zero intervall
				if (t >= 2.0) return 0.0;
				// t in [0,1]: t^3/6 - t + 1
				if (t < 1.0) return sechstel * t * t * t - t + 1.0;
				// t in [1,2]: (2-t)^3 / 6
				double t2 = 2.0 - t;
				return sechstel * t2 * t2 * t2;
			}
			// last curve: use symmetry
			if (i == e) return N3cite(0, static_cast<double>(e) - t, e);
			// second curve
			if (i == 1)
			{
				// not in non-zero intervall
				if (t >= 3.0) return 0.0;
				// t in [0,1]: -t^3/3 + t
				if (t < 1.0) return -drittel * t * t * t + t;
				// t in [1,2]: 1/6(3t^3 - 15t^2 + 21t - 5)
				if (t < 2.0) return sechstel * (((3.0 * t - 15.0) * t + 21.0) * t -5.0);
				// t in [2,3]: (3-t)^3 / 6
				double t2 = 3.0 - t;
				return sechstel * t2 * t2 * t2;
			}
			// second last curve: use symmetry
			if (i == e-1) return N3cite(1, static_cast<double>(e) - t, e);
			// else: normal unclamped n3 curve
			return N3it(i,t);
		}
		// length 2 => only 3 basis functions
		if (e == 2)
		{
			// first curve
			if (i == 0)
			{
				// t in [0,1]: t^3 / 6 - t + 1
				if (t < 1.0) return sechstel * t * t * t - t + 1.0;
				// t in [1,2]: (2-t)^3 / 6
				double t2 = 2.0 - t;
				return sechstel * t2 * t2 * t2;
			}
			// second curve
			if (i == 1)
			{
				// t in [0,1]: -t^3 / 3 + t
				if (t < 1.0) return - drittel * t * t * t + t;
				// t in [1,2]: -(2-t)^3 / 3 + (2-t)
				double t2 = 2.0 - t;
				return -drittel * t2 * t2 * t2 + t2;
			}
			// third curve, t in [0,1]: t^3 / 6
			if (t < 1.0) return sechstel * t * t * t;
			// t in [1,2]: (2-t)^3 / 6 - (2-t) + 1
			double t2 = 2.0 - t;
			return sechstel * t2 * t2 * t2 - t2 + 1.0;
		}
		// length 1 => only 2 basis functions => linear interpolation
		if (i == 0) return 1.0 - t;
		if (i == 1) return t;
		// default return 0 (but all cases should be handled above)
		return 0.0;
	}

	// returns the value of i-th cubic uniform b spline basis function at parameter t, clamped at s and e.
	static double N3citse(const size_t i, const double t, const size_t s, const size_t e)
	{
		assert(s < e);
		assert(i >= s);
		assert(i <= e);
		assert(t >= s);
		assert(t <= e);
		// reduce to N3cite case
		if (s >= e || i < s || i > e) return 0.0;
		return N3cite(i - s, t - static_cast<double>(s), e - s);
	}

	template <int n_ = -1>
	static void EvaluateBSplineBasisFunction(Eigen::Matrix<double, n_, 1> & bf, const double u_, const char options)
	{
		if (options & quadratic)
			EvaluateBSplineBasisFunction<n_>(bf, 2, n_ > 0 ? n_ : 3, u_, options);
		else if (options & cubic)
			EvaluateBSplineBasisFunction<n_>(bf, 3, n_ > 0 ? n_ : 4, u_, options);
		else
			assert(0);
	}

	template <int n_ = -1>
	static void EvaluateBSplineBasisFunction(Eigen::Matrix<double, n_, 1> & bf, const int p, int n, double u_, const char options = 0)
	{
		std::vector<int> u;
		int dimU;

		// calculate knot vector for (non-)uniform B-spline of degree p with n control points
		if (options & uniform)
		{
			// uniform
			n += p + 1;
			dimU = n;
			u.resize(dimU);
			for (int i = 0; i < n; ++i)
				u[i] = i;

			// scale u_ from [0,1] to [p,n-p-1]
			u_ = p + u_*(n - 2 * p - 1);
		}
		else
		{
			// non-uniform
			n -= p - 1;
			dimU = n + 2 * p;
			u.resize(dimU);
			int uMax = n - 1;
			for (int i = 0; i < p; ++i)
			{
				u[i] = 0;
				u[dimU - i - 1] = uMax;
			}
			for (int i = 0; i < n; ++i)
				u[p + i] = i;

			// scale u_ from [0,1] to [0,n-1]
			u_ *= uMax;
		}

		CalculateSplineBasisFunction<n_>(bf, u.data(), dimU, p, u_, (options & calculateDerivative) != 0);
	}

	template <int n_ = -1>
	static void CalculateSplineBasisFunction(Eigen::Matrix<double, n_, 1> & bf_, const int * u, const int dimU, const int p, const double u_, const bool calculateDerivative)
	{
		int m = dimU - 1;

		Eigen::VectorXd bf = Eigen::VectorXd::Zero(m);

		// initialize intervals of u with 1, which contain u_
		for (int i = 0; i < m; ++i)
		{
			if (u[i] <= u_ && u_ <= u[i + 1])
			{
				bf(i) = 1;
				if (u[i] != u[i + 1])
					break;
			}
		}

		// recursive calculation of B-spline basis functions of degree p
		for (int p_ = 1; p_ <= p; ++p_)
		{
			int cols = m - p_;
			Eigen::VectorXd temp(cols);
			for (int i = 0; i < cols; ++i)
			{
				double y1 = bf(i);
				double y2 = bf(i + 1);

				double d1, d2, e1, e2;

				double c1 = static_cast<double>(u[i + p_] - u[i]);
				if (c1 == 0.f)
					d1 = 0.f;
				else
					d1 = 1.f / c1;

				double c2 = static_cast<double>(u[i + p_ + 1] - u[i + 1]);
				if (c2 == 0.f)
					d2 = 0.f;
				else
					d2 = 1.f / c2;

				if (p_ == p && calculateDerivative)
				{
					// calculate derivatives of basis functions in the final step
					e1 = static_cast<double>(p);
					e2 = static_cast<double>(-p);
				}
				else
				{
					// calculate basis functions
					e1 = u_ - u[i];
					e2 = u[i + p_ + 1] - u_;
				}

				temp(i) = e1*d1*y1 + e2*d2*y2;
			}
			bf = temp;
		}

		bf_ = bf;
	}

private:

	// ===== EVALUATE HELPER FUNCTIONS =====

	// returns true, if t is in the range of the UBS parameter interval [0, #controlPoints - 1]
	bool parameterIsInIntervalRange(const double t) const
	{
		return parameterIsInIntervalRange(t, 0, controlPoints.size());
	}

	// returns true, if t is in the range of [s, e]
	bool parameterIsInIntervalRange(const double t, const size_t s, const size_t e) const
	{
		if (t < static_cast<double>(s) || t > static_cast<double>(e)) return false;
		return true;
	}

	// returns <s, e> where s is the previous sharp knot and e the next sharp knot of parameter t.
	std::pair<size_t, size_t> extractSharpInterval(const double t) const
	{
		assert(isValid());
		assert(parameterIsInIntervalRange(t));
		size_t s = static_cast<size_t>(t);
		while (sharpFlags[s] == false && s) --s;
		size_t e = static_cast<size_t>(t) + 1;
		while (sharpFlags[e] == false && e < sharpFlags.size() - 1) ++e;
		return std::pair<size_t, size_t>(s,e);
	}
};

typedef std::shared_ptr<UBS<3>> UBSPtr;
typedef std::weak_ptr<UBS<3>> UBSWeakPtr;
typedef std::vector<UBSPtr> UBSVec;

#ifdef RPE_DEFINE_UUID
RPE_DEFINE_UUID(UBSPtr, "9b2a99b3-77b6-4c4e-859d-4cbbad91e1fd")
RPE_DEFINE_UUID(UBSVec, "d47b92c8-1758-430d-930d-8f80bd576f5b")
#endif

#endif
