#define HOLDMAXLINEPOINTERS		512 // 256
#define HOLDCIRCULARBUFFERSIZE		65534
// 200 x 256 = 51,200

#ifndef INBOXDEFINITION
#include <stdio.h>

int main(int argc, char **argv) {
#else
#ifndef INBOXINOUTVARS
#define INBOXINOUTVARS
//char *inboxinstream;
//char *inboxoutstream;
char inboxinstream[60000];
char inboxoutstream[60000];
int inboxinstreami;
int inboxoutstreami;
#endif
#ifndef INBOXREADWRITE
#define INBOXREADWRITE
int inboxwritecharout(char ch) {
  // stdout overwrites stdin ; sync issue returns -1
  if (inboxinstream == inboxoutstream && inboxoutstreami >= inboxinstreami) {
    inboxoutstream[inboxinstreami - 1] = '#';
    inboxoutstream[inboxinstreami] = '\0';
    return -1;
  }
  YLOG1("%d", inboxoutstreami);
  inboxoutstream[inboxoutstreami] = ch;
  inboxoutstreami++;
  return 0;
}
int inboxwritestrout(char *str) {
  int avi = 0;
  while (str[avi] != '\0') {
    if (inboxwritecharout(str[avi]) == -1) {
      return -1;
    }
    avi++;
  }
  return 0;
}
int inboxreadcharin() {
  char ch = inboxinstream[inboxinstreami];
  inboxinstreami++;
  if (ch == '\0') {
    return EOF;
  } else {
    return (int)ch;
  }
}
#endif
int inboxholdmain(int argc, char **argv) {
#endif
  char *linepointers[HOLDMAXLINEPOINTERS];
  int linepointeri = 0;
  char *lp = linepointers[0];
  char cbuffer[HOLDCIRCULARBUFFERSIZE + 2];
  cbuffer[HOLDCIRCULARBUFFERSIZE] = '\b';
  cbuffer[HOLDCIRCULARBUFFERSIZE + 1] = '\0';
  linepointers[0] = &cbuffer[0];
  linepointeri++;
  int cbufferi = 0;
  int lines = 0;
  int lii = 0, lij = 0;
  if (argc > 1) {
    while (argv[1][lii] >= '0' && argv[1][lii] <= '9') {
      lines = (lines * 10) + (int)(argv[1][lii] - '0');
      lii++;
    }
  }
  if (lines < 1) {
    lines = 1;
  }
  char *refreshtrigger = "\r!";
  char *endfiletrigger = "\r.";
  char state = '-';
  int counter = 0;
  char ch, lch;
  int inch = EOF;
#ifndef INBOXDEFINITION
  while (inch = getchar()) {
#else
  while (inch = inboxreadcharin()) {
#endif
    if (inch == EOF) break;
    ch = (char)inch;
    cbuffer[cbufferi] = ch;
    cbufferi++;
    if (cbufferi >= HOLDCIRCULARBUFFERSIZE) {
      cbuffer[HOLDCIRCULARBUFFERSIZE] = '\b';
      cbufferi = 0;
    }
    cbuffer[cbufferi] = '\0';
CLOG1("cbufferi=%d", cbufferi);
//    if (state == '-' && (ch == '\n' || ch == '\r')) {
    if (state == '-' && ch == '\n') {
      linepointers[linepointeri] = &cbuffer[cbufferi];
      if (linepointeri > lines && ch == '\n') {
        lii = 0;
        while (lii <= lines) {
          linepointers[lii] = linepointers[lii + 1];
          lii++;
        }
        linepointeri--;
//      } else if (ch == '\r') {
//        linepointeri = 0;
      }
#ifndef INBOXDEFINITION
      fprintf(stdout, "%s", refreshtrigger);
#else
      if (inboxwritestrout(refreshtrigger) == -1) return -1;
#endif
      lii = 0;
      while (lii < linepointeri && lii <= lines) {
        lp = linepointers[lii];
        lij = 0;
        if (lp[0] != '\0') {
          lch = '\b';
          while (lch != '\0') {
//          while ((lch = lp[lij % HOLDCIRCULARBUFFERSIZE]) != '\n') {
            lch = lp[lij];
            if (lch == '\0') break;
            if (lch == '\n') break;
            if (lch == '\b') {
              lp = &cbuffer[0];
              lij = 0;
            } else {
#ifndef INBOXDEFINITION
              fprintf(stdout, "%c", lch);
#else
              if (inboxwritecharout(lch) == -1) return -1;
#endif
              lij++;
            }
          }
#ifndef INBOXDEFINITION
          fprintf(stdout, "\n");
#else
          if (inboxwritecharout('\n') == -1) return -1;
#endif
        }
        lii++;
      }
      linepointeri++;
    }
  }
  // send EOF to SEA
#ifndef INBOXDEFINITION
  fprintf(stdout, "%s", endfiletrigger);
#else
  if (inboxwritestrout(endfiletrigger) == -1) return -1;
#endif
  return 0;
}
