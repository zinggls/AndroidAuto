#ifndef __PHONEDATASEND_H__
#define __PHONEDATASEND_H__

#include "cyu3types.h"
#include "cyu3os.h"
#include "Counter.h"

#define PHONEDATASEND_THREAD_STACK		(0x800)
#define PHONEDATASEND_THREAD_PRIORITY	(8)

CyU3PReturnStatus_t
CreatePhoneDataSendThread(
		void);

void
PhoneDataSendThread(
		uint32_t Value);

Counter phoneSendCounter;
CyU3PThread PhoneDataSendThreadHandle;

#endif
