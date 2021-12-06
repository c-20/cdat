  char keylogmessage[BUFFERLISTPACKETSIZE];
  int kli = 0;
  if (keydown[0] == '2' && keydown[1] == 'b') {
    tabmodevalue = (tabmodevalue == 0) ? 1 : 0;
    if (tabmodevalue == 0) {
      // send XX to keystream, disconnect and disable
      for (kli = ((BUFFERLISTNUMPACKETS - 1) << 1) - 1; kli >= 0; kli--) {
        tabmodekeystream[kli + 2] = tabmodekeystream[kli];
      }
      tabmodekeystream[0] = '#';
      tabmodekeystream[1] = '#';
      indexbufferwriteintstringpair(2, tabmodekeyindex, tabmodekeystream);
      indexbufferdisconnectgracefully(2);
      sprintf(keylogmessage, "{ \"key\": { \"ix\": %4d, \"reg\": \"%s\" } }",
											tabmodekeyindex, tabmodekeystream);
      addtoindexedbufferlist(BUFFERLISTKEYLOGCONTENTINDEX, keylogmessage);
      drawindexedbufferlist(KEYLOGBUFFERLISTINDEX, BUFFERLISTKEYLOGCONTENTINDEX);
      tabmodekeyindex = 0;
    }
  } else if (tabmodevalue == 1) {
    if (tabmodekeyindex == 0) {
      // connect and enable
      indexbufferconnectforwriting(2);
      for (kli = 0; kli < (BUFFERLISTNUMPACKETS << 1); kli++) {
        tabmodekeystream[kli] = '.';
      }
      tabmodekeystream[kli] = '\0';
      indexbufferwriteintstringpair(2, tabmodekeyindex, tabmodekeystream);
    } else {
      // shift keystream and add to buffer
      for (kli = ((BUFFERLISTNUMPACKETS - 1) << 1) - 1; kli >= 0; kli--) {
        tabmodekeystream[kli + 2] = tabmodekeystream[kli];
      }
//      char tabkeydown = keylookup(keysdown[0][0], keysdown[0][1], 0);
//      char tabkeyup = keylookup(keysup[0][0], keysup[0][1], 0);
      char tabkeydown = keylookup(keysdown[0][0], keysdown[0][1], 0);
      char tabkeyup = keylookup(keysup[0][0], keysup[0][1], 0);
      char tabaltkeydown = altkeylookup(keysdown[0][0], keysdown[0][1], 0);
      char tabaltkeyup = altkeylookup(keysup[0][0], keysup[0][1], 0);
      char filler = '.';
      if (tabaltkeydown != '\0') {
        filler = '!';
        tabkeydown = tabaltkeydown;
      } else if (tabaltkeyup != '\0') {
        filler = '!';
        tabkeyup = tabaltkeyup;
      }
      tabmodekeystream[0] = (tabkeydown == '\0') ? filler : tabkeydown;
      tabmodekeystream[1] = (tabkeyup == '\0') ? filler : tabkeyup;
      indexbufferwriteintstringpair(2, tabmodekeyindex, tabmodekeystream);
    }
    sprintf(keylogmessage, "{ \"key\": { \"ix\": %4d, \"reg\": \"%s\" } }",
										tabmodekeyindex, tabmodekeystream);
    addtoindexedbufferlist(BUFFERLISTKEYLOGCONTENTINDEX, keylogmessage);
    drawindexedbufferlist(KEYLOGBUFFERLISTINDEX, BUFFERLISTKEYLOGCONTENTINDEX);
    tabmodekeyindex++;
    syncsleepcount = 0;
  }
