//==============================================================================
// Project   :  BLE Gateway
// File      :  BLE_HELPER.h
// Author    :  Arthur Habicht
// Compiler  :  GCC C++
// Brief     :  Implements various Helper functions
//==============================================================================

#ifndef CH_ETHZ_AH_BLEG_HELPER_H_
#define CH_ETHZ_AH_BLEG_HELPER_H_

//---------- Includes ----------------------------------------------------------

#include <string>
#include <vector>
#include <iostream>
#include "tpl.h"
#include "apitypes.h"
#include <algorithm>
#include <fstream>
#include "boost/date_time/posix_time/posix_time.hpp"





#define DEBUG 1

#define DebugPrint(fmt, ...) \
            do { if (DEBUG){ FILE* log; log=fopen("logfile.log", "a"); fprintf(log, fmt, __VA_ARGS__); fclose(log);}} while (0) //use fprintf instead of cerr because of thread-safety



//---------- Function Definitions ----------------------------------------------

namespace BLEGateway {

//==============================================================================
/**
 * @brief Creates a vector containing the substrings separated by the delimiter.
 * Empty substrings are not added to the vector
 *
 * @param[in] str
 * @param[in] delimiter
 * @param[out] result
 */
//==============================================================================
void ConstructSubstrings(std::string str, std::string delimiter,
		std::vector<std::string> & result);

//==============================================================================
/**
 * @brief Creates an integer after checking whether string contains valid
 * integer characters
 *
 * @param[in] str
 * @param[out] i
 * @return true if correct integer
 */
//==============================================================================
bool ConstructValidInteger(std::string str, int &i);

//==============================================================================
/**
 * @brief Creates a MAC Address after checking whether string contains a valid
 * MAC Address
 *
 * @param[in] str
 * @param[out] mac
 * @return true if correct MAC
 */
//==============================================================================
bool ConstructValidMAC(std::string str, std::vector<uint8_t> &mac);

//==============================================================================
/**
 * @brief Checks whether string contains valid integer characters
 *
 * @param[in] str
 * @return true if valid integer
 */
//==============================================================================
bool IsValidInteger(std::string str);

//==============================================================================
/**
 * @brief Checks whether string contains valid MAC Address
 *
 * @param[in] str
 * @return true if valid MAC
 */
//==============================================================================
bool IsValidMAC(std::string str);

//==============================================================================
/**
 * @brief Creates string representation of bd_addr (MAC Address)
 *
 * @param[in] mac (bd_addr contains MAC elements in little endian)
 * @return string representation of MAC in big endian
 */
//==============================================================================
std::string MACToString(bd_addr mac);


//==============================================================================
/**
 * @brief Creates byte array from strings with hex characters
 * Ignores characters which are not in the hexadecimal system
 * case-insensitive
 * @param[in] str Hex string
 * @return vector with byte values of hex string
 */
//==============================================================================
std::vector<uint8_t> HexStrToHexBuf(std::string str);
}
#endif /* CH_ETHZ_AH_BLEDG_HELPER_H_ */

