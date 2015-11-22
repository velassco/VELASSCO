//
// VELaSSCo real-time file format draft
//

// ----------------------------------------------------------------------------

#pragma once

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

static std::istream& operator>>(std::istream& is, VELaSSCo::RTFormat::File& file){
  is.read((char*)(&file.header),                  sizeof(VELaSSCo::RTFormat::Header));

  if( file.header.descriptionBytes       > 0 )  {  file.data.description        = new uint8_t[file.header.descriptionBytes];        is.read((char*)(file.data.description),        file.header.descriptionBytes);        }
  if( file.header.vertexDefinitionsBytes > 0 )  {  file.data.vertexDefinitions  = new uint8_t[file.header.vertexDefinitionsBytes];  is.read((char*)(file.data.vertexDefinitions),  file.header.vertexDefinitionsBytes);  }
  if( file.header.vertexAttributesBytes  > 0 )  {  file.data.vertexAttributes   = new uint8_t[file.header.vertexAttributesBytes];   is.read((char*)(file.data.vertexAttributes),   file.header.vertexAttributesBytes);   } 
  if( file.header.edgeDefinitionsBytes   > 0 )  {  file.data.edgeDefinitions    = new uint8_t[file.header.edgeDefinitionsBytes];    is.read((char*)(file.data.edgeDefinitions),    file.header.edgeDefinitionsBytes);    }
  if( file.header.edgeAttributesBytes    > 0 )  {  file.data.edgeAttributes     = new uint8_t[file.header.edgeAttributesBytes];     is.read((char*)(file.data.edgeAttributes),     file.header.edgeAttributesBytes);     }
  if( file.header.faceDefinitionsBytes   > 0 )  {  file.data.faceDefinitions    = new uint8_t[file.header.faceDefinitionsBytes];    is.read((char*)(file.data.faceDefinitions),    file.header.faceDefinitionsBytes);    }
  if( file.header.faceAttributesBytes    > 0 )  {  file.data.faceAttributes     = new uint8_t[file.header.faceAttributesBytes];     is.read((char*)(file.data.faceAttributes),     file.header.faceAttributesBytes);     } 
  if( file.header.cellDefinitionsBytes   > 0 )  {  file.data.cellDefinitions    = new uint8_t[file.header.cellDefinitionsBytes];    is.read((char*)(file.data.cellDefinitions),    file.header.cellDefinitionsBytes);    }
  if( file.header.cellAttributesBytes    > 0 )  {  file.data.cellAttributes     = new uint8_t[file.header.cellAttributesBytes];     is.read((char*)(file.data.cellAttributes),     file.header.cellAttributesBytes);     }

  return is;
}

static std::ostream& operator<<(std::ostream& os, const VELaSSCo::RTFormat::File& file){
  os.write((char*)(&file.header),                  sizeof(VELaSSCo::RTFormat::Header));

  if( file.header.descriptionBytes       > 0 )    os.write((char*)(file.data.description),        file.header.descriptionBytes);
  if( file.header.vertexDefinitionsBytes > 0 )    os.write((char*)(file.data.vertexDefinitions),  file.header.vertexDefinitionsBytes);
  if( file.header.vertexAttributesBytes  > 0 )    os.write((char*)(file.data.vertexAttributes),   file.header.vertexAttributesBytes);
  if( file.header.edgeDefinitionsBytes   > 0 )    os.write((char*)(file.data.edgeDefinitions),    file.header.edgeDefinitionsBytes);
  if( file.header.edgeAttributesBytes    > 0 )    os.write((char*)(file.data.edgeAttributes),     file.header.edgeAttributesBytes);
  if( file.header.faceDefinitionsBytes   > 0 )    os.write((char*)(file.data.faceDefinitions),    file.header.faceDefinitionsBytes);
  if( file.header.faceAttributesBytes    > 0 )    os.write((char*)(file.data.faceAttributes),     file.header.faceAttributesBytes);
  if( file.header.cellDefinitionsBytes   > 0 )    os.write((char*)(file.data.cellDefinitions),    file.header.cellDefinitionsBytes);
  if( file.header.cellAttributesBytes    > 0 )    os.write((char*)(file.data.cellAttributes),     file.header.cellAttributesBytes);

  return os;
}