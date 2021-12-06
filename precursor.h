
// negative movement is 'shadow' movement

#define BACKCURSORINDEX    0
#define MAXCURSORS         (MAXBOXES)

typedef struct cursortype {
  int x0, y0;
  int offx, offy;
  int dx, dy;
  int savedx, savedy;
} cursor;

cursor cursors[MAXCURSORS];

void cursorsethome(int index, int x0, int y0, int offx, int offy) {
  cursors[index].x0 = x0;
  cursors[index].y0 = y0;
  cursors[index].offx = offx;
  cursors[index].offy = offy;
  cursors[index].dx = 0;
  cursors[index].dy = 0;
}
