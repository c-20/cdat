#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <math.h>
#include <sched.h>

#define WITHNULL					+ 1

#define FILEACTIONINPUTBUFFER		"/tau/buf/page0/readinputoutput"
#define FILEACTIONINPUTCOUNTER		"/tau/buf/page0/count-readinputoutput"
#define FILEACTIONCOMMANDFILEARGS	"/tau/buf/page0/cfa"
#define FILEACTIONCOMMANDFILELIST	"/tau/buf/page0/cfl"
#define FILEACTIONCOMMANDLISTCAT	"cat /tau/buf/page0/cfl"

#include "colours.h"
#include "readfile.h"

#define MAXBOXES        100

typedef struct boxtype {
  int x, y, width, height;
  int innerwidth, innerheight;
  int keepalive, hasmoved;
  int fillleft, fillright;
  int fillabove, fillbelow;
  int backgroundpid; // move to instance later?
} box;

// note that BACKBOX is not counted in numboxes
box boxes[MAXBOXES];
int numboxes = 0;

#include "precursor.h"

#include "backbox.h"
#include "boxes.h"
#include "drawbox.h"
#include "cursor.h"
#include "keepalive.h"
#include "keylookup.h"
#include "modlookup.h"
#include "fixedges.h"
#include "outbox.h"

#define BLUETOOTHSTREAMSOURCE	"/dev/vcs6"
#define INPUTBUFFERSIZE			1024
#define OUTPUTSTREAMSIZE		1024
char inputbuffer[INPUTBUFFERSIZE + 1];
int inputbufferlen;
char outputstream[OUTPUTSTREAMSIZE + 1];
int outputstreamlen;

int bufferboxcount;
#include "bufferbox.h"
#include "bufferlist.h"
#include "indexbuffer.h"

#define LOOPTIME                10000
#define SYNCSLEEPCOUNT			20 //	25
char syncboxstate;
char commandcursorposmin;
char commandcursorposmax;
int tabmodevalue;			// set by KEYLOGBUFFERLIST, used by ACTIONBUFFERLIST
#include "syncgetline.h"

#define HOTKEYRESPONSENOACTION			0
#define HOTKEYRESPONSEREDRAWOUTBOX		1
#define HOTKEYRESPONSEDOBIGERASE		2
#define HOTKEYRESPONSEDOLINEERASE		3
#define HOTKEYRESPONSEWRITEFOOTER		4
#define HOTKEYRESPONSEREDRAWINBOX		5
#include "ejectstack.h"


#include "strequal.h"
#include "instances.h"

#define DOUBLEBUFFERVDATSIGNALINDEX		0
#include "doublebuffer.h"
#include "inbox.h"

#include "sync.h"

#include "commandcursor.h"
#include "hotkey.h"

#include "handlekeydown.h"
#include "writekeycode.h"
//#include "varbox.h"
#include "keyout.h"

/////////#define PACKETSIZE              328
#define BUFFERSIZE              1640
#define BUFFERLEN               1641
#define MAXPACKETS              10
#define MESSAGEOFFSET           172
#define MESSAGESIZE             50//29
#define MESSAGELEN              51//30
#define MODIFIERSTART           9//6
#define MODIFIEREND             10//7
#define MODIFIERSIZE			2
#define USEFULSTART             12//15//12
#define USEFULEND               28//31//28
#define USEFULSIZE              17//43
#define PACKETLEN				50
#define KEYSIZE                 2
#define MAXKEYS                 12
#define MAXVALIDKEYS			6
#define LINESIZE                60
//#define KEYARRSTARTBASE			13
#define KEYARRSTARTBASEOFFSET	0

#define NUMHELDPACKETSTOTAL		10
#define NUMHELDPACKETSLIMIT		9

//#define INTMAX(int1, int2)							\
//		  ((int1 > int2) ? int1 : (int2 > int1) ? int2 : int1)
typedef struct {
  int widthpx, heightpx;
  int rows, cols;
  int redrawdue;
} screen_s;
screen_s cdatscreen;

void resizedatscreen(int sig) {
  struct winsize ws;
  ioctl(0, TIOCGWINSZ, &ws);
  cdatscreen.cols = ws.ws_col - 1;
  cdatscreen.rows = ws.ws_row - 1;
  cdatscreen.redrawdue = 1;
}

int main(int argc, char **argv) {
  resizedatscreen(0);
  cdatscreen.redrawdue = 0;
  signal(SIGWINCH, resizedatscreen);

  MODSTRINGSDEF
//  KEEPALIVEDEF
  int fd = open(BLUETOOTHSTREAMSOURCE, O_RDONLY);
  char dbuf1[BUFFERLEN];
  char dbuf2[BUFFERLEN];
  char *dfront, *dback, *dswap;
  int messagecount = 0;
  int syncsleepcount;
  char packet[PACKETLEN];
  int hotkeyresponse = HOTKEYRESPONSENOACTION;
//  int keyarrstart = KEYARRSTARTBASE;
//  int keyarrstartoffset = KEYARRSTARTBASEOFFSET;
  int keyfound = 0;
  int keysingle = 1;
  int keydownstart = -1;
  char keydown[KEYSIZE];
  keydown[0] = '0';
  keydown[1] = '0';
  int keyupstart = -1;
  char keyup[KEYSIZE];
  keyup[0] = 'F';
  keyup[1] = 'F';
  int keyarrlen = 0;
  int keycanpush = 0;
  int keycanpop = 0;
  int keypopindex = -1;

  int prevmods[MODIFIERSIZE];
  prevmods[0] = '0';
  prevmods[1] = '0';
  char *modstring = "";
  int prevlastshiftstate = 0;
  int lastshiftstate = 0;
  int shiftstate = 0;
  int ctrlstate = 0;
  int altstate = 0;
  int cmdstate = 0;

  char pendingclearscreen = 'n';
  char pendingpopejectstack = 'n';
  char pendingquitdrplinux = 'n';

  bufferboxcount = 0;

  int firstwrite = 1;
  int autoexecloadparam = 0;

  syncboxstate = '!';

  inputbufferlen = 0;
  inputbuffer[INPUTBUFFERSIZE] = '\0';
  inputbuffer[inputbufferlen] = '\0';

  outputstreamlen = 0;
  outputstream[OUTPUTSTREAMSIZE] = '\0';
  outputstream[outputstreamlen] = '\0';

//  char heldpackets[NUMHELDPACKETSTOTAL][PACKETLEN];
//  int numheldpackets = 0;

  // declare that the keylog bufferlist is empty
  bufferlistnumheldpackets[0] = 0;

  // used by KEYLOGBUFFERLIST and ACTIONBUFFERLIST
  tabmodevalue = 0;
  int tabmodekeyindex = 0;
  char tabmodekeystream[(BUFFERLISTNUMPACKETS << 1) + 1];

  int x, y;
  char keylist[MAXKEYS][KEYSIZE];
  char equlist[MAXKEYS][KEYSIZE];
  char oldlist[MAXKEYS][KEYSIZE];
  char keysdown[MAXKEYS][KEYSIZE];
  char keysup[MAXKEYS][KEYSIZE];
  for (x = 0; x < MAXKEYS; x++) {
    for (y = 0; y < KEYSIZE; y++) {
      keylist[x][y] = '?';
      equlist[x][y] = 'x';
      oldlist[x][y] = 'F';
      keysdown[x][y] = '-';
      keysup[x][y] = '-';
    }
  }
  int keysdownlen = 0;
  int keysuplen = 0;

  dfront = dbuf1;
  dback = dbuf2;
  dback[0] = '\0';

  argc--; argv = &argv[1];

/* Now with dynamic sizing
  int screenwidth = SCREENWIDTH;
  int screenheight = SCREENHEIGHT;
  // --------------------------------------------------------
  // read resolution parameter if available
  if (argc >= 1 && argv[0][0] == '-' && argv[0][1] == 'r') {
    if (argc >= 2) {
      int ri = 0;
      int rs = 'w';
      int neww = 0;
      int newh = 0;
      while (argv[1][ri] != '\0') {
        if (argv[1][ri] == 'x') {
          rs = 'h';
        } else if (argv[1][ri] >= '0' && argv[1][ri] <= '9') {
          if (rs == 'w') {
            neww = (neww * 10) + (int)(argv[1][ri] - '0');
          } else if (rs == 'h') {
            newh = (newh * 10) + (int)(argv[1][ri] - '0');
          }
        } else break;
        ri++;
      }
      screenwidth = neww;
      screenheight = newh;
      argc -= 2; argv = &argv[2];
    } else {
      fprintf(stderr, "Undefined screen resolution.\n");
      argc--; argv = &argv[1];
    }
  }
  // --------------------------------------------------------
  // translate resolution from pixels to characters
#include "restranslate.h"
  // a -g parameter will override all this
  // --------------------------------------------------------
  // read grid parameter if available
  if (argc >= 1 && argv[0][0] == '-' && argv[0][1] == 'g') {
    if (argc >= 2) {
      int ri = 0;
      int rs = 'w';
      int neww = 0;
      int newh = 0;
      while (argv[1][ri] != '\0') {
        if (argv[1][ri] == 'x') {
          rs = 'h';
        } else if (argv[1][ri] >= '0' && argv[1][ri] <= '9') {
          if (rs == 'w') {
            neww = (neww * 10) + (int)(argv[1][ri] - '0');
          } else if (rs == 'h') {
            newh = (newh * 10) + (int)(argv[1][ri] - '0');
          }
        } else break;
        ri++;
      }
      screenwidth = neww;
      screenheight = newh - 1;
      argc -= 2; argv = &argv[2];
    } else {
      fprintf(stderr, "Undefined grid resolution.\n");
      argc--; argv = &argv[1];
    }
  }
*/
  // --------------------------------------------------------
  // enable autoexec load parameter if included
  if (argc >= 1 && argv[0][0] == '-' && argv[0][1] == 'l' && argv[0][2] == '\0')
    { autoexecloadparam = 1; argc--; argv = &argv[1]; }
  if (syncstartup() != 'y')
    { fprintf(stdout, "syncbox failed to start\n"); return 1; }
#include "autoexec.h"

  int samplei = 0;
  int samplen = 10; // NUMBER OF SAMPLES
  char dread[BUFFERLEN];
  int dreadi = 0;
  int dreado = 0;
  int dfronti = 0;

  int lastpacketfound = 'n';
  char *lastfrontpacket = &dfront[BUFFERSIZE];
  char *lastbackpacket = &dback[BUFFERSIZE];
  int lastpacketfindex = 0;
  int lastpacketbindex = 0;
  int thispacketindex = -1;

  int apc = 0;
  while (1) {
  //  if (cdatscreen.resizedue == 1) {
//////////////      

    dfront[BUFFERSIZE] = '\0';
    dback[BUFFERSIZE] = '\0';
    // read BUFFERSIZE chars into dfront
    // sample up to n times and store valid chars
    // consecutive whitespace is ignored
    lastpacketfound = 'n';
    thispacketindex = -1;
    samplei = 0;
    dreado = 0;
    dfronti = BUFFERSIZE - 1;
    while (dfronti >= 0 && samplei < samplen) {
      lseek(fd, -(BUFFERSIZE + dreado), SEEK_END);
      read(fd, dread, BUFFERSIZE);
      dreadi = BUFFERSIZE - 1;
      while (dreadi >= 0 && lastpacketfound != 'a') {
        // ignore ]32-126[
        if (dread[dreadi] >= ' ' && dread[dreadi] <= '~') {
          // ignore consecutive spaces
          if (dread[dreadi] != ' ' || dfront[dfronti + 1] != ' ') {
            // mangle packet delimeter
            if ((dread[dreadi] == '>' ||
                 dread[dreadi] == '<' ||
                 dread[dreadi] == '@') &&
                 dfront[dfronti + 1] == ' ') {
              if (strncmp(" Connect Failed", &dfront[dfronti + 1], 15) == 0) {
                cursormoveright(BACKCURSORINDEX, 40);
                REDOUT(BACKCURSORINDEX, "KEYBOARD FAILED TO CONNECT", -26)
                cursormoveleft(BACKCURSORINDEX, 66);
              } else if (strncmp(" ACL Data RX", &dfront[dfronti + 1], 12) == 0) {
                dfront[dfronti] = '\n';
                thispacketindex++;
                dfront[dfronti + 1] = '~';
                // if no search has been successful so far
                if (lastpacketfound == 'n') {
                  int fbmi = 0;
                  // compare packets
                  while (dfront[dfronti + 1 + fbmi] != '\0' &&
                         dfront[dfronti + 1 + fbmi] != '\n') {
                    if (dback[lastpacketbindex + fbmi] == '\0' ||
                        dback[lastpacketbindex + fbmi] == '\n') {
                      fbmi = -1;
                      break;
                    }
                    if (dfront[dfronti + 1 + fbmi] !=
                        dback[lastpacketbindex + fbmi]) {
                      fbmi = -1;
                      break;
                    }
                    fbmi++;
                  }
                  // if no break encountered and back + front end at the same time
                  if (fbmi != -1 && (dback[lastpacketbindex + fbmi] == '\0' ||
                                     dback[lastpacketbindex + fbmi] == '\n')) {
                    // a match was found
                    if (thispacketindex == 0) {
                      // the data has not changed - stop reading
                      lastpacketfound = 'a'; // (abort)
                    } else if (thispacketindex == 1) {
                      // a wild packet appears...
                      dfront[dfronti + 1 + fbmi] = '\0';
                      lastpacketfound = 'y';
                      lastfrontpacket = &dfront[dfronti + 1];
                      lastpacketfindex = dfronti + 1;
                    } else if (thispacketindex > 1) {
                      // pretend we only saw this one
                      // the rest can be handled in future cycles
                      dfront[dfronti + 1 + fbmi] = '\0';
                      lastpacketfound = 'y';
                      lastfrontpacket = &dfront[dfronti + 1];
                      lastpacketfindex = dfronti + 1;
                    }
                  } else if (fbmi == -1 && lastbackpacket[0] == '\0') {
                    // make an exception for the first packet
                    // pretend we only saw this one
                    // the rest can be handled in future cycles
                    dfront[dfronti + 1 + fbmi] = '\0';
                    lastpacketfound = 'y';
                    lastfrontpacket = &dfront[dfronti + 1];
                    lastpacketfindex = dfronti + 1;
//  drawbufferbox(0, lastfrontpacket);
                  }
                }
              } else {
//                cursormoveright(BACKCURSORINDEX, 40);
//                REDOUT(BACKCURSORINDEX, "UNRECOGNISED PACKET HEADER", -26)
//                cursormoveleft(BACKCURSORINDEX, 66);
              }
            } else {
              dfront[dfronti] = dread[dreadi];
            }
            dfronti--;
            if (dfronti < 0) break;
          }
        }
        dreadi--;
      }
      dreado += BUFFERSIZE;
      samplei++;
    }
    dfront[BUFFERSIZE] = '\0';
//    drawbufferbox(0, dfront);
/*
    // find last back packet (or point to end if none)
    int lastbacktilde = (lastpacketbindex == -1) ? -1 : BUFFERSIZE - 1;
    while (lastbacktilde >= 0 && dback[lastbacktilde] != '~') {
      lastbacktilde--;
    }
    if (lastbacktilde < 0) {
      lastbackpacket = &dback[BUFFERSIZE];
    } else {
      lastbackpacket = &dback[lastbacktilde];
      lastpacketbindex = lastbacktilde;
    }
*/
    // if a packet change was detected (or no record in back buffer)
    if (lastpacketfound == 'y' && (thispacketindex > 0 || lastbackpacket[0] == '\0')) {
//    if (lastpacketfound == 'y') {
/*
    char *lastfrontpacket = &dfront[lastfronttilde];
    char *lastbackpacket = &dback[lastbacktilde];
    if (strncmp(lastfrontpacket, lastbackpacket, BUFFERSIZE - lastfronttilde) != 0) {
*/
//---------------------------------------------------------------------------
// Header is "> ACL Data RX: Handle %d" - keyboard had handle 72 then 71
// Should probably check the handle if one wants nultiplayer support
//---------------------------------------------------------------------------
      char *detectedpacketheader = "~ACL Data RX";
      int detectedpacketheaderlen = 12;
      int alignedpacket = (strncmp(lastfrontpacket, detectedpacketheader, detectedpacketheaderlen) == 0);
      if (!alignedpacket) {
        REDOUT1(BACKCURSORINDEX, "!! UNALIGNED PACKET %3d !!", apc, -26)
        cursorrelease(BACKCURSORINDEX);
        apc++;
      }
      // extract useful information
      int i, j;
      char channelindex = '?';
      int bufmessagelen = 0;
      if (alignedpacket) {
        char buf[MESSAGELEN];
//        keyarrstartoffset = KEYARRSTARTBASEOFFSET;
        int keyarrstartoffset = 0;
        while (lastfrontpacket[keyarrstartoffset] != '\0') {
          if (bufmessagelen > 0 ||
              (keyarrstartoffset >= 7 && lastfrontpacket[keyarrstartoffset] == '}')) {
            char *lfph = &lastfrontpacket[keyarrstartoffset - 7];
            if (bufmessagelen > 0 ||
                (lfph[0] == '{' && lfph[1] == 'c' && lfph[2] == 'h' && lfph[3] == 'a' &&
                 lfph[4] == 'n' && lfph[5] == ' ' && lfph[6] >= '0' && lfph[6] <= '9' && lfph[7] == '}')) {
              if (channelindex == '?') channelindex = lfph[6];
              bufmessagelen++;
              if (lastfrontpacket[keyarrstartoffset] == '.') {
                strncpy(buf, &lastfrontpacket[keyarrstartoffset - bufmessagelen], bufmessagelen + 1);
                buf[bufmessagelen + 1] = '\0';
//                drawbufferbox(0, buf);
                break;
              }
            }
          }
          keyarrstartoffset++;
        }
        buf[MESSAGESIZE] = '\0';
        if (lastfrontpacket[keyarrstartoffset] == '\0') {
          MAGENTAOUT1(BACKCURSORINDEX, "!! MESSAGE NOT FOUND @ %5d !!", bufmessagelen, -31)
          cursorrelease(BACKCURSORINDEX);
//          drawbufferbox(0, lastfrontpacket);
        }
/*
        // packet format: MODS ?? KEYS ?? ?? ?? ?? ?? ??
        int ml = MODIFIERSIZE;//MODIFIEREND - MODIFIERSTART + 1;
        char mods[MODIFIERSIZE + 1];
        strncpy(mods, &buf[MODIFIERSTART], ml);
        int kl = USEFULSIZE;//USEFULEND - USEFULSTART + 1;
        char keys[USEFULSIZE + 1];
        strncpy(keys, &buf[USEFULSTART], kl);
*/


//        drawbufferbox(0, &buf[USEFULSTART]);

        if (buf[USEFULSTART] == '.') {
          sprintf(packet, "CH %c MODS %c%c KEYS                  ",
                  channelindex, buf[9], buf[10]);
        } else {
          sprintf(packet, "CH %c MODS %c%c KEYS %c%c %c%c %c%c %c%c %c%c %c%c",
                  channelindex, buf[9], buf[10], buf[15], buf[16], buf[18], buf[19],
                  buf[21], buf[22], buf[24], buf[25], buf[27], buf[28], buf[30], buf[31]);
        }
        char *mods = &packet[10];
        char *keys = &packet[18];

//        drawbufferbox(0, keys);

/*
        strncpy(&packet[0], "MODS ", 5);
        strncpy(&packet[5], mods, ml);
        strncpy(&packet[5 + ml], " KEYS ", 6);
        strncpy(&packet[11 + ml], keys, kl);
        packet[11 + ml + kl] = '\0';
*/
//        drawbufferbox(0, packet);
/*
        // shift the existing packets down the list
        int packetlisti = numheldpackets - 1;
        while (packetlisti >= 0) {
          if (packetlisti < NUMHELDPACKETSLIMIT) {
            int plti = 0;
            while (heldpackets[packetlisti][plti] != '\0') {
              heldpackets[packetlisti + 1][plti] = heldpackets[packetlisti][plti];
              plti++;
            }
            heldpackets[packetlisti + 1][plti] = '\0';
          }
          packetlisti--;
        }
        if (numheldpackets < NUMHELDPACKETSTOTAL) {
          numheldpackets++;
        }
        // add this packet to the packet list
        sprintf(heldpackets[0], "%s", packet);
//        drawbufferlist(0, &heldpackets[0][0], PACKETLEN, numheldpackets);
*/
//-------------------------------------------------------------
// KEYOUT STARTS HERE
//-------------------------------------------------------------
//-------------------------------------------------------------
// KEYBOX STARTS HERE
//-------------------------------------------------------------
#ifdef KEYOUTENABLED
        drawkeyoutbox();
// ^^^^^ code below to fill keyoutbox needs to be in this function
        cursorswap(KEYBOXINDEX, KEYOUTINDEX);
        fillkeyoutbox(inputbuffer, &syncboxstate, commandcursorposmin, commandcursorposmax);
        cursorswap(KEYOUTINDEX, KEYBOXINDEX);
#endif
#ifdef KEYBOXENABLED
        cursordrawbox(KEYBOXINDEX, KEYBOXSIZEW, KEYBOXSIZEH);
        YELLOWOUT(KEYBOXINDEX, "-i-=[", -5)
        GREENOUT(KEYBOXINDEX, "MODS ", -5)
#endif
//        int keyslen = USEFULSIZE;
        int keysblank = (strncmp(keys, "                 ", USEFULSIZE) == 0) ? 1 : 0;
        int keyszero  = (strncmp(keys, "00 00 00 00 00 00", USEFULSIZE) == 0) ? 1 : 0;
        int keyserror = (strncmp(keys, "01 01 01 01 01 01", USEFULSIZE) == 0) ? 1 : 0;
        if (keyserror) modstring = MODSTRINGERROR;
        else modstring = MODSTRINGEMPTY;
        if (keysblank || keyszero) {
          int keymodtype = (keysblank) ? 2 : (keyszero) ? 1 : 0;
          char *modstringpair[2];
          modlookup(mods[0], mods[1], keymodtype, modstringpair, shiftstate);
          if (modstringpair[1] == MODSTRINGCLEARSCREEN) {
            pendingclearscreen = 'y';
          } else if (modstringpair[1] == MODSTRINGPOPEJECTSTACK) {
            pendingpopejectstack = 'y';
          } else if (modstringpair[1] == MODSTRINGQUITDRPLINUX) {
            pendingquitdrplinux = 'y';
          }
          modstring = modstringpair[0];
#ifdef KEYBOXENABLED
          WHITEOUT2(KEYBOXINDEX, "%c%c ", prevmods[0], prevmods[1], -3)
          YELLOWOUT(KEYBOXINDEX, "-> ", -3)
          WHITEOUT2(KEYBOXINDEX, "%c%c ", mods[0], mods[1], -3)
          if (PAIREQUALS(mods, '0','0')) {
            CYANOUT1(KEYBOXINDEX, "%s", modstring, -MODSTRINGSIZE)
          } else {
            GREENOUT1(KEYBOXINDEX, "%s", modstring, -MODSTRINGSIZE)
          }
#endif
#ifdef KEYOUTENABLED
          cursorswap(KEYBOXINDEX, KEYOUTINDEX);
          YELLOWOUT1(KEYOUTINDEX, "%s", modstringpair[0], -MODSTRINGSIZE)
//          cursorlinebreak(KEYOUTINDEX);
          YELLOWOUT1(KEYOUTINDEX, " %s", modstringpair[1], -(MODSTRINGSIZE + 1))
          cursorlinebreak(KEYOUTINDEX);
          cursorswap(KEYOUTINDEX, KEYBOXINDEX);
#endif
          prevmods[0] = mods[0];
          prevmods[1] = mods[1];
// NOTE THAT SHIFTSTATE SEEMS TO BE USED FOR MODLOOKUP BEFORE IT IS DECLARED ^^^^^^^ COULD BE OLD STATE
          prevlastshiftstate = lastshiftstate;
          lastshiftstate = shiftstate;
          shiftstate = (keymodtype == 1) ? getshiftstate(mods) : 0;
          ctrlstate = (keymodtype == 1) ? getctrlstate(mods) : 0;
          altstate = (keymodtype == 1) ? getaltstate(mods) : 0;
          cmdstate = (keymodtype == 1) ? getcmdstate(mods) : 0;
        } else {
#ifdef KEYBOXENABLED
          DARKOUT2(KEYBOXINDEX, "%c%c ", prevmods[0], prevmods[1], -3)
          LIGHTOUT(KEYBOXINDEX, "-> ", -3)
          DARKOUT2(KEYBOXINDEX, "%c%c ", mods[0], mods[1], -3)
          CYANOUT1(KEYBOXINDEX, "%s", MODSTRINGEMPTY, -MODSTRINGSIZE)
#endif
#ifdef KEYOUTENABLED
          cursorswap(KEYBOXINDEX, KEYOUTINDEX);
          CYANOUT1(KEYOUTINDEX, "%s", MODSTRINGEMPTY, -MODSTRINGSIZE)
//          cursorlinebreak(KEYOUTINDEX);
          CYANOUT1(KEYOUTINDEX, " %s", MODSTRINGEMPTY, -(MODSTRINGSIZE + 1))
          cursorlinebreak(KEYOUTINDEX);
          cursorswap(KEYOUTINDEX, KEYBOXINDEX);
#endif
        }
#ifdef KEYBOXENABLED
        YELLOWOUT(KEYBOXINDEX, "]=-i-", -5)
        cursorlinebreak(KEYBOXINDEX);
#endif
        // double-buffering watches the packet log for changes
        int dfrontpackets[MAXPACKETS];
        int dbackpackets[MAXPACKETS];
        int dfp = 0, dbp = 0;
        // find packet locations in front and back buffers
        for (i = BUFFERSIZE - 1; i >= 0; i--)
        { if (strncmp(&dfront[i], detectedpacketheader, detectedpacketheaderlen) == 0)
          { dfrontpackets[dfp++] = i;              } }
        for (i = BUFFERSIZE - 1; i >= 0; i--)
        { if (strncmp(&dback[i], detectedpacketheader, detectedpacketheaderlen) == 0)
          { dbackpackets[dbp++] = i;              } }
        // log packet and message counts from the double buffer
        messagecount = (messagecount + 1) % 1000000;
#ifdef KEYBOXENABLED
        YELLOWOUT(KEYBOXINDEX, "-s-=[", -5)
        WHITEOUT(KEYBOXINDEX, "BP=", -3)
        YELLOWOUT1(KEYBOXINDEX, "%-2d ", dbp, -3)
        WHITEOUT(KEYBOXINDEX, "FP=", -3)
        if (dfp == dbp) {
          GREENOUT1(KEYBOXINDEX, "%-2d ", dfp, -3)
        } else {
          REDOUT1(KEYBOXINDEX, "%-2d ", dfp, -3)
        }
        WHITEOUT(KEYBOXINDEX, "MC=", -3)
        GREENOUT1(KEYBOXINDEX, "%-6d ", messagecount, -7)
#endif
        // assume we watched a single movement
        keyfound = 1;
        keysingle = 1;
        strncpy(dback, dfront, BUFFERSIZE);
        lastpacketbindex = lastpacketfindex;
/*
        // determine if we successfully watched a single movement
        for (i = 0; i < dfp; i++) {
          keyfound = 0;
          keysingle = 1;
          for (j = 0; j < dbp && !keyfound; j++) {
            

//            if (strncmp(&dfront[dfrontpackets[i]],
//                        &dback[dbackpackets[j]], PACKETSIZE) == 0)
            if (strncmp(
            { keyfound = 1;
              if (i - j != 1) keysingle = 0;                     } } }
        if (keysingle && !keyserror) {
#ifdef KEYBOXENABLED
          GREENOUT(KEYBOXINDEX, (keyfound) ? " NXT" : " 1ST", -4)
#endif
          strncpy(dback, dfront, BUFFERSIZE);
        } else {
          if (!keysingle) {
#ifdef KEYBOXENABLED
            YELLOWOUT(KEYBOXINDEX, " MUL", -4)
#endif
          }
          if (keyserror) {
#ifdef KEYBOXENABLED
            REDOUT(KEYBOXINDEX, " END", -4)
#endif
          }
        }
*/
// ---------------------------------------------------------------------
        hotkeyresponse = HOTKEYRESPONSENOACTION;
        //keyarrstartoffset = KEYARRSTARTBASEOFFSET;
        keycanpush = (keyarrlen >= 0 && keyarrlen < MAXKEYS) ? 1 : 0;
        keycanpop = (keyarrlen > 0 && keyarrlen <= MAXKEYS) ? 1 : 0;
        if (keyfound && keycanpush) {
          keydownstart = keyarrlen * 3;
          keydown[0] = keys[keydownstart];
          keydown[1] = keys[keydownstart + 1];
          if (PAIREQUALS(keydown, ' ',' '))
          { keydown[0] = '0';
            keydown[1] = '0';             }
          if (!PAIREQUALS(keydown, '0','0')) {
            keylist[keyarrlen][0] = keydown[0];
            keylist[keyarrlen][1] = keydown[1];
            for (x = 0; x < MAXKEYS; x++) {
              if (PAIRSEQUAL(keysup[x], keydown)) {
#ifdef KEYBOXENABLED
                YELLOWOUT(KEYBOXINDEX, " REPUSH", -7)
#endif
                if (syncboxstate == 'S' && tabmodevalue == 0) {
#ifdef OUTBOXENABLED
                  cursorswap(KEYBOXINDEX, OUTBOXINDEX);
                  handlekeydown(keydown, shiftstate, hotkeyresponse);
                  cursorswap(OUTBOXINDEX, KEYBOXINDEX);
#endif
                }
                keysdown[x][0] = keydown[0];
                keysdown[x][1] = keydown[1];
                keysup[x][0] = '-';
                keysup[x][1] = '-';
                keydown[0] = '!';
                keydown[1] = '!';
              }
            }
            if (!PAIREQUALS(keydown, '!','!')) {
#ifdef KEYBOXENABLED
              GREENOUT(KEYBOXINDEX, " NEWPUSH", -8)
#endif
              hotkeyresponse = hotkeylookup(keydown,
                shiftstate, ctrlstate, altstate, cmdstate);
              if (syncboxstate == 'S' && tabmodevalue == 0) {
#ifdef OUTBOXENABLED
                cursorswap(KEYBOXINDEX, OUTBOXINDEX);
                handlekeydown(keydown, shiftstate, hotkeyresponse);
                if (hotkeyresponse == HOTKEYRESPONSEREDRAWOUTBOX) {
                  cursordetour(OUTBOXINDEX, keepalives[OUTBOX.keepalive].x, keepalives[OUTBOX.keepalive].y);
                  drawoutbox(outputstream, outputstreamlen);
                  cursortour(OUTBOXINDEX, keepalives[OUTBOX.keepalive].x, keepalives[OUTBOX.keepalive].y);
// added june 2014
                  cursorsavepos(OUTBOXINDEX);
                }
                cursorswap(OUTBOXINDEX, KEYBOXINDEX);
#endif
              } else if (syncboxstate == 'C' && tabmodevalue == 0) {
                cursorswap(KEYBOXINDEX, INBOXINDEX);
                if (hotkeyresponse == HOTKEYRESPONSEREDRAWINBOX) {
                  drawinbox(INBOXMAININBOXINDEX, INBOXTYPESTDIN, inputbuffer, -1);
                }
#include "command.h"
                cursorswap(INBOXINDEX, KEYBOXINDEX);
              }
/*
              } else if (syncboxstate == 'W') {
                cursorswap(KEYBOXINDEX, KEYOUTINDEX);
                cursormoveup(KEYOUTINDEX, 1);
                int bbi = 0, bli = 0;
                while (bli < KEYOUTSIZEW - 2) {
                  if (inputbuffer[bbi] != '\0') {
                    GREENOUT1(KEYOUTINDEX, "%c", inputbuffer[bbi], -1);
                    bbi++;
                  } else {
                    OUT(KEYOUTINDEX, " ", -1)
                  }
                  bli++;
                }
                cursormoveleft(KEYOUTINDEX, KEYOUTSIZEW - 2);
                cursormovedown(KEYOUTINDEX, 1);
                cursorswap(KEYOUTINDEX, KEYBOXINDEX);
              }
*/
/*
#ifdef OUTBOXENABLED
              if (hotkeyresponse == HOTKEYRESPONSEREDRAWOUTBOX) {
                cursorswap(KEYBOXINDEX, OUTBOXINDEX);
                cursordetour(OUTBOXINDEX, keepalives[OUTBOX.keepalive].x, keepalives[OUTBOX.keepalive].y);
                drawoutbox(outputstream, outputstreamlen);
                cursortour(OUTBOXINDEX, keepalives[OUTBOX.keepalive].x, keepalives[OUTBOX.keepalive].y);
                cursorswap(OUTBOXINDEX, KEYBOXINDEX);
              }
#endif
*/
//              int newkeyindex = INTMAX(keysdownlen, keysuplen);
              int newkeyindex = keysdownlen + keysuplen;
              keysdown[newkeyindex][0] = keydown[0];
              keysdown[newkeyindex][1] = keydown[1];
            }
            if (keyarrlen < MAXKEYS - 2) {
              keyarrlen++;
            }
          }
        }
#ifdef KEYBOXENABLED
        YELLOWOUT(KEYBOXINDEX, "]=-s-", -5)
        cursorlinebreak(KEYBOXINDEX);
        // display oldlist
        YELLOWOUT(KEYBOXINDEX, "-1-=[", -5)
        for (x = 0; x < MAXKEYS; x++) {
          for (y = 0; y < KEYSIZE; y++) {
            WHITEOUT1(KEYBOXINDEX, "%c", oldlist[x][y], -1)
          }
        }
        YELLOWOUT(KEYBOXINDEX, "]=-1-", -5)
        cursorlinebreak(KEYBOXINDEX);
        YELLOWOUT(KEYBOXINDEX, "-2-=[", -5)
#endif
        int foundpop = 0;
        int lostpop = 0;
        char *pchar;
        for (x = 0; x < keyarrlen; x++) {
//          pchar = &packet[keyarrstart + keyarrstartoffset + (x * 3)];
          pchar = &keys[x * 3];
          if (pchar[0] == 0 || pchar[1] == 0) {
            pchar[0] = '?';
            pchar[1] = '?';
          }
#ifdef KEYBOXENABLED
          MAGENTAOUT(KEYBOXINDEX, "(", -1)
#endif
          if (PAIRSNEQUAL(pchar, keylist[x])) {
            equlist[x][0] = '!';
            equlist[x][1] = 'X';
            int equfound = -1;
            for (y = 0; y < keyarrlen; y++)
            { if (PAIRSEQUAL(pchar, keylist[y]))
              { equfound = y;
                equlist[x][1] = '0' + y;     } }
#ifdef KEYBOXENABLED
            WHITEOUT2(KEYBOXINDEX, "%c%c", pchar[0], pchar[1], -2)
#endif
            if (equfound >= 0) {
#ifdef KEYBOXENABLED
              GREENOUT(KEYBOXINDEX, "*", -1)
#endif
              foundpop++;
            } else {
#ifdef KEYBOXENABLED
              REDOUT(KEYBOXINDEX, "^", -1)
#endif
              lostpop++;
            }
            keylist[x][0] = pchar[0];
            keylist[x][1] = pchar[1];
            pchar += 3;
          } else {
            equlist[x][0] = '=';
            equlist[x][1] = '0' + x;
#ifdef KEYBOXENABLED
            LIGHTOUT2(KEYBOXINDEX, "%c%c", pchar[0], pchar[1], -2);
            GREENOUT(KEYBOXINDEX, "=", -1)
#endif
          }
        }
#ifdef KEYBOXENABLED
        YELLOWOUT(KEYBOXINDEX, "]=-2-", -5)
        cursorlinebreak(KEYBOXINDEX);
        YELLOWOUT(KEYBOXINDEX, "-3-=[", -5)
#endif
        int pushconfirmed = (keydown[0] != '0' || keydown[1] != '0') ? 1 : 0;
        int newpushconfirmed = (pushconfirmed && PAIRNEQUALS(keydown, '!','!'));
        int repushconfirmed = (pushconfirmed && PAIREQUALS(keydown, '!','!'));
        int goodpopconfirmed = (lostpop == 1 && foundpop >= 0) ? 1 : 0;
        int badpopconfirmed = (lostpop > 0 && !goodpopconfirmed) ? 1 : 0;
        int errorconfirmed = (!pushconfirmed && !goodpopconfirmed
                              && !badpopconfirmed                ) ? 1 : 0;
        int nopopconfirmed = !goodpopconfirmed && !badpopconfirmed;
#ifdef KEYBOXENABLED
        for (x = 0; x < keyarrlen; x++) {
          MAGENTAOUT(KEYBOXINDEX, "(", -1)
          WHITEOUT2(KEYBOXINDEX, "%c%c", equlist[x][0], equlist[x][1], -2)
          if (goodpopconfirmed && equlist[x][0] == '!') {
            if (equlist[x][1] == 'X') {
              REDOUT(KEYBOXINDEX, "!", -1)
            } else { GREENOUT(KEYBOXINDEX, "!", -1) }
          } else { CYANOUT(KEYBOXINDEX, "!", -1) }
        }
        YELLOWOUT(KEYBOXINDEX, "]=-3-", -5)
        cursorlinebreak(KEYBOXINDEX);
#endif
// ---------------------------------------------------------------
// KEYSINFO
// ---------------------------------------------------------------
#ifdef KEYBOXENABLED
        YELLOWOUT(KEYBOXINDEX, "-o-=[", -5)
#endif
        if (!keysingle || keyserror) {
          if (!keysingle) {
#ifdef KEYBOXENABLED
            YELLOWOUT(KEYBOXINDEX, "SPLASH", -6)
#endif
          }
          if (keyserror) {
#ifdef KEYBOXENABLED
            GREENOUT(KEYBOXINDEX, "TRIGGER", -7)
#endif
          }
        }
        if (goodpopconfirmed) {
          char iterations = 0;
          x = keyarrlen - 1;
          keyup[0] = oldlist[x][0];
          keyup[1] = oldlist[x][1];
          while (++iterations) {
            for (y = 0; y < keyarrlen; y++) {
              if (equlist[y][0] == '!' && (equlist[y][1] - '0') == x) {
#ifdef KEYBOXENABLED
                CYANOUT2(KEYBOXINDEX, "<<%d -> %d>> ", x, y, -11)
#endif
                keyup[0] = oldlist[y][0];
                keyup[1] = oldlist[y][1];
                oldlist[y][0] = oldlist[x][0];
                oldlist[y][1] = oldlist[x][1];
                x = y;
                break;
              }
            }
            if (y == keyarrlen)
              break;
          }
          keyarrlen--;
        } else if (lostpop > 1) {
#ifdef KEYBOXENABLED
          REDOUT2(KEYBOXINDEX, "BADPOP %dmv %drm ", foundpop, lostpop, -15)
#endif
        } else {
#ifdef KEYBOXENABLED
          WHITEOUT2(KEYBOXINDEX, "NOPOP %dmv %drm ", foundpop, lostpop, -14)
#endif
        }
        if (pushconfirmed && nopopconfirmed) {
          // this is a push
          strncpy(&oldlist[0][0], &keylist[0][0], keyarrlen * KEYSIZE);
        } else if (!nopopconfirmed && !pushconfirmed) {
          // this is a pop
          for (x = 0; x < keyarrlen; x++)
          { for (y = 0; y < KEYSIZE; y++)
            { oldlist[x][y] = keylist[x][y]; } }
          oldlist[keyarrlen][0] = 'F';
          oldlist[keyarrlen][1] = 'F';
          for (x = 0; x < MAXKEYS; x++) {
            if (PAIRSEQUAL(keysdown[x], keyup)) {
              keysup[x][0] = keysdown[x][0];
              keysup[x][1] = keysdown[x][1];
              keysdown[x][0] = '-';
              keysdown[x][1] = '-';
            }
          }
        } else {
          // this is a problem
          keyserror = 1;
#ifdef KEYBOXENABLED
          CYANOUT(KEYBOXINDEX, "PUSHPOP ", -8)
          if (pushconfirmed) {
            GREENOUT(KEYBOXINDEX, "T", -1)
          } else { REDOUT(KEYBOXINDEX, "F", -1) }
          if (!nopopconfirmed) {
            GREENOUT(KEYBOXINDEX, "T", -1)
          } else { REDOUT(KEYBOXINDEX, "F", -1) }
#endif
        }
#ifdef KEYBOXENABLED
        YELLOWOUT(KEYBOXINDEX, "]=-o-", -5)
        cursorlinebreak(KEYBOXINDEX);
#endif
// ---------------------------------------------------------------
// KEYSDOWN
// ---------------------------------------------------------------
/*
#ifdef KEYOUTENABLED
        cursorswap(KEYBOXINDEX, KEYOUTINDEX);
        cursormovedown(KEYOUTINDEX, 1);
        cursorswap(KEYOUTINDEX, KEYBOXINDEX);
#endif
*/
        char key;
        keysdownlen = 0;
        for (x = 0; x < MAXKEYS; x++) {
#ifdef KEYBOXENABLED
          writekeycode(KEYBOXINDEX, keysdown[x], 0);
#endif
          if (x < MAXVALIDKEYS) {
#ifdef KEYOUTENABLED
            cursorswap(KEYBOXINDEX, KEYOUTINDEX);
            writekeycode(KEYOUTINDEX, keysdown[x], 0);
            WHITEOUT(KEYOUTINDEX, " ", -1)
            cursorswap(KEYOUTINDEX, KEYBOXINDEX);
#endif
          }
#ifdef KEYBOXENABLED
          WHITEOUT(KEYBOXINDEX, " ", -1)
#endif
          if (PAIRNEQUALS(keysdown[x], '-','-')) {
            keysdownlen++;
          }
        }
#ifdef KEYBOXENABLED
        WHITEOUT(KEYBOXINDEX, "(", -1)
        GREENOUT1(KEYBOXINDEX, "%d", keysdownlen, -((keysdownlen >= 10) ? 2 : 1))
        WHITEOUT(KEYBOXINDEX, ")", -1)
        if (outputstreamlen >= OUTPUTSTREAMSIZE) {
          REDOUT(KEYBOXINDEX, " FULL", -5)
        }
        cursorlinebreak(KEYBOXINDEX);
#endif
#ifdef KEYOUTENABLED
        cursorswap(KEYBOXINDEX, KEYOUTINDEX);
        REDOUT(KEYOUTINDEX, "       ", -7)
//        cursorlinebreak(KEYOUTINDEX);
        cursorswap(KEYOUTINDEX, KEYBOXINDEX);
#endif
// ---------------------------------------------------------------------
// KEYSUP
// ---------------------------------------------------------------------
        keysuplen = 0;
        for (x = 0; x < MAXKEYS; x++) {
#ifdef KEYBOXENABLED
          writekeycode(KEYBOXINDEX, keysup[x], 0);
#endif
          if (x < MAXVALIDKEYS) {
#ifdef KEYOUTENABLED
            cursorswap(KEYBOXINDEX, KEYOUTINDEX);
            writekeycode(KEYOUTINDEX, keysup[x], 0);
            WHITEOUT(KEYOUTINDEX, " ", -1)
            cursorswap(KEYOUTINDEX, KEYBOXINDEX);
#endif
          }
#ifdef KEYBOXENABLED
          WHITEOUT(KEYBOXINDEX, " ", -1)
#endif
          if (PAIRNEQUALS(keysup[x], '-','-')) {
            keysuplen++;
          }
        }
#ifdef KEYBOXENABLED
        WHITEOUT(KEYBOXINDEX, "(", -1)
#endif
// ---------------------------------------------------------------------
// KEYLOG
// ---------------------------------------------------------------------
#ifdef KEYLOGBUFFERLISTENABLED
#include "mainkeylogbufferlist.h"
#endif
  // ACTIONBUFFERLIST is handled in sync.h
// ---------------------------------------------------------------------
// CLEARKEYS
// ---------------------------------------------------------------------
        // clear list if no keys are down
        if (keysdownlen == 0 || keyserror || !keysingle) {
          for (x = 0; x < MAXKEYS; x++) {
            keysdown[x][0] = '-';
            keysdown[x][1] = '-';
            keysup[x][0] = '-';
            keysup[x][1] = '-';
          }
          keysuplen = 0;
          keyarrlen = 0;
#ifdef KEYBOXENABLED
          REDOUT1(KEYBOXINDEX, "%d", keysuplen, -((keysuplen >= 10) ? 2 : 1))
#endif
        } else {
#ifdef KEYBOXENABLED
          YELLOWOUT1(KEYBOXINDEX, "%d", keysuplen, -((keysuplen >= 10) ? 2 : 1))
#endif
        }
#ifdef KEYBOXENABLED
        WHITEOUT(KEYBOXINDEX, ")", -1)
        cursorlinebreak(KEYBOXINDEX);
#endif
#ifdef KEYOUTENABLED
        cursorswap(KEYBOXINDEX, KEYOUTINDEX);
        cursorlinebreak(KEYOUTINDEX);
        cursormoveup(KEYOUTINDEX, 3);
        cursorrelease(KEYOUTINDEX);
        cursorswap(KEYOUTINDEX, KEYBOXINDEX);
#endif
#ifdef KEYBOXENABLED
        cursormoveup(KEYBOXINDEX, 8);
        cursorrelease(KEYBOXINDEX);
#endif
      }
      PLAINTEXT
      FLUSHBUFFER
// KEYBOX ENDS HERE
// ----------------------------------------------------------------------
// (1111)
//-------------------------------------------------------------
/*
  if (firstwrite == 1) {
#ifdef OUTBOXENABLED
    drawoutbox(outputstream, outputstreamlen);
    cursorrelease(OUTBOXINDEX);
#endif
    cdatscreen.redrawdue = 0;
  }
*/
// OUTBOX ENDS HERE
// ----------------------------------------------------------------------
      firstwrite = 0;
      syncsleepcount = 0;
// ----------------------------------------------------------------------
// PENDING TASKS
      if (pendingclearscreen == 'y') {
        FLUSHBUFFER
        PLAINTEXT
        RESETBG
        CLEARSCREEN
//        system("clear");
        FLUSHBUFFER
        pendingclearscreen = 'n';
      } else if (pendingpopejectstack == 'y') {
        popejectstack();
        pendingpopejectstack = 'n';
      } else if (pendingquitdrplinux == 'y') {
        FLUSHBUFFER
//        cursorrelease(KEYOUTINDEX);
        PLAINTEXT RESETBG
#ifdef SYNCBOXENABLED
        synckill = 1;
        drawsyncbox();
#endif
        FLUSHBUFFER
        syncshutdown();
        usleep(LOOPTIME);
        _exit(prevlastshiftstate);
      }
// ----------------------------------------------------------------------
    } else if (++syncsleepcount >= SYNCSLEEPCOUNT) {
      //drawvarbox();
      //drawkeepalivebox();

      if (cdatscreen.redrawdue == 1) {
        CLEARSCREEN
        initboxes(cdatscreen.cols, cdatscreen.rows);
#ifdef KEYOUTENABLED
// SWAPS CURSOR
        drawkeyoutbox();
#endif
#ifdef OUTBOXENABLED
// DOES NOT SWAP CURSOR
        drawoutbox(outputstream, outputstreamlen);
        cursorrelease(OUTBOXINDEX);
#endif
        cdatscreen.redrawdue = 0;
      }
#ifdef SYNCBOXENABLED
      drawsyncbox();
#endif
      syncsleepcount = 0;
    }

    dswap = dfront;
    dfront = dback;
    dback = dswap;

    fflush(stdout);
    usleep(LOOPTIME);
  }
  close(fd);

//  fprintf(stdout, "\033[2J\033[1;1H");

  return 0;
}
