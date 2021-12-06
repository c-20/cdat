
int drawbufferbox(int index, char *buffer) {
  int numlines = 0;
  cursorsetpos(BUFFERBOXINDEX, BUFFERBOXPOSX, BUFFERBOXPOSY);
  cursordrawbox(BUFFERBOXINDEX, BUFFERBOXSIZEW, BUFFERBOXSIZEH);
  int x = BUFFERBOXPOSX + 1;
  int y = BUFFERBOXPOSY + 1;
  int bi = 0, bil = 0;
  while (buffer[bi] != '\0') {
    if (buffer[bi] == '\n' || bil + 1 >= BUFFERBOXSIZEIW) {
      if (numlines + 1 >= BUFFERBOXSIZEIH) {
        while (bil < BUFFERBOXSIZEIW) {
          OUT(BUFFERBOXINDEX, " ", -1)
          bil++;
        }
        REDOUT(BUFFERBOXINDEX, ">", -1)
        break;
      } else {
        if (bil + 1 >= BUFFERBOXSIZEIW) {
          OUT1(BUFFERBOXINDEX, "%c", buffer[bi], -1)
        }
        bil = -1;
        cursorlinebreak(BUFFERBOXINDEX);
        numlines++;
      }
    } else if (buffer[bi] < ' ' || buffer[bi] > '~') {
      REDOUT(BUFFERBOXINDEX, "!", -1)
    } else {
      YELLOWOUT1(BUFFERBOXINDEX, "%c", buffer[bi], -1)
    }
    bi++;
    bil++;
  }
  cursorlinebreak(BUFFERBOXINDEX);
  numlines++;
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
  PLAINTEXT
  cursormoveup(BUFFERBOXINDEX, numlines);
  cursorrelease(BUFFERBOXINDEX);
}

