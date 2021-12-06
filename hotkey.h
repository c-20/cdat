
#define PAIRSEQUAL(a, b)		(a[0] == b[0] && a[1] == b[1])
#define PAIRSNEQUAL(a, b)		(a[0] != b[0] || a[1] != b[1])
#define PAIREQUALS(a, b1, b2)	(  a[0] == b1 && a[1] == b2  )
#define PAIRNEQUALS(a, b1, b2)	(  a[0] != b1 || a[1] != b2  )

#define MODSTATE(shiftstate, ctrlstate, altstate, cmdstate)	\
		  (shiftstate == shift && ctrlstate == ctrl			\
		  && altstate == alt && cmdstate == cmd)

int hotkeylookup(char *key, int shift, int ctrl, int alt, int cmd) {
  // non-standard keys
  if (PAIREQUALS(key, '5','2')) {
    // up
    if (syncboxstate == 'C') {
      if (MODSTATE(1, 0, 0, 0)) {
        return commandcursorshiftup();
      }
    } else if (syncboxstate == 'S') {
      if (MODSTATE(1, 0, 0, 0) && OUTBOX.y > 0) {
        // shift + up (move up)
        OUTBOX.y--;
        OUTBOX.fillbelow = 1;
        OUTBOX.hasmoved = 1;
      } else if (MODSTATE(0, 1, 0, 0) && OUTBOX.y > 0) {
        // ctrl + up (expand up)
        OUTBOX.y--;
        OUTBOX.height++;
        OUTBOX.hasmoved = 1;
      } else if (MODSTATE(1, 1, 0, 0) && OUTBOX.height > 3) {
        // ctrl + shift up (contract down)
        OUTBOX.height--;
        OUTBOX.fillbelow = 1;
        OUTBOX.hasmoved = 1;
//      } else if (MODSTATE(1, 1, 1, 0)) {
//        // ctrl + alt + shift up (expand up+down)
//        if (outboxy > 0) outboxy--;
//        if (outboxy + outboxheight < SCREENHEIGHT) outboxheight++;
//        if (outboxy + outboxheight < SCREENHEIGHT) outboxheight++;
//        outboxhasmoved = 1;
      }
    }
  } else if (PAIREQUALS(key, '5','1')) {
    // down
    if (syncboxstate == 'C') {
      if (MODSTATE(1, 0, 0, 0)) {
        return commandcursorshiftdown();
      }
    } else if (syncboxstate == 'S') {
      if (MODSTATE(1, 0, 0, 0) && OUTBOX.y + OUTBOX.height < SCREENHEIGHT) {
        // shift + down (move down)
        OUTBOX.y++;
        OUTBOX.fillabove = 1;
        OUTBOX.hasmoved = 1;
      } else if (MODSTATE(0, 1, 0, 0) && OUTBOX.y + OUTBOX.height < SCREENHEIGHT) {
        // ctrl + down (expand down)
        OUTBOX.height++;
        OUTBOX.hasmoved = 1;
      } else if (MODSTATE(1, 1, 0, 0) && OUTBOX.height > 3) {
        // ctrl + shift down (contract up)
        OUTBOX.y++;
        OUTBOX.height--;
        OUTBOX.fillabove = 1;
        OUTBOX.hasmoved = 1;
//      } else if (MODSTATE(1, 1, 1, 0) && outboxheight > 3) {
//        // ctrl + alt + shift down (contract up+down)
//        outboxy++;
//        outboxheight--;
//        outboxfillabove = 1;
//        if (outboxheight > 3) {
//          outboxheight--;
//          outboxfillbelow = 1;
//        }
//        outboxhasmoved = 1;
      }
    }
  } else if (PAIREQUALS(key, '5','0')) {
    // left
    if (syncboxstate == 'C') {
      if (MODSTATE(1, 0, 0, 0)) {
        return commandcursorshiftleft();
      }
    } else if (syncboxstate == 'S') {
      if (MODSTATE(1, 0, 0, 0) && OUTBOX.x > 0) {
        // shift + left (move left)
        OUTBOX.x--;
        OUTBOX.fillright = 1;
        OUTBOX.hasmoved = 1;
      } else if (MODSTATE(0, 1, 0, 0) && OUTBOX.x > 0) {
        // ctrl + left (expand left)
        OUTBOX.x--;
        OUTBOX.width++;
        OUTBOX.hasmoved = 1;
      } else if (MODSTATE(1, 1, 0, 0) && OUTBOX.width > 3) {
        // ctrl + shift left (contract right)
        OUTBOX.width--;
        OUTBOX.fillright = 1;
        OUTBOX.hasmoved = 1;
//      } else if (MODSTATE(1, 1, 1, 0)) {
//        // ctrl + alt + shift up (expand left+right)
//        outboxx--;
//        outboxwidth += 2;
//        outboxhasmoved = 1;
      }
    }
  } else if (PAIREQUALS(key, '4','f')) {
    // right
    if (syncboxstate == 'C') {
      if (MODSTATE(1, 0, 0, 0)) {
        return commandcursorshiftright();
      }
    } else if (syncboxstate == 'S') {
      if (MODSTATE(1, 0, 0, 0) && OUTBOX.x + OUTBOX.width < SCREENWIDTH) {
        // shift + right (move right)
        OUTBOX.x++;
        OUTBOX.hasmoved = 1;
        OUTBOX.fillleft = 1;
      } else if (MODSTATE(0, 1, 0, 0) && OUTBOX.x + OUTBOX.width < SCREENWIDTH) {
        // ctrl + right (expand right)
        OUTBOX.width++;
        OUTBOX.hasmoved = 1;
      } else if (MODSTATE(1, 1, 0, 0) && OUTBOX.width > 3) {
        // ctrl + shift right (contract left)
        OUTBOX.x++;
        OUTBOX.width--;
        OUTBOX.hasmoved = 1;
        OUTBOX.fillleft = 1;
//      } else if (MODSTATE(1, 1, 1, 0)) {
//        // ctrl + alt + shift right (contract left+right)
//        outboxx++;
//        outboxwidth -= 2;
//        outboxhasmoved = 1;
//        outboxfillleft = 1;
//        outboxfillright = 1;
      }
    }
  } else if (PAIREQUALS(key, '2','a')) {
    // backspace
    if (MODSTATE(1, 0, 0, 0)) {
      // shift + backspace (erase an extra 4 characters)
      return HOTKEYRESPONSEDOBIGERASE;
    } else if (MODSTATE(1, 1, 0, 0)) {
      // ctrl + shift backspace (erase entire line)
      return HOTKEYRESPONSEDOLINEERASE;
    }
  } else if (PAIREQUALS(key, '2', '9')) {
    // escape
    if (MODSTATE(0, 0, 0, 0) || MODSTATE(1, 0, 0, 0) || MODSTATE(1, 1, 0, 0)) {
      // sync -> command -> run -> sync
      if (syncboxstate != 'S') {
        syncboxstate = 'S';
      } else if (syncboxstate == 'S') {
        if (MODSTATE(1, 0, 0, 0)) {
          // shift + escape - clear
          inputbufferlen = 0;
          inputbuffer[inputbufferlen] = '\0';
        } else if (MODSTATE(1, 1, 0, 0)) {
          // ctrl + shift + escape - do not clear
        } else {
          // normal escape - crop at redirect (but keep suffix if available)
          // and stop left of (last) neat if available
          int ibc = 0;
          commandcursorposmin = 0;
          commandcursorposmax = 0;
          while (ibc < inputbufferlen - 2) {
            char ic1 = inputbuffer[ibc];
            char ic2 = inputbuffer[ibc + 1];
            char ic3 = inputbuffer[ibc + 2];
            if (ic1 == ' ' && ic2 == '|' && ic3 == ' ') {
              if (strncmp(&inputbuffer[ibc + 3], "neat ", 5) == 0) {
                commandcursorposmin = ibc + 1;
                commandcursorposmax = ibc + 1;
              }
            }
            if (ic1 == ' ' && ic2 == '>' && ic3 == ' ') {
              if (commandcursorposmax == 0) {
                commandcursorposmin = ibc;
                commandcursorposmax = ibc;
              }
              int suffixpos = ibc + 3;
              while (inputbuffer[suffixpos] != '\0' && inputbuffer[suffixpos] != '-' &&
                     (inputbuffer[suffixpos] < '1' || inputbuffer[suffixpos] > '9')) {
                suffixpos++;
              }
              if (inputbuffer[suffixpos] != '\0') {
                inputbufferlen = ibc + 3;
                while (inputbuffer[suffixpos] != '\0') {
                  inputbuffer[inputbufferlen] = inputbuffer[suffixpos];
                  inputbufferlen++;
                  suffixpos++;
                }
              } else inputbufferlen = ibc;
              inputbuffer[inputbufferlen] = '\0';
              break;
            }
            ibc++;
          }
        }
        syncboxstate = 'W';
      }
      return HOTKEYRESPONSENOACTION;
    }
  }
  if (OUTBOX.hasmoved) {
    return HOTKEYRESPONSEREDRAWOUTBOX;
  } else {
    return HOTKEYRESPONSENOACTION;
  }
}
