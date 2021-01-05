#ifndef __THREAD_H__
#define __THREAD_H__

#include "cyu3os.h"
#include "Counter.h"

typedef struct Thread_t
{
	CyU3PThread Handle_;
	void *StackPtr_;
	Counter Count_;
} Thread_t;

#endif
