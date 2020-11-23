#include "ZingToPhoneUsb.h"
#include "cyu3os.h"
#include "cyu3system.h"
#include "cyu3error.h"
#include "dma.h"
#include "Zing.h"

CyU3PThread ZingToPhoneUsbThreadHandle;
extern CyU3PDmaChannel glChHandlePhoneDataOut;

CyU3PReturnStatus_t
CreateZingToPhoneUsbThread(
		void)
{
	void *StackPtr = NULL;
	CyU3PReturnStatus_t Status;

	StackPtr = CyU3PMemAlloc(ZINGTOPHONEUSB_THREAD_STACK);
	Status = CyU3PThreadCreate(&ZingToPhoneUsbThreadHandle,	// Handle to my Application Thread
				"202:ZingToPhoneUsb",						// Thread ID and name
				ZingToPhoneUsbThread,						// Thread entry function
				0,											// Parameter passed to Thread
				StackPtr,									// Pointer to the allocated thread stack
				ZINGTOPHONEUSB_THREAD_STACK,				// Allocated thread stack size
				ZINGTOPHONEUSB_THREAD_PRIORITY,				// Thread priority
				ZINGTOPHONEUSB_THREAD_PRIORITY,				// = Thread priority so no preemption
				CYU3P_NO_TIME_SLICE,						// Time slice no supported
				CYU3P_AUTO_START							// Start the thread immediately
	);
	return Status;
}

void
ZingToPhoneUsbThread(
		uint32_t Value)
{
	uint32_t rt_len;
	uint8_t *buf = (uint8_t *)CyU3PDmaBufferAlloc (Dma.DataIn_.Channel_.size);
	CyU3PReturnStatus_t Status;

	CyU3PDebugPrint(4,"[Z-P] GpifDataIn.size=%d\n",Dma.DataIn_.Channel_.size);
	while(1){

	}
}
