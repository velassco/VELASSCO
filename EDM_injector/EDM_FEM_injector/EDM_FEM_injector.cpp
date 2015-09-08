// EDM_FEM_injector.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"

#include "FEM_Injector.h"
//#include <iostream>
//#include <boost/filesystem.hpp>
//#include <boost/algorithm/string.hpp>
//#include <vector>
//#include "boost/program_options.hpp" 
//
//#include "..\..\ThriftInjector\Inject\Logger.h"
//
//#include <boost/lexical_cast.hpp>
//#include <boost/uuid/uuid_io.hpp>
//#include <boost/uuid/nil_generator.hpp>
//#include <thrift/protocol/TBinaryProtocol.h>
//#include <thrift/transport/TSocket.h>
//#include <thrift/transport/TTransportUtils.h>
//
//#include "..\..\ThriftInjector\Inject\ParseAsciiMesh.h"
//#include "..\..\ThriftInjector\Inject\ParseAsciiResult.h"
//#include "..\..\ThriftInjector\Inject\BinarySerialization.h"
//
//using namespace boost::filesystem;
//using boost::lexical_cast;
//using boost::bad_lexical_cast;
//
//using namespace apache::thrift;
//using namespace apache::thrift::protocol;
//using namespace apache::thrift::transport;
//
////using namespace apache::hadoop::hbase::thrift;
//
//typedef std::vector<std::string> StrVec;
//typedef std::map<std::string, std::string> StrMap;
////typedef std::vector<ColumnDescriptor> ColVec;
////typedef std::map<std::string,ColumnDescriptor> ColMap;
////typedef std::vector<TCell> CellVec;
////typedef std::map<std::string,TCell> CellMap;
//
//typedef std::map<int, path> PathContainerType;
//
//
//#define SUCCESS 0
//#define ERROR_IN_COMMAND_LINE 1
//

struct GlobalMeshHeaderType
{
   GID::ElementType elementType;
   GID::UInt32 sizeElement;
   GID::IdNode numberOfNodes;
   GID::IdElement numberOfElements;
   std::string color;
   GID::UInt32 indexElementSet;

   GlobalMeshHeaderType()
   {
      this->elementType = GID::TETRAHEDRA;
      this->sizeElement = 0;
      this->numberOfNodes = 0;
      this->numberOfElements = 0;
      this->indexElementSet = 0;
   }

   int InitFromPartition(const GID::MeshResultType &meshPart,
      GID::UInt32 index)
   {
      if (this->indexElementSet == 0)
      {
         this->indexElementSet = index;
         this->elementType = meshPart.header.elementType;
         this->sizeElement = meshPart.header.numberOfNodes;
      } else
      {
         // TODO: check if meshPart matches first partition header
      }
      this->numberOfNodes += meshPart.nodes.size();
      this->numberOfElements += meshPart.elements.size();
      return SUCCESS;
   }
};

struct GlobalMeshInfoType
{
   typedef std::map<std::string, GlobalMeshHeaderType> MapHeaderType;

   MapHeaderType headers;

   int Update(const GID::MeshResultType &meshPart)
   {
      GlobalMeshHeaderType &meshInfo = this->headers[meshPart.header.name];

      return meshInfo.InitFromPartition(meshPart, this->headers.size());
   }

   GID::UInt32 GetIndexElementSet(const std::string name)
   {
      MapHeaderType::const_iterator it = this->headers.find(name);

      if (it != this->headers.end())
      {
         return it->second.indexElementSet;
      } else
      {
         return 0;
      }
   }
};

struct GlobalResultInfoType
{
   // TODO: collect also GP & RT

   typedef std::map<std::string, GID::ResultHeaderType> MapHeaderType;
   typedef std::map<double, MapHeaderType> MapTimeStepType;
   typedef std::map<std::string, MapTimeStepType> MapAnalysisType;

   MapAnalysisType analysis;

   int Update(GID::ResultHeaderType & h)
   {
      MapTimeStepType &mapT = this->analysis[h.analysis];
      MapHeaderType &headers = mapT[h.step];
      GID::ResultHeaderType &header = headers[h.name];
      if (header.indexMData == 0)
      {
         header = h;
         header.indexMData = headers.size();
      } else
      {
         // TODO: check if resultPart matches first partition header
      }
      h.indexMData = header.indexMData;
      return SUCCESS;
   }

   // TODO: update GP & RT
   int Update(GID::ResultContainerType &resultPart)
   {
      int status = SUCCESS;
      for (size_t i = 0; i < resultPart.results.size(); i++)
      {
         status = this->Update(resultPart.results[i].header);
         if (status != SUCCESS)
         {
            break;
         }
      }
      return status;
   }

   GID::UInt64 GetIndexMetaData(const std::string analysis, double step,
      const std::string &name)
   {
      MapAnalysisType::const_iterator itA = this->analysis.find(analysis);
      if (itA == this->analysis.end())
      {
         return 0;
      }
      MapTimeStepType::const_iterator itT = itA->second.find(step);
      if (itT == itA->second.end())
      {
         return 0;
      }
      MapHeaderType::const_iterator itH = itT->second.find(name);
      if (itH != itT->second.end())
      {
         return itH->second.indexMData;
      } else
      {
         return 0;
      }
   }
};



namespace po = boost::program_options;

static std::string strTableModels("VELaSSCo_Models");
static std::string strTableMetaData("Simulations_Metadata");
static std::string strTableData("Simulations_Data");


struct ModelFileParts
{
   path m_PathRoot;
   path::string_type m_ModelName;
   PathContainerType m_Parts;

   int GetNumberOfParts() const
   {
      return this->m_Parts.size();
   }
};


bool is_number(const path::string_type& s)
{
   char *cp = (char*)s.data();
   if (!*cp) {
      return false;
   } else {
      while (*cp && isdigit(*cp)) cp++;
      return !*cp;
   }
   //std::string::const_iterator it = s.begin();
   //while (it != s.end() && std::isdigit(*it)) ++it;
   //return !s.empty() && it == s.end();
}


int collect_sequence(const path::string_type &one_file, bool isbinary,
   ModelFileParts &res_files, ModelFileParts &msh_files)
{
   path::string_type name(one_file.begin(), one_file.end() - 9);
   //LOG(info) << "colleting sequence for file \"" << name << "\"";
   int count = 0;
   path::string_type::const_reverse_iterator it;
   for (it = name.rbegin();
      it != name.rend() && isdigit(*it); ++it)
   {
      ++count;
   }
   if (it == name.rend() && *it != '-' && *it != '_')
   {
      //LOG(error) << "the name \"" << name << "\" does not match a sequence file";
      return -1;
   }
   path::string_type prefix(name.begin(),
      name.begin() + name.length() - count);
   //LOG(info) << "using sequence's prefix \"" << prefix << "\"";
   path pathPrefix(prefix);
   path pathSearch(pathPrefix.parent_path());
   path namePrefix(pathPrefix.filename());
   size_t lenPrefix = namePrefix.native().length();

   //LOG(info) << "Searching for prefix " << namePrefix;
   //LOG(info) << "withing folder " << pathSearch;

   std::map<int, path> mapFiles;
   res_files.m_PathRoot = pathSearch;
   //  res_files.m_ModelName = pathPrefix.filename( ).string();
   msh_files.m_PathRoot = pathSearch;
   //  msh_files.m_ModelName = pathPrefix.filename( ).string();
   for (directory_iterator it = directory_iterator(pathSearch);
      it != directory_iterator(); it++)
   {
      path curname(it->path().filename());
      size_t l = curname.native().length();
      if (l <= lenPrefix + 9)
      {
         continue;
      }
      path::string_type curprefix(curname.native().begin(),
         curname.native().begin() + lenPrefix);
      if (curprefix != namePrefix.native())
      {
         continue;
      }

      path::string_type strIndex(curname.native().begin() + lenPrefix,
         curname.native().end() - 9);
      path::string_type::const_iterator iti;
      if (!is_number(strIndex))
      {
         continue;
      }
      int idx = atoi((const char *)strIndex.c_str());

      path::string_type gid_ext(curname.native().end() - 9, curname.native().end());

      if ((isbinary && gid_ext == ".post.bin") || gid_ext == ".post.res")
      {
         path tmp(pathSearch);
         tmp /= curname;
         res_files.m_Parts[idx] = tmp;
      } else if (!isbinary && gid_ext == ".post.msh")
      {
         path tmp(pathSearch);
         tmp /= curname;
         msh_files.m_Parts[idx] = tmp;
      }
   }
   return 0;
}



int ProcessInput(const po::variables_map &vm, FEM_Injector &theFEMinjector, char *fileName, char *modelName)
{


   //std::string input("O:\\projects\\VELaSSCo\\prototype\\testdata\\VELaSSCo_HbaseBasicTest-part-4.post.res");
   std::string input(fileName);
   //std::string input(vm["input"].as<std::string>());
   //std::string host(vm["host"].as<std::string>());
   //size_t port(vm["port"].as<size_t>());
   int status;

   theFEMinjector.setCurrentModel(modelName);

   path pathInputFile(input);
   if (!is_regular_file(pathInputFile))
   {
      //LOG(error) << "Input file " << pathInputFile << " is not a regular file";
      return -1;
   }
   //LOG(info) << "Going to process sequence for file " << pathInputFile;
   if (pathInputFile.native().length() <= 9)
   {
      //LOG(error) << "Input file must have one of the extensions: " << ".post.msh, .post.res or .post.bin";
      return -1;
   }
   path::string_type gid_ext(pathInputFile.native().end() - 9, pathInputFile.native().end());
   //LOG(trace) << "ext = " << gid_ext;
   bool isbinary = false;
   if (gid_ext == ".post.bin")
   {
      isbinary = true;
   } else if (gid_ext != ".post.msh" || gid_ext != ".post.res")
   {
      isbinary = false;
   } else
   {
      //LOG(error) << "Input file must have one of the extensions: " << ".post.msh, .post.res or .post.bin";
      return -1;
   }

   ModelFileParts res_files, msh_files;
   status = collect_sequence(pathInputFile.native(), isbinary, res_files, msh_files);
   if (status != SUCCESS)
   {
      return -1;
   }

   if (!isbinary && res_files.m_Parts.size() != msh_files.m_Parts.size())
   {
      //LOG(error) << "ASCII format requires the same number of msh and res files";
      return -1;
   }
   if (!isbinary)
   {
      for (PathContainerType::const_iterator it = res_files.m_Parts.begin();
         it != res_files.m_Parts.end(); it++)
      {
         if (msh_files.m_Parts.find(it->first) == msh_files.m_Parts.end())
         {
            //LOG(error) << "ASCII format missmatch, index " << it->first << " not found in msh container";
            return -1;
         }
      }
      for (PathContainerType::const_iterator it = msh_files.m_Parts.begin();
         it != msh_files.m_Parts.end(); it++)
      {
         if (res_files.m_Parts.find(it->first) == res_files.m_Parts.end())
         {
            //LOG(error) << "ASCII format missmatch, index " << it->first << " not found in res container";
            return -1;
         }
      }
   }

   //if (vm.count("drop"))
   //{
   //   status = DropVelasscoTables(host, port);
   //   if (status != SUCCESS)
   //   {
   //      return status;
   //   }
   //}
   //if (CheckVelasscoTables(host, port) != SUCCESS)
   //{
   //   CreateVelasscoTables(host, port);
   //}
   GlobalMeshInfoType meshInfo;
   GlobalResultInfoType resultInfo;
   std::string keyModel;
//Obs   status = InsertModelInfo(host, port, res_files, keyModel);
   for (PathContainerType::const_iterator it = res_files.m_Parts.begin();
      it != res_files.m_Parts.end(); it++)
   {
      GID::MeshResultType meshPart;
      GID::ResultContainerType resultPart;

      if (isbinary)
      {
//Obs         status = ProcessBinaryPart(host, port, keyModel, meshPart, resultPart, it->first, it->second);
         if (status != SUCCESS)
         {
            return status;
         }
      } else
      {
         status = GID::ParseMeshFile(msh_files.m_Parts[it->first].string(), meshPart);
         if (status != SUCCESS)
         {
            return status;
         }

         theFEMinjector.storeFEM_mesh(meshPart);

         // TODO: check status returned
         meshInfo.Update(meshPart);
         status = GID::ParseResultFile(it->second.string(), resultPart);
         if (status != SUCCESS)
         {
            return status;
         }
         resultInfo.Update(resultPart);
         //status = InsertPartResult_Data(host, port, keyModel, it->first,
         //   meshInfo.GetIndexElementSet(meshPart.header.name),
         //   meshPart, resultPart);
         if (status != SUCCESS)
         {
            return status;
         }

         theFEMinjector.storeFEM_results(resultPart);

      }
   }
   // status = InsertResult_MetaData( host, port, keyModel, meshInfo, resultInfo );
   if (status != SUCCESS)
   {
      return status;
   }

   theFEMinjector.writeAndClose();

   return SUCCESS;
}


int main(int argc, char* argv[])
{
   int rstat;
   char errTxt[1024];

   init_logger();
   try 
   { 

      Database VELaSSCo_db("O:\\projects\\VELaSSCo\\SVN_src\\EDM_plug_in\\db_template", "VELaSSCo", "VELaSSCo");
      Repository femRepository(&VELaSSCo_db, "FEM_models");
      
      FEM_Injector theFEMinjector(&fem_schema_velassco_SchemaObject);
      theFEMinjector.setCurrentSchemaName("fem_schema_velassco");

      theFEMinjector.setDatabase(&VELaSSCo_db);
      VELaSSCo_db.open();
      femRepository.open(sdaiRW);

      theFEMinjector.setCurrentRepository(&femRepository);




      std::string appName = boost::filesystem::basename(argv[0]); 
      int add = 0; 
      int like = 0; 
      std::vector<std::string> sentence; 

      /** Define and parse the program options 
       */ 
      namespace po = boost::program_options; 
      po::options_description desc("Options"); 
     
     //desc.add_options()
     //  ("help,", "Print help messages")
     //  ("input,i", po::value<std::string>()->required(), "one of the input files") 
     //  ("drop,d", "Drop the tables before insert") 
     //  ("force,f", "Force update if the model already exists") 
     //  ("suffix,s", po::value<std::string>()->default_value("_V4CIMNE"), "Table name suffix") 
     //  ("host,h", po::value<std::string>()->default_value("localhost"), "Thrift server host") 
     //  ("port,p", po::value<size_t>()->default_value(9090), "Thrift server port");

     po::positional_options_description positionalOptions; 
     positionalOptions.add("input", 1); 

     po::variables_map vm; 

     //try 
     //{ 
     //  po::store(po::command_line_parser(argc, argv).options(desc) 
     //            .positional(positionalOptions).run(), 
     //            vm); // throws on error 

     //  /** --help option 
     //   */ 
     //  if ( vm.count("help")  ) 
     //  { 
     //    std::cout << "This is just a template app that should be modified" 
     //              << " and added to in order to create a useful command" 
     //              << " line application" << std::endl << std::endl; 
     //    std::cout << desc << std::endl;
     //    return SUCCESS;
     //  } 

     //  po::notify(vm); // throws on error, so do after help in case 
     //                  // there are any problems 
     //} 
     //catch(boost::program_options::required_option& e) 
     //{ 
     //  std::cerr << "ERROR: " << e.what() << std::endl << std::endl; 
     //  std::cout << desc << std::endl;
     //  return ERROR_IN_COMMAND_LINE; 
     //} 
     //catch(boost::program_options::error& e) 
     //{ 
     //  std::cerr << "ERROR: " << e.what() << std::endl << std::endl; 
     //  std::cout << desc << std::endl;
     //  return ERROR_IN_COMMAND_LINE; 
     //} 

     //std::string suffix( vm["suffix"].as<std::string>( ) );
     //strTableModels += suffix;
     //strTableMetaData += suffix;
     //strTableData += suffix;
     if (argc >= 3 && strcmp(argv[1], "BuildIndexes") == 0) {
        theFEMinjector.buildFemSearchStructures(argv[2]);
     } else if (argc >= 4 && strcmp(argv[1], "Inject") == 0) {
        ProcessInput(vm, theFEMinjector, argv[2], argv[3]);
     }
     
      
   } catch (std::exception& e)  { 
     std::cerr << "Unhandled Exception reached the top of main: " 
               << e.what() << ", application will now exit" << std::endl; 
     return ERROR_UNHANDLED_EXCEPTION; 

   } catch (CedmError *e) {
      rstat = e->rstat;
      if (e->message) {
         strncpy(errTxt, e->message, sizeof(errTxt));
      } else {
         strncpy(errTxt, edmiGetErrorText(rstat), sizeof(errTxt));
      }
      delete e;
      printf(errTxt);
   } catch (int thrownRstat) {
      strncpy(errTxt, edmiGetErrorText(thrownRstat), sizeof(errTxt));
      printf(errTxt);
   }
   exit(0);
}

