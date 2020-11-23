#include "PhoneUsbToZing.h"
#include "cyu3os.h"
#include "cyu3system.h"
#include "cyu3error.h"
#include "Zing.h"

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
	uint32_t rt_len;
	uint8_t *buf = (uint8_t *)CyU3PDmaBufferAlloc (glChHandlePhoneDataIn.size);
	CyU3PReturnStatus_t Status;

	CyU3PDebugPrint(4,"[P-Z] PhoneDataIn.size=%d\n",glChHandlePhoneDataIn.size);
	while(1){
		if((Status=Zing_Transfer_Recv(&glChHandlePhoneDataIn,buf,&rt_len,CYU3P_WAIT_FOREVER))==CY_U3P_SUCCESS) {
			if((Status=Zing_DataWrite(buf,rt_len))==CY_U3P_SUCCESS) {
				CyU3PDebugPrint(4,"P");
			}else{
				CyU3PDebugPrint (4, "[P-Z] Zing_DataWrite error(0x%x)\n",Status);
			}
		}else{
			CyU3PDebugPrint (4, "[P-Z] Zing_Transfer_Recv error(0x%x)\n",Status);
		}
	}
}
