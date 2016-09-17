
/**
 *
 * Stream flow tracing library
 *
 * This class generates stream visualizations from vector fields.
 *
 */

#ifndef __STREAM_TRACER__
#define __STREAM_TRACER__

// GLM
#include "glm/glm.hpp"
#include "glm/gtx/quaternion.hpp"

// STD
#include <vector>

#include "AABB.h"
#include "UnstructDataset.h"
#include "Streamline.h"

enum TraceDirection { TD_FORWARD, TD_BACKWARD, TD_BOTH };
enum IntegrationMethod { IM_EULER, IM_CASHKARP };

struct SurfaceParameters
  {
    // Tracing parameters
    TraceDirection      traceDirection;
    IntegrationMethod   traceIntegrationMethod;
    double               traceStepSize;
    unsigned int        traceMaxSteps;
    unsigned int        traceSpecialBackStepsLimit;

		double traceRefineConst;
		double traceDiscountConst;

    bool doStepAdaptation;

    bool operator==(const SurfaceParameters& rval){
      if (traceDirection              == rval.traceDirection              &&
          traceIntegrationMethod      == rval.traceIntegrationMethod      &&
          traceStepSize               == rval.traceStepSize               &&
          traceMaxSteps               == rval.traceMaxSteps	              &&
			    traceRefineConst	          == rval.traceRefineConst			      &&
			    traceDiscountConst	        == rval.traceDiscountConst          &&
          doStepAdaptation            == rval.doStepAdaptation            &&
          traceSpecialBackStepsLimit  == rval.traceSpecialBackStepsLimit){
        return true;
      }

      return false;
    }
  };

struct StreamerUtilVariables
{
	glm::dvec3 previousPoint;
	glm::dvec3 currentPoint;
	glm::dvec3 currentDerivate;
	glm::dvec3 nextPoint;

	double stepSize;
	int   stepSizeAdapted;
};

class StreamTracer
{
public:

  StreamTracer();
  virtual ~StreamTracer();

  void getParameters(SurfaceParameters &parameters);
  void setParameters(const SurfaceParameters &parameters);

  unsigned int get_n_computed_stream_lines()
  { return m_n_computed_lines;    }

  double get_last_lines_per_second()
  { return m_last_lines_per_second;    }

  unsigned int get_sum_n_computed_stream_lines()
  {	return m_sum_n_computed_lines;	}

  double get_sum_n_lines_per_second()
  {	return m_sum_lines_per_second;	}

  void clear_statistics(){
  m_sum_n_computed_lines = 0u;
  m_sum_lines_per_second = 0.0f;
  }

  void traceStreamline(UnstructDataset* dataset, std::vector<Streamline>& streamlines, double stepsize);
  void traceStreamline(UnstructDataset* dataset, glm::dvec3 seed, double stepsize, std::vector<glm::dvec3>& vertices, std::vector<glm::dvec3>& derivs, std::vector < glm::i64 > &streamLines);

private:
  glm::dvec3 step(UnstructDataset* dataset, const glm::dvec3& p0, const glm::dvec3& deriv, double& stepsize, int& stepsizeAdapted);

  SurfaceParameters m_surface_parameters;
    
  unsigned int m_n_computed_lines;
  double m_last_lines_per_second;

	unsigned int m_sum_n_computed_lines;
	double m_sum_lines_per_second;
};


#endif
