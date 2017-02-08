#ifndef __STREAMLINE_H__
#define __STREAMLINE_H__

#include "glm/glm.hpp"

#include <vector>

class Streamline {
public:
  Streamline(glm::dvec3 p);
  ~Streamline();

  void addFrontPoint(glm::dvec3 p);
  void addFrontPoint(glm::dvec3 p, glm::dvec3 r);
  glm::dvec3 getFrontPoint() const;

  glm::i64 getNumberOfIntegratedSteps() const;

  const std::vector<glm::dvec3>& points() const {
    return m_points;
  }

  const std::vector<glm::dvec3>& results() const {
    return m_results;
  }

private:
  std::vector<glm::dvec3> m_points;
  std::vector<glm::dvec3> m_results;

  bool initialized;
};

#endif
