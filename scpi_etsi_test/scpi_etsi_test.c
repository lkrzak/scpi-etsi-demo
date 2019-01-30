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

// buffer size macros for different types to help printing (considering max number of characters and additional new line character)
enum {
	UINT8_BUFF_SIZE = 4,
	INT8_BUFF_SIZE = 5,
	UINT16_BUFF_SIZE = 6,
	INT16_BUFF_SIZE = 7,
	UINT32_BUFF_SIZE = 11,
	INT32_BUFF_SIZE = 12,
	STRING_BUFF_SIZE = 256,
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

// declaration of functions called when given SCPI command appears
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
scpi_result_t SCPI_ETSI_TEST_SetTRXMode(scpi_t* context);
scpi_result_t SCPI_ETSI_TEST_StartPERTest(scpi_t* context);
scpi_result_t SCPI_ETSI_TEST_IsPERTestRunning(scpi_t* context);
scpi_result_t SCPI_ETSI_TEST_GetPERTestResult(scpi_t* context);

static scpi_error_t scpiErrorBuffer[SCPI_ERROR_QUEUE_SIZE];
static char scpiInputBuffer[SCPI_INPUT_BUFFER_LENGTH];
static char commandBuffer[SCPI_COMMAND_BUFFER_LENGTH];

// handled SCPI command list
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
										{ .pattern = "TRXmode",							.callback = SCPI_ETSI_TEST_SetTRXMode, },
										{ .pattern = "PER",								.callback = SCPI_ETSI_TEST_StartPERTest, },
										{ .pattern = "PER?",							.callback = SCPI_ETSI_TEST_IsPERTestRunning, },
										{ .pattern = "PERRESULT?",						.callback = SCPI_ETSI_TEST_GetPERTestResult, },
										SCPI_CMD_LIST_END };

static scpi_interface_t scpiInterface = {   .write = NULL,
											.error = NULL,
											.reset = NULL,
											.flush = NULL, };

// device structure descriptor
static SCPI_ETSI_TEST_DeviceDescriptor deviceDesc;
// scpi parser handler
static scpi_t scpiContext;

SCPIResult SCPI_ETSI_TEST_Init(void){
	if(NULL != scpiInputBuffer){
		if(NULL != scpiErrorBuffer){
			// initialize parser library
			SCPI_Init(&scpiContext, scpiCommands, &scpiInterface, scpi_units_def, NULL, NULL, NULL, NULL, scpiInputBuffer,
					SCPI_INPUT_BUFFER_LENGTH, scpiErrorBuffer, SCPI_ERROR_QUEUE_SIZE);
			// initialize user implementation (filling up data structures)
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
		// try to get character from input at least once
		do{
			// check if there is antyhing to receive
			if(true == SCPI_ETSI_TEST_USER_GetChar(&commandBuffer[byte])){
				if(commandBuffer[byte] != 0){
					// check if received character is not a communication termination character
					if(SCPI_READ_TERMINATION_CHARACTER == commandBuffer[byte]){
						// if you get termination character start parsing received command
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

void SCPI_ETSI_TEST_Send(const void* data, size_t size){
	if(NULL != data) {
		// send given message character by character
		for(int chunk=0; chunk<size; chunk++){
			const char* wordStart = data;
			SCPI_ETSI_TEST_USER_PutChar(*(wordStart+chunk));
		}
	}
}

scpi_result_t SCPI_ETSI_TEST_GetIDN(scpi_t* context){
	char buffer[STRING_BUFF_SIZE];
	if(NULL != context) {
		const char* description = deviceDesc.idn;
		if(NULL != description){
			snprintf(buffer, sizeof(buffer), "%s\n", description);
			SCPI_ETSI_TEST_Send(buffer, strlen(buffer));
			return SCPI_RES_OK;
		}
	}
	SCPI_ETSI_TEST_Send("ERR\n", 4);
	return SCPI_RES_ERR;
}

scpi_result_t SCPI_ETSI_TEST_Reset(scpi_t* context){
	if(NULL != context) {
		SCPI_ETSI_TEST_Send("OK\n", 3);
		SCPI_ETSI_TEST_USER_Reset(&deviceDesc);
		return SCPI_RES_OK;
	}
	SCPI_ETSI_TEST_Send("ERR\n", 4);
	return SCPI_RES_ERR;
}

scpi_result_t SCPI_ETSI_TEST_GetPhyCount(scpi_t* context){
	if(NULL != context) {
		char buffer[UINT8_BUFF_SIZE];
		const uint8_t phyCount = deviceDesc.phyCount;
		snprintf(buffer, sizeof(buffer), "%u\n", phyCount);
		SCPI_ETSI_TEST_Send(buffer, strlen(buffer));
		return SCPI_RES_OK;
	}
	SCPI_ETSI_TEST_Send("ERR\n", 4);
	return SCPI_RES_ERR;
}

scpi_result_t SCPI_ETSI_TEST_GetPhyCapabilities(scpi_t* context){
	if(NULL != context) {
		char buffer[STRING_BUFF_SIZE];
		int32_t phy;
		// get phy suffix from command
		SCPI_CommandNumbers(context, &phy, 1, 0);
		// check if phy value is not out of bounds
		if(phy <= deviceDesc.phyCount-1){
			snprintf(buffer, sizeof(buffer), "%"PRIu32",%"PRIu32",%"PRIu16",%"PRIu32",%"PRIu32",%i,%i,%i,%"PRIu16",%"PRIu16",%"PRIu16",%u,%u,%u\n", deviceDesc.phyCapabilities[phy].lowestFrequency,
					deviceDesc.phyCapabilities[phy].highestFrequency, deviceDesc.phyCapabilities[phy].channelCount, deviceDesc.phyCapabilities[phy].channelBandwidth, deviceDesc.phyCapabilities[phy].baudrate,
					deviceDesc.phyCapabilities[phy].lowestPower, deviceDesc.phyCapabilities[phy].highestPower, deviceDesc.phyCapabilities[phy].defaultPower, deviceDesc.phyCapabilities[phy].minimalPacketLength,
					deviceDesc.phyCapabilities[phy].maximalPacketLength, deviceDesc.phyCapabilities[phy].defaultPERPacketLength, deviceDesc.phyCapabilities[phy].modulationType, deviceDesc.phyCapabilities[phy].supportedSignals,
					deviceDesc.phyCapabilities[phy].antennaCount);
			SCPI_ETSI_TEST_Send(buffer, strlen(buffer));
			return SCPI_RES_OK;
		}
	}
	SCPI_ETSI_TEST_Send("ERR\n", 4);
	return SCPI_RES_ERR;
}

scpi_result_t SCPI_ETSI_TEST_GetLowestFrequency(scpi_t* context){
	if(NULL != context) {
		char buffer[UINT32_BUFF_SIZE];
		int32_t phy;
		// get phy suffix from command
		SCPI_CommandNumbers(context, &phy, 1, 0);
		// check if phy value is not out of bounds
		if(phy <= deviceDesc.phyCount-1){
			const uint32_t freq = deviceDesc.phyCapabilities[phy].lowestFrequency;
			snprintf(buffer, sizeof(buffer), "%"PRIu32"\n", freq);
			SCPI_ETSI_TEST_Send(buffer, strlen(buffer));
			return SCPI_RES_OK;
		}
	}
	SCPI_ETSI_TEST_Send("ERR\n", 4);
	return SCPI_RES_ERR;
}

scpi_result_t SCPI_ETSI_TEST_GetHighestFrequency(scpi_t* context){
	if(NULL != context) {
		char buffer[UINT32_BUFF_SIZE];
		int32_t phy;
		// get phy suffix from command
		SCPI_CommandNumbers(context, &phy, 1, 0);
		// check if phy value is not out of bounds
		if(phy <= deviceDesc.phyCount-1){
			const uint32_t freq = deviceDesc.phyCapabilities[phy].highestFrequency;
			snprintf(buffer, sizeof(buffer), "%"PRIu32"\n", freq);
			SCPI_ETSI_TEST_Send(buffer, strlen(buffer));
			return SCPI_RES_OK;
		}
	}
	SCPI_ETSI_TEST_Send("ERR\n", 4);
	return SCPI_RES_ERR;
}

scpi_result_t SCPI_ETSI_TEST_GetChannelCount(scpi_t* context){
	if(NULL != context) {
		char buffer[UINT16_BUFF_SIZE];
		int32_t phy;
		// get phy suffix from command
		SCPI_CommandNumbers(context, &phy, 1, 0);
		// check if phy value is not out of bounds
		if(phy <= deviceDesc.phyCount-1){
			const uint16_t channelCount = deviceDesc.phyCapabilities[phy].channelCount;
			snprintf(buffer, sizeof(buffer), "%"PRIu16"\n", channelCount);
			SCPI_ETSI_TEST_Send(buffer, strlen(buffer));
			return SCPI_RES_OK;
		}
	}
	SCPI_ETSI_TEST_Send("ERR\n", 4);
	return SCPI_RES_ERR;
}

scpi_result_t SCPI_ETSI_TEST_GetChannelBandwidth(scpi_t* context){
	if(NULL != context) {
		char buffer[UINT32_BUFF_SIZE];
		int32_t phy;
		// get phy suffix from command
		SCPI_CommandNumbers(context, &phy, 1, 0);
		// check if phy value is not out of bounds
		if(phy <= deviceDesc.phyCount-1){
			const uint32_t channelBandwidth = deviceDesc.phyCapabilities[phy].channelBandwidth;
			snprintf(buffer, sizeof(buffer), "%"PRIu32"\n", channelBandwidth);
			SCPI_ETSI_TEST_Send(buffer, strlen(buffer));
			return SCPI_RES_OK;
		}
	}
	SCPI_ETSI_TEST_Send("ERR\n", 4);
	return SCPI_RES_ERR;
}

scpi_result_t SCPI_ETSI_TEST_GetBaudrate(scpi_t* context){
	if(NULL != context) {
		char buffer[UINT32_BUFF_SIZE];
		int32_t phy;
		// get phy suffix from command
		SCPI_CommandNumbers(context, &phy, 1, 0);
		// check if phy value is not out of bounds
		if(phy <= deviceDesc.phyCount-1){
			const uint32_t baudrate = deviceDesc.phyCapabilities[phy].baudrate;
			snprintf(buffer, sizeof(buffer), "%"PRIu32"\n", baudrate);
			SCPI_ETSI_TEST_Send(buffer, strlen(buffer));
			return SCPI_RES_OK;
		}
	}
	SCPI_ETSI_TEST_Send("ERR\n", 4);
	return SCPI_RES_ERR;
}

scpi_result_t SCPI_ETSI_TEST_GetLowestPower(scpi_t* context){
	if(NULL != context) {
		char buffer[INT8_BUFF_SIZE];
		int32_t phy;
		// get phy suffix from command
		SCPI_CommandNumbers(context, &phy, 1, 0);
		// check if phy value is not out of bounds
		if(phy <= deviceDesc.phyCount-1){
			const int8_t power = deviceDesc.phyCapabilities[phy].lowestPower;
			snprintf(buffer, sizeof(buffer), "%i\n", power);
			SCPI_ETSI_TEST_Send(buffer, strlen(buffer));
			return SCPI_RES_OK;
		}
	}
	SCPI_ETSI_TEST_Send("ERR\n", 4);
	return SCPI_RES_ERR;
}

scpi_result_t SCPI_ETSI_TEST_GetHighestPower(scpi_t* context){
	if(NULL != context) {
		char buffer[INT8_BUFF_SIZE];
		int32_t phy;
		// get phy suffix from command
		SCPI_CommandNumbers(context, &phy, 1, 0);
		// check if phy value is not out of bounds
		if(phy <= deviceDesc.phyCount-1){
			const int8_t power = deviceDesc.phyCapabilities[phy].highestPower;
			snprintf(buffer, sizeof(buffer), "%i\n", power);
			SCPI_ETSI_TEST_Send(buffer, strlen(buffer));
			return SCPI_RES_OK;
		}
	}
	SCPI_ETSI_TEST_Send("ERR\n", 4);
	return SCPI_RES_ERR;
}

scpi_result_t SCPI_ETSI_TEST_GetMinPacketLength(scpi_t* context){
	if(NULL != context) {
		char buffer[UINT16_BUFF_SIZE];
		int32_t phy;
		// get phy suffix from command
		SCPI_CommandNumbers(context, &phy, 1, 0);
		// check if phy value is not out of bounds
		if(phy <= deviceDesc.phyCount-1){
			const uint16_t pktLen = deviceDesc.phyCapabilities[phy].minimalPacketLength;
			snprintf(buffer, sizeof(buffer), "%"PRIu16"\n", pktLen);
			SCPI_ETSI_TEST_Send(buffer, strlen(buffer));
			return SCPI_RES_OK;
		}
	}
	SCPI_ETSI_TEST_Send("ERR\n", 4);
	return SCPI_RES_ERR;
}

scpi_result_t SCPI_ETSI_TEST_GetMaxPacketLength(scpi_t* context){
	if(NULL != context) {
		char buffer[UINT16_BUFF_SIZE];
		int32_t phy;
		// get phy suffix from command
		SCPI_CommandNumbers(context, &phy, 1, 0);
		// check if phy value is not out of bounds
		if(phy <= deviceDesc.phyCount-1){
			const uint16_t pktLen = deviceDesc.phyCapabilities[phy].maximalPacketLength;
			snprintf(buffer, sizeof(buffer), "%"PRIu16"\n", pktLen);
			SCPI_ETSI_TEST_Send(buffer, strlen(buffer));
			return SCPI_RES_OK;
		}
	}
	SCPI_ETSI_TEST_Send("ERR\n", 4);
	return SCPI_RES_ERR;
}

scpi_result_t SCPI_ETSI_TEST_GetModulationType(scpi_t* context){
	if(NULL != context) {
		char buffer[UINT8_BUFF_SIZE];
		int32_t phy;
		// get phy suffix from command
		SCPI_CommandNumbers(context, &phy, 1, 0);
		// check if phy value is not out of bounds
		if(phy <= deviceDesc.phyCount-1){
			const uint8_t mod = deviceDesc.phyCapabilities[phy].modulationType;
			snprintf(buffer, sizeof(buffer), "%u\n", mod);
			SCPI_ETSI_TEST_Send(buffer, strlen(buffer));
			return SCPI_RES_OK;
		}
	}
	SCPI_ETSI_TEST_Send("ERR\n", 4);
	return SCPI_RES_ERR;
}

scpi_result_t SCPI_ETSI_TEST_GetSupportedSignals(scpi_t* context){
	if(NULL != context) {
		char buffer[UINT8_BUFF_SIZE];
		int32_t phy;
		// get phy suffix from command
		SCPI_CommandNumbers(context, &phy, 1, 0);
		// check if phy value is not out of bounds
		if(phy <= deviceDesc.phyCount-1){
			const uint8_t signals = deviceDesc.phyCapabilities[phy].supportedSignals;
			snprintf(buffer, sizeof(buffer), "%u\n", signals);
			SCPI_ETSI_TEST_Send(buffer, strlen(buffer));
			return SCPI_RES_OK;
		}
	}
	SCPI_ETSI_TEST_Send("ERR\n", 4);
	return SCPI_RES_ERR;
}

scpi_result_t SCPI_ETSI_TEST_GetAntennaCount(scpi_t* context){
	if(NULL != context) {
		char buffer[UINT8_BUFF_SIZE];
		int32_t phy;
		// get phy suffix from command
		SCPI_CommandNumbers(context, &phy, 1, 0);
		// check if phy value is not out of bounds
		if(phy <= deviceDesc.phyCount-1){
			const uint8_t antennaCount = deviceDesc.phyCapabilities[phy].antennaCount;
			snprintf(buffer, sizeof(buffer), "%u\n", antennaCount);
			SCPI_ETSI_TEST_Send(buffer, strlen(buffer));
			return SCPI_RES_OK;
		}
	}
	SCPI_ETSI_TEST_Send("ERR\n", 4);
	return SCPI_RES_ERR;
}

scpi_result_t SCPI_ETSI_TEST_GetPhyDescription(scpi_t* context){
	if(NULL != context) {
		char buffer[STRING_BUFF_SIZE];
		int32_t phy;
		// get phy suffix from command
		SCPI_CommandNumbers(context, &phy, 1, 0);
		// check if phy value is not out of bounds
		if(phy <= deviceDesc.phyCount-1){
			const char* description = *(deviceDesc.phyDescriptions+phy);
			if(NULL != description){
				snprintf(buffer, sizeof(buffer), "%s\n", description);
				SCPI_ETSI_TEST_Send(buffer, strlen(buffer));
				return SCPI_RES_OK;
			}
		}
	}
	SCPI_ETSI_TEST_Send("ERR\n", 4);
	return SCPI_RES_ERR;
}

scpi_result_t SCPI_ETSI_TEST_GetChannelList(scpi_t* context){
	char buffer[1024];
	if(NULL != context) {
		int32_t phy;
		// get phy suffix from command
		SCPI_CommandNumbers(context, &phy, 1, 0);
		// check if phy value is not out of bounds
		if(phy <= deviceDesc.phyCount-1){
			const uint32_t* channelList = *deviceDesc.phyChannelList;
			if(NULL != channelList){
				// print about all channel list
				for(int channel=0; channel < deviceDesc.phyCapabilities[phy].channelCount; channel++){
					if(channel == deviceDesc.phyCapabilities[phy].channelCount-1){
						snprintf(buffer, sizeof(buffer), "%d,%d\n", channel, channelList[channel]);
						SCPI_ETSI_TEST_Send(buffer, strlen(buffer));
					} else{
						snprintf(buffer, sizeof(buffer), "%d,%d;", channel, channelList[channel]);
						SCPI_ETSI_TEST_Send(buffer, strlen(buffer));
					}
				}
				return SCPI_RES_OK;
			}
		}
	}
	SCPI_ETSI_TEST_Send("ERR\n", 4);
	return SCPI_RES_ERR;
}

scpi_result_t SCPI_ETSI_TEST_GetChannel(scpi_t* context){
	if(NULL != context) {
		char buffer[UINT32_BUFF_SIZE];
		int32_t params[2];
		// get phy suffix and channel suffix from command
		SCPI_CommandNumbers(context, params, 2, 0);
		int32_t phy = params[0];
		int32_t channelNumber = params[1];
		// check if phy value is not out of bounds
		if(phy <= deviceDesc.phyCount-1){
			const uint32_t* channelList = *deviceDesc.phyChannelList;
			if(NULL != channelList){
				// check if channel number is not out of bounds
				if(channelNumber <= deviceDesc.phyCapabilities[phy].channelCount - 1){
					const uint32_t channelFreq = channelList[channelNumber];
					snprintf(buffer, sizeof(buffer), "%"PRIu32"\n", channelFreq);
					SCPI_ETSI_TEST_Send(buffer, strlen(buffer));
					return SCPI_RES_OK;
				}
			}
		}
	}
	SCPI_ETSI_TEST_Send("ERR\n", 4);
	return SCPI_RES_ERR;
}

scpi_result_t SCPI_ETSI_TEST_GetSettings(scpi_t* context){
	if(NULL != context) {
		char buffer[STRING_BUFF_SIZE];
		snprintf(buffer, sizeof(buffer), "%u,%"PRIu16",%u,%i,%u,%"PRIu16",%"PRIu16"\n", deviceDesc.phySettings.phyNumber, deviceDesc.phySettings.channelNumber,
				deviceDesc.phySettings.signalType, deviceDesc.phySettings.power, deviceDesc.phySettings.antennaNumber, deviceDesc.phySettings.perTotalPacketsNumber,
				deviceDesc.phySettings.perPacketLength);
		SCPI_ETSI_TEST_Send(buffer, strlen(buffer));
		return SCPI_RES_OK;
	}
	SCPI_ETSI_TEST_Send("ERR\n", 4);
	return SCPI_RES_ERR;
}

scpi_result_t SCPI_ETSI_TEST_SetPhy(scpi_t* context){
	if(NULL != context) {
		uint32_t phy;
		// get phy number from parser
		if(SCPI_ParamUInt32(context, &phy, TRUE)){
			// check if phy value is not out of bounds
			if(phy <= deviceDesc.phyCount-1){
				// if yes clear existing settings structure and set new phy
				memset(&(deviceDesc.phySettings), 0, sizeof(SCPI_ETSI_TEST_PhySettings));
				deviceDesc.phySettings.phyNumber = (uint8_t)phy;
				// then fill up with default settings
				// check each one if it has proper default value
				if(deviceDesc.phyCapabilities[phy].defaultChannelNumber <= (deviceDesc.phyCapabilities[phy].channelCount - 1)){
					deviceDesc.phySettings.channelNumber = deviceDesc.phyCapabilities[phy].defaultChannelNumber;
				}
				if(deviceDesc.phyCapabilities[phy].defaultSignalType <= deviceDesc.phyCapabilities[phy].supportedSignals){
					deviceDesc.phySettings.signalType = deviceDesc.phyCapabilities[phy].defaultSignalType;
				}
				if((deviceDesc.phyCapabilities[phy].defaultPower <= deviceDesc.phyCapabilities[phy].highestPower)
									&& (deviceDesc.phyCapabilities[phy].defaultPower >= deviceDesc.phyCapabilities[phy].lowestPower)){
					deviceDesc.phySettings.power = deviceDesc.phyCapabilities[phy].defaultPower;
				}
				if(deviceDesc.phyCapabilities[phy].defaultAntennaNumber <= deviceDesc.phyCapabilities[phy].antennaCount){
					deviceDesc.phySettings.antennaNumber = deviceDesc.phyCapabilities[phy].defaultAntennaNumber;
				}
				deviceDesc.phySettings.perTotalPacketsNumber = deviceDesc.phyCapabilities[phy].defaultPERTotalPacketsNumber;
				if((deviceDesc.phyCapabilities[phy].defaultPERPacketLength <= deviceDesc.phyCapabilities[phy].maximalPacketLength)
								&& (deviceDesc.phyCapabilities[phy].defaultPERPacketLength >= deviceDesc.phyCapabilities[phy].minimalPacketLength)){
					deviceDesc.phySettings.perPacketLength = deviceDesc.phyCapabilities[phy].defaultPERPacketLength;
				}
				SCPI_ETSI_TEST_Send("OK\n", 3);
				return SCPI_RES_OK;
			}
		}
	}
	SCPI_ETSI_TEST_Send("ERR\n", 4);
	return SCPI_RES_ERR;
}

scpi_result_t SCPI_ETSI_TEST_GetSelectedPhy(scpi_t* context){
	if(NULL != context) {
		char buffer[UINT8_BUFF_SIZE];
		const uint8_t phy = deviceDesc.phySettings.phyNumber;
		snprintf(buffer, sizeof(buffer), "%u\n", phy);
		SCPI_ETSI_TEST_Send(buffer, strlen(buffer));
		return SCPI_RES_OK;
	}
	SCPI_ETSI_TEST_Send("ERR\n", 4);
	return SCPI_RES_ERR;
}

scpi_result_t SCPI_ETSI_TEST_SetChannel(scpi_t* context){
	if(NULL != context) {
		uint32_t channel;
		// if user put a value into command use it, otherwise use default
		if(SCPI_ParamUInt32(context, &channel, TRUE)){
			if(channel <= (deviceDesc.phyCapabilities[deviceDesc.phySettings.phyNumber].channelCount - 1)){
				deviceDesc.phySettings.channelNumber = (uint16_t)channel;
				SCPI_ETSI_TEST_Send("OK\n", 3);
				return SCPI_RES_OK;
			}
		} else{
			// check if default value can be set (is not out of range)
			if(deviceDesc.phyCapabilities[deviceDesc.phySettings.phyNumber].defaultChannelNumber <= (deviceDesc.phyCapabilities[deviceDesc.phySettings.phyNumber].channelCount - 1)){
				deviceDesc.phySettings.channelNumber = deviceDesc.phyCapabilities[deviceDesc.phySettings.phyNumber].defaultChannelNumber;
				SCPI_ETSI_TEST_Send("OK\n", 3);
				return SCPI_RES_OK;
			}
		}
	}
	SCPI_ETSI_TEST_Send("ERR\n", 4);
	return SCPI_RES_ERR;
}

scpi_result_t SCPI_ETSI_TEST_GetSelectedChannel(scpi_t* context){
	if(NULL != context) {
		char buffer[UINT16_BUFF_SIZE];
		const uint16_t channel = deviceDesc.phySettings.channelNumber;
		snprintf(buffer, sizeof(buffer), "%"PRIu16"\n", channel);
		SCPI_ETSI_TEST_Send(buffer, strlen(buffer));
		return SCPI_RES_OK;
	}
	SCPI_ETSI_TEST_Send("ERR\n", 4);
	return SCPI_RES_ERR;
}

scpi_result_t SCPI_ETSI_TEST_SetSignal(scpi_t* context){
	if(NULL != context) {
		uint32_t signal;
		// if user put a value into command use it, otherwise use default
		if(SCPI_ParamUInt32(context, &signal, TRUE)){
			if(signal <= deviceDesc.phyCapabilities[deviceDesc.phySettings.phyNumber].supportedSignals){
				deviceDesc.phySettings.signalType = (uint8_t)signal;
				SCPI_ETSI_TEST_Send("OK\n", 3);
				return SCPI_RES_OK;
			}
		} else{
			// check if default value can be set (is not out of range)
			if(deviceDesc.phyCapabilities[deviceDesc.phySettings.phyNumber].defaultSignalType <= deviceDesc.phyCapabilities[deviceDesc.phySettings.phyNumber].supportedSignals){
				deviceDesc.phySettings.signalType = deviceDesc.phyCapabilities[deviceDesc.phySettings.phyNumber].defaultSignalType;
				SCPI_ETSI_TEST_Send("OK\n", 3);
				return SCPI_RES_OK;
			}
		}
	}
	SCPI_ETSI_TEST_Send("ERR\n", 4);
	return SCPI_RES_ERR;
}

scpi_result_t SCPI_ETSI_TEST_GetSelectedSignal(scpi_t* context){
	if(NULL != context) {
		char buffer[UINT8_BUFF_SIZE];
		const uint8_t signal = deviceDesc.phySettings.signalType;
		snprintf(buffer, sizeof(buffer), "%u\n", signal);
		SCPI_ETSI_TEST_Send(buffer, strlen(buffer));
		return SCPI_RES_OK;
	}
	SCPI_ETSI_TEST_Send("ERR\n", 4);
	return SCPI_RES_ERR;
}

scpi_result_t SCPI_ETSI_TEST_SetPower(scpi_t* context){
	if(NULL != context) {
		int32_t power;
		// if user put a value into command use it, otherwise use default
		if(SCPI_ParamInt32(context, &power, TRUE)){
			if((power <= deviceDesc.phyCapabilities[deviceDesc.phySettings.phyNumber].highestPower) && (power >= deviceDesc.phyCapabilities[deviceDesc.phySettings.phyNumber].lowestPower)){
				deviceDesc.phySettings.power = (int8_t)power;
				SCPI_ETSI_TEST_Send("OK\n", 3);
				return SCPI_RES_OK;
			}
		} else{
			// check if default value can be set (is not out of range)
			if((deviceDesc.phyCapabilities[deviceDesc.phySettings.phyNumber].defaultPower <= deviceDesc.phyCapabilities[deviceDesc.phySettings.phyNumber].highestPower)
				&& (deviceDesc.phyCapabilities[deviceDesc.phySettings.phyNumber].defaultPower >= deviceDesc.phyCapabilities[deviceDesc.phySettings.phyNumber].lowestPower)){
				deviceDesc.phySettings.power = deviceDesc.phyCapabilities[deviceDesc.phySettings.phyNumber].defaultPower;
				SCPI_ETSI_TEST_Send("OK\n", 3);
				return SCPI_RES_OK;
			}
		}
	}
	SCPI_ETSI_TEST_Send("ERR\n", 4);
	return SCPI_RES_ERR;
}

scpi_result_t SCPI_ETSI_TEST_GetSelectedPower(scpi_t* context){
	if(NULL != context) {
		char buffer[INT8_BUFF_SIZE];
		const int8_t power = deviceDesc.phySettings.power;
		snprintf(buffer, sizeof(buffer), "%d\n", power);
		SCPI_ETSI_TEST_Send(buffer, strlen(buffer));
		return SCPI_RES_OK;
	}
	SCPI_ETSI_TEST_Send("ERR\n", 4);
	return SCPI_RES_ERR;
}

scpi_result_t SCPI_ETSI_TEST_SetAntenna(scpi_t* context){
	if(NULL != context) {
		uint32_t antenna;
		// if user put a value into command use it, otherwise use default
		if(SCPI_ParamUInt32(context, &antenna, TRUE)){
			if(antenna <= deviceDesc.phyCapabilities[deviceDesc.phySettings.phyNumber].antennaCount){
				deviceDesc.phySettings.antennaNumber = (uint8_t)antenna;
				SCPI_ETSI_TEST_Send("OK\n", 3);
				return SCPI_RES_OK;
			}
		} else{
			// check if default value can be set (is not out of range)
			if(deviceDesc.phyCapabilities[deviceDesc.phySettings.phyNumber].defaultAntennaNumber <= deviceDesc.phyCapabilities[deviceDesc.phySettings.phyNumber].antennaCount){
				deviceDesc.phySettings.antennaNumber = deviceDesc.phyCapabilities[deviceDesc.phySettings.phyNumber].defaultAntennaNumber;
				SCPI_ETSI_TEST_Send("OK\n", 3);
				return SCPI_RES_OK;
			}
		}
	}
	SCPI_ETSI_TEST_Send("ERR\n", 4);
	return SCPI_RES_ERR;
}

scpi_result_t SCPI_ETSI_TEST_GetSelectedAntenna(scpi_t* context){
	if(NULL != context) {
		char buffer[UINT8_BUFF_SIZE];
		const uint8_t antenna = deviceDesc.phySettings.antennaNumber;
		snprintf(buffer, sizeof(buffer), "%u\n", antenna);
		SCPI_ETSI_TEST_Send(buffer, strlen(buffer));
		return SCPI_RES_OK;
	}
	SCPI_ETSI_TEST_Send("ERR\n", 4);
	return SCPI_RES_ERR;
}

scpi_result_t SCPI_ETSI_TEST_SetPERTotalPackets(scpi_t* context){
	if(NULL != context) {
		uint32_t packets;
		// if user put a value into command use it, otherwise use default
		if(SCPI_ParamUInt32(context, &packets, TRUE)){
			deviceDesc.phySettings.perTotalPacketsNumber = (uint16_t)packets;
			SCPI_ETSI_TEST_Send("OK\n", 3);
			return SCPI_RES_OK;
		} else{
			deviceDesc.phySettings.perTotalPacketsNumber = deviceDesc.phyCapabilities[deviceDesc.phySettings.phyNumber].defaultPERTotalPacketsNumber;
			SCPI_ETSI_TEST_Send("OK\n", 3);
			return SCPI_RES_OK;
		}
	}
	SCPI_ETSI_TEST_Send("ERR\n", 4);
	return SCPI_RES_ERR;
}

scpi_result_t SCPI_ETSI_TEST_GetSelectedPERTotalPackets(scpi_t* context){
	if(NULL != context) {
		char buffer[UINT16_BUFF_SIZE];
		const uint16_t packets = deviceDesc.phySettings.perTotalPacketsNumber;
		snprintf(buffer, sizeof(buffer), "%"PRIu16"\n", packets);
		SCPI_ETSI_TEST_Send(buffer, strlen(buffer));
		return SCPI_RES_OK;
	}
	SCPI_ETSI_TEST_Send("ERR\n", 4);
	return SCPI_RES_ERR;
}

scpi_result_t SCPI_ETSI_TEST_SetPERPacketLength(scpi_t* context){
	if(NULL != context) {
		uint32_t packetLen;
		// if user put a value into command use it, otherwise use default
		if(SCPI_ParamUInt32(context, &packetLen, TRUE)){
			if((packetLen <= deviceDesc.phyCapabilities[deviceDesc.phySettings.phyNumber].maximalPacketLength) && (packetLen >= deviceDesc.phyCapabilities[deviceDesc.phySettings.phyNumber].minimalPacketLength)){
				deviceDesc.phySettings.perPacketLength = (uint8_t)packetLen;
				SCPI_ETSI_TEST_Send("OK\n", 3);
				return SCPI_RES_OK;
			}
		} else{
			// check if default value can be set (is not out of range)
			if((deviceDesc.phyCapabilities[deviceDesc.phySettings.phyNumber].defaultPERPacketLength <= deviceDesc.phyCapabilities[deviceDesc.phySettings.phyNumber].maximalPacketLength)
				&& (deviceDesc.phyCapabilities[deviceDesc.phySettings.phyNumber].defaultPERPacketLength >= deviceDesc.phyCapabilities[deviceDesc.phySettings.phyNumber].minimalPacketLength)){
				deviceDesc.phySettings.perPacketLength = deviceDesc.phyCapabilities[deviceDesc.phySettings.phyNumber].defaultPERPacketLength;
				SCPI_ETSI_TEST_Send("OK\n", 3);
				return SCPI_RES_OK;
			}
		}
	}
	SCPI_ETSI_TEST_Send("ERR\n", 4);
	return SCPI_RES_ERR;
}

scpi_result_t SCPI_ETSI_TEST_GetSelectedPERPacketLength(scpi_t* context){
	if(NULL != context) {
		char buffer[UINT16_BUFF_SIZE];
		const uint16_t packetLen = deviceDesc.phySettings.perPacketLength;
		snprintf(buffer, sizeof(buffer), "%"PRIu16"\n", packetLen);
		SCPI_ETSI_TEST_Send(buffer, strlen(buffer));
		return SCPI_RES_OK;
	}
	SCPI_ETSI_TEST_Send("ERR\n", 4);
	return SCPI_RES_ERR;
}

scpi_result_t SCPI_ETSI_TEST_SetTRXMode(scpi_t* context){
	if(NULL != context) {
		uint32_t mode;
		// get mode number from parser
		if(SCPI_ParamUInt32(context, &mode, TRUE)){
			// check if parameter has proper value (0,1 or 2)
			if(mode == 0 || mode == 1 || mode == 2){
				if(true == SCPI_ETSI_TEST_USER_SetTRXMode(&deviceDesc, (uint8_t)mode)){
					SCPI_ETSI_TEST_Send("OK\n", 3);
					return SCPI_RES_OK;
				} else{
					SCPI_ETSI_TEST_Send("ERR\n", 4);
					return SCPI_RES_ERR;
				}
			}
		}
	}
	SCPI_ETSI_TEST_Send("ERR\n", 4);
	return SCPI_RES_ERR;
}

scpi_result_t SCPI_ETSI_TEST_StartPERTest(scpi_t* context){
	if(NULL != context) {
		uint32_t testID;
		// get test ID from parser
		if(SCPI_ParamUInt32(context, &testID, TRUE)){
			// check if any test is running at the moment
			if(false == SCPI_ETSI_TEST_USER_IsPERTestRunning(&deviceDesc)){
				// if not, try to start new test
				if(true == SCPI_ETSI_TEST_USER_StartPERTest(&deviceDesc, testID)){
					SCPI_ETSI_TEST_Send("OK\n", 3);
					return SCPI_RES_OK;
				}
			}
		}
	}
	SCPI_ETSI_TEST_Send("ERR\n", 4);
	return SCPI_RES_ERR;
}

scpi_result_t SCPI_ETSI_TEST_IsPERTestRunning(scpi_t* context){
	if(NULL != context) {
		char buffer[UINT8_BUFF_SIZE];
		const bool testStatus = SCPI_ETSI_TEST_USER_IsPERTestRunning(&deviceDesc);
		snprintf(buffer, sizeof(buffer), "%d\n", testStatus);
		SCPI_ETSI_TEST_Send(buffer, strlen(buffer));
		return SCPI_RES_OK;
	}
	SCPI_ETSI_TEST_Send("ERR\n", 4);
	return SCPI_RES_ERR;
}

scpi_result_t SCPI_ETSI_TEST_GetPERTestResult(scpi_t* context){
	if(NULL != context) {
		char buffer[STRING_BUFF_SIZE];
		SCPI_ETSI_TEST_PERTestResult* testResult = SCPI_ETSI_TEST_USER_GetPERTestResult(&deviceDesc);
		if(NULL != testResult){
			snprintf(buffer, sizeof(buffer), "%"PRIu32",%"PRIu16",%"PRIu16"\n", testResult->testID, testResult->totalPacketsNumber, testResult->receivedPacketsNumber);
			SCPI_ETSI_TEST_Send(buffer, strlen(buffer));
			return SCPI_RES_OK;
		}
	}
	SCPI_ETSI_TEST_Send("ERR\n", 4);
	return SCPI_RES_ERR;
}
