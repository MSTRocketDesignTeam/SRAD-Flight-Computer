// Author: Seth Sievers
// Date: 4/20/24
// Desc: This cursed file is so that I can declare global variables and include them
// in other files. This is not the best practice but without rewriting premade libraries
// (or excessive pass by ref) this is the easiest. 

#include <Arduino.h>
#include "timer.h"

// TIMERS --------------
extern Timer doComCheck; 
// ---------------------