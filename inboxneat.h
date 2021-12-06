int neatinboxvars[INBOXMAXINBOXES][4];

void setneat(int boxindex, int x, int y, int width, int height) {
  neatinboxvars[boxindex][0] = x;
  neatinboxvars[boxindex][1] = y;
  neatinboxvars[boxindex][2] = width;
  neatinboxvars[boxindex][3] = height;
  char neatname[INBOXMAXNEATLENGTH];
  sprintf(inboxneats[boxindex], "neat %d %d %d %d", x, y, width, height);
}

void resetneat(int boxindex) {
  int x = 0, y = 0;
  int width = inboxnsizeiw[boxindex];
  int height = inboxnsizeih[boxindex];
  neatinboxvars[boxindex][0] = x;
  neatinboxvars[boxindex][1] = y;
  neatinboxvars[boxindex][2] = width;
  neatinboxvars[boxindex][3] = height;
  char neatname[INBOXMAXNEATLENGTH];
  sprintf(inboxneats[boxindex], "neat %d %d %d %d", x, y, width, height);
}

void resetneats(int min, int max) {
  int i = min;
  while (max >= min && i <= max) {
    resetneat(i);
    i++;
  }
}
