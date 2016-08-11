#include <stdio.h>
typedef union  {
  double d;
  char c[ 8];
} t_u_dc;

void testSwap( double value) {
  t_u_dc dc;
  dc.d = value;
  printf( "Hexadecimal:");
  for ( int i = 0; i < 8; i++) {
    char c = dc.c[ i];
    printf( " %02x", ( unsigned int)( unsigned char)c);
  }
  printf( "\n");
  for ( int i = 0; i < 4; i++) {
    char c = dc.c[ i];
    dc.c[ i] = dc.c[ 7 - i];
    dc.c[ 7 - i] = c;
  }
  printf( "value = %.18g\n", value);
  printf( "swap  = %.18g\n", dc.d);
}

int main () {
   t_u_dc dc;

//   // double lst[] = { 6.81353607719685E13, 5.483205298968781E23, 5.48320529976043E23, -3.961762473406214E-9, -2.0590817735107401E21, -2.0590813509718576E21 };
//   // double lst[] = { 3.0576006386805636E299, 4.392512360524972E301, 60.0, -2.2727404460889676E304, -1.198200581152502E307, 0.0 };
//   // double lst[] = { 	2.6776527269065475E305 ,	3.460195063747691E287 ,	2.1180744554273793E277 ,	-2.1508966256030255E305 ,	-1.022517579872302E304 ,	-2.781757137547606E284 };
//   double lst[] = {	1.4111782168453345E277, 	1.4111782168453345E277, 	4.034880275012215E175, 	-7.845911676394542E298, 	-2.458343745902715E255, 	-3.571439782774524E250 };
//   for ( size_t i = 0; i < sizeof( lst) / sizeof( double); i++) {
//     testSwap( lst[ i]);
//   }
//   
//   dc.c[ 0] = '\x3f';
//   dc.c[ 1] = '\xf8';
//   dc.c[ 2] = '\xf5';
//   dc.c[ 3] = '\xc2';
//   dc.c[ 4] = '\x8f';
//   dc.c[ 5] = '\x5c';
//   dc.c[ 6] = '\x28';
//   dc.c[ 7] = '\xfa';
//   testSwap( dc.d);
//   dc.c[ 0] = '\xc2';
//   dc.c[ 1] = '\x78';
//   dc.c[ 2] = '\x05';
//   dc.c[ 3] = '\xe0';
//   dc.c[ 4] = '\x17';
//   dc.c[ 5] = '\x2f';
//   dc.c[ 6] = '\xad';
//   dc.c[ 7] = '\x3e';
//   testSwap( dc.d);
//   testSwap( 9);

  dc.c[ 0] = '?';
  dc.c[ 1] = '@';
  dc.c[ 2] = 'b';
  dc.c[ 3] = 'M';
  dc.c[ 4] = '\xFA';
  dc.c[ 5] = '6';
  dc.c[ 6] = 'p';
  dc.c[ 7] = '\xA7';
  testSwap( dc.d);
  dc.c[ 0] = '?';
  dc.c[ 1] = '\xF0';
  dc.c[ 2] = '\x00';
  dc.c[ 3] = '\x00';
  dc.c[ 4] = '\x00';
  dc.c[ 5] = '\x00';
  dc.c[ 6] = '\x00';
  dc.c[ 7] = '\x00';

  testSwap( dc.d);
}
