#include < time.h >


#define indent "    "
#define ENTER_FUNCTION
#define LEAVE_FUNCTION


/*================================================================================================*/
class CLogger
/*================================================================================================*/
{
   bool                    logToStdout;
   bool                    alwaysFlush;
   char                    *leftCol;
public:
   int                     nErrors;
   int                     nWarnings;
   FILE                    *fp;
   CLogger(FILE *_fp, bool _logToStdout, bool _alwaysFlush) {
      fp = _fp; logToStdout = _logToStdout; alwaysFlush = _alwaysFlush; leftCol = "   ";
      nErrors = 0; nWarnings = 0;
   }
   ~CLogger() { if (fp) fclose(fp); }
   void                   logg(EDMLONG nParams, char *format, ...);
   void                   warning(EDMLONG nParams, char *format, ...);
   void                   loggError(EDMLONG nParams, char *format, ...);
   void                   fatalError(EDMLONG nParams, char *format, ...);
   void                   edmError(EDMLONG rstat);
   void                   setLeftCol(char *lc) { leftCol = lc; }
};


class CLoggWriter : public CLogger
{
public:
   CLoggWriter(FILE *_fp) : CLogger(_fp, _fp == NULL, true) { }
   CLoggWriter(FILE *_fp, bool _logToStdout, bool _alwaysFlush) : CLogger(_fp,_logToStdout, _alwaysFlush) { }

   void                    error(EDMLONG nParams, char *format, ...);
   void                    rstatError(EDMLONG rstat, int nParams, char *format, ...);
   void                    printStatistics() {
      logg(1, nErrors > 1 ? "%d errors found" : (nErrors == 0 ? "No errors found." : "One error found"), nErrors);
      newLine();
   }
   void                    newLine() { logg(0, "\n"); }
   void                    sectionHeader() {
      newLine(); newLine();
      logg(0, "========================================================================="); newLine();
   }
   void                    sectionHeader(char *text) {
      sectionHeader(); logg(0, text); newLine();
   }
   void                    sectionTrailer() {
      logg(0, "========================================================================="); newLine();
   }
   void                    printTimeUsed(char *format, time_t startTime) {
      time_t now;
      time(&now);
      char buf[128], tbuf[256], *bp = buf;
      EDMULONG timeUsed = now - startTime;
      EDMULONG nHours = timeUsed / 3600, nMin = (timeUsed % 3600) / 60, nSec = timeUsed % 60;
      if (timeUsed == 0) {
         sprintf(tbuf, format, "less than one second.");
      } else {
         if (nHours) {
            bp += sprintf(bp, "%d hours%s", nHours, (nMin && nSec) ? ", " : ((nMin || nSec) ? " and " : ""));
         }
         if (nMin) {
            bp += sprintf(bp, "%d minutes%s", nMin, nSec ? " and " : "");
         }
         if (nSec) {
            bp += sprintf(bp, "%d seconds", nSec);
         }
         sprintf(tbuf, format, buf);
      }
      logg(0, tbuf);
   }
   void                    aLine() {
      logg(0, indent"-------------------------------------------------------------------------"); newLine();
   }
};
