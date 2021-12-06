#include <stdio.h>
#include <signal.h>
#include <sys/ioctl.h>

void signalwinch(int sig) {
  struct winsize ws;
  ioctl(0, TIOCGWINSZ, &ws);
  int width = ws.ws_col;
  int height = ws.ws_row;
  fprintf(stdout, "{w:%d,h:%d}\n", width, height);
}

int main(int argc, char **argv) {
  signal(SIGWINCH, signalwinch);
  fprintf(stdout, "Press [any] key to continue...\n");
  getchar();
  return 0;
}
