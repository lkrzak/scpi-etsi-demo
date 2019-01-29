/**
@file
@license   $License$
@copyright $Copyright$
@version   $Revision$
@purpose   SCPI ETSI TEST demo application
@brief     Demo application showing how to use SCPI parser and SCPI ETSI TEST components
*/

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "scpi_etsi_test.h"
#include "scpi_etsi_test_user.h"

enum {
	// number of PHYs
	PHY_COUNT = 1,
};

/// Device identification string
static const char* idn = "RADIOMASTERS,RM-01,SN071284B,1.3.216";

/// Device capabilities structure
static const SCPI_ETSI_TEST_PhyCapabilities phyCapabilities[PHY_COUNT] = {
	{
		.lowestFrequency = 868000000,
		.highestFrequency = 868300000,
		.channelCount = 3,
		.channelBandwidth = 100000,
		.baudrate = 50000,
		.lowestPower = -10,
		.highestPower = 13,
		.minimalPacketLength = 12,
		.maximalPacketLength = 100,
		.modulationType = 1,
		.supportedSignals = 3,
		.antennaCount = 2,
		.defaultPhyNumber = 0,
		.defaultChannelNumber = 1,
		.defaultSignalType = 1,
		.defaultPower = 0,
		.defaultAntennaNumber = 0,
		.defaultPERTotalPacketsNumber= 15,
		.defaultPERPacketLength = 20,
	},
};

/// PHY descriptions
static const char* phyDescriptions[PHY_COUNT] = {
		"SUN-PHY-FSK 50kbps",
};

/// PHY channel lists
static uint32_t* channelList[PHY_COUNT] = {
	(uint32_t[]){868050000,868150000,868250000},
};

/**
 * THIS FUNCTION IS IMPLEMENTED BY THE USER.
 *
 * Gets a single character from the input stream to be passed to the SCPI parser.
 * @return character read from the input stream
 */
bool SCPI_ETSI_TEST_USER_GetChar(char *c) {
	*c = getchar();
	return true;
}

/**
 * THIS FUNCTION IS IMPLEMENTED BY THE USER.
 *
 * Sends a single character to the output stream.
 * @param[in] c character to be written to the output stream
 */
void SCPI_ETSI_TEST_USER_PutChar(char c) {
	putchar(c);
}

/**
 * THIS FUNCTION IS IMPLEMENTED BY THE USER.
 *
 * Initializes the device descriptor providing information about device capabilities, channel map and description.
 * @param[inout] deviceDescriptor pointer to the device descriptor structure
 */
void SCPI_ETSI_TEST_USER_Init(SCPI_ETSI_TEST_DeviceDescriptor* deviceDescriptor) {
	printf("Running SCPI_ETSI_TEST_USER_Init\n");
	if (deviceDescriptor) {
		deviceDescriptor->phyCount = PHY_COUNT;
		deviceDescriptor->idn = idn;
		deviceDescriptor->phyDescriptions = phyDescriptions;
		deviceDescriptor->phyCap = phyCapabilities;
		deviceDescriptor->phyChannelList = channelList;
	}
}

/**
 * THIS FUNCTION IS IMPLEMENTED BY THE USER.
 *
 * Resets the device.
 */
void SCPI_ETSI_TEST_USER_Reset() {
	printf("Running SCPI_ETSI_TEST_USER_Reset\n");
}

int main(void) {
	// Initialize the SCPI parser
	SCPI_ETSI_TEST_Init();
	printf("Ready for SCPI input (try entering \"PHY:CAP?\"):\n");
	while (1) {
		// Run the SCPI parser
		SCPI_ETSI_TEST_Proc();
	}
	return 0;
}
