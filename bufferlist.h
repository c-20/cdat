// index can be KEYLOGBUFFERLISTINDEX or ACTIONBUFFERLISTINDEX
int drawbufferlist(int boxindex, char *bufferlist, int elementlen, int numelements) {
  int numlines = 0;
  int posx = boxes[boxindex].x;
  int posy = boxes[boxindex].y;
  int sizew = boxes[boxindex].width;
  int sizeiw = boxes[boxindex].innerwidth;
  int sizeh = boxes[boxindex].height;
  int sizeih = boxes[boxindex].innerheight;
  cursorsetpos(boxindex, posx, posy);
  cursordrawbox(boxindex, sizew, sizeh);
  int x = posx + 1;
  int y = posy + 1;
  int bli = 0, bloff = 0, bi = 0;
  while (bli < numelements) {
    bloff = bli * elementlen; // <- bufferlist can have a variable width
    bi = 0;
//    while (bufferlist[blo + bi] != '\0') {
    while (bi < elementlen && bi < sizeiw) {
      if (bufferlist[bloff + bi] < ' ' || bufferlist[bloff + bi] > '~') {
        if (bufferlist[bloff + bi] == '\0') {
          DARKOUT(boxindex, "-", -1)
        } else {
      //    REDOUT(index, "!", -1)
          REDOUT(boxindex, "$", -1)
        }
      } else {
        GREENOUT1(boxindex, "%c", bufferlist[bloff + bi], -1)
      }
      bi++;
    }
    bli++;
    cursorlinebreak(boxindex);
    numlines++;
  }
  PLAINTEXT
  cursormoveup(boxindex, numlines);
  cursorrelease(boxindex);
}

#define BUFFERLISTNUMLISTS			3
#define BUFFERLISTNUMPACKETS		5
#define BUFFERLISTPACKETSIZE		256	// STRING SIZE LIMIT!!!!
char bufferlistheldpackets[BUFFERLISTNUMLISTS][BUFFERLISTNUMPACKETS][BUFFERLISTPACKETSIZE];
int bufferlistnumheldpackets[BUFFERLISTNUMLISTS];

void addtobufferlist(int bufindex, char *fixedbuffer) {
  // shift the existing packets down the list
  char *packetlistbuffer = &bufferlistheldpackets[bufindex][0][0];
  int *packetlisttotal = &bufferlistnumheldpackets[bufindex];
  int packetlisti = *packetlisttotal - 1;
  int elementlen = BUFFERLISTPACKETSIZE;
  while (packetlisti >= 0) {
    if (packetlisti < BUFFERLISTNUMPACKETS - 1) { // -1 added 4/2/14
      int pltfrom = packetlisti * elementlen;
      int pltto = (packetlisti + 1) * elementlen;
//fprintf(stderr, "[copy from %d to %d]\n", pltfrom, pltto);
      memcpy(&packetlistbuffer[pltto], &packetlistbuffer[pltfrom], elementlen);
    }
    packetlisti--;
  }
  if (*packetlisttotal < BUFFERLISTNUMPACKETS) {
    (*packetlisttotal)++;
  }
  // add this packet to the packet list
//fprintf(stderr, "[add %d ('%s') for index %d]\n", elementlen, fixedbuffer, bufindex);
  memcpy(packetlistbuffer, fixedbuffer, elementlen);
}

#define BUFFERLISTKEYLOGCONTENTINDEX		0
#define BUFFERLISTACTIONCONTENTINDEX		1
void addtoindexedbufferlist(int bufindex, char *buffer) {
  char fixedbuffer[BUFFERLISTPACKETSIZE];
  int i = 0;
  while (i < BUFFERLISTPACKETSIZE) {
    if ((i > 0 && fixedbuffer[i - 1] == '\0') || buffer[i] == '\0') {
      fixedbuffer[i] = '\0';
    } else {
      fixedbuffer[i] = buffer[i];
    }
    i++;
  }
  addtobufferlist(bufindex, fixedbuffer);
}

void drawindexedbufferlist(int boxindex, int bufindex) {
  char *bufferlist = &bufferlistheldpackets[bufindex][0][0];
  int numpackets = bufferlistnumheldpackets[bufindex];
  int elementlen = BUFFERLISTPACKETSIZE;
  drawbufferlist(boxindex, bufferlist, elementlen, numpackets);
}
