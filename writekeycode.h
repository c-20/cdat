
void writekeycode(int boxindex, char *keycode, int shiftstate) {
  if (PAIREQUALS(keycode, '-','-')) {
    // shiftstate -1 gives no output
    if (shiftstate != -1)
      GREENOUT(boxindex, "-", -1)
  } else {
    char keychar = keylookup(keycode[0], keycode[1], shiftstate);
    if (keychar == '\0') {
      // unknown/special/hot keys
      CYANOUT2(boxindex, "%c%c", keycode[0], keycode[1], -2)
    } else {
      // standard keys
      if (keychar == '\n') {
        REDOUT(boxindex, "n", -1)
      } else if (keychar == '\b') {
        REDOUT(boxindex, "b", -1)
      } else if (keychar == ' ') {
        REDOUT(boxindex, "_", -1)
      } else if (keychar > ' ' && keychar <= '~') {
        WHITEOUT1(boxindex, "%c", keychar, -1)
      } else {
        CYANOUT(boxindex, "?", -1)
      }
    }
  }
}

