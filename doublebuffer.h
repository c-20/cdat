#define MAXDOUBLEBUFFERS			100
#define MAXDOUBLEBUFFERNAMELENGTH	64
#define DOUBLEBUFFERWAITTIME		2000 // 20000
#define DOUBLEBUFFERSEEKTIME		100 // 1000
#define MAXCHANNELS				32 //	16
#define MAXSIZEPERCHANNEL			340 // 120
//50000
#define DOUBLEBUFFERNAMEPREFIX		"/dev/shm/page0/buffer-"
// 012345678901234567890
// S[AA]F[    0]B[    0]
//   ^^   ^       ^ 2,3,7,15
#define DOUBLEBUFFERWRITELOC		2
#define DOUBLEBUFFERREADLOC			3
#define DOUBLEBUFFERFRONTLOC		7
#define DOUBLEBUFFERBACKLOC			15

FILE *bufferds[MAXDOUBLEBUFFERS];
FILE *bufferdf[MAXDOUBLEBUFFERS];
FILE *bufferdb[MAXDOUBLEBUFFERS];

int doublebufferconnectforwriting(int index) {
  char filename[MAXDOUBLEBUFFERNAMELENGTH];
  // open state for read/write, front for write, back for write
  sprintf(filename, "%s%d-state", DOUBLEBUFFERNAMEPREFIX, index);
  if (!(bufferds[index] = fopen(filename, "w+"))) return 0;
  sprintf(filename, "%s%d-front", DOUBLEBUFFERNAMEPREFIX, index);
  if (!(bufferdf[index] = fopen(filename, "w"))) return 0;
  sprintf(filename, "%s%d-back", DOUBLEBUFFERNAMEPREFIX, index);
  if (!(bufferdb[index] = fopen(filename, "w"))) return 0;
  // apply ready state (reader syncs by waiting for this)
  fseek(bufferds[index], 0, SEEK_SET);
  fprintf(bufferds[index], "S[AA]F[    0]B[    0]");
  fflush(bufferds[index]);
  return 1;
}

#include <errno.h>
int doublebufferconnectforreading(int index) {
  char filename[MAXDOUBLEBUFFERNAMELENGTH];
  // open state for read/write, front for read, back for read
  sprintf(filename, "%s%d-state", DOUBLEBUFFERNAMEPREFIX, index);
  if (!(bufferds[index] = fopen(filename, "r+"))) {
    fprintf(stderr, "failure: %s\n", strerror(errno));
    return 0; }
  sprintf(filename, "%s%d-front", DOUBLEBUFFERNAMEPREFIX, index);
  if (!(bufferdf[index] = fopen(filename, "r"))) return 0;
  sprintf(filename, "%s%d-back", DOUBLEBUFFERNAMEPREFIX, index);
  if (!(bufferdb[index] = fopen(filename, "r"))) return 0;
  return 1;
}

//////////////// not sure if useful:
int doublebuffersyncreader(int index) {
  int insync = 0;
  while (!insync) {
    fseek(bufferds[index], 0, SEEK_SET);
    if (fgetc(bufferds[index]) == (int)'S') {
      if (fgetc(bufferds[index]) == (int)'[') {
        if (fgetc(bufferds[index]) == (int)'A') {
          if (fgetc(bufferds[index]) == (int)'A') {
            if (fgetc(bufferds[index]) == (int)']') {
              insync = 1;
              break;
            }
          }
        }
      }
    }
    usleep(DOUBLEBUFFERWAITTIME);
  }
  return insync;
}

char doublebufferwritefloatarray(int index, float *src, int len) {
  char writestate = '?';
  char readstate = '?';
  int frontlen = 0;
  int backlen = 0;
  int inch;
  char ch;
  // extract existing state information
  fseek(bufferds[index], 0, SEEK_SET);
  if (fgetc(bufferds[index]) != (int)'S') { ERT '1'; }
  if (fgetc(bufferds[index]) != (int)'[') { ERT '2'; }
  writestate = (char)fgetc(bufferds[index]);
  readstate = (char)fgetc(bufferds[index]);
  if (fgetc(bufferds[index]) != (int)']') { ERT '3'; }
  if (fgetc(bufferds[index]) != (int)'F') { ERT '4'; }
  if (fgetc(bufferds[index]) != (int)'[') { ERT '5'; }
  ch = '[';
  while (ch != ']') {
    inch = fgetc(bufferds[index]);
    if (inch == EOF) return '\0';
    ch = (char)inch;
    if (ch >= '0' && ch <= '9') {
      frontlen = (frontlen * 10) + (int)(ch - '0');
    }
  }
  if (fgetc(bufferds[index]) != (int)'B') { ERT '6'; }
  if (fgetc(bufferds[index]) != (int)'[') { ERT '7'; }
  ch = '[';
  while (ch != ']') {
    inch = fgetc(bufferds[index]);
    if (inch == EOF) return '\0';
    ch = (char)inch;
    if (ch >= '0' && ch <= '9') {
      backlen = (backlen * 10) + (int)(ch - '0');
    }
  }
  // perform appropriate operation
  if (writestate == 'A') {
    // ready - fill front buffer
    fseek(bufferdf[index], 0, SEEK_SET);
YLOG1("%d",
    fwrite(src, sizeof(float), len, bufferdf[index]) );
    fflush(bufferdf[index]);
    // rewrite front size
    fseek(bufferds[index], DOUBLEBUFFERFRONTLOC, SEEK_SET);
    fprintf(bufferds[index], "%5d", len);
    // apply new state (A -> B)
    fseek(bufferds[index], DOUBLEBUFFERWRITELOC, SEEK_SET);
    fputc('B', bufferds[index]);
    fflush(bufferds[index]);
  } else if (writestate == 'B') {
    // front filled - fill back
    fseek(bufferdb[index], 0, SEEK_SET);
//GLOG1("%d",
    fwrite(src, sizeof(float), len, bufferdb[index]); // );
    fflush(bufferdb[index]);
    // rewrite back size
    fseek(bufferds[index], DOUBLEBUFFERBACKLOC, SEEK_SET);
    fprintf(bufferds[index], "%5d", len);
    // apply new state (B -> C)
    fseek(bufferds[index], DOUBLEBUFFERWRITELOC, SEEK_SET);
    fputc('C', bufferds[index]);
    fflush(bufferds[index]);
  } EF (writestate == 'C') {
    // front and back filled - wait for front to be read
//    usleep(DOUBLEBUFFERWAITTIME);
    WI (readstate != 'B') {
//      usleep(DOUBLEBUFFERSEEKTIME);
      fflush(bufferds[index]);
      fseek(bufferds[index], DOUBLEBUFFERREADLOC, SEEK_SET);
      readstate = (char)fgetc(bufferds[index]);
    }
    // ready - fill front buffer
    fseek(bufferdf[index], 0, SEEK_SET);
// WLOG2("F%d:%d", len,
    fwrite(src, sizeof(float), len, bufferdf[index]); // );
    fflush(bufferdf[index]);
    // rewrite front size
    fseek(bufferds[index], DOUBLEBUFFERFRONTLOC, SEEK_SET);
    fprintf(bufferds[index], "%5d", len);
    // apply new state (C -> D)
    fseek(bufferds[index], DOUBLEBUFFERWRITELOC, SEEK_SET);
    fputc('D', bufferds[index]);
    fflush(bufferds[index]);
  } EF (writestate == 'D') {
    // back and front filled - wait for back to be read
//    usleep(DOUBLEBUFFERWAITTIME);
    WI (readstate != 'A') {
//      usleep(DOUBLEBUFFERSEEKTIME);
      fflush(bufferds[index]);
      fseek(bufferds[index], DOUBLEBUFFERREADLOC, SEEK_SET);
      readstate = (char)fgetc(bufferds[index]);
    }
    // ready - fill back buffer
    fseek(bufferdb[index], 0, SEEK_SET);
// MLOG2("B%d:%d", len,
    fwrite(src, sizeof(float), len, bufferdb[index]); // );
// RLOG2(">B%d*%d", sizeof(float), len);
    fflush(bufferdb[index]);
    // rewrite back size
    fseek(bufferds[index], DOUBLEBUFFERBACKLOC, SEEK_SET);
    fprintf(bufferds[index], "%5d", len);
    // apply new state (D -> C)
    fseek(bufferds[index], DOUBLEBUFFERWRITELOC, SEEK_SET);
// WLOG("->C");
    fputc('C', bufferds[index]);
    fflush(bufferds[index]);
  } EL { RT '\0'; }
  RT writestate;
}

//int doublebufferreadfailcount = 0;
//int doublebufferreadfailmax = 10;
char doublebufferreadfloatarray(int index, float *dest, int *len, int lastread) {
  char writestate = '?';
  char readstate = '?';
  int frontlen = 0;
  int backlen = 0;
  int inch;
  char ch;
  // extract existing state information
  // note that front/back will be re-read
  fseek(bufferds[index], 0, SEEK_SET);
  if (fgetc(bufferds[index]) != (int)'S') return '\0';
  if (fgetc(bufferds[index]) != (int)'[') return '\0';
  writestate = (char)fgetc(bufferds[index]);
  readstate = (char)fgetc(bufferds[index]);
  if (fgetc(bufferds[index]) != (int)']') return '\0';
  if (fgetc(bufferds[index]) != (int)'F') return '\0';
  if (fgetc(bufferds[index]) != (int)'[') return '\0';
  ch = '[';
  while (ch != ']') {
    inch = fgetc(bufferds[index]);
    if (inch == EOF) return '\0';
    ch = (char)inch;
    if (ch >= '0' && ch <= '9') {
      frontlen = (frontlen * 10) + (int)(ch - '0');
    }
  }
  if (fgetc(bufferds[index]) != (int)'B') return '\0';
  if (fgetc(bufferds[index]) != (int)'[') return '\0';
  ch = '[';
  while (ch != ']') {
    inch = fgetc(bufferds[index]);
    if (inch == EOF) return '\0';
    ch = (char)inch;
    if (ch >= '0' && ch <= '9') {
      backlen = (backlen * 10) + (int)(ch - '0');
    }
  }
  // perform appropriate operation
  int numyields = 0;
  int maxyields = 1000;
  if (readstate == 'A') {
    // wait sure writer is active
    while (writestate != 'C') {
//      usleep(DOUBLEBUFFERSEEKTIME);
      sched_yield();
      fflush(bufferds[index]);
      fseek(bufferds[index], DOUBLEBUFFERWRITELOC, SEEK_SET);
      writestate = (char)fgetc(bufferds[index]);
      numyields++;
      // if too many read failures, swap state
      if (numyields > maxyields && lastread) {
        fseek(bufferds[index], DOUBLEBUFFERREADLOC, SEEK_SET);
        fputc('B', bufferds[index]);
        fflush(bufferds[index]);
        return readstate;
      }
    }
    // re-read front buffer length
    fseek(bufferds[index], DOUBLEBUFFERFRONTLOC, SEEK_SET);
    frontlen = 0;
    ch = '[';
    while (ch != ']') {
      inch = fgetc(bufferds[index]);
      if (inch == EOF) return '\0';
      ch = (char)inch;
      if (ch >= '0' && ch <= '9') {
        frontlen = (frontlen * 10) + (int)(ch - '0');
      }
    }
    *len = frontlen;
    // read front buffer
    if (dest != NULL) {
      fflush(bufferdf[index]);
      fseek(bufferdf[index], 0, SEEK_SET);
      if (frontlen > 0) {
        fread(dest, sizeof(float), frontlen, bufferdf[index]);
      }
      // apply new state (A -> B)
      if (lastread) {
        fseek(bufferds[index], DOUBLEBUFFERREADLOC, SEEK_SET);
        fputc('B', bufferds[index]);
        fflush(bufferds[index]);
      }
    }
  } else if (readstate == 'B') {
    // make sure writer is active
    while (writestate != 'D') {
//      usleep(DOUBLEBUFFERSEEKTIME);
      sched_yield();
      fflush(bufferds[index]);
      fseek(bufferds[index], DOUBLEBUFFERWRITELOC, SEEK_SET);
      writestate = (char)fgetc(bufferds[index]);
      numyields++;
      // if too many read failures, swap state
      if (numyields > maxyields && lastread) {
        fseek(bufferds[index], DOUBLEBUFFERREADLOC, SEEK_SET);
        fputc('A', bufferds[index]);
        fflush(bufferds[index]);
        return readstate;
      }
    }
    // re-read back buffer length
    fseek(bufferds[index], DOUBLEBUFFERBACKLOC, SEEK_SET);
    backlen = 0;
    ch = '[';
    while (ch != ']') {
      inch = fgetc(bufferds[index]);
      if (inch == EOF) return '\0';
      ch = (char)inch;
      if (ch >= '0' && ch <= '9') {
        backlen = (backlen * 10) + (int)(ch - '0');
      }
    }
    *len = backlen;
    // read back buffer (if requested)
    if (dest != NULL) {
      fflush(bufferdb[index]);
      fseek(bufferdb[index], 0, SEEK_SET);
      if (backlen > 0) {
        fread(dest, sizeof(float), backlen, bufferdb[index]);
      }
      // apply new state (B -> A)
      if (lastread) {
        fseek(bufferds[index], DOUBLEBUFFERREADLOC, SEEK_SET);
        fputc('A', bufferds[index]);
        fflush(bufferds[index]);
      }
    }
  } else return '\0';
/*
  } else {
    if (lastread) {
      if (doublebufferreadfailcount < doublebufferreadfailmax) {
        doublebufferreadfailcount++;
      } else {
        doublebufferreadfailcount = 0;
        if (
    return '\0';
  }
*/
  return readstate;
}

char doublebufferreadfloatsubarray(int index, int i, int n, float *dest, int *len, int lastread) {
  float destbox[MAXCHANNELS * MAXSIZEPERCHANNEL];
  int newlen;
  char readstate = doublebufferreadfloatarray(index, destbox, &newlen, lastread);
  newlen /= n;
  int avi = 0;
  while (avi < newlen) {
    dest[avi] = destbox[(avi * n) + (i - 1)];
    avi++;
  }
  *len = newlen;
  return readstate;
}

void doublebufferdisconnect(int index) {
  fseek(bufferds[index], DOUBLEBUFFERWRITELOC, SEEK_SET);
  fputc('X', bufferds[index]);
  fseek(bufferds[index], DOUBLEBUFFERREADLOC, SEEK_SET);
  fputc('X', bufferds[index]);
  fflush(bufferds[index]);
  fclose(bufferdb[index]);
  fclose(bufferdf[index]);
  fclose(bufferds[index]);
}

void doublebufferdisconnectgracefully(int index) {
  fseek(bufferds[index], DOUBLEBUFFERREADLOC, SEEK_SET);
  fputc('X', bufferds[index]);
  fflush(bufferds[index]);
  fclose(bufferdb[index]);
  fclose(bufferdf[index]);
  fclose(bufferds[index]);
}

