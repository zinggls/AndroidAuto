#include "cyu3error.h"
#include "cyu3system.h"
#include "cyu3usb.h"
#include "cyu3gpio.h"
#include "setup.h"
#include "gpio.h"
#include "i2c.h"
#include "PIB.h"
#include "dma.h"
#include "ControlCh.h"
#include "Zing.h"
#include "ZingHw.h"
#include "AutoUsbToZing.h"
#include "ZingToAutoUsb.h"
#include "util.h"

void
CyFxAutoSetupGpio (
		void)
{
    CyU3PReturnStatus_t apiRetStatus = SetupGPIO();
    if (apiRetStatus != CY_U3P_SUCCESS)
    {
        CyU3PDebugPrint (4, "Setup GPIO failed, Error code = %d\n", apiRetStatus);
        CyFxAppErrorHandler(apiRetStatus);
    }
}

void
CyFxAutoI2cInit (
		void)
{
    CyU3PReturnStatus_t apiRetStatus = I2C_Init();
    if (apiRetStatus != CY_U3P_SUCCESS)
    {
        CyU3PDebugPrint (4, "I2C initialization failed, Error code = %d\n", apiRetStatus);
        CyFxAppErrorHandler(apiRetStatus);
    }
}

void
CyFxAutoPibInit (
		void)
{
    CyU3PReturnStatus_t apiRetStatus = PIB_Init();
    if (apiRetStatus != CY_U3P_SUCCESS)
    {
        CyU3PDebugPrint (4, "PIB initialization failed, Error code = %d\n", apiRetStatus);
        CyFxAppErrorHandler(apiRetStatus);
    }
}

void
CyFxSetDmaChannelCfg(
		CyU3PDmaChannelConfig_t *pDmaCfg,
		uint16_t size,
		uint16_t count,
		CyU3PDmaSocketId_t prodSckId,
		CyU3PDmaSocketId_t consSckId,
		uint32_t notification,
		CyU3PDmaCallback_t cb)
{
	pDmaCfg->size  = size;
	pDmaCfg->count = count;
	pDmaCfg->prodSckId = prodSckId;
	pDmaCfg->consSckId = consSckId;
	pDmaCfg->dmaMode = CY_U3P_DMA_MODE_BYTE;
	pDmaCfg->notification = notification;
	pDmaCfg->cb = cb;
	pDmaCfg->prodHeader = 0;
	pDmaCfg->prodFooter = 0;
	pDmaCfg->consHeader = 0;
	pDmaCfg->prodAvailCount = 0;
}

void
CyFxCreateChannel(
		uint16_t size,
		uint16_t count,
		CyU3PDmaSocketId_t prodSckId,
		CyU3PDmaSocketId_t consSckId,
		uint32_t notification,
		CyU3PDmaCallback_t cb,
		CyU3PDmaChannel *handle,
		CyU3PDmaType_t type)
{
	CyU3PDmaChannelConfig_t dmaCfg;
	CyU3PReturnStatus_t apiRetStatus = CY_U3P_SUCCESS;

	CyFxSetDmaChannelCfg(&dmaCfg, size, count, prodSckId, consSckId, notification, cb);
	apiRetStatus = CyU3PDmaChannelCreate(handle, type, &dmaCfg);
    if (apiRetStatus != CY_U3P_SUCCESS)
    {
        CyU3PDebugPrint (4, "CyU3PDmaChannelCreate failed, Error code = %d\n", apiRetStatus);
        CyFxAppErrorHandler(apiRetStatus);
    }

    /* Set DMA Channel transfer size to INFINITE */
    apiRetStatus = CyU3PDmaChannelSetXfer(handle, 0);
    if (apiRetStatus != CY_U3P_SUCCESS)
    {
        CyU3PDebugPrint (4, "CyU3PDmaChannelSetXfer failed, Error code = %d\n", apiRetStatus);
        CyFxAppErrorHandler(apiRetStatus);
    }
}

void
CyFxCreateCpuPibDmaChannels (
		uint16_t dataBurstLength)
{
    uint16_t size = 1024; // super speed <- assumed condition , temporary code
    Dma.Mode_ = DMA_SYNC;
	CyFxCreateChannel(size,
                      8,
                      CY_U3P_CPU_SOCKET_PROD,
                      CY_U3P_PIB_SOCKET_0,
                      CY_U3P_DMA_CB_PROD_EVENT,
                      0,
                      &Dma.ControlOut_.Channel_,
                      CY_U3P_DMA_TYPE_MANUAL_OUT);
	CyU3PDebugPrint(4,"[Auto] CPU-PIB ControlOut Channel created\n");

	CyFxCreateChannel(size,
                      8,
                      CY_U3P_PIB_SOCKET_1,
                      CY_U3P_CPU_SOCKET_CONS,
                      CY_U3P_DMA_CB_PROD_EVENT,
                      0,
                      &Dma.ControlIn_.Channel_,
                      CY_U3P_DMA_TYPE_MANUAL_IN);
	CyU3PDebugPrint(4,"[Auto] CPU-PIB ControlIn Channel created\n");

	CyFxCreateChannel(size*dataBurstLength,
                      4,
                      CY_U3P_CPU_SOCKET_PROD,
                      CY_U3P_PIB_SOCKET_2,
                      CY_U3P_DMA_CB_PROD_EVENT,
                      0,
                      &Dma.DataOut_.Channel_,
                      CY_U3P_DMA_TYPE_MANUAL_OUT);
	CyU3PDebugPrint(4,"[Auto] CPU-PIB DataOut Channel created\n");

	CyFxCreateChannel(size*dataBurstLength,
                      4,
                      CY_U3P_PIB_SOCKET_3,
                      CY_U3P_CPU_SOCKET_CONS,
                      CY_U3P_DMA_CB_PROD_EVENT,
                      0,
                      &Dma.DataIn_.Channel_,
                      CY_U3P_DMA_TYPE_MANUAL_IN);
	CyU3PDebugPrint(4,"[Auto] CPU-PIB DataIn Channel created\n");
}

void
CyFxCreateControlChannel (
        void)
{
    CyU3PReturnStatus_t apiRetStatus = ControlChThread_Create();
    if (apiRetStatus != CY_U3P_SUCCESS)
    {
        CyU3PDebugPrint (4, "Control Channel Thread Creation failed, Error code = %d\n", apiRetStatus);
        CyFxAppErrorHandler(apiRetStatus);
    }
}

void
CyFxZingInit (
		void)
{
    CyU3PReturnStatus_t apiRetStatus = Zing_Init();
    if (apiRetStatus != CY_U3P_SUCCESS)
    {
        CyU3PDebugPrint (4, "ZING initialization failed, Error code = %d\n", apiRetStatus);
        CyFxAppErrorHandler(apiRetStatus);
    }
}

void
CyFxSetHRCP (
		void)
{
#ifdef HRCP_PPC
	Zing_SetHRCP(PPC);
#elif HRCP_DEV
	Zing_SetHRCP(DEV);
#else
	Zing_SetHRCP(DEV);
#endif
}

void
CyFxCreateAutoUsbToZingThread (
        void)
{
    CyU3PReturnStatus_t apiRetStatus = CreateAutoUsbToZingThread();
    if (apiRetStatus != CY_U3P_SUCCESS)
    {
        CyU3PDebugPrint (4, "AutoUSB to Zing Thread Creation failed, Error code = %d\n", apiRetStatus);
        CyFxAppErrorHandler(apiRetStatus);
    }
}

void
CyFxCreateZingToAutoUsbThread (
        void)
{
    CyU3PReturnStatus_t apiRetStatus = CreateZingToAutoUsbThread();
    if (apiRetStatus != CY_U3P_SUCCESS)
    {
        CyU3PDebugPrint (4, "Zing to AutoUSB Thread Creation failed, Error code = %d\n", apiRetStatus);
        CyFxAppErrorHandler(apiRetStatus);
    }
}

void
CyFxUsbConnect (
		void)
{
	/* type-c connector
	 * Check the enumeration MuxControl_GPIO to High */

	CyU3PReturnStatus_t apiRetStatus = CyU3PConnectState(CyTrue, CyTrue);
    if (apiRetStatus != CY_U3P_SUCCESS)
    {
        CyU3PDebugPrint (4, "USB Connect failed, Error code = %d\n", apiRetStatus);
        CyFxAppErrorHandler(apiRetStatus);
    }

	CyU3PUSBSpeed_t usbSpeed;
	if((usbSpeed=CyU3PUsbGetSpeed()) != CY_U3P_SUPER_SPEED)
	{
		CyU3PDebugPrint (4, "CyU3PUsbGetSpeed = %d\n",usbSpeed);
		apiRetStatus = CyU3PConnectState(CyFalse, CyFalse);
	    if (apiRetStatus != CY_U3P_SUCCESS)
	    {
	        CyU3PDebugPrint (4, "USB Disconnect failed, Error code = %d\n", apiRetStatus);
	        CyFxAppErrorHandler(apiRetStatus);
	    }

		/* Check in other orientation */
	    apiRetStatus = CyU3PGpioSetValue(GPIO57, CyTrue);
	    if (apiRetStatus != CY_U3P_SUCCESS)
	    {
	        CyU3PDebugPrint (4, "GPIO Set Value failed, Error code = %d\n", apiRetStatus);
	        CyFxAppErrorHandler(apiRetStatus);
	    }

		apiRetStatus = CyU3PUsbControlUsb2Support (CyTrue);
	    if (apiRetStatus != CY_U3P_SUCCESS)
	    {
	        CyU3PDebugPrint (4, "Enable USB2.0 device operation failed, Error code = %d\n", apiRetStatus);
	        CyFxAppErrorHandler(apiRetStatus);
	    }

		apiRetStatus = CyU3PConnectState(CyTrue, CyTrue);
	    if (apiRetStatus != CY_U3P_SUCCESS)
	    {
	        CyU3PDebugPrint (4, "USB Connect failed, Error code = %d\n", apiRetStatus);
	        CyFxAppErrorHandler(apiRetStatus);
	    }
	}
}
