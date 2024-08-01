#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define ARRAY_LEN(a) (sizeof(a) / sizeof(a[0]))

static void
die(char *s)
{
  fprintf(2, "%s\n", s);
  exit(1);
}

int
main(int argc, char *argv[])
{
  int parent_pipe[2];
  if (pipe(parent_pipe) != 0) {
    die("failed to create pipe");
  }

  int parent_to_child_read = parent_pipe[0];
  int parent_to_child_write = parent_pipe[1];

  int child_pipe[2];
  if (pipe(child_pipe) != 0) {
    die("failed to create pipe");
  }

  int child_to_parent_read = child_pipe[0];
  int child_to_parent_write = child_pipe[1];

  const char ping[] = "ping";
  const char pong[] = "pong";

  int pid = fork();
  if (pid > 0) { // parent
    if (write(parent_to_child_write, ping, ARRAY_LEN(ping)) != ARRAY_LEN(ping)) {
      die("failed to write from parent");
    }
    char buf[ARRAY_LEN(pong)];
    if (read(child_to_parent_read, buf, ARRAY_LEN(buf)) != ARRAY_LEN(buf)) {
      die("failed to read from parent");
    }
    printf("%d: received %s\n", getpid(), buf);
  } else if (pid == 0) { // child
    char buf[ARRAY_LEN(ping)];
    if (read(parent_to_child_read, buf, ARRAY_LEN(buf)) != ARRAY_LEN(buf)) {
      die("failed to read from child");
    }
    printf("%d: received %s\n", getpid(), buf);
    if (write(child_to_parent_write, pong, ARRAY_LEN(pong)) != ARRAY_LEN(pong)) {
      die("failed to write from child");
    }
  } else {
    die("fork error");
  }
  
  exit(0);
}
