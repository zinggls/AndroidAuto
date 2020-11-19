#include "AutoUsbToZing.h"
#include "cyu3os.h"
#include "cyu3system.h"

CyU3PThread AutoUsbToZingThreadHandle;

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

}
