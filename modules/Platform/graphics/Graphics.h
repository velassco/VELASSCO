/* -*- c++ -*- */
#ifndef _GRAPHICS_MODULE_H_
#define _GRAPHICS_MODULE_H_

#include <iostream>
#include <string>
#include <vector>

// may be i don't need this:
#include "VELaSSCoSM.h"
using namespace VELaSSCoSM;

#include "RealTimeFormat.h"

#include "GraphicsElementTypes.h"

// handles connection to Storage Module:
class GraphicsModule
{
public:
  static GraphicsModule *getInstance();

  // methods:
  void fromatMeshForDrawing(VELaSSCo::RTFormat::File& _return_, const MeshInfo& meshInfo, const std::vector<std::vector<Vertex>>& vertices, const std::vector<Element>& elements, const std::vector<ElementAttrib>& elementAttribs, const std::vector<ElementGroup>& elementGroupInfos) ;

private:
  GraphicsModule() {};
  GraphicsModule( GraphicsModule const&) {};
  GraphicsModule &operator=( GraphicsModule const&) { return *this;};
  
  void fromatElementaryMeshForDrawing(VELaSSCo::RTFormat::File& _return_, const MeshInfo& meshInfo, const std::vector<std::vector<Vertex>>& vertices, const std::vector<Element>& elements, const std::vector<ElementAttrib>& elementAttribs) ;
  void fromatTetrahedraMeshForDrawing(VELaSSCo::RTFormat::File& _return_, const MeshInfo& meshInfo, const std::vector<Vertex>& vertices, const std::vector<Element>& elements, const std::vector<ElementAttrib>& elementAttribs) ;
  void fromatSphereMeshForDrawing(VELaSSCo::RTFormat::File& _return_, const MeshInfo& meshInfo, const std::vector<Vertex>& vertices, const std::vector<Element>& elements, const std::vector<ElementAttrib>& elementAttribs) ;
    
private:
  static GraphicsModule *m_pInstance;
};

#endif // _GRAPHICS_MODULE_H_
