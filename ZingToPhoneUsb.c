#include "ZingToPhoneUsb.h"
#include "cyu3os.h"
#include "cyu3system.h"
#include "cyu3error.h"
#include "dma.h"
#include "Zing.h"
#include "phonedrv.h"
#include "cyu3usbhost.h"

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
	CyU3PReturnStatus_t Status;
	CyU3PDmaBuffer_t Buf;
	CyU3PUsbHostEpStatus_t epStatus;

	uint32_t rt_len;
	uint8_t *buf = (uint8_t *)CyU3PDmaBufferAlloc (Dma.DataIn_.Channel_.size);

	CyU3PDebugPrint(4,"[Z-P] Zing to Phone USB thread starts\n");
	CyU3PDebugPrint(4,"[Z-P] GpifDataIn.size=%d\n",Dma.DataIn_.Channel_.size);

	while(1){
		if((Status=Zing_Transfer_Recv(&Dma.DataIn_.Channel_,buf,&rt_len,CYU3P_WAIT_FOREVER))==CY_U3P_SUCCESS) {
			CyU3PDebugPrint(4,"[Z-P] %d bytes received from GpifDataIn\r\n",rt_len);
			Buf.buffer = buf;
			Buf.count = rt_len;
			Buf.size = ((rt_len + 0x0F) & ~0x0F);;
			Buf.status = 0;
			CyU3PDebugPrint(4,"[Z-P] count=%d, size=%d\r\n",Buf.count,Buf.size);
			Status = CyU3PDmaChannelSetupSendBuffer (&glChHandlePhoneDataOut, &Buf);
		    if (Status == CY_U3P_SUCCESS)
		    	Status = CyU3PUsbHostEpSetXfer (0x81, CY_U3P_USB_HOST_EPXFER_NORMAL, Buf.count);

		    if (Status == CY_U3P_SUCCESS)
		    	Status = CyU3PUsbHostEpWaitForCompletion (0x81, &epStatus, PHONE_DATAOUT_WAIT_TIMEOUT);

		    if (Status == CY_U3P_SUCCESS)
		    	Status = CyU3PDmaChannelWaitForCompletion (&glChHandlePhoneDataOut, CYU3P_NO_WAIT);

		    if (Status != CY_U3P_SUCCESS)
				CyU3PDebugPrint(4,"[Z-P] sending %d bytes to PhoneDataOut failed error(%d),EP=0x%x,Buf.count=%d,Buf.size=%d,epStatus=0x%x\r\n",
						Buf.count,Status,Phone.outEp,Buf.count,Buf.size,epStatus);
		    else
		    	CyU3PDebugPrint(4,"[Z-P] %d bytes sent to PhoneDataOut\r\n",Buf.size);
		}else{
			CyU3PDebugPrint (4, "[Z-P] Zing_Transfer_Recv error(0x%x)\n",Status);
		}
	}
}
