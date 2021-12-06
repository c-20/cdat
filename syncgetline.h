void syncgetline(FILE *f, int linenum, char *buffer) {
  int inch;
  char ch;
  int thislinenum = 1;
  int bufferi = 0;
  buffer[0] = '\0';
  fseek(f, 0, SEEK_SET);
  while (1) {
    inch = fgetc(f);
    if (inch == EOF) break;
    ch = (char)inch;
    if (linenum == thislinenum && ch != '\n') {
      buffer[bufferi] = ch;
      bufferi++;
      buffer[bufferi] = '\0';
    } else if (ch == '\n') {
      thislinenum++;
    }
  }
}
