/*
Copyright (c) <year> yuki-js
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <stdint.h>
#include "config.h"
struct motorConfig motorList[MOTOR_LENGTH]={
  {24,"FL1",M_X|M_P|M_CW,0},
  {23,"FL2",M_Y|M_P|M_CCW,0},
  {15,"FR1",M_X|M_N|M_CW,0},
  {18,"FR2",M_Y|M_P|M_CCW,0},
  {10,"BL1",M_X|M_P|M_CW,0},
  {22,"BL2",M_Y|M_N|M_CCW,0},
  {17,"BR1",M_X|M_N|M_CW,0},
  {27,"BR2",M_Y|M_N|M_CCW,0}
};
