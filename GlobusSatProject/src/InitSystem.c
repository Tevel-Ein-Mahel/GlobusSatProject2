#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <freertos/task.h>
#include <hal/Drivers/I2C.h>
#include <hal/Drivers/SPI.h>
#include <hal/Timing/Time.h>
#include <at91/utility/exithandler.h>
#include <string.h>
#include "GlobalStandards.h"
#include "SubSystemModules/PowerManagement/EPS.h"
#include "SubSystemModules/Communication/TRXVU.h"
#include "SubSystemModules/Communication/SubsystemCommands/TRXVU_Commands.h"
#include "SubSystemModules/Maintenance/Maintenance.h"
#include "InitSystem.h"
#include "TLM_management.h"
#include <satellite-subsystems/IsisAntS.h>
#include <SubSystemModules/Housekepping/TelemetryCollector.h>

#ifdef GOMEPS
	#include <satellite-subsystems/GomEPS.h>
#endif
#ifdef ISISEPS
	#include <satellite-subsystems/imepsv2_piu.h>
#endif
#define I2c_SPEED_Hz 100000
#define I2c_Timeout 10
#define I2c_TimeoutTest portMAX_DELAY

int StartFRAM()
{
	return logError(FRAM_start() ,"StartFRAM");
}

int StartI2C()
{
	return logError(I2C_start(I2c_SPEED_Hz , I2c_Timeout) ,"StartI2C");
}

int StartSPI()
{
	return logError(SPI_start(bus1_spi , slave1_spi) ,"SPI_start");
}

int StartTIME()
{
	int error = 0;
	Time expected_deploy_time = UNIX_DATE_JAN_D1_Y2000;
	error = Time_start(&expected_deploy_time, 0);
	if (0 != error) {
		return logError(error ,"StartTIME-Time_start");
	}
	// TODO: udpate to sat time that we had before the restart in FRAM
	return 0;
}

int InitSubsystems()
{
	StartI2C();

	StartSPI();

	StartFRAM();

	StartTIME();

	InitializeFS(TRUE); //TODO: change true to the first activation

	InitSavePeriodTimes();

	EPS_Init();

	InitTrxvu();

	vTaskDelay(1000); // rest a little before we start working

	return 0;
}
