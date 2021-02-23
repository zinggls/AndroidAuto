#ifndef __PHONEUSBTOZING_H__
#define __PHONEUSBTOZING_H__

#include "cyu3types.h"
#include "thread.h"

#define PHONEUSBTOZING_THREAD_STACK		(0x800)
#define PHONEUSBTOZING_THREAD_PRIORITY	(8)

CyU3PReturnStatus_t
CreatePhoneUsbToZingThread(
		void);

typedef struct CyU3PDmaChannel CyU3PDmaChannel;

CyBool_t
ReceiveFromPhoneDataIn(
		uint8_t ep,
		CyU3PDmaChannel *dmaCh,
        uint8_t *buffer,
        uint16_t count,
        uint32_t *length);

CyBool_t
SendToGpifDataOutByPhone(
		PacketFormat *pf,
		uint32_t pfSize);

void
PhoneUsbToZingThread(
		uint32_t Value);

Thread_t phoneUsbToZing;
CyBool_t phoneUsbToZingTerminate;

#endif
