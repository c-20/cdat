#define MAXINDEXBUFFERS				8
#define MAXINDEXBUFFERNAMELENGTH	64
#define INDEXBUFFERWAITTIME			20000
#define INDEXBUFFERSEEKTIME			1000
//50000
#define INDEXBUFFERNAMEPREFIX		"/dev/shm/page0/buffer-"
// 012345678901234567890
// S[AA]F[    0]B[    0]
//   ^^   ^       ^ 2,3,7,15
#define INDEXBUFFERWRITELOC			2
#define INDEXBUFFERREADLOC			3
#define INDEXBUFFERFRONTLOC			7
#define INDEXBUFFERBACKLOC			15

//#define INDEXMAXMAXWIDTH			16
//#define INDEXMAXLENGTH			16
#define INDEXBUFFERMAXINDEX				99999
#define INDEXBUFFERMAXSTRINGLENGTH		64

FILE *indexbufferds[MAXINDEXBUFFERS];
FILE *indexbufferdl[MAXINDEXBUFFERS];

int indexbufferconnectforwriting(int index) {
  char filename[MAXINDEXBUFFERNAMELENGTH];
  // open state for read/write, front for write, back for write
  sprintf(filename, "%s%d-state", INDEXBUFFERNAMEPREFIX, index);
  if (!(indexbufferds[index] = fopen(filename, "w+"))) _exit(1);
  sprintf(filename, "%s%d-value", INDEXBUFFERNAMEPREFIX, index);
  if (!(indexbufferdl[index] = fopen(filename, "w"))) _exit(2);
  // apply ready state (reader syncs by waiting for this)
  fseek(indexbufferds[index], 0, SEEK_SET);
  fprintf(indexbufferds[index], "S[AA]I[    0]L[    0]");
  fflush(indexbufferds[index]);
  return 1;
}

int indexbufferconnectforreading(int index) {
  char filename[MAXINDEXBUFFERNAMELENGTH];
  // open state for read/write, front for read, back for read
  sprintf(filename, "%s%d-state", INDEXBUFFERNAMEPREFIX, index);
  if (!(indexbufferds[index] = fopen(filename, "r+"))) _exit(3);
  sprintf(filename, "%s%d-value", INDEXBUFFERNAMEPREFIX, index);
  if (!(indexbufferdl[index] = fopen(filename, "r"))) _exit(4);
  return 1;
}

// this needs to check the write state and act accordingly -
// - if A, front fill
// - if B, back fill
// - if C, wait front read, front fill
// - if D, wait back read, back fill
// - if read front, set state B (if C, read front, set B)
// - if read back, set state A (if D, read back, set A)
int indexbuffersyncreader(int index) {
  int insync = 0;
  while (!insync) {
    fseek(indexbufferds[index], 0, SEEK_SET);
    if (fgetc(indexbufferds[index]) == (int)'S') {
      if (fgetc(indexbufferds[index]) == (int)'[') {
        if (fgetc(indexbufferds[index]) == (int)'A') {
          if (fgetc(indexbufferds[index]) == (int)'A') {
            if (fgetc(indexbufferds[index]) == (int)']') {
              insync = 1;
              break;
            }
          }
        }
      }
    }
    usleep(INDEXBUFFERWAITTIME);
  }
  return insync;
}

//char indexbufferwriteintarray(int index, int *src, int len) {
char indexbufferwriteintstringpair(int index, int valueindex, char *value) {
  char fixedvalue[INDEXBUFFERMAXSTRINGLENGTH + 1];
  int valuelen = 0;
  int fvi = 0;
  while (fvi < INDEXBUFFERMAXSTRINGLENGTH) {
    if ((fvi > 0 && fixedvalue[fvi - 1] == '\0') || value[fvi] == '\0') {
      fixedvalue[fvi] = '\0';
      if (fvi == 0 || fixedvalue[fvi - 1] != '\0') {
        valuelen = fvi;
      }
    } else {
      fixedvalue[fvi] = value[fvi];
    }
    fvi++;
  }
  fixedvalue[fvi] = '\0';
//  char indexbuffer[INDEXMAXMAXWIDTH * INDEXMAXLENGTH];
  char writestate = '?';
  char readstate = '?';
  int frontlen = 0;
  int backlen = 0;
  int inch;
  char ch;
  // extract existing state information
  fseek(indexbufferds[index], 0, SEEK_SET);
  if (fgetc(indexbufferds[index]) != (int)'S') return '\0';
  if (fgetc(indexbufferds[index]) != (int)'[') return '\0';
  writestate = (char)fgetc(indexbufferds[index]);
  readstate = (char)fgetc(indexbufferds[index]);
  if (fgetc(indexbufferds[index]) != (int)']') return '\0';
  if (fgetc(indexbufferds[index]) != (int)'I') return '\0';
  if (fgetc(indexbufferds[index]) != (int)'[') return '\0';
  ch = '[';
  while (ch != ']') {
    inch = fgetc(indexbufferds[index]);
    if (inch == EOF) return '\0';
    ch = (char)inch;
    if (ch >= '0' && ch <= '9') {
      frontlen = (frontlen * 10) + (int)(ch - '0');
    }
  }
  if (fgetc(indexbufferds[index]) != (int)'L') return '\0';
  if (fgetc(indexbufferds[index]) != (int)'[') return '\0';
  ch = '[';
  while (ch != ']') {
    inch = fgetc(indexbufferds[index]);
    if (inch == EOF) return '\0';
    ch = (char)inch;
    if (ch >= '0' && ch <= '9') {
      backlen = (backlen * 10) + (int)(ch - '0');
    }
  }
  // find maximum width in array
  int min = 0;
  int max = 0;
  int avi = 0;
/*
  int maxwidth = 1;
  while (avi < len) {
    if (src[avi] > max) {
      max = src[avi];
    }
    if (src[avi] < min) {
      min = src[avi];
    }
    avi++;
  }
  if (min < 0 && (min * -1) > (max / 10)) {
    maxwidth++;
    max = min * -1;
  } else if (min == 0 && max == 0) {
    max = 1;
  }
  while (max > 0) {
    max /= 10;
    maxwidth++;
  }
*/
/*
  int maxwidth = 16;
  // prepare live buffer
  char formatstring[16];
  char *indexbufferp = &indexbuffer[0];
  sprintf(formatstring, "%%%dd", maxwidth);
  avi = 0;
  while (avi < len) {
    sprintf(indexbufferp, formatstring, src[avi]);
    indexbufferp += maxwidth;
    avi++;
  }
*/

//instead of indexbufferp, write fixedvalue

  // perform appropriate operation
  if (writestate == 'A') {
    // indicate writing by making state lowercase
    fseek(indexbufferds[index], INDEXBUFFERWRITELOC, SEEK_SET);
    fputc('a', indexbufferds[index]);
    fflush(indexbufferds[index]);
    // ready - fill front buffer
    fseek(indexbufferdl[index], 0, SEEK_SET);
//    fwrite(src, sizeof(float), len, bufferdl[index]);
//    fwrite(indexbuffer, maxwidth, len, indexbufferdl[index]);
    fwrite(fixedvalue, 1, INDEXBUFFERMAXSTRINGLENGTH, indexbufferdl[index]);
//    fprintf(stderr, "fixedvalue: %s\n", value);
//    fprintf(indexbufferdl[index], "   ");
    fflush(indexbufferdl[index]);
    // rewrite front size - contains index value
    fseek(indexbufferds[index], INDEXBUFFERFRONTLOC, SEEK_SET);
    fprintf(indexbufferds[index], "%5d", valueindex);
    // apply new state (A -> B)
//    fseek(bufferds[index], LIVEBUFFERWRITELOC, SEEK_SET);
//    fputc('B', bufferds[index]);
//    fflush(bufferds[index]);
//  } else if (writestate == 'B') {
    // front filled - fill back
//    fseek(bufferdb[index], 0, SEEK_SET);
//    fwrite(src, sizeof(float), len, bufferdb[index]);
//    fflush(bufferdb[index]);
    // rewrite back size - contains string length
    fseek(indexbufferds[index], INDEXBUFFERBACKLOC, SEEK_SET);
    fprintf(indexbufferds[index], "%5d", valuelen);//len);
    // indicate writing is complete by making state uppercase
    fseek(indexbufferds[index], INDEXBUFFERWRITELOC, SEEK_SET);
    fputc('A', indexbufferds[index]);
    fflush(indexbufferds[index]);
    // apply new state (B -> C)
//    fseek(bufferds[index], LIVEBUFFERWRITELOC, SEEK_SET);
//    fputc('C', bufferds[index]);
//    fflush(bufferds[index]);
//  } else if (writestate == 'C') {
    // front and back filled - wait for front to be read
//    usleep(DOUBLEBUFFERWAITTIME);
//    while (readstate != 'B') {
//      usleep(LIVEBUFFERSEEKTIME);
//      fflush(bufferds[index]);
//      fseek(bufferds[index], LIVEBUFFERREADLOC, SEEK_SET);
//      readstate = (char)fgetc(bufferds[index]);
//    }
    // ready - fill front buffer
//    fseek(bufferdl[index], 0, SEEK_SET);
//    fwrite(src, sizeof(float), len, bufferdl[index]);
//    fflush(bufferdl[index]);
    // rewrite front size
//    fseek(bufferds[index], LIVEBUFFERFRONTLOC, SEEK_SET);
//    fprintf(bufferds[index], "%5d", len);
    // apply new state (C -> D)
//    fseek(bufferds[index], LIVEBUFFERWRITELOC, SEEK_SET);
//    fputc('D', bufferds[index]);
//    fflush(bufferds[index]);
//  } else if (writestate == 'D') {
    // back and front filled - wait for back to be read
//    usleep(DOUBLEBUFFERWAITTIME);
//    while (readstate != 'A') {
//      usleep(LIVEBUFFERSEEKTIME);
//      fflush(bufferds[index]);
//      fseek(bufferds[index], LIVEBUFFERREADLOC, SEEK_SET);
//      readstate = (char)fgetc(bufferds[index]);
//    }
    // ready - fill back buffer
//    fseek(bufferdb[index], 0, SEEK_SET);
//    fwrite(src, sizeof(float), len, bufferdb[index]);
//    fflush(bufferdb[index]);
    // rewrite back size
//    fseek(bufferds[index], LIVEBUFFERBACKLOC, SEEK_SET);
//    fprintf(bufferds[index], "%5d", len);
    // apply new state (D -> C)
//    fseek(bufferds[index], LIVEBUFFERWRITELOC, SEEK_SET);
//    fputc('C', bufferds[index]);
//    fflush(bufferds[index]);
  } else return '\0';
  return writestate;
}

int indexbufferreadfailcount = 0;
int indexbufferreadfailmax = 10;
char indexbufferreadintstringpair(int index, int *valueindex, char *fixedvalue, int *valuelen) {
  char writestate = '?';
  char readstate = '?';
  int firstvalue = 0;
  int secondvalue = 0;
  int inch;
  char ch;
  // extract existing state information
  // note that front/back will be re-read
  fseek(indexbufferds[index], 0, SEEK_SET);
  if (fgetc(indexbufferds[index]) != (int)'S') return '\0';
  if (fgetc(indexbufferds[index]) != (int)'[') return '\0';
  writestate = (char)fgetc(indexbufferds[index]);
  readstate = (char)fgetc(indexbufferds[index]);
  if (fgetc(indexbufferds[index]) != (int)']') return '\0';
  if (fgetc(indexbufferds[index]) != (int)'I') return '\0';
  if (fgetc(indexbufferds[index]) != (int)'[') return '\0';
  ch = '[';
  while (ch != ']') {
    inch = fgetc(indexbufferds[index]);
    if (inch == EOF) return '\0';
    ch = (char)inch;
    if (ch >= '0' && ch <= '9') {
      firstvalue = (firstvalue * 10) + (int)(ch - '0');
    }
  }
  if (fgetc(indexbufferds[index]) != (int)'L') return '\0';
  if (fgetc(indexbufferds[index]) != (int)'[') return '\0';
  ch = '[';
  while (ch != ']') {
    inch = fgetc(indexbufferds[index]);
    if (inch == EOF) return '\0';
    ch = (char)inch;
    if (ch >= '0' && ch <= '9') {
      secondvalue = (secondvalue * 10) + (int)(ch - '0');
    }
  }
  // perform appropriate operation
  if (readstate == 'A' || readstate == 'B' || readstate == 'X') {
    while (writestate != 'A') {
      sched_yield();
      fflush(indexbufferds[index]);
      fseek(indexbufferds[index], INDEXBUFFERWRITELOC, SEEK_SET);
      writestate = (char)fgetc(indexbufferds[index]);
    }
    // read front buffer
    if (fixedvalue != NULL) {
      fflush(indexbufferdl[index]);
      fseek(indexbufferdl[index], 0, SEEK_SET);
      fread(fixedvalue, 1, secondvalue, indexbufferdl[index]);
      fixedvalue[secondvalue] = '\0';
      *valuelen = secondvalue;
      *valueindex = firstvalue;
      // apply new state (A -> B)
//      if (lastread) {
      fseek(indexbufferds[index], INDEXBUFFERREADLOC, SEEK_SET);
      fputc('B', indexbufferds[index]);
      fflush(indexbufferds[index]);
//      }
    }
/*
  } else if (readstate == 'B') {
//  fprintf(stdout, "B");
//  fflush(stdout);
    // make sure writer is active
//    while (writestate != 'C' && writestate != 'D') {
    while (writestate != 'D') {
      usleep(LIVEBUFFERSEEKTIME);
      fflush(bufferds[index]);
      fseek(bufferds[index], LIVEBUFFERWRITELOC, SEEK_SET);
      writestate = (char)fgetc(bufferds[index]);
    }
    // re-read back buffer length
    fseek(bufferds[index], LIVEBUFFERBACKLOC, SEEK_SET);
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
//      fflush(bufferdb[index]);
//      fseek(bufferdb[index], 0, SEEK_SET);
//      if (backlen > 0) {
//        fread(dest, sizeof(float), backlen, bufferdb[index]);
//      }
      // apply new state (B -> A)
      if (lastread) {
        fseek(bufferds[index], LIVEBUFFERREADLOC, SEEK_SET);
        fputc('A', bufferds[index]);
        fflush(bufferds[index]);
      }
    }
*/
  } else {
    if (indexbufferreadfailcount < indexbufferreadfailmax) {
      fprintf(stdout, "\033[0;31m!\033[0m");
      indexbufferreadfailcount++;
      fflush(stdout);
    }
    return '\0';
  }
  return readstate;
}
/*
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
*/
void indexbufferdisconnect(int index) {
  fseek(indexbufferds[index], INDEXBUFFERWRITELOC, SEEK_SET);
  fputc('X', indexbufferds[index]);
  fseek(indexbufferds[index], INDEXBUFFERREADLOC, SEEK_SET);
  fputc('X', indexbufferds[index]);
  fflush(indexbufferds[index]);
  fclose(indexbufferdl[index]);
  fclose(indexbufferds[index]);
}

void indexbufferdisconnectgracefully(int index) {
  fseek(indexbufferds[index], INDEXBUFFERREADLOC, SEEK_SET);
  fputc('X', indexbufferds[index]);
  fflush(indexbufferds[index]);
  fclose(indexbufferdl[index]);
  fclose(indexbufferds[index]);
}

