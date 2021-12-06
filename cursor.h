
void cursorsetpos(int index, int x0, int y0) {
  cursors[index].x0 = x0;
  cursors[index].y0 = y0;
}

void cursormoveup(int index, int n) {
  if (n == 0) return;
  if (n < 0) n = -n;
  else fprintf(stdout, "\033[%dA", n);
  cursors[index].dy -= n;
  if (index != BACKCURSORINDEX)
    cursors[BACKCURSORINDEX].dy -= n;
}

void cursormovedown(int index, int n) {
  if (n == 0) return;
  if (n < 0) n = -n;
  else fprintf(stdout, "\033[%dB", n);
  cursors[index].dy += n;
  if (index != BACKCURSORINDEX)
    cursors[BACKCURSORINDEX].dy += n;
}

void cursormoveleft(int index, int n) {
  if (n == 0) return;
  if (n < 0) n = -n;
  else fprintf(stdout, "\033[%dD", n);
  cursors[index].dx -= n;
  if (index != BACKCURSORINDEX)
    cursors[BACKCURSORINDEX].dx -= n;
}

void cursormoveright(int index, int n) {
  if (n == 0) return;
  if (n < 0) n = -n;
  else fprintf(stdout, "\033[%dC", n);
  cursors[index].dx += n;
  if (index != BACKCURSORINDEX)
    cursors[BACKCURSORINDEX].dx += n;
}

void cursormoveleftright(int index, int n) {
  if (n < 0) cursormoveleft(index, -n);
  if (n > 0) cursormoveright(index, n);
}

void cursormoverightleft(int index, int n) {
  if (n < 0) cursormoveright(index, -n);
  if (n > 0) cursormoveleft(index, n);
}

void cursormoveupdown(int index, int n) {
  if (n < 0) cursormoveup(index, -n);
  if (n > 0) cursormovedown(index, n);
}

void cursormovedownup(int index, int n) {
  if (n < 0) cursormovedown(index, -n);
  if (n > 0) cursormoveup(index, n);
}

void cursorlinebreak(int index) {
  cursormoveleft(index, cursors[index].dx);
  cursormovedown(index, 1);
}

// cursorgoto expects that the selected cursor is active
void cursorgoto(int index, int dx, int dy) {
  int oldx = cursors[index].dx;
  int oldy = cursors[index].dy;
  if (oldx < dx)
    cursormoveright(index, dx - oldx);
  if (oldx > dx)
    cursormoveleft(index, oldx - dx);
  if (oldy < dy)
    cursormovedown(index, dy - oldy);
  if (oldy > dy)
    cursormoveup(index, oldy - dy);
}

void cursorgohome(int index) {
  int dx = cursors[index].dx;
  int dy = cursors[index].dy;
  if (dx < 0)
    cursormoveright(index, -dx);
  if (dx > 0)
    cursormoveleft(index, dx);
  if (dy < 0)
    cursormovedown(index, -dy);
  if (dy > 0)
    cursormoveup(index, dy);
}

void cursordetour(int index, int fromx, int fromy) {
/*
  cursormoveleft(index, fromx);
  cursormoveup(index, fromy);
*/
  cursormoverightleft(index, fromx);
  cursormovedownup(index, fromy);
}

void cursortour(int index, int tox, int toy) {
/*
  cursormoveright(index, tox);
  cursormovedown(index, toy);
*/
  cursormoveleftright(index, tox);
  cursormoveupdown(index, toy);
}

void cursordrawbox(int index, int width, int height) {
  cursor c = cursors[index];
  cursorgoto(BACKCURSORINDEX, c.x0, c.y0);
  drawbox(c.x0, c.y0, width, height);
  cursormoveright(BACKCURSORINDEX, -c.offx);
  cursormovedown(BACKCURSORINDEX, -c.offy);
//  fflush(stdout);
}

void cursorsavepos(int index) {
  cursors[index].savedx = cursors[index].dx;
  cursors[index].savedy = cursors[index].dy;
}

void cursorloadpos(int index) {
  cursorgoto(index, cursors[index].savedx, cursors[index].savedy);
}


/*
void cursorwritetext(int index, char *buffer) {
  int i;
  for (i = 0; buffer[i] != '\0'; i++) {
    WRITECHAR(buffer[i]);
    cursormoveright(index, -1);
  }
}
*/
/*
void cursorcreatebox(int index, int x, in

void cursorgotobox(int index) {
//-------------------------------------------------------------
// OUTBOX STARTS HERE
//-------------------------------------------------------------
  cursormoveright(BACKCURSORINDEX, outputboxx);
  cursormovedown(BACKCURSORINDEX, outputboxy);
  addbox(outputboxx, outputboxy, outputboxwidth,
         outputboxheight, outputstream);
  if (outputstreamlen > 0)
    cursormoveright(OUTCURSORINDEX, -outputstreamlen);
  cursormoveright(BACKCURSORINDEX, -1);
  cursormovedown(BACKCURSORINDEX, -1);
//  fflush(stdout);
  cursorsethome(OUTCURSORINDEX, outputboxx, outputboxy, 1, 1);
}
*/
void cursorrelease(int index) {
//  cursorlinebreak(index);
/*
MAJOR BUG: THIS SHOULD BE HERE BUT ONLY DEALS WITH DX
  if (index != BACKCURSORINDEX) {
    cursorgohome(index);
  }
*/
  cursorgohome(BACKCURSORINDEX);
//  fflush(stdout);
//  cursor oldc = cursors[index];
//  cursorgoto(BACKCURSORINDEX, -(oldc.x0 + oldc.offx), -(oldc.y0 + oldc.offy));
//  cursorgoto(BACKCURSORINDEX, -oldc.x0, -oldc.y0);
//  cursorgoto(BACKCURSORINDEX, 0, 0);
}

void cursorswap(int oldindex, int newindex) {
  cursorrelease(oldindex);
  cursor newc = cursors[newindex];
/*
  cursorgoto(BACKCURSORINDEX, newc.x0, newc.y0);
  cursorgoto(BACKCURSORINDEX, newc.offx, newc.offy);
  cursorgoto(BACKCURSORINDEX, newc.dx, newc.dy);
*/
  cursormoveright(BACKCURSORINDEX, newc.x0 + newc.offx + newc.dx);
  cursormovedown(BACKCURSORINDEX, newc.y0 + newc.offy + newc.dy);
}


