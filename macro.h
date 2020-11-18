#ifndef __MACRO_H__
#define __MACRO_H__

#include "cyu3types.h"

#define CHECK(x) do { CyU3PReturnStatus_t retval = (x); if(retval != CY_U3P_SUCCESS) { return retval; } } while (0)

#endif
