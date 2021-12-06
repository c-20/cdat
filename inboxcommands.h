// --------------------------------------------------------------
// + handle special commands (\r{Y} == yellow, \r{} == default, \r! == refresh)
                stdoutfilei++;
//RLOG("<<");
                inch = fgetc(stdoutfile);
                IF (inch == EOF) {
MLOG("EOF");
                } EF (inch != EOF) {
                  ch = (char)inch;
                  stdoutfilei++;
                  if (ch == '{') {
                    if ((inch = fgetc(stdoutfile)) != EOF) {
                      ch = (char)inch;
                      stdoutfilei++;
                      if (ch == '}') {
                        PLAINTEXT
                      } else {
                        CHARCOLOUR(ch)
                        if ((inch = fgetc(stdoutfile)) != EOF) {
                          ch = (char)inch;
                          stdoutfilei++;
                          if (ch != '}') {
                            PLAINTEXT
                          }
                        }
                      }
                    }
                  } else if (ch == '!' || ch == '.') {
//CLOG("!");
                    int footerwidth = 8;
                    int footerxoffset = inboxnsizeiw[boxindex] - cursors[inboxindex].dx - footerwidth;
                    int footeryoffset = inboxnsizeih[boxindex] - cursors[inboxindex].dy;
                    if (boxtype == INBOXTYPEBACKGROUND && superquietmode != 1) {
                      cursortour(inboxindex, footerxoffset, (quietdeath == 1) ? 0 : footeryoffset);
//                      cursormovedown(inboxindex, 10);
                      MAGENTAOUT(inboxindex, "[", -1)
                      // export - apply colour based on state returned
                      REDOUT1(inboxindex, "%6d", (int)buffervalues[0], -6)
                      cursormoveleft(inboxindex, 6);
                    }
                    char writestate = '?';
                    if (boxtype == INBOXTYPEBACKGROUND && bufferindex != -1) {
//WLOG1("[[[writefloatarray%d", buffervaluei);
                      writestate = doublebufferwritefloatarray(bufferindex, buffervalues, buffervaluei);
//YLOG1("-%d]]]", writestate);
                    }
                    if (boxtype == INBOXTYPEBACKGROUND && superquietmode != 1) {
                      if (writestate == 'A') {
                        WHITEOUT(inboxindex, "BUFFER", -6)
                      } else if (writestate == 'B') {
                        YELLOWOUT1(inboxindex, "%6d", buffervaluei, -6)
                      } else if (writestate == 'C') {
                        GREENOUT1(inboxindex, "%6d", buffervaluei, -6)
                      } else if (writestate == 'D') {
                        BLUEOUT1(inboxindex, "%6d", buffervaluei, -6)
                      } else if (writestate == 'X') {
                        WHITEOUT(inboxindex, "------", -6)
                      } else {
                        REDOUT(inboxindex, "??????", -6)
                      }
                      MAGENTAOUT(inboxindex, "]", -1)
                      cursormoveleft(inboxindex, footerwidth);
//                        cursormoveup(inboxindex, 10);
                      cursordetour(inboxindex, footerxoffset, (quietdeath == 1) ? 0 : footeryoffset);
                      PLAINTEXT
                      FLUSHBUFFER
                    }
                    if (boxtype == INBOXTYPEBACKGROUND && writestate == 'A') {
                      MAGENTAOUT(inboxindex, "> ", -2)
                      GREENOUT(inboxindex, "BUF0", -4)
                      PLAINTEXT
                      cursormoveleft(inboxindex, 6);
                      FLUSHBUFFER
                      bufferinactivestate = 1;
                      if (bufferinactivestate == 1 && returnifbufferisactive == 1) {
                        if ((backgroundchildpid = fork()) == 0) {
                          // this is the child ... continue, but with no output
                          quietdeath = 1;
                          // suppress titlebar/footer output
                          superquietmode = 1;
                        } else {
                          // this is the main thread.... quietly return to the main loop
                          // assuming current position is top left of running inbox
                          // assuming user wants to know child pid
                          cursormovedown(inboxindex, 1);
                          REDOUT(inboxindex, "^ ", -2)
                          MAGENTAOUT1(inboxindex, "%-5d", backgroundchildpid, -5)
                          INBOX1.backgroundpid = backgroundchildpid;
// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ should be removed once all inboxes can be background threads
                          instancessetbackgroundpidforinbox(inboxindex, backgroundchildpid);
                          cursormoveleft(inboxindex, 7);
                          cursormoveup(inboxindex, 1);
                          cursorrelease(inboxindex);
                          FLUSHBUFFER
                          return ejectpid;
                        }
                        returnifbufferisactive = 0;
                      }
                    }
                    buffervaluei = 0;
//                                                                                    
                    if (ch == '!') {
                      if (quietmode != 1 && quietdeath != 1) {
                        cursorlinebreak(inboxindex);
                        numrows++;
                        numcols = 0;
                      }
                      if (quietmode != 1 && quietdeath != 1) {
                        cursormoveup(inboxindex, numrows);
                        numrows = 0;
                      }
  // ------------------------------------------------------------
  // + clear, sleep and flush on refresh 
                      if (quietmode == 1 && superquietmode != 1 && quietdeath == 0) {
                        quietkeepalive = spinkeepalive(quietkeepalive);
                        YELLOWOUT1(inboxindex, "%c", quietkeepalive, -1)
                        PLAINTEXT
                        cursormoveleft(inboxindex, 1);
                        FLUSHBUFFER
                        sched_yield();
                        //usleep(INBOXHOLDTIME);
                      }
                      //clearbox(inboxsizeiw, inboxsizeih);
  // - clear, sleep and flush on refresh
  // --------------------------------------------------------------
                    }
                  }
                }
// - handle special commands
// --------------------------------------------------------------
