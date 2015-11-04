#include <sstream>
#include <stddef.h>  // defines NULL
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#include "Helpers.h"
#include "Graphics.h"

// Global static pointer used to ensure a single instance of the class.
GraphicsModule *GraphicsModule::m_pInstance = NULL;

GraphicsModule *GraphicsModule::getInstance() {
  // Only allow one instance of class to be generated.
  std::cout << "in GraphicsModule::getInstance" << std::endl;
  if ( !m_pInstance) {
    m_pInstance = new GraphicsModule;
  }
  return m_pInstance;
}
