
void fixedges(int boxindex, int x, int y, int width, int height) {
  int xi, yi;
  box b = boxes[boxindex];
  // fixing the edges
  if (b.hasmoved) {
    int ofa = b.fillabove;
    int ofb = b.fillbelow;
    // fix left (and top-left/bottom-left)
    if (b.fillleft) {
      cursorsavepos(boxindex);
      cursormoveleft(boxindex, 2);
      cursormoveup(boxindex, 1 + b.fillabove);
      for (yi = 0 - b.fillabove; yi < height + b.fillbelow; yi++) {
        drawbackboxpixel(x - 1, y + yi, '.', ' ');
        cursormoveright(boxindex, -1);
        cursormoveleft(boxindex, 1);
        cursormovedown(boxindex, 1);
      }
      cursorloadpos(boxindex);
    }
    // fix right (and top-right/bottom-right)
    if (b.fillright) {
      cursorsavepos(boxindex);
      cursormoveright(boxindex, width - 1);
      cursormoveup(boxindex, 1 + b.fillabove);
      for (yi = 0 - b.fillabove; yi < height + b.fillbelow; yi++) {
        drawbackboxpixel(x + width, y + yi, '.', ' ');
        cursormoveright(boxindex, -1);
        cursormoveleft(boxindex, 1);
        cursormovedown(boxindex, 1);
      }
      cursorloadpos(boxindex);
    }
    // fix above
    if (b.fillabove) {
      cursorsavepos(boxindex);
      cursormoveup(boxindex, 2);
      cursormoveleft(boxindex, 1);
      for (xi = 0; xi < width; xi++) {
        drawbackboxpixel(x + xi, y - 1, '.', ' ');
        cursormoveright(boxindex, -1);
      }
      cursorloadpos(boxindex);
    }
    // fix below
    if (b.fillbelow) {
      cursorsavepos(boxindex);
      cursormovedown(boxindex, height - 1);
      cursormoveleft(boxindex, 1);
      for (xi = 0; xi < width; xi++) {
        drawbackboxpixel(x + xi, y + height, '.', ' ');
        cursormoveright(boxindex, -1);
      }
      cursorloadpos(boxindex);
    }
    // clear flags
    boxes[boxindex].fillleft = 0;
    boxes[boxindex].fillright = 0;
    boxes[boxindex].fillabove = 0;
    boxes[boxindex].fillbelow = 0;
    boxes[boxindex].hasmoved = 0;
  }
}
