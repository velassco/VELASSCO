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
// Content: representation of discrete curves with chord length parametrization.
// ==========================================================================

// Explanation:
//   The discrete curve is represented by a set of adjacent points.
//   The chord length is calculated and saved too.

#ifndef DISCRETE_CURVE_H
#define DISCRETE_CURVE_H

#include <vector>
#include <memory>

#include <Eigen/Core>

template <size_t Dim>
class DiscreteCurve
{
private:

	// ===== MEMBERS =====

	std::vector<Eigen::Matrix<double, Dim, 1>>	points;						// set of discrete points
	std::vector<double>				chordalParametrization;		// chordal length for each point from the start of the set of discrete points
	double							segmentMinLength;			// the shortest segment in points
	double							segmentMaxLength;			// the longest segment in points

public:

	// creates a curve with given points and calculates its chordal parametrization
	DiscreteCurve(const std::vector<Eigen::Matrix<double, Dim, 1>> points_)
		: points(points_)
		, segmentMinLength(std::numeric_limits<double>::infinity())
		, segmentMaxLength(-std::numeric_limits<double>::infinity())
	{
		calculateChordalParametrization();
	}

	~DiscreteCurve() {}

	// returns true, when there are at least two points and the chordalParametrization is sorted and segmentMin/MaxLength are set.
	bool isValid() const
	{
		if (!hasValidSize()) return false;
		if (!hasValidParametrization()) return false;
		if (!hasValidSegmentLengths()) return false;
		return true;
	}
	bool hasValidSize() const
	{
		if (points.size() < 1) return false;
		if (chordalParametrization.size() != points.size()) return false;
		return true;
	}
	bool hasValidParametrization() const
	{
		for (size_t i = 1; i < chordalParametrization.size(); ++i)
			if (chordalParametrization[i] < chordalParametrization[i - 1]) return false;
		return true;
	}
	bool hasValidSegmentLengths() const
	{
		if (segmentMinLength != segmentMinLength) return false;
		if (segmentMaxLength != segmentMaxLength) return false;
		return true;
	}

	// getters
	size_t								getNumberPoints() const				{ return points.size(); }
	const std::vector<Eigen::Matrix<double, Dim, 1>>& getPoints() const		{ return points; }
	size_t								getNumberParameters() const			{ return chordalParametrization.size(); }
	const std::vector<double>&			getChordalParametrization() const	{ return chordalParametrization; }
	double								getChordalLength() const			{ return chordalParametrization.back(); }
	double								getSegmentMinLength() const			{ return segmentMinLength; }
	double								getSegmentMaxLength() const			{ return segmentMaxLength; }

	// replaces points with a uniform re-sampling and recalculates the chordal parametrization. numberPoints has to be bigger then 1. segmentLength has to be bigger then 0.
	void resample(const size_t numberPoints)
	{
		if (points.size() < 2 || numberPoints < 2) return;
		// place samples every segmentLength
		std::vector<Eigen::Matrix<double, Dim, 1>> newPoints;
		newPoints.reserve(numberPoints);
		newPoints.push_back(points.front());
		size_t iPoints = 1;
		double segmentLength =  chordalParametrization.back() / double(numberPoints - 1);
		for (size_t iNewPoints = 1; iNewPoints < numberPoints - 1; ++iNewPoints)
		{
			// find iPoints so that chordalParametrization[iPoints - 1] < targetParameter <= chordalParametrization[iPoints]
			double targetParameter = double(iNewPoints) * segmentLength;
			while (iPoints < points.size() && chordalParametrization[iPoints] < targetParameter) ++iPoints;
			// set new sample
			double distance = chordalParametrization[iPoints] - chordalParametrization[iPoints-1];
			double lambda1 = (chordalParametrization[iPoints] - targetParameter) / distance;
			double lambda2 = 1.0 - lambda1;
			Eigen::Matrix<double, Dim, 1> newPoint = lambda1 * points[iPoints - 1] + lambda2 * points[iPoints];
			newPoints.push_back(newPoint);
		}
		newPoints.push_back(points.back());
		points = newPoints;
		// finally calculate the new parametrization
		calculateChordalParametrization();
	}

	void resample(const double segmentLength)
	{
		if (segmentLength <= 0.0) return;
		// the required number of points is 1 + how often segmentLength fits into the curve length (rounded)
		size_t numberPoints = std::max(size_t(2), size_t(std::floor((chordalParametrization.back() / segmentLength) + 1.5)));
		resample(numberPoints);
	}

	// performs several iterations of a Gaussian blur with weights (0.25, 0.5, 0.25). does not modify the first and last point. recalculates the chordal parametrization afterwards.
	void smooth(const size_t iterations)
	{
		if (points.size() < 3) return;
		for (size_t iteration = 0; iteration < iterations; ++iteration)
		{
			std::vector<Eigen::Matrix<double, Dim, 1>> newPoints;
			newPoints.reserve(points.size());
			newPoints.push_back(points.front());
			for (size_t i = 1; i < points.size() - 1; ++i)
				newPoints.push_back(0.25 * (points[i-1] + points[i] + points[i] + points[i+1]));
			newPoints.push_back(points.back());
			points = newPoints;
		}
		calculateChordalParametrization();
	}

	// evaluate the curve at parameter t with linear interpolation between the discrete points
	Eigen::Matrix<double, Dim, 1> evaluate(const double t) const
	{
		//clamp to bounds of the curve
		if(t > chordalParametrization.back())
			return points.back();
		if(t <= chordalParametrization.front())
			return points.front();

		//linear interpolation between surrounding points
		auto nextIndex = std::lower_bound(chordalParametrization.begin(), chordalParametrization.end(), t) - chordalParametrization.begin();
		auto prevIndex = nextIndex - 1;
		double alpha = (t - chordalParametrization[prevIndex]) / (chordalParametrization[nextIndex] - chordalParametrization[prevIndex]);
		return alpha*points[nextIndex] + (1.0-alpha)*points[prevIndex];
	}

private:

	void calculateChordalParametrization()
	{
		segmentMinLength = std::numeric_limits<double>::infinity();
		segmentMaxLength = -std::numeric_limits<double>::infinity();
		chordalParametrization.resize(points.size());
		if (!points.empty())
		{
			chordalParametrization.front() = 0.0;
			for (size_t i = 1; i < points.size(); ++i)
			{
				double length = (points[i] - points[i-1]).norm();
				chordalParametrization[i] = chordalParametrization[i-1] + length;
				segmentMinLength = std::min(segmentMinLength, length);
				segmentMaxLength = std::max(segmentMaxLength, length);
			}
		}
	}
};

typedef std::shared_ptr<DiscreteCurve<3>> DiscreteCurvePtr;

typedef std::weak_ptr<DiscreteCurve<3>> DiscreteCurveWeakPtr;

template <size_t Dim>
std::ostream& operator<< (std::ostream& os, const DiscreteCurve<Dim>& discreteCurve)
{
	if (discreteCurve.isValid())
	{
		os << discreteCurve.getNumberPoints() << " points, chordal length "
			<< discreteCurve.getChordalLength() << ", delta t in ["
			<< discreteCurve.getSegmentMinLength() << ", "
			<< discreteCurve.getSegmentMaxLength() << "]"
			<< std::endl;
		for (size_t i = 0; i < discreteCurve.getNumberPoints(); ++i)
			os << "  " << discreteCurve.getPoints()[i].transpose() << ", t = " << discreteCurve.getChordalParametrization()[i] << std::endl;
	}
	else
	{
		os << "INVALID DISCRETE CURVE!" << std::endl;
		if (!discreteCurve.hasValidSize())
			os << "  " << discreteCurve.getNumberPoints() << " points but " << discreteCurve.getNumberParameters() << " parameters." << std::endl;
		if (!discreteCurve.hasValidParametrization())
			os << "  Parametrization invalid." << std::endl;
		if (!discreteCurve.hasValidSegmentLengths())
			os << "  Segment Lengths (min and max) not set." << std::endl;
	}
	return os;
}

template <size_t Dim>
std::ostream& operator<< (std::ostream& os, const std::shared_ptr<DiscreteCurve<Dim>> discreteCurve)
{
	os << discreteCurve.get();
	return os;
}

#endif
