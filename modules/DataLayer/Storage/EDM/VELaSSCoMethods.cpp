
#include "stdafx.h"
#include "VELaSSCoMethods.h"
#include "modules/DataLayer/Storage/EDM_server_dll/EDM_server_dll.h"
#include <omp.h>




/*===============================================================================================*/
void VELaSSCoMethods::ListModels()
/*===============================================================================================*/
{
   if (serverContexts) {
      SdaiServerContext *scs = serverContexts->getElementArray();
      EDMULONG nsc = serverContexts->size();
#pragma omp parallel for
      for (int i = 0; i < nsc; i++) {
         SdaiString        *foundNames;
         CHECK(edmiRemoteListModels(scs[i], NULL, NULL, NULL, NULL, NULL, 0, &foundNames, NULL));
         printf("Thread number %llu\n", omp_get_thread_num());
         for (int i = 0; foundNames[i]; i++) {
            printf("%s\n", foundNames[i]);
         }
         printf("\n\n");
      }
   }
}

/*===============================================================================================*/
void VELaSSCoMethods::ValidateModels()
/*===============================================================================================*/
{
   int startTime = GetTickCount();
   if (serverContexts) {
      SdaiServerContext *scs = serverContexts->getElementArray();
      EDMULONG nsc = serverContexts->size();
#pragma omp parallel for
      for (int i = 0; i < nsc; i++) {
         SdaiInteger        warnings, errors;
         char diaFileName[2048];
         sprintf(diaFileName, "O:\\projects\\VELaSSCo\\SVN_src\\EDM_plug_in\\db_cluster\\validate_%d.txt", i);
         CHECK(edmiRemoteValidateModel(scs[i], "FEM_models", "VELaSSCo_HbaseBasicTest_part_1", diaFileName, 0, NULL, FULL_VALIDATION,
            NULL, NULL, 0, NULL, 0, &warnings, &errors, NULL));
      }
   }
   int endTime = GetTickCount();
   printf("Elapsed time for the validation is %d millisecponds\n", endTime - startTime);
}


/*===============================================================================================*/
void VELaSSCoMethods::getBoundingBox()
/*===============================================================================================*/
{
   int startTime = GetTickCount();
   if (serverContexts) {
      SdaiServerContext *scs = serverContexts->getElementArray();
      EDMULONG nsc = serverContexts->size();
      SdaiInteger          edmSystemType;

#pragma omp parallel for
      for (int i = 0; i < nsc; i++) {
         SdaiInteger        warnings, errors;
         //CHECK(edmiRemoteExecuteCppMethod(scs[i], "FEM_models", "VELaSSCo_HbaseBasicTest_part_1", "cpp_plugins", "VELaSSCo", "getBoundingBox",
         //   0, 0, NULL, NULL, &returnValue, NULL));
      }
   }
   int endTime = GetTickCount();
   printf("Elapsed time is %d milliseconds\n", endTime - startTime);
}



/*===============================================================================================*/
void VELaSSCoMethods::GetListOfAnalyses(rvGetListOfAnalyses& _return, const std::string& modelID)
/*===============================================================================================*/
{
   int startTime = GetTickCount();
   if (serverContexts) {
      SdaiServerContext *scs = serverContexts->getElementArray();
      EDMULONG nsc = serverContexts->size();
      SdaiInteger          edmSystemType;

#pragma omp parallel for
      for (int i = 0; i < nsc; i++) {
         SdaiInteger        warnings, errors;
         //CHECK(edmiRemoteExecuteCppMethod(scs[i], "FEM_models", "VELaSSCo_HbaseBasicTest_part_1", "cpp_plugins", "VELaSSCo", "getBoundingBox",
         //   0, 0, NULL, NULL, &returnValue, NULL));
      }
   }
   int endTime = GetTickCount();
   printf("Elapsed time is %d milliseconds\n", endTime - startTime);
}


/*===============================================================================================*/
void VELaSSCoMethods::GetListOfResults(char *modelId, char *analysisID, double timeStep)
/*===============================================================================================*/
{
   int startTime = GetTickCount();
   if (serverContexts) {
      SdaiServerContext *scs = serverContexts->getElementArray();
      EDMULONG nsc = serverContexts->size();
      SdaiInteger          edmSystemType;

#pragma omp parallel for
      for (int i = 0; i < nsc; i++) {

      }
   }
   int endTime = GetTickCount();
   printf("Elapsed time for the validation is %d millisecponds\n", endTime - startTime);
}


/*===============================================================================================*/
void VELaSSCoMethods::GetListOfTimeSteps(rvGetListOfTimeSteps& rv, char *modelId, char *analysisID)
/*===============================================================================================*/
{
   int startTime = GetTickCount();
   if (serverContexts) {
      SdaiServerContext *scs = serverContexts->getElementArray();
      EDMULONG nsc = serverContexts->size();
      SdaiInteger          edmSystemType;

#pragma omp parallel for
      for (int i = 0; i < nsc; i++) {
         //SdaiInteger        warnings, errors;
         //CHECK(edmiRemoteExecuteCppMethod(scs[i], "FEM_models", "VELaSSCo_HbaseBasicTest_part_1", "cpp_plugins", "VELaSSCo", "", 0, 0, NULL, &params[i], NULL));
      }
   }
   int endTime = GetTickCount();
   printf("Elapsed time for the validation is %d millisecponds\n", endTime - startTime);
}
