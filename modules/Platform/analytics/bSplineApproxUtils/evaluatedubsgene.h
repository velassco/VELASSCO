// ==========================================================================
// Dipl.-Informatiker Matthias Borner
// 'Interaktive Engineering Technologien'
//
// Fraunhofer-Institut f�r Graphische Datenverarbeitung IGD
// Fraunhoferstr. 5 | 64283 Darmstadt | Germany
// Telefon +49 6151 155-663 | Fax +49 6151 155-139
// matthias.borner@igd.fraunhofer.de | http://www.igd.fraunhofer.de/iet
//
// Creation Date: 2015_06_02
// ==========================================================================

#ifndef EVALUATED_UBS_GENE_H
#define EVALUATED_UBS_GENE_H

#include <vector>

#include "ubsgene.h"
#include "ubs.h"

template <size_t Dim>
class EvaluatedUBSGene
{
private:

	// ===== MEMBERS =====
	const std::shared_ptr<UBSGene<Dim>>				ubsGene;						// the referenced ubs gene
	std::shared_ptr<UBS<Dim>>							ubs;							// the approximated ubs corresponding to the gene
	std::vector<Eigen::Matrix<double, Dim, 1>>	points;							// points of the ubs evaluated at the parametrization of the gene
	std::vector<double>				errors;							// the distance between each pair of points on the discrete curve and the approximated ubs
	double							maxError;						// the largest error between the ubs and the discrete curve
	size_t							maxErrorIndex;					// the index of the largest error in the curve
	double							summedError;					// sum of all errors
	double							summedSquaredError;				// sum of all squared errors. this is minimized by the least squares approximation
	std::vector<double>				absoluteSegmentErrors;			// sum of all errors for each segment
	double							absoluteMaxSegmentError;		// the largest segment error of all segments
	size_t							absoluteMaxSegmentErrorIndex;	// the index of the largest segment error
	std::vector<double>				relativeSegmentErrors;			// sum of all errors for each segment, divided by the chordal length of the segment
	double							relativeMaxSegmentError;		// the largest relative segment error of all segments
	size_t							relativeMaxSegmentErrorIndex;	// the index of the largest relative segment error

public:

	// takes the gene, approximates and evaluates the ubs
	EvaluatedUBSGene(const std::shared_ptr<UBSGene<Dim>> ubsGene)
		: ubsGene(ubsGene)
		, ubs(nullptr)
		, maxError(-std::numeric_limits<double>::infinity())
		, maxErrorIndex(0)
		, summedError(0.0)
		, summedSquaredError(0.0)
		, absoluteMaxSegmentError(-std::numeric_limits<double>::infinity())
		, absoluteMaxSegmentErrorIndex(0)
		, relativeMaxSegmentError(-std::numeric_limits<double>::infinity())
		, relativeMaxSegmentErrorIndex(0)
	{
		// approximate a ubs with the given gene. we suppose the gene is set up correctly.
		ubs = std::make_shared<UBS<Dim>>(ubsGene->calculateApproximation());
		// evaluate the ubs at the same parameters used for the approximation
		points = ubs->evaluate(ubsGene->getParametrization());
		// evaluate the error
		errors.reserve(points.size());
		absoluteSegmentErrors.resize(ubsGene->getSegmentation().size() - 1, 0.0);
		relativeSegmentErrors.resize(ubsGene->getSegmentation().size() - 1, 0.0);
		summedError = 0.0;
		const auto& discretePoints = ubsGene->getDiscreteCurve()->getPoints();
		const auto& parameterization = ubsGene->getParametrization();
		for (size_t i = 0; i  < points.size(); ++i)
		{
			// calculate single errors
			double d = (points[i] - discretePoints[i]).norm();
			errors.push_back(d);
			if (d > maxError)
			{
				maxError = d;
				maxErrorIndex = i;
			}
			summedError += d;
			summedSquaredError += d*d;
			// calculate segment errors
			size_t segmentIndex = std::min(static_cast<size_t>(parameterization[i]), absoluteSegmentErrors.size() - 1);
			absoluteSegmentErrors[segmentIndex] += d;
		}
		// find largest segment error and calculate relative segment errors
		const auto& segmentation = ubsGene->getSegmentation();
		for (size_t i = 0; i < absoluteSegmentErrors.size(); ++i)
		{
			if (absoluteSegmentErrors[i] > absoluteMaxSegmentError)
			{
				absoluteMaxSegmentError = absoluteSegmentErrors[i];
				absoluteMaxSegmentErrorIndex = i;
			}
			relativeSegmentErrors[i] = absoluteSegmentErrors[i] / (segmentation[i+1] - segmentation[i]);
			if (relativeSegmentErrors[i] > relativeMaxSegmentError)
			{
				relativeMaxSegmentError = relativeSegmentErrors[i];
				relativeMaxSegmentErrorIndex = i;
			}
		}
	}

	~EvaluatedUBSGene() {}

	// direct getters
	const std::shared_ptr<UBSGene<Dim>> getUbsGene() const { return ubsGene; }
	const std::shared_ptr<UBS<Dim>> getUbs() const { return ubs; }
	const std::vector<Eigen::Matrix<double, Dim, 1>>& getPoints() const { return points; }
	const double getMaxError() const { return maxError; }
	const size_t getMaxErrorIndex() const { return maxErrorIndex; };
	const double getSummedError() const { return summedError; }
	const double getSummedSquaredError() const { return summedSquaredError; }
	const std::vector<double> getAbsoluteSegmentErrors() const { return absoluteSegmentErrors; }
	const double getAbsoluteMaxSegmentError() const { return absoluteMaxSegmentError; }
	const size_t getAbsoluteMaxSegmentErrorIndex() const { return absoluteMaxSegmentErrorIndex; }
	const std::vector<double> getRelativeSegmentErrors() const { return relativeSegmentErrors; }
	const double getRelativeMaxSegmentError() const { return relativeMaxSegmentError; }
	const size_t getRelativeMaxSegmentErrorIndex() const { return relativeMaxSegmentErrorIndex; }

private:
};

typedef std::shared_ptr<EvaluatedUBSGene<3>> EvaluatedUBSGenePtr;

typedef std::weak_ptr<EvaluatedUBSGene<3>> EvaluatedUBSGeneWeakPtr;

template <size_t Dim>
std::ostream& operator<< (std::ostream& os, const EvaluatedUBSGene<Dim>& evaluatedUBSGene)
{
	os << "\n---------------------------------" <<
		"\nApproximation statistics:" <<
		"\n  discrete points = " << evaluatedUBSGene.getPoints().size() <<
		"\n  control points = " << evaluatedUBSGene.getUbs()->getNumberControlPoints() <<
		//"\n" <<
		"\nApproximation error:" <<
		"\n  max = " << evaluatedUBSGene.getMaxError() <<
		" at index " << evaluatedUBSGene.getMaxErrorIndex() <<
		"\n  sum = " << evaluatedUBSGene.getSummedError() <<
		"\n  squared sum = " << evaluatedUBSGene.getSummedSquaredError() <<
		"\n  average = " << evaluatedUBSGene.getSummedError() / evaluatedUBSGene.getPoints().size() <<
		//"\n" <<
		"\n  max segment error = " << evaluatedUBSGene.getAbsoluteMaxSegmentError() <<
		" at index " << evaluatedUBSGene.getAbsoluteMaxSegmentErrorIndex() <<
		//"\n" <<
		"\n  rel segment error = " << evaluatedUBSGene.getRelativeMaxSegmentError() <<
		" at index " << evaluatedUBSGene.getRelativeMaxSegmentErrorIndex() <<
		"\n---------------------------------" <<
		"\n";
	return os;
}

template <size_t Dim>
std::ostream& operator<< (std::ostream& os, const std::shared_ptr<EvaluatedUBSGene<Dim>> evaluatedUBSGene)
{
	os << evaluatedUBSGene.get();
	return os;
}

#endif
