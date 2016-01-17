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
      std::vector<uint8_t> description;

      // additional data
      std::vector<uint8_t> meta;

      // vertex block
      std::vector<uint8_t> vertexDefinitions;
      std::vector<uint8_t> vertexAttributes;

      // edge block
      std::vector<uint8_t> edgeDefinitions;
      std::vector<uint8_t> edgeAttributes;

      // face block
      std::vector<uint8_t> faceDefinitions;
      std::vector<uint8_t> faceAttributes;

      // cell block
      std::vector<uint8_t> cellDefinitions;
      std::vector<uint8_t> cellAttributes;
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

inline std::istream& operator>>(std::istream& is, VELaSSCo::RTFormat::File& file){
  is.read((char*)(&file.header),                  sizeof(VELaSSCo::RTFormat::Header));

  if( file.header.descriptionBytes       > 0 )  {  file.data.description.resize( file.header.descriptionBytes);        		 is.read((char*)(file.data.description.data()),        file.header.descriptionBytes);        }
  if( file.header.vertexDefinitionsBytes > 0 )  {  file.data.vertexDefinitions.resize( file.header.vertexDefinitionsBytes);  is.read((char*)(file.data.vertexDefinitions.data()),  file.header.vertexDefinitionsBytes);  }
  if( file.header.vertexAttributesBytes  > 0 )  {  file.data.vertexAttributes.resize(  file.header.vertexAttributesBytes);   is.read((char*)(file.data.vertexAttributes.data()),   file.header.vertexAttributesBytes);   } 
  if( file.header.edgeDefinitionsBytes   > 0 )  {  file.data.edgeDefinitions.resize(   file.header.edgeDefinitionsBytes);    is.read((char*)(file.data.edgeDefinitions.data()),    file.header.edgeDefinitionsBytes);    }
  if( file.header.edgeAttributesBytes    > 0 )  {  file.data.edgeAttributes.resize(    file.header.edgeAttributesBytes);     is.read((char*)(file.data.edgeAttributes.data()),     file.header.edgeAttributesBytes);     }
  if( file.header.faceDefinitionsBytes   > 0 )  {  file.data.faceDefinitions.resize(   file.header.faceDefinitionsBytes);    is.read((char*)(file.data.faceDefinitions.data()),    file.header.faceDefinitionsBytes);    }
  if( file.header.faceAttributesBytes    > 0 )  {  file.data.faceAttributes.resize(    file.header.faceAttributesBytes);     is.read((char*)(file.data.faceAttributes.data()),     file.header.faceAttributesBytes);     } 
  if( file.header.cellDefinitionsBytes   > 0 )  {  file.data.cellDefinitions.resize(   file.header.cellDefinitionsBytes);    is.read((char*)(file.data.cellDefinitions.data()),    file.header.cellDefinitionsBytes);    }
  if( file.header.cellAttributesBytes    > 0 )  {  file.data.cellAttributes.resize(    file.header.cellAttributesBytes);     is.read((char*)(file.data.cellAttributes.data()),     file.header.cellAttributesBytes);     }

  return is;
}

inline std::ostream& operator<<(std::ostream& os, const VELaSSCo::RTFormat::File& file){
  std::cout << "=====> hi";
  os.write((char*)(&file.header),                  sizeof(VELaSSCo::RTFormat::Header));
  if( file.header.descriptionBytes       > 0 )    os.write((char*)(file.data.description.data()),        file.header.descriptionBytes);
  if( file.header.vertexDefinitionsBytes > 0 )    os.write((char*)(file.data.vertexDefinitions.data()),  file.header.vertexDefinitionsBytes);
  if( file.header.vertexAttributesBytes  > 0 )    os.write((char*)(file.data.vertexAttributes.data()),   file.header.vertexAttributesBytes);
  if( file.header.edgeDefinitionsBytes   > 0 )    os.write((char*)(file.data.edgeDefinitions.data()),    file.header.edgeDefinitionsBytes);
  if( file.header.edgeAttributesBytes    > 0 )    os.write((char*)(file.data.edgeAttributes.data()),     file.header.edgeAttributesBytes);
  if( file.header.faceDefinitionsBytes   > 0 )    os.write((char*)(file.data.faceDefinitions.data()),    file.header.faceDefinitionsBytes);
  if( file.header.faceAttributesBytes    > 0 )    os.write((char*)(file.data.faceAttributes.data()),     file.header.faceAttributesBytes);
  if( file.header.cellDefinitionsBytes   > 0 )    os.write((char*)(file.data.cellDefinitions.data()),    file.header.cellDefinitionsBytes);
  if( file.header.cellAttributesBytes    > 0 )    os.write((char*)(file.data.cellAttributes.data()),     file.header.cellAttributesBytes);
  std::cout << "=====> bye";
  return os;
}
