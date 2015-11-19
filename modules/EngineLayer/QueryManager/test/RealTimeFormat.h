
//
// VELaSSCo real-time file format draft
//

// ----------------------------------------------------------------------------

#include <stdint.h>

#include <iostream>

namespace VELaSSCo
{

  namespace RTFormat
  {

    // ----------------------------------------------------------------------------

    struct Header
    {
      // identification as VELaSSCo data
      uint8_t  magic[8];                // magic number ("VELaSSCo", UTF-8)
      uint32_t version;                 // version number (100 = V1.00)

      // information about the content
      uint64_t descriptionBytes;       // size of description block in bytes
      uint64_t metaBytes;              // size of meta block in bytes

      // sizes of the contained buffers
      uint64_t vertexDefinitionsBytes; // size of vertex definition buffer in bytes
      uint64_t vertexAttributesBytes;  // size of vertex attribute buffer in bytes
      uint64_t edgeDefinitionsBytes;   // size of edge definition buffer in bytes
      uint64_t edgeAttributesBytes;    // size of edge attribute buffer in bytes
      uint64_t faceDefinitionsBytes;   // size of face definition buffer in bytes
      uint64_t faceAttributesBytes;    // size of face attribute buffer in bytes
      uint64_t cellDefinitionsBytes;   // size of cell definition buffer in bytes
      uint64_t cellAttributesBytes;    // size of cell attribute buffer in bytes
    };

    // ----------------------------------------------------------------------------

    struct Data
    {
      // description of file content
      uint8_t* description;

      // additional data
      uint8_t* meta;

      // vertex block
      uint8_t* vertexDefinitions;
      uint8_t* vertexAttributes;

      // edge block
      uint8_t* edgeDefinitions;
      uint8_t* edgeAttributes;

      // face block
      uint8_t* faceDefinitions;
      uint8_t* faceAttributes;

      // cell block
      uint8_t* cellDefinitions;
      uint8_t* cellAttributes;
    };

    // ----------------------------------------------------------------------------

    struct File
    {
      // file information
      Header header;

      // actual data;
      Data data;
    };
    // ----------------------------------------------------------------------------

  }

}