#include "kernel/types.h"
#include "user/user.h"

#define MAX_NUM 35


int PrimeCheck (int left_neigh);

int
main (int argc, char *argv[])
{
  // create main pipe for feeding number
  int pp[2];
  pipe (pp);

  // create the first process to check primes
  PrimeCheck (pp[0]);

  // start feeding
  int i;
  for (i = 2; i <= MAX_NUM; i++)
    {
      write (pp[1], &i, 4);
    }
  // write a negative number as an end flag
  i = -1;
  write (pp[1], &i, 4);

  wait (0);
  exit (0);
}

int
PrimeCheck (int left_neigh)
{
  // create the process to check primes
  int pid = fork ();
  if (pid == 0)
    { // check prime in child process
      // close unused file descriptors
      for (int i = 3; i < left_neigh; i++)
        {
          close (i);
        }
      // get a number from left neighbor which should be a prime
      int p;
      read (left_neigh, &p, 4);
      printf ("prime %d\n", p);

      // create pipe for next prime check process
      int pp[2];
      int ret = pipe (pp);
      // check pipe created successfully
      if (ret != 0)
        {
          printf ("Failed to create pipe\n", p);
          exit (1);
        }
      // create another process to check primes further
      int created = 0;
      // continue reading number from left neighbor
      while (1)
        {
          int n;
          read (left_neigh, &n, 4);
          if (n < 0)
            {
              write (pp[1], &n, 4);
              break;
            }
          if (n % p != 0)
            {
              if (created == 0)
                {
                  PrimeCheck (pp[0]);
                  created = 1;
                }
              write (pp[1], &n, 4);
            }
        }

      if (created)
        {
          wait (0);
        }
      exit (0);
    }
  return pid;
}