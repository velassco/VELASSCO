// FemLab -*- C++ -*- Created: galindo Sep-1993, Modified: galindo Sep-1993
#ifndef Crono_h
#define Crono_h

#ifdef _WIN32
#include <time.h>
#include <sys/types.h>
#include <sys/timeb.h>
#else
#include <unistd.h>
#include <sys/times.h>
#include <sys/timeb.h>
#include <time.h> // CLK_TCK
#  ifndef CLK_TCK
#   define CLK_TCK      CLOCKS_PER_SEC
#  endif
#endif

//class ofstream; // defined elsewhere
//class ostream;

#include <stdio.h>

inline clock_t currentTime() {
// #ifndef _WIN32
//   struct tms time;
//   times(&time);
//   // return time.tms_utime; // CPU time
//   // return time.tms_stime; // sys time
//   return time.tms_utime + time.tms_stime; // CPU + sys time
// #else
   return clock();
// #endif
}

inline int currentWallTime( struct timeb *tp) {
#ifndef _WIN32
  return ftime( tp);
#else
  // MS Wind de los co...... que no respeta los estandares.
  ftime( tp);
  return 0;
#endif
}

inline std::string getStrCurrentWallTime() {
#define MAX_getStrCurrentWallTime   255
  char tmp[ MAX_getStrCurrentWallTime + 1];
  time_t time_tmp = time( NULL);
  bool ok = false;
  if ( time_tmp != ((time_t) -1)) {
    struct tm *time_ptr = localtime( &time_tmp);
    if ( time_ptr != NULL) {
      int rc = strftime( tmp, MAX_getStrCurrentWallTime, "%Y.%m.%d %H:%M:%S", time_ptr);
      ok = ( rc > 0);
    }
  }
  if ( !ok) {
    strcpy( tmp, "N/A");
  }
  return std::string( tmp);
#undef MAX_getStrCurrentWallTime
}

typedef enum {
  CRONO_SYS_TIME = 0, CRONO_WALL_TIME = 1
} _t_crono_measure_type;

class Crono {
public:
  void ini( void) {
    if ( _time_type == CRONO_SYS_TIME)
      _clock_inicio = currentTime();
    else
      currentWallTime( &_time_inicio);
  }
  float fin( void) {
    float res = 0.0;
    if ( _time_type == CRONO_SYS_TIME)
      res = ( float)( currentTime() - _clock_inicio) / ( float)CLK_TCK;
    else {
      struct timeb tmp;
      currentWallTime( &tmp);
      time_t dif_sec = tmp.time - _time_inicio.time;
      int dif_msec = tmp.millitm - _time_inicio.millitm;
      res = ( float)( ( double)( dif_sec * 1000 + dif_msec) / 1000.0);
    }
    ini();
    return res;
  }
  Crono( _t_crono_measure_type tt = CRONO_SYS_TIME): _time_type( tt) {
    ini();
  };

private:
  _t_crono_measure_type _time_type;
  clock_t _clock_inicio;
  struct timeb _time_inicio;
};

#endif // #ifndef Crono_h
