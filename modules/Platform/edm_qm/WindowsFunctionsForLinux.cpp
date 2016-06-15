#include <sys/time.h>

unsigned int GetTickCount()
{
   struct timeval tv;
   if (gettimeofday(&tv, 0) != 0)
      return 0;

   return (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
}