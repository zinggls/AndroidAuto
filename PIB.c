#include "PIB.h"
#include "cyu3pib.h"
#include "cyu3error.h"
#include "cyu3gpif.h"
#include "macro.h"

#if ZING_BUG_WM3 == 0

#if ZING_GPIF_BUSWIDTH == 32
	#include "gpif/GpifZing32bit.h"
#elif ZING_GPIF_BUSWIDTH == 16
	#include "gpif/GpifZing16bit.h"
#elif ZING_GPIF_BUSWIDTH == 8
	#include "gpif/GpifZing8bit.h"
#endif

#else

#if ZING_GPIF_BUSWIDTH == 32
	#include "gpif/GpifZing32bit_NoZLP.h"
#elif ZING_GPIF_BUSWIDTH == 16
	#include "gpif/GpifZing16bit_NoZLP.h"
#elif ZING_GPIF_BUSWIDTH == 8
	#include "gpif/GpifZing8bit_NoZLP.h"
#endif

#endif

CyU3PReturnStatus_t PIB_Init(void)
{
	CyU3PPibClock_t pibClock;

   /* Initialize the p-port block. */
   pibClock.clkDiv = 2;             /* in AN87216, value is 2 */
   pibClock.clkSrc = CY_U3P_SYS_CLK;
   pibClock.isHalfDiv = CyFalse;
   /* Disable DLL for sync GPIF */
   pibClock.isDllEnable = CyFalse;

   CHECK(CyU3PPibInit(CyTrue, &pibClock));

   /* Load the GPIF configuration for Master mode. */
   CHECK(CyU3PGpifLoad (&CyFxGpifConfig));

   CyU3PGpifSocketConfigure (0,CY_U3P_PIB_SOCKET_0,4,CyFalse,1);
   CyU3PGpifSocketConfigure (1,CY_U3P_PIB_SOCKET_1,4,CyFalse,1);
   CyU3PGpifSocketConfigure (2,CY_U3P_PIB_SOCKET_2,4,CyFalse,1);
   CyU3PGpifSocketConfigure (3,CY_U3P_PIB_SOCKET_3,4,CyFalse,1);

   //CyU3PGpifRegisterCallback(CyFxBulkLpApplnGPIFEventCB);	//	refer to AN87216

   /* Start the state machine. */
   CHECK(CyU3PGpifSMStart (START, ALPHA_START));
   return CY_U3P_SUCCESS;
}
