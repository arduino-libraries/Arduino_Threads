#include "Wire.h"
#define Wire WireReal
#define Wire1 WireReal1
#include "../Wire/Wire.cpp"
#undef Wire
#undef Wire1

WireClassDispatcher Wire(WireReal);
WireClassDispatcher Wire1(WireReal1);
//WireClassDispatcher Wire(WireReal);
//WireClassDispatcher Wire1(Wire1Real);