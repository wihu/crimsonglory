#include <iostream>
#include <ostream>
#include <istream>
#include <fstream>
#include <list>
#include <string>
#include <map>
#include <deque>
#include <vector>
#include <stack>
#include <algorithm>
#include <functional>
#include <cassert>
#include <sstream>
#include <cmath>
#include <ctime>
#include <wchar.h>
#include <malloc.h> // alloca
#include <new>
#include <cassert>
#include <stdlib.h> // atoi, strtoul (binds: rand, srand)
#include <stdio.h> // fprintf, sprintf, _snprintf, _vnsprintf
#include <stdarg.h> // va_start, va_end
#include <ctype.h> // isdigit
#include <math.h> // floorf, fmodf

#include "../Irrlicht/irrlicht.h"

// typedefs and using namespaces
typedef irr::core::stringw WideString;
typedef irr::core::stringc String;


#ifdef _WIN32
// WINDOWS
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#else
//OTHER
#include "unistd.h"
#include <sys/time.h>
#include <signal.h>
#endif
