/**
@file
@license   $License$
@copyright $Copyright$
@version   $Revision$
@purpose   SCPI ETSI TEST parser
@brief     SCPI ETSI TEST parser
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>
#include "scpi_etsi_test.h"
#include "scpi.h"
#include "scpi_etsi_test_user.h"

enum {
	UINT8_BUFF_SIZE = 4,
	INT8_BUFF_SIZE = 5,
	UINT16_BUFF_SIZE = 6,
	INT16_BUFF_SIZE = 7,
	UINT32_BUFF_SIZE = 11,
	INT32_BUFF_SIZE = 12,
};

#define SCPI_READ_TERMINATION_CHARACTER '\n'

#ifndef SCPI_COMMAND_BUFFER_LENGTH
#define	SCPI_COMMAND_BUFFER_LENGTH 256
#endif

#ifndef SCPI_INPUT_BUFFER_LENGTH
#define SCPI_INPUT_BUFFER_LENGTH 256
#endif

#ifndef SCPI_ERROR_QUEUE_SIZE
#define SCPI_ERROR_QUEUE_SIZE 17
#endif


scpi_result_t SCPI_ETSI_TEST_GetIDN(scpi_t* context);
scpi_result_t SCPI_ETSI_TEST_Reset(scpi_t* context);
scpi_result_t SCPI_ETSI_TEST_GetPhyCount(scpi_t* context);
scpi_result_t SCPI_ETSI_TEST_GetPhyCapabilities(scpi_t* context);
scpi_result_t SCPI_ETSI_TEST_GetLowestFrequency(scpi_t* context);
scpi_result_t SCPI_ETSI_TEST_GetHighestFrequency(scpi_t* context);
scpi_result_t SCPI_ETSI_TEST_GetChannelCount(scpi_t* context);
scpi_result_t SCPI_ETSI_TEST_GetChannelBandwidth(scpi_t* context);
scpi_result_t SCPI_ETSI_TEST_GetBaudrate(scpi_t* context);
scpi_result_t SCPI_ETSI_TEST_GetLowestPower(scpi_t* context);
scpi_result_t SCPI_ETSI_TEST_GetHighestPower(scpi_t* context);
scpi_result_t SCPI_ETSI_TEST_GetMinPacketLength(scpi_t* context);
scpi_result_t SCPI_ETSI_TEST_GetMaxPacketLength(scpi_t* context);
scpi_result_t SCPI_ETSI_TEST_GetModulationType(scpi_t* context);
scpi_result_t SCPI_ETSI_TEST_GetSupportedSignals(scpi_t* context);
scpi_result_t SCPI_ETSI_TEST_GetAntennaCount(scpi_t* context);
scpi_result_t SCPI_ETSI_TEST_GetPhyDescription(scpi_t* context);
scpi_result_t SCPI_ETSI_TEST_GetChannelList(scpi_t* context);
scpi_result_t SCPI_ETSI_TEST_GetChannel(scpi_t* context);
scpi_result_t SCPI_ETSI_TEST_GetSettings(scpi_t* context);
scpi_result_t SCPI_ETSI_TEST_SetPhy(scpi_t* context);
scpi_result_t SCPI_ETSI_TEST_GetSelectedPhy(scpi_t* context);
scpi_result_t SCPI_ETSI_TEST_SetChannel(scpi_t* context);
scpi_result_t SCPI_ETSI_TEST_GetSelectedChannel(scpi_t* context);
scpi_result_t SCPI_ETSI_TEST_SetSignal(scpi_t* context);
scpi_result_t SCPI_ETSI_TEST_GetSelectedSignal(scpi_t* context);
scpi_result_t SCPI_ETSI_TEST_SetPower(scpi_t* context);
scpi_result_t SCPI_ETSI_TEST_GetSelectedPower(scpi_t* context);
scpi_result_t SCPI_ETSI_TEST_SetAntenna(scpi_t* context);
scpi_result_t SCPI_ETSI_TEST_GetSelectedAntenna(scpi_t* context);
scpi_result_t SCPI_ETSI_TEST_SetPERTotalPackets(scpi_t* context);
scpi_result_t SCPI_ETSI_TEST_GetSelectedPERTotalPackets(scpi_t* context);
scpi_result_t SCPI_ETSI_TEST_SetPERPacketLength(scpi_t* context);
scpi_result_t SCPI_ETSI_TEST_GetSelectedPERPacketLength(scpi_t* context);


static scpi_error_t scpiErrorBuffer[SCPI_ERROR_QUEUE_SIZE];
static char scpiInputBuffer[SCPI_INPUT_BUFFER_LENGTH];
static char commandBuffer[SCPI_COMMAND_BUFFER_LENGTH];

static const scpi_command_t scpiCommands[] = {
										{ .pattern = "*IDN?", 							.callback = SCPI_ETSI_TEST_GetIDN, },
										{ .pattern = "*RST",							.callback = SCPI_ETSI_TEST_Reset, },
										{ .pattern = "PHY?",							.callback = SCPI_ETSI_TEST_GetPhyCount, },
										{ .pattern = "PHY#:CAPabilities?",				.callback = SCPI_ETSI_TEST_GetPhyCapabilities, },
										{ .pattern = "PHY#:CAPabilities:FREQLow?",		.callback = SCPI_ETSI_TEST_GetLowestFrequency, },
										{ .pattern = "PHY#:CAPabilities:FREQHigh?",		.callback = SCPI_ETSI_TEST_GetHighestFrequency, },
										{ .pattern = "PHY#:CAPabilities:CHANCount?", 	.callback = SCPI_ETSI_TEST_GetChannelCount, },
										{ .pattern = "PHY#:CAPabilities:CHANBandwidth?", .callback = SCPI_ETSI_TEST_GetChannelBandwidth, },
										{ .pattern = "PHY#:CAPabilities:BAUDrate?", 	.callback = SCPI_ETSI_TEST_GetBaudrate, },
										{ .pattern = "PHY#:CAPabilities:POWLow?", 		.callback = SCPI_ETSI_TEST_GetLowestPower, },
										{ .pattern = "PHY#:CAPabilities:POWHigh?", 		.callback = SCPI_ETSI_TEST_GetHighestPower, },
										{ .pattern = "PHY#:CAPabilities:PCKTLENMIN?", 	.callback = SCPI_ETSI_TEST_GetMinPacketLength, },
										{ .pattern = "PHY#:CAPabilities:PCKTLENMAX?", 	.callback = SCPI_ETSI_TEST_GetMaxPacketLength, },
										{ .pattern = "PHY#:CAPabilities:MODType?", 		.callback = SCPI_ETSI_TEST_GetModulationType, },
										{ .pattern = "PHY#:CAPabilities:SIGnals?", 		.callback = SCPI_ETSI_TEST_GetSupportedSignals, },
										{ .pattern = "PHY#:CAPabilities:ANTenna?", 		.callback = SCPI_ETSI_TEST_GetAntennaCount, },
										{ .pattern = "PHY#:DESCription?",				.callback = SCPI_ETSI_TEST_GetPhyDescription, },
										{ .pattern = "PHY#:CHANList?",					.callback = SCPI_ETSI_TEST_GetChannelList, },
										{ .pattern = "PHY#:CHANnel#?",					.callback = SCPI_ETSI_TEST_GetChannel, },
										{ .pattern = "SETtings?", 						.callback = SCPI_ETSI_TEST_GetSettings, },
										{ .pattern = "SETtings:PHY",					.callback = SCPI_ETSI_TEST_SetPhy, },
										{ .pattern = "SETtings:PHY?",					.callback = SCPI_ETSI_TEST_GetSelectedPhy, },
										{ .pattern = "SETtings:CHANnel",				.callback = SCPI_ETSI_TEST_SetChannel, },
										{ .pattern = "SETtings:CHANnel?",				.callback = SCPI_ETSI_TEST_GetSelectedChannel, },
										{ .pattern = "SETtings:SIGnal",					.callback = SCPI_ETSI_TEST_SetSignal, },
										{ .pattern = "SETtings:SIGnal?",				.callback = SCPI_ETSI_TEST_GetSelectedSignal, },
										{ .pattern = "SETtings:POWer",					.callback = SCPI_ETSI_TEST_SetPower, },
										{ .pattern = "SETtings:POWer?",					.callback = SCPI_ETSI_TEST_GetSelectedPower, },
										{ .pattern = "SETtings:ANTenna",				.callback = SCPI_ETSI_TEST_SetAntenna, },
										{ .pattern = "SETtings:ANTenna?",				.callback = SCPI_ETSI_TEST_GetSelectedAntenna, },
										{ .pattern = "SETtings:PER:TOTALpackets",		.callback = SCPI_ETSI_TEST_SetPERTotalPackets, },
										{ .pattern = "SETtings:PER:TOTALpackets?",		.callback = SCPI_ETSI_TEST_GetSelectedPERTotalPackets, },
										{ .pattern = "SETtings:PER:PCKTLENgth",			.callback = SCPI_ETSI_TEST_SetPERPacketLength, },
										{ .pattern = "SETtings:PER:PCKTLENgth?",		.callback = SCPI_ETSI_TEST_GetSelectedPERPacketLength, },
										SCPI_CMD_LIST_END };

static scpi_interface_t scpiInterface = {   .write = NULL,
											.error = NULL,
											.reset = NULL,
											.flush = NULL, };

static SCPI_ETSI_TEST_DeviceDescriptor deviceDesc;
static scpi_t scpiContext;

SCPIResult SCPI_ETSI_TEST_Init(void){
	if(NULL != scpiInputBuffer){
		if(NULL != scpiErrorBuffer){
			SCPI_Init(&scpiContext, scpiCommands, &scpiInterface, scpi_units_def, NULL, NULL, NULL, NULL, scpiInputBuffer,
					SCPI_INPUT_BUFFER_LENGTH, scpiErrorBuffer, SCPI_ERROR_QUEUE_SIZE);
			SCPI_ETSI_TEST_USER_Init(&deviceDesc);
			return SCPI_OK;
		}
	}
	return SCPI_ERROR;
}

SCPIResult SCPI_ETSI_TEST_Proc(void){
	SCPIResult result = SCPI_ERROR;
	uint8_t byte = 0;
	if(NULL != commandBuffer){
		do{
			if(true == SCPI_ETSI_TEST_USER_GetChar(&commandBuffer[byte])){
				if(commandBuffer[byte] != 0){
					if(SCPI_READ_TERMINATION_CHARACTER == commandBuffer[byte]){
						SCPI_Input(&scpiContext, commandBuffer, byte+1);
						result = SCPI_OK;
						break;
					}
					byte++;
				}
			}
		} while(byte < SCPI_COMMAND_BUFFER_LENGTH && byte > 0);
	}
	return result;
}

void SCPI_ETSI_TEST_Send(scpi_t* context, const void* data, size_t size){
	if(NULL != context) {
		if(NULL != data) {
			for(int chunk=0; chunk<size; chunk++){
				const char* wordStart = data;
				SCPI_ETSI_TEST_USER_PutChar(*(wordStart+chunk));
			}
		}
	}
}

scpi_result_t SCPI_ETSI_TEST_GetIDN(scpi_t* context){
	char buffer[200];
	if(NULL != context) {
		const char* description = deviceDesc.idn;
		if(NULL != description){
			snprintf(buffer, sizeof(buffer), "%s\n", description);
			SCPI_ETSI_TEST_Send(context, buffer, strlen(buffer));
			return SCPI_RES_OK;
		}
	}
	SCPI_ETSI_TEST_Send(context, "ERR\n", 4);
	return SCPI_RES_ERR;
}

scpi_result_t SCPI_ETSI_TEST_Reset(scpi_t* context){
	if(NULL != context) {
		SCPI_ETSI_TEST_Send(context, "OK\n", 3);
		SCPI_ETSI_TEST_USER_Reset();
		return SCPI_RES_OK;
	}
	SCPI_ETSI_TEST_Send(context, "ERR\n", 4);
	return SCPI_RES_ERR;
}

scpi_result_t SCPI_ETSI_TEST_GetPhyCount(scpi_t* context){
	if(NULL != context) {
		char buffer[UINT8_BUFF_SIZE];
		const uint8_t phyCount = deviceDesc.phyCount;
		snprintf(buffer, sizeof(buffer), "%u\n", phyCount);
		SCPI_ETSI_TEST_Send(context, buffer, strlen(buffer));
		return SCPI_RES_OK;
	}
	SCPI_ETSI_TEST_Send(context, "ERR\n", 4);
	return SCPI_RES_ERR;
}

scpi_result_t SCPI_ETSI_TEST_GetPhyCapabilities(scpi_t* context){
	if(NULL != context) {
		char buffer[256];
		int32_t phy;
		SCPI_CommandNumbers(context, &phy, 1, 0);
		if(phy <= deviceDesc.phyCount-1){
			snprintf(buffer, sizeof(buffer), "%"PRIu32",%"PRIu32",%"PRIu16",%"PRIu32",%"PRIu32",%i,%i,%i,%"PRIu16",%"PRIu16",%"PRIu16",%u,%u,%u\n", deviceDesc.phyCap[phy].lowestFrequency,
					deviceDesc.phyCap[phy].highestFrequency, deviceDesc.phyCap[phy].channelCount, deviceDesc.phyCap[phy].channelBandwidth, deviceDesc.phyCap[phy].baudrate,
					deviceDesc.phyCap[phy].lowestPower, deviceDesc.phyCap[phy].highestPower, deviceDesc.phyCap[phy].defaultPower, deviceDesc.phyCap[phy].minimalPacketLength,
					deviceDesc.phyCap[phy].maximalPacketLength, deviceDesc.phyCap[phy].defaultPERPacketLength, deviceDesc.phyCap[phy].modulationType, deviceDesc.phyCap[phy].supportedSignals,
					deviceDesc.phyCap[phy].antennaCount);
			SCPI_ETSI_TEST_Send(context, buffer, strlen(buffer));
			return SCPI_RES_OK;
		}
	}
	SCPI_ETSI_TEST_Send(context, "ERR\n", 4);
	return SCPI_RES_ERR;
}

scpi_result_t SCPI_ETSI_TEST_GetLowestFrequency(scpi_t* context){
	if(NULL != context) {
		char buffer[UINT32_BUFF_SIZE];
		int32_t phy;
		SCPI_CommandNumbers(context, &phy, 1, 0);
		if(phy <= deviceDesc.phyCount-1){
			const uint32_t freq = deviceDesc.phyCap[phy].lowestFrequency;
			snprintf(buffer, sizeof(buffer), "%"PRIu32"\n", freq);
			SCPI_ETSI_TEST_Send(context, buffer, strlen(buffer));
			return SCPI_RES_OK;
		}
	}
	SCPI_ETSI_TEST_Send(context, "ERR\n", 4);
	return SCPI_RES_ERR;
}

scpi_result_t SCPI_ETSI_TEST_GetHighestFrequency(scpi_t* context){
	if(NULL != context) {
		char buffer[UINT32_BUFF_SIZE];
		int32_t phy;
		SCPI_CommandNumbers(context, &phy, 1, 0);
		if(phy <= deviceDesc.phyCount-1){
			const uint32_t freq = deviceDesc.phyCap[phy].highestFrequency;
			snprintf(buffer, sizeof(buffer), "%"PRIu32"\n", freq);
			SCPI_ETSI_TEST_Send(context, buffer, strlen(buffer));
			return SCPI_RES_OK;
		}
	}
	SCPI_ETSI_TEST_Send(context, "ERR\n", 4);
	return SCPI_RES_ERR;
}

scpi_result_t SCPI_ETSI_TEST_GetChannelCount(scpi_t* context){
	if(NULL != context) {
		char buffer[UINT16_BUFF_SIZE];
		int32_t phy;
		SCPI_CommandNumbers(context, &phy, 1, 0);
		if(phy <= deviceDesc.phyCount-1){
			const uint16_t channelCount = deviceDesc.phyCap[phy].channelCount;
			snprintf(buffer, sizeof(buffer), "%"PRIu16"\n", channelCount);
			SCPI_ETSI_TEST_Send(context, buffer, strlen(buffer));
			return SCPI_RES_OK;
		}
	}
	SCPI_ETSI_TEST_Send(context, "ERR\n", 4);
	return SCPI_RES_ERR;
}

scpi_result_t SCPI_ETSI_TEST_GetChannelBandwidth(scpi_t* context){
	if(NULL != context) {
		char buffer[UINT32_BUFF_SIZE];
		int32_t phy;
		SCPI_CommandNumbers(context, &phy, 1, 0);
		if(phy <= deviceDesc.phyCount-1){
			const uint32_t channelBandwidth = deviceDesc.phyCap[phy].channelBandwidth;
			snprintf(buffer, sizeof(buffer), "%"PRIu32"\n", channelBandwidth);
			SCPI_ETSI_TEST_Send(context, buffer, strlen(buffer));
			return SCPI_RES_OK;
		}
	}
	SCPI_ETSI_TEST_Send(context, "ERR\n", 4);
	return SCPI_RES_ERR;
}

scpi_result_t SCPI_ETSI_TEST_GetBaudrate(scpi_t* context){
	if(NULL != context) {
		char buffer[UINT32_BUFF_SIZE];
		int32_t phy;
		SCPI_CommandNumbers(context, &phy, 1, 0);
		if(phy <= deviceDesc.phyCount-1){
			const uint32_t baudrate = deviceDesc.phyCap[phy].baudrate;
			snprintf(buffer, sizeof(buffer), "%"PRIu32"\n", baudrate);
			SCPI_ETSI_TEST_Send(context, buffer, strlen(buffer));
			return SCPI_RES_OK;
		}
	}
	SCPI_ETSI_TEST_Send(context, "ERR\n", 4);
	return SCPI_RES_ERR;
}

scpi_result_t SCPI_ETSI_TEST_GetLowestPower(scpi_t* context){
	if(NULL != context) {
		char buffer[INT8_BUFF_SIZE];
		int32_t phy;
		SCPI_CommandNumbers(context, &phy, 1, 0);
		if(phy <= deviceDesc.phyCount-1){
			const int8_t power = deviceDesc.phyCap[phy].lowestPower;
			snprintf(buffer, sizeof(buffer), "%i\n", power);
			SCPI_ETSI_TEST_Send(context, buffer, strlen(buffer));
			return SCPI_RES_OK;
		}
	}
	SCPI_ETSI_TEST_Send(context, "ERR\n", 4);
	return SCPI_RES_ERR;
}

scpi_result_t SCPI_ETSI_TEST_GetHighestPower(scpi_t* context){
	if(NULL != context) {
		char buffer[INT8_BUFF_SIZE];
		int32_t phy;
		SCPI_CommandNumbers(context, &phy, 1, 0);
		if(phy <= deviceDesc.phyCount-1){
			const int8_t power = deviceDesc.phyCap[phy].highestPower;
			snprintf(buffer, sizeof(buffer), "%i\n", power);
			SCPI_ETSI_TEST_Send(context, buffer, strlen(buffer));
			return SCPI_RES_OK;
		}
	}
	SCPI_ETSI_TEST_Send(context, "ERR\n", 4);
	return SCPI_RES_ERR;
}

scpi_result_t SCPI_ETSI_TEST_GetMinPacketLength(scpi_t* context){
	if(NULL != context) {
		char buffer[UINT16_BUFF_SIZE];
		int32_t phy;
		SCPI_CommandNumbers(context, &phy, 1, 0);
		if(phy <= deviceDesc.phyCount-1){
			const uint16_t pktLen = deviceDesc.phyCap[phy].minimalPacketLength;
			snprintf(buffer, sizeof(buffer), "%"PRIu16"\n", pktLen);
			SCPI_ETSI_TEST_Send(context, buffer, strlen(buffer));
			return SCPI_RES_OK;
		}
	}
	SCPI_ETSI_TEST_Send(context, "ERR\n", 4);
	return SCPI_RES_ERR;
}

scpi_result_t SCPI_ETSI_TEST_GetMaxPacketLength(scpi_t* context){
	if(NULL != context) {
		char buffer[UINT16_BUFF_SIZE];
		int32_t phy;
		SCPI_CommandNumbers(context, &phy, 1, 0);
		if(phy <= deviceDesc.phyCount-1){
			const uint16_t pktLen = deviceDesc.phyCap[phy].maximalPacketLength;
			snprintf(buffer, sizeof(buffer), "%"PRIu16"\n", pktLen);
			SCPI_ETSI_TEST_Send(context, buffer, strlen(buffer));
			return SCPI_RES_OK;
		}
	}
	SCPI_ETSI_TEST_Send(context, "ERR\n", 4);
	return SCPI_RES_ERR;
}

scpi_result_t SCPI_ETSI_TEST_GetModulationType(scpi_t* context){
	if(NULL != context) {
		char buffer[UINT8_BUFF_SIZE];
		int32_t phy;
		SCPI_CommandNumbers(context, &phy, 1, 0);
		if(phy <= deviceDesc.phyCount-1){
			const uint8_t mod = deviceDesc.phyCap[phy].modulationType;
			snprintf(buffer, sizeof(buffer), "%u\n", mod);
			SCPI_ETSI_TEST_Send(context, buffer, strlen(buffer));
			return SCPI_RES_OK;
		}
	}
	SCPI_ETSI_TEST_Send(context, "ERR\n", 4);
	return SCPI_RES_ERR;
}

scpi_result_t SCPI_ETSI_TEST_GetSupportedSignals(scpi_t* context){
	if(NULL != context) {
		char buffer[UINT8_BUFF_SIZE];
		int32_t phy;
		SCPI_CommandNumbers(context, &phy, 1, 0);
		if(phy <= deviceDesc.phyCount-1){
			const uint8_t signals = deviceDesc.phyCap[phy].supportedSignals;
			snprintf(buffer, sizeof(buffer), "%u\n", signals);
			SCPI_ETSI_TEST_Send(context, buffer, strlen(buffer));
			return SCPI_RES_OK;
		}
	}
	SCPI_ETSI_TEST_Send(context, "ERR\n", 4);
	return SCPI_RES_ERR;
}

scpi_result_t SCPI_ETSI_TEST_GetAntennaCount(scpi_t* context){
	if(NULL != context) {
		char buffer[UINT8_BUFF_SIZE];
		int32_t phy;
		SCPI_CommandNumbers(context, &phy, 1, 0);
		if(phy <= deviceDesc.phyCount-1){
			const uint8_t antennaCount = deviceDesc.phyCap[phy].antennaCount;
			snprintf(buffer, sizeof(buffer), "%u\n", antennaCount);
			SCPI_ETSI_TEST_Send(context, buffer, strlen(buffer));
			return SCPI_RES_OK;
		}
	}
	SCPI_ETSI_TEST_Send(context, "ERR\n", 4);
	return SCPI_RES_ERR;
}

scpi_result_t SCPI_ETSI_TEST_GetPhyDescription(scpi_t* context){
	if(NULL != context) {
		char buffer[100];
		int32_t phy;
		SCPI_CommandNumbers(context, &phy, 1, 0);
		if(phy <= deviceDesc.phyCount-1){
			const char* description = *(deviceDesc.phyDescriptions+phy);
			if(NULL != description){
				snprintf(buffer, sizeof(buffer), "%s\n", description);
				SCPI_ETSI_TEST_Send(context, buffer, strlen(buffer));
				return SCPI_RES_OK;
			}
		}
	}
	SCPI_ETSI_TEST_Send(context, "ERR\n", 4);
	return SCPI_RES_ERR;
}

scpi_result_t SCPI_ETSI_TEST_GetChannelList(scpi_t* context){
	char buffer[1024];
	if(NULL != context) {
		int32_t phy;
		SCPI_CommandNumbers(context, &phy, 1, 0);
		if(phy <= deviceDesc.phyCount-1){
			const uint32_t* channelList = *deviceDesc.phyChannelList;
			if(NULL != channelList){
				for(int channel=0; channel < deviceDesc.phyCap[phy].channelCount; channel++){
					if(channel == deviceDesc.phyCap[phy].channelCount-1){
						snprintf(buffer, sizeof(buffer), "%d,%d\n", channel, channelList[channel]);
						SCPI_ETSI_TEST_Send(context, buffer, strlen(buffer));
					} else{
						snprintf(buffer, sizeof(buffer), "%d,%d;", channel, channelList[channel]);
						SCPI_ETSI_TEST_Send(context, buffer, strlen(buffer));
					}
				}
				return SCPI_RES_OK;
			}
		}
	}
	SCPI_ETSI_TEST_Send(context, "ERR\n", 4);
	return SCPI_RES_ERR;
}

scpi_result_t SCPI_ETSI_TEST_GetChannel(scpi_t* context){
	if(NULL != context) {
		char buffer[UINT32_BUFF_SIZE];
		int32_t params[2];
		SCPI_CommandNumbers(context, params, 2, 0);
		int32_t phy = params[0];
		int32_t channelNumber = params[1];
		if(phy <= deviceDesc.phyCount-1){
			const uint32_t* channelList = *deviceDesc.phyChannelList;
			if(NULL != channelList){
				if(channelNumber <= deviceDesc.phyCap[phy].channelCount - 1){
					const uint32_t channelFreq = channelList[channelNumber];
					snprintf(buffer, sizeof(buffer), "%"PRIu32"\n", channelFreq);
					SCPI_ETSI_TEST_Send(context, buffer, strlen(buffer));
					return SCPI_RES_OK;
				}
			}
		}
	}
	SCPI_ETSI_TEST_Send(context, "ERR\n", 4);
	return SCPI_RES_ERR;
}

scpi_result_t SCPI_ETSI_TEST_GetSettings(scpi_t* context){
	if(NULL != context) {
		char buffer[128];
		snprintf(buffer, sizeof(buffer), "%u,%"PRIu16",%i,%u,%u,%"PRIu16",%"PRIu16"\n", deviceDesc.phySet.phyNumber, deviceDesc.phySet.channelNumber,
				deviceDesc.phySet.signalType, deviceDesc.phySet.power, deviceDesc.phySet.antennaNumber, deviceDesc.phySet.perTotalPacketsNumber,
				deviceDesc.phySet.perPacketLength);
		SCPI_ETSI_TEST_Send(context, buffer, strlen(buffer));
		return SCPI_RES_OK;
	}
	SCPI_ETSI_TEST_Send(context, "ERR\n", 4);
	return SCPI_RES_ERR;
}

scpi_result_t SCPI_ETSI_TEST_SetPhy(scpi_t* context){
	if(NULL != context) {
		uint32_t phy;
		if(SCPI_ParamUInt32(context, &phy, TRUE)){
			if(phy <= deviceDesc.phyCount-1){
				// do you want to change phy to another, or just try to set current one ?
				if(phy != deviceDesc.phySet.phyNumber){
					// if yes clear existing settings structure and set new phy
					memset(&(deviceDesc.phySet), 0, sizeof(SCPI_ETSI_TEST_PhySettings));
					deviceDesc.phySet.phyNumber = (uint8_t)phy;
				}
				SCPI_ETSI_TEST_Send(context, "OK\n", 3);
				return SCPI_RES_OK;
			}
		}
	}
	SCPI_ETSI_TEST_Send(context, "ERR\n", 4);
	return SCPI_RES_ERR;
}

scpi_result_t SCPI_ETSI_TEST_GetSelectedPhy(scpi_t* context){
	if(NULL != context) {
		char buffer[5];
		const uint8_t phy = deviceDesc.phySet.phyNumber;
		snprintf(buffer, sizeof(buffer), "%u\n", phy);
		SCPI_ETSI_TEST_Send(context, buffer, strlen(buffer));
		return SCPI_RES_OK;
	}
	SCPI_ETSI_TEST_Send(context, "ERR\n", 4);
	return SCPI_RES_ERR;
}

scpi_result_t SCPI_ETSI_TEST_SetChannel(scpi_t* context){
	if(NULL != context) {
		uint32_t channel;
		// if user put a value into command use it, otherwise use default
		if(SCPI_ParamUInt32(context, &channel, TRUE)){
			if(channel <= (deviceDesc.phyCap[deviceDesc.phySet.phyNumber].channelCount - 1)){
				deviceDesc.phySet.channelNumber = (uint16_t)channel;
				SCPI_ETSI_TEST_Send(context, "OK\n", 3);
				return SCPI_RES_OK;
			}
		} else{
			// check if default value can be set (is not out of range)
			if(deviceDesc.phyCap[deviceDesc.phySet.phyNumber].defaultChannelNumber <= (deviceDesc.phyCap[deviceDesc.phySet.phyNumber].channelCount - 1)){
				deviceDesc.phySet.channelNumber = deviceDesc.phyCap[deviceDesc.phySet.phyNumber].defaultChannelNumber;
				SCPI_ETSI_TEST_Send(context, "OK\n", 3);
				return SCPI_RES_OK;
			}
		}
	}
	SCPI_ETSI_TEST_Send(context, "ERR\n", 4);
	return SCPI_RES_ERR;
}

scpi_result_t SCPI_ETSI_TEST_GetSelectedChannel(scpi_t* context){
	if(NULL != context) {
		char buffer[6];
		const uint16_t channel = deviceDesc.phySet.channelNumber;
		snprintf(buffer, sizeof(buffer), "%"PRIu16"\n", channel);
		SCPI_ETSI_TEST_Send(context, buffer, strlen(buffer));
		return SCPI_RES_OK;
	}
	SCPI_ETSI_TEST_Send(context, "ERR\n", 4);
	return SCPI_RES_ERR;
}

scpi_result_t SCPI_ETSI_TEST_SetSignal(scpi_t* context){
	if(NULL != context) {
		uint32_t signal;
		// if user put a value into command use it, otherwise use default
		if(SCPI_ParamUInt32(context, &signal, TRUE)){
			if(signal <= deviceDesc.phyCap[deviceDesc.phySet.phyNumber].supportedSignals){
				deviceDesc.phySet.signalType = (uint8_t)signal;
				SCPI_ETSI_TEST_Send(context, "OK\n", 3);
				return SCPI_RES_OK;
			}
		} else{
			// check if default value can be set (is not out of range)
			if(deviceDesc.phyCap[deviceDesc.phySet.phyNumber].defaultSignalType <= deviceDesc.phyCap[deviceDesc.phySet.phyNumber].supportedSignals){
				deviceDesc.phySet.signalType = deviceDesc.phyCap[deviceDesc.phySet.phyNumber].defaultSignalType;
				SCPI_ETSI_TEST_Send(context, "OK\n", 3);
				return SCPI_RES_OK;
			}
		}
	}
	SCPI_ETSI_TEST_Send(context, "ERR\n", 4);
	return SCPI_RES_ERR;
}

scpi_result_t SCPI_ETSI_TEST_GetSelectedSignal(scpi_t* context){
	if(NULL != context) {
		char buffer[5];
		const uint8_t signal = deviceDesc.phySet.signalType;
		snprintf(buffer, sizeof(buffer), "%u\n", signal);
		SCPI_ETSI_TEST_Send(context, buffer, strlen(buffer));
		return SCPI_RES_OK;
	}
	SCPI_ETSI_TEST_Send(context, "ERR\n", 4);
	return SCPI_RES_ERR;
}

scpi_result_t SCPI_ETSI_TEST_SetPower(scpi_t* context){
	if(NULL != context) {
		int32_t power;
		// if user put a value into command use it, otherwise use default
		if(SCPI_ParamInt32(context, &power, TRUE)){
			if((power <= deviceDesc.phyCap[deviceDesc.phySet.phyNumber].highestPower) && (power >= deviceDesc.phyCap[deviceDesc.phySet.phyNumber].lowestPower)){
				deviceDesc.phySet.power = (int8_t)power;
				SCPI_ETSI_TEST_Send(context, "OK\n", 3);
				return SCPI_RES_OK;
			} else{
				// check if default value can be set (is not out of range)
				if((deviceDesc.phyCap[deviceDesc.phySet.phyNumber].defaultPower <= deviceDesc.phyCap[deviceDesc.phySet.phyNumber].highestPower)
					&& (deviceDesc.phyCap[deviceDesc.phySet.phyNumber].defaultPower >= deviceDesc.phyCap[deviceDesc.phySet.phyNumber].lowestPower)){
					deviceDesc.phySet.power = deviceDesc.phyCap[deviceDesc.phySet.phyNumber].defaultPower;
					SCPI_ETSI_TEST_Send(context, "OK\n", 3);
					return SCPI_RES_OK;
				}
			}
		}
	}
	SCPI_ETSI_TEST_Send(context, "ERR\n", 4);
	return SCPI_RES_ERR;
}

scpi_result_t SCPI_ETSI_TEST_GetSelectedPower(scpi_t* context){
	if(NULL != context) {
		char buffer[6];
		const int8_t power = deviceDesc.phySet.power;
		snprintf(buffer, sizeof(buffer), "%d\n", power);
		SCPI_ETSI_TEST_Send(context, buffer, strlen(buffer));
		return SCPI_RES_OK;
	}
	SCPI_ETSI_TEST_Send(context, "ERR\n", 4);
	return SCPI_RES_ERR;
}

scpi_result_t SCPI_ETSI_TEST_SetAntenna(scpi_t* context){
	if(NULL != context) {
		uint32_t antenna;
		// if user put a value into command use it, otherwise use default
		if(SCPI_ParamUInt32(context, &antenna, TRUE)){
			if(antenna <= deviceDesc.phyCap[deviceDesc.phySet.phyNumber].antennaCount){
				deviceDesc.phySet.antennaNumber = (uint8_t)antenna;
				SCPI_ETSI_TEST_Send(context, "OK\n", 3);
				return SCPI_RES_OK;
			}
		} else{
			// check if default value can be set (is not out of range)
			if(deviceDesc.phyCap[deviceDesc.phySet.phyNumber].defaultAntennaNumber <= deviceDesc.phyCap[deviceDesc.phySet.phyNumber].antennaCount){
				deviceDesc.phySet.antennaNumber = deviceDesc.phyCap[deviceDesc.phySet.phyNumber].defaultAntennaNumber;
				SCPI_ETSI_TEST_Send(context, "OK\n", 3);
				return SCPI_RES_OK;
			}
		}
	}
	SCPI_ETSI_TEST_Send(context, "ERR\n", 4);
	return SCPI_RES_ERR;
}

scpi_result_t SCPI_ETSI_TEST_GetSelectedAntenna(scpi_t* context){
	if(NULL != context) {
		char buffer[5];
		const uint8_t antenna = deviceDesc.phySet.antennaNumber;
		snprintf(buffer, sizeof(buffer), "%u\n", antenna);
		SCPI_ETSI_TEST_Send(context, buffer, strlen(buffer));
		return SCPI_RES_OK;
	}
	SCPI_ETSI_TEST_Send(context, "ERR\n", 4);
	return SCPI_RES_ERR;
}

scpi_result_t SCPI_ETSI_TEST_SetPERTotalPackets(scpi_t* context){
	if(NULL != context) {
		uint32_t packets;
		// if user put a value into command use it, otherwise use default
		if(SCPI_ParamUInt32(context, &packets, TRUE)){
			deviceDesc.phySet.perTotalPacketsNumber = (uint16_t)packets;
			SCPI_ETSI_TEST_Send(context, "OK\n", 3);
			return SCPI_RES_OK;
		} else{
			deviceDesc.phySet.perTotalPacketsNumber = deviceDesc.phyCap[deviceDesc.phySet.phyNumber].defaultPERTotalPacketsNumber;
			SCPI_ETSI_TEST_Send(context, "OK\n", 3);
			return SCPI_RES_OK;
		}
	}
	SCPI_ETSI_TEST_Send(context, "ERR\n", 4);
	return SCPI_RES_ERR;
}

scpi_result_t SCPI_ETSI_TEST_GetSelectedPERTotalPackets(scpi_t* context){
	if(NULL != context) {
		char buffer[6];
		const uint16_t packets = deviceDesc.phySet.perTotalPacketsNumber;
		snprintf(buffer, sizeof(buffer), "%"PRIu16"\n", packets);
		SCPI_ETSI_TEST_Send(context, buffer, strlen(buffer));
		return SCPI_RES_OK;
	}
	SCPI_ETSI_TEST_Send(context, "ERR\n", 4);
	return SCPI_RES_ERR;
}

scpi_result_t SCPI_ETSI_TEST_SetPERPacketLength(scpi_t* context){
	if(NULL != context) {
		uint32_t packetLen;
		// if user put a value into command use it, otherwise use default
		if(SCPI_ParamUInt32(context, &packetLen, TRUE)){
			if((packetLen <= deviceDesc.phyCap[deviceDesc.phySet.phyNumber].maximalPacketLength) && (packetLen >= deviceDesc.phyCap[deviceDesc.phySet.phyNumber].minimalPacketLength)){
				deviceDesc.phySet.perPacketLength = (uint8_t)packetLen;
				SCPI_ETSI_TEST_Send(context, "OK\n", 3);
				return SCPI_RES_OK;
			}
		} else{
			// check if default value can be set (is not out of range)
			if((deviceDesc.phyCap[deviceDesc.phySet.phyNumber].defaultPERPacketLength <= deviceDesc.phyCap[deviceDesc.phySet.phyNumber].maximalPacketLength)
				&& (deviceDesc.phyCap[deviceDesc.phySet.phyNumber].defaultPERPacketLength >= deviceDesc.phyCap[deviceDesc.phySet.phyNumber].minimalPacketLength)){
				deviceDesc.phySet.perPacketLength = deviceDesc.phyCap[deviceDesc.phySet.phyNumber].defaultPERPacketLength;
				SCPI_ETSI_TEST_Send(context, "OK\n", 3);
				return SCPI_RES_OK;
			}
		}
	}
	SCPI_ETSI_TEST_Send(context, "ERR\n", 4);
	return SCPI_RES_ERR;
}

scpi_result_t SCPI_ETSI_TEST_GetSelectedPERPacketLength(scpi_t* context){
	if(NULL != context) {
		char buffer[6];
		const uint16_t packetLen = deviceDesc.phySet.perPacketLength;
		snprintf(buffer, sizeof(buffer), "%"PRIu16"\n", packetLen);
		SCPI_ETSI_TEST_Send(context, buffer, strlen(buffer));
		return SCPI_RES_OK;
	}
	SCPI_ETSI_TEST_Send(context, "ERR\n", 4);
	return SCPI_RES_ERR;
}
