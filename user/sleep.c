#include "kernel/types.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  if(argc <= 1){
    fprintf(2, "usage: sleep duration\n");
    exit(1);
  }

  char *arg = argv[1];
  int duration = atoi(arg);
  sleep(duration);
  exit(0);
}
