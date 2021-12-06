char *inboxshellcolour(int boxtype) {
  if (boxtype == INBOXTYPECACHE)              { return WHITECOLOUR; }
  else if (boxtype == INBOXTYPESTRING)        { return BLUECOLOUR; }
  else if (boxtype == INBOXTYPESTDIN)         { return GREENCOLOUR; }
  else if (boxtype == INBOXTYPESOURCE)        { return REDCOLOUR; }
  else if (boxtype == INBOXTYPENEAT)          { return CYANCOLOUR; }
  else if (boxtype == INBOXTYPEHOLD)          { return MAGENTACOLOUR; }
  else if (boxtype == INBOXTYPEBACKGROUND)    { return GROUNDCOLOUR; }
  else if (boxtype == INBOXTYPELIST)          { return YELLOWCOLOUR; }
  return LIGHTCOLOUR;
}

