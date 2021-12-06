#include <stdio.h>
#include <errno.h>
#include "readfile.h"

int main(int argc, char **argv) {
  char *filename = "/c/readtest.txt";
  writefirstline(filename, READFILEWRITETYPELISTCLEAR, NULL);
  writefirstline(filename, READFILEWRITETYPELISTAPPEND, "test1");
  writefirstline(filename, READFILEWRITETYPELISTAPPEND, "2test");
  writefirstline(filename, READFILEWRITETYPELISTAPPEND, "2test");
  writefirstline(filename, READFILEWRITETYPELISTAPPEND, "test3");
  writefirstline(filename, READFILEWRITETYPELISTAPPEND, "2test");
  writefirstline(filename, READFILEWRITETYPELISTAPPEND, "4test");
  writefirstline(filename, READFILEWRITETYPELISTAPPEND, "test5");
  writefirstline(filename, READFILEWRITETYPELISTREMOVE, "test1");
  writefirstline(filename, READFILEWRITETYPELISTREMOVE, "test2");
  writefirstline(filename, READFILEWRITETYPELISTREMOVE, "4test");
  writefirstline(filename, READFILEWRITETYPELISTREMOVE, "test5");
  writefirstline(filename, READFILEWRITETYPELISTREMOVE, "test5");
  writefirstline(filename, READFILEWRITETYPELISTREMOVE, "test5");
  writefirstline(filename, READFILEWRITETYPELISTREMOVE, "test5");
  writefirstline(filename, READFILEWRITETYPELISTREMOVE, "test5");
  writefirstline(filename, READFILEWRITETYPELISTREMOVE, "test5");
  writefirstline(filename, READFILEWRITETYPELISTREMOVE, "test5");
  return 0;
}
