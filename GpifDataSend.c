#include "GpifDataSend.h"
#include "cyu3system.h"
#include "cyu3error.h"
#include "dma.h"
#include "Zing.h"
#include "phonedrv.h"
#include "cyu3usbhost.h"
#include "PacketFormat.h"
#include "uhbuf.h"

extern CyU3PDmaChannel glChHandlePhoneDataOut;

CyU3PReturnStatus_t
CreateGpifDataSendThread(
		void)
{
	void *StackPtr = NULL;
	CyU3PReturnStatus_t Status;

	StackPtr = CyU3PMemAlloc(GPIFDATASEND_THREAD_STACK);
	Status = CyU3PThreadCreate(&GpifDataSendThreadHandle,	// Handle to my Application Thread
				"203:GpifDataSend",							// Thread ID and name
				GpifDataSendThread,							// Thread entry function
				0,											// Parameter passed to Thread
				StackPtr,									// Pointer to the allocated thread stack
				GPIFDATASEND_THREAD_STACK,					// Allocated thread stack size
				GPIFDATASEND_THREAD_PRIORITY,				// Thread priority
				GPIFDATASEND_THREAD_PRIORITY,				// = Thread priority so no preemption
				CYU3P_NO_TIME_SLICE,						// Time slice no supported
				CYU3P_AUTO_START							// Start the thread immediately
	);
	return Status;
}

void
GpifDataSendThread(
		uint32_t Value)
{
	CyU3PReturnStatus_t Status;

	CyU3PDebugPrint(4,"[GpifDataSend] Gpif Data Sending thread starts\r\n");
	memset(&gpifSendCounter,0,sizeof(gpifSendCounter));
	while(1){
		/* TO DO
		 * retrieve message from queue */

        uint8_t *buffer = 0;
        uint16_t size = 0;

	    if ((Status=CyFxSendBuffer (Phone.outEp,&glChHandlePhoneDataOut,buffer,size)) != CY_U3P_SUCCESS) {
	    	gpifSendCounter.Err++;
			CyU3PDebugPrint(4,"[GpifDataSend] sending %d bytes to PhoneDataOut failed error(0x%x),EP=0x%x\r\n",size,Status,Phone.outEp);
	    }else{
	    	gpifSendCounter.Ok++;
#ifdef DEBUG_THREAD_LOOP
	    	CyU3PDebugPrint(4,"[GpifDataSend] %d bytes sent to PhoneDataOut,EP=0x%x\r\n",size,Phone.outEp);
#endif
	    }
	}
}
