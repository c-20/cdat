
void drawoutbox(char *stream, int len) {
  int x = OUTBOX.x;
  int y = OUTBOX.y;
  int width = OUTBOX.width;
  int height = OUTBOX.height;
  int xi, yi, li = 0;
  // building the box
  cursorsetpos(OUTBOXINDEX, x, y);
  cursordrawbox(OUTBOXINDEX, width, height);
  fixedges(OUTBOXINDEX, x, y, width, height);
  // inside the box
  x += OUTCURSOR.offx;
  y += OUTCURSOR.offy;
  width -= OUTCURSOR.offx + OUTCURSOR.offx;
  height -= OUTCURSOR.offy + OUTCURSOR.offy;
  int linesize = 0;
  int linebreaklen = 0;
  int numlines = 0;
  for (yi = 0; yi < height; yi++) {
    for (xi = 0; xi < width; xi++) {
      if (stream[li] == '\n') {
        DARKOUT(OUTBOXINDEX, ".", -1)
        linesize++;
        linebreaklen++;
      } else {
        if (li < len - 1) {
          // if (yi % 2) DARKBG
/*
          if (li % 5 < 3) {
            BLUEBG
          } else {
            RESETBG
          }
*/
          YELLOWOUT1(OUTBOXINDEX, "%c", stream[li], -1)
        } else if (li == len - 1) {
          REDBG
          YELLOWOUT1(OUTBOXINDEX, "%c", stream[li], -1)
          RESETBG
        } else if (li == len) {
          GREENBG
          WHITEOUT(OUTBOXINDEX, " ", -1)
          RESETBG
//          int kax = xi;//OUTCURSOR.x0 + OUTCURSOR.offx + xi;
//          int kay = yi;//OUTCURSOR.y0 + OUTCURSOR.offy + yi;
          if (OUTBOX.keepalive) {
            setkeepalivepos(OUTBOX.keepalive, xi, yi);
          } else {
            OUTBOX.keepalive = addkeepalive(KEEPALIVETYPESPIN, xi, yi);
          }
        }
        li++;
        linesize++;
      }
      if (linesize >= width) {
        cursormovedown(OUTBOXINDEX, 1);
        cursormoveleft(OUTBOXINDEX, OUTCURSOR.dx);
//        cursorlinebreak(OUTBOXINDEX);
//        cursormoveright(OUTBOXINDEX, -(OUTCURSOR.x0 + OUTCURSOR.offx));
//        fflush(stdout);
        linesize = 0;
        numlines++;
      }
    }
    if (stream[li] == '\n' && linebreaklen > 0) {
      linebreaklen = 0;
      li++;
    }
  }
  RESETBG
  cursormoveup(OUTBOXINDEX, numlines);// + OUTCURSOR.y0 + OUTCURSOR.offy);
  cursorrelease(OUTBOXINDEX);
}

