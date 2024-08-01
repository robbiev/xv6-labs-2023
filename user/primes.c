#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

static void
die(char *s)
{
  fprintf(2, "%s\n", s);
  exit(1);
}

void sieve(int left_neighbor_read) {
  // p = get a number from left neighbor
  int p = 0;
  int bytes_read = read(left_neighbor_read, &p, sizeof(p));
  if (bytes_read <= 0) {
    return;
  }

  // print p
  printf("prime %d\n", p);

  // create this pipe later to avoid creating unnecessary pipes
  int right_neighbor[2] = {-1};

  // loop:
  for(;;) {
    // n = get a number from left neighbor
    int n = 0;
    int bytes_read = read(left_neighbor_read, &n, sizeof(n));
    if (bytes_read <= 0) {
      break;
    }
    // if (p does not divide n)
    if (n % p != 0) {
      if (right_neighbor[0] == -1) {
        if (pipe(right_neighbor) != 0) {
          die("failed to create right neighbor pipe");
        }
      }
      // send n to right neighbor
      if (write(right_neighbor[1], &n, sizeof(n)) != sizeof(n)) {
        die("failed to write to right neighbor");
      }
    }
  }

  close(left_neighbor_read);
  close(right_neighbor[1]);

  int pid = fork();
  if (pid == 0) {
    sieve(right_neighbor[0]);
  } else if (pid < 0) {
    die("fork error");
  } else {
    close(right_neighbor[0]);
    wait(0);
  }
}

int
main(void)
{
  int start_pipe[2];
  if (pipe(start_pipe) != 0) {
    die("failed to create initial pipe");
  }

  for (int i = 2; i <= 35; i++) {
    if (write(start_pipe[1], &i, sizeof(i)) != sizeof(i)) {
      die("failed to write initial value");
    }
  }

  close(start_pipe[1]);
  sieve(start_pipe[0]);
  exit(0);
}
