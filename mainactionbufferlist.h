  char actionmessage[BUFFERLISTPACKETSIZE];
  if (tabmodevalue == 1) {
    if (synccount >= 0) {
      if (prevsynccount < synccount) {
        while (prevsynccount < synccount) {
          // get line
          prevsynccount++;
          syncgetline(syncactionfile, prevsynccount + 1, actionmessage);
          addtoindexedbufferlist(BUFFERLISTACTIONCONTENTINDEX, actionmessage);
          // process line
          int actionhasheader = (actionmessage[0] == '[') ? 1 : 0;
          int actionheader = 0;
          int ali = actionhasheader;
          while (actionhasheader && actionmessage[ali] != ']') {
            if (actionmessage[ali] < '0' || actionmessage[ali] > '9') {
              actionhasheader = 0;
              break;
            } else {
              actionheader = (actionheader * 10) + (int)(actionmessage[ali] - '0');
            }
            ali++;
          }
          if (actionhasheader && actionmessage[ali] == ']') {
            actionmessage[ali] = '=';
            ali++;
            if (actionhasheader && actionmessage[ali] == ' ') {
              if (strcmp(&actionmessage[ali + 1], "load") == 0) {
// ^^ what is actionmessage?
// >> load all 
              //  drawinbox(1, INBOXTYPEBACKGROUND, "== =");
                char cmdbuf[2000];
                readfirstline(FILEACTIONCOMMANDFILEARGS, cmdbuf);
                drawinbox(1, INBOXTYPEBACKGROUND, cmdbuf, DOUBLEBUFFERVDATSIGNALINDEX);
              } else if (strcmp(&actionmessage[ali + 1], "unload") == 0) {
                if (INBOX1.backgroundpid > 0) {
                  if (kill(INBOX1.backgroundpid, SIGINT) == 0) {
                    INBOX1.backgroundpid = 0;
                  }
                }
                drawinbox(1, INBOXTYPENEAT, "echo", -1);
              } else if (strcmp(&actionmessage[ali + 1], "run") == 0) {
//                drawinbox(2, INBOXTYPEBACKGROUND, "/v/vdat ^", -1);
                int ejectpid = drawinbox(2, INBOXTYPELAUNCH, "/v/vdat ^", -1);
                if (ejectpid > 0)
                  { pushejectstack(EJECTSTACKTYPEKILL, ejectpid); }
              } else if (strcmp(&actionmessage[ali + 1], "stop") == 0) {
//                if (INBOX2.backgroundpid > 0) {
//                  if (kill(INBOX2.backgroundpid, SIGINT) == 0) {
//                    INBOX2.backgroundpid = 0;
//                  }
//                }
              } else if (strncmp(&actionmessage[ali + 1], "append ", 7) == 0) {
                actionmessage[ali + 4] = actionmessage[ali + 7] = '/';
                actionmessage[ali + 5] = 's'; actionmessage[ali + 6] = '0';
                writefirstline(FILEACTIONCOMMANDFILELIST, READFILEWRITETYPELISTAPPEND, &actionmessage[ali + 4]);
                drawinbox(3, INBOXTYPELIST, FILEACTIONCOMMANDLISTCAT, -1);
              } else if (strncmp(&actionmessage[ali + 1], "remove ", 7) == 0) {
                actionmessage[ali + 4] = actionmessage[ali + 7] = '/';
                actionmessage[ali + 5] = 's'; actionmessage[ali + 6] = '0';
                writefirstline(FILEACTIONCOMMANDFILELIST, READFILEWRITETYPELISTREMOVE, &actionmessage[ali + 4]);
                drawinbox(3, INBOXTYPELIST, FILEACTIONCOMMANDLISTCAT, -1);
              } else if (strcmp(&actionmessage[ali + 1], "clear") == 0) {
                writefirstline(FILEACTIONCOMMANDFILELIST, READFILEWRITETYPELISTCLEAR, "");
                drawinbox(3, INBOXTYPELIST, FILEACTIONCOMMANDLISTCAT, -1);
              } else {
                // pass action through to the shell (handled by the = command)
                system(&actionmessage[ali - 1]);
              }
            }
          }
//          fprintf(stderr, "%s <<\n", &actionmessage[ali - 1]);
        }
        drawindexedbufferlist(ACTIONBUFFERLISTINDEX, BUFFERLISTACTIONCONTENTINDEX);
      }
    }
  }
