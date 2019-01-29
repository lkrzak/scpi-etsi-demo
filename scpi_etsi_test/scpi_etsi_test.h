/**
@file
@license   $License$
@copyright $Copyright$
@version   $Revision$
@purpose   SCPI ETSI TEST parser
@brief     SCPI ETSI TEST parser
*/

#ifndef SCPI_ETSI_TEST_H_
#define SCPI_ETSI_TEST_H_

#include "scpi.h"

typedef enum{
	SCPI_OK = 0,
	SCPI_ERROR = 1,
}SCPIResult;

typedef struct{
	uint8_t phyNumber;
	uint16_t channelNumber;
	int8_t power;
	uint8_t signalType;
	uint8_t antennaNumber;
	uint16_t perTotalPacketsNumber;
	uint16_t perPacketLength;
}SCPI_ETSI_TEST_PhySettings;

typedef struct{
	uint32_t lowestFrequency;
	uint32_t highestFrequency;
	uint16_t channelCount;
	uint32_t channelBandwidth;
	uint32_t baudrate;
	int8_t lowestPower;
	int8_t highestPower;
	uint16_t minimalPacketLength;
	uint16_t maximalPacketLength;
	uint8_t modulationType;
	uint8_t supportedSignals;
	uint8_t antennaCount;
	uint8_t defaultPhyNumber;
	uint16_t defaultChannelNumber;
	uint8_t defaultSignalType;
	int8_t defaultPower;
	uint8_t defaultAntennaNumber;
	uint16_t defaultPERTotalPacketsNumber;
	uint16_t defaultPERPacketLength;
}SCPI_ETSI_TEST_PhyCapabilities;

typedef uint32_t* SCPI_ETSI_TEST_PhyChannelList;

typedef struct{
	uint8_t phyCount;
	const char* idn;
	SCPI_ETSI_TEST_PhySettings phySet;
	const char** phyDescriptions;
	const SCPI_ETSI_TEST_PhyCapabilities* phyCap;
	const SCPI_ETSI_TEST_PhyChannelList* phyChannelList;
}SCPI_ETSI_TEST_DeviceDescriptor;

SCPIResult SCPI_ETSI_TEST_Init(void);
SCPIResult SCPI_ETSI_TEST_Proc(void);
void SCPI_ETSI_TEST_Send(scpi_t* context, const void* data, size_t size);

#endif /* SCPI_ETSI_TEST_H_ */
