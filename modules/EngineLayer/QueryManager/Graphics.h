/* -*- c++ -*- */
#ifndef _GRAPHICS_MODULE_H_
#define _GRAPHICS_MODULE_H_

#include <iostream>
#include <string>

// may be i don't need this:
#include "VELaSSCoSM.h"
using namespace VELaSSCoSM;

// handles connection to Storage Module:
class GraphicsModule
{
public:
  static GraphicsModule *getInstance();

  // methods:

private:
  GraphicsModule() {};
  GraphicsModule( GraphicsModule const&) {};
  GraphicsModule &operator=( GraphicsModule const&) { return *this;};
    
private:
  static GraphicsModule *m_pInstance;
};

#endif // _GRAPHICS_MODULE_H_
