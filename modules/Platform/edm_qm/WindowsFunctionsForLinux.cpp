#include <sys/time.h>
#include <sys/stat.h>
#include "WindowsFunctionsForLinux.h"

unsigned int GetTickCount()
{
   struct timeval tv;
   if (gettimeofday(&tv, 0) != 0)
      return 0;

   return (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
}

int _mkdir(char * _Path)
{
   mode_t mode = 0777; 
   return  mkdir(_Path, mode);
}
