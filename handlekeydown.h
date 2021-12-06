#define KEYDOWNBIGERASELENGTH	5
#define OUTBOXFOOTERWIDTH		22

char sendoutboxkeyaction(char key, int hotkeyresponse, int recursionindex) {
  char needsredraw = 'n';
  // only send to living entities
  if (OUTBOX.keepalive) {
    // determine current location
    int kax = keepalives[OUTBOX.keepalive].x;// - OUTCURSOR.x0 - OUTCURSOR.offx;
    int kay = keepalives[OUTBOX.keepalive].y;// - OUTCURSOR.y0 - OUTCURSOR.offy;
    int innerwidth = OUTBOX.width - OUTCURSOR.offx - OUTCURSOR.offx;
    int innerheight = OUTBOX.height - OUTCURSOR.offy - OUTCURSOR.offy;
    // always redraw if overflowing or erasing a line
    if (kay >= innerheight ||
        hotkeyresponse == HOTKEYRESPONSEDOLINEERASE) {
      return 'y';
    }
    // go there
//    cursormoveright(OUTBOXINDEX, -kax);
//    cursormovedown(OUTBOXINDEX, -kay);
    // determine current line length
    int linelen = 0;
    while (outputstreamlen - linelen - recursionindex >= 0 &&
           outputstream[outputstreamlen - linelen - recursionindex] != '\n') {
      linelen++;
    }
    // determine previous line length
    int prevlinelen = 0;
    while (outputstreamlen - linelen - 1 - prevlinelen - recursionindex >= 0 &&
           outputstream[outputstreamlen - linelen - 1 - prevlinelen - recursionindex] != '\n') {
      prevlinelen++;
    }
    // fix backspace alignment (confusing)
    if (key == '\b') {
      linelen -= 2;
    } else if (hotkeyresponse == HOTKEYRESPONSEWRITEFOOTER) {
      linelen -= 1;
    }
    // handle key
    if (key == '\b') {
      if (outputstreamlen - recursionindex <= 0) {
        // backspacing an empty box - redraw
        needsredraw = 'y';
//        return 'y';
//        return 'y';
      } else if (outputstream[outputstreamlen - recursionindex - 1] == '\n') {
        // backspacing to a previous line - move up and right
        cursormoveup(OUTBOXINDEX, 1);
        kay -= 1;
        cursormoveright(OUTBOXINDEX, prevlinelen % innerwidth);
        kax += prevlinelen % innerwidth;

//        // backspacing to a previous line - redraw
        needsredraw = 'y';

      } else {
        // normal backspace
//        if (outputstream[outputstreamlen - recursionindex - 1] == '\n')
        if (kax > 0) {// && kax - recursionindex != -1) {
          // moving left
          cursormoveleft(OUTBOXINDEX, 1);
          REDOUT(OUTBOXINDEX, "X", -1)
          cursormoveleft(OUTBOXINDEX, 1);
          kax -= 1;
        } else {
          // wrapping to a previous line
          cursormoveup(OUTBOXINDEX, 1);
          kay -= 1;
          cursormoveright(OUTBOXINDEX, innerwidth - 1);
          kax += innerwidth - 1;
          REDOUT(OUTBOXINDEX, "X", -1)
          cursormoveleft(OUTBOXINDEX, 1);
        }
      }
      // set new location
      setkeepalivepos(OUTBOX.keepalive, kax, kay);
      // recurse 1 2 3 4 - this is index 0
      if (hotkeyresponse == HOTKEYRESPONSEDOBIGERASE &&
          recursionindex < KEYDOWNBIGERASELENGTH - 1 &&
//          (linelen % innerwidth) - recursionindex >= 0) {// &&
          needsredraw == 'n') {
        needsredraw =
          (sendoutboxkeyaction(key, hotkeyresponse, recursionindex + 1)
            == 'y') ? 'y' : needsredraw;
      }
    } else if (key == '\n') {
      // line break - redraw
      cursorlinebreak(OUTBOXINDEX);
      prevlinelen = linelen - 1;
      linelen = 0;
//      cursormovedown(OUTBOXINDEX, 1);
      kay += 1;
//      cursormoveleft(OUTBOXINDEX, prevlinelen % innerwidth);
      kax -= prevlinelen % innerwidth;
//      kax -= prevlinelen % innerwidth;
      // set new location
      setkeepalivepos(OUTBOX.keepalive, kax, kay);
      needsredraw = 'y';
/*
      // line break - move down and left
      if (outputstreamlen < OUTPUTSTREAMSIZE - 1) {
        cursormovedown(OUTBOXINDEX, 1);
        kay += 1;
        cursormoveleft(OUTBOXINDEX, linelen - 1);
        kax -= linelen - 1;
      }
*/
    } else if (key != '\0') {
      // normal key - set char + (move right | wrap if necessary)
      if (outputstreamlen < OUTPUTSTREAMSIZE - 1) {
        if (kay < innerheight) {
          CYANOUT1(OUTBOXINDEX, "%c", key, -1)
          kax += 1;
          if ((linelen % innerwidth) == 0) {
            cursorlinebreak(OUTBOXINDEX);
            kay += 1;
            kax -= innerwidth;
          }
        }
      }
      // set new location
      setkeepalivepos(OUTBOX.keepalive, kax, kay);
    }
    // write footer
    if (hotkeyresponse != HOTKEYRESPONSEDOBIGERASE ||
        recursionindex == KEYDOWNBIGERASELENGTH - 1) {
      int footerxoffset = 0;
      int footeryoffset = innerheight - kay;
//      if (key == '\n') {
      if (linelen == -1) {
//        if (key == '\b') {
//          footerxoffset = innerwidth + 2;
//        } else if (key == '\n') {
        footerxoffset = innerwidth - (prevlinelen % innerwidth) - OUTBOXFOOTERWIDTH;
//        }
      } else {
        footerxoffset = innerwidth - (linelen % innerwidth) - OUTBOXFOOTERWIDTH;
      }
      if (hotkeyresponse != HOTKEYRESPONSEWRITEFOOTER) {
        cursormovedown(OUTBOXINDEX, footeryoffset);
        cursormoveleftright(OUTBOXINDEX, footerxoffset);
      }
      CYANOUT(OUTBOXINDEX, "[", -1);
      YELLOWOUT1(OUTBOXINDEX, "%3d", kax, -3)
      CYANOUT(OUTBOXINDEX, "|", -1);
      YELLOWOUT1(OUTBOXINDEX, "%3d", linelen, -3)
      CYANOUT(OUTBOXINDEX, "|", -1);
      YELLOWOUT1(OUTBOXINDEX, "%3d", prevlinelen, -3)
      CYANOUT(OUTBOXINDEX, "|", -1);
      YELLOWOUT1(OUTBOXINDEX, "%3d", kay, -3)
      CYANOUT(OUTBOXINDEX, "|", -1);
      if (hotkeyresponse == HOTKEYRESPONSEWRITEFOOTER) {
        GREENOUT1(OUTBOXINDEX, "%4d", outputstreamlen, -4)
      } else {
        REDOUT1(OUTBOXINDEX, "%4d", outputstreamlen, -4)
      }
      CYANOUT(OUTBOXINDEX, "]", -1);
      cursormoveleft(OUTBOXINDEX, OUTBOXFOOTERWIDTH);
      if (hotkeyresponse != HOTKEYRESPONSEWRITEFOOTER) {
        cursormoveup(OUTBOXINDEX, footeryoffset);
        cursormoverightleft(OUTBOXINDEX, footerxoffset);
      }
    }
    // indicate that work was performed
    char kac = keepalives[OUTBOX.keepalive].c;
    keepalives[OUTBOX.keepalive].c = spinkeepalive(kac);
    // go back
//    cursormoveleft(OUTBOXINDEX, -kax);
//    cursormoveup(OUTBOXINDEX, -kay);
  }
  return needsredraw;
}


void handlekeydown(char *key, int shiftstate, int hotkeyresponse) {
  char keychar = keylookup(key[0], key[1], shiftstate);
  char needsredraw = 'n';
  if (keychar != '\0') {
    // tell outbox first
#ifdef OUTBOXENABLED
    needsredraw = sendoutboxkeyaction(keychar, hotkeyresponse, 0);
#endif
    // then modify the stream
    if (keychar == '\b') {
      if (hotkeyresponse == HOTKEYRESPONSEDOBIGERASE) {
        int n = KEYDOWNBIGERASELENGTH;
        char k = '\0';
        while (n > 0 && outputstreamlen > 0) {
          k = outputstream[outputstreamlen - 1];
          outputstream[--outputstreamlen] = '\0';
          n--;
          if (k == '\n') {
            break;
          }
        }
      } else if (hotkeyresponse == HOTKEYRESPONSEDOLINEERASE) {
        // if at beginning of next line, move to end of previous line
        if (outputstreamlen > 0 &&
            outputstream[outputstreamlen - 1] == '\n') {
          outputstream[--outputstreamlen] = '\0';
        }
        // erase until start of file or end of previous line
//        int n = 0;
        while (outputstreamlen > 0 &&
               outputstream[outputstreamlen - 1] != '\n') {
          outputstream[--outputstreamlen] = '\0';
//          n++;
        }
      } else {
        if (outputstreamlen > 0) {
          outputstream[--outputstreamlen] = '\0';
        }
      }
    } else if (outputstreamlen < OUTPUTSTREAMSIZE - 1) {
      outputstream[outputstreamlen++] = keychar;
      outputstream[outputstreamlen] = '\0';
      //if (keychar == '\n' || keychar == '`') {
      if (keychar == ' ') {
        needsredraw = 'y';
      }
    }

    // then perform any necessary post-operations
    if (needsredraw == 'y') {
#ifdef OUTBOXENABLED
      cursordetour(OUTBOXINDEX, keepalives[OUTBOX.keepalive].x, keepalives[OUTBOX.keepalive].y);
      drawoutbox(outputstream, outputstreamlen);

//      cursorrelease(OUTBOXINDEX);
      cursortour(OUTBOXINDEX, OUTCURSOR.x0, OUTCURSOR.y0);
      cursortour(OUTBOXINDEX, OUTBOX.width - OUTBOXFOOTERWIDTH - 1, OUTBOX.height - 1);
//      cursorgoto(BACKCURSORINDEX
      sendoutboxkeyaction('\0', HOTKEYRESPONSEWRITEFOOTER, 0);
      cursordetour(OUTBOXINDEX, OUTBOX.width - OUTBOXFOOTERWIDTH - 1, OUTBOX.height - 1);
      cursordetour(OUTBOXINDEX, OUTCURSOR.x0, OUTCURSOR.y0);
      cursortour(OUTBOXINDEX, keepalives[OUTBOX.keepalive].x, keepalives[OUTBOX.keepalive].y);
#endif
      needsredraw = 'n';
//      cursorrelease(OUTBOXINDEX);
//      cursorgoto(OUTBOXINDEX, OUTCURSOR.offx, OUTCURSOR.offy);
//      cursorgoto(OUTBOXINDEX, keepalives[OUTBOX.keepalive].x, keepalives[OUTBOX.keepalive].y);
    }
  }
}
