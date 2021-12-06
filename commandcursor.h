int commandcursorshiftleft() {
  if (commandcursorposmin > 0) {
    commandcursorposmin--;
    while (inputbuffer[commandcursorposmin] == ' ' && commandcursorposmin > 0) {
      commandcursorposmin--;
    }
    commandcursorposmax = commandcursorposmin;
    if (inputbuffer[commandcursorposmin] >= '0' && inputbuffer[commandcursorposmin] <= '9') {
      while (commandcursorposmin > 0 &&
             (inputbuffer[commandcursorposmin - 1] >= '0' && inputbuffer[commandcursorposmin - 1] <= '9')) {
        commandcursorposmin--;
      }
    } else if (inputbuffer[commandcursorposmin] >= 'A' && inputbuffer[commandcursorposmin] <= 'Z' ||
               inputbuffer[commandcursorposmin] >= 'a' && inputbuffer[commandcursorposmin] <= 'z') {
      while (commandcursorposmin > 0 &&
             (inputbuffer[commandcursorposmin - 1] >= 'A' && inputbuffer[commandcursorposmin - 1] <= 'Z' ||
              inputbuffer[commandcursorposmin - 1] >= 'a' && inputbuffer[commandcursorposmin - 1] <= 'z')) {
        commandcursorposmin--;
      }
    }
  }
  return HOTKEYRESPONSENOACTION;
}

int commandcursorshiftright() {
  if (inputbuffer[commandcursorposmax] != '\0' && inputbuffer[commandcursorposmax + 1] != '\0') {
    commandcursorposmax++;
    while (inputbuffer[commandcursorposmax] == ' ' && inputbuffer[commandcursorposmax + 1] != '\0') {
      commandcursorposmax++;
    }
    commandcursorposmin = commandcursorposmax;
    if (inputbuffer[commandcursorposmax] >= '0' && inputbuffer[commandcursorposmax] <= '9') {
      while (inputbuffer[commandcursorposmax + 1] >= '0' && inputbuffer[commandcursorposmax + 1] <= '9') {
        commandcursorposmax++;
      }
    } else if (inputbuffer[commandcursorposmax] >= 'A' && inputbuffer[commandcursorposmax] <= 'Z' ||
               inputbuffer[commandcursorposmax] >= 'a' && inputbuffer[commandcursorposmax] <= 'z') {
      while ((inputbuffer[commandcursorposmax + 1] >= 'A' && inputbuffer[commandcursorposmax + 1] <= 'Z') ||
             (inputbuffer[commandcursorposmax + 1] >= 'a' && inputbuffer[commandcursorposmax + 1] <= 'z')) {
        commandcursorposmax++;
      }
    }
  }
  return HOTKEYRESPONSENOACTION;
}

int commandcursorshiftup() {
  int ccl = commandcursorposmax - commandcursorposmin + 1;
  char cch = inputbuffer[commandcursorposmin];
  if (cch >= '0' && cch <= '9') {
    // increment number
    int carry = 1;
    int cci = commandcursorposmax;
    while (cci >= commandcursorposmin) {
      if (inputbuffer[cci] >= '0' && inputbuffer[cci] <= '9') {
        inputbuffer[cci] = inputbuffer[cci] + carry;
        carry = 0;
        while (inputbuffer[cci] > '9') {
          inputbuffer[cci] -= 10;
          carry++;
        }
      }
      cci--;
    }
    // if carry - e.g. 99 -> 100 - add leading char
    cci++;
    if (carry > 0 && carry <= 9) {
      inputbufferlen++;
      int ccj = inputbufferlen;
      while (ccj > cci) {
        inputbuffer[ccj] = inputbuffer[ccj - 1];
        ccj--;
      }
      if (ccj == cci) {
        inputbuffer[ccj] = (char)(carry + '0');
        commandcursorposmax++;
      }
    }
    return HOTKEYRESPONSEREDRAWINBOX;
  } else if (ccl == 1 && ((cch > ' ' && cch < '0') || (cch > '9' && cch < 'A') ||
                          (cch > 'Z' && cch < 'a') || (cch > 'z' && cch <= '~'))) {
    // increment symbol
    cch++;
    if (cch == '0') cch = '9' + 1;
    else if (cch == 'A') cch = 'Z' + 1;
    else if (cch == 'a') cch = 'z' + 1;
    else if (cch - 1 == '~') cch = ' ' + 1;
    inputbuffer[commandcursorposmin] = cch;
    return HOTKEYRESPONSENOACTION;
  }
  return HOTKEYRESPONSENOACTION;
}

int commandcursorshiftdown() {
  int ccl = commandcursorposmax - commandcursorposmin + 1;
  char cch = inputbuffer[commandcursorposmin];
  if (cch >= '0' && cch <= '9') {
    // decrement number (if > 0)
    if (ccl == 1 && cch == '0') {
      return HOTKEYRESPONSENOACTION;
    }
    int carry = 1;
    int cci = commandcursorposmax;
    while (cci >= commandcursorposmin) {
      if (inputbuffer[cci] >= '0' && inputbuffer[cci] <= '9') {
        inputbuffer[cci] = inputbuffer[cci] - carry;
        carry = 0;
        while (inputbuffer[cci] < '0') {
          inputbuffer[cci] += 10;
          carry++;
        }
      }
      cci--;
    }
    // if carry - e.g. 100 -> 099 - remove leading char
    cci++;
    if (inputbuffer[cci] == '0' && commandcursorposmin < commandcursorposmax) {
      int ccj = cci;
      while (ccj < inputbufferlen) {
        inputbuffer[ccj] = inputbuffer[ccj + 1];
        ccj++;
      }
      inputbufferlen--;
      commandcursorposmax--;
    }
    return HOTKEYRESPONSEREDRAWINBOX;
  } else if (ccl == 1 && ((cch > ' ' && cch < '0') || (cch > '9' && cch < 'A') ||
                          (cch > 'Z' && cch < 'a') || (cch > 'z' && cch <= '~'))) {
    // decrement symbol
    cch--;
    if (cch == '9') cch = '0' - 1;
    else if (cch == 'Z') cch = 'A' - 1;
    else if (cch == 'z') cch = 'a' - 1;
    else if (cch == ' ') cch = '~';
    inputbuffer[commandcursorposmin] = cch;
    return HOTKEYRESPONSENOACTION;
  }
  return HOTKEYRESPONSENOACTION;
}
