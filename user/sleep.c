#include "kernel/types.h"
#include "user/user.h"


int
main(int argc, char *argv[])
{
  // Check arguments number
  if (argc != 2)
  {
    fprintf(2, "Invalid number of arguments.\n");
    exit(1);
  }
  // Convert argument string to integer
  int second = atoi(argv[1]);
  sleep(second);
  exit(0);
}
