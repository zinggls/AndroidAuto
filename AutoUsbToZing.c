#include "AutoUsbToZing.h"
#include "cyu3os.h"
#include "cyu3system.h"
#include "cyu3error.h"
#include "Zing.h"

CyU3PThread AutoUsbToZingThreadHandle;
extern CyU3PDmaChannel glChHandleAutoDataIn;

CyU3PReturnStatus_t
CreateAutoUsbToZingThread(
		void)
{
	void *StackPtr = NULL;
	CyU3PReturnStatus_t Status;

	StackPtr = CyU3PMemAlloc(AUTOUSBTOZING_THREAD_STACK);
	Status = CyU3PThreadCreate(&AutoUsbToZingThreadHandle,	// Handle to my Application Thread
				"101:AutoUsbToZing",						// Thread ID and name
				AutoUsbToZingThread,						// Thread entry function
				0,											// Parameter passed to Thread
				StackPtr,									// Pointer to the allocated thread stack
				AUTOUSBTOZING_THREAD_STACK,					// Allocated thread stack size
				AUTOUSBTOZING_THREAD_PRIORITY,				// Thread priority
				AUTOUSBTOZING_THREAD_PRIORITY,				// = Thread priority so no preemption
				CYU3P_NO_TIME_SLICE,						// Time slice no supported
				CYU3P_AUTO_START							// Start the thread immediately
	);
	return Status;
}

void
AutoUsbToZingThread(
		uint32_t Value)
{
	uint32_t rt_len;
	uint8_t *buf = (uint8_t *)CyU3PDmaBufferAlloc (glChHandleAutoDataIn.size);
	CyU3PReturnStatus_t Status;

	CyU3PDebugPrint(4,"[A-Z] AutoDataIn.size=%d\n",glChHandleAutoDataIn.size);
	while(1){
		if((Status=Zing_Transfer_Recv(&glChHandleAutoDataIn,buf,&rt_len,CYU3P_WAIT_FOREVER))==CY_U3P_SUCCESS) {
			CyU3PDebugPrint(4,"A");
		}else{
			CyU3PDebugPrint (4, "[A-Z] Zing_Transfer_Recv error(0x%x)\n",Status);
		}
	}
}
