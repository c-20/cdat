#define SYNCRINGSIZEW		7
#define SYNCRINGSIZEH		3
#define SYNCRINGMIDRIGHT	SYNCRINGSIZEW
#define SYNCRINGMIDLEFT		((SYNCRINGSIZEW * 2) + (SYNCRINGSIZEH - 2))

#define SYNCRINGHOME											\
		  REDBG WHITEOUT1(SYNCBOXINDEX, "%c", syncchar, -1) RESETBG
#define SYNCRINGLOW												\
		  GREENBG WHITEOUT1(SYNCBOXINDEX, "%c", syncchar, -1) RESETBG
#define SYNCRINGHIGH											\
		  BLACKBG MAGENTAOUT1(SYNCBOXINDEX, "%c", syncchar, -1) RESETBG
#define SYNCWRITELABEL(name, nsize)								\
		  GREENOUT(SYNCBOXINDEX, name, -nsize)					\
          cursorlinebreak(SYNCBOXINDEX);
#define SYNCWRITEINTFORMAT		" %s = "
#define SYNCWRITEINT(name, format, value, nsize, vsize)			\
		  WHITEOUT1(SYNCBOXINDEX, SYNCWRITEINTFORMAT, name, -(4 + nsize))	\
          MAGENTAOUT1(SYNCBOXINDEX, format, value, -vsize)		\
          cursorlinebreak(SYNCBOXINDEX);
#define SYNCWRITE(str, ssize)									\
		  WHITEOUT(SYNCBOXINDEX, str, -ssize)
#define SYNCLINEBREAK											\
		  cursorlinebreak(SYNCBOXINDEX);
/* \ */
int synctoggle = 0;
int syncloop = 0;
int synckill = 0;
char syncspin = '?';
FILE *synccountfile = NULL;
FILE *syncactionfile = NULL;
int prevsynccount = 0, synccount = 0;

char syncstartup() {
  synccountfile = fopen(FILEACTIONINPUTCOUNTER, "r");
  if (synccountfile <= 0) { return 'n'; }
  syncactionfile = fopen(FILEACTIONINPUTBUFFER, "r");
  if (syncactionfile <= 0) { return 'n'; }
  prevsynccount = rereadfirstlineint(synccountfile);
  synccount = prevsynccount;
  return 'y';
}

void syncshutdown() {
  if (synccountfile) {
    fclose(synccountfile);
    synccountfile = NULL;
  }
  if (syncactionfile) {
    fclose(syncactionfile);
    syncactionfile = NULL;
  }
}

// timing/load loop
void drawsyncring() {
  int i;
//  int flagp = system("table -count-flags @incrementreadinputoutput -c 3 == raised");
//  flagp = WEXITSTATUS(flagp);
//  char syncchar = '0' + flagp; //(synctoggle) ? 'P' : 'N';
#ifdef ACTIONBUFFERLISTENABLED
  char syncchar = '?';
  int syncfilecount = 0;
  int inch;
  char ch;
  if (synccountfile) {
    syncfilecount = rereadfirstlineint(synccountfile);
  }
  if (syncfilecount == synccount) {
    char syncsubchar = '+';
    if (INBOX1.backgroundpid > 0) syncsubchar = '&';
    syncchar = (synctoggle) ? syncsubchar : ' ';
  } else if (syncfilecount > synccount) {
    synccount = syncfilecount;
    syncchar = ((synccount - prevsynccount) < 10) ? '0' + (synccount - prevsynccount) : '#';
  } else syncchar = '!';
  // KEYLOGBUFFERLIST is handled in main.h
  cursorswap(SYNCBOXINDEX, ACTIONBUFFERLISTINDEX);
#include "mainactionbufferlist.h"
  cursorswap(ACTIONBUFFERLISTINDEX, SYNCBOXINDEX);
#else
  char syncchar = (synctoggle) ? '+' : ' ';
#endif
  // top row
  if (syncloop < SYNCRINGSIZEW) {
    for (i = 0; i < SYNCRINGSIZEW; i++) {
      if (syncloop == i) {
        if (synckill || i == 0) { SYNCRINGHOME }
        else if (synctoggle)    { SYNCRINGHIGH }
        else			        { SYNCRINGLOW  }
      } else { SYNCWRITE(" ", 1) }
    }
  } else { SYNCWRITE("       ", 7) }
  SYNCLINEBREAK
  // middle left
  if (syncloop == SYNCRINGMIDLEFT) {
    if (synckill)        { SYNCRINGHOME }
    else if (synctoggle) { SYNCRINGHIGH }
    else	             { SYNCRINGLOW  }
  } else { SYNCWRITE(" ", 1) }
  // centre 5x1 display
  if (synckill) {
    REDOUT(SYNCBOXINDEX, "12:00", -5)
  } else if (synctoggle) {
    GREENOUT(SYNCBOXINDEX, "12:00", -5)
  } else {
    DARKOUT(SYNCBOXINDEX, "12:00", -5)
  }
  // middle right
  if (syncloop == SYNCRINGMIDRIGHT) {
    if (synckill)        { SYNCRINGHOME }
    else if (synctoggle) { SYNCRINGHIGH }
    else                 { SYNCRINGLOW  }
  } else { SYNCWRITE(" ", 1) }
  SYNCLINEBREAK
  // bottom row
  if (syncloop > SYNCRINGMIDRIGHT && syncloop < SYNCRINGMIDLEFT) {
    for (i = SYNCRINGMIDLEFT - 1; i >= SYNCRINGMIDRIGHT + 1; i--) {
      if (syncloop == i) {
        if (synckill)        { SYNCRINGHOME }
        else if (synctoggle) { SYNCRINGHIGH }
        else                 { SYNCRINGLOW  }
      } else { SYNCWRITE(" ", 1) }
    }
  } else { SYNCWRITE("       ", 7) }
  SYNCLINEBREAK
  synctoggle = (synctoggle) ? 0 : 1;
  syncloop = ++syncloop % 16;
}

void drawcommandline() {
    int ibi = 0;
    while (ibi < inputbufferlen) {
      if (ibi == commandcursorposmin) {
        BLUEBG
      }
      WHITEOUT1(BACKCURSORINDEX, "%c", inputbuffer[ibi], -1)
      if (ibi == commandcursorposmax) {
        RESETBG
      }
      ibi++;
    }
    if (ibi == commandcursorposmin) {
      BLUEBG
    } else {
      YELLOWBG
    }
    WHITEOUT1(BACKCURSORINDEX, "%c", syncspin, -1)
    RESETBG
    cursormoveleft(BACKCURSORINDEX, inputbufferlen + 1);
    PLAINTEXT
}

void drawsyncbox() {
  if (syncboxstate == '!') {
    cursorsetpos(SYNCBOXINDEX, SYNCBOXPOSX, SYNCBOXPOSY);
    cursordrawbox(SYNCBOXINDEX, SYNCBOXSIZEW, SYNCBOXSIZEH);
    int x = SYNCBOXPOSX + 1;
    int y = SYNCBOXPOSY + 1;
    drawsyncring();
    cursormoveup(SYNCBOXINDEX, 3);
    PLAINTEXT
    cursorrelease(SYNCBOXINDEX);
    syncboxstate = 'S';
  } else if (synckill || syncboxstate == 'S') {
    cursorswap(BACKCURSORINDEX, SYNCBOXINDEX);
    drawsyncring();
    PLAINTEXT
    cursormoveup(SYNCBOXINDEX, 3);
    cursorswap(SYNCBOXINDEX, BACKCURSORINDEX);
//    cursorrelease(SYNCBOXINDEX);
/*
    cursorsetpos(SYNCBOXINDEX, SYNCBOXPOSX, SYNCBOXPOSY);
    cursordrawbox(SYNCBOXINDEX, SYNCBOXSIZEW, SYNCBOXSIZEH);
    int x = SYNCBOXPOSX + 1;
    int y = SYNCBOXPOSY + 1;
*/
//    SYNCWRITELABEL("vars", 4)
//    SYNCWRITEINT("a", "%2d", cursors[OUTBOXINDEX].x0, 1, 2)
  } else if (syncboxstate == 'C') {
    syncspin = spinkeepalive(syncspin);
//    cursorgoto(BACKCURSORINDEX, KEYOUTCURSOR.x0, KEYOUTCURSOR.y0);
    cursorswap(BACKCURSORINDEX, KEYOUTINDEX);
    drawcommandline();
    cursorswap(KEYOUTINDEX, BACKCURSORINDEX);
//    cursorgohome(BACKCURSORINDEX);
  }
}

