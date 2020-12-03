#ifndef __COUNTER_H__
#define __COUNTER_H__

#include "cyu3types.h"

typedef struct _Counter{
    uint32_t	receiveOk;
    uint32_t	receiveErr;
    uint32_t	sendOk;
    uint32_t	sendErr;	
}Counter;

#endif
