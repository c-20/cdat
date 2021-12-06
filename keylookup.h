
#define ifequal(a1, b1, a2, b2)	if (a1 == a2 && b1 == b2)

char keylookup(char a, char b, int shift) {
  ifequal(a,b, '3','5') return (shift) ? '~' : '`';
  ifequal(a,b, '1','e') return (shift) ? '!' : '1';
  ifequal(a,b, '1','f') return (shift) ? '@' : '2';
  ifequal(a,b, '2','0') return (shift) ? '#' : '3';
  ifequal(a,b, '2','1') return (shift) ? '$' : '4';
  ifequal(a,b, '2','2') return (shift) ? '%' : '5';
  ifequal(a,b, '2','3') return (shift) ? '^' : '6';
  ifequal(a,b, '2','4') return (shift) ? '&' : '7';
  ifequal(a,b, '2','5') return (shift) ? '*' : '8';
  ifequal(a,b, '2','6') return (shift) ? '(' : '9';
  ifequal(a,b, '2','7') return (shift) ? ')' : '0';

  ifequal(a,b, '1','4') return (shift) ? 'Q' : 'q';
  ifequal(a,b, '1','a') return (shift) ? 'W' : 'w';
  ifequal(a,b, '0','8') return (shift) ? 'E' : 'e';
  ifequal(a,b, '1','5') return (shift) ? 'R' : 'r';
  ifequal(a,b, '1','7') return (shift) ? 'T' : 't';
  ifequal(a,b, '1','c') return (shift) ? 'Y' : 'y';
  ifequal(a,b, '1','8') return (shift) ? 'U' : 'u';
  ifequal(a,b, '0','c') return (shift) ? 'I' : 'i';
  ifequal(a,b, '1','2') return (shift) ? 'O' : 'o';
  ifequal(a,b, '1','3') return (shift) ? 'P' : 'p';

  ifequal(a,b, '0','4') return (shift) ? 'A' : 'a';
  ifequal(a,b, '1','6') return (shift) ? 'S' : 's';
  ifequal(a,b, '0','7') return (shift) ? 'D' : 'd';
  ifequal(a,b, '0','9') return (shift) ? 'F' : 'f';
  ifequal(a,b, '0','a') return (shift) ? 'G' : 'g';
  ifequal(a,b, '0','b') return (shift) ? 'H' : 'h';
  ifequal(a,b, '0','d') return (shift) ? 'J' : 'j';
  ifequal(a,b, '0','e') return (shift) ? 'K' : 'k';
  ifequal(a,b, '0','f') return (shift) ? 'L' : 'l';

  ifequal(a,b, '1','d') return (shift) ? 'Z' : 'z';
  ifequal(a,b, '1','b') return (shift) ? 'X' : 'x';
  ifequal(a,b, '0','6') return (shift) ? 'C' : 'c';
  ifequal(a,b, '1','9') return (shift) ? 'V' : 'v';
  ifequal(a,b, '0','5') return (shift) ? 'B' : 'b';
  ifequal(a,b, '1','1') return (shift) ? 'N' : 'n';
  ifequal(a,b, '1','0') return (shift) ? 'M' : 'm';

  ifequal(a,b, '3','6') return (shift) ? '<' : ',';
  ifequal(a,b, '3','7') return (shift) ? '>' : '.';
  ifequal(a,b, '3','8') return (shift) ? '?' : '/';
  ifequal(a,b, '3','3') return (shift) ? ':' : ';';
  ifequal(a,b, '3','4') return (shift) ? '"' : '\'';
  ifequal(a,b, '2','f') return (shift) ? '{' : '[';
  ifequal(a,b, '3','0') return (shift) ? '}' : ']';
  ifequal(a,b, '3','1') return (shift) ? '|' : '\\';
  ifequal(a,b, '2','d') return (shift) ? '_' : '-';
  ifequal(a,b, '2','e') return (shift) ? '+' : '=';

  ifequal(a,b, '2','c') return (shift) ? ' ' : ' ';

  ifequal(a,b, '2','8') return '\n';
  ifequal(a,b, '2','a') return '\b';

  return '\0';
}

char altkeylookup(char a, char b, int shift) {
  ifequal(a,b, '4','f') return (shift) ? 'r' : 'w';
  ifequal(a,b, '5','0') return (shift) ? 'l' : 'a';
  ifequal(a,b, '5','1') return (shift) ? 'd' : 'z';
  ifequal(a,b, '5','2') return (shift) ? 'u' : 's';

  return '\0';
}

#undef ifequal
