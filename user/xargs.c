#include "kernel/param.h"
#include "kernel/types.h"
#include "user/user.h"

char buf[512];

int
readline ()
{
  int n;
  char c;
  int i = 0;
  while ((n = read (0, &c, sizeof (c))) > 0)
    {
      if (c != '\n')
        {
          buf[i] = c;
          i++;
        }
      else
        {
          break;
        }
    }
  buf[i] = 0;
  if (n < 0)
    {
      fprintf (2, "xargs: read error\n");
      exit (1);
    }
  // printf("xargs: readline '%s' i:%d\n",buf,i);
  return i;
}

void
readargs (int cur, char *argv[], int max)
{
  int i = 0;
  int len = strlen (buf);
  while (cur < max && i < len)
    {
      // read an arg from buff
      int j = i;
      while (buf[i] && buf[i] != ' ')
        {
          i++;
        }
      buf[i++] = 0;
      // add the arg to argv
      argv[cur] = &buf[j];
      cur++;
    }
  argv[cur] = 0;
  if (i < len)
    {
      fprintf (2, "xargs: too many arguments\n");
      exit (1); // too many arguments
    }
}

int
main (int argc, char *argv[])
{
  char *arg_arr[MAXARG];
  if (MAXARG - argc < 0)
    {
      fprintf (2, "xargs: too many arguments\n");
      exit (1);
    }
  memmove (arg_arr, argv, argc * sizeof (char *));
  int ret = 0;
  while ((ret = readline ()) > 0)
    {
      readargs (argc, arg_arr, MAXARG);
      if (fork () > 0)
        {
          wait (0);
        }
      else
        {
          // printf("exec(%s,", arg_arr[1]);
          // for(int n = 1; arg_arr[n]; n++)
          //   printf("%s ", arg_arr[n]);
          // printf(")\n");
          exec (arg_arr[1], &arg_arr[1]);
        }
    }
  exit (0);
}