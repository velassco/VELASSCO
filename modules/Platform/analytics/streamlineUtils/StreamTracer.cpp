#include "StreamTracer.h"

// STD
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

// OpenMP
#include <omp.h>

// RPE
#include "AABB.h"
#include "Grid.h"

// #define STREAM_TRACER_USE_CELL_LIST // Test all primitives in an acceleration cell
//#define STREAM_TRACER_USE_OMP       // Use OpenMP multi-threading

StreamTracer::StreamTracer()
{
  // Default surface tracing parameters
  m_surface_parameters.traceDirection = TD_BOTH;
  m_surface_parameters.traceStepSize  = 0.01f;
  m_surface_parameters.traceMaxSteps  = 100;
	m_surface_parameters.traceRefineConst   = 0.1f;
	m_surface_parameters.traceDiscountConst = 1 / 80.0f * m_surface_parameters.traceRefineConst;
  m_surface_parameters.doStepAdaptation = true;
  m_surface_parameters.traceSpecialBackStepsLimit = 3;
  m_surface_parameters.traceIntegrationMethod = IM_CASHKARP;

    /*m_surface_parameters.seedingLineCenter = glm::dvec3(0.0f, 0.0f, 0.0f);
    m_surface_parameters.seedingLineDirection = glm::dvec3(0.0f, 0.0f, 1.0f);*/ 
}

StreamTracer::~StreamTracer()
{
}

//#define SNAP_TO_MESH_SURFACE
//#define CHOOSE_INITIAL_STEP_SIZE
//#define STEP_ADAPTATION
#define SPECIAL_BACK_STEPS
#define RUNGE_KUTTA_INTEGERATION

void StreamTracer::traceStreamline(UnstructDataset* dataset, std::vector<Streamline>& streamlines, double stepsize) {
  
  for (size_t i = 0; i < streamlines.size(); i++) {
    if(i % 100 == 0) std::cout << "Streamline " << i << " from " << streamlines.size() << std::endl;
    if (streamlines[i].getNumberOfIntegratedSteps() >= m_surface_parameters.traceMaxSteps) {
      continue;
    }

    glm::dvec3 seed = streamlines[i].getFrontPoint();

    //std::cout << "seed = " << seed.x << " " << seed.y << " " << seed.z << std::endl;

    std::vector<glm::dvec3> vertices, results;
    std::vector<glm::i64>   streamlines_indices;

    traceStreamline(dataset, seed, stepsize, vertices, results, streamlines_indices);

    //std::cout << "vertices size = " << vertices.size() << std::endl;
    //std::cout << "results  size = " << results.size() << std::endl;

    for (size_t j = 0; j < vertices.size(); j++) {
      streamlines[i].addFrontPoint(vertices[j], results[j]);
      //std::cout << vertices[j].x << " " << vertices[j].y << vertices[j].z << std::endl;
      //std::cout << results[j].x << " " << results[j].y << results[j].z << std::endl;
    }
  }
}

void StreamTracer::traceStreamline(UnstructDataset* dataset, glm::dvec3 seed, double stepsize, std::vector<glm::dvec3>& vertices, std::vector<glm::dvec3>& derivs, std::vector< glm::i64 > &streamLine){

  StreamerUtilVariables utilVars;

  utilVars.previousPoint    = seed;
  utilVars.currentPoint     = seed;
  utilVars.stepSize         = stepsize;
  utilVars.stepSizeAdapted  = 0;

#ifndef STUPID_TRACING
	// snap the seed point to mesh surface
  //if(m_surface_parameters.doInCellInterpolation){
#ifdef SNAP_TO_MESH_SURFACE
  dataset->snapToMeshSurface(utilVars.currentPoint);
  utilVars.previousPoint = utilVars.currentPoint;
#endif // SNAP_TO_MESH_SURFACE
  
	//}

	// Step Size Selection
	//if(m_parameters.doStepAdaptation)
#ifdef STEP_ADAPTATION
#ifdef CHOOSE_INITIAL_STEP_SIZE
	{
		if(utilVars.stepSize > 0)
      utilVars.stepSize = dataset->chooseInitialStepSize(utilVars.currentPoint);
		else
			utilVars.stepSize = -dataset->chooseInitialStepSize(utilVars.currentPoint);
	}
#endif // CHOOSE_INITIAL_STEP_SIZE
#endif // STEP_ADAPTATION

#endif // STUPID_TRACING
   
  unsigned int s = 0;
  unsigned int specialBackSteps = m_surface_parameters.traceSpecialBackStepsLimit;
  while(s < m_surface_parameters.traceMaxSteps)
  {
#ifndef STUPID_TRACING
#ifdef SPECIAL_BACK_STEPS
    // if the line has left the vector field
    if (!dataset->contains(utilVars.currentPoint)){
      if(specialBackSteps > 0 && !streamLine.empty()){
        --specialBackSteps;

        utilVars.currentPoint = vertices[streamLine.back()];
        streamLine.pop_back();
        vertices.pop_back();
        derivs.pop_back();

        if(streamLine.empty()){
          utilVars.previousPoint = utilVars.currentPoint;
        } else {
          utilVars.previousPoint = vertices[streamLine.back()];
        }

        utilVars.stepSize *= 0.33f;
        continue;
      }
      else
        break;
    }
#endif // SPECIAL_BACK_STEPS
#endif // STUPID_TRACING

    utilVars.currentDerivate = dataset->derivate(utilVars.currentPoint);			// Derivate at point s0
#ifndef STUPID_TRACING
#ifdef SPECIAL_BACK_STEPS
    if (glm::length(utilVars.currentDerivate) < 0){
      if(specialBackSteps > 0 && !streamLine.empty()){
        --specialBackSteps;

        utilVars.currentPoint = vertices[streamLine.back()];
        streamLine.pop_back();
        vertices.pop_back();
        derivs.pop_back();

        if(streamLine.empty()){
          utilVars.previousPoint = utilVars.currentPoint;
        } else {
          utilVars.previousPoint = vertices[streamLine.back()];
        }

        utilVars.stepSize *= 0.33f;
        continue;
      }
      else
        break;
    }
#endif // SPECIAL_BACK_STEPS
#endif // STUPID_TRACING

#ifdef STUPID_TRACING
    utilVars.nextPoint = utilVars.currentPoint + utilVars.stepSize * utilVars.currentDerivate;
#else

    utilVars.nextPoint = step(dataset, utilVars.currentPoint, utilVars.currentDerivate, utilVars.stepSize, utilVars.stepSizeAdapted);

    //std::cout << utilVars.nextPoint.x << " " << utilVars.nextPoint.y << " " << utilVars.nextPoint.z << std::endl;

#ifdef SPECIAL_BACK_STEPS
	  // if the point was not changed.
    if (utilVars.nextPoint == utilVars.currentPoint){
      if(specialBackSteps > 0 && !streamLine.empty()){
        --specialBackSteps;

        utilVars.currentPoint = vertices[streamLine.back()];
        streamLine.pop_back();
        vertices.pop_back();
        derivs.pop_back();

        if(streamLine.empty()){
          utilVars.previousPoint = utilVars.currentPoint;
        } else {
          utilVars.previousPoint = vertices[streamLine.back()];
        }

        utilVars.stepSize *= 0.33f;
        continue;
      }
      else
        break;
    }
#endif

	  
#ifdef STEP_ADAPTATION
    if (m_surface_parameters.doStepAdaptation && utilVars.stepSizeAdapted == -1) // stepsize was decreased by step()
    {
      // instead of using nextPoint, we choose the currentPoint + suggested stepsize * currentDerivate
	  	// as nextPoint
      utilVars.nextPoint = utilVars.currentPoint + utilVars.currentDerivate * utilVars.stepSize;
	  }
#endif // STEP_ADAPTATION
#endif // STUPID_TRACING
    vertices.push_back(utilVars.currentPoint);
    derivs.push_back(utilVars.currentDerivate);

    

    streamLine.push_back( static_cast<unsigned int>( vertices.size() - 1 ) );

    //std::cout << vertices.size() << " " << utilVars.currentPoint.x << " " << utilVars.currentPoint.y << " " << utilVars.currentPoint.z << std::endl;
    //if(vertices.size() > 1000) return;

    utilVars.previousPoint = utilVars.currentPoint;
    utilVars.currentPoint = utilVars.nextPoint;

    s++;
  } 

  //std::cout << "Number of verts  = " << vertices.size() << std::endl;
  //std::cout << "Number of derivs = " << derivs.size() << std::endl;

}

// butcher tableau for cash karp
const double bt_ck_A[8][8] =
{
    { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f },
    { 1.0f / 5.0f, 1.0f / 5.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f },
    { 3.0f / 10.0f, 3.0f / 40.0f, 9.0f / 40.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f },
    { 3.0f / 5.0f, 3.0f / 10.0f, -9.0f / 10.0f, 6.0f / 5.0f, 0.0f, 0.0f, 0.0f, 0.0f },
    { 1.0f, -11.0f / 54.0f, 5.0f / 2.0f, -70.0f / 27.0f, 35.0f / 27.0f, 0.0f, 0.0f, 0.0f },
    { 7.0f / 8.0f, 1631.0f / 55296.0f, 175.0f / 512.0f, 575.0f / 13824.0f, 44275.0f / 110592.0f, 253.0f / 4096.0f, 0.0f, 0.0f },
    { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f },
    { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f }
};
const double bt_ck_B[2][8] =
{
    { 37.0f / 378.0f, 0.0f, 250.0f / 621.0f, 125.0f / 594.0f, 0.0f, 512.0f / 1771.0f, 0.0f, 0.0f }, // b row #1: fifth order solution
    { 2825.0f / 27648.0f, 0.0f, 18575.0f / 48384.0f, 13525.0f / 55296.0f, 277.0f / 14336.0f, 1.0f / 4.0f, 0.0f, 0.0f }  // b row #2: fourth order solution
};
glm::dvec3 StreamTracer::step(UnstructDataset* dataset, const glm::dvec3& p0, const glm::dvec3& deriv, double& stepsize, int& stepsizeAdapted)
{
  glm::dvec3 p1;

  if (m_surface_parameters.traceIntegrationMethod == IM_EULER)
  {
    p1 = p0 + stepsize * deriv;
	  return p1;
  }
  else if (m_surface_parameters.traceIntegrationMethod == IM_CASHKARP)
  {
    stepsizeAdapted = 0;

    glm::dvec3 q[8];
    q[0] = stepsize * deriv;
    q[1] = stepsize * dataset->derivate(p0 + bt_ck_A[1][1] * q[0]);
    q[2] = stepsize * dataset->derivate(p0 + bt_ck_A[2][1] * q[0] + bt_ck_A[2][2] * q[1]);
    q[3] = stepsize * dataset->derivate(p0 + bt_ck_A[3][1] * q[0] + bt_ck_A[3][2] * q[1] + bt_ck_A[3][3] * q[2]);
    q[4] = stepsize * dataset->derivate(p0 + bt_ck_A[4][1] * q[0] + bt_ck_A[4][2] * q[1] + bt_ck_A[4][3] * q[2] + bt_ck_A[4][4] * q[3]);
    q[5] = stepsize * dataset->derivate(p0 + bt_ck_A[5][1] * q[0] + bt_ck_A[5][2] * q[1] + bt_ck_A[5][3] * q[2] + bt_ck_A[5][4] * q[3] + bt_ck_A[5][5] * q[4]);

    // solution for the integration
    q[6] = bt_ck_B[1][0] * q[0] + /* skip bt_ck_B[1][1] */ +bt_ck_B[1][2] * q[2] + bt_ck_B[1][3] * q[3] + bt_ck_B[1][4] * q[4] + bt_ck_B[1][5] * q[5];

    // error estimate
    q[7] = bt_ck_B[0][0] * q[0] + /* skip bt_ck_B[0][1] */ +bt_ck_B[0][2] * q[2] + bt_ck_B[0][3] * q[3] + /* skip bt_ck_B[0][4] */ +bt_ck_B[0][5] * q[5];

    p1 = p0 + q[6];

#ifdef STEP_ADAPTATION
    const double epsilon = glm::length(q[6] - q[7]);
    const double eps_range = dataset->getMinExtent();

    if (m_surface_parameters.doStepAdaptation && epsilon <= (eps_range * 1e-12f/*adaptionMinLimit.getValue()*/))
    {
        stepsize *= 2.0f;
        stepsizeAdapted = +1;
    }
    else if (m_surface_parameters.doStepAdaptation && epsilon >= (eps_range * 1e-9f/*adaptionMaxLimit.getValue()*/))
    {
        stepsize *= 0.5f;
        stepsizeAdapted = -1;
    }
#endif
  }

  return p1;
}

void StreamTracer::getParameters( SurfaceParameters &parameters )
{
  parameters = m_surface_parameters;
}

void StreamTracer::setParameters( const SurfaceParameters &paramters )
{
  m_surface_parameters = paramters;
}
