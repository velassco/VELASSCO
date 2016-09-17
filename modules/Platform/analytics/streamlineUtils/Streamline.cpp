#include "Streamline.h"

Streamline::Streamline(glm::dvec3 p)
{
  addFrontPoint(p);
}

Streamline::~Streamline()
{
}

void Streamline::addFrontPoint(glm::dvec3 p)
{
  m_points.push_back(p);
  m_results.push_back(glm::dvec3(0.0));
}

void Streamline::addFrontPoint(glm::dvec3 p, glm::dvec3 r)
{
  m_points.push_back(p);
  m_results.push_back(r);
}

glm::dvec3 Streamline::getFrontPoint() const
{
  return m_points.back();
}

glm::i64 Streamline::getNumberOfIntegratedSteps() const
{
  return static_cast<glm::i64>(m_points.size() - 1);
}
