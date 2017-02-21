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

#ifndef ITERATIVE_APPROXIMATION_H
#define ITERATIVE_APPROXIMATION_H

#include "evaluatedubsgene.h"

namespace IterativeApproximation
{
	// different methods how to approximate a discrete curve
	// only good working methods are included
	enum class ApproximationMode : size_t
	{
		UNIFORM = 0,					// simply segment the discrete curve uniformly
		UNIFORM_OPTIMIZE,				// simply segment the discrete curve uniformly and optimize by moving segment points afterwards
		ITERATIVE_SEGMENT_OPTIMIZED		// iteratively insert segment points in segments and optimize by moving segment points after each insert
	};

	// returns an optimized version of the evaluatedUBSGene by jiggling(moving) control points around.
	// it iterates over all inner points and moves them left and right in their segmentation interval (with a relative distance defined by relativeJiggleDistane).
	// when all points are optimal, the distance is decreased to the half size. this happens jiggleDistanceReductions times.
	// optionally returns the number of approximations done.
	// NYI: time and error based exit.
	template <size_t Dim>
	std::shared_ptr<EvaluatedUBSGene<Dim>> optimizeByJiggling(
		const std::shared_ptr<EvaluatedUBSGene<Dim>> evaluatedUBSGene,
		size_t* nrApproximations = nullptr,
		const double relativeStartJiggleDistance = 0.5,
		const size_t jiggleDistanceReductions = 6)
	{
		// remember original data. keep reference on discrete curve and copy segmentation, since it gets altered here
		auto bestEvaluatedGene = evaluatedUBSGene;
		auto nrCPs = evaluatedUBSGene->getUbsGene()->getSegmentation().size();

		// first move points a long distance, later reduce the distance by half
		for (size_t iDistanceReductions = 0; iDistanceReductions < jiggleDistanceReductions; ++iDistanceReductions)
		{
			// determine the current relative movement distance
			auto relativeMovementDistance = relativeStartJiggleDistance / std::pow(2.0, static_cast<double>(iDistanceReductions));

			// keep optimizing as long any point got improved
			bool anyBetter = false;
			do
			{
				// iterate over all inner points
				anyBetter = false;
				for (size_t iCP = 1; iCP < nrCPs - 1; iCP++)
				{
					// jiggle that point until it is optimized
					bool pointBetter = false;
					do
					{
						pointBetter = false;

						// create new genes and approximate them
						std::shared_ptr<UBSGene<Dim>> geneLeft = std::make_shared<UBSGene<Dim>>(bestEvaluatedGene->getUbsGene()->createNewGeneByMovingSegmentPoint(iCP, -relativeMovementDistance));
						std::shared_ptr<UBSGene<Dim>> geneRight = std::make_shared<UBSGene<Dim>>(bestEvaluatedGene->getUbsGene()->createNewGeneByMovingSegmentPoint(iCP, relativeMovementDistance));
						std::shared_ptr<EvaluatedUBSGene<Dim>> evaluatedGeneLeft = std::make_shared<EvaluatedUBSGene<Dim>>(geneLeft);
						std::shared_ptr<EvaluatedUBSGene<Dim>> evaluatedGeneRight = std::make_shared<EvaluatedUBSGene<Dim>>(geneRight);
						if (nrApproximations)
							*nrApproximations += 2;

						// keep the best
						auto bestError = bestEvaluatedGene->getSummedError();
						auto errorLeft = evaluatedGeneLeft->getSummedError();
						auto errorRight = evaluatedGeneRight->getSummedError();
						if (errorLeft < bestError || errorRight < bestError)
						{
							if (errorLeft < errorRight)
							{
								bestEvaluatedGene = evaluatedGeneLeft;
								pointBetter = true;
							}
							else
							{
								bestEvaluatedGene = evaluatedGeneRight;
								pointBetter = true;
							}
						}
					} while (pointBetter); // jiggle that point until it is optimized
					if (pointBetter) anyBetter = true;
				} // iterate over all inner points
			} while (anyBetter); // keep optimizing as long any point got improved
		} // first move points a long distance, later reduce the distance by half

		return bestEvaluatedGene;
	}

	// insert a control point in the middle of the segment with the largest summed error
	template <size_t Dim>
	std::shared_ptr<EvaluatedUBSGene<Dim>> insertSegmentAtAbsoluteMaxSegmentError(
		const std::shared_ptr<EvaluatedUBSGene<Dim>> evaluatedUBSGene,
		size_t* nrApproximations = nullptr)
	{
		auto absoluteMaxSegmentErrorIndex = evaluatedUBSGene->getAbsoluteMaxSegmentErrorIndex();
		auto newGene = std::make_shared<UBSGene<Dim>>(evaluatedUBSGene->getUbsGene()->createNewGeneByInsertingSegmentPoint(absoluteMaxSegmentErrorIndex, false));
		auto newEvaluatedGene = std::make_shared<EvaluatedUBSGene<Dim>>(newGene);
		if (nrApproximations) *nrApproximations += 1;
		return newEvaluatedGene;
	}

	// iteratively insert a control point in the middle of the segment with the largest summed error and optimize it afterwards by moving control points
	template <size_t Dim>
	std::shared_ptr<EvaluatedUBSGene<Dim>> insertSegmentsAtAbsoluteMaxSegmentErrorAndOptimize(
		const std::shared_ptr<EvaluatedUBSGene<Dim>> evaluatedUBSGene,
		const size_t iterations,
		size_t* nrApproximations = nullptr)
	{
		std::shared_ptr<EvaluatedUBSGene<Dim>> result = evaluatedUBSGene;
		for (size_t i = 0; i < iterations; ++i)
		{
			result = IterativeApproximation::optimizeByJiggling(result, nrApproximations);
			result = IterativeApproximation::insertSegmentAtAbsoluteMaxSegmentError(result, nrApproximations);
		}
		result = IterativeApproximation::optimizeByJiggling(result, nrApproximations);
		return result;
	}

	// convenience function for easy calls of various approximations
	template <size_t Dim>
	std::shared_ptr<EvaluatedUBSGene<Dim>> iterativeApproximation(
		const std::shared_ptr<DiscreteCurve<Dim>> discreteCurve,
		const size_t numCps,
		size_t* nrApproximations = nullptr,
		ApproximationMode approximationMode = ApproximationMode::ITERATIVE_SEGMENT_OPTIMIZED)
	{
		switch (approximationMode)
		{
		case ApproximationMode::UNIFORM:
			{
				std::shared_ptr<UBSGene<Dim>> ubsGene = std::make_shared<UBSGene<Dim>>(discreteCurve, numCps - 1);
				std::shared_ptr<EvaluatedUBSGene<Dim>> evaluatedUbsGene = std::make_shared<EvaluatedUBSGene<Dim>>(ubsGene);
				if (nrApproximations) nrApproximations += 1;
				return evaluatedUbsGene;
			}
			break;
		case ApproximationMode::UNIFORM_OPTIMIZE:
			{
				std::shared_ptr<UBSGene<Dim>> ubsGene = std::make_shared<UBSGene<Dim>>(discreteCurve, numCps - 1);
				std::shared_ptr<EvaluatedUBSGene<Dim>> evaluatedUbsGene = std::make_shared<EvaluatedUBSGene<Dim>>(ubsGene);
				evaluatedUbsGene = IterativeApproximation::optimizeByJiggling(evaluatedUbsGene, nrApproximations);
				return evaluatedUbsGene;
			}
			break;
		case ApproximationMode::ITERATIVE_SEGMENT_OPTIMIZED:
		default:
			{
				std::shared_ptr<UBSGene<Dim>> ubsGene = std::make_shared<UBSGene<Dim>>(discreteCurve, 1);
				std::shared_ptr<EvaluatedUBSGene<Dim>> evaluatedUbsGene = std::make_shared<EvaluatedUBSGene<Dim>>(ubsGene);
				evaluatedUbsGene = IterativeApproximation::insertSegmentsAtAbsoluteMaxSegmentErrorAndOptimize(evaluatedUbsGene, numCps - 2, nrApproximations);
				return evaluatedUbsGene;
			}
			break;
		}
	}

	// insert a control point at the largest deviation of the evaluated ubs
	template <size_t Dim>
	std::shared_ptr<EvaluatedUBSGene<Dim>> insertSegmentAtLargesDeviation(
		const std::shared_ptr<EvaluatedUBSGene<Dim>> evaluatedUBSGene,
		size_t* nrApproximations = nullptr,
		const double relativeBlockDistance = 0.0)
	{
		//if (relativeBlockDistance <= 0.0)
		//{
		auto maxErrorIndex = evaluatedUBSGene->getMaxErrorIndex();
		auto newSegmentPoint = evaluatedUBSGene->getUbsGene()->getDiscreteCurve()->getChordalParametrization()[maxErrorIndex];
		auto const & segmentation = evaluatedUBSGene->getUbsGene()->getSegmentation();
		//check if point already exists
		if(std::find(std::begin(segmentation), std::end(segmentation), newSegmentPoint) != std::end(segmentation))
		{
			//if yes, we should insert a point somewhere else
			//currently we just return
			return evaluatedUBSGene;
		}
		auto newGene = std::make_shared<UBSGene<Dim>>(evaluatedUBSGene->getUbsGene()->createNewGeneByInsertingSegmentPoint(newSegmentPoint, false));
		auto newEvaluatedGene = std::make_shared<EvaluatedUBSGene<Dim>>(newGene);
		if (nrApproximations) *nrApproximations += 1;
		return newEvaluatedGene;
		//}
		// TODO: take block distance into account to prevent creating close segment points.
	}

	// iteratively insert a control point at the largest deviation of the evaluated ubs and optimize it afterwards by moving control points
	template <size_t Dim>
	std::shared_ptr<EvaluatedUBSGene<Dim>> insertSegmentsAtLargesDeviationAndOptimize(
		const std::shared_ptr<EvaluatedUBSGene<Dim>> evaluatedUBSGene,
		const size_t iterations,
		size_t* nrApproximations = nullptr,
		double relativeBlockDistance = 0.0)
	{
		std::shared_ptr<EvaluatedUBSGene<Dim>> result = evaluatedUBSGene;
		for (size_t i = 0; i < iterations; ++i)
		{
			result = IterativeApproximation::optimizeByJiggling(result, nrApproximations);
			result = IterativeApproximation::insertSegmentAtLargesDeviation(result, nrApproximations, relativeBlockDistance);
		}
		result = IterativeApproximation::optimizeByJiggling(result, nrApproximations);
		return result;
	}

	// insert a control point in the middle of the segment with the largest relative error (summed error divided by chordal length of the segment)
	template <size_t Dim>
	std::shared_ptr<EvaluatedUBSGene<Dim>> insertSegmentAtRelativeMaxSegmentError(
		const std::shared_ptr<EvaluatedUBSGene<Dim>> evaluatedUBSGene,
		size_t* nrApproximations = nullptr)
	{
		auto relativeMaxSegmentErrorIndex = evaluatedUBSGene->getRelativeMaxSegmentErrorIndex();
		auto newGene = std::make_shared<UBSGene<Dim>>(evaluatedUBSGene->getUbsGene()->createNewGeneByInsertingSegmentPoint(relativeMaxSegmentErrorIndex, false));
		auto newEvaluatedGene = std::make_shared<EvaluatedUBSGene<Dim>>(newGene);
		if (nrApproximations) *nrApproximations += 1;
		return newEvaluatedGene;
	}

	// iteratively insert a control point in the middle of the segment with the largest relative error (summed error divided by chordal length of the segment) and optimize it afterwards by moving control points
	template <size_t Dim>
	std::shared_ptr<EvaluatedUBSGene<Dim>> insertSegmentsAtRelativeMaxSegmentErrorAndOptimize(
		const std::shared_ptr<EvaluatedUBSGene<Dim>> evaluatedUBSGene,
		const size_t iterations,
		size_t* nrApproximations = nullptr)
	{
		std::shared_ptr<EvaluatedUBSGene<Dim>> result = evaluatedUBSGene;
		for (size_t i = 0; i < iterations; ++i)
		{
			result = IterativeApproximation::optimizeByJiggling(result, nrApproximations);
			result = IterativeApproximation::insertSegmentAtRelativeMaxSegmentError(result, nrApproximations);
		}
		result = IterativeApproximation::optimizeByJiggling(result, nrApproximations);
		return result;
	}
};

#endif
