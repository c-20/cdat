  instancesinit();
  initboxes(cdatscreen.cols, cdatscreen.rows);

//  drawinbox(INBOXTYPESTRING, "Hello, World!", 'n');
//  drawinbox(INBOXTYPESOURCEFILE, "scanchar/main.c", 'n');
//  drawinbox(INBOXTYPECACHEFILE, "bla.txt", 'n');

//  drawinbox(INBOXTYPESTDIN, "res", 'n');
//  drawinbox(INBOXTYPESTDIN, "nanosnap scanchar/main.c 1", 'n');

//  setneat(INBOXMAININBOXINDEX, 0, 0, INBOXSIZEIW, INBOXSIZEIH);
  resetneats(INBOXMAININBOXINDEX, INBOXMAXINBOXINDEX);
#ifdef INBOX4AUTOEXEC
//  drawinbox(4, INBOXTYPESTDIN, "ample $(- ls sensortest PATH) 15 -", -1);
//  drawinbox(4, INBOXTYPESTDIN, "ample $(cat /tau/buf/save/dataloadercache/D9_S1_sacrum_26_Jul_2011/list.txt) 15 -");
#endif
//  drawinbox(3, INBOXTYPESTDIN, "matserv downlist | hold 12");
#ifdef INBOX3AUTOEXEC
  drawinbox(3, INBOXTYPENEAT, "res", -1);
//  drawinbox(4, INBOXTYPESTDIN, "~/@/sensor | hold 12", -1);
/////////////////////  

//drawinbox(3, INBOXTYPELIST, FILEACTIONCOMMANDLISTCAT, -1);


#endif
#ifdef INBOX2AUTOEXEC
//  drawinbox(2, INBOXTYPENEAT, "dhcp -l", -1);
/////////////////////  drawinbox(2, INBOXTYPENEAT, "echo", -1);
#endif
#ifdef INBOX1AUTOEXEC
//  drawinbox(1, INBOXTYPESTDIN, "matserv connect", -1);
  if (autoexecloadparam == 1) {
    char autoexecbuf[2000];
    readfirstline(FILEACTIONCOMMANDFILEARGS, autoexecbuf);
    drawinbox(1, INBOXTYPEBACKGROUND, autoexecbuf, DOUBLEBUFFERVDATSIGNALINDEX);
  } else {
    drawinbox(1, INBOXTYPENEAT, "echo", -1);
  }
#endif
#ifdef INBOXAUTOEXEC
//  drawinbox(INBOXMAININBOXINDEX, INBOXTYPEBACKGROUND, "ample $(- ls sensortest PATH) 15 - 0 3000 | hold 30");
//  drawinbox(0, INBOXTYPEBACKGROUND, "ample $(cat /tau/buf/save/dataloadercache/D9_S1_sacrum_26_Jul_2011/list.txt) 15 - 0 3000 | hold 30");
//  char cmdbuf[2000];
//  readfirstline("/tau/sys/commands/===", cmdbuf);
//  drawinbox(0, INBOXTYPEBACKGROUND, cmdbuf);
#endif

