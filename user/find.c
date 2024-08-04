#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/param.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"

static void
die(char *s)
{
  fprintf(2, "%s\n", s);
  exit(1);
}

void
find(char *path, char *name, char *filter)
{
  int fd;
  if ((fd = open(path, O_RDONLY)) < 0) {
    fprintf(2, "find: cannot open %s\n", path);
    return;
  }

  struct stat st;
  if (fstat(fd, &st) < 0) {
    fprintf(2, "find: cannot stat %s\n", path);
    close(fd);
    return;
  }

  switch(st.type){
  case T_FILE:
    if (filter == 0 || strcmp(name, filter) == 0) {
      printf("%s\n", path);
    }
    break;
  case T_DIR:
    char buf[MAXPATH];

    // need space for:
    //   path + '/' + DIRSIZ + '\0'
    if (strlen(path) + 1 + DIRSIZ + 1 > sizeof buf) {
      printf("find: path too long\n");
      break;
    }

    strcpy(buf, path);

    // overwrite the null terminator with path separator
    char *p = &buf[strlen(buf)];
    *p++ = '/';

    struct dirent de;
    while (read(fd, &de, sizeof(de)) == sizeof(de)) {
      if (de.inum == 0) {
        continue;
      }
      if (de.name[0] == '.') {
        continue;
      }
      memmove(p, de.name, DIRSIZ);

      // Ensure the string is null terminated which is needed because the name
      // is only null terminated if it doesn't fill DIRSIZ. For example if
      // DIRSIZ is 14 and the string is 14 characters long, it won't be null
      // terminated. If it's 13 characters long, it will be null terminated,
      // and adding the null terminator here will be a no-op.
      p[DIRSIZ] = 0;

      find(buf, de.name, filter);
    }
    break;
  }
  close(fd);
}

int
main(int argc, char *argv[])
{
  switch (argc) {
    case 1:
      find(".", ".", 0);
      break;
    case 2:
      find(argv[1], argv[1], 0);
      break;
    case 3:
      find(argv[1], argv[1], argv[2]);
      break;
    default:
      die("Usage: find [path] [name]");
      break;
  }
  exit(0);
}
