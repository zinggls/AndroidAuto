#include "cyu3error.h"
#include "cyu3system.h"
#include "setup.h"
#include "gpio.h"
#include "i2c.h"
#include "PIB.h"
#include "dma.h"
#include "ControlCh.h"
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
