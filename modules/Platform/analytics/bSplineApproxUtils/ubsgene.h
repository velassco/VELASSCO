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
// Content: mathematics for approximation of discrete curves with uniform cubic bsplines
// ==========================================================================

// Explanation:
//   The discrete curve is segmented and a C1 continuous parametrization is calculated accordingly.
//   The segmentation is based on the chordal length parametrization of the discrete curve. So its values are in [0.0, discreteCurve.getChordalLength()].
//   Each segment has the parameter length of 1. The parameter interval is [0, #segments - 1].

#ifndef UBS_GENE_H
#define UBS_GENE_H

#include <vector>

#include "discretecurve.h"
#include "ubs.h"

#include <Eigen/Cholesky>

template <size_t Dim>
class UBSGene
{
private:

	// ===== MEMBERS =====
	const std::shared_ptr<DiscreteCurve<Dim>>	discreteCurve;		// the discrete curve we want to approximate
	std::vector<double>		segmentation;		// tells where b spline segments start and end in [0.0, discreteCurve.getChordalLength()]. it is based on the chordal length parametrization of the discrete curve.
	std::vector<bool>		sharpFlags;			// flag for each control point, if it is considered smooth or sharp
	size_t					numberSharp;		// the number of sharp declared flags
	std::vector<double>		parametrization;	// the parametrization according to the segmentation of the discrete curve (C1 continuous). each segment has the parameter length of 1.0.

public:

	// creates a uniform b spline gene with uniformly distributed segments.
	// all inner control points are flagged smooth. ends are sharp.
	// numberUniformSegments has to be bigger then 0.
	UBSGene(const std::shared_ptr<DiscreteCurve<Dim>> discreteCurve, const size_t numberUniformSegments)
		: discreteCurve(discreteCurve)
	{
		setSegmentation(createUniformDistribution(0.0, discreteCurve->getChordalLength(), numberUniformSegments + 1));
	}

	// creates a b spline gene with the given segmentation.
	// if sharpFlags are not specified, the default sharp flags (all inner points are smooth) is applied
	UBSGene(const std::shared_ptr<DiscreteCurve<Dim>> discreteCurve, const std::vector<double>& segmentation, const std::vector<bool>& sharpFlags = std::vector<bool>())
		: discreteCurve(discreteCurve)
	{
		setSegmentation(segmentation, sharpFlags);
	}

	~UBSGene() {}

	// direct getters
	const std::shared_ptr<DiscreteCurve<Dim>> getDiscreteCurve() const { return discreteCurve; }
	const std::vector<double>& getSegmentation() const { return segmentation; }
	const std::vector<bool>& getSharpFlags() const { return sharpFlags; }
	const std::vector<double>& getParametrization() const { return parametrization; }

	// sets the segmentation of the discrete curve (at which chordal length parameter a segment starts and ends).
	// parameters have to be within [0.0, discreteCurve.getChordalLength()].
	// also needs the sharp flags for each control point.
	// recalculates the parametrization
	void setSegmentation(const std::vector<double>&	segmentation_, const std::vector<bool>& sharpFlags_ = std::vector<bool>())
	{
		segmentation = segmentation_;
		if (segmentation.size() != sharpFlags_.size())
			sharpFlags = createDefaultSharpFlags(segmentation.size());
		else
			sharpFlags = sharpFlags_;
		// force sharp start end end, no matter what has been given
		if (!sharpFlags.empty())
		{
			sharpFlags.front() = true;
			sharpFlags.back() = true;
		}
		// recalculate stuff
		numberSharp = 0;
		for (auto const & sharp : sharpFlags) if (sharp) numberSharp++;
		calculateC1Parametrization();
	}

	// calculates the uniform b spline to the given parametrization which is deduced from the segmentation
	UBS<Dim> calculateApproximation() const
	{
		std::vector<Eigen::Matrix<double, Dim, 1>> cps;
		cps.reserve(segmentation.size() + 1);
		cps.emplace_back(discreteCurve->getPoints().front());
		// iterate over sharp segments and approximate separately
		auto sharpFlagIndizes = extractSharpFlagIndizes();
		for (size_t i = 0; i < sharpFlagIndizes.size() - 1; ++i)
		{
			// approximate cps for this segment
			auto partialCps = calculatePartialApproximation(sharpFlagIndizes[i], sharpFlagIndizes[i + 1]);
			cps.insert(cps.end(), partialCps.begin() + 1, partialCps.end());
		}
		return UBS<Dim>(cps, sharpFlags);
	}

	// returns a new gene, where the i-th segment point has moved closer to its previous or next segment point, depending on the sign of the relative moving distance-
	// only works for inner segment points (segmentationIndex > 0 && segmentationIndex < segmentation.size() - 1)
	// e.g. relativeMovement -0.5 will move the segment point half the distance to the previous segment point.
	// e.g. relativeMovement +0.2 will move the segment point half 20% the distance to the next segment point.
	UBSGene<Dim> createNewGeneByMovingSegmentPoint(const size_t segmentationIndex, const double relativeMovement) const
	{
		assert(segmentationIndex > 0);
		assert(segmentationIndex < segmentation.size() - 1);
		assert(relativeMovement > -1.0);
		assert(relativeMovement < 1.0);

		// create new segmentation
		double distance;
		if (relativeMovement < 0.0)
			distance = segmentation[segmentationIndex] - segmentation[segmentationIndex - 1];
		else
			distance = segmentation[segmentationIndex + 1] - segmentation[segmentationIndex];
		auto newSegmentation = segmentation;
		newSegmentation[segmentationIndex] += distance * relativeMovement;

		return UBSGene(discreteCurve, newSegmentation, sharpFlags);
	}

	// returns a new gene, where the segmentPoint has been inserted (and sorted) along with its sharp flag.
	UBSGene<Dim> createNewGeneByInsertingSegmentPoint(const double segmentPoint, const bool sharpFlag) const
	{
		auto newSegmentation = segmentation;
		auto newSharpFlags = sharpFlags;
		auto segmentationIndex = getLowerSegmentationIndex(segmentPoint);
		newSegmentation.insert(newSegmentation.begin() + segmentationIndex, segmentPoint);
		newSharpFlags.insert(newSharpFlags.begin() + segmentationIndex, sharpFlag);
		return UBSGene(discreteCurve, newSegmentation, newSharpFlags);
	}

	// returns a new gene, where a segmentPoint has been inserted (and sorted) in the middle of segment[segmentIndex] along with its sharp flag.
	UBSGene<Dim> createNewGeneByInsertingSegmentPoint(const size_t segmentIndex, const bool sharpFlag) const
	{
		double segmentPoint = 0.5 * (segmentation[segmentIndex] + segmentation[segmentIndex + 1]);
		return createNewGeneByInsertingSegmentPoint(segmentPoint, sharpFlag);
	}

	// returns the index i so that segmentation[i] <= segmentationPoint < segmentation[i+1]
	size_t getLowerSegmentationIndex(const double segmentPoint) const
	{
		auto low = std::lower_bound(segmentation.begin(), segmentation.end(), segmentPoint);
		return low - segmentation.begin();
	}

private:

	std::vector<double> createUniformDistribution(double start, double end, size_t nrValues)
	{
		std::vector<double> distribution(nrValues, start);
		double length = end - start;
		double delta = length / static_cast<double>(nrValues - 1);
		for (size_t i = 1; i < nrValues - 1; ++i)
		{
			distribution[i] = static_cast<double>(i) * delta + start;
		}
		distribution.back() = end;
		return distribution;
	}

	std::vector<bool> createDefaultSharpFlags(size_t nrFlags)
	{
		std::vector<bool> sharpFlags(nrFlags, false);
		if (sharpFlags.empty()) return sharpFlags;
		sharpFlags.front() = true;
		sharpFlags.back() = true;
		return sharpFlags;
	}

	// returns a vector with indices of cps which are flagged sharp
	std::vector<size_t> extractSharpFlagIndizes() const
	{
		std::vector<size_t> indizes;
		indizes.reserve(numberSharp);
		for (size_t i = 0; i < sharpFlags.size(); ++i)
			if (sharpFlags[i]) indizes.push_back(i);
		return indizes;
	}

	// calculates the C0 continuous parametrization to the current segmentation
	void calculateC0Parametrization()
	{
		// initialize
		parametrization.resize(discreteCurve->getNumberPoints(), 0.0);

		// simple linear interpolation depending on the chordal parametrization
		auto& chordalParametrization = discreteCurve->getChordalParametrization();
		double startParameter = 0.0;
		double segmentOffset = -1.0;
		size_t parameterIndex = 0;
		for (auto segmentParameter : segmentation)
		{
			double segmentLength = segmentParameter - startParameter;
			while (chordalParametrization[parameterIndex] < segmentParameter)
			{
				parametrization[parameterIndex] = segmentOffset + (chordalParametrization[parameterIndex] - startParameter) / segmentLength;
				parameterIndex++;
			}
			startParameter = segmentParameter;
			segmentOffset += 1.0;
		}
		// last point is not mapped by the loop above, so append it manually
		parametrization.back() = segmentOffset;
	}

	// calculates the C1 continuous parametrization to the current segmentation
	void calculateC1Parametrization()
	{
		// initialize
		parametrization.resize(discreteCurve->getNumberPoints(), 0.0);

		// iterate over sharp segments
		auto& chordalParametrization = discreteCurve->getChordalParametrization();
		double segmentOffset = 0.0;
		size_t iParameter = 0;
		auto sharpFlagIndizes = extractSharpFlagIndizes();
		for (size_t iSharp = 0; iSharp < sharpFlagIndizes.size() - 1; ++iSharp)
		{
			// determine the derivatives at each segmentation point
			size_t startSegment = sharpFlagIndizes[iSharp];
			size_t endSegment = sharpFlagIndizes[iSharp + 1];
			std::vector<double> derivatives(endSegment - startSegment + 1, 0.0);
			for (size_t iSharp = startSegment + 1; iSharp < endSegment; ++iSharp)
			{
				derivatives[iSharp - startSegment] = 2.0 / (segmentation[iSharp+1] - segmentation[iSharp-1]);
			}
			if (derivatives.size() > 2)
			{
				derivatives.front() = 2.0 / (segmentation[startSegment + 1] - segmentation[startSegment]) - derivatives[1];
				derivatives.back() = 2.0 / (segmentation[endSegment] - segmentation[endSegment-1]) - derivatives[derivatives.size() - 2];
			}
			else
			{
				derivatives.front() = 1.0 / (segmentation[startSegment + 1] - segmentation[startSegment]);
				derivatives.back() = 1.0 / (segmentation[endSegment] - segmentation[endSegment-1]);
			}
			// iterate over every single segment and map the parameters
			for (size_t iSegment = 0; iSegment < derivatives.size() - 1; ++iSegment)
			{
				double startParameter = segmentation[startSegment + iSegment];
				double endParameter = segmentation[startSegment + iSegment + 1];
				double paraLength = endParameter - startParameter;
				double a, b, c;
				calculateC1Coefficients(paraLength, derivatives[iSegment], derivatives[iSegment + 1], a, b, c);
				while (iParameter < chordalParametrization.size() && chordalParametrization[iParameter] <= endParameter)
				{
					parametrization[iParameter] = segmentOffset + mapParameterC1(chordalParametrization[iParameter] - startParameter, paraLength, a, b, c);
					iParameter++;
				}
				segmentOffset += 1.0f;
			}
		}
	}

	// calculates the approximation for a segment starting and ending at the given indices.
	// at start and end, the sharpFlags have to be true and in between, the sharpFlags have to be false.
	// so it is one smooth b spline interpolating start and end.
	std::vector<Eigen::Matrix<double, Dim, 1>> calculatePartialApproximation(size_t startIndex, size_t endIndex) const
	{
		std::vector<Eigen::Matrix<double, Dim, 1>> result;
		size_t numCPs = endIndex - startIndex + 1;
		result.reserve(numCPs);
		//first CP is taken directly from the discrete curve
		result.emplace_back(discreteCurve->evaluate(segmentation[startIndex]));

		// more than 2 CPs => approximate intermediate CPs
		if (numCPs > 2)
		{
			// extract clamping parameter as int
			int clampS = static_cast<int>(startIndex);
			int clampE = static_cast<int>(endIndex);
			auto pointStart = std::lower_bound(parametrization.begin(), parametrization.end(), static_cast<double>(startIndex)) - parametrization.begin();
			auto pointEnd = std::upper_bound(parametrization.begin(), parametrization.end(), static_cast<double>(endIndex)) - parametrization.begin() - 1;

			// calculate modified data points P (extract influence of first and last point which are interpolated) => reduces linear system complexity
			size_t nPoints = pointEnd - pointStart + 1;
			auto const & points = discreteCurve->getPoints();
			Eigen::MatrixXd P(nPoints, Dim);
			for (size_t i = 0; i < nPoints; ++i)
			{
				size_t currentPointIndex = pointStart + i;
				double v1 = UBS<Dim>::N3citse(startIndex, parametrization[currentPointIndex], clampS, clampE);
				double v2 = UBS<Dim>::N3citse(endIndex, parametrization[currentPointIndex], clampS, clampE);
				for (size_t d = 0; d < Dim; d++)
					P(i,d) = points[currentPointIndex](d) - v1*points[pointStart](d) - v2*points[pointEnd](d);
			}

			// calculate matrix Nij = N3citse(cpStart + i, parameters(pointStart  + j), clampS, clampE)
			Eigen::MatrixXd Nij(numCPs - 2, nPoints);
			for (size_t i = 0; i < numCPs - 2; ++i)
			{
				size_t currentCPIndex = startIndex + i + 1;
				for (size_t j = 0; j < nPoints; ++j)
				{
					Nij(i, j) = UBS<Dim>::N3citse(currentCPIndex, parametrization[pointStart + j], clampS, clampE);
				}
			}

			// calculate right side Q = Nij * P
			Eigen::MatrixXd Q = Nij * P;
			// calculate square, pos definit and symmetric Matrix Mij = Nij*Nij'
			Eigen::MatrixXd Mij = Nij * Nij.transpose();
			// solve system Nij * Nij' * X = Nij * P, which is now Mij * X = Q (least squares result of Nij' * X = P)
			Eigen::MatrixXd X(numCPs - 2, Dim);
			X = Mij.ldlt().solve(Q);
			// Write solution for inner cps
			for (size_t i = 0; i < numCPs - 2; i++)
				result.emplace_back(X.row(i));
		}

		//last CP is taken directly from the discrete curve
		result.emplace_back(discreteCurve->evaluate(segmentation[endIndex]));
		//return set of calculated CPs
		return result;
	}


	void calculateC1Coefficients(const double paraLength, const double derivativeStart, const double derivativeEnd, double& a, double& b, double& c)
	{
		assert(paraLength > 0.0);
		assert(derivativeStart > 0.0);
		assert(derivativeEnd > 0.0);
		// we will scale the given parameter to [0, 1] so the polynomial can be evaluated more easily
		// therefor we have to scale the derivatives as well => derivativeStart�, derivativeEnd�
		// calculate polynomial coefficients a,b,c,d of P(x) = ax^3 + bx^2 + cx + d. with P(0) = 0, P'(0) = derivativeStart�, P(1) = 1, P'(1) = derivativeEnd�
		// P(0)  = d = 0
		// P'(0) = c = derivativeStart� = derivativeStart * paraLength (due to scaling to local length 1.0)
		c = derivativeStart * paraLength;
		// P(1)  =  a +  b + c    (1 1) (a) (P(1) -c)    (a) (-2  1) (P(1) -c) = -2 + 2c + derivativeEnd� - c = -2 + c + derivativeEnd�                 =>  (derivativeStart+derivativeEnd)� - 2
		// P'(1) = 3a + 2b + c => (3 2)*(b)=(P'(1)-c) => (b)=( 3 -1)*(P'(1)-c) = 3 - 3c - derivativeEnd� + c  = 3 - 2derivativeStart� - derivativeEnd�  =>  3 - (derivativeStart+derivativeStart+derivativeEnd)�
		a = ((derivativeStart + derivativeEnd) * paraLength) - 2.0;
		b = 3.0 - ((derivativeStart + derivativeStart + derivativeEnd) * paraLength);
	}

	double mapParameterC1(const double parameter, const double paraLength, const double a, const double b, const double c)
	{
		double x = parameter / paraLength;
		double result = x * (c + x * (b + x * a));
		return std::max(0.0, std::min(result, 1.0));
	}
};

typedef std::shared_ptr<UBSGene<3>> UBSGenePtr;

typedef std::weak_ptr<UBSGene<3>> UBSGeneWeakPtr;

#endif
