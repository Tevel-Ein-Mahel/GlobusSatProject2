#include <hal/Storage/FRAM.h>
#include <hal/errors.h>
#include <hal/Drivers/I2C.h>
#include <hal/Timing/Time.h>
#include "GlobalStandards.h"

// defaults for I2C
#define SPEED_i2cBusSpeed_Hz 100000
#define TIMEOUT_i2cTransferTimeout 10

// defaults for time
#define TIME_year 1970
#define TIME 1767656734


int StartFRAM()
{
	int flag = FRAM_start(); // also starts SPI
	return flag;
}

int StartI2C()
{
	int flag = I2C_start(SPEED_i2cBusSpeed_Hz, TIMEOUT_i2cTransferTimeout);
	return flag;
}

int StartTIME()
{
	const Time t1 = UNIX_DATE_JAN_D1_Y2000;
	int flag = Time_start(&t1, 0);
	return flag;
}

int InitSubsystems()
{
	int flag;
	flag = StartI2C();
	if (flag != E_NO_SS_ERR)
	{
		return 1;
	}
	flag = StartFRAM();
	if (flag != E_NO_SS_ERR)
	{
		return 1;
	}
	flag = StartTIME();
	if (flag != E_NO_SS_ERR)
	{
		return 1;
	}
	flag = InitializeFS();
	if (flag != E_NO_SS_ERR)
	{
		return 1;
	}
	flag = EPS_Init();
	if (flag != E_NO_SS_ERR)
	{
		return 1;
	}

	// TODO: TRX, Payload (what kind of init to payload?)
	return 0;
}



// TODO: check if SPI need to be init

int AddToLog(int e, const char* message)
{
     printf(message);
     return e;
}

/*
 * if (flag != E_NO_SS_ERR)
	{
		// TODO: telemetry log error
			// TODO: after filesystem log created
		printf// print_error(flag);
	}
	return AddToLog(flag, "start fram");
	// TODO: return value as error*/
