#ifndef __ZINGTOPHONEUSB_H__
#define __ZINGTOPHONEUSB_H__

#include "cyu3types.h"

#define ZINGTOPHONEUSB_THREAD_STACK		(0x1000)
#define ZINGTOPHONEUSB_THREAD_PRIORITY	(8)

CyU3PReturnStatus_t
CreateZingToPhoneUsbThread(
		void);

void
ZingToPhoneUsbThread(
		uint32_t Value);

#endif
