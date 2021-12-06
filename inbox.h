#define INBOXDEFINITION
#include "hold.h"
#undef INBOXDEFINITION

#define INBOXBUFFERDIR			"/tau/buf/"
#define INBOXSOURCEDIR			"/tau/src/"
#define INBOXDIRLENGTH			9
#define INBOXBUFFERNAME			"page0"
#define INBOXBUFFERNAMELENGTH	5
#define INBOXSTDINPREFIX		"stdin"
#define INBOXSTDINNAME			""
#define INBOXSTDINSUFFIX		" 2> /tau/buf/page0/stderr"
#define INBOXSTDINNAMELENGTH	5
#define INBOXMAXFILENAMELENGTH	1024

#define INBOXWAITTIME			200
#define INBOXHOLDTIME			200

#define INBOXTYPESTRING			1
#define INBOXTYPESOURCE			2
#define INBOXTYPECACHE			3
#define INBOXTYPESTDIN			4
#define INBOXTYPELAUNCH			5
#define INBOXTYPENEAT			6
#define INBOXTYPEHOLD			7
#define INBOXTYPEBACKGROUND		8
#define INBOXTYPELIST			9

#define INBOXMAININBOXINDEX		0
#define INBOXMAXINBOXES			100 // (00 - 99)
#define INBOXMAXNEATLENGTH		100
#define INBOXMAXINBOXINDEX		4

char inboxneats[INBOXMAXINBOXES][INBOXMAXNEATLENGTH];

#include "inboxshell.h"
#include "inboxneat.h"

// in in box box ?
// in box in in box .

int drawinbox(int boxindex, int boxtype, char *value, int bufferindex) {
  // launch ( ^ suffix) == stdin + eject pid
  int ejectpid = 0;
  char overrideeject = 'n';
  if (boxtype == INBOXTYPELAUNCH)
    { overrideeject = 'y'; boxtype = INBOXTYPESTDIN; }
  // if a redirect was given, remember it but destroy it
  char *stdinredirectname = strsnip(value, " > ");
  int redirectedboxindex = 0;
  if (stdinredirectname != NULL)
    { redirectedboxindex = strtailint(stdinredirectname, 'U'); }
  if (redirectedboxindex != 0)
    { boxindex = redirectedboxindex; }
  // draw inbox
  int inboxindex = inboxnindex[boxindex];
  int inboxposx = inboxnposx[boxindex];
  int inboxposy = inboxnposy[boxindex];
  int inboxsizew = inboxnsizew[boxindex];
  int inboxsizeiw = inboxnsizeiw[boxindex];
  int inboxsizeh = inboxnsizeh[boxindex];
  int inboxsizeih = inboxnsizeih[boxindex];
  cursorsetpos(inboxindex, inboxposx, inboxposy);
  cursordrawbox(inboxindex, inboxsizew, inboxsizeh);
  int x = inboxposx + 1;
  int y = inboxposy + 1;
  // background processes (& _____) may die quietly
  int quietdeath = 0;
  char boxlistdelimeter = '\t';

//  char *shellcolour = inboxshellcolour(boxtype);
  // write header
  instance_s *ins = instancesaddcache(boxindex, "(string)");
  if (ins == NULL) { fprintf(stderr, "NULL\n"); return -1; }
  sprintf(ins->plainheader, "[$(%s)]", value);
  sprintf(ins->colourheader, "[%s$(%s%s%s)%s]", YELLOWCOLOUR,
          WHITECOLOUR, value, YELLOWCOLOUR, PLAINCOLOUR);
  cursormoveup(inboxindex, 1);
  int cml = lenofstr(ins->plainheader, '\0');
  OUT1(inboxindex, "%s", ins->colourheader, -cml)
  cursormoveleft(inboxindex, cml);
  cursormovedown(inboxindex, 1);
  PLAINTEXT

  if (boxtype == INBOXTYPESTRING || boxtype == INBOXTYPECACHE) {
    instancesfillcache(boxindex, CACHETYPESTRING, value);
// translate '\n' to cursorlinebreak(inboxindex); when redrawing
    int vi = 0;
    while (value[vi] != '\0') {
      if (value[vi] == '\n')
        { cursorlinebreak(inboxindex); }
      else
        { OUT1(inboxindex, "%c", value[vi], -1) }
      vi++;
    }
    cursorlinebreak(inboxindex);
//    instancesaddcache(inboxindex, "(string)");
//    instancesfillcache(inboxindex, CACHETYPESTRING, value);
  } else {
    // build filename string
    char filename[INBOXMAXFILENAMELENGTH + 1];
    filename[INBOXMAXFILENAMELENGTH] = '\0';
    int fi = 0;
    int prefixlen = 0;
    if (boxtype == INBOXTYPESTDIN || boxtype == INBOXTYPENEAT ||
        boxtype == INBOXTYPEHOLD || boxtype == INBOXTYPEBACKGROUND || boxtype == INBOXTYPELIST) {
      fi = prefixlen = strtostr(value, '\0', filename, '\0');
      // pipe into neat
      if (boxtype == INBOXTYPENEAT) {
        fi = prefixlen = strappendend(filename, '\0', " | ", inboxneats[boxindex], '\0');
        boxtype = INBOXTYPESTDIN;
      } else if (boxtype == INBOXTYPELIST) {
        boxlistdelimeter = ',';
        boxtype = INBOXTYPESTDIN;
      } // ELSE IF TYPE HOLD ..
      fi = prefixlen = strappend(filename, '\0', " > ", '\0');
    }
    // remember the target filename
    char *outfile = &filename[fi];
    if (boxtype == INBOXTYPESTDIN || boxtype == INBOXTYPEBACKGROUND) {
      fi = strappendendend(filename, '\0', INBOXBUFFERDIR, INBOXBUFFERNAME, "/", '\0');
      char *stdinname = (stdinredirectname = NULL) ? stdinredirectname : INBOXSTDINNAME;
      fi = strappendend(filename, '\0', INBOXSTDINPREFIX, INBOXSTDINNAME, '\0');
    } else {
      fi = strappend(filename, '\0', value, '\0');
    }
//    fprintf(stderr, "[[%s]]\n", filename);
//    fflush(stderr);
//    _exit(0);
//    filename[fi] = '\0';

    // open and write - only if background process
//                                                                                                 
    if (boxtype == INBOXTYPEBACKGROUND && bufferindex != -1) {
      if (doublebufferconnectforwriting(bufferindex) == 0) {
        REDOUT(inboxindex, "BUFFER ERROR", -12)
        cursorlinebreak(inboxindex);
        cursormoveup(inboxindex, 1);
        cursorrelease(inboxindex);
        FLUSHBUFFER
        return -1;
      }
    }
//                                                                                                 

// so, instance needs to remember inbox type (source file, cache file, stdin, background, list)
// or rather, cache types need to be consolidated to suit all possibilities
// strtostr(outputbuffer, '\0', stringcache, '\0');
// once cache is saved in instances, find drawinbox() loop and create appropriate redrawinbox() loop
//
// --------------------------

    if (boxtype == INBOXTYPESOURCE) {
      OUT1(inboxindex, "source: %s", filename, -(fi + 8))
      cursorlinebreak(inboxindex);
      cursormoveup(inboxindex, 1);
    } else if (boxtype == INBOXTYPECACHE) {
      OUT1(inboxindex, "cache: %s", filename, -(fi + 7))
      cursorlinebreak(inboxindex);
      cursormoveup(inboxindex, 1);
    } else if (boxtype == INBOXTYPESTDIN || boxtype == INBOXTYPEBACKGROUND || boxtype == INBOXTYPELIST) {
      inputbufferlen = 0;
      while (filename[inputbufferlen] != '\0') {
        inputbuffer[inputbufferlen] = filename[inputbufferlen];
        inputbufferlen++;
      }
      inputbuffer[inputbufferlen] = '\0';
      // record stderr, make it background
      char *filenamesuffix = INBOXSTDINSUFFIX;
      int fnsi = 0;
      while (filenamesuffix[fnsi] != '\0') {
        filename[fi + fnsi] = filenamesuffix[fnsi];
        fnsi++;
      }
      filename[fi + fnsi] = '\0';
      int shellpid = 0;
      // extract file buffers
      char *stdoutfilename = &filename[prefixlen]; // after " > "
      filename[fi] = '\0';
      char *stderrfilename = &filename[fi + 4];    // after " 2> "
      // remove old target file
      remove(stdoutfilename);
      // split process - child is shell
      if ((shellpid = fork()) == 0) {
        // restore command for execution
        filename[fi] = ' ';
        // make sure system stderr is turned off
        freopen("/dev/null", "w", stderr);
        // run command (ignore errors but show nonzero return code)
        int returncode = system(filename);
        _exit(returncode);
      } else {
// --------------------------------------------------------------
// write result to inbox
        int inch = EOF;
        char ch = '?';
        char prevch = '?';
        int numrows = 0;
        int numcols = 0;
        char overflowspin = '?';
        char filewaitspin = '?';
        FILE *stdoutfile;
        // wait for the output file to become available
        while ((stdoutfile = fopen(&filename[prefixlen], "r")) == NULL) {
          filewaitspin = spinkeepalive(filewaitspin);
          REDOUT1(inboxindex, "%c", filewaitspin, -1)
          cursormoveleft(inboxindex, 1);
          PLAINTEXT
          fflush(stdout);
          sched_yield();
          //usleep(INBOXWAITTIME);
        }
        int stdoutfilei = 0;
        int shellstatus = 'a';
        int shellreturncode = -1;
        int waitstatvar = 0;
        char inboxkeepalive = '?';
        int mode = 'w';
        float TEMPBUFFER[2000];
int tbbi = 0;
if (boxtype == INBOXTYPEBACKGROUND) {
  while (tbbi < 2000) {
    TEMPBUFFER[tbbi] = (float)tbbi;
    tbbi++;
  }
}
        float *buffervalues = TEMPBUFFER;
        int buffervaluei = 0;
        float buffernum = -1.0f;
        float buffernumbase = -1.0f;
        char bufferstate = '\n';
        int quietmode = (boxtype == INBOXTYPEBACKGROUND) ? 1 : 0;
        int superquietmode = 0;//(boxtype == INBOXTYPEBACKGROUND) ? 1 : 0;
        char quietkeepalive = '?';
        int bufferinactivestate = 0;
        int returnifbufferisactive = 1;//(boxtype == INBOXTYPEBACKGROUND) ? 1 : 0;
        int backgroundchildpid = 0;
//        instance_s *ins = instancesaddcache(inboxindex, filename);
//        instancesfillcache(inboxindex, CACHETYPESTRING, "");
        while (shellstatus != 'c') {
          fseek(stdoutfile, stdoutfilei, SEEK_SET);
          while ((inch = fgetc(stdoutfile)) != EOF) {
            ch = (char)inch;
            // save cache before evaluating
            instancesfillcacheaddchar(inboxindex, ch);
            if (mode == 'w') {
              // line break or list delimeter
              if (ch == '\n' || (boxlistdelimeter != '\t' && ch == boxlistdelimeter)) {
                if (quietmode != 1 && superquietmode != 1) {
                  cursorlinebreak(inboxindex);
                  numrows++;
                  numcols = 0;
                }
                stdoutfilei++;
// --------------------------------------------------------------
// + handle float linebreak (extra linebreaks skip samples)
                if (ch == '\n' && bufferstate != ' ') {
                  if (bufferstate == '\n') {
                    // skip to next value
                    buffervaluei++;
                  } else {
                    // apply polarity and save value
                    if (bufferstate == '-' || bufferstate - '.' == '-') {
                      buffernum *= -1.0f;
                      bufferstate = bufferstate - '-' + '+';
                    }
                    buffervalues[buffervaluei] = buffernum;
                    buffervaluei++;
                  }
                }
                bufferstate = '\n';
// - handle float linebreak
// --------------------------------------------------------------
              } else if (ch == '\r') {
// --------------------------------------------------------------
// + handle special commands (\r{Y} == yellow, \r{} == default, \r! == refresh)
#include "inboxcommands.h"
// - handle special commands
// --------------------------------------------------------------
              } else if (numcols < inboxsizeiw) {
                // valid character
                if (overrideeject == 'y') {
                  // input will be the pid of the process
                  if (ch >= '0' && ch <= '9') {
                    ejectpid = (ejectpid * 10) + (int)(ch - '0');
                    MAGENTAOUT1(inboxindex, "%c", ch, -1)
                    PLAINTEXT
                  }
                } else {
                  // input is text - send to stdin
                  if (quietmode == 0 && quietdeath != 1) {
                    OUT1(inboxindex, "%c", ch, -1)
// ^^^^^^^^^^^^^^^ write char to cache
                    numcols++;
                  }
// --------------------------------------------------------------
// + handle float export
                  if (ch == ' ' || ch == '\t') {
                    // save value to buffer
                    if (bufferstate != ' ' && bufferstate != '\n') {
                      // apply polarity and save value
                      if (bufferstate == '-' || bufferstate - '.' == '-') {
                        buffernum *= -1.0f;
                        bufferstate = bufferstate - '-' + '+';
                      }
                      buffervalues[buffervaluei] = buffernum;
                      buffervaluei++;
                      bufferstate = ' ';
                    }
                  } else if (ch == '-' || ch == '.' || (ch >= '0' && ch <= '9')) {
                    // fresh value
                    if (bufferstate == ' ' || bufferstate == '\n') {
                      buffernum = 0.0f;
                      buffernumbase = 10.0f;
                      bufferstate = '+';
                    }
                    // count each char
                    if (ch == '-' && buffernum == 0.0f) {
                      bufferstate = bufferstate - '+' + '-';
                    } else if (ch == '.') {
                      bufferstate += '.';
                      buffernumbase = 0.1f;
                    } else if (ch >= '0' && ch <= '9') {
                      if (bufferstate - '+' == '.' || bufferstate - '-' == '.') {
                        buffernum = buffernum + (((float)(int)(ch - '0')) * buffernumbase);
                      } else {
                        buffernum = (buffernum * buffernumbase) + ((float)(int)(ch - '0'));
                      }
                    }
                  }
// - handle float export
// --------------------------------------------------------------
                }
          //      numcols++;
                stdoutfilei++;
              } else {
                overflowspin = spinkeepalive(overflowspin);
                if (quietdeath != 1) {
                  REDOUT1(inboxindex, "%c", overflowspin, -1)
                  cursormoveleft(inboxindex, 1);
                  PLAINTEXT
                  FLUSHBUFFER
                }
                stdoutfilei++;
              }
            }
            prevch = ch;
          }
          if (shellstatus == 'a') {
            if (waitpid(shellpid, &waitstatvar, WNOHANG) == shellpid) {
              if (WIFEXITED(waitstatvar) != 0) {
                shellreturncode = WEXITSTATUS(waitstatvar);
                shellstatus = 'b';
              }
            }
            inboxkeepalive = spinkeepalive(inboxkeepalive);
            if (quietdeath != 1) {
              GREENOUT1(inboxindex, "%c", inboxkeepalive, -1)
              cursormoveleft(inboxindex, 1);
// WHILE WAITING, CHECK FOR INPUT? HOW? DIFFERENT SHELL STATUS
              PLAINTEXT
              FLUSHBUFFER
            }
            //usleep(INBOXWAITTIME);
            sched_yield();
          } else if (shellstatus == 'b') {
            shellstatus = 'c';
          } else {
            if (quietdeath == 1) {
              break;
            } else {
              if (shellstatus == 'c') {
                GREENOUT(inboxindex, ".", -1)
              } else {
                REDOUT(inboxindex, ".", -1)
              }
              cursormoveleft(inboxindex, 1);
              RESETBG
              PLAINTEXT
              break;
            }
          }
        }
        fclose(stdoutfile);
        if (quietdeath != 1) {
          cursorlinebreak(inboxindex);
          numrows++;
        }
        if (shellreturncode != 0) {
          if (quietdeath != 1) {
            BLUEOUT(inboxindex, "^ ", -2)
            MAGENTAOUT1(inboxindex, "%-5d", shellreturncode, -5)
            cursorlinebreak(inboxindex);
            numrows++;
          }
        }
        if (quietdeath != 1) {
          cursormoveup(inboxindex, numrows);
        }
/*
        cursorswap(inboxindex, KEYOUTINDEX);
        LIGHTOUT1(KEYOUTINDEX, "%s", inputbuffer, -inputbufferlen)
        cursormoveleft(KEYOUTINDEX, inputbufferlen);
        cursorswap(KEYOUTINDEX, inboxindex);
*/
      }
    }
//                                                                                                 
    if (boxtype == INBOXTYPEBACKGROUND && bufferindex != -1) {
      doublebufferdisconnect(bufferindex);
    }
//                                                                                                 
    if (quietdeath == 1) {
      _exit(0);
    } else {
      cursorrelease(inboxindex);
      FLUSHBUFFER
      return ejectpid;
    }
  }
}
