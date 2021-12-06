#define READFILEMAXLINELENGTH			65534 // 8192

#define READFILEWRITETYPELISTSINGLE     1
#define READFILEWRITETYPELISTAPPEND	    2
#define READFILEWRITETYPELISTREMOVE	    3
#define READFILEWRITETYPELISTCLEAR	    4
// read the first line of source (path) into target (buffer) and return length
int rereadfirstline(FILE *source, char *target) {
  int fcinch;
  int fci = 0;
  fflush(source);
  fseek(source, 0, SEEK_SET);
  while (1) {
    fcinch = fgetc(source);
    if (fcinch == EOF || fcinch == '\n' || fcinch == '\0') {
      target[fci] = '\0';
      break;
    }
    target[fci] = (char)fcinch;
    fci++;
  }
  return fci;
}
int readfirstline(char *source, char *target) {
  FILE *fc = fopen(source, "r");
  int length = rereadfirstline(fc, target);
  fclose(fc);
  return length;
}
int writefirstline(char *source, int writetype, char *target) {
  char writebuffer[READFILEMAXLINELENGTH];
  char *writebufferp = &writebuffer[0];
  int writebufferlen = 0;
  int writesuccess = 0;
  FILE *fc = NULL;//fopen(source, "r+");
  //if (!fc) fc = fopen(source, "w+");
  //if (!fc) return -1;
  if (writetype == READFILEWRITETYPELISTCLEAR) {
    fc = fopen(source, "w");
    fprintf(fc, "\n");
    fflush(fc);
    fclose(fc);
    writesuccess = 1;
  } else if (writetype == READFILEWRITETYPELISTSINGLE) {
    fc = fopen(source, "w");
    fprintf(fc, "%s\n", target);
    fflush(fc);
    fclose(fc);
    writesuccess = 1;
  } else if (writetype == READFILEWRITETYPELISTAPPEND) {
    int wbi = 0;
    writebufferlen = readfirstline(source, writebuffer);
MLOG1("writebufferlen %d", writebufferlen);
    fc = fopen(source, "w");
    if (writebufferlen == 0) {
      fprintf(fc, "%s\n", target);
    } else {
      fprintf(fc, "%s, %s\n", writebuffer, target);
    }
    fflush(fc);
    fclose(fc);
    writesuccess = 1;
  } else if (writetype == READFILEWRITETYPELISTREMOVE) {
    writebufferlen = readfirstline(source, writebuffer);
    int wbj = 0;
    while (wbj < writebufferlen) {
      if (wbj == 0 || (wbj > 1 && writebuffer[wbj - 2] == ',' && writebuffer[wbj - 1] == ' ')) {
        int wbk = 0;
        int wbkmatch = 1;
        while (writebuffer[wbj + wbk] != '\0') {
          if (writebuffer[wbj + wbk] == ',') break;
          if (writebuffer[wbj + wbk] == '\n') break;
          if (target[wbk] == '\0' || target[wbk] != writebuffer[wbj + wbk]) {
            wbkmatch = 0;
            break;
          }
          wbk++;
        }
        if (writebuffer[wbj + wbk] == '\0' && target[wbk] != '\0') {
          wbkmatch = 0;
        }
        // wbj = start index of target object
        // wbk = length of target object to remove
        if (wbkmatch == 1) {
          if (writebuffer[wbj + wbk] == '\n' || writebuffer[wbj + wbk] == '\0') {
            fc = fopen(source, "w");
            if (wbj == 0) {
              // remove first/last (only) element
              fprintf(fc, "\n");
            } else {
              // remove last (not only) element
              writebuffer[wbj - 2] = '\0';
              fprintf(fc, "%s\n", writebuffer);
            }
            fflush(fc);
            fclose(fc);
          } else if (wbj == 0) {
            // remove first (not only) element
            writebufferp = &writebuffer[wbk + 2];
            fc = fopen(source, "w");
            fprintf(fc, "%s\n", writebufferp);
            fflush(fc);
            fclose(fc);
          } else if (writebuffer[wbj + wbk] == ',') {
            // remove middle element
            int wbl = wbj - 3; // -(, ) - 1
            while (wbl >= 0) {
              writebuffer[wbl + wbk + 2] = writebuffer[wbl];
              wbl--;
            }
            writebufferp = &writebuffer[wbk + 2];
            fc = fopen(source, "w");
            fprintf(fc, "%s\n", writebufferp);
            fflush(fc);
            fclose(fc);
          }
          writesuccess = 1;
          break;
        }
      }
      wbj++;
    }
  }
  return (writesuccess == 1) ? 0 : 1;
}
int writefirstlinestr(char *source, char *string) {
  return writefirstline(source, READFILEWRITETYPELISTSINGLE, string);
}
int writefirstlineint(char *source, int number) {
  char numbuffer[32];
  sprintf(numbuffer, "%d", number);
  return writefirstline(source, READFILEWRITETYPELISTSINGLE, numbuffer);
}
int writetwolinesintint(char *source, int number1, int number2) {
  char numbuffer[64];
  sprintf(numbuffer, "%d\n%d", number1, number2);
  return writefirstline(source, READFILEWRITETYPELISTSINGLE, numbuffer);
}
// (re-)read the first line of source path(file) and return as int
FILE *readfirstlineopen(char *source) {
  FILE *fc = fopen(source, "r");
  return fc;
}
int rereadalineint(FILE *source, int linenum) {
  int inch;
  char ch;
  int target = 0;
  int firstch = 1;
  int tpol = '+';
  fflush(source);
  fseek(source, 0, SEEK_SET);
  int linei = 1;
  while (1) {
    inch = fgetc(source);
    if (inch == EOF)
      { break; }
    if (inch == '\n')
      { linei++; continue; }
    if (linei < linenum) {
      continue;
    } else if (linei == linenum) {
      ch = (char)inch;
      if (ch == '-' && firstch == 1)
        { tpol = '-'; }
      if (ch >= '0' && ch <= '9')
        { target = (target * 10) + (int)(ch - '0'); }
      firstch = 0;
    } else { break; }
  }
  return (tpol == '-') ? (0 - target) : target;
}
int rereadfirstlineint(FILE *source) {
  return rereadalineint(source, 1);
}
int rereadsecondlineint(FILE *source) {
  return rereadalineint(source, 2);
}
void readfirstlineclose(FILE *source) {
  fclose(source);
}
int readfirstlineint(char *source) {
  FILE *fc = readfirstlineopen(source);
  if (fc == NULL) { return 0; }
  int target = rereadfirstlineint(fc);
  readfirstlineclose(fc);
  return target;
}
int readsecondlineint(char *source) {
  FILE *fc = readfirstlineopen(source);
  if (fc == NULL) { return 0; }
  int target = rereadsecondlineint(fc);
  readfirstlineclose(fc);
  return target;
}

