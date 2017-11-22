//==============================================================================
// Project   :  BLE Gateway
// File      :  BLE_DRIVER.h
// Author    :  Arthur Habicht
// Compiler  :  GCC G++
// Brief     :  Basic functions of a BLE Device instance running on the gateway
//==============================================================================

#ifndef CH_ETHZ_AH_BLEG_DRIVER_H_
#define CH_ETHZ_AH_BLEG_DRIVER_H_

//---------- Includes ----------------------------------------------------------
#include "cmd_def.h"
#include "BLE_HELPER.h"
#include "GATT_SPECIFICATIONS.h"
#include <stdio.h>

#include <exception>
#include <vector>
#include <set>
#include <map>
#include <unordered_map>
#include <iostream>
#include <time.h>
#include <algorithm>
#include <mutex>
#include <shared_mutex>
#include <boost/lockfree/queue.hpp>
#include "boost/variant.hpp"
#include <atomic>

#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>
#include <errno.h>
#include "tpl.h"
#include <stdlib.h>
#include <fstream>
#include <fcntl.h>
#include <sys/poll.h>
#include <chrono>
#include <ctime>

namespace BLEGateway {

//---------- Enum Definitions --------------------------------------------------

//Default values for discovery and connect
typedef enum DEFAULTS { //Todo: change to class enum, remove typedef

	DISCOVERY_SCAN_INTERVAL = 0x4B, //units of 625 microseconds
	DISCOVERY_SCAN_WINDOW = 0x32, //units of 625 microseconds
	DISCOVERY_ACTIVE_SCAN = 1, //true/false
	DISCOVERY_MODE = 1, //0: limited, 1:generic,2: observation
	DISCOVER_TIME = 0, // in seconds, 0 = no discovery just read available data
	CONNECT_ADDRESS_TYPE = 0x01, //default address type is random
	CONNECT_INTERVAL = 0x004c, // in 1.25 ms
	CONNECT_TIMEOUT = 0x0064, // in 10ms
	CONNECT_SAVE_LATENCY = 0x0, //nr

	EVENT_TIMEOUT = 200, //defines timeout until a command is aborted (in s) if f.e. the dongle does not respons with a certain event or response, the command is aborted and a Timeout operation error is returned

	CONNECTION_EVENT_TIMEOUT = 15 //timeout for connection attempts in seconds. If MAC does not exists, it tries to wait for a device with this MAC for f.e. 10 seconds

} Defaults;

//Advertisement data types
typedef enum GAP_ADTYPE {
	GAP_ADTYPE_FLAGS = 0x01, //Discovery Mode: -> GAP_ADTYPE_FLAGS_MODES
	GAP_ADTYPE_16BIT_MORE = 0x02, //Service: More 16-bit UUIDs available
	GAP_ADTYPE_16BIT_COMPLETE = 0x03, //Service: Complete list of 16-bit UUIDs
	GAP_ADTYPE_32BIT_MORE = 0x04, //Service: More 32-bit UUIDs available
	GAP_ADTYPE_32BIT_COMPLETE = 0x05, // Service: More 32-bit UUIDs available
	GAP_ADTYPE_128BIT_MORE = 0x06, //Service: More 128-bit UUIDs available
	GAP_ADTYPE_128BIT_COMPLETE = 0x07, //Service: Complete list of 128-bit UUIDs
	GAP_ADTYPE_LOCAL_NAME_SHORT = 0x08, //Shortened local name
	GAP_ADTYPE_LOCAL_NAME_COMPLETE = 0x09, // Complete local name
	GAP_ADTYPE_POWER_LEVEL = 0x0A, //TX Power Level: 0xXX: -127 to +127 dBm
	GAP_ADTYPE_OOB_CLASS_OF_DEVICE = 0x0D, //Simple Pairing OOB Tag: Class of device (3 octets)
	GAP_ADTYPE_OOB_SIMPLE_PAIRING_HASHC = 0x0E, //Simple Pairing OOB Tag: Simple Pairing Hash C (16 octets)
	GAP_ADTYPE_OOB_SIMPLE_PAIRING_RANDR = 0x0F, //Simple Pairing OOB Tag: Simple Pairing Randomizer R (16 octets)
	GAP_ADTYPE_SM_TK = 0x10, // Security Manager TK Value
	GAP_ADTYPE_SM_OOB_FLAG = 0x11, // Security Manager OOB Flags
	GAP_ADTYPE_SLAVE_CONN_INTERVAL_RANGE = 0x12, // Min and Max values of the connection interval (2 octets Min, 2 octets Max) (0xFFFF indicates no conn interval min or max)
	GAP_ADTYPE_SIGNED_DATA = 0x13, //Signed Data field
	GAP_ADTYPE_SERVICES_LIST_16BIT = 0x14, //Service Solicitation: list of 16-bit Service UUIDs
	GAP_ADTYPE_SERVICES_LIST_128BIT = 0x15, //Service Solicitation: list of 128-bit Service UUIDs
	GAP_ADTYPE_SERVICE_DATA = 0x16, //Service Data
	GAP_ADTYPE_APPEARANCE = 0x19, //Appearance
	GAP_ADTYPE_MANUFACTURER_SPECIFIC = 0xFF //Manufacturer Specific Data: first 2 octets contain the Company Identifier Code followed by the additional manufacturer specific data

} GAPAdtype;

typedef enum GAP_ADTYPE_FLAGS_MODES {
	GAP_ADTYPE_FLAGS_LIMITED = 0x01, //Discovery Mode: LE Limited Discoverable Mode
	GAP_ADTYPE_FLAGS_GENERAL = 0x02, //Discovery Mode: LE General Discoverable Mode
	GAP_ADTYPE_FLAGS_BREDR_NOT_SUPPORTED = 0x04 //Discovery Mode: BR/EDR Not Supported
} GAPAdtypeFlagsModes;

//Discovery packet types
typedef enum BGAPI_PACKETTYPE {
	DISCOVER_STANDARD = 0x00, DISCOVER_SCAN_RESPONSE = 0x04 //Additional 31 Bytes when Active Scan is enabled ( -> Scan Request)
} BGAPIPacketType;

//GATT Definitions/////////////////

typedef enum GATT_SERVICES_UUID {

	GENERIC_ACCESS_PROFILE = 0x1800, GENERIC_ATTRIBUTE_PROFILE = 0x1801

} GATTServicesUuid;

typedef enum GATT_ATTRIBUTE_TYPE_UUID {

	PRIMARY_SERVICE = 0x2800,
	SECONDARY_SERVICE = 0x2801,
	INCLUDE = 0x2802,
	CHARACTERISTIC = 0x2803

} GATTAttrTypeUuid;

typedef enum GATT_CHARACTERISTIC_DESCRIPTORS_UUID {

	CHARACTERISTIC_EXTENDED_PROPERTIES = 0x2900,
	CHARACTERISTIC_USER_DEFINITION = 0x2901,
	CLIENT_CHARACTERISTIC_CONFIGURATION = 0x2902,
	SERVER_CHARACTERISTIC_CONFIGURATION = 0x2903,
	CHARACTERISTIC_FORMAT = 0x2904,
	CHARACTERSITIC_AGGREGATE_FORMAT = 0x2905

} GATTCharDescriptUuid;

typedef enum GATT_CHARACTERISTIC_TYPES_UUID {

	DEVICE_NAME = 0x2A00,
	APPEARANCE = 0x2A01,
	PERIPHERAL_PRIVACY_FLAG = 0x2A02,
	RECONNECTION_ADDRESS = 0x2A03,
	PERIPHERAL_PREFERRED_CONNECTION_PARAM = 0x2A04,
	SERVICE_CHANGED = 0x2A05

} GATTCharTypeUuid;

typedef enum COMPLETION_EVENT { //do not mix up with BGAPI events, These are Gateway Internal events!
//numbers are bitflags, alows combination of events with OR
	GATT_PROCEDURE_COMPLETE_EVT = 0x1, //BGAPI procedure complete event
	GAP_CONNECTION_EVT = 0x2, //Connection Status event with new connect flags
	GAP_CONNECTION_RSP_EVT = 0x4, //response to Connect command
	GAP_DISCOVER_RSP_EVT = 0x8, //response to connect command
	CONNECTION_UPDATE_RSP_EVT = 0x10, //response to update command
	CONNECTION_UPDATE_EVT = 0x20, // event when connection parameters updated
	CONNECTION_DISCONNECT_RSP_EVT = 0x40, //response of disconnect command
	CONNECTION_DISCONNECT_EVT = 0x80, //disconnect event
	ATTCLIENT_READ_RSP_EVT = 0x100, // event when response to read command is returned
	ATTCLIENT_WRITE_RSP_EVT = 0x200, // response to write command
	ATTCLIENT_READGROUP_RSP_EVT = 0x400,
	ATTCLIENT_FINDINFO_RSP_EVT = 0x800,
	ATTCLIENT_READTYPE_RSP_EVT = 0x1000,
	GAP_ENDPROCEDURE_RSP_EVT = 0x2000,
	GAP_SET_SCANPARAMETER_RSP_EVT = 0x4000,
	SYS_GET_MAXCON_RSP_EVT = 0x8000,
	CONNECTION_GET_STATUS_RSP_EVT = 0x10000,
	SYS_GET_INFO_RSP_EVT = 0x20000,
	ATTCLIENT_ATTRIBUTE_FOUND_EVT = 0x40000

} CompletionEvent;

//---------- Struct/Type Definitions -------------------------------------------------

//==============================================================================
/**
 * @brief Superclass of structs in BLE Gateway driver
 * Used to enforce an interface for all sub-structs. Notably functions that allow
 * a HTML or JSON Visitor to create a corresponding representation. Since a struct can contain a struct, the visitor needs
 * to be able to deal with nested structs. Thus, a boost::variant is used.
 */
//==============================================================================
struct GatewayBaseStruct {

	virtual std::string GetId() = 0; //used for JSON root node for each struct
	virtual std::vector<
			std::pair<std::string,
					boost::variant<std::string, GatewayBaseStruct*> > > ToString() = 0;

	virtual ~GatewayBaseStruct() {
	}
	;

};

//Types used by the visitor for pretty print, see HTTP_HELPER for visitor example
using visitorValue = boost::variant<std::string, GatewayBaseStruct*>;
using visitorVector = std::vector<std::pair<std::string, visitorValue> >;
using visitorTuple = std::pair<std::string, visitorValue>;

//==============================================================================
/**
 * @brief BLE Dongle Information
 * Contains basic information about the BLE hardware which is used to connect to BLE Devices.
 */
//==============================================================================
struct BLEDeviceInfo: GatewayBaseStruct {

	BLEDeviceInfo() :
			d_major(0), d_minor(0), patch(0), build(0), ll_version(0), protocol_version(
					0), hw(0), maxConnections(0) {
	}

	uint16 d_major;
	uint16 d_minor;
	uint16 patch;
	uint16 build;
	uint16 ll_version;
	uint8 protocol_version;
	uint8 hw;
	uint8 maxConnections;

	std::string GetId() {
		return "BLEDeviceInfo";
	}

	visitorVector ToString() { //returns visitorVector, to be handled by a visitor

		visitorValue maj { std::to_string(d_major) };
		visitorValue min { std::to_string(d_minor) };
		visitorValue p { std::to_string(patch) };
		visitorValue b { std::to_string(build) };
		visitorValue lv { std::to_string(ll_version) };
		visitorValue pv { std::to_string(protocol_version) };
		visitorValue h { std::to_string(hw) };
		visitorValue mc { std::to_string(maxConnections) };

		visitorVector vv { visitorTuple("MAJOR", maj), visitorTuple("MINOR",
				min), visitorTuple("PATCH", p), visitorTuple("BUILD", b),
				visitorTuple("LL_VERSION", lv), visitorTuple("PROTOCOL_VERSION",
						pv), visitorTuple("HW", h), visitorTuple(
						"MAX_CONNECTIONS", mc) };

		return vv;

	}
	;

};

//==============================================================================
/**
 * @brief Payload of an Advertisement Packet
 * @Todo Still uses C-like syntax instead of an C++ STL Container
 */
//==============================================================================
struct AdvertisementPayload: GatewayBaseStruct {

	AdvertisementPayload() :
			length(0) {
	}

	uint8 length;
	uint8 data[BGAPI_PAYLOAD_SIZE]; //BGAPI payload has max of 60 bytes

	std::string GetId() {
		return "PAYLOAD";
	}

	visitorVector ToString() { //returns visitorVector, to be handled by a visitor

		std::stringstream ss;

		for (int i = 0; i < length; i++) {

			ss << std::setfill('0') << std::setw(2) << std::hex
					<< (int) data[i];

		}

		visitorVector vv { visitorTuple("DATA", ss.str()) };
		return vv;

	}

};

using AdvDataPayloadMap = std::map<uint8, AdvertisementPayload>;
//Type (GAP_ADTYPE) to payload

//==============================================================================
/**
 * @brief Metadata of an Advertisement Packet
 */
//==============================================================================
struct AdvertisementPacket: GatewayBaseStruct {

	AdvertisementPacket() :
			rssi(0), address_type(0), bond(0) {
	}

	int8 rssi;
	uint8 address_type;
	uint8 bond;
	AdvDataPayloadMap data;

	std::string GetId() {
		return "CONTENT";
	}

	visitorVector ToString() { //returns visitorVector, to be handled by a visitor

		visitorValue r { std::to_string(rssi) };
		visitorValue at { std::to_string(address_type) };
		visitorValue b { std::to_string(bond) };

		visitorVector vv { visitorTuple("RSSI", r), visitorTuple("ADDRESS_TYPE",
				at), visitorTuple("BOND", b) };

		for (AdvDataPayloadMap::iterator it = data.begin(); it != data.end();
				it++) {
			vv.push_back(
					visitorTuple(std::to_string((*it).first), &(*it).second));
		}

		return vv;
	}
};

//==============================================================================
/**
 * @brief Metadata of an Advertisement Packet
 * Differentiates between type 0x00 and 0x04 Packets. 0x04 Packets are
 * received if Discovery Scan Mode is "Active"
 */
//==============================================================================
struct AdvertisementData: GatewayBaseStruct {

	bd_addr_t macAddress;
	AdvertisementPacket discoverDefaultPacket; //0x00 Packettype
	AdvertisementPacket scanResponsePacket; //0x04 Packettype

	std::string GetId() {
		return MACToString(macAddress);
	}

	visitorVector ToString() { //returns visitorVector, to be handled by a visitor
		visitorValue mac { MACToString(macAddress) };
		visitorValue ddp { &discoverDefaultPacket };
		visitorValue srp { &scanResponsePacket };

		visitorVector vv { visitorTuple("MAC_ADDRESS", mac), visitorTuple(
				"0x00", ddp), visitorTuple("0x04", srp) };

		return vv;

	}

};

//==============================================================================
/**
 * @brief UUID Representation
 * Saved in little endian (reversed) order
 * @Todo still uses C-like Syntax instead of C++ STL Container
 */
//==============================================================================
struct Uuid: GatewayBaseStruct {
//Little endian format

	Uuid() :
			length(0) {
	}

	size_t length; //Note: This is a "bit" C-ish. It should be changed to a more C++ way of using Arrays...
	uint8_t addr[UUID_SIZE];

	std::string GetId() {
		return "UUID";
	}

	visitorVector ToString() { //returns visitorVector, to be handled by a visitor

		std::stringstream ss;
		for (unsigned int i = 0; i < length; i++) {
			ss << std::setfill('0') << std::setw(2) << std::hex
					<< (int) addr[i];
		}

		visitorValue u { ss.str() };

		visitorVector vv { visitorTuple("VALUE", u) };

		return vv;

	}

};

//==============================================================================
/**
 * @brief Data of an BLE Attribute
 * The serviceUuid is only set if the Attribute is of Type 0x2800 Primary Services
 * (with a "unique" 16bit UUID).
 */
//==============================================================================
struct AttributeData: GatewayBaseStruct {

	AttributeData() :
			timestamp(boost::posix_time::second_clock::universal_time()), connectionHandle(
					0), description(""), attributeType(0), attributeHandle(0), endHandle(
					0) {
	}
	boost::posix_time::ptime timestamp; //last update of cache. Note that only read operations update the cached value (AttributeValue and InfoFound Events)
	Uuid serviceUuid; //this is the "unique" 16 byte UUID for the service (only defined for handles that point to a service attribute) for services the field uuid only contains the type UUID (2800). It is 0 for non-service attributes
	Uuid uuid; //2800: primary, 2801:secondary, 2802: include, 2803: characteristic. Note: Characteristics have a handle with a type UUID (Descriptor). The subsequent handle contains the unique 16byte UUID and the device data as raw value
	uint8_t connectionHandle; //this is not the service handle! it is the handle of the device connection
	std::string description;

	uint8_t attributeType;
	uint16_t attributeHandle; // also group start handle
	uint16_t endHandle; //last handle that belongs to this service (Group end)

	std::vector<uint8_t> rawData;

	std::string GetId() {
		return "ATTRIBUTE";
	}

	visitorVector ToString() { //returns visitorVector, to be handled by a visitor

		visitorValue su { &serviceUuid };
		visitorValue u { &uuid };
		visitorValue ch { std::to_string(connectionHandle) };
		visitorValue d { description };
		visitorValue ah { std::to_string(attributeHandle) };
		visitorValue eh { "" };
		if (endHandle != 0) {
			visitorValue eh = { std::to_string(endHandle) };
		}

		std::stringstream time;
		time << timestamp.date().day_of_week() << ", " << timestamp.date().day()
				<< " " << timestamp.date().month() << " " << timestamp.date().year() << " "
				<< timestamp.time_of_day() << " GMT";
		visitorValue t { time.str() };

		std::stringstream ss;
		for (auto &d : rawData) {
			ss << std::setfill('0') << std::setw(2) << std::hex << (int) d;
		}

		visitorValue data { ss.str() };

		visitorVector vv { visitorTuple("LAST_UPDATE", t), visitorTuple(
				"SERVICE_UUID", su), visitorTuple("TYPE_UUID", u), visitorTuple(
				"CONNECTION_HANDLE", ch), visitorTuple("DESCRIPTION", d),
				visitorTuple("START_HANDLE", ah), visitorTuple("END_HANDLE",
						eh), visitorTuple("DATA", data) };

		return vv;

	}

};

using AttributeMap = std::map<uint16_t, AttributeData>;
//attribute handle -> Attribute struct
//assumed to be an ordered map for ordered printing by HTML/JSON visitors

//==============================================================================
/**
 * @brief Overview of Connected Device
 * Contains all the metadata of the connected device and an ordered map which contains
 * an attribute handle to attribute data mapping
 */
//==============================================================================
struct ConnectedDeviceInfo: GatewayBaseStruct {

	ConnectedDeviceInfo() :
			connectionHandle(0), connectionFlags(0), addressType(0), connInterval(
					0), timeout(0), slaveLatency(0), bonding(0) {
	}
	bd_addr macAddress; //Bluetooth device address
	uint8_t connectionHandle; //handle of the attribute at GATT Server
	uint8_t connectionFlags; //bit0: 1 if connection exists, bit1: 1 if connection is encrypted, bit2: 1 if new connection has been created, bit3: 1 if connection parameters have changed due to link layer operation
	uint8_t addressType;  //Public or Random Bluetooth address
	uint16_t connInterval; //interval between connection events
	uint16_t timeout; // how many ms the device can be out of range until disconnect
	uint16_t slaveLatency; // how many connection events a slave can ignore if no new data available
	uint16_t bonding; // bonding handle
	AttributeMap attributeMap; //attributes of this device

	std::string GetId() {
		return MACToString(macAddress);
	}

	visitorVector ToString() { //returns visitorVector, to be handled by a visitor

		visitorValue mac { MACToString(macAddress) };
		visitorValue ch { std::to_string(connectionHandle) };
		visitorValue cf { std::to_string(connectionFlags) };
		visitorValue at { std::to_string(addressType) };
		visitorValue ci { std::to_string(connInterval) };
		visitorValue t { std::to_string(timeout) };
		visitorValue sl { std::to_string(slaveLatency) };
		visitorValue b { std::to_string(bonding) };

		visitorVector vv { visitorTuple("MAC_ADDRESS", mac), visitorTuple(
				"CONNECTION_HANDLE", ch), visitorTuple("CONNECTION_FLAGS", cf),
				visitorTuple("ADDRESS_TYPE", at), visitorTuple(
						"CONNECTION_INTERVAL", ci), visitorTuple("TIMEOUT", t),
				visitorTuple("SLAVE_LATENCY", sl) };

		for (AttributeMap::iterator it = attributeMap.begin();
				it != attributeMap.end(); it++) {
			vv.push_back(
					visitorTuple(std::to_string((*it).first), &(*it).second));
		}

		return vv;

	}

};

//==============================================================================
/**
 * @brief Struct for internal event handling
 * Used to synchronize asynchronous processes by notifying interested observes if
 * a certain COMPLETION_EVENT occurred.
 */
//==============================================================================
struct ConnectionEventStruct { //used for observer pattern to notify events for specific connections
	ConnectionEventStruct() :
			eventHandle(nullptr), connectionHandle(-1), errorCode(0x0003) {
	}

	int* eventHandle; //value which is used to notify if an event occured
	int connectionHandle; //returns 0 to 255 if a connectionHandle exists for a specific MAC Address, negative if no connection handle found
	int errorCode; //0 if no errors returned by a driver function

};

//==============================================================================
/**
 * @brief Contains results of a driver operation
 * Used for the communication between driver and gateway logic. A Result struct is
 * returned by all interface functions to inform the gateway about upstream errors
 * (f.e. a device MAC Address could not be detected while trying to connect). It also contains pointers
 * to data which was changed by an interface function so that the gateway can access it.
 * The connectionHandle is set to -1 if it is unknown. Only existing connections
 * already have a handle. If f.e. the gateway issues a connect command, the connectionHandle
 * will be -1.
 */
//==============================================================================
struct Result {

	Result() :
			errorCode(0), connectionHandle(0), errorMessage("") {
	}

	int errorCode;
	int connectionHandle; //to address a certain connection on which an action was performed
	std::string errorMessage;
	std::vector<GatewayBaseStruct*> data; //should be const pointer...

};

//==============================================================================
/**
 * @brief Strict weak ordering of MAC Addresses
 * Used for maps which have MAC Addresses as keys.
 */
//==============================================================================
struct CompareMAC { //strict weak ordering for Maps!
	bool operator ()(bd_addr const& lhs, bd_addr const& rhs) const //in order to use mac address as key in map, a weak strict ordering of the struct for operator < needs to be defined
			{
		int i;
		for (i = 0; i < 6; i++) {
			if (lhs.addr[i] < rhs.addr[i]) {
				return true;
			} else if (lhs.addr[i] > rhs.addr[i]) {
				return false;
			}
		}
		return false; // all equal
	}
};

//Overload == for UUIDs
inline bool operator==(const Uuid& lhs, const Uuid& rhs) {

	if (lhs.length != rhs.length) {
		return false;
	}

	for (size_t i = 0; i < lhs.length; i++) {

		if (lhs.addr[i] != rhs.addr[i]) {
			return false;
		}

	}

	return true;

}
//Overload == for MAC Addresses
inline bool operator==(const bd_addr_t& lhs, const bd_addr_t& rhs) {

	for (int i = 0; i < MAC_SIZE; i++) {

		if (lhs.addr[i] != rhs.addr[i]) {
			return false;
		}

	}

	return true;

}
using DiscoveredDeviceMap = std::map<bd_addr_t, AdvertisementData, CompareMAC>;
// MAC -> Advertisement data
using ConnectedDeviceMap = std::map<uint8_t, ConnectedDeviceInfo>;
// connection Handle -> ConnectedDevice data
//Both maps could be unordered for higher performance. However, for the ConnectedDeviceMap the html/json printing assumes that the keys are ordered according to connection handle.
//so the output might occur in any order if it was unordered.

//---------- Class Definitions -------------------------------------------------------------------

// Abstract interface for BLE Central Devices

//==============================================================================
/**
 *
 * @brief Superclass of Bluetooth Low Energy (BLE) Devices
 *
 * Contains basic fields and functions to manage BLE Devices and acts as an interface for application software
 */
class Device
//==============================================================================

{
protected:
	int clientSocketDesc; //File descriptor of TCP Socket
	char clientIPv4[INET_ADDRSTRLEN]; //IPv4 Address of accepted Client
	unsigned short clientPort; //Port of accepted Client
	void SetClientSocketDesc(int clientSocketDesc);
	void SetClientPort(sockaddr_in clientConnectionData);
	void SetClientIPv4Address(sockaddr_in clientConnectionData);

	BLEDeviceInfo bledInfo;
	DiscoveredDeviceMap discoveredDevices;
	ConnectedDeviceMap connectedDevices;

public:

	std::atomic<bool> isConnected { false }; // checks if the Dongle is still connected through
	//a TCP connection (when EOF is read when processing TCP Messages, this is set to false)

	//==============================================================================
	/**
	 * @brief Reads from TCP Socket. Non-Blocking using poll()
	 */
	//==============================================================================
	virtual void ProcessTCPMsgFromSocket()=0;

	//==============================================================================
	/**
	 * @brief Write to TCP Socket
	 * @param[in] buffer The memory buffer which contains the data
	 * @param[in] length Size of data in bytes
	 */
	//==============================================================================
	virtual void WriteTCPMsgToSocket(char* buffer, size_t length) = 0;

	//==============================================================================
	/**
	 * @brief Read bytes from TCP Socket
	 * Makes sure that the specified numbers of bytes are actually read in case data
	 * is still in transit
	 * @param[in,out] buffer The memory buffer which receives the data
	 * @param[in] length Size of data to be read (in bytes)
	 * @return Number of bytes read
	 */
	//==============================================================================
	virtual int ReadBytesFromTCPSocket(char* buffer, int length) = 0;

	//==============================================================================
	/**
	 * @brief Decodes TCP Message
	 * Reads the command ID of a received TCP message and calls the corresponding driver functions
	 * @param[in] buffer The memory buffer which contains the message
	 * @param[in] length Size of data in bytes
	 */
	//==============================================================================
	virtual void DecodeIncomingTCPMessage(char* buffer, uint32_t length) = 0;

	//==============================================================================
	/**
	 * @brief Command: Update BLED112 information struct
	 */
	//==============================================================================
	virtual Result UpdateBledInformation() = 0;

	//==============================================================================
	/**
	 * @brief Command: Read a specific BLE attribute by handle
	 * @param[in] macAddress Address of the connected device
	 * @param[in] attributeHandle Handle of the attribute
	 * @param[in] lastUpdate max time until cached value is outdated (in seconds)
	 * Do not return cached value if outdated
	 */
	//==============================================================================
	virtual Result ReadAttribute(std::vector<uint8> macAddress,
			uint16_t attributeHandle, int lastUpdate) = 0;

	//==============================================================================
	/**
	 * @brief Command: Write to a specific BLE attribute by handle
	 * Write can transmit 20 bytes at maximum
	 * @param[in] macAddress Address of the connected device
	 * @param[in] attributeHandle Handle of the attribute
	 * @param[in] data String of data to be written to handle, this needs to be a string of hex characters
	 * @return Result struct
	 */
	//==============================================================================
	virtual Result WriteAttribute(std::vector<uint8> macAddress,
			uint16_t attributeHandle, std::string data)=0;

	//==============================================================================
	/**
	 * @brief Command: Initiate BLE Discovery
	 *
	 * Starts discovery mode with given parameters. Calls GapEndProcedure command after
	 * displayTime seconds.
	 * Note: scanWindow <= scanInterval
	 * @param[in] discoverMode 0:limited, 1:generic, 2:observation, default:generic
	 * @param[in] scanInterval In units of 625 microseconds, default:75ms (0x4B) must be between 0x4 and 0x4000
	 * @param[in] scanWindow In units of 625 microseconds, default 50ms (0x32)  must be between 0x4 and 0x4000
	 * @param[in] activeScan 1: request additional 0x4 Scan Response Packet, 0: request only normal 0x0 Packet, default:0
	 * @param[in] discoverTime Duration of discovery in seconds. Value = 0 just displays cached data.
	 * @return Result struct
	 */
	//==============================================================================
	virtual Result Discover(gap_discover_mode discoverMode,
			unsigned short scanInterval, unsigned short scanWindow,
			bool activeScan, unsigned short discoverTime) = 0;

	//==============================================================================
	/**
	 * @brief Command: Direct connect to device or update device parameters
	 * Connects if device is not already connected, otherwise updates the connection parameters.
	 * Result struct connectionHandle = -1 in case a new device was connected, otherwise
	 * connectionHandle is between 0 and 255.
	 * Note that connectionIntervalMin is not used in this implementation (only connectionIntervalMax)
	 * Note: slaveLatency*connectionInterval <= timeout
	 * Note: connectionIntervalMin <= connectionIntervalMax
	 *
	 * @param[in] macAddress vector of bytes.
	 * @param[in] addressType 0: Public, 1: Random, default: 0
	 * @param[in] connectionIntervalMin: in units of 1.25ms must be between 6 and 3200 (7.5ms to 4000ms), default: 60
	 * @param[in] connectionIntervalMax: in units of 1.25ms must be between 6 and 3200 (7.5ms to 4000ms), default: 60
	 * @param[in] timeout Supervision timeout. Time devices can be out of range until connection is closed. In units of 10ms, must be between 10 and 3200, default:100
	 * @param[in] slaveLatency 0: disabled, must be between 0 and 500, default: 0
	 * @return Result struct
	 */
	//==============================================================================
	virtual Result Connect(std::vector<uint8> macAddress, uint8_t addressType,
			uint16_t connectionIntervalMin, uint16_t connectionIntervalMax,
			uint16_t timeout, uint16_t slaveLatency) = 0;

	//==============================================================================
	/**
	 * @brief Command: Disconnect from device
	 * @param[in] macAddress vector of bytes.
	 * @return Result struct
	 */
	//==============================================================================
	virtual Result Disconnect(std::vector<uint8> macAddress) = 0;

	//==============================================================================
	/**
	 * @brief Command: Read all attributes of a connected device
	 * @param[in] macAddress vector of bytes.
	 * @return Result struct
	 */
	//==============================================================================
	virtual Result ReadAllAttributes(std::vector<uint8> macAddress) = 0;

	//==============================================================================
	/**
	 * @brief Command: Read data of all connected devices
	 * @return Result struct
	 */
	//==============================================================================
	virtual Result GetConnectedDevices() = 0;

	//==============================================================================
	/**
	 * @brief Command: Get info from the BLE Dongle
	 * Note that this reads the cached information and is only updated when the dongle
	 * is reconnected
	 * @return BLEDeviceInfo pointer
	 */
	//==============================================================================
	virtual BLEDeviceInfo* GetBLEDInfo() = 0;

	//==============================================================================
	/**
	 * @brief Command: Create error string from error code
	 * @param[in] errorCode
	 * @return string
	 */
	//==============================================================================
	virtual std::string CreateErrorMessage(int errorCode)= 0;

	virtual ~Device() {
	}
	;

//==============================================================================
	//Getter/Setter Functions
	int GetClientSocketDesc();
	char* GetIPv4Address();
	unsigned short GetClientPort();

	AttributeData* GetAttribute(uint8_t connectionHandle,
			uint16_t attributeHandle);
	ConnectedDeviceInfo* GetConnectionInfo(uint8_t connectionHandle);
	void GetAdvertismentData(std::vector<GatewayBaseStruct*>* advVector);
	uint8_t GetConnectedDevCount();

};

// Definition of concrete BlueGiga BLED112 driver
//==============================================================================

/**
 * @brief Representation of a BlueGiga BLED112 Device
 *
 *Implements relevant functions to manage/control a Bluegiga BLED112 Device in the context of a gateway application
 *The implementation follows the specifications of BGAPI to communicate with the BLED112 over a TCP connection.
 *The basic architecture looks as follows:
 \dot
 digraph G{
 Gateway_Application -> BLE_DRIVER [label="uses"];
 BLE_DRIVER -> C_BLED_Program [label="TCP connection"] ;
 C_BLED_Program -> BLED112_Device [label = "Serial connection"];
 }
 \enddot
 */

//==============================================================================
class BLED112: public Device
//==============================================================================

{

private:

	std::vector<std::pair<int, ConnectionEventStruct*> > completionEventRegister; //Observer Pattern. function that busy wait until a completion  event occurs register their condition variable. Procedure completed event will deregister the variable and set them to true. Better than global
	//variable because not all commands that produce a procedure completed event necessarily need to wait for it. This procedure comp. event would set global variable to true without setting it to false afterwards which could lead to serious errors if
	//afterwards a function is called which polls this global variable. Used with waitForProcedureCompleted function.
	//Assumes BGAPI Command/Response/Event architecture
public:

	BLED112(int clientSocketDesc, sockaddr_in clientConnectionData);
	~BLED112() {
	}
	;

	//Implementation of Interface
	//==============================================================================
	void ProcessTCPMsgFromSocket();
	//==============================================================================

	//==============================================================================
	void WriteTCPMsgToSocket(char* buffer, size_t length);
	//==============================================================================

	//==============================================================================
	int ReadBytesFromTCPSocket(char* buffer, int length);
	//==============================================================================

	//==============================================================================
	void DecodeIncomingTCPMessage(char* buffer, uint32_t length);
	//==============================================================================


	//==============================================================================
	Result UpdateBledInformation();
	//==============================================================================


	//==============================================================================
	Result ReadAttribute(std::vector<uint8> macAddress,
			uint16_t attributeHandle, int lastUpdate);
	//==============================================================================

	//==============================================================================
	Result WriteAttribute(std::vector<uint8> macAddress,
			uint16_t attributeHandle, std::string data);
	//==============================================================================

	//==============================================================================
	Result Discover(gap_discover_mode discoverMode, unsigned short scanInterval,
			unsigned short scanWindow, bool activeScan,
			unsigned short discoverTime);
	//==============================================================================

	//==============================================================================
	Result Connect(std::vector<uint8> macAddress, uint8_t addressType,
			uint16_t connectionIntervalMin, uint16_t connectionIntervalMax,
			uint16_t timeout, uint16_t slaveLatency);
	//==============================================================================

	//==============================================================================
	Result Disconnect(std::vector<uint8> macAddress);
	//==============================================================================

	//==============================================================================
	Result ReadAllAttributes(std::vector<uint8> macAddress);
	//==============================================================================

	//==============================================================================
	Result GetConnectedDevices();
	//==============================================================================

	//==============================================================================
	BLEDeviceInfo* GetBLEDInfo();
	//==============================================================================

	//==============================================================================
	std::string CreateErrorMessage(int errorCode);
	//==============================================================================

	//Internal Driver Commands for communication with BlueGiga BLED112----------------------------------

	//==============================================================================
	/**
	 * @brief Command: Sends simple commands with no additional parameter but the command index
	 * @param[in] commandIndex commandindex as given by cmd_def.h
	 * @param[in] serializeFormat format for TPL serialization
	 * @Todo serializeFormat is probably obsolete
	 */
	//==============================================================================
	void SendSimpleCommand(uint16_t commandIndex, const char* serializeFormat);

	//==============================================================================
	/**
	 * @brief Command: Serialize and transmit GetInfo Command
	 * Spawns a getInfo response
	 */
	//==============================================================================
	void BleCmdSystemGetInfo();

	//==============================================================================
	/**
	 * @brief Command: Serialize and transmit GapEndProcedure Command
	 * Spawns a EndProcedure response
	 */
	//==============================================================================
	void BleCmdGapEndProcedure();

	//==============================================================================
	/**
	 * @brief Command: Serialize and transmit GetMaxConnections Command
	 * Spawns a GetConnections response
	 * Spawns a ConnectionGetStatus event (connection status flag: connection_completed = 0)
	 */
	//==============================================================================
	void BleCmdSystemGetMaxConnections();

	//==============================================================================
	/**
	 * @brief Command: Serialize and transmit Discovery Command
	 *
	 * Spawns a Discovery response
	 * Spawns a ScanResponse event (type 0x0 and/or 0x4)
	 * Note: scanWindow <= scanInterval
	 * @param[in] discoverMode 0:limited, 1:generic, 2:observation
	 * @param[in] scanInterval In units of 625 microseconds, default:75ms (0x4B) must be between 0x4 and 0x4000
	 * @param[in] scanWindow In units of 625 microseconds, default 50ms (0x32)  must be between 0x4 and 0x4000
	 * @param[in] activeScan 1: request additional 0x4 Scan Response Packet, 0: request only normal 0x0 Packet
	 */
	//==============================================================================
	void BleCmdGapDiscover(gap_discover_mode discoverMode,
			unsigned short scanInterval, unsigned short scanWindow,
			bool activeScan);

	//==============================================================================
	/**
	 * @brief Command:  Serialize and transmit direct connect Command
	 * Note: slaveLatency*connectionInterval <= timeout
	 * Note: connectionIntervalMin <= connectionIntervalMax
	 *
	 * Spawns a ConnectDirect response
	 * Spawns a ConnectionGetStatus event (connection status flag: connection_completed = 1 if successful)
	 *
	 * @param[in] macAddress vector of bytes.
	 * @param[in] addressType 0: Public, 1: Random
	 * @param[in] connectionIntervalMin: in units of 1.25ms must be between 6 and 3200 (7.5ms to 4000ms)
	 * @param[in] connectionIntervalMax: in units of 1.25ms must be between 6 and 3200 (7.5ms to 4000ms)
	 * @param[in] timeout Supervision timeout. Time devices can be out of range until connection is closed. In units of 10ms, must be between 10 and 3200
	 * @param[in] slaveLatency 0: disabled, must be between 0 and 500
	 */
	//==============================================================================
	void BleCmdGapConnectDirect(bd_addr macAddress, uint8 addressType,
			uint16 connectionIntervalMin, uint16 connectionIntervalMax,
			uint16 timeout, uint16 slaveLatency);

	//==============================================================================
	/**
	 * @brief Command: Serialize and transmit GetStatus Command
	 *
	 * Spawns a ConnectionGetStatus response
	 * Spawns a ConnectionGetStatus event (connection status flag: connection_completed = 0)
	 *
	 * @param[in] connectionHandle
	 */
	//==============================================================================
	void BleCmdConnectionGetStatus(uint8 connectionHandle);

	//==============================================================================
	/**
	 * @brief Command: Serialize and transmit Update Command
	 *
	 * Send parameter update request to the Bluetooth link layer
	 * Spawns an Update response
	 *
	 * Note: slaveLatency*connectionInterval <= timeout
	 * Note: connectionIntervalMin <= connectionIntervalMax
	 * @param[in] connectionHandle
	 * @param[in] connectionIntervalMin: in units of 1.25ms must be between 6 and 3200 (7.5ms to 4000ms)
	 * @param[in] connectionIntervalMax: in units of 1.25ms must be between 6 and 3200 (7.5ms to 4000ms)
	 * @param[in] timeout Supervision timeout. Time devices can be out of range until connection is closed. In units of 10ms, must be between 10 and 3200
	 * @param[in] slaveLatency 0: disabled, must be between 0 and 500
	 */
	//==============================================================================
	void BleCmdConnectionUpdate(uint8 connectionHandle, uint16 connIntervalMin,
			uint16 connIntervalMax, uint16 slaveLatency, uint16 timeout);

	//==============================================================================
	/**
	 * @brief Command: Serialize and transmit Disconnect Command
	 *
	 * Spawns a Disconnect response
	 * Spawns a ConnectionDisconnect event
	 *
	 * @param[in] connectionHandle
	 */
	//==============================================================================
	void BleCmdConnectionDisconnect(uint8 connectionHandle);

	//==============================================================================
	/**
	 * @brief Command: Serialize and transmit FindPrimaryServices Command
	 * Retrieves all primary services and their UUIDs (16bit)
	 *
	 * spawns a readByType response
	 * spawns a GroupFound event
	 * spawns a ProcedureCompleted event
	 *
	 * @param[in] connectionHandle
	 */
	//==============================================================================
	void BleCmdGattFindPrimaryServices(uint8 connectionHandle);

	//==============================================================================
	/**
	 * @brief Command: Serialize and transmit FindCharacteristics Command
	 *
	 *
	 * spawns a readByType response
	 * spawns a GroupFound event
	 * spawns a ProcedureCompleted event
	 *
	 * @param[in] connectionHandle
	 * @param[in] startHandle minimum:0
	 * @param[in] endHandle maximum: 65535
	 * @Todo not tested yet
	 */
	//==============================================================================
	void BleCmdGattFindCharacteristics(uint8 connectionHandle,
			uint16 startHandle, uint16 endHandle); //Only finds char. UUID. Not used in this implementation -> use FindAttributeInfo instead. reason: find info already finds all info including descriptors

	//==============================================================================
	/**
	 * @brief Command: Serialize and transmit FindAttributeInfo Command
	 * Retrieves all attributes in range startHandle to endHandle
	 *
	 * spawns a findInformation response
	 * spawns a AttributeInfoFound event
	 * spawns a ProcedureCompleted event
	 *
	 * @param[in] connectionHandle
	 * @param[in] startHandle minimum:0
	 * @param[in] endHandle maximum: 65535
	 *
	 */
	//==============================================================================
	void BleCmdGattFindAttributeInfo(uint8 connectionHandle, uint16 startHandle,
			uint16 endHandle); //finds all descriptors, typeUUID and charUUID

	//==============================================================================
	/**
	 * @brief Command: Serialize and transmit ReadAttributeByHandle Command
	 * Read raw data of an attribute
	 *
	 * spawns a readByHandle response
	 * spawns a AttributeValue event
	 * spawns a ProcedureCompleted event
	 *
	 * @param[in] connectionHandle
	 * @param[in] attributeHandle
	 *
	 */
	//==============================================================================
	void BleCmdGattReadAttributeByHandle(uint8 connectionHandle,
			uint16 attributeHandle);

	//==============================================================================
	/**
	 * @brief Command: Serialize and transmit WriteAttributeByHandle Command
	 * Write up to 20 bytes to attribute
	 *
	 * spawns a write response
	 * BlueGiga API seems to be wrong here, does NOT spawn a Procedure Completed event
	 *
	 * @param[in] connectionHandle
	 * @param[in] attributeHandle
	 *
	 */
	//==============================================================================
	void BleCmdGattWriteAttributeByHandle(uint8 connectionHandle,
			uint16 attributeHandle, char* dataBuffer, uint32_t length);

//---------------------------------------------------------------------------------------
	/*
	 * Driver Callbacks: These are called after Decoding a TCP Message
	 * BGAPI defines responses and events. Responses of a certain command
	 * are acknowledgments by the Dongle and always send before an event spawned by
	 * that command (they contain error codes and if available the connection handle
	 * and/or attribute handle). However, responses and events from different commands
	 * can interleave. Events can be spawned at any time asynchronously and contain
	 * the data sent by the BLE devices (f.e. attributes of a sensor).
	 */

	void BleSimpleResponse(char* msgData, uint32_t length);
	void BleSimpleConnectionResponse(char* msgData, uint32_t length);
	void BleRspSystemGetInfo(char* msgData, uint32_t length);
	void BleRspGapConnectDirect(char* msgData, uint32_t length);
	void BleRspConnectionGetStatus(char* msgData, uint32_t length);
	void BleRspSystemGetMaxConnections(char* msgData, uint32_t length);
	void BleEvtGapScanResponse(char* msgData, uint32_t length);
	void BleEvtConnectionGetStatus(char* msgData, uint32_t length);
	void BleEvtConnectionDisconnect(char* msgData, uint32_t length);
	void BleEvtGattServiceFound(char* msgData, uint32_t length);
	void BleEvtGattProcedureCompleted(char* msgData, uint32_t length);
	void BleEvtGattAttributeValue(char* msgData, uint32_t length); //returned by read functions of attr client
	void BleEvtGattAttributeInfoFound(char* msgData, uint32_t length);

	void BleRspConnectionDisconnect(char* SerializeMsgData, uint32_t length);
	//---------- Helper Functions --------------------------------------------------

	//==============================================================================
	/**
	 * @brief Checks validity of Connect Parameters and sets them to default if they are illegal

	 * @param[in] connectionIntervalMin in units of 1.25ms must be between 6 and 3200 (7.5ms to 4000ms)
	 * @param[in] connectionIntervalMax in units of 1.25ms must be between 6 and 3200 (7.5ms to 4000ms)
	 * @param[in] timeout Supervision timeout. Time devices can be out of range until connection is closed. In units of 10ms, must be between 10 and 3200
	 * @param[in] slaveLatency 0: disabled, must be between 0 and 500
	 * @return true if parameters were correct, false otherwise
	 */
	//==============================================================================
	bool CheckConnectionParameters(uint16_t& connIntervalMin,
			uint16_t& connIntervalMax, uint16_t& timeout,
			uint16_t& slaveLatency);

	//==============================================================================
	/**
	 * @brief Check Discover Parameters and set the to default if they are illegal

	 * @param[in] discoverMode 0:limited, 1:generic, 2:observation
	 * @param[in] scanInterval In units of 625 microseconds, default:75ms (0x4B) must be between 0x4 and 0x4000
	 * @param[in] scanWindow In units of 625 microseconds, default 50ms (0x32)  must be between 0x4 and 0x4000
	 * @param[in] activeScan 1: request additional 0x4 Scan Response Packet, 0: request only normal 0x0 Packet
	 * @return true if parameters were correct, false otherwise
	 */
	//==============================================================================
	bool CheckDiscoverParameters(gap_discover_mode& discoverMode,
			unsigned short& scanInterval, unsigned short& scanWindow,
			bool& activeScan);

	//==============================================================================
	/**
	 * @brief Translates MAC Addresses to connection handles by O(n) search

	 * @param[in] mac MAC Address to be translated
	 * @param[out] connectionHandle contains correct handle if search was successful
	 * @return true if MAC was found, false otherwise
	 */
	//==============================================================================
	bool MACToConnectionHandle(bd_addr& mac, uint8_t& connectionHandle);

	//==============================================================================
	/**
	 * @brief Populates Uuid struct
	 * @param[in] rawData bytstream
	 * @param[in] length length in bytes
	 * @param[in] uuid pointer to Uuid struct
	 */
	//==============================================================================
	void ParseUUID(uint8* rawData, uint32_t length, Uuid* uuid);

	//==============================================================================
	/**
	 * @brief Parses an Advertisement/Scan Response packet
	 * @param[in] rawData bytstream
	 * @param[in] length length in bytes
	 * @param[in] dataMap maps GAP_ADTYPE to data in bytes
	 */
	//==============================================================================
	void ParseBGAPIAdvertisementPacket(uint8* rawData, uint32_t dataLength,
			AdvDataPayloadMap &dataMap);

	//==============================================================================
	/**
	 * @brief Registers an internal event to wait for
	 * @param[in] ce Type of event
	 * @param[in] es CompletionEvent struct which is populated and returned by WaitForCompletionEvent
	 * @Todo Not thread-safe. For now only one Thread per dongle is allowed
	 */
	//==============================================================================
	void RegisterForCompletionEvent(int ce, ConnectionEventStruct* es);

	//==============================================================================
	/**
	 * @brief Waits for an internal event to occur. Reads from TCP Socket while waiting
	 * Note: Do not read data from TCP socket manually between sending
	 * command and waiting for an event/response. Otherwise, the event might occur before
	 * waiting for it, leading to an "infinite" loop
	 * @param[in] ce Type of event
	 * @param[in] ch connection for which a command waits
	 * @param[in] waitTime time until timeout (in seconds)
	 * @result ConnectionEventStruct which contains handles and error Code. It is used to
	 * initialize a Result struct
	 * @Todo: add attribute handle as parameters to have a more fine grained system. Now
	 * reading from to different attributes is handled by the same event. This could be
	 * problematic if concurrent reads are allowed. Now only one read can happen at a time because
	 * worker threads serialize the requests.
	 */
	//==============================================================================
	ConnectionEventStruct WaitForCompletionEvent(int ce, uint8 ch,
			unsigned short waitTime);

	//==============================================================================
	/**
	 * @brief Notifies that an event occurred and frees all processes waiting for this event
	 * @param[in] ce Type of event
	 * @param[in] ch connectionHandle
	 * @param[in] errorCode errorCode of event/response
	 */
	//==============================================================================
	void PostCompletionEvent(int ce, uint8 ch, int errorCode);

};

//---------- Exceptions --------------------------------------------------------
//==============================================================================
//BLED112Exception
// Error: -1  Default
// Error: -2  Client side disconnect
class BLED112Exception: public std::exception {
	const char* errmsg;
	int errcode;

public:
	BLED112Exception(std::string errmsg) {
		this->errmsg = errmsg.c_str();
		this->errcode = -1;
	}
	BLED112Exception(std::string errmsg, int errcode) {
		this->errmsg = errmsg.c_str();
		this->errcode = errcode;
	}
	virtual const char* what() const throw () {
		return errmsg;
	}
	int GetErrCode() {
		return errcode;
	}
	const char* GetErrMsg() {
		return errmsg;
	}

};
//==============================================================================

}

#endif /* CH_ETHZ_AH_BLEDG_DRIVER_H_ */
