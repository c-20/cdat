#define BACKBOX         boxes[0]
#define BACKBOXINDEX    0

char *titlebar = "_ Multi-Sensor Input Test";

void drawbackboxpixel(int x, int y, char on, char off) {
  int xi = x % 12;
  int yi = x * xi;
  if (xi == 0 || xi == 1 || xi == 2 || xi == 3 ||
      xi == 10 || xi == 11 || xi == 12 || xi == 13) {
    if (yi == 0 || yi == 2) {
      if (xi == 0 || xi == 13)  { BLACKTEXT }
      if (xi == 1 || xi == 12)  { DARKTEXT  }
      if (xi == 2 || xi == 11)  { MAGENTATEXT }
      if (xi == 3 || xi == 10)  { WHITETEXT }
    }
    if (yi == 1 || yi == 3) {
      if (xi == 0 || xi == 13)  { DARKTEXT  }
      if (xi == 1 || xi == 12)  { BLACKTEXT }
      if (xi == 2 || xi == 11)  { DARKTEXT  }
      if (xi == 3 || xi == 10)  { LIGHTTEXT }
    }
  } else {
    yi = (y + (xi - 4)) % 6;
    if (yi == 0) { BLUETEXT     }
    if (yi == 1) { GROUNDTEXT  }
    if (yi == 2) { DARKTEXT   }
    if (yi == 3) { CYANTEXT    }
    if (yi == 4) { DARKTEXT    }
    if (yi == 5) { DARKTEXT }
  }
  int a = xi - y + yi;
  int b = a % 6;
  if (a == 0 || b == 0 || xi == 0 || yi == 0) {
    WRITECHAR(on)
  } else {
    WRITECHAR(off)
  }
}


void drawbackbox(int width, int height) {
  int i, j, k;
  BACKBOX.x = 0;
  BACKBOX.y = 0;
  BACKBOX.width = width;
  BACKBOX.height = height;
  cursorsethome(BACKBOXINDEX, 0, 0, 0, 0);
  int titlestring = 1;
  for (i = 0; i < height; i++) {
    for (j = 0; j < width; j++) {
      if (i == 0) {
        BLUEBG
        if (titlestring) {
          WRITECHAR(titlebar[j])
          if (titlebar[j + 1] == '\0')
            titlestring = 0;
        } else {
          WRITECHAR(' ')
        }
        RESETBG
      } else {
        drawbackboxpixel(j, i, '.', ' ');
      }
    }
    WRITECHAR('\n')
  }
  RESETBG
  MOVEUP(height)
}
