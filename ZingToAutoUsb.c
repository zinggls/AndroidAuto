#include "ZingToAutoUsb.h"
#include "cyu3os.h"
#include "cyu3system.h"

CyU3PThread ZingToAutoUsbThreadHandle;

CyU3PReturnStatus_t
CreateZingToAutoUsbThread(
		void)
{
	void *StackPtr = NULL;
	CyU3PReturnStatus_t Status;

	StackPtr = CyU3PMemAlloc(ZINGTOAUTOUSB_THREAD_STACK);
	Status = CyU3PThreadCreate(&ZingToAutoUsbThreadHandle,	// Handle to my Application Thread
				"102:ZingToAutoUsb",						// Thread ID and name
				ZingToAutoUsbThread,						// Thread entry function
				0,											// Parameter passed to Thread
				StackPtr,									// Pointer to the allocated thread stack
				ZINGTOAUTOUSB_THREAD_STACK,					// Allocated thread stack size
				ZINGTOAUTOUSB_THREAD_PRIORITY,				// Thread priority
				ZINGTOAUTOUSB_THREAD_PRIORITY,				// = Thread priority so no preemption
				CYU3P_NO_TIME_SLICE,						// Time slice no supported
				CYU3P_AUTO_START							// Start the thread immediately
	);
	return Status;
}

void
ZingToAutoUsbThread(
		uint32_t Value)
{

}
