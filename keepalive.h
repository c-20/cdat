
#define KEEPALIVEINDEX          3

#define KEEPALIVEPOSX           70
#define KEEPALIVEPOSY           3
#define KEEPALIVESIZEW          9
#define KEEPALIVESIZEH          5

#define KEEPALIVEHOME				\
		  REDBG WHITEOUT(KEEPALIVEINDEX, " ", -1) RESETBG
#define KEEPALIVEZERO				\
		  GREENBG WHITEOUT(KEEPALIVEINDEX, " ", -1) RESETBG
#define KEEPALIVEONE				\
		  BLACKBG MAGENTAOUT(KEEPALIVEINDEX, "+", -1) RESETBG
#define KEEPALIVESPACE(str, size)	\
		  WHITEOUT(KEEPALIVEINDEX, str, size)
#define KEEPALIVEBREAK				\
		  cursorlinebreak(KEEPALIVEINDEX);

#define MAXKEEPALIVES		10
#define KEEPALIVETYPESPIN	1

#define KEEPALIVEDEF						\
  keepalives[0].type = KEEPALIVETYPESPIN;	\
  keepalives[0].x = 0;						\
  keepalives[0].y = 0;						\
  keepalives[0].c = '-';

typedef struct keepalivestruct {
  int type;
  int x, y;
  char c;
} keepalive;
keepalive keepalives[MAXKEEPALIVES];
int numkeepalives = 0;

int addkeepalive(int type, int x, int y) {
  numkeepalives++;
  keepalives[numkeepalives].type = type;
  keepalives[numkeepalives].x = x;
  keepalives[numkeepalives].y = y;
  keepalives[numkeepalives].c = '-';
  return numkeepalives;
}

void setkeepalivepos(int index, int x, int y) {
  keepalives[index].x = x;
  keepalives[index].y = y;
}

char spinkeepalive(char k) {
  if (k == '|') return '/';
  if (k == '/') return '-';
  if (k == '-') return '\\';
  if (k == '\\') return '|';
  return '-';
}

void dokeepalive() {
  int i;
  for (i = 0; i < numkeepalives; i++) {
    cursorgoto(BACKCURSORINDEX, keepalives[i].x, keepalives[i].y);
    BLUEBG
    WHITEOUT1(BACKCURSORINDEX, "%c", keepalives[i].c, -1)
    RESETBG
    cursorrelease(BACKCURSORINDEX);
    keepalives[i].c = spinkeepalive(keepalives[i].c);
  }
}

int keepalivetoggle = 0;
int keepaliveloop = 0;
void drawkeepalivebox() {
  int x;
  cursordrawbox(KEEPALIVEINDEX, KEEPALIVESIZEW, KEEPALIVESIZEH);
  if (keepaliveloop < 7) {
    for (x = 0; x < 7; x++) {
      if (keepaliveloop == x) {
        if (keepalivetoggle)
          { KEEPALIVEONE }
        else {
          if (x == 0)
            { KEEPALIVEHOME }
          else
            { KEEPALIVEZERO }
        }
      } else
        { KEEPALIVESPACE(" ", -1) }
    }
  } else
    { KEEPALIVESPACE("       ", -7) }
  KEEPALIVEBREAK
  if (keepaliveloop == 15) {
    if (keepalivetoggle)
      { KEEPALIVEONE }
    else
      { KEEPALIVEZERO }
  } else
    { KEEPALIVESPACE(" ", -1) }
  if (keepalivetoggle)
    { GREENOUT(KEEPALIVEINDEX, "ALIVE", -5) }
  else
    { DARKOUT(KEEPALIVEINDEX, "ALIVE", -5) }
  if (keepaliveloop == 7) {
    if (keepalivetoggle)
      { KEEPALIVEONE }
    else
      { KEEPALIVEZERO }
  } else
    { KEEPALIVESPACE(" ", -1) }
  KEEPALIVEBREAK
  if (keepaliveloop > 7 && keepaliveloop < 15) {
    for (x = 14; x >= 8; x--) {
      if (keepaliveloop == x) {
        if (keepalivetoggle)
          { KEEPALIVEONE }
        else
          { KEEPALIVEZERO }
      } else
        { KEEPALIVESPACE(" ", -1) }
    }
  } else
    { KEEPALIVESPACE("       ", -7) }
  KEEPALIVEBREAK
  keepalivetoggle = (keepalivetoggle) ? 0 : 1;
  keepaliveloop = ++keepaliveloop % 16;

  cursorrelease(KEEPALIVEINDEX);

  dokeepalive();

//  fflush(stdout);
}

