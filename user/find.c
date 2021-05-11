#include "kernel/types.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/stat.h"

char *getname(char *path) {
  char *p;

  // Find first character after last slash.
  for (p = path + strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;

  return p;
}

void find(char *path, char *str) {
  // printf("find: find %s in %s\n", str, path);
  char buf[512], *p;
  int fd;
  struct dirent de;
  struct stat st;
  // open fd of given path
  if ((fd = open(path, 0)) < 0) {
    fprintf(2, "find: cannot open %s\n", path);
    return;
  }
  // get fd state
  if (fstat(fd, &st) < 0) {
    fprintf(2, "find: cannot stat %s\n", path);
    close(fd);
    return;
  }
  // process according to fd type
  switch (st.type) {
  case T_FILE:
    // if file, print error
    printf("Invalid path: %d is file\n", path);
    break;
  case T_DIR: // if dir
    // check path length
    if (strlen(path) + 1 + DIRSIZ + 1 > sizeof buf) {
      printf("find: path too long\n");
      break;
    }
    // generate path string of the dir for further read
    strcpy(buf, path);
    p = buf + strlen(buf);
    *p++ = '/';
    // read dirents
    while (read(fd, &de, sizeof(de)) == sizeof(de)) {
      if (de.inum == 0)
        continue;
      // append dirent name to dir path to get full path of a dirent
      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;
      // get file info by full path
      if (stat(buf, &st) < 0) {
        printf("find: cannot stat %s\n", buf);
        continue;
      }
      // check fd type
      char *name = getname(buf);
      // printf("find: name: '%s', buf: '%s'\n", name, buf);
      switch (st.type) {
      case T_FILE: // if file, check it
        // printf("find: checking '%s' '%s'\n", name, str);
        if (!strcmp(name, str)) {
          printf("%s\n", buf);
        }
        break;
      case T_DIR: // if dir: skip . & .. , recursive call on sub dir
        if (name[0] == '.')
          break;
        find(buf, str);
        break;
      }
      // printf("%s %d %d %d\n", fmtname(buf), st.type, st.ino, st.size);
    }
    break;
  }
  close(fd);
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    printf("find: invalid argument\n");
    exit(0);
  }
  if (argc == 2) {
    find(".", argv[1]);
    exit(0);
  }

  find(argv[1], argv[2]);
  exit(0);
}
