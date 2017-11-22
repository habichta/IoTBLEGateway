//==============================================================================
// Project   :  BLE Gateway
// File      :  BLE_HELPER.h
// Author    :  Arthur Habicht
// Compiler  :  GCC C++ (requires -std=c++11 in compiler settings)
// Brief     :  Implements various Helper functions
//==============================================================================

#include "Headers/BLE_HELPER.h"

namespace BLEGateway {

//==============================================================================
void ConstructSubstrings(std::string str, std::string delimiter,
//==============================================================================
		std::vector<std::string> &result) {

	size_t pos = 0;
	std::string subs;
	while ((pos = str.find(delimiter)) != std::string::npos) {
		subs = str.substr(0, pos);

		if (!subs.empty()) {
			result.push_back(subs);
		}
		str.erase(0, pos + delimiter.length());
	}
	if (!str.empty()) {
		result.push_back(str);
	}

}
//==============================================================================
bool ConstructValidInteger(std::string str, int &i) {
//==============================================================================

	if (std::all_of(str.begin(), str.end(), ::isdigit)) {

		try {
			i = std::stoi(str, nullptr, 10);
		} catch (std::exception& e) {
			return false;
		}

		return true;
	} else {
		return false;
	}
}
//==============================================================================
bool ConstructValidMAC(std::string str, std::vector<uint8_t> &mac) {
//==============================================================================
	std::vector<std::string> macStr;
	std::string del(":");
	ConstructSubstrings(str, del, macStr);

	if (macStr.size() == MAC_SIZE) {

		for (auto byte : macStr) {

			if (byte.length() == 2
					&& std::all_of(byte.begin(), byte.end(), ::isxdigit)) {

				try {
					mac.push_back(std::stoi(byte, nullptr, 16));
				} catch (std::exception& e) {
					return false;
				}
			} else {
				return false;
			}

		}

	} else {

		return false;
	}

	return true;

}
//==============================================================================
bool IsValidInteger(std::string str) {
//==============================================================================

	if (std::all_of(str.begin(), str.end(), ::isdigit)) {

		return true;
	} else {
		return false;
	}
}

bool IsValidMAC(std::string str) {
	std::vector<std::string> macStr;
	std::string del(":");
	ConstructSubstrings(str, del, macStr);

	if (macStr.size() == MAC_SIZE) {

		for (auto byte : macStr) {

			if (byte.length() != 2 //Todo: length() might not return number of characters for some encodings!
			|| !std::all_of(byte.begin(), byte.end(), ::isxdigit)) {

				return false;

			}

		}

	} else {

		return false;
	}

	return true;

}

//Note: bd_addr saves data in little endian format, so to print go backwards
//==============================================================================
std::string MACToString(bd_addr mac) {
//==============================================================================

	std::stringstream ss;

	for (int i = MAC_SIZE - 1; i >= 0; i--) { //reversed!

		ss << std::setfill('0') << std::setw(2) << std::hex
				<< (int) mac.addr[i];

		if (i > 0) {

			ss << ":";
		}

	}

	return ss.str();

}


//==============================================================================
std::vector<uint8_t> HexStrToHexBuf(std::string str) {
//==============================================================================

	std::vector<uint8_t> bytes;



		if(str.length()%2 != 0){

			str = "0"+str;
		}

		std::string::iterator sit = str.begin();

		while(sit != str.end()){

			std::string byte(sit,sit+2);

			try{
			uint8_t uintb = (uint8_t)std::stoi(byte,nullptr,16);
			bytes.push_back(uintb);
			}catch(std::exception&e){

			}
			sit = sit+2;
		}

	return bytes;
}

}

