// returns the length of str, terminated by term
int lenofstr(char *str, char term) {
  int stn = 0;
  while (str[stn] != term)
    { stn++; }
  return stn;
}

// checks if str1, terminated by term1, is equal to
// str2, terminated by term2
int strequal(char *str1, char term1, char *str2, char term2) {
  int sti = 0;
  while (str1[sti] != term1 && str2[sti] != term2)
    { sti++; }
  if (str1[sti] != term1 || str2[sti] != term2)
    { return 0; }
  else { return 1; }
}

// copies str1, terminated by term1, char by char, to str2,
// then writes terminating char term2, and returns num chars copied
int strtostr(char *str1, char term1, char *str2, char term2) {
  int sti = 0;
  while (str1[sti] != term1)
    { str2[sti] = str1[sti]; sti++; }
  str2[sti] = term2;
  return sti;
}

// appends end to str and returns new str length
int strappend(char *str, char term, char *end, char eterm) {
  int sti = lenofstr(str, term);
  int stj = 0;
  while (end[stj] != eterm)
    { str[sti + stj] = end[stj]; stj++; }
  str[sti + stj] = term;
  return sti + stj;
}

// appends two ends to str and returns new str length
int strappendend(char *str, char term, char *end1, char *end2, char eterm) {
  strappend(str, term, end1, eterm);
  return strappend(str, term, end2, eterm);
}

// appends three ends to str and returns new str length
int strappendendend(char *str, char term, char *end1, char *end2, char *end3, char eterm) {
  strappend(str, term, end1, eterm);
  strappend(str, term, end2, eterm);
  return strappend(str, term, end3, eterm);
}

// finds the first occurrence of the snip delimeter, terminates
// the string before it and returns the string after it
char *strsnip(char *str, char *snip) {
  int sniplen = 0;
  while (snip[sniplen] != '\0')
    { sniplen++; }
  int i = 0;
  while (str[i] != '\0') {
    int j = 0;
    while (str[i - j] == snip[sniplen - j - 1]) {
      if (sniplen == j + 1) {
        str[i - j] = '\0';
        return &str[i + 1];
      } else {
        j++;
        if (i - j < 0)
          { break; }
      }
    }
    i++;
  }
  return NULL;
}

// copies snap to the end of str and returns str
char *strsnap(char *str, char *snap) {
  int i = 0;
  while (str[i] != '\0')
    { i++; }
  int j = 0;
  while (snap[j] != '\0') {
    str[i + j] = snap[j];
    j++;
  }
  str[i + j] = '\0';
  return str;
}

// copies an int of type (B)inary, (H)exadecimal,
// (U)nsigned or (S)igned from the beginning of str
int strheadint(char *str, char type) {
  int sti = 0;
  int stval = 0;
  int stneg = 0;
  if (type == 'B') {		// binary
    while (str[sti] == '0' || str[sti] == '1')
      { stval = (stval << 1) | ((str[sti] == '1') ? 1 : 0); sti++; }
    return stval;
  } else if (type == 'H') {	// hexadecimal
    while ((str[sti] >= '0' && str[sti] <= '9') ||
           (str[sti] >= 'A' && str[sti] <= 'F') ||
           (str[sti] >= 'a' && str[sti] <= 'f')) {
      if (str[sti] >= '0' && str[sti] <= '9')
        { stval = (stval << 4) | (int)(str[sti] - '0'); }
      else if (str[sti] >= 'A' && str[sti] <= 'F')
        { stval = (stval << 4) | (int)(str[sti] - 'A' + 10); }
      else if (str[sti] >= 'a' && str[sti] <= 'f')
        { stval = (stval << 4) | (int)(str[sti] - 'a' + 10); }
      sti++;
    }
    return stval;
  } else if (type == 'U') {	// unsigned integer
    while (str[sti] >= '0' && str[sti] <= '9')
      { stval = (stval * 10) + (int)(str[sti] - '0'); sti++; }
    return stval;
  } else if (type == 'S') {	// signed integer
    if (str[0] == '-')
      { stneg = 1; str = &str[1]; }
    while (str[sti] >= '0' && str[sti] <= '9')
      { stval = (stval * 10) + (int)(str[sti] - '0'); sti++; }
    return (stneg == 1) ? (0 - stval) : stval;
  }
}

// copies an int of type (B)inary, (H)exadecimal,
// (U)nsigned or (S)igned from the end of str
int strtailint(char *str, char type) {
  int stlen = 0;
  while (str[stlen] != '\0')
    { stlen++; }
  int sti = 1;
  int stval = 0;
  int stmul = 1;
  if (type == 'B') {		// binary
    while ((stlen - sti >= 0) &&
           (str[stlen - sti] == '0' || str[stlen - sti] == '1'))
      { stval = stval | (((str[stlen - sti] == '1') ? 1 : 0) << (sti - 1)); sti++; }
    return stval;
  } else if (type == 'H') {	// hexadecimal
    while ((stlen - sti >= 0) &&
           ((str[stlen - sti] >= '0' && str[stlen - sti] <= '9') ||
            (str[stlen - sti] >= 'A' && str[stlen - sti] <= 'F') ||
            (str[stlen - sti] >= 'a' && str[stlen - sti] <= 'f'))) {
      if (str[stlen - sti] >= '0' && str[stlen - sti] <= '9')
        { stval = stval | (((int)(str[stlen - sti] - '0')) << (4 * (sti - 1))); }
      else if (str[stlen - sti] >= 'A' && str[stlen - sti] <= 'F')
        { stval = stval | (((int)(str[stlen - sti] - 'A' + 10)) << (4 * (sti - 1))); }
      else if (str[stlen - sti] >= 'a' && str[stlen - sti] <= 'f')
        { stval = stval | (((int)(str[stlen - sti] - 'a' + 10)) << (4 * (sti - 1))); }
      sti++;
    }
    return stval;
  } else if (type == 'U') {	// unsigned integer
    while ((stlen - sti >= 0) &&
           (str[stlen - sti] >= '0' && str[stlen - sti] <= '9'))
      { stval = stval + (((int)(str[stlen - sti] - '0')) * stmul); stmul *= 10; sti++; }
    return stval;
  } else if (type == 'S') {	// signed integer
    while ((stlen - sti >= 0) &&
           (str[stlen - sti] >= '0' && str[stlen - sti] <= '9'))
      { stval = stval + (((int)(str[stlen - sti] - '0')) * stmul); stmul *= 10; sti++; }
    if ((stlen - sti >= 0) && (str[stlen - sti] == '-'))
      { return 0 - stval; } else { return stval; }
  }
  return 0;
}

