#ifndef SCRIPTFUNCTIONS_H_INCLUDED
#define SCRIPTFUNCTIONS_H_INCLUDED

#include "../App/app.h"
#include "gmThread.h"

#define SCRIPTBIND(CFunction, GMName) \
{ \
gmFunctionEntry newGMFunction = {GMName, CFunction};  \
SCRIPT.machine->RegisterLibrary(&newGMFunction, 1); \
} \

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\

extern void BindScriptFunctions();


#endif