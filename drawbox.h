
int dbt = 0;
int dbl = 0;

void openbox(int x0, int y0, int width, int height) {
  char lcchar = (dbl == 0) ? '/' : (dbl == 1) ? '-' : (dbl == 2) ? '\\': '|';
  char rcchar = (dbl == 0) ? '|' : (dbl == 1) ? '/' : (dbl == 2) ? '-' : '\\';
  char hchar = (dbl == 1 || dbl == 2) ? '+' : '-';
  char vchar = (dbl == 2 || dbl == 3) ? '+' : '|';
  int i, j;
//  LIGHTTEXT
  // top left
//  if (dbl != 2) DARKTEXT // CYANTEXT
//  else LIGHTTEXT // WHITETEXT
  drawbackboxpixel(x0, y0, '[', '[');//lcchar, '.');
//  WRITECHAR(lcchar);
  // top
//  if (dbl == 1 || dbl == 2) DARKTEXT // CYANTEXT
//  else LIGHTTEXT // WHITETEXT
  for (i = 2; i < width; i++) {
    drawbackboxpixel(x0 + i, y0, '-', '-');//hchar, '.');
//    MOVERIGHT(1)
  }
//    WRITECHAR(hchar);
  // top right
//  if (dbl != 1) DARKTEXT // CYANTEXT
//  else LIGHTTEXT // WHITETEXT
  drawbackboxpixel(x0 + i, y0, ']', ']');//rcchar, '.');
//  SETCHAR(rcchar)
  MOVELEFT(width)
  // left
//  if (dbl == 2 || dbl == 3) DARKTEXT // CYANTEXT
//  else LIGHTTEXT // WHITETEXT
  for (j = 2; j < height; j++) {
    MOVEDOWN(1)
    drawbackboxpixel(x0, y0 + j, '[', '[');//vchar, '.');
    MOVELEFT(1)
//    SETCHAR(vchar)
  }
  MOVEDOWN(1)
  PLAINTEXT
  FLUSHBUFFER
  // cursor waits at bottom left
}

void closebox(int x0, int y0, int width, int height) {
  char lcchar = (dbl == 0) ? '-' : (dbl == 1) ? '\\': (dbl == 2) ? '|' : '/';
  char rcchar = (dbl == 0) ? '\\': (dbl == 1) ? '|' : (dbl == 2) ? '/' : '-';
  char hchar = (dbl == 3 || dbl == 0) ? '+' : '-';
  char vchar = (dbl == 0 || dbl == 1) ? '+' : '|';
  int i;
//  LIGHTTEXT
  // bottom left
//  if (dbl != 3) DARKTEXT // CYANTEXT
//  else LIGHTTEXT // WHITETEXT
  drawbackboxpixel(x0, y0, '[', '[');//lcchar, '.');
//  WRITECHAR(lcchar);
  // bottom
//  if (dbl == 3 || dbl == 0) DARKTEXT // CYANTEXT
//  else LIGHTTEXT // WHITETEXT
  for (i = 2; i < width; i++) {
    drawbackboxpixel(x0 + i, y0, '-', '-');//hchar, '.');
//    WRITECHAR(hchar);
  }
  // bottom right
//  if (dbl != 0) DARKTEXT // CYANTEXT
//  else LIGHTTEXT // WHITETEXT
  drawbackboxpixel(x0, y0, ']', ']');//rcchar, '.');
  MOVELEFT(1)
//  SETCHAR(rcchar)
  // right
//  if (dbl == 0 || dbl == 1) DARKTEXT // CYANTEXT
//  else LIGHTTEXT // WHITETEXT
  for (i = 2; i < height; i++) {
    MOVEUP(1)
    drawbackboxpixel(x0, y0 + i, ']', ']');//vchar, '.');
    MOVELEFT(1)
  }
  MOVELEFT(width - 2)
  PLAINTEXT
  FLUSHBUFFER
  // cursor waits inside top left of box
}

void clearbox(int x0, int y0, int width, int height) {
  int i, j;
  for (i = 0; i < height; i++) {
    for (j = 0; j < width; j++)
      WRITECHAR(' ');
    MOVELEFT(width)
    MOVEDOWN(1)
  }
  MOVEUP(height)
  FLUSHBUFFER
  // cursor waits where it began
}

void drawbox(int x0, int y0, int width, int height) {
  openbox(x0, y0, width, height);
  closebox(x0, y0, width, height);
  clearbox(x0 + 1, y0 + 1, width - 2, height - 2);
  dbt = (dbt) ? 0 : 1;
  dbl = ++dbl % 4;
  FLUSHBUFFER
}

