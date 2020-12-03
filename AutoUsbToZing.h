#ifndef __AUTOUSBTOZING_H__
#define __AUTOUSBTOZING_H__

#include "cyu3types.h"
#include "Counter.h"

#define AUTOUSBTOZING_THREAD_STACK		(0x1000)
#define AUTOUSBTOZING_THREAD_PRIORITY	(8)

CyU3PReturnStatus_t
CreateAutoUsbToZingThread(
		void);

void
AutoUsbToZingThread(
		uint32_t Value);

Counter autoUsbToZingCnt;

#endif
