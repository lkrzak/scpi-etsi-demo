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

/** Possible SCPI function results */
typedef enum{
	SCPI_OK = 0,
	SCPI_ERROR = 1,
}SCPIResult;

/** PER test result descriptor */
typedef struct{
	uint32_t testID;
	uint16_t totalPacketsNumber;
	uint16_t receivedPacketsNumber;
}SCPI_ETSI_TEST_PERTestResult;

/** PHY setting descriptor */
typedef struct{
	uint8_t phyNumber;
	uint16_t channelNumber;
	int8_t power;
	uint8_t signalType;
	uint8_t antennaNumber;
	uint16_t perTotalPacketsNumber;
	uint16_t perPacketLength;
}SCPI_ETSI_TEST_PhySettings;

/** PHY capabilities descriptor */
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

/** type definition of handler to channel lists for each PHY */
typedef uint32_t* SCPI_ETSI_TEST_PhyChannelList;

/** device descriptor */
typedef struct{
	uint8_t phyCount;
	const char* idn;
	SCPI_ETSI_TEST_PhySettings phySettings;
	const char** phyDescriptions;
	const SCPI_ETSI_TEST_PhyCapabilities* phyCapabilities;
	const SCPI_ETSI_TEST_PhyChannelList* phyChannelList;
}SCPI_ETSI_TEST_DeviceDescriptor;

/**
 *  Initializes the SCPI parser and used data structures.
 *
 *  @return SCPI_OK on success or SCPI_ERROR otherwise
*/
SCPIResult SCPI_ETSI_TEST_Init(void);

/**
 *  Tries to receive characters from data input. When full command is received
 *  calls parser function. Should be called periodically.
 *
 *  @return SCPI_OK on success or SCPI_ERROR otherwise
*/
SCPIResult SCPI_ETSI_TEST_Proc(void);

/**
 *  Sends given message into output using user's putchar method implementation
 *
 *  @param[in] data - address of data to send
 *  @param[in] size - number of bytes to send
*/
void SCPI_ETSI_TEST_Send(const void* data, size_t size);

#endif /* SCPI_ETSI_TEST_H_ */
