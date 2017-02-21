// ==========================================================================
//
// Fraunhofer-Institut f�r Graphische Datenverarbeitung IGD
// Fraunhoferstr. 5 | 64283 Darmstadt | Germany
// Telefon +49 6151 155-663 | Fax +49 6151 155-139
//
// ==========================================================================

#include <Eigen/Core>
#include "bSplineApproxUtils/eigen/unsupported/Eigen/Splines"

#include <iostream>
#include <fstream>
#include <iomanip>

#include "bSplineApproxUtils/discretecurve.h"
#include "bSplineApproxUtils/iterativeapproximation.h"


#define DEBUG_SPLINE

class BSplineApprox
{
public:
	BSplineApprox(const std::vector<Eigen::Vector3d>& points)
	{
		m_points = points;
	}

	~BSplineApprox(){};


	std::vector<Eigen::Vector3d> getBSplineControlPoints(IterativeApproximation::ApproximationMode mode = IterativeApproximation::ApproximationMode::UNIFORM)
	{
		std::vector<Eigen::Vector3d> result;
		
		std::size_t numControlPoints = estimateNumControlPoints(m_points);
		auto currCurve = std::make_shared<DiscreteCurve<3>>(m_points);
		
		std::size_t numApproximations = 0;
		auto evaluatedGene = IterativeApproximation::iterativeApproximation(currCurve, numControlPoints, &numApproximations, IterativeApproximation::ApproximationMode::UNIFORM);

		//std::cout << "Optimized approximations: " << numApproximations << std::endl;
		
		return evaluatedGene->getUbs()->getControlPoints();
		
		// =============== Eigen B-Spline Method ========== // 

		//Eigen::MatrixXd pts(3, m_points.size());

		//std::cout << "=========== Input values ===========" << std::endl;

		//for (int i = 0; i < m_points.size(); i++)
		//{
		//	pts.col(i) << m_points[i](0), m_points[i](0), m_points[i](0);
		//	std::cout << "[" << i << "] " << "( " << m_points.at(i)(0, 0) << ", " << std::setprecision(2) <<
		//		m_points.at(i)(1, 0) << ", " << m_points.at(i)(2, 0) << " )" << std::endl;

		//}

		//auto m_spline = Eigen::SplineFitting<Eigen::Spline3d>::Interpolate(pts, 1);
		//auto ctrl = m_spline.ctrls();

		//for(int i = 0; i < ctrl.cols(); i++)
		//{
		//	Eigen::Vector3d v(ctrl(0, i), ctrl(1, i), ctrl(2, i));
		//	result.push_back(v);
		//	
		//}

		//std::cout << "=========== Calculated/approx values ===========" << std::endl;

		//std::ofstream myfile;
		//myfile.open("approx.csv");

		//for (const auto& cpnt : result) {
		//	myfile << cpnt << std::endl;
		//	std::cout << cpnt.x() << " " << cpnt.y() << " " << cpnt.z() << std::endl;
		//}

		////double delta = 1.0 / 60.0;
		////for (auto i = 0.0; i <= 1.0; i += delta)
		////{
		////	auto pnt = m_spline(i);
		////	std::vector<float> v;
		////	v.push_back(pnt(0, 0));
		////	v.push_back(pnt(1, 0));
		////	v.push_back(pnt(2, 0));
		////	
		////	//std::cout << "Vector for: i = " << i << " - [" << v.at(0) << ", " << v.at(1) << ", " << v.at(2) << "]" << std::endl;
		////	myfile << v.at(0) << ", " << v.at(1) << ", " << v.at(2) << "\n";
		////}

		//myfile.close();
	}

	size_t estimateNumControlPoints(const std::vector<Eigen::Vector3d> & discreteCurve)
	{
		return discreteCurve.size() == 2 ? 2 : std::max<std::size_t>(3, discreteCurve.size() / 10); 
	}

private:

	// Member
	std::vector<Eigen::Vector3d> m_points;
};