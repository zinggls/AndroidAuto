#include "PhoneUsbToZing.h"
#include "cyu3os.h"
#include "cyu3system.h"
#include "cyu3error.h"
#include "Zing.h"
#include "phonedrv.h"
#include "cyu3usbhost.h"
#include "uhbuf.h"

CyU3PThread PhoneUsbToZingThreadHandle;
extern CyU3PDmaChannel glChHandlePhoneDataIn;

CyU3PReturnStatus_t
CreatePhoneUsbToZingThread(
		void)
{
	void *StackPtr = NULL;
	CyU3PReturnStatus_t Status;

	StackPtr = CyU3PMemAlloc(PHONEUSBTOZING_THREAD_STACK);
	Status = CyU3PThreadCreate(&PhoneUsbToZingThreadHandle,	// Handle to my Application Thread
				"201:PhoneUsbToZing",						// Thread ID and name
				PhoneUsbToZingThread,						// Thread entry function
				0,											// Parameter passed to Thread
				StackPtr,									// Pointer to the allocated thread stack
				PHONEUSBTOZING_THREAD_STACK,				// Allocated thread stack size
				PHONEUSBTOZING_THREAD_PRIORITY,				// Thread priority
				PHONEUSBTOZING_THREAD_PRIORITY,				// = Thread priority so no preemption
				CYU3P_NO_TIME_SLICE,						// Time slice no supported
				CYU3P_AUTO_START							// Start the thread immediately
	);
	return Status;
}

void
PhoneUsbToZingThread(
		uint32_t Value)
{
	CyU3PReturnStatus_t Status;
	uint32_t rt_len;
	CyU3PDmaBuffer_t Buf;
	uint8_t *buf = (uint8_t *)CyU3PDmaBufferAlloc (glChHandlePhoneDataIn.size);

	CyU3PThreadSleep (1000);
	CyU3PDebugPrint(4,"[P-Z] Phone USB to Zing thread starts\n");
	CyU3PDebugPrint(4,"[P-Z] PhoneDataIn.size=%d\n",glChHandlePhoneDataIn.size);

	Buf.buffer = buf;
	Buf.count = 0;
	Buf.size = glChHandlePhoneDataIn.size;
	Buf.status = 0;
	while(1){
	    if ((Status=CyFxRecvBuffer (Phone.inEp,&glChHandlePhoneDataIn,Buf.buffer,Buf.size,&rt_len)) != CY_U3P_SUCCESS)
			CyU3PDebugPrint(4,"[P-Z] receiving from PhoneDataIn failed error(0x%x),EP=0x%x\r\n",Status,Phone.inEp);
	    else
	    	CyU3PDebugPrint(4,"[P-Z] %d bytes received from PhoneDataIn\r\n",rt_len);

		if((Status=Zing_DataWrite(buf,rt_len))==CY_U3P_SUCCESS) {
			CyU3PDebugPrint(4,"[P-Z] %d bytes sent to GpifDataOut\r\n",rt_len);
		}else{
			CyU3PDebugPrint (4, "[P-Z] Zing_DataWrite error(0x%x)\n",Status);
		}
	}
}
