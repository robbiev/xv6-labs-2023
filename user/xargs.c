#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/param.h"
#include "user/user.h"

#define MAX_ARG_BYTES 128

static void
die(char *s)
{
  fprintf(2, "%s\n", s);
  exit(1);
}

static char *
argdup(char *arg)
{
  char *argdup = malloc(MAX_ARG_BYTES);
  if (argdup == 0) {
    die("malloc failed");
  }
  strcpy(argdup, arg);
  return argdup;
}

int
main(int argc, char *argv[])
{
  if (argc < 2) {
    die("Usage: xargs command");
  }

  char *nargv[MAXARG] = {0};
  int nargv_idx = 0;

  // copy initial args over, including the program name
  for (int i = 1; i < argc; i++) {
    nargv[nargv_idx++] = argv[i];
  }
  int nargv_first_idx = nargv_idx;

  char arg[MAX_ARG_BYTES] = {0};
  int arg_idx = 0;

  char c;
  while (read(0, &c, sizeof c) == sizeof c) {
    if (c == '\n') {
      if (arg_idx != 0) {
        arg[arg_idx] = '\0';
        arg_idx = 0;
        nargv[nargv_idx++] = argdup(arg);
      }
      nargv[nargv_idx] = '\0';

      int pid = fork();
      if (pid > 0) {
        wait(0);
        for (int i = nargv_first_idx; i < nargv_idx; i++) {
          free(nargv[i]);
        }
        nargv_idx = nargv_first_idx;
      } else if (pid == 0) {
        int ret = exec(argv[1], nargv);
        if (ret < 0) {
          die("exec error");
        }
      } else {
        die("fork error");
      }
    } else if (c == ' ') {
      arg[arg_idx] = '\0';
      arg_idx = 0;
      nargv[nargv_idx++] = argdup(arg);
    } else {
      arg[arg_idx++] = c;
    }
  }
  return 0;
}
