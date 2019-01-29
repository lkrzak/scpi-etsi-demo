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
 */
void SCPI_ETSI_TEST_USER_Reset();

#endif /* SCPI_ETSI_TEST_USER_H */
