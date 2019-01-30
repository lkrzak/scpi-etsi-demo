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

// flag informing that some PER test is running (when set)
static bool isPERTestOn;

// PER test result structure declaration
static SCPI_ETSI_TEST_PERTestResult testResult;

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
		deviceDescriptor->phyCapabilities = phyCapabilities;
		deviceDescriptor->phyChannelList = channelList;
	}
}

/**
 * THIS FUNCTION IS IMPLEMENTED BY THE USER.
 *
 * Resets the device.
 */
void SCPI_ETSI_TEST_USER_Reset(SCPI_ETSI_TEST_DeviceDescriptor* deviceDescriptor) {
	printf("Running SCPI_ETSI_TEST_USER_Reset\n");
}

/**
 * THIS FUNCTION IS IMPLEMENTED BY THE USER.
 *
 * Sets the transceiver mode of operation
 * 0 = TRX disabled, no reception and no transmissions (OFF)
 * 1 = enable radio transmission (TX)
 * 2 = enable radio reception (RX)
 * @param[inout] deviceDescriptor pointer to the device descriptor structure
 * @param[in] mode number describing transceiver operation mode
 * @return true on success, false otherwise
 */
bool SCPI_ETSI_TEST_USER_SetTRXMode(SCPI_ETSI_TEST_DeviceDescriptor* deviceDescriptor, uint8_t mode){
	// configuration of transceiver using data stored in phySettings structure
	if(mode == 0){
		printf("TRX off\n");
	}
	else if(mode == 1){
		printf("TRX enabled radio transmission (TX)\n");
	}
	else if(mode == 2){
		printf("TRX enabled radio reception (RX)\n");
	}
	return true;
}

/**
 * THIS FUNCTION IS IMPLEMENTED BY THE USER.
 *
 * Starts Packet Error Rate (PER) test. This command is issued to the device being the source of packets in a PER test.
 * @param[inout] deviceDescriptor pointer to the device descriptor structure
 * @param[in] testID identification number of PER test
 * @return true on success, false otherwise
 */
bool SCPI_ETSI_TEST_USER_StartPERTest(SCPI_ETSI_TEST_DeviceDescriptor* deviceDescriptor, uint32_t testID){
	// starting PER test using data stored in phySettings structure and given testID
	if(deviceDescriptor){
		testResult.testID = testID;
		testResult.totalPacketsNumber = deviceDescriptor->phySettings.perTotalPacketsNumber;
		isPERTestOn = true;
		return true;
	}
	return false;
}

/**
 * THIS FUNCTION IS IMPLEMENTED BY THE USER.
 *
 * Checks if there is an ongoing PER test.
 * @param[inout] deviceDescriptor pointer to the device descriptor structure
 * @return true when some PER test is running, false otherwise
 */
bool SCPI_ETSI_TEST_USER_IsPERTestRunning(SCPI_ETSI_TEST_DeviceDescriptor* deviceDescriptor){
	return isPERTestOn;
}

/**
 * THIS FUNCTION IS IMPLEMENTED BY THE USER.
 *
 * Gets the result of a PER test.
 * @param[inout] deviceDescriptor pointer to the device descriptor structure
 * @return address of structure storing information about PER test result
 */
SCPI_ETSI_TEST_PERTestResult* SCPI_ETSI_TEST_USER_GetPERTestResult(SCPI_ETSI_TEST_DeviceDescriptor* deviceDescriptor){
	// returning address of filled per test result data structure
	// it's not necessary to fill structure inside this method, if it has been done somewhere else
	testResult.receivedPacketsNumber = 973;
	return &testResult;
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
