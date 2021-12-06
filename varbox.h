
#define VARBOXINDEX		4
#define VARBOXPOSX		70
#define VARBOXPOSY		8
#define VARBOXSIZEW		9
#define VARBOXSIZEH		9

int drawvarbox() {
  cursorsetpos(VARBOXINDEX, VARBOXPOSX, VARBOXPOSY);
  cursordrawbox(VARBOXINDEX, VARBOXSIZEW, VARBOXSIZEH);
  int x = VARBOXPOSX + 1;
  int y = VARBOXPOSY + 1;
  GREENOUT(VARBOXINDEX, "var ", -4)
  cursorlinebreak(VARBOXINDEX);
  WHITEOUT1(VARBOXINDEX, " a = %2d", cursors[OUTBOXINDEX].x0, -7)
  cursorlinebreak(VARBOXINDEX);
  WHITEOUT1(VARBOXINDEX, " b = %2d", cursors[OUTBOXINDEX].y0, -7)
  cursorlinebreak(VARBOXINDEX);
  WHITEOUT1(VARBOXINDEX, " c = %2d", cursors[OUTBOXINDEX].offx, -7)
  cursorlinebreak(VARBOXINDEX);
  WHITEOUT1(VARBOXINDEX, " d = %2d", cursors[OUTBOXINDEX].offy, -7)
  cursorlinebreak(VARBOXINDEX);
  WHITEOUT1(VARBOXINDEX, " e = %2d", cursors[OUTBOXINDEX].dx, -7)
  cursorlinebreak(VARBOXINDEX);
  WHITEOUT1(VARBOXINDEX, " f = %2d", cursors[OUTBOXINDEX].dy, -7)
  cursorlinebreak(VARBOXINDEX);
/*
  WHITEOUT1(VARBOXINDEX, "c = %2d", outboxwidth, -6)
  cursorlinebreak(VARBOXINDEX);
  WHITEOUT1(VARBOXINDEX, "d = %2d", outboxheight, -6)
  cursorlinebreak(VARBOXINDEX);
  MAGENTAOUT1(VARBOXINDEX, "e = %2d", keepalives[outboxkeepalive].x, -6)
  cursorlinebreak(VARBOXINDEX);
  MAGENTAOUT1(VARBOXINDEX, "f = %2d", keepalives[outboxkeepalive].y, -6)
  cursorlinebreak(VARBOXINDEX);
  WHITEOUT1(VARBOXINDEX, "g = %2d", outputstreamlen, -6)
  cursorlinebreak(VARBOXINDEX);
*/
  cursorrelease(VARBOXINDEX);
}

