#include "kernel/types.h"
#include "user/user.h"


int
main(int argc, char *argv[])
{
  // create pipe
  int p[2];
  pipe(p);
  char data = 'a';
  // fork a child process
  if(fork() > 0) {
    // In parent: write then read
    write(p[1], &data, 1);
    wait((int *)0);
    read(p[0], &data, 1);
    printf("%d: received pong\n",getpid());
  } else {
    // In child: read then write
    read(p[0], &data, 1);
    printf("%d: received ping\n",getpid());
    write(p[1], &data, 1);
    exit(0);
  }
  exit(0);
}