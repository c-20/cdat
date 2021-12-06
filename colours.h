
#define MOVEUP(n)     fprintf(stdout, "\033[%dA", n);
#define MOVEDOWN(n)   fprintf(stdout, "\033[%dB", n);
#define MOVETOSOL     fprintf(stdout, "\n\033[1A");
#define MOVETOEOL     fprintf(stdout, "\n\033[1D");
#define LINEBREAK(w)  fprintf(stdout, "\033[%dD\033[1B", w);
#define MOVERIGHT(n)  fprintf(stdout, "\033[%dC", n);
#define MOVELEFT(n)   fprintf(stdout, "\033[%dD", n);
#define SETCHAR(c)    fprintf(stdout, "%c\033[1D", c);
#define WRITECHAR(c)  fprintf(stdout, "%c", c);
#define LWRITECHAR(c) fprintf(stdout, "%c\033[2D", c);
#ifndef FLUSHBUFFER
#define FLUSHBUFFER   fflush(stdout);
#endif
#define CLEARLINE     fprintf(stdout, "\033[2K");
// http://linux.about.com/library/cmd/blcmdl4_console_codes.htm

#define TEXTPRINT(str)					\
		  fprintf(stdout, str);
#define TEXTPRINT1(str, p1)				\
		  fprintf(stdout, str, p1);
#define TEXTPRINT2(str, p1, p2)			\
		  fprintf(stdout, str, p1, p2);
#define TEXTPRINT3(str, p1, p2, p3)		\
		  fprintf(stdout, str, p1, p2, p3);
#define TEXTPRINT4(str, p1, p2, p3, p4)	\
		  fprintf(stdout, str, p1, p2, p3, p4);
#define CURSORMOVE(cur, len)			\
		  cursormoveright(cur, len);

#define REDCOLOUR		"\033[1;31m"
#define REDTEXT			fprintf(stdout, "%s", REDCOLOUR);
#define YELLOWCOLOUR	"\033[1;33m"
#define YELLOWTEXT		fprintf(stdout, "%s", YELLOWCOLOUR);
#define GREENCOLOUR		"\033[1;32m"
#define GREENTEXT		fprintf(stdout, "%s", GREENCOLOUR);
#define CYANCOLOUR		"\033[1;36m"
#define CYANTEXT		fprintf(stdout, "%s", CYANCOLOUR);
#define BLUECOLOUR		"\033[1;34m"
#define BLUETEXT		fprintf(stdout, "%s", BLUECOLOUR);
#define MAGENTACOLOUR	"\033[1;35m"
#define MAGENTATEXT		fprintf(stdout, "%s", MAGENTACOLOUR);
#define BLACKCOLOUR		"\033[0;30m"
#define BLACKTEXT		fprintf(stdout, "%s", BLACKCOLOUR);
#define DARKCOLOUR		"\033[1;30m"
#define DARKTEXT		fprintf(stdout, "%s", DARKCOLOUR);
#define LIGHTCOLOUR		"\033[0;37m"
#define LIGHTTEXT		fprintf(stdout, "%s", LIGHTCOLOUR);
#define WHITECOLOUR		"\033[1;37m"
#define WHITETEXT		fprintf(stdout, "%s", WHITECOLOUR);
#define GROUNDCOLOUR	"\033[0;33m"
#define GROUNDTEXT		fprintf(stdout, "%s", GROUNDCOLOUR);
#define PLAINCOLOUR		"\033[0m"
#define PLAINTEXT		fprintf(stdout, "%s", PLAINCOLOUR);
/*
#define REDBG        fprintf(stdout, "\033[41m");
#define YELLOWBG     fprintf(stdout, "\033[43m");
#define GREENBG      fprintf(stdout, "\033[42m");
#define CYANBG       fprintf(stdout, "\033[46m");
#define BLUEBG       fprintf(stdout, "\033[44m");
#define MAGENTABG    fprintf(stdout, "\033[45m");
#define BLACKBG      fprintf(stdout, "\033[40m");
#define WHITEBG      fprintf(stdout, "\033[47m");
#define RESETBG      fprintf(stdout, "\033[0m");

#define CLEARSCREEN  fprintf(stdout, "\033[2J");
*/
#define CHARCOLOUR(ch)	\
       if (ch == 'R') { REDTEXT     } \
  else if (ch == 'Y') { YELLOWTEXT  } \
  else if (ch == 'G') { GREENTEXT   } \
  else if (ch == 'C') { CYANTEXT    } \
  else if (ch == 'B') { BLUETEXT    } \
  else if (ch == 'M') { MAGENTATEXT } \
  else if (ch == 'O') { GROUNDTEXT  } \
  else if (ch == 'W') { WHITETEXT   }

#define OUT(cur, str, len)							\
		  TEXTPRINT(str) CURSORMOVE(cur, len)
#define OUT1(cur, str, p1, len)						\
		  TEXTPRINT1(str, p1) CURSORMOVE(cur, len)
#define OUT2(cur, str, p1, p2, len)					\
		  TEXTPRINT2(str, p1, p2) CURSORMOVE(cur, len)

#define REDOUT(cur, str, len)						\
		  REDTEXT TEXTPRINT(str) CURSORMOVE(cur, len)
#define REDOUT1(cur, str, p1, len)					\
		  REDTEXT TEXTPRINT1(str, p1) CURSORMOVE(cur, len)
#define REDOUT2(cur, str, p1, p2, len)				\
		  REDTEXT TEXTPRINT2(str, p1, p2) CURSORMOVE(cur, len)
#define YELLOWOUT(cur, str, len)					\
		  YELLOWTEXT TEXTPRINT(str) CURSORMOVE(cur, len)
#define YELLOWOUT1(cur, str, p1, len)				\
		  YELLOWTEXT TEXTPRINT1(str, p1) CURSORMOVE(cur, len)
#define YELLOWOUT2(cur, str, p1, p2, len)			\
		  YELLOWTEXT TEXTPRINT2(str, p1, p2) CURSORMOVE(cur, len)
#define GREENOUT(cur, str, len)						\
		  GREENTEXT TEXTPRINT(str) CURSORMOVE(cur, len)
#define GREENOUT1(cur, str, p1, len)				\
		  GREENTEXT TEXTPRINT1(str, p1) CURSORMOVE(cur, len)
#define GREENOUT2(cur, str, p1, p2, len)			\
		  GREENTEXT TEXTPRINT2(str, p1, p2) CURSORMOVE(cur, len)
#define CYANOUT(cur, str, len)						\
		  CYANTEXT TEXTPRINT(str) CURSORMOVE(cur, len)
#define CYANOUT1(cur, str, p1, len)					\
		  CYANTEXT TEXTPRINT1(str, p1) CURSORMOVE(cur, len)
#define CYANOUT2(cur, str, p1, p2, len)				\
		  CYANTEXT TEXTPRINT2(str, p1, p2) CURSORMOVE(cur, len)
#define BLUEOUT(cur, str, len)						\
		  BLUETEXT TEXTPRINT(str) CURSORMOVE(cur, len)
#define BLUEOUT1(cur, str, p1, len)					\
		  BLUETEXT TEXTPRINT1(str, p1) CURSORMOVE(cur, len)
#define BLUEOUT2(cur, str, p1, p2, len)				\
		  BLUETEXT TEXTPRINT2(str, p1, p2) CURSORMOVE(cur, len)
#define MAGENTAOUT(cur, str, len)					\
		  MAGENTATEXT TEXTPRINT(str) CURSORMOVE(cur, len)
#define MAGENTAOUT1(cur, str, p1, len)				\
		  MAGENTATEXT TEXTPRINT1(str, p1) CURSORMOVE(cur, len)
#define MAGENTAOUT2(cur, str, p1, p2, len)			\
		  MAGENTATEXT TEXTPRINT2(str, p1, p2) CURSORMOVE(cur, len)
#define BLACKOUT(cur, str, len)						\
		  BLACKTEXT TEXTPRINT(str) CURSORMOVE(cur, len)
#define BLACKOUT1(cur, str, p1, len)				\
		  BLACKTEXT TEXTPRINT1(str, p1) CURSORMOVE(cur, len)
#define BLACKOUT2(cur, str, p1, p2, len)			\
		  BLACKTEXT TEXTPRINT2(str, p1, p2) CURSORMOVE(cur, len)
#define DARKOUT(cur, str, len)						\
		  DARKTEXT TEXTPRINT(str) CURSORMOVE(cur, len)
#define DARKOUT1(cur, str, p1, len)					\
		  DARKTEXT TEXTPRINT1(str, p1) CURSORMOVE(cur, len)
#define DARKOUT2(cur, str, p1, p2, len)				\
		  DARKTEXT TEXTPRINT2(str, p1, p2) CURSORMOVE(cur, len)
#define LIGHTOUT(cur, str, len)						\
		  LIGHTTEXT TEXTPRINT(str) CURSORMOVE(cur, len)
#define LIGHTOUT1(cur, str, p1, len)				\
		  LIGHTTEXT TEXTPRINT1(str, p1) CURSORMOVE(cur, len)
#define LIGHTOUT2(cur, str, p1, p2, len)			\
		  LIGHTTEXT TEXTPRINT2(str, p1, p2) CURSORMOVE(cur, len)
#define WHITEOUT(cur, str, len)						\
		  WHITETEXT TEXTPRINT(str) CURSORMOVE(cur, len)
#define WHITEOUT1(cur, str, p1, len)				\
		  WHITETEXT TEXTPRINT1(str, p1) CURSORMOVE(cur, len)
#define WHITEOUT2(cur, str, p1, p2, len)			\
		  WHITETEXT TEXTPRINT2(str, p1, p2) CURSORMOVE(cur, len)
#define GROUNDOUT(cur, str, len)					\
		  GROUNDTEXT TEXTPRINT(str) CURSORMOVE(cur, len)
#define GROUNDOUT1(cur, str, p1, len)				\
		  GROUNDTEXT TEXTPRINT1(str, p1) CURSORMOVE(cur, len)
#define GROUNDOUT2(cur, str, p1, p2, len)			\
		  GROUNDTEXT TEXTPRINT2(str, p1, p2) CURSORMOVE(cur, len)
