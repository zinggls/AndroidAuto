#ifndef __AUTOUSBTOZING_H__
#define __AUTOUSBTOZING_H__

#include "cyu3types.h"
#include "thread.h"

#define AUTOUSBTOZING_THREAD_STACK		(0x800)
#define AUTOUSBTOZING_THREAD_PRIORITY	(8)

CyU3PReturnStatus_t
CreateAutoUsbToZingThread(
		void);

void
AutoUsbToZingThread(
		uint32_t Value);

Thread_t autoUsbToZing;

#endif
