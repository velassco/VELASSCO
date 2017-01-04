#ifndef CLOGGER_H
#define CLOGGER_H


#include <time.h>


#define indent "    "
#define ENTER_FUNCTION
#define LEAVE_FUNCTION


/*================================================================================================*/
class CLogger
/*================================================================================================*/
{
   bool                    traceToLogg;
   bool                    logToStdout;
   bool                    alwaysFlush;
   const char              *leftCol;
public:
   int                     nErrors;
   int                     nWarnings;
   FILE                    *fp;
   CLogger(FILE *_fp, bool _logToStdout, bool _alwaysFlush, bool _traceToLogg = false) {
      fp = _fp; logToStdout = _logToStdout; alwaysFlush = _alwaysFlush; leftCol = "   ";
      nErrors = 0; nWarnings = 0; traceToLogg = _traceToLogg;
   }
   ~CLogger() { if (fp) fclose(fp); }
   void                   trace(EDMLONG nParams, const char *format, ...);
   void                   logg(EDMLONG nParams, const char *format, ...);
   void                   warning(EDMLONG nParams, const char *format, ...);
   void                   loggError(EDMLONG nParams, const char *format, ...);
   void                   fatalError(EDMLONG nParams, const char *format, ...);
   void                   edmError(EDMLONG rstat);
   void                   setLeftCol(const char *lc) { leftCol = lc; }
   void                   setTrace(bool t) { traceToLogg = t; }
};


/*================================================================================================*/
class CLoggWriter : public CLogger
/*================================================================================================*/
{
public:
   CLoggWriter(FILE *_fp) : CLogger(_fp, _fp == NULL, true) { }
   CLoggWriter(FILE *_fp, bool _logToStdout, bool _alwaysFlush, bool _traceToLogg = false) : CLogger(_fp,_logToStdout, _alwaysFlush, _traceToLogg) { }

   void                    error(EDMLONG nParams, const char *format, ...);
   void                    rstatError(EDMLONG rstat, int nParams, const char *format, ...);
   void                    printStatistics();
   void                    newLine();
   void                    sectionHeader();
   void                    sectionTrailer();
   void                    sectionHeader(char *text);
   void                    printTimeUsed(char *format, time_t startTime);
   void                    aLine();
};

#endif 