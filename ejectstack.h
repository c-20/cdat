#define EJECTSTACKTYPEKILL		1

#define MAXEJECTCOMMANDLENGTH	32
#define MAXEJECTLISTLENGTH		32

char ejectstack[MAXEJECTLISTLENGTH][MAXEJECTCOMMANDLENGTH];
int ejectstacki = -1;

void pushejectstack(int type, int ejectpid) {
  if (type == EJECTSTACKTYPEKILL) {
    ejectstacki++;
    sprintf(ejectstack[ejectstacki], "kill %d", ejectpid);
  }
}

void popejectstack() {
  if (ejectstacki >= 0) {
    system(ejectstack[ejectstacki]);
    ejectstacki--;
  }
}
