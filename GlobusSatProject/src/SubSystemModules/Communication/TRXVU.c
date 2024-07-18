#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <freertos/task.h>

#include <hal/Timing/Time.h>
#include <hal/errors.h>

#include <satellite-subsystems/IsisTRXVU.h>
#include <satellite-subsystems/IsisAntS.h>

#include <stdlib.h>
#include <string.h>

#include <utils.h>
#include "GlobalStandards.h"
#include "TRXVU.h"
#include "AckHandler.h"
#include "SubsystemCommands/TRXVU_Commands.h"
#include "TLM_management.h"

#include "SubSystemModules/PowerManagement/EPS.h"
#include "SubSystemModules/Maintenance/Maintenance.h"
#include "SubSystemModules/Housekepping/TelemetryCollector.h"

#ifdef TESTING_TRXVU_FRAME_LENGTH
#include <hal/Utility/util.h>
#endif

time_unix 		g_idle_end_time = 1;				// time at which the idel will end

xQueueHandle xDumpQueue = NULL;
xSemaphoreHandle xDumpLock = NULL;
xSemaphoreHandle xIsTransmitting = NULL; // mutex on transmission.
xTaskHandle xDumpHandle = NULL;

time_unix g_prev_beacon_time = 0;				// the time at which the previous beacon occured
time_unix g_beacon_interval_time = 0;			// seconds between each beacon

void InitBeaconParams()
{

	if (0 != FRAM_read((unsigned char*) &g_beacon_interval_time,BEACON_INTERVAL_TIME_ADDR,BEACON_INTERVAL_TIME_SIZE)){
		g_beacon_interval_time = DEFAULT_BEACON_INTERVAL_TIME;
	}
}

void InitTxModule()
{
	if(xIsTransmitting == NULL)
		vSemaphoreCreateBinary(xIsTransmitting);
}

void InitSemaphores()
{

	if(xDumpLock == NULL)
		vSemaphoreCreateBinary(xDumpLock);
	if(xDumpQueue == NULL)
		xDumpQueue = xQueueCreate(1, sizeof(Boolean));
}


int InitTrxvu() {
	ISIStrxvuI2CAddress myTRXVUAddress;
	ISIStrxvuFrameLengths myTRXVUFramesLenght;


	//Buffer definition
	myTRXVUFramesLenght.maxAX25frameLengthTX = SIZE_TXFRAME;//SIZE_TXFRAME;
	myTRXVUFramesLenght.maxAX25frameLengthRX = SIZE_RXFRAME;

	//I2C addresses defined
	myTRXVUAddress.addressVu_rc = I2C_TRXVU_RC_ADDR;
	myTRXVUAddress.addressVu_tc = I2C_TRXVU_TC_ADDR;


	//Bitrate definition
	ISIStrxvuBitrate myTRXVUBitrates;
	myTRXVUBitrates = trxvu_bitrate_9600;
	if (logError(IsisTrxvu_initialize(&myTRXVUAddress, &myTRXVUFramesLenght,&myTRXVUBitrates, 1) ,"InitTrxvu-IsisTrxvu_initialize") ) return -1;

	vTaskDelay(1000); // wait a little

	ISISantsI2Caddress myAntennaAddress;
	myAntennaAddress.addressSideA = ANTS_I2C_SIDE_A_ADDR;
	myAntennaAddress.addressSideB = ANTS_I2C_SIDE_B_ADDR;

	//Initialize the AntS system
	if (logError(IsisAntS_initialize(&myAntennaAddress, 1),"InitTrxvu-IsisAntS_initialize")) return -1;

	InitTxModule();
	InitBeaconParams();
	InitSemaphores();
	//checkTransponderStart();// lets see if we need to turn on the transponder


	return 0;
}


////////////////////////////// SEMINAR 2 ////////////////////////////

int TRX_Logic(){
	int frames = GetNumberOfFramesInBuffer();
	sat_packet_t *cmd = {0};
	if (frames > 0){
		GetOnlineCommand(cmd);
		ActUponCommand(cmd);
	}
}

int GetNumberOfFramesInBuffer(){
	unsigned short RxCounter = 0;
	logError(IsisTrxvu_rcGetFrameCount(0, &RxCounter), "IsisTrxvu_rcGetFrameCount err");
	return RxCounter;
}

int GetOnlineCommand(sat_packet_t *cmd){
	if (cmd == NULL)
		return -1;

	unsigned char rxframebuffer[SIZE_RXFRAME] = {0};
	ISIStrxvuRxFrame rxFrameCmd = {0,0,0, rxframebuffer};
	logError(IsisTrxvu_rcGetCommandFrame(0, &rxFrameCmd),"IsisTrxvu_rcGetCommandFrame err");
	int err = ParseDataToCommand(rxFrameCmd.rx_framedata, cmd);
	return err;
}
