#define ANYOFFOUR(k, a, b, c, d)	(k == a || k == b || k == c || k == d)

char cmdkeychar = keylookup(keydown[0], keydown[1], shiftstate);
if (cmdkeychar >= 'a' && cmdkeychar <= 'z') {
  inputbuffer[inputbufferlen] = cmdkeychar;
  inputbuffer[++inputbufferlen] = '\0';
} else if (cmdkeychar >= 'A' && cmdkeychar <= 'Z') {
  inputbuffer[inputbufferlen] = cmdkeychar;
  inputbuffer[++inputbufferlen] = '\0';
} else if (cmdkeychar >= '0' && cmdkeychar <= '9') {
  inputbuffer[inputbufferlen] = cmdkeychar;
  inputbuffer[++inputbufferlen] = '\0';
} else if (cmdkeychar == ' ') {
  inputbuffer[inputbufferlen] = cmdkeychar;
  inputbuffer[++inputbufferlen] = '\0';
} else if (ANYOFFOUR(cmdkeychar, '+', '-', '.', ',') ||
           ANYOFFOUR(cmdkeychar, '|', '/', '"', '\'') ||
           ANYOFFOUR(cmdkeychar, '^', '%', '[', ']') ||
           ANYOFFOUR(cmdkeychar, '!', '$', '(', ')')) {
  inputbuffer[inputbufferlen] = cmdkeychar;
  inputbuffer[++inputbufferlen] = '\0';
} else if (cmdkeychar == '\b') {
  if (shiftstate && ctrlstate) {
    inputbufferlen = 0;
  } else if (shiftstate) {
    inputbufferlen -= KEYDOWNBIGERASELENGTH;
  } else inputbufferlen--;
  if (inputbufferlen < 0) {
    inputbufferlen = 0;
  }
  inputbuffer[inputbufferlen] = '\0';
} else if (cmdkeychar == '\n') {
  if (inputbufferlen > 0) {
    int boxtype = INBOXTYPESTDIN;
    if (inputbufferlen > 2 &&
        inputbuffer[inputbufferlen - 1] == '^' &&
        inputbuffer[inputbufferlen - 2] == ' ') {
      boxtype = INBOXTYPELAUNCH;
    }
//    setneat(INBOXMAININBOXINDEX, 0, 0, INBOXSIZEIW, INBOXSIZEIH);
    int ejectpid = drawinbox(INBOXMAININBOXINDEX, boxtype, inputbuffer, -1);
    if (boxtype == INBOXTYPELAUNCH && ejectpid > 0) {
      pushejectstack(EJECTSTACKTYPEKILL, ejectpid);
    }
  }
  syncboxstate = 'S';
}
