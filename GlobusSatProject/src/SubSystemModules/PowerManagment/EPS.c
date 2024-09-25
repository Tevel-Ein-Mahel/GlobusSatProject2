#include "EPS.h"
#include <satellite-subsystems/GomEPS.h>

/*
* EPS.c
 *
 *  Created on: 18 באפר 2024
 *      Author: hp
 */

Boolean GomEPSInit(void);

int EPS_Init()
{
	Boolean b = GomEPSInit();
	return b;
}

int EPS_Conditioning()
{
	// TODO: get code from Demo code
}

int GetBatteryVoltage(voltage_t *vbat)
{
	gom_eps_hk_t response;
}

voltage_t GetFiltered_Voltage()
{
}

Boolean GomEPSInit(void)
{
    unsigned char i2c_address = 0x02;
    int rv;

	rv = GomEpsInitialize(&i2c_address, 1);
	if(rv != E_NO_SS_ERR && rv != E_IS_INITIALIZED)
	{
		// we have a problem. Indicate the error. But we'll gracefully exit to the higher menu instead of
		// hanging the code
		TRACE_ERROR("\n\r GomEpsInitialize() failed; err=%d! Exiting ... \n\r", rv);
		return FALSE;
	}

	return TRUE;
}
