#ifndef __ZING_H__
#define __ZING_H__

#include "cyu3types.h"
#include "cyu3dma.h"

#define ZING_RF_SERDES_PATH		(1)		//0 : serdes path, 1: rf path

CyU3PReturnStatus_t Zing_Init(void);
CyU3PReturnStatus_t Zing_RegWrite(uint16_t addr, uint8_t* buf, uint16_t len);
CyU3PReturnStatus_t Zing_RegRead(uint16_t addr, uint8_t* buf, uint16_t len);
CyU3PReturnStatus_t Zing_AFC2(float f_tg);
uint32_t Zing_GetHRCP();
CyU3PReturnStatus_t Zing_SetHRCP(uint32_t val);
CyU3PReturnStatus_t Zing_SetPath(uint32_t val);
CyU3PReturnStatus_t Zing_DataWrite(uint8_t* buf, uint32_t len);
CyU3PReturnStatus_t Zing_DataRead(uint8_t* buf, uint32_t* len_pt);
CyU3PReturnStatus_t Zing_Reset(uint8_t type);
CyU3PReturnStatus_t Zing_AutoHRCP(void);
CyU3PReturnStatus_t Zing_GetVersion(uint8_t *version);
void Zing_Test_DataTx2 (uint32_t repeat, uint32_t length, uint32_t pattern);
void Zing_Test_DataSink2 (uint32_t cnt, uint32_t timeout);
CyU3PReturnStatus_t Zing_Transfer_Send(CyU3PDmaChannel* dma_ch,uint8_t *data,uint32_t length);
CyU3PReturnStatus_t Zing_Transfer_Recv(CyU3PDmaChannel *dma_ch,uint8_t *data,uint32_t *length_pt,uint32_t waitOption);
CyU3PReturnStatus_t Zing_Management_Send(uint8_t *data,uint32_t length);
CyU3PReturnStatus_t Zing_Golay(uint32_t val);

void Zing_Header(
        uint8_t *pt,            /* pt : buffer pointer */
        uint16_t payload_size,  /* payload_size : memory size in bytes */
        uint16_t addr,          /* addr : zing internal address , address unit = 32bit */
        uint16_t type           /* type : Read/write */);
void Zing_Header2(
        uint8_t *pt,            /* pt : buffer pointer */
        uint16_t dir,
        uint16_t interrupt,
        uint16_t target,
        uint16_t type,
        uint16_t req_resp,
        uint16_t fr_type,
        uint16_t intr_flags,
        uint16_t addr,
        uint16_t payload_size);

#endif
