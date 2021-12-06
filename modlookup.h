
#define MODSTRINGSIZE 18
#define MODSTRINGSDEF						\
  modstrings[0]  = "NONE A            ";	\
  modstrings[1]  = "NONE B            ";	\
  modstrings[2]  = "FUNCTION          ";	\
  modstrings[3]  = "EJECT             ";	\
  modstrings[4]  = "FUNCTION EJECT    ";	\
  modstrings[5]  = "CTRL              ";	\
  modstrings[6]  = "SHIFT             ";	\
  modstrings[7]  = "ALT               ";	\
  modstrings[8]  = "CMD               ";	\
  modstrings[9]  = "CTRL SHIFT        ";	\
  modstrings[10] = "CTRL ALT          ";	\
  modstrings[11] = "CTRL CMD          ";	\
  modstrings[12] = "SHIFT ALT         ";	\
  modstrings[13] = "CMD SHIFT         ";	\
  modstrings[14] = "CTRL ALT SHIFT    ";	\
  modstrings[15] = "CTRL ALT CMD      ";	\
  modstrings[16] = "CTRL SHIFT CMD    ";	\
  modstrings[17] = "CTRL ALT SHIFT CMD";	\
  modstrings[18] = "CMD ALT           ";	\
  modstrings[19] = "CMD ALT SHIFT     ";	\
  modstrings[20] = "UNKNOWN           ";	\
  modstrings[21] = "EMPTY             ";	\
  modstrings[22] = "ERROR             ";	\
  modstrings[23] = "CLEAR SCREEN      ";	\
  modstrings[24] = "POP EJECT STACK   ";	\
  modstrings[25] = "EXIT              ";
#define MODSTRINGSTOTAL 		26
#define MODSTRINGEMPTY			modstrings[21]
#define MODSTRINGERROR			modstrings[22]
#define MODSTRINGCLEARSCREEN	modstrings[23]
#define MODSTRINGPOPEJECTSTACK	modstrings[24]
#define MODSTRINGQUITDRPLINUX	modstrings[25]

#define ifequal(a1, b1, a2, b2)	if (a1 == a2 && b1 == b2)
#define ifequals(a,b1,b2)		if (a[0] == b1 && a[1] == b2)

char *modstrings[MODSTRINGSTOTAL];
int modejfntoggle = 0;
void modlookup(char a, char b, int type, char **result, int shiftstate) {
  // get both left and right modifier strings
  result[0] = modstrings[20];
  result[1] = modstrings[20];
  if (type == 2) {
    ifequal(a,b, '0','0') {
      result[0] = modstrings[1];
      result[1] = modstrings[1];
    }
    ifequal(a,b, '1','0') {
      result[0] = modstrings[2];
      result[1] = modstrings[2];
    }
    ifequal(a,b, '0','8') {
      // eject (as a pending task)
      result[0] = modstrings[3];
      if (shiftstate) {
        result[1] = MODSTRINGCLEARSCREEN;
      } else {
        result[1] = MODSTRINGPOPEJECTSTACK;
      }
    }
    ifequal(a,b, '1','8') {
      result[0] = modstrings[4];
      result[1] = MODSTRINGQUITDRPLINUX;
/*
      result[1] = modstrings[4];
      FLUSHBUFFER
      cursorrelease(KEYBOXINDEX);
      PLAINTEXT RESETBG
      _exit(1024);
*/
    }
  } else if (type == 1) {
    if (b == '0') result[0] = modstrings[0];
    if (a == '0') result[1] = modstrings[0];
    if (b == '1') result[0] = modstrings[5];
    if (a == '1') result[1] = modstrings[5];
    if (b == '2') result[0] = modstrings[6];
    if (a == '2') result[1] = modstrings[6];
    if (b == '4') result[0] = modstrings[7];
    if (a == '4') result[1] = modstrings[7];
    if (b == '8') result[0] = modstrings[8];
    if (a == '8') result[1] = modstrings[8];
    if (b == '3') result[0] = modstrings[9];
    if (a == '3') result[1] = modstrings[9];
    if (b == '5') result[0] = modstrings[10];
    if (a == '5') result[1] = modstrings[10];
    if (b == '9') result[0] = modstrings[11];
    if (a == '9') result[1] = modstrings[11];
    if (b == '6') result[0] = modstrings[12];
    if (a == '6') result[1] = modstrings[12];
    if (b == 'a') result[0] = modstrings[13];
    if (a == 'a') result[1] = modstrings[13];
    if (b == '7') result[0] = modstrings[14];
    if (a == '7') result[1] = modstrings[14];
    if (b == 'd') result[0] = modstrings[15];
    if (a == 'd') result[1] = modstrings[15];
    if (b == 'b') result[0] = modstrings[16];
    if (a == 'b') result[1] = modstrings[16];
    if (b == 'f') result[0] = modstrings[17];
    if (a == 'f') result[1] = modstrings[17];
    if (b == 'c') result[0] = modstrings[18];
    if (a == 'c') result[1] = modstrings[18];
    if (b == 'e') result[0] = modstrings[19];
    if (a == 'e') result[1] = modstrings[19];
  }
}

int getshiftstate(char *mods) {
  ifequals(mods, '0','2') return 1;
  ifequals(mods, '0','3') return 1;
  ifequals(mods, '0','6') return 1;
  ifequals(mods, '0','a') return 1;
  ifequals(mods, '0','7') return 1;
  ifequals(mods, '0','b') return 1;
  ifequals(mods, '0','f') return 1;
  ifequals(mods, '0','e') return 1;
  return 0;
}

int getctrlstate(char *mods) {
  ifequals(mods, '0','1') return 1;
  ifequals(mods, '0','3') return 1;
  ifequals(mods, '0','5') return 1;
  ifequals(mods, '0','9') return 1;
  ifequals(mods, '0','7') return 1;
  ifequals(mods, '0','d') return 1;
  ifequals(mods, '0','b') return 1;
  ifequals(mods, '0','f') return 1;
  return 0;
}

int getaltstate(char *mods) {
  ifequals(mods, '0','4') return 1;
  ifequals(mods, '0','5') return 1;
  ifequals(mods, '0','6') return 1;
  ifequals(mods, '0','7') return 1;
  ifequals(mods, '0','d') return 1;
  ifequals(mods, '0','f') return 1;
  ifequals(mods, '0','c') return 1;
  ifequals(mods, '0','e') return 1;
  return 0;
}

int getcmdstate(char *mods) {
  ifequals(mods, '0','8') return 1;
  ifequals(mods, '0','9') return 1;
  ifequals(mods, '0','a') return 1;
  ifequals(mods, '0','d') return 1;
  ifequals(mods, '0','b') return 1;
  ifequals(mods, '0','f') return 1;
  ifequals(mods, '0','c') return 1;
  ifequals(mods, '0','e') return 1;
  return 0;
}

#undef ifequals
#undef ifequal
