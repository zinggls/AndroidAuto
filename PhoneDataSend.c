#include "PhoneDataSend.h"
#include "cyu3os.h"
#include "cyu3system.h"
#include "cyu3error.h"
#include "Zing.h"
#include "phonedrv.h"
#include "cyu3usbhost.h"
#include "uhbuf.h"
#include "PacketFormat.h"

CyU3PReturnStatus_t
CreatePhoneDataSendThread(
		void)
{
	void *StackPtr = NULL;
	CyU3PReturnStatus_t Status;

	StackPtr = CyU3PMemAlloc(PHONEDATASEND_THREAD_STACK);
	Status = CyU3PThreadCreate(&PhoneDataSendThreadHandle,	// Handle to my Application Thread
				"302:PhoneDataSend",						// Thread ID and name
				PhoneDataSendThread,						// Thread entry function
				0,											// Parameter passed to Thread
				StackPtr,									// Pointer to the allocated thread stack
				PHONEDATASEND_THREAD_STACK,					// Allocated thread stack size
				PHONEDATASEND_THREAD_PRIORITY,				// Thread priority
				PHONEDATASEND_THREAD_PRIORITY,				// = Thread priority so no preemption
				CYU3P_NO_TIME_SLICE,						// Time slice no supported
				CYU3P_AUTO_START							// Start the thread immediately
	);
	return Status;
}

void
PhoneDataSendThread(
		uint32_t Value)
{
	CyU3PReturnStatus_t Status;

	PacketFormat *pf;
	if((pf=(PacketFormat*)CyU3PDmaBufferAlloc(512*2))==0){
		CyU3PDebugPrint(4,"[PhoneDataSend] PacketFormat CyU3PDmaBufferAlloc error\r\n");
		return;
	}

	CyU3PDebugPrint(4,"[PhoneDataSend] Phone Data Sending thread starts\n");
	memset(&phoneSendCounter,0,sizeof(phoneSendCounter));
	while(1){
		/* TO DO
		 * retrieve message from queue */

		if((Status=Zing_DataWrite((uint8_t*)pf,pf->size+sizeof(uint32_t)))==CY_U3P_SUCCESS) {
			phoneSendCounter.Ok++;
#ifdef DEBUG_THREAD_LOOP
			CyU3PDebugPrint(4,"[PhoneDataSend] %d bytes sent to GpifDataOut\r\n",rt_len);
#endif
		}else{
			phoneSendCounter.Err++;
			CyU3PDebugPrint (4, "[PhoneDataSend] Zing_DataWrite(%d) error(0x%x)\n",pf->size+sizeof(uint32_t),Status);
		}
	}
}
