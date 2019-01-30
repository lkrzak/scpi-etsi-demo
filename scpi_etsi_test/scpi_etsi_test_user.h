/**
@file
@license   $License$
@copyright $Copyright$
@version   $Revision$
@purpose   SCPI ETSI TEST parser
@brief     Declaration of the interface to be realized by the user
*/

#ifndef SCPI_ETSI_TEST_USER_H
#define SCPI_ETSI_TEST_USER_H

#include "scpi_etsi_test.h"

/**
 * THIS FUNCTION IS IMPLEMENTED BY THE USER.
 *
 * Gets a single character from the input stream to be passed to the SCPI parser.
 * @return character read from the input stream
 */
bool SCPI_ETSI_TEST_USER_GetChar(char *c);

/**
 * THIS FUNCTION IS IMPLEMENTED BY THE USER.
 *
 * Sends a single character to the output stream.
 * @param[in] c character to be written to the output stream
 */
void SCPI_ETSI_TEST_USER_PutChar(char c);

/**
 * THIS FUNCTION IS IMPLEMENTED BY THE USER.
 *
 * Initializes the device descriptor providing information about device capabilities, channel map and description.
 * @param[inout] deviceDescriptor pointer to the device descriptor structure
 */
void SCPI_ETSI_TEST_USER_Init(SCPI_ETSI_TEST_DeviceDescriptor* deviceDescriptor);

/**
 * THIS FUNCTION IS IMPLEMENTED BY THE USER.
 *
 * Resets the device.
 * @param[inout] deviceDescriptor pointer to the device descriptor structure
 */
void SCPI_ETSI_TEST_USER_Reset(SCPI_ETSI_TEST_DeviceDescriptor* deviceDescriptor);

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
bool SCPI_ETSI_TEST_USER_SetTRXMode(SCPI_ETSI_TEST_DeviceDescriptor* deviceDescriptor, uint8_t mode);

/**
 * THIS FUNCTION IS IMPLEMENTED BY THE USER.
 *
 * Starts Packet Error Rate (PER) test. This command is issued to the device being the source of packets in a PER test.
 * @param[inout] deviceDescriptor pointer to the device descriptor structure
 * @param[in] testID identification number of PER test
 * @return true on success, false otherwise
 */
bool SCPI_ETSI_TEST_USER_StartPERTest(SCPI_ETSI_TEST_DeviceDescriptor* deviceDescriptor, uint32_t testID);

/**
 * THIS FUNCTION IS IMPLEMENTED BY THE USER.
 *
 * Checks if there is an ongoing PER test.
 * @param[inout] deviceDescriptor pointer to the device descriptor structure
 * @return true when some PER test is running, false otherwise
 */
bool SCPI_ETSI_TEST_USER_IsPERTestRunning(SCPI_ETSI_TEST_DeviceDescriptor* deviceDescriptor);

/**
 * THIS FUNCTION IS IMPLEMENTED BY THE USER.
 *
 * Gets the result of a PER test.
 * @param[inout] deviceDescriptor pointer to the device descriptor structure
 * @return address of structure storing information about PER test result
 */
SCPI_ETSI_TEST_PERTestResult* SCPI_ETSI_TEST_USER_GetPERTestResult(SCPI_ETSI_TEST_DeviceDescriptor* deviceDescriptor);

#endif /* SCPI_ETSI_TEST_USER_H */
