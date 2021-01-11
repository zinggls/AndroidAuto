#include "helper.h"
#include "cyu3types.h"
#include "cyu3os.h"
#include "cyu3system.h"
#include "cyu3error.h"
#include "Zing.h"
#include "ZingHw.h"
#include "PacketFormat.h"
#include "dma.h"

void
SendMessage (
		const char *msg)
{
	CyU3PReturnStatus_t status;
	PacketFormat *pf;
	if((pf=(PacketFormat*)CyU3PDmaBufferAlloc(520))==0){
		CyU3PDebugPrint(4,"[Z-A] %s, PacketFormat CyU3PDmaBufferAlloc error\r\n",msg);
		return;
	}

	pf->size = strlen(msg);
	CyU3PMemCopy (pf->data,(uint8_t*)msg,pf->size);
	if ((status = Zing_DataWrite((uint8_t*)pf, pf->size+sizeof(uint32_t))) == CY_U3P_SUCCESS) {
#ifdef DEBUG
		CyU3PDebugPrint(4,"[Phone] %s %d bytes sent\n",msg,pf->size);
#endif
	}else{
		CyU3PDebugPrint(4,"[Phone] %s sent failed error: %d\n",msg,status);
		CyU3PThreadSleep (10);
		CyU3PDeviceReset(CyFalse);
	}
	CyU3PDmaBufferFree(pf);

	/* Experiment, send message via control channel */
	Zing_Header2(Dma.ControlOut_.Buffer_,1,0,0,0,0,1,0,0,strlen(msg));
	memcpy(Dma.ControlOut_.Buffer_+ZING_HDR_SIZE, (uint8_t*)msg, strlen(msg));
	if ((status = Zing_Transfer_Send(&Dma.ControlOut_.Channel_,Dma.ControlOut_.Buffer_,strlen(msg)+ZING_HDR_SIZE)) == CY_U3P_SUCCESS) {
#ifdef DEBUG
		CyU3PDebugPrint(4,"[Phone] %s %d bytes sent\n",msg,strlen(msg));
#endif
	}else{
		CyU3PDebugPrint(4,"[Phone] %s sent failed error: %d\n",msg,status);
	}
}
