#include "PhoneUsbToZing.h"
#include "cyu3os.h"
#include "cyu3system.h"
#include "cyu3error.h"
#include "Zing.h"
#include "phonedrv.h"
#include "cyu3usbhost.h"

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
	CyU3PDmaBuffer_t Buf;
	CyU3PUsbHostEpStatus_t epStatus;

	CyU3PDebugPrint(4,"[P-Z] Phone USB to Zing thread starts\n");
	CyU3PDebugPrint(4,"[P-Z] PhoneDataIn.size=%d\n",glChHandlePhoneDataIn.size);

	Buf.buffer = (uint8_t *)CyU3PDmaBufferAlloc (glChHandlePhoneDataIn.size);
	Buf.count = 0;
	Buf.size = glChHandlePhoneDataIn.size;
	Buf.status = 0;
	while(1){
		Status = CyU3PDmaChannelSetupRecvBuffer (&glChHandlePhoneDataIn, &Buf);
		if(Status!=CY_U3P_SUCCESS) {
			CyU3PDebugPrint (4, "PhoneUsbToZingThread, CyU3PUsbHostEpSetXfer error(%d)\n", Status);
			continue;
		}

        Status = CyU3PUsbHostEpSetXfer (Phone.inEp, 0, glChHandlePhoneDataIn.size);
        if(Status!=CY_U3P_SUCCESS) {
			CyU3PDebugPrint (4, "PhoneUsbToZingThread, CyU3PUsbHostEpSetXfer error(%d)\n", Status);
			continue;
        }

        Status = CyU3PUsbHostEpWaitForCompletion (Phone.inEp, &epStatus, CYU3P_WAIT_FOREVER);
        if(Status!=CY_U3P_SUCCESS) {
			CyU3PDebugPrint (4, "PhoneUsbToZingThread, CyU3PUsbHostEpWaitForCompletion error(%d)\n", Status);
			continue;
        }

        Status = CyU3PDmaChannelWaitForCompletion (&glChHandlePhoneDataIn,CYU3P_NO_WAIT);
        if(Status!=CY_U3P_SUCCESS) {
			CyU3PDebugPrint (4, "PhoneUsbToZingThread, CyU3PDmaChannelWaitForCompletion error(%d)\n", Status);
        }
        CyU3PDebugPrint (4, "P");
	}
}
