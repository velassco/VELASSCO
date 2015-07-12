#include "Logger.h"

int main()
{
  init_logger( );
  LOG(warning) << "Hello world";
  LOG(error) << "Hello world";
  LOG(info) << "Hello world";
}
