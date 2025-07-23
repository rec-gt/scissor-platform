#ifndef Utils_h
#define Utils_h

int getBitIndex(int num) {
  if (num == 0) { return 0; }
  int idx = 0;
  while ((num & 1) != 1) {
    num = num >> 1;
    idx++;
  }
  return 7 - idx;
}

boolean isNumber(String str) {
  for (int i = 0; i < str.length(); i++) {
    if (!isDigit(str.charAt(i))) {
      return false;
    }
  }
  return true;
}

#endif