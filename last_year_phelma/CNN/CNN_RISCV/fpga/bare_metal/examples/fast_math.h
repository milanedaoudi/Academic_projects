float floorf(float x) { return (float)((int)x); }
float ceilf(float x)  { int i = (int)x; return (x == (float)i) ? x : (float)(i + 1); }
float fminf(float a, float b) { return a < b ? a : b; }
float fmaxf(float a, float b) { return a > b ? a : b; }


