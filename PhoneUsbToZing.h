#ifndef __PHONEUSBTOZING_H__
#define __PHONEUSBTOZING_H__

#include "cyu3types.h"

#define PHONEUSBTOZING_THREAD_STACK		(0x1000)
#define PHONEUSBTOZING_THREAD_PRIORITY	(8)

CyU3PReturnStatus_t
CreatePhoneUsbToZingThread(
		void);

void
PhoneUsbToZingThread(
		uint32_t Value);

#endif
