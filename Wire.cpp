#include "Wire.h"
#define Wire WireReal
#include "../Wire/Wire.cpp"
#undef Wire

WireClassDispatcher Wire(Wire1);
//WireClassDispatcher Wire(WireReal);
//WireClassDispatcher Wire1(Wire1Real);