int drawkeyoutbox() {
  if (syncboxstate == '!') {
    cursorsetpos(KEYOUTINDEX, KEYOUTPOSX, KEYOUTPOSY);
    cursordrawbox(KEYOUTINDEX, KEYOUTSIZEW, KEYOUTSIZEH);
    int x = KEYOUTPOSX + 1;
    int y = KEYOUTPOSY + 1;
    // ... ready to draw ...
    cursorrelease(KEYOUTINDEX);
  }
}

void fillkeyoutbox(char *buffer, char *syncboxstate, int cursorposmin, int cursorposmax) {
  char sbs = *syncboxstate;
  int kbbi = 0, kbli = 0;
  while (kbli < KEYOUTSIZEIW) {
    if (inputbuffer[kbbi] != '\0') {
      if (sbs == '!')
        { REDOUT1(KEYOUTINDEX, "%c", inputbuffer[kbbi], -1) }
      else if (sbs == 'W')
        { YELLOWOUT1(KEYOUTINDEX, "%c", inputbuffer[kbbi], -1) }
      else if (sbs == 'C') {
        if (kbli == cursorposmin)
          { REDBG }
        WHITEOUT1(KEYOUTINDEX, "%c", inputbuffer[kbbi], -1)
        if (kbli == cursorposmax)
          { RESETBG }
      } else if (sbs == 'S')
        { LIGHTOUT1(KEYOUTINDEX, "%c", inputbuffer[kbbi], -1) }
      kbbi++;
    } else
      { OUT(KEYOUTINDEX, " ", -1) }
    kbli++;
  }
  RESETBG
  cursorlinebreak(KEYOUTINDEX);
//        cursormoveleft(KEYOUTINDEX, KEYOUTSIZEW - 2);
  if (sbs == 'W')
    { *syncboxstate = 'C'; }
}
