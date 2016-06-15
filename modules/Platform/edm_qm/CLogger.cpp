

#include <stdio.h>
#include <tchar.h>
#include <stdarg.h>
#include <string.h>

extern "C" {
   #include "sdai.h"
   #include "cpp10_EDM_interface.h"
}

#include "CLogger.h"


/*================================================================================================*/
void CLoggWriter::error(EDMLONG nParams, const char *format, ...)
/*================================================================================================*/
{
   char              *params[10];
   va_list           pList;

   ENTER_FUNCTION
   nErrors++;
   va_start(pList, format);
   if (nParams > 10) nParams = 10;
   for (int i = 0; i < nParams; i++) params[i] = va_arg(pList, char *);
   logg(0, "\n"indent"Error: ");
   logg(nParams, format, params[0], params[1], params[2], params[3], params[4], params[5], params[6], params[7], params[8], params[9]);
   va_end(pList);
   LEAVE_FUNCTION
}
/*================================================================================================*/
void CLoggWriter::rstatError(EDMLONG rstat, int nParams, const char *format, ...)
/*================================================================================================*/
{
   char              *params[10];
   va_list           pList;

   ENTER_FUNCTION
   nErrors++;
   va_start(pList, format);
   if (nParams > 10) nParams = 10;
   for (int i = 0; i < nParams; i++) params[i] = va_arg(pList, char *);
   logg(0, "\n"indent"Error: ");
   logg(nParams, format, params[0], params[1], params[2], params[3], params[4], params[5], params[6], params[7], params[8], params[9]);
   va_end(pList);
   LEAVE_FUNCTION
}


/*===============================================================================*/
/*  getDateAndTimeString                                                         */
/*===============================================================================*/
char *getDateAndTimeString()
{
   static char    dateAndTimeString[100];
   time_t         ctime;
   struct tm      *pm;
   char           *cp;

   ENTER_FUNCTION
      dateAndTimeString[0] = '\0';
   ctime = time(NULL);
   pm = localtime((time_t *)&ctime);
   if (pm != NULL) {
      strcpy(dateAndTimeString, asctime(pm));
      cp = dateAndTimeString + EDM_STRLEN(dateAndTimeString) - 1;
      if (*cp == '\n') {
         *cp = '\0';
      }
   }
   LEAVE_FUNCTION
      return(dateAndTimeString);
}


/*================================================================================================*/
void CLogger::logg(EDMLONG nParams, const char *format, ...)
/*================================================================================================*/
{
   char              *params[10];
   va_list           pList;

   if (nParams > 10) nParams = 10;
   va_start(pList, format);
   for (int i = 0; i < nParams; i++) params[i] = va_arg(pList, char *);
   va_end(pList);
   if (fp) {
      fprintf(fp, format, params[0], params[1], params[2], params[3], params[4], params[5], params[6], params[7], params[8], params[9]);
      if (alwaysFlush) {
         fflush(fp);
      }
   }
   if (logToStdout) {
      printf(format, params[0], params[1], params[2], params[3], params[4], params[5], params[6], params[7], params[8], params[9]);
   }
}
/*================================================================================================*/
void CLogger::fatalError(EDMLONG nParams, const char *format, ...)
/*================================================================================================*/
{
   char              *params[10];
   va_list           pList;

   if (nParams > 10) nParams = 10;
   va_start(pList, format);
   for (int i = 0; i < nParams; i++) params[i] = va_arg(pList, char *);
   va_end(pList);
   logg(1, "\n---  Fatal error detected %s ---\n", getDateAndTimeString()); logg(0, leftCol);
   logg(nParams, format, params[0], params[1], params[2], params[3], params[4], params[5], params[6], params[7], params[8], params[9]);
   logg(1, "\n");
   //THROW(sdaiENOERROR);
}
/*================================================================================================*/
void CLogger::loggError(EDMLONG nParams, const char *format, ...)
/*================================================================================================*/
{
   char              *params[10];
   va_list           pList;

   if (nParams > 10) nParams = 10;
   va_start(pList, format);
   for (int i = 0; i < nParams; i++) params[i] = va_arg(pList, char *);
   va_end(pList);
   logg(1, "\n---  Error detected %s ---\n", getDateAndTimeString()); logg(0, leftCol);
   logg(nParams, format, params[0], params[1], params[2], params[3], params[4], params[5], params[6], params[7], params[8], params[9]);
   logg(1, "\n"); nErrors++;
}
/*================================================================================================*/
void CLogger::warning(EDMLONG nParams, const char *format, ...)
/*================================================================================================*/
{
   char              *params[10];
   va_list           pList;

   if (nParams > 10) nParams = 10;
   va_start(pList, format);
   for (int i = 0; i < nParams; i++) params[i] = va_arg(pList, char *);
   va_end(pList);
   logg(0, "---  Warning: ");
   logg(nParams, format, params[0], params[1], params[2], params[3], params[4], params[5], params[6], params[7], params[8], params[9]);
   logg(1, "\n"); nWarnings++;
}
/*================================================================================================*/
void CLogger::edmError(EDMLONG rstat)
/*================================================================================================*/
{
   loggError(2, "EDM error nr %u, %s", rstat, edmiGetErrorText(rstat));
}
