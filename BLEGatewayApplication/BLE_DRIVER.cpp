//==============================================================================
// Project   :  BLE Gateway
// File      :  BLE_DRIVER.cpp
// Author    :  Arthur Habicht
// Compiler  :  GCC C++
// Brief     :  Basic functions of a BLE Device instance running on the gateway
//==============================================================================

//---------- Includes ----------------------------------------------------------
#include "Headers/BLE_DRIVER.h"
namespace BLEGateway {

//==============================================================================
int Device::GetClientSocketDesc() {
//==============================================================================
	return this->clientSocketDesc;
}

//==============================================================================
char* Device::GetIPv4Address() {
//==============================================================================
	return this->clientIPv4;

}
//==============================================================================
unsigned short Device::GetClientPort() {
//==============================================================================

	return clientPort;
}

//==============================================================================
AttributeData* Device::GetAttribute(uint8_t connectionHandle,
		uint16_t attributeHandle) {
//==============================================================================

	return &connectedDevices[connectionHandle].attributeMap[attributeHandle];
}

//==============================================================================
ConnectedDeviceInfo* Device::GetConnectionInfo(uint8_t connectionHandle) {
//==============================================================================

	return &connectedDevices[connectionHandle];
}

//==============================================================================
void Device::GetAdvertismentData(std::vector<GatewayBaseStruct*>* advVector) {
//==============================================================================

	for (std::map<bd_addr_t, AdvertisementData>::iterator it =
			discoveredDevices.begin(); it != discoveredDevices.end(); it++) {
		advVector->push_back(&(*it).second);
	}

}

//==============================================================================
uint8_t Device::GetConnectedDevCount() {
//==============================================================================

	return this->connectedDevices.size();
}

//==============================================================================
BLEDeviceInfo* BLED112::GetBLEDInfo() {
//==============================================================================

	return &this->bledInfo;
}

//==============================================================================
void Device::SetClientSocketDesc(int clientSocketDesc) {
//==============================================================================
	this->clientSocketDesc = clientSocketDesc;
}

//==============================================================================
void Device::SetClientIPv4Address(sockaddr_in clientConnectionData) {
//==============================================================================
	inet_ntop(AF_INET, &(clientConnectionData.sin_addr), clientIPv4,
	INET_ADDRSTRLEN); //retrieve client IP address
}

//==============================================================================
void Device::SetClientPort(sockaddr_in clientConnectionData) {
//==============================================================================

	this->clientPort = htons(clientConnectionData.sin_port);
}

//==============================================================================
BLED112::BLED112(int clientSocketDesc, sockaddr_in clientConnectionData) {
//==============================================================================

	this->SetClientSocketDesc(clientSocketDesc);

	this->SetClientIPv4Address(clientConnectionData);

	this->SetClientPort(clientConnectionData);

	try {
		BleCmdSystemGetInfo(); //Driver command to retrieve BLED112 Info and check validity by checking integrity of response header format
		BleCmdSystemGetMaxConnections(); // Retrieve maximum possible connections for this BLED112
	} catch (BLED112Exception& e) {

		DebugPrint("%s\n", e.what());
		throw BLED112Exception(
				"BLED112 Error: Could not instantiate BLED112 instance for socket "
						+ GetClientSocketDesc());
	}

	DebugPrint("Connection with client socket %d:%s:%d successful \n",
			GetClientSocketDesc(), GetIPv4Address(), GetClientPort());

	isConnected = true; //connection successful, set connection to true, is set to false in ReadBytesFromTCPSocket if EOF is received
	//in Socket paradigm, EOF = other party closed connection
}

//==============================================================================
//BLED112::~BLED112() {} //default destructor
//==============================================================================

// Interface Implementations--------------------------------------------------------------------

//==============================================================================
void BLED112::WriteTCPMsgToSocket(char* buffer, size_t length) { //Throws exception
//==============================================================================
	uint32_t length_no = length;
	length_no = htonl(length_no); // 4 byte header for transmission of data size, network byte order

	//Send size of bytestream first
	if (send(clientSocketDesc, &length_no, sizeof(length_no), 0) < 0) {

		DebugPrint(
				"error %d:server could not send buffer length to %d:%s:%d: %s\n",
				errno, GetClientSocketDesc(), GetIPv4Address(), GetClientPort(),
				strerror(errno));

		throw BLED112Exception(
				"BLED112 Error: Send header data to client failed");

	}

	DebugPrint(
			"Server->Client Write: TCP header %zu bytes, dataSize %zu bytes from %d:%s:%d \n ",
			sizeof(length_no), length, GetClientSocketDesc(), GetIPv4Address(),
			GetClientPort());

	//send buffer data
	if (send(clientSocketDesc, buffer, length, 0) < 0) {
		DebugPrint("error %d:server could not send data to %d:%s:%d: %s\n",
				errno, GetClientSocketDesc(), GetIPv4Address(), GetClientPort(),
				strerror(errno));

		throw BLED112Exception("BLED112 Error: Send data to client failed");
	}

	DebugPrint(
			"Server->Client Write: TCP data %zu bytes, bytes from %d:%s:%d \n ",
			length, GetClientSocketDesc(), GetIPv4Address(), GetClientPort());

}

//==============================================================================
void BLED112::ProcessTCPMsgFromSocket() { //Throws exception
//==============================================================================

	char header[RECEIVE_HEADER_SIZE];
	int rread = 0;
	struct pollfd pdesc[1];

	pdesc[0].fd = clientSocketDesc;
	pdesc[0].events = POLLIN;

	//polling is done here not in ReadBytesFromTCPSocket to avoid the situation where only the header is read and the payload is lost due to timeout.
	//this would bring the whole protocol out of sync, as 4 byte of late payload data in the socket buffer would be treated as header
	int pollRes = poll(pdesc, 1, SOCKET_POLL_TIMEOUT); //Timeout need to be large enough. avoid tight loop

	if (pdesc[0].revents & POLLIN) {

		try {
			rread = ReadBytesFromTCPSocket(header, RECEIVE_HEADER_SIZE);
		} catch (BLED112Exception& e) {
			DebugPrint("%s\n", e.what());
			throw BLED112Exception(
					"BLED112 Error: Read header data from client failed");
		}

		uint32_t dataSize = 0;

		for (int i = 0; i < RECEIVE_HEADER_SIZE; ++i) {
			dataSize = (dataSize << 8) + header[i];
		}

		DebugPrint(
				"Client->Server READ: TCP header %d bytes, dataSize %d bytes from %d:%s:%d \n ",
				rread, dataSize, GetClientSocketDesc(), GetIPv4Address(),
				GetClientPort());

		//read data_size bytes  data if more than 0 bytes had been sent
		if (dataSize) {

			char data[dataSize];

			try {

				rread = ReadBytesFromTCPSocket(data, dataSize);
			} catch (BLED112Exception& e) {
				DebugPrint("%s \n", e.what());
				throw BLED112Exception(
						"BLED112 Error: Read data from client failed");
			}

			DebugPrint(
					"Client->Server READ: TCP data %d bytes, dataSize %d bytes from %d:%s:%d \n ",
					rread, dataSize, GetClientSocketDesc(), GetIPv4Address(),
					GetClientPort());

			//deserialize the incoming message and call corresponding commands
			try {

				DecodeIncomingTCPMessage(data, dataSize);
			} catch (BLED112Exception& e) {
				DebugPrint("%s \n", e.what());
				throw BLED112Exception(
						"BLED112 Error: Could not Decode TCP Message");
			}
		}

	} else if (pollRes == -1) {
		throw BLED112Exception("BLED112 Error: Polling for read failed");
	}

}

//==============================================================================
void BLED112::DecodeIncomingTCPMessage(char* message, uint32_t length) { //Throws exception
//==============================================================================

	uint16_t commandIdx = 0;
	if (tpl_peek(TPL_MEM | TPL_DATAPEEK, message, length, "v",
			&commandIdx) == NULL) {
		DebugPrint(
				"error, could not decode incoming TCP message from %d:%s:%d \n",
				GetClientSocketDesc(), GetIPv4Address(), GetClientPort());

		throw BLED112Exception("BLED112 Error: Incoming TCP decode failure");
	}

	try {
		switch (commandIdx) {

		case ble_rsp_system_get_info_idx: {
			BleRspSystemGetInfo(message, length);
			break;
		}
		case ble_rsp_gap_set_scan_parameters_idx: {

			BleSimpleResponse(message, length);
			break;
		}
		case ble_rsp_gap_discover_idx: {

			BleSimpleResponse(message, length);
			break;
		}
		case ble_rsp_gap_end_procedure_idx: {

			BleSimpleResponse(message, length);
			break;
		}
		case ble_rsp_system_get_connections_idx: {
			BleRspSystemGetMaxConnections(message, length);
			break;
		}

		case ble_rsp_gap_connect_direct_idx: {
			BleRspGapConnectDirect(message, length);
			break;
		}
		case ble_rsp_connection_get_status_idx: {
			BleRspConnectionGetStatus(message, length);
			break;
		}

		case ble_evt_gap_scan_response_idx: {
			BleEvtGapScanResponse(message, length);
			break;
		}
		case ble_evt_connection_status_idx: {

			BleEvtConnectionGetStatus(message, length);
			break;
		}
		case ble_rsp_connection_disconnect_idx: {

			BleSimpleConnectionResponse(message, length);

			break;
		}
		case ble_evt_connection_disconnected_idx: {
			BleEvtConnectionDisconnect(message, length);
			break;
		}
		case ble_rsp_connection_update_idx: {

			BleSimpleConnectionResponse(message, length);

			break;
		}
		case ble_rsp_attclient_read_by_group_type_idx: { //group = primary service, secondary service not supported

			BleSimpleConnectionResponse(message, length);

			break;
		}
		case ble_evt_attclient_group_found_idx: { //group = primary service, secondary service not supported
			BleEvtGattServiceFound(message, length);
			break;
		}
		case ble_evt_attclient_procedure_completed_idx: {
			BleEvtGattProcedureCompleted(message, length);
			break;

		}
		case ble_rsp_attclient_read_by_type_idx: {

			BleSimpleConnectionResponse(message, length);

			break;

		}
		case ble_evt_attclient_attribute_value_idx: {
			BleEvtGattAttributeValue(message, length);
			break;

		}
		case ble_rsp_attclient_find_information_idx: {

			BleSimpleConnectionResponse(message, length);
			break;

		}
		case ble_evt_attclient_find_information_found_idx: {

			BleEvtGattAttributeInfoFound(message, length);

			break;

		}
		case ble_rsp_attclient_read_by_handle_idx: {

			BleSimpleConnectionResponse(message, length);
			break;

		}
		case ble_rsp_attclient_attribute_write_idx: {

			BleSimpleConnectionResponse(message, length);
			break;

		}
		default: {
			DebugPrint("error, operation idx %d not handled\n", commandIdx);
			throw BLED112Exception(
					"BLED112 Error: Decoded TCP message not supported");
		}

		}
	} catch (BLED112Exception& e) {
		DebugPrint("%s \n", e.what());
		throw BLED112Exception("BLED112 Error: Server Callback failure");
	}

}

//==============================================================================
int BLED112::ReadBytesFromTCPSocket(char* dataBuffer, int length) { //Throws exception
//==============================================================================

	int l = length;
	ssize_t bytesRead;

	while (length) {

		bytesRead = recv(clientSocketDesc, dataBuffer, length, 0);

		if (bytesRead < 0) {
			DebugPrint(
					"error %d:server could not read data from %d:%s:%d: bytes read = %d, remaining bytes = %d:  %s\n",
					errno, GetClientSocketDesc(), GetIPv4Address(),
					GetClientPort(), RECEIVE_HEADER_SIZE - length, length,
					strerror(errno));

			throw BLED112Exception("BLED112 Error: Read failed");

		} else if (bytesRead == 0) //EOF
				{
			isConnected = false; //EOF: Client closed connection, signal this by setting is connected to false
			throw BLED112Exception("BLED112 Error: Client Closed Socket", -2); //-2 -> socket closed

		} else {

			length -= bytesRead;
			dataBuffer += length;
		}
	}

	return l;

}

//==============================================================================
Result BLED112::UpdateBledInformation() {
//==============================================================================


	BleCmdSystemGetInfo();

				ConnectionEventStruct conEvSt = WaitForCompletionEvent( //wait for the BGAPI response packet
						SYS_GET_INFO_RSP_EVT, 0, 5);
				Result result;
				result.errorCode = conEvSt.errorCode;
				result.errorMessage = CreateErrorMessage(conEvSt.errorCode);
				result.connectionHandle = 0;
				if (result.errorCode) {
					return result; // Error when trying to read from a handle
				}else{

					BleCmdSystemGetMaxConnections();
					conEvSt = WaitForCompletionEvent( //wait for the BGAPI response packet
							SYS_GET_MAXCON_RSP_EVT, 0, 5);

					result.errorCode = conEvSt.errorCode;
									result.errorMessage = CreateErrorMessage(conEvSt.errorCode);
									result.connectionHandle = 0;
									if (result.errorCode) {
										return result; // Error when trying to read from a handle
									}else{


					BLEDeviceInfo* dInfo = GetBLEDInfo();
					GatewayBaseStruct* sData = dInfo;
					std::vector<GatewayBaseStruct*> resultStructs;
					resultStructs.push_back(sData);
					result.data = resultStructs;

					return result;
									}
				}



}

//==============================================================================
Result BLED112::ReadAttribute(std::vector<uint8> macAddress,
		uint16_t attributeHandle, int lastUpdate) {
//==============================================================================

	DebugPrint("%s\n", "Command: ReadAttribute");

	bd_addr macAddr;
	int i = MAC_SIZE - 1;
	for (auto a : macAddress) {
		macAddr.addr[i--] = a; //little endian
	}
	Result result;
	uint8 connectionHandle = 0;

	if (MACToConnectionHandle(macAddr, connectionHandle)) {

		try {



				boost::posix_time::ptime cached(
						connectedDevices[connectionHandle].attributeMap[attributeHandle].timestamp);
				boost::posix_time::ptime current(
						boost::posix_time::second_clock::universal_time());
				boost::posix_time::time_duration diff = current - cached;

				result.errorCode = 0x0000;
				result.errorMessage = "Returning cached values";

				if (diff.seconds() >= lastUpdate) {

					BleCmdGattFindAttributeInfo(connectionHandle,
							attributeHandle, attributeHandle);

					ConnectionEventStruct conEvSt = WaitForCompletionEvent(
							//wait for the BGAPI response packet
							ATTCLIENT_FINDINFO_RSP_EVT, connectionHandle,
							EVENT_TIMEOUT);

					result.errorCode = conEvSt.errorCode;
					result.errorMessage = CreateErrorMessage(conEvSt.errorCode);
					result.connectionHandle = connectionHandle;

					if (result.errorCode) {
						return result; // Error when trying to read from a handle
					}
					conEvSt = WaitForCompletionEvent(
							//wait for the BGAPI response packet
							GATT_PROCEDURE_COMPLETE_EVT
									| CONNECTION_DISCONNECT_EVT,
							connectionHandle, EVENT_TIMEOUT);

					result.errorCode = conEvSt.errorCode;
					result.errorMessage = CreateErrorMessage(conEvSt.errorCode);

					if (result.errorCode) {
						return result; // Error when trying to read from a handle
					}

					BleCmdGattReadAttributeByHandle(connectionHandle,
							attributeHandle);

					conEvSt = WaitForCompletionEvent(
							//wait for the BGAPI response packet
							ATTCLIENT_READ_RSP_EVT, connectionHandle,
							EVENT_TIMEOUT);

					result.errorCode = conEvSt.errorCode;
					result.errorMessage = CreateErrorMessage(conEvSt.errorCode);

					if (result.errorCode) {
						return result; // Error when trying to read from a handle
					}

					conEvSt = WaitForCompletionEvent(
							//wait for the BGAPI response packet
							ATTCLIENT_ATTRIBUTE_FOUND_EVT
									| GATT_PROCEDURE_COMPLETE_EVT
									| CONNECTION_DISCONNECT_EVT,
							connectionHandle, EVENT_TIMEOUT);

					result.errorCode = conEvSt.errorCode;
					result.errorMessage = CreateErrorMessage(result.errorCode);

					if (result.errorCode) {
						return result; // Error when trying to read from a handle (event error)
					}

				}
				if (connectedDevices.count(connectionHandle) > 0) {

					std::vector<GatewayBaseStruct*> resultStructs;

					GatewayBaseStruct* attribute =
							&connectedDevices[connectionHandle].attributeMap[attributeHandle];

					resultStructs.push_back(attribute);

					result.data = resultStructs; //move semantics?
					result.connectionHandle = connectionHandle;

					return result;

				} else {
					result.errorCode = 0x0003;
					result.errorMessage = CreateErrorMessage(result.errorCode);
					result.connectionHandle = -1;
					return result;
				}




		} catch (BLED112Exception& e) {
			DebugPrint("%s \n", e.what());
			result.errorCode = 0x0001;
			result.errorMessage = CreateErrorMessage(result.errorCode);
			result.connectionHandle = connectionHandle;
			return result;

		}
	} else {

		result.errorCode = 0x0000;
		result.errorMessage = "No Device with this MAC Address found";
		result.connectionHandle = -1;
		return result;
	}

}

//==============================================================================
Result BLED112::WriteAttribute(std::vector<uint8> macAddress,
		uint16_t attributeHandle, std::string data) {
//==============================================================================
	DebugPrint("%s\n", "Command: WriteAttribute");
	bd_addr macAddr;
	int i = MAC_SIZE - 1;
	for (auto a : macAddress) {
		macAddr.addr[i--] = a; //little endian
	}
	Result result;
	uint8 connectionHandle = 0;

	std::vector<uint8_t> bytes = HexStrToHexBuf(data);

	if (bytes.size() > BGAPI_WRITE_PAYLOAD) {
		bytes.resize(BGAPI_WRITE_PAYLOAD); //Payload can be at max 20 bytes!
	}

	unsigned int length = bytes.size();

	char dataBuffer[BGAPI_WRITE_PAYLOAD];
	std::copy(bytes.begin(), bytes.end(), dataBuffer);

	if (MACToConnectionHandle(macAddr, connectionHandle)) {

		try {

			BleCmdGattWriteAttributeByHandle(connectionHandle, attributeHandle,
					dataBuffer, length);

			ConnectionEventStruct conEvSt = WaitForCompletionEvent( //wait for the BGAPI response packet
					ATTCLIENT_WRITE_RSP_EVT, connectionHandle, EVENT_TIMEOUT);

			result.errorCode = conEvSt.errorCode;
			result.errorMessage = CreateErrorMessage(conEvSt.errorCode);
			result.connectionHandle = connectionHandle;
			if (result.errorCode) {
				return result; // Error when trying to read from a handle
			}

			conEvSt = WaitForCompletionEvent(
					//wait for the BGAPI response packet
					GATT_PROCEDURE_COMPLETE_EVT | CONNECTION_DISCONNECT_EVT,
					connectionHandle, EVENT_TIMEOUT);

			result.errorCode = conEvSt.errorCode;
			result.errorMessage = CreateErrorMessage(result.errorCode);

			return result;

		} catch (BLED112Exception& e) {

			DebugPrint("%s \n", e.what());
			result.errorCode = 0x0001;
			result.errorMessage = CreateErrorMessage(result.errorCode);
			result.connectionHandle = connectionHandle;
			return result;

		}
	} else {

		result.errorCode = 0x0003;
		result.errorMessage = "No Device with this MAC Address found";
		result.connectionHandle = -1;
		return result;

	}

}

//==============================================================================
Result BLED112::Discover(gap_discover_mode discoverMode,
		unsigned short scanInterval, unsigned short scanWindow, bool activeScan,
		unsigned short discoverTime) {
//==============================================================================
	DebugPrint("%s\n", "Command: Discover");

	bool correctParameters = CheckDiscoverParameters(discoverMode, scanInterval,
			scanWindow, activeScan); //checks and sets default if not correct

	Result result;

	if (discoverTime > 0) { // do not start discovery if requested time was 0

		try {

			BleCmdGapDiscover(discoverMode, scanInterval, scanWindow,
					activeScan);

			//Wait for response to Discovery Command
			ConnectionEventStruct conEvSt = WaitForCompletionEvent(
					GAP_DISCOVER_RSP_EVT, 0, EVENT_TIMEOUT);

			result.errorCode = conEvSt.errorCode;
			result.errorMessage = CreateErrorMessage(conEvSt.errorCode);

			if (result.errorCode) {
				return result; //Discovery could not be started due to error in response
			}

			//If response errorcode = 0 then discovery started
			time_t begTime = std::time(NULL);
			while ((std::difftime(time(NULL), begTime)) < discoverTime) {

				try {
					ProcessTCPMsgFromSocket();
				} catch (BLED112Exception& e) {
					DebugPrint("%s \n", e.what());
				}
			}
			BleCmdGapEndProcedure(); //stops discovery mode
		} catch (BLED112Exception& e) { //catch any exception
			DebugPrint("%s \n", e.what());

			result.errorCode = 0x0001;
			result.errorMessage = CreateErrorMessage(result.errorCode);

			return result;
		}

		if (!correctParameters) { //if the default values had to be resetted, notify the client

			result.errorCode = 0x0000;
			result.errorMessage = "Parameters were changed to default values";
		}
	} else {
		result.errorMessage = "Returning cached values";
	}

	std::vector<GatewayBaseStruct*> resultStructs;
	GetAdvertismentData(&resultStructs);

	result.errorCode = 0x0000;

	result.data = resultStructs; //move data

	return result;
}

//conn handke -1 if connect, 0-255 if update
//==============================================================================
Result BLED112::Connect(std::vector<uint8> macAddress, uint8_t addressType,
		uint16_t connectionIntervalMin, uint16_t connectionIntervalMax,
		uint16_t timeout, uint16_t slaveLatency) {
//==============================================================================

	DebugPrint("%s\n", "Command: Connect/Update");

	bool correctParameters = CheckConnectionParameters(connectionIntervalMin,
			connectionIntervalMax, timeout, slaveLatency);

	//convert Mac Address to !little endian! array
	bd_addr macAddr;
	int i = MAC_SIZE - 1;
	for (auto a : macAddress) {
		macAddr.addr[i--] = a;
	}

	Result result;

	uint8 connectionHandle = 0;

	//Differentiate between Update and Connect on the basis whether this mac address already exists and has a connection handle
	if (MACToConnectionHandle(macAddr, connectionHandle)) {
		//UPDATE: MAC does already exist

		try {

			BleCmdConnectionUpdate(connectionHandle, connectionIntervalMin,
					connectionIntervalMax, slaveLatency, timeout);

			ConnectionEventStruct conEvSt = WaitForCompletionEvent( //wait for the BGAPI response packet
					CONNECTION_UPDATE_RSP_EVT, connectionHandle, EVENT_TIMEOUT);

			result.errorCode = conEvSt.errorCode;
			result.errorMessage = CreateErrorMessage(result.errorCode);
			result.connectionHandle = connectionHandle;

			if (result.errorCode) {
				return result;
			}

			conEvSt = WaitForCompletionEvent(CONNECTION_UPDATE_EVT|CONNECTION_DISCONNECT_EVT,
					connectionHandle, EVENT_TIMEOUT); //wait for the update complete event, BGAPI ensures that the response arrives before the update event
			result.errorCode = conEvSt.errorCode;
			result.errorMessage = CreateErrorMessage(result.errorCode);

			if (result.errorCode) {

				return result; // Error when trying to read from a handle (event error)
			}

			if (!correctParameters) { //if the default values had to be set, notify the client
				result.errorCode = 0x0000;
				result.errorMessage =
						"Invalid parameters: changed to default values";
			}

		} catch (BLED112Exception& e) {
			DebugPrint("%s \n", e.what());
			result.errorCode = 0x0001;
			result.errorMessage = CreateErrorMessage(result.errorCode);
			result.connectionHandle = connectionHandle;
			return result;
		}

		//return updated values (connection handle is known), result.data is a non empty vector
		std::vector<GatewayBaseStruct*> resultStructs;

		// it could be possible that the device was updated and then disconnected and the update event arrives after the disconnect event
		//then the device is not in the connected devices anymore. This should lead to an error message (update not successful 0x0003)
		//also it is important to check whether the connection still exists before accessing the map with [] operator, because this could create
		//a new key with an inexistent connectionHandle.

		if (connectedDevices.count(connectionHandle) > 0) {

			GatewayBaseStruct* connectedDevice =
					&connectedDevices[connectionHandle];

			resultStructs.push_back(connectedDevice);

			result.data = resultStructs; //Todo move semantics instead of copy?
			result.connectionHandle = connectionHandle;

			return result;

		} else {
			result.errorCode = 0x0003;
			result.errorMessage = CreateErrorMessage(result.errorCode);
			result.connectionHandle = -1;
			return result;
		}

	} else {
		//CONNECT: MAC does not exist, connectionHandle is set to -1. This is used by the Gateway application to
		//differentiate between connect and update. Update returns a 8 bit connection handle from 0 to 255

		try {

			BleCmdGapConnectDirect(macAddr, addressType, connectionIntervalMin,
					connectionIntervalMax, timeout, slaveLatency);

			ConnectionEventStruct conEvSt = WaitForCompletionEvent( //wait for the BGAPI response packet
					GAP_CONNECTION_RSP_EVT, 0, EVENT_TIMEOUT);

			result.errorCode = conEvSt.errorCode;
			result.errorMessage = CreateErrorMessage(result.errorCode);
			result.connectionHandle = -1;
			if (result.errorCode) {
				return result;
			}

			conEvSt = WaitForCompletionEvent(GAP_CONNECTION_EVT, 0,
					CONNECTION_EVENT_TIMEOUT); //wait for the connection complete event, BGAPI ensures that the response arrives before the connection event
			result.errorCode = conEvSt.errorCode;
			result.errorMessage = CreateErrorMessage(result.errorCode);
			if (result.errorCode) {
				BleCmdGapEndProcedure(); //end connection attempt
				return result;
			}

			if (!correctParameters) { //if the default values had to be set, notify the client
				result.errorCode = 0x0000;
				result.errorMessage =
						"Parameters were changed to default values";
			}

		} catch (BLED112Exception& e) {
			DebugPrint("%s \n", e.what());
			result.errorCode = 0x0001;
			result.errorMessage = CreateErrorMessage(result.errorCode);
			result.connectionHandle = -1;
			return result;
		}

		return result;

	}

}

//==============================================================================
Result BLED112::Disconnect(std::vector<uint8> macAddress) {
//==============================================================================
	DebugPrint("%s\n", "Command: Disconnect");
	//convert Mac Address to !little endian! array
	bd_addr macAddr;
	int i = MAC_SIZE - 1;
	for (auto a : macAddress) {
		macAddr.addr[i--] = a;
	}

	Result result;

	uint8 connectionHandle = 0;

	if (MACToConnectionHandle(macAddr, connectionHandle)) {

		try {

			BleCmdConnectionDisconnect(connectionHandle);

			ConnectionEventStruct conEvSt = WaitForCompletionEvent(
					//wait for the BGAPI response packet
					CONNECTION_DISCONNECT_RSP_EVT, connectionHandle,
					EVENT_TIMEOUT);

			result.errorCode = conEvSt.errorCode;
			result.errorMessage = CreateErrorMessage(result.errorCode);
			result.connectionHandle = connectionHandle;
			if (result.errorCode) { //non zero error code
				return result;
			}

			conEvSt = WaitForCompletionEvent( //wait for the BGAPI event packet
					CONNECTION_DISCONNECT_EVT, connectionHandle, EVENT_TIMEOUT);
			result.errorCode = conEvSt.errorCode;
			result.errorMessage = CreateErrorMessage(result.errorCode);
			return result;

		} catch (BLED112Exception& e) {
			perror(e.what());
			result.errorCode = 0x0001;
			result.errorMessage = CreateErrorMessage(result.errorCode);
			result.connectionHandle = connectionHandle;

			return result;
		}
	} else {

		result.errorCode = 0x0000;
		result.errorMessage = "No Device with this MAC Address found";
		result.connectionHandle = -1;
		return result;

	}
}


//==============================================================================
Result BLED112::ReadAllAttributes(std::vector<uint8> macAddress) {
//==============================================================================
	DebugPrint("%s\n", "Command: ReadAllAttributes");
	//convert Mac Address to !little endian! array
	bd_addr macAddr;
	int i = MAC_SIZE - 1;
	for (auto a : macAddress) {
		macAddr.addr[i--] = a;
	}

	Result result;

	uint8 connectionHandle = 0;

	if (MACToConnectionHandle(macAddr, connectionHandle)) {

		try {

			//Update primary service info
			BleCmdGattFindPrimaryServices(connectionHandle);

			ConnectionEventStruct conEvSt = WaitForCompletionEvent(
					//wait for the BGAPI response packet
					ATTCLIENT_READGROUP_RSP_EVT, connectionHandle,
					EVENT_TIMEOUT);

			result.errorCode = conEvSt.errorCode;
			result.errorMessage = CreateErrorMessage(conEvSt.errorCode);
			result.connectionHandle = connectionHandle;
			if (result.errorCode) {
				return result; // Error when trying to read from a handle
			}

			conEvSt = WaitForCompletionEvent(
					//wait for the BGAPI response packet
					GATT_PROCEDURE_COMPLETE_EVT | CONNECTION_DISCONNECT_EVT,
					connectionHandle, EVENT_TIMEOUT);
			//no procedure completed if device disconnects during the operation
			result.errorCode = conEvSt.errorCode;
			result.errorMessage = CreateErrorMessage(conEvSt.errorCode);

			if (result.errorCode) {
				return result; // Error when trying to read from a handle
			}

			//Update all attribute info
			BleCmdGattFindAttributeInfo(connectionHandle, GATT_MIN_HANDLE,
					GATT_MAX_HANDLE);

			conEvSt = WaitForCompletionEvent(
					//wait for the BGAPI response packet
					ATTCLIENT_FINDINFO_RSP_EVT, connectionHandle,
					EVENT_TIMEOUT);

			result.errorCode = conEvSt.errorCode;
			result.errorMessage = CreateErrorMessage(conEvSt.errorCode);

			if (result.errorCode) {
				return result; // Error when trying to read from a handle
			}
			conEvSt = WaitForCompletionEvent(
					//wait for the BGAPI response packet
					GATT_PROCEDURE_COMPLETE_EVT | CONNECTION_DISCONNECT_EVT,
					connectionHandle, EVENT_TIMEOUT);

			result.errorCode = conEvSt.errorCode;
			result.errorMessage = CreateErrorMessage(conEvSt.errorCode);

			if (result.errorCode) {
				return result; // Error when trying to read from a handle
			}

			//return pointer to data
			if (connectedDevices.count(connectionHandle) > 0) {

				std::vector<GatewayBaseStruct*> resultStructs;

				GatewayBaseStruct* device =
						&(connectedDevices[connectionHandle]);

				resultStructs.push_back(device);

				result.data = resultStructs;
				result.connectionHandle = connectionHandle;

				return result;

			} else {
				result.errorCode = 0x0003;
				result.errorMessage = CreateErrorMessage(result.errorCode);
				result.connectionHandle = -1;
				return result;
			}

		} catch (std::exception& e) {

			DebugPrint("%s \n", e.what());
			result.errorCode = 0x0001;
			result.errorMessage = CreateErrorMessage(result.errorCode);
			result.connectionHandle = connectionHandle;

			return result;
		}
	} else {

		result.errorCode = 0x0000;
		result.errorMessage = "No Device with this MAC Address found";
		result.connectionHandle = -1;
		return result;
	}

}

//==============================================================================
Result BLED112::GetConnectedDevices() {
//==============================================================================

	Result result;
	std::vector<GatewayBaseStruct*> resultStructs;
	for (auto& c : connectedDevices) {
		resultStructs.push_back(&(c.second));
	}

	result.errorCode = 0x0000;
	result.errorMessage = CreateErrorMessage(result.errorCode);
	result.connectionHandle = -1;

	result.data = resultStructs; //todo: move semantics instead of copy?

	return result;
}

//==============================================================================
std::string BLED112::CreateErrorMessage(int errorCode) {
//==============================================================================

	switch (errorCode) {

	case 0x0000:
		return "Success";
	case 0x0001:
		return "Gateway internal exception";
	case 0x0002:
		return "Operation timeout";
	case 0x0003:
		return "Unsuccessful, maybe the server disconnected or was shut down";
	case 0x0180:
		return "Command contained invalid parameter";
	case 0x0181:
		return "Device is in wrong state to receive command";
	case 0x0182:
		return "Device has run out of memory";
	case 0x0183:
		return "Feature is not implemented";
	case 0x0184:
		return "Command was not recognized";
	case 0x0185:
		return "Command or Procedure failed due to timeout";
	case 0x0186:
		return "Connection handle passed is to command is not a valid handle";
	case 0x0187:
		return "Command would cause either underflow or overflow error";
	case 0x0188:
		return "User attribute was accessed through API which is not supported";
	case 0x0189:
		return "No valid license key found";
	case 0x018A:
		return "Command maximum length exceeded";
	case 0x018B:
		return "Bonding procedure can't be started because device has no space left for bond.";
	case 0x0205:
		return "Pairing or authentication failed due to incorrect results in the pairing or authentication procedure. This could be due to an incorrect PIN or Link Key";
	case 0x0206:
		return "Pairing failed because of missing PIN, or authentication failed because of missing Key";
	case 0x0207:
		return "Controller is out of memory";
	case 0x0208:
		return "Link supervision timeout has expired";
	case 0x0209:
		return "Controller is at limit of connections it can support";
	case 0x020C:
		return "Command requested cannot be executed because the Controller is in a state where it cannot process this command at this time";
	case 0x0212:
		return "Command contained invalid parameters";
	case 0x0213:
		return "User on the remote device terminated the connection";
	case 0x0216:
		return "Local device terminated the connection";
	case 0x0222:
		return "Connection terminated due to link-layer procedure timeout";
	case 0x0228:
		return "Received link-layer control packet where instant was in the past";
	case 0x023A:
		return "Operation was rejected because the controller is busy and unable to process the request";
	case 0x023B:
		return "The Unacceptable Connection Interval error code indicates that the remote device terminated the connection because of an unacceptable connection interval";
	case 0x023C:
		return "Directed advertising completed without a connection being created";
	case 0x023D:
		return "Connection was terminated because the Message Integrity Check (MIC) failed on a received packet";
	case 0x023E:
		return "LL initiated a connection but the connection has failed to be established. Controller did not receive any packets from remote end";
	case 0x0401:
		return "The attribute handle given was not valid on this server";
	case 0x0402:
		return "The attribute cannot be read";
	case 0x0403:
		return "The attribute cannot be written";
	case 0x0404:
		return "The attribute PDU was invalid";
	case 0x0405:
		return "The attribute requires authentication before it can be read or written";
	case 0x0406:
		return "Attribute Server does not support the request received from the client";
	case 0x0407:
		return "Offset specified was past the end of the attribute";
	case 0x0408:
		return "The attribute requires authorization before it can be read or written";
	case 0x0409:
		return "Too many prepare writes have been queue";
	case 0x040A:
		return "No attribute found within the given attribute handle range";
	case 0x040B:
		return "The attribute cannot be read or written using the Read Blob Request";
	case 0x040C:
		return "The Encryption Key Size used for encrypting this link is insufficient";
	case 0x040D:
		return "The attribute value length is invalid for the operation";
	case 0x040E:
		return "The attribute request that was requested has encountered an error that was unlikely, and therefore could not be completed as requested";
	case 0x040F:
		return "The attribute requires encryption before it can be read or written";
	case 0x0410:
		return "The attribute type is not a supported grouping attribute as defined by a higher layer specification";
	case 0x0411:
		return "Insufficient Resources to complete the request";
	case 0x0480:
		return "Application error code defined by a higher layer specification";
	case 0x0301:
		return "The user input of passkey failed, for example, the user cancelled the operation";
	case 0x0302:
		return "Out of Band data is not available for authentication";
	case 0x0303:
		return "The pairing procedure cannot be performed as authentication requirements cannot be met due to IO capabilities of one or both devices";
	case 0x0304:
		return "The confirm value does not match the calculated compare value";
	case 0x0305:
		return "Pairing is not supported by the device";
	case 0x0306:
		return "The resultant encryption key size is insufficient for the security requirements of this device";
	case 0x0307:
		return "The SMP command received is not supported on this device";
	case 0x0308:
		return "Pairing failed due to an unspecified reason";
	case 0x0309:
		return "Pairing or authentication procedure is disallowed because too little time has elapsed since last pairing request or security request";
	case 0x030A:
		return "The Invalid Parameters error code indicates: the command length is invalid or a parameter is outside of the specified range";
	default:
		return "Unknown Error";
	}

}

//---------- Command Functions ------------------------------------------------

//==============================================================================
void BLED112::BleCmdSystemGetInfo() { //throws exception
//==============================================================================

	SendSimpleCommand(ble_cmd_system_get_info_idx, "v");
}

//==============================================================================
void BLED112::BleCmdGapEndProcedure() { //Stop discovery mode //throws exception
//==============================================================================

	SendSimpleCommand(ble_cmd_gap_end_procedure_idx, "v");
}

//==============================================================================
void BLED112::BleCmdSystemGetMaxConnections() { //Get Max connections //throws exception
//==============================================================================

	SendSimpleCommand(ble_cmd_system_get_connections_idx, "v");
}

//==============================================================================
void BLED112::BleCmdGapDiscover(gap_discover_mode discoverMode,
		unsigned short scanInterval, unsigned short scanWindow,
		bool activeScan) { //throws exception
//==============================================================================

	size_t length = 0;
	tpl_node* tn;
	char* buffer;

	//reset list of previously discovered devices;
	discoveredDevices.clear();

	//set scan parameters
	//serializing data for transmission over sockets and little-endian to big-endian conversion
	uint16_t id = ble_cmd_gap_set_scan_parameters_idx;
	uint16_t scanInt = scanInterval;
	uint16_t scanWin = scanWindow;
	char activeSc = activeScan;
	if ((tn = tpl_map((char*) "vvvc", &id, &scanInt, &scanWin, &activeSc))
			== NULL) { //serialize to  bytestream

		DebugPrint("error %d, %d:%s:%d could not serialize data: %s\n ", errno,
				GetClientSocketDesc(), GetIPv4Address(), GetClientPort(),
				strerror(errno));

		throw BLED112Exception("BLED112 Error: Serialization failed");

	}
	tpl_pack(tn, 0);
	tpl_dump(tn, TPL_MEM, &buffer, &length);
	tpl_free(tn);

	try {
		WriteTCPMsgToSocket(buffer, length);
	} catch (std::exception& e) {
		DebugPrint("%s \n", e.what());
		free(buffer);
		throw BLED112Exception("BLED112 Error: Discover failed");
	}

	free(buffer); //tpl allocates data in heap with malloc, so must be freed

	//start Discovery
	id = ble_cmd_gap_discover_idx;
	uint16_t scanMode = discoverMode;

	if ((tn = tpl_map((char*) "vv", &id, &scanMode)) == NULL) { //serialize to  bytestream

		DebugPrint("error %d, %d:%s:%d could not serialize data: %s\n ", errno,
				GetClientSocketDesc(), GetIPv4Address(), GetClientPort(),
				strerror(errno));

		throw BLED112Exception("BLED112 Error: Discover Serialization failed");

	}
	tpl_pack(tn, 0);
	tpl_dump(tn, TPL_MEM, &buffer, &length);
	tpl_free(tn);

	try {
		WriteTCPMsgToSocket(buffer, length);
	} catch (std::exception& e) {
		DebugPrint("%s \n", e.what());
		free(buffer);
		throw BLED112Exception("BLED112 Error: Discover failed");
	}

	free(buffer); //tpl allocates data in heap with malloc, so must be freed

}

//============================================================================== //selective connect not implemented
void BLED112::BleCmdGapConnectDirect(bd_addr macAddress, uint8 addressType,
		uint16 connIntervalMin, uint16 connIntervalMax, uint16 timeout,
		uint16 slaveLatency) { //throws exception
//==============================================================================

	//TODO: try to find a way that the correct addresstype of a device is found and then used here automatically. Now, the user
	// needs to know the addresstype and the connection attempt fails if the wrong type is chosen.
	// This should be simple: The discovered devices map can be searched for the mac address. Then the address type
	//should be inside one of the advertisement packets.

// conn_Interval range 6 - 3200 in units of 1.25ms, minimum 7.5ms max. 4000ms
// timeout, how long devices can be out of range: range 10 - 3200 in units of 10ms
// latency: 0 - 500  in nr. of connection slave is allowed to skip  slave latency*con_int <= timeout

	//Note: connection parameters need to be checked on upper layer. Invalid parameters will cause an error
	// in the response message

	size_t length = 0;
	tpl_node* tn;
	char* buffer;
	uint16_t id = ble_cmd_gap_connect_direct_idx;

	if ((tn = tpl_map((char*) "vS(c#)cvvvv", &id, &macAddress, MAC_SIZE,
			&addressType, &connIntervalMin, &connIntervalMax, &timeout,
			&slaveLatency)) == NULL) { //serialize to  bytestream

		DebugPrint("error %d, %d:%s:%d could not serialize data: %s\n ", errno,
				GetClientSocketDesc(), GetIPv4Address(), GetClientPort(),
				strerror(errno));

		throw BLED112Exception("BLED112 Error: Connect Serialization failed");

	}
	tpl_pack(tn, 0);
	tpl_dump(tn, TPL_MEM, &buffer, &length);
	tpl_free(tn);

	try {

		WriteTCPMsgToSocket(buffer, length);
	} catch (std::exception& e) {
		DebugPrint("%s \n", e.what());
		free(buffer);
		throw BLED112Exception("BLED112 Error: Connect failed");
	}
	free(buffer); //tpl allocates data in heap with malloc, so must be freed

}

//==============================================================================
void BLED112::BleCmdConnectionGetStatus(uint8 connectionHandle) { //throws exception
//==============================================================================

	size_t length = 0;
	tpl_node* tn;
	char* buffer;
	uint16_t id = ble_cmd_connection_get_status_idx;

	if ((tn = tpl_map((char*) "vc", &id, &connectionHandle)) == NULL) { //serialize to  bytestream

		DebugPrint("error %d, %d:%s:%d could not serialize data: %s\n ", errno,
				GetClientSocketDesc(), GetIPv4Address(), GetClientPort(),
				strerror(errno));
		throw BLED112Exception("BLED112 Error: GetStatus Serialization failed");

	}
	tpl_pack(tn, 0);
	tpl_dump(tn, TPL_MEM, &buffer, &length);
	tpl_free(tn);

	try {
		WriteTCPMsgToSocket(buffer, length);
	} catch (std::exception& e) {
		DebugPrint("%s \n", e.what());
		free(buffer);
		throw BLED112Exception("BLED112 Error: GetStatus failed");
	}
	free(buffer); //tpl allocates data in heap with malloc, so must be freed

}

//==============================================================================
void BLED112::BleCmdConnectionUpdate(uint8 connectionHandle,
		uint16 connIntervalMin, uint16 connIntervalMax, uint16 slaveLatency,
		uint16 timeout) { //throws exception
//==============================================================================

	size_t length = 0;
	tpl_node* tn;
	char* buffer;
	uint16_t id = ble_cmd_connection_update_idx;

	if ((tn = tpl_map((char*) "vcvvvv", &id, &connectionHandle,
			&connIntervalMin, &connIntervalMax, &timeout, &slaveLatency))
			== NULL) { //serialize to  bytestream

		DebugPrint("error %d, %d:%s:%d could not serialize data: %s\n ", errno,
				GetClientSocketDesc(), GetIPv4Address(), GetClientPort(),
				strerror(errno));

		throw BLED112Exception("BLED112 Error: Update Serialization failed");

	}
	tpl_pack(tn, 0);
	tpl_dump(tn, TPL_MEM, &buffer, &length);
	tpl_free(tn);

	try {
		WriteTCPMsgToSocket(buffer, length);
	} catch (std::exception& e) {
		DebugPrint("%s \n", e.what());
		free(buffer);
		throw BLED112Exception("BLED112 Error: Update failed");
	}
	free(buffer); //tpl allocates data in heap with malloc, so must be freed

}

//============================================================================== Triggers a connection disconnected event
void BLED112::BleCmdConnectionDisconnect(uint8 connectionHandle) { //throws exception
//==============================================================================

	size_t length = 0;
	tpl_node* tn;
	char* buffer;
	uint16_t id = ble_cmd_connection_disconnect_idx;

	if ((tn = tpl_map((char*) "vc", &id, &connectionHandle)) == NULL) { //serialize to  bytestream

		DebugPrint("error %d, %d:%s:%d could not serialize data: %s\n ", errno,
				GetClientSocketDesc(), GetIPv4Address(), GetClientPort(),
				strerror(errno));

		throw BLED112Exception(
				"BLED112 Error: Disconnect Serialization failed");

	}
	tpl_pack(tn, 0);
	tpl_dump(tn, TPL_MEM, &buffer, &length);
	tpl_free(tn);

	try {
		WriteTCPMsgToSocket(buffer, length);
	} catch (std::exception& e) {
		DebugPrint("%s \n", e.what());
		free(buffer);
		throw BLED112Exception("BLED112 Error: Disconnect failed");
	}
	free(buffer); //tpl allocates data in heap with malloc, so must be freed

}

//============================================================================== Triggers a group found event and procedure completed event
void BLED112::BleCmdGattFindPrimaryServices(uint8 connectionHandle) { //throws exception //uuid 2800 type. Secondary services not supported
//==============================================================================

	//search for all possible handles:
	uint16 start = GATT_MIN_HANDLE;
	uint16 end = GATT_MAX_HANDLE;

	uint8 typeUUID[2] = { 0x00, 0x28 };

	size_t length = 0;
	tpl_node* tn;
	tpl_bin tb;
	char* buffer;
	uint16_t id = ble_cmd_attclient_read_by_group_type_idx;

	if ((tn = tpl_map((char*) "vcvvB", &id, &connectionHandle, &start, &end,
			&tb)) == NULL) { //serialize to  bytestream

		DebugPrint("error %d, %d:%s:%d could not serialize data: %s\n ", errno,
				GetClientSocketDesc(), GetIPv4Address(), GetClientPort(),
				strerror(errno));

		throw BLED112Exception(
				"BLED112 Error: FindPrimaryServices Serialization failed");

	}

	tb.sz = 2;
	tb.addr = typeUUID;

	tpl_pack(tn, 0);
	tpl_dump(tn, TPL_MEM, &buffer, &length);
	tpl_free(tn);

	try {
		WriteTCPMsgToSocket(buffer, length);
	} catch (std::exception& e) {
		DebugPrint("%s \n", e.what());
		free(buffer);
		throw BLED112Exception("BLED112 Error: FindPrimaryServices failed");
	}
	free(buffer); //tpl allocates data in heap with malloc, so must be freed

}

//==============================================================================
void BLED112::BleCmdGattFindCharacteristics(uint8 connectionHandle, //Warning: This function has not been tested and is normally not used
		uint16 startHandle, uint16 endHandle) { //throws exception //uuid 2803 type.
//==============================================================================

	//TODO: Does NOT work yet! This function spawns group_type_found events (BleEvtGattServiceFound), that event callback function needs to
	//be adapted, because for now it assumes that it only is called if primary services are found (not characteristics!)

	uint16 typeUUID = CHARACTERISTIC;

	size_t length = 0;
	tpl_node* tn;
	tpl_bin tb;
	char* buffer;
	uint16_t id = ble_cmd_attclient_read_by_group_type_idx;

	if ((tn = tpl_map((char*) "vcvvB", &id, &connectionHandle, &startHandle,
			&endHandle, &tb)) == NULL) { //serialize to  bytestream
		DebugPrint("error %d, %d:%s:%d could not serialize data: %s\n ", errno,
				GetClientSocketDesc(), GetIPv4Address(), GetClientPort(),
				strerror(errno));
		throw BLED112Exception(
				"BLED112 Error: FindCharacteristics Serialization failed");

	}

	tb.sz = 2;
	tb.addr = &typeUUID;

	tpl_pack(tn, 0);
	tpl_dump(tn, TPL_MEM, &buffer, &length);
	tpl_free(tn);

	try {
		WriteTCPMsgToSocket(buffer, length);
	} catch (std::exception& e) {
		DebugPrint("%s \n", e.what());
		free(buffer);
		throw BLED112Exception("BLED112 Error: FindCharacteristics failed");
	}
	free(buffer); //tpl allocates data in heap with malloc, so must be freed

}

//==============================================================================
void BLED112::BleCmdGattFindAttributeInfo(uint8 connectionHandle,
		uint16 startHandle, uint16 endHandle) { //throws exception
//==============================================================================

	size_t length = 0;
	tpl_node* tn;
	char* buffer;
	uint16_t id = ble_cmd_attclient_find_information_idx;

	if ((tn = tpl_map((char*) "vcvv", &id, &connectionHandle, &startHandle,
			&endHandle)) == NULL) { //serialize to  bytestream
		DebugPrint("error %d, %d:%s:%d could not serialize data: %s\n ", errno,
				GetClientSocketDesc(), GetIPv4Address(), GetClientPort(),
				strerror(errno));
		throw BLED112Exception(
				"BLED112 Error: FindAttributeInfo Serialization failed");

	}

	tpl_pack(tn, 0);
	tpl_dump(tn, TPL_MEM, &buffer, &length);
	tpl_free(tn);

	try {
		WriteTCPMsgToSocket(buffer, length);
	} catch (std::exception& e) {
		DebugPrint("%s \n", e.what());
		free(buffer);
		throw BLED112Exception("BLED112 Error: FindAttributeInfo failed");
	}
	free(buffer); //tpl allocates data in heap with malloc, so must be freed

}

//==============================================================================
void BLED112::BleCmdGattReadAttributeByHandle(uint8 connectionHandle,
		uint16 attributeHandle) { //throws exception
//==============================================================================

	if (connectedDevices.count(connectionHandle) == 0) {
		throw BLED112Exception(
				"Cannot read attribute by handle from a non-existent connection");
	}

	size_t length = 0;
	tpl_node* tn;
	char* buffer;
	uint16_t id = ble_cmd_attclient_read_by_handle_idx;

	if ((tn = tpl_map((char*) "vcv", &id, &connectionHandle, &attributeHandle))
			== NULL) { //serialize to  bytestream

		DebugPrint("error %d, %d:%s:%d could not serialize data: %s\n ", errno,
				GetClientSocketDesc(), GetIPv4Address(), GetClientPort(),
				strerror(errno));

		throw BLED112Exception("BLED112 Error: Serialization failed");

	}

	tpl_pack(tn, 0);
	tpl_dump(tn, TPL_MEM, &buffer, &length);
	tpl_free(tn);

	try {
		WriteTCPMsgToSocket(buffer, length);
	} catch (std::exception& e) {
		DebugPrint("%s \n", e.what());
		free(buffer);
		throw BLED112Exception("BLED112 Error: ReadAttributeByHandle failed");
	}
	free(buffer); //tpl allocates data in heap with malloc, so must be freed

}

//==============================================================================
void BLED112::BleCmdGattWriteAttributeByHandle(uint8 connectionHandle,
		uint16 attributeHandle, char* dataBuffer, uint32_t length) { //throws exception
//==============================================================================

	tpl_bin tb;
	tpl_node* tn;
	char* buffer;
	uint16_t id = ble_cmd_attclient_attribute_write_idx;
	if ((tn = tpl_map((char*) "vcvB", &id, &connectionHandle, &attributeHandle,
			&tb)) == NULL) { //serialize to  bytestream

		DebugPrint("error %d, %d:%s:%d could not serialize data: %s\n ", errno,
				GetClientSocketDesc(), GetIPv4Address(), GetClientPort(),
				strerror(errno));

		throw BLED112Exception(
				"BLED112 Error: WriteAttributeByHandle Serialization failed");

	}
	tb.addr = dataBuffer;
	tb.sz = length;

	tpl_pack(tn, 0);
	tpl_dump(tn, TPL_MEM, &buffer, &length);
	tpl_free(tn);

	try {
		WriteTCPMsgToSocket(buffer, length);
	} catch (std::exception& e) {
		DebugPrint("%s \n", e.what());
		free(buffer);
		throw BLED112Exception("BLED112 Error: WriteAttributeByHandle failed");
	}

	free(buffer); //tpl allocates data in heap with malloc, so must be freed

}

//---------- Callback Functions ------------------------------------------------
//==============================================================================
void BLED112::BleRspSystemGetInfo(char* SerializeMsgData, uint32_t length) { //throws exception
//==============================================================================

	DebugPrint("%s\n", "Response: SystemGetInfo");
//deserialize
	tpl_node *tn;

	uint16_t commandIdx = 0;
	ble_msg_system_get_info_rsp_t getInfoMsg;
	if ((tn = tpl_map((char*) "vvvvvvcc", &commandIdx, &getInfoMsg.build,&getInfoMsg.ll_version,&getInfoMsg.major,&getInfoMsg.minor,&getInfoMsg.patch,&getInfoMsg.hw,&getInfoMsg.protocol_version)) == NULL) {

		DebugPrint("error %d, %d:%s:%d could not deserialize data: %s \n",
				errno, GetClientSocketDesc(), GetIPv4Address(), GetClientPort(),
				strerror(errno));

		throw BLED112Exception(
				"BLED112 Error: RspSystemGetInfo Deserialization failed");
	}
	tpl_load(tn, TPL_MEM, SerializeMsgData, length);
	tpl_unpack(tn, 0);
	tpl_free(tn);


	this->bledInfo.build = getInfoMsg.build;
	this->bledInfo.hw = getInfoMsg.hw;
	this->bledInfo.ll_version = getInfoMsg.ll_version;
	this->bledInfo.d_major = getInfoMsg.major;
	this->bledInfo.d_minor = getInfoMsg.minor;
	this->bledInfo.patch = getInfoMsg.patch;
	this->bledInfo.protocol_version = getInfoMsg.protocol_version;

	PostCompletionEvent(SYS_GET_INFO_RSP_EVT, 0, 0);

}

//==============================================================================
void BLED112::BleRspGapConnectDirect(char* SerializeMsgData, uint32_t length) { //throws exception
//==============================================================================
	DebugPrint("%s\n", "Response: Connect");
//deserialize
	tpl_node *tn;

	uint16_t commandIdx = 0;
	ble_msg_gap_connect_direct_rsp_t msg;

	if ((tn = tpl_map((char*) "vvc", &commandIdx, &(msg.result),
			&(msg.connection_handle))) == NULL) {
		DebugPrint("error %d, %d:%s:%d could not deserialize data: %s \n",
				errno, GetClientSocketDesc(), GetIPv4Address(), GetClientPort(),
				strerror(errno));
		throw BLED112Exception("BLED112 Error: Deserialization failed");
	}
	tpl_load(tn, TPL_MEM, SerializeMsgData, length);
	tpl_unpack(tn, 0);

	tpl_free(tn);

	PostCompletionEvent(GAP_CONNECTION_RSP_EVT, 0, msg.result);

}

//==============================================================================
void BLED112::BleRspConnectionGetStatus(char* SerializeMsgData,
		uint32_t length) { //throws exception
//==============================================================================
	DebugPrint("%s\n", "Response: ConnectionGetStatus");
//deserialize
	tpl_node *tn;

	uint16_t commandIdx = 0;
	ble_msg_connection_get_status_rsp_t msg;

	if ((tn = tpl_map((char*) "vc", &commandIdx, &(msg.connection))) == NULL) {

		DebugPrint("error %d, %d:%s:%d could not deserialize data: %s \n",
				errno, GetClientSocketDesc(), GetIPv4Address(), GetClientPort(),
				strerror(errno));
		throw BLED112Exception("BLED112 Error: Deserialization failed");

	}

	tpl_load(tn, TPL_MEM, SerializeMsgData, length);
	tpl_unpack(tn, 0);
	tpl_free(tn);

	PostCompletionEvent(CONNECTION_GET_STATUS_RSP_EVT, msg.connection, 0);

}

//==============================================================================
void BLED112::BleRspConnectionDisconnect(char* SerializeMsgData,
		uint32_t length) { //throws exception
//==============================================================================
	DebugPrint("%s\n", "Response: Disconnect");
//deserialize
	tpl_node *tn;

	uint16_t commandIdx = 0;
	ble_msg_connection_disconnect_rsp_t msg;
	uint16_t result = 1;
	uint8_t con = 0;

	if ((tn = tpl_map((char*) "vcv", &commandIdx, &con, &result)) == NULL) {
		DebugPrint("error %d, %d:%s:%d could not deserialize data: %s \n",
				errno, GetClientSocketDesc(), GetIPv4Address(), GetClientPort(),
				strerror(errno));

		throw BLED112Exception("BLED112 Error: Deserialization failed");
	}
	tpl_load(tn, TPL_MEM, SerializeMsgData, length);
	tpl_unpack(tn, 0);

	msg.result = result;
	msg.connection = con;

	tpl_free(tn);

	PostCompletionEvent(CONNECTION_DISCONNECT_RSP_EVT, msg.connection,
			msg.result);
}

//==============================================================================
void BLED112::BleRspSystemGetMaxConnections(char* SerializeMsgData,
		uint32_t length) { //throws exception
//==============================================================================

	DebugPrint("%s\n", "Response: GetMaxConnections");
//deserialize
	tpl_node *tn;

	uint16_t commandIdx = 0;
	ble_msg_system_get_connections_rsp_t msg;

	if ((tn = tpl_map((char*) "vc", &commandIdx, &(msg.maxconn))) == NULL) {
		DebugPrint("error %d, %d:%s:%d could not deserialize data: %s \n",
				errno, GetClientSocketDesc(), GetIPv4Address(), GetClientPort(),
				strerror(errno));

		throw BLED112Exception("BLED112 Error: Deserialization failed");
	}
	tpl_load(tn, TPL_MEM, SerializeMsgData, length);
	tpl_unpack(tn, 0);
	tpl_free(tn);

	this->bledInfo.maxConnections = msg.maxconn;

	PostCompletionEvent(SYS_GET_MAXCON_RSP_EVT, 0, 0);
}

//==============================================================================
void BLED112::BleSimpleResponse(char* msgData, uint32_t length) { //throws exception // simple responses with vS(v). structs need to have result feel
//==============================================================================

	tpl_node *tn;

	uint16_t commandIdx = 0;
	uint16_t result = 0;

	if ((tn = tpl_map((char*) "vv", &commandIdx, &result)) == NULL) {
		DebugPrint("error %d, %d:%s:%d could not deserialize data: %s\n ",
				errno, GetClientSocketDesc(), GetIPv4Address(), GetClientPort(),
				strerror(errno));

		throw BLED112Exception(
				"BLED112 Error: SimpleResponse Deserialization failed");
	}
	tpl_load(tn, TPL_MEM, msgData, length); //message data doesnt need a free
	tpl_unpack(tn, 0);
	tpl_free(tn);

	switch (commandIdx) {

	case ble_rsp_gap_discover_idx: {

		DebugPrint("Response error code %d, %d:%s:%d at start discovery\n",
				result, GetClientSocketDesc(), GetIPv4Address(),
				GetClientPort());

		//For Processes that wait for a response to the Discover Command
		PostCompletionEvent(GAP_DISCOVER_RSP_EVT, 0, result); //0 if successful

		break;
	}
	case ble_rsp_gap_set_scan_parameters_idx: {

		DebugPrint(
				"Response error code %d, %d:%s:%d at update scan parameters\n",
				result, GetClientSocketDesc(), GetIPv4Address(),
				GetClientPort());

		PostCompletionEvent(GAP_SET_SCANPARAMETER_RSP_EVT, 0, result);

		break;
	}
	case ble_rsp_gap_end_procedure_idx: {

		DebugPrint("Response error code %d, %d:%s:%d at GAP end procedure\n",
				result, GetClientSocketDesc(), GetIPv4Address(),
				GetClientPort());

		PostCompletionEvent(GAP_ENDPROCEDURE_RSP_EVT, 0, result);

		break;
	}

	}
}

//==============================================================================

void BLED112::BleSimpleConnectionResponse(char* msgData, uint32_t length) { //throws exception // simple responses with vS(cv). structs need to have result feel
//==============================================================================

	tpl_node *tn;

	uint16_t commandIdx = 0;

	uint16_t result;
	uint8_t connection;

	if ((tn = tpl_map((char*) "vcv", &commandIdx, &connection, &result)) == NULL) {

		DebugPrint("error %d, %d:%s:%d could not deserialize data: %s\n ",
				errno, GetClientSocketDesc(), GetIPv4Address(), GetClientPort(),
				strerror(errno));

		throw BLED112Exception(
				"BLED112 Error: SimpleConnectionResponse Deserialization failed");
	}
	tpl_load(tn, TPL_MEM, msgData, length);
	tpl_unpack(tn, 0);
	tpl_free(tn);

	switch (commandIdx) {

	case ble_rsp_connection_update_idx: {

		DebugPrint("Response error code %d, %d:%s:%d at update connection\n",
				result, GetClientSocketDesc(), GetIPv4Address(),
				GetClientPort());

		PostCompletionEvent(CONNECTION_UPDATE_RSP_EVT, connection, result); //update of a certain connection, post that event happend

		break;
	}

	case ble_rsp_connection_disconnect_idx: {

		DebugPrint("Response error code %d, %d:%s:%d at disconnect\n", result,
				GetClientSocketDesc(), GetIPv4Address(), GetClientPort());

		PostCompletionEvent(CONNECTION_DISCONNECT_RSP_EVT, connection, result); //release all processes waiting for a disconnect response for this connection handle

		break;
	}

	case ble_rsp_attclient_read_by_group_type_idx: {

		DebugPrint("Response error code %d, %d:%s:%d at read by group type\n",
				result, GetClientSocketDesc(), GetIPv4Address(),
				GetClientPort());

		PostCompletionEvent(ATTCLIENT_READGROUP_RSP_EVT, connection, result); //0 if successful

		break;
	}
	case ble_rsp_attclient_read_by_type_idx: {

		DebugPrint("Response error code %d, %d:%s:%d at read by type\n", result,
				GetClientSocketDesc(), GetIPv4Address(), GetClientPort());

		PostCompletionEvent(ATTCLIENT_READTYPE_RSP_EVT, connection, result); //0 if successful

		break;
	}

	case ble_rsp_attclient_find_information_idx: {

		DebugPrint(
				"Response error code %d, %d:%s:%d at find attribute information\n",
				result, GetClientSocketDesc(), GetIPv4Address(),
				GetClientPort());

		PostCompletionEvent(ATTCLIENT_FINDINFO_RSP_EVT, connection, result); //0 if successful

		break;
	}

	case ble_rsp_attclient_read_by_handle_idx: {

		DebugPrint("Response error code %d, %d:%s:%d at read by handle\n",
				result, GetClientSocketDesc(), GetIPv4Address(),
				GetClientPort());

		PostCompletionEvent(ATTCLIENT_READ_RSP_EVT, connection, result);

		break;
	}

	case ble_rsp_attclient_attribute_write_idx: {

		DebugPrint("Response error code %d, %d:%s:%d at write by handle\n",
				result, GetClientSocketDesc(), GetIPv4Address(),
				GetClientPort());

		PostCompletionEvent(ATTCLIENT_WRITE_RSP_EVT, connection, result);

		break;
	}

	}

}

//==============================================================================
void BLED112::BleEvtGapScanResponse(char* msgData, uint32_t length) { //throws exception //BGAPI uses Packettype (0x00, 0x04) to distinguish normal discovery packets and scan reponse packets (active scan)
//==============================================================================

	DebugPrint("%s\n", "Event: GapScanResponse");

	tpl_node *tn;

	tpl_bin tb;

	uint16_t commandIdx = 0;
	bd_addr macAddress;
	uint8 packetType = 0, addressType = 0, bond = 0;
	int8 rssi = 0;

	if ((tn = tpl_map((char*) "vS(c#)ccccB", &commandIdx, &macAddress, MAC_SIZE,
			&rssi, &packetType, &addressType, &bond, &tb)) == NULL) {

		DebugPrint("error %d, %d:%s:%d could not deserialize data: %s\n ",
				errno, GetClientSocketDesc(), GetIPv4Address(), GetClientPort(),
				strerror(errno));

		throw BLED112Exception(
				"BLED112 Error: GapScanResponse Deserialization failed");
	}
	tpl_load(tn, TPL_MEM, msgData, length);
	tpl_unpack(tn, 0);
	tpl_free(tn);

	AdvDataPayloadMap advPayloadMap;

	uint8* dataPoiner = (uint8*) tb.addr;
	uint32_t dataSize = tb.sz;

	if (dataSize > BGAPI_PAYLOAD_SIZE) { // BLE: Gap Scan Response has payload of max 31 bytes, however, BGAPI packets may have up to 60 bytes payload
		free(tb.addr);
		throw BLED112Exception(
				"BLED112 Error: Unexpected size of BGAPI packet payload (>60 bytes)");
	}

	try {
		ParseBGAPIAdvertisementPacket(dataPoiner, dataSize, advPayloadMap); //populate map with payload of scan response packets
	} catch (BLED112Exception& e) {
		DebugPrint("%s\n ", e.what());
		free(tb.addr); //tpl allocates heap memory when unpacking tpl_bin
		throw BLED112Exception("Corrupted Scan Response Packet: Discarding");
	}

	free(tb.addr); //tpl allocates heap memory when unpacking tpl_bin

//Insert Data into the discoveredDevices map. Note [] will create a new value if it does not exist, existing data will be modified, insert() function does not modify data!

	discoveredDevices[macAddress].macAddress = macAddress;

	if (packetType == DISCOVER_STANDARD) {

		discoveredDevices[macAddress].discoverDefaultPacket.address_type =
				addressType;
		discoveredDevices[macAddress].discoverDefaultPacket.bond = bond;
		discoveredDevices[macAddress].discoverDefaultPacket.rssi = rssi;
		discoveredDevices[macAddress].discoverDefaultPacket.data =
				advPayloadMap;

	} else if (packetType == DISCOVER_SCAN_RESPONSE) {
		discoveredDevices[macAddress].scanResponsePacket.address_type =
				addressType;
		discoveredDevices[macAddress].scanResponsePacket.bond = bond;
		discoveredDevices[macAddress].scanResponsePacket.rssi = rssi;
		discoveredDevices[macAddress].scanResponsePacket.data = advPayloadMap;

	} else {
		throw BLED112Exception(
				"BLED112Error: Encountered an invalid packet type while discovering");
	}

}

//==============================================================================
void BLED112::BleEvtConnectionGetStatus(char* msgData, uint32_t length) { //throws exception //starts service discovery and char discovery when new device connected
//==============================================================================

	DebugPrint("%s\n", "Event: ConnectionGetStatus");

	tpl_node *tn;
	uint16_t commandIdx = 0, connInterval = 0, timeout = 0, latency = 0;
	bd_addr macAddress;

	uint8 connectionHandle = 0, connectionFlags = 0, bonding = 0, addressType =
			0;

	if ((tn = tpl_map((char*) "vS(c#)cccvvvc", &commandIdx, &macAddress,
			MAC_SIZE, &connectionHandle, &connectionFlags, &addressType,
			&connInterval, &timeout, &latency, &bonding)) == NULL) {

		DebugPrint("error %d, %d:%s:%d could not deserialize data: %s\n ",
				errno, GetClientSocketDesc(), GetIPv4Address(), GetClientPort(),
				strerror(errno));

		throw BLED112Exception(
				"BLED112 Error: GetStatus Deserialization failed");
	}

	tpl_load(tn, TPL_MEM, msgData, length);
	tpl_unpack(tn, 0);
	tpl_free(tn);

//Add device to connectedDevices map  connectionFlag bits: 0: connected|1: encrypted|2: completed|3: parameters_change

	if (connectionFlags & connection_connected) { //check if bit0 is 1 -> if device is connected. This is necessary because [] adds new values to maps. A disconnected device would reappear in map after every Get Status command

		//device parameters updated
		connectedDevices[connectionHandle].addressType = addressType;
		connectedDevices[connectionHandle].bonding = bonding;
		connectedDevices[connectionHandle].connInterval = connInterval; //1.25ms
		connectedDevices[connectionHandle].connectionFlags = connectionFlags;
		connectedDevices[connectionHandle].connectionHandle = connectionHandle;
		connectedDevices[connectionHandle].macAddress = macAddress;
		connectedDevices[connectionHandle].slaveLatency = latency;
		connectedDevices[connectionHandle].timeout = timeout; //10ms

		if (connectionFlags & connection_completed) { //When connection is first established (new connection)

			//post completion event with success code 0
			PostCompletionEvent(GAP_CONNECTION_EVT, 0, 0); //new connection established, stop loop in connect direct command

		} else { //when a connection is updated, note this is also called if a connected device (firmware) changes its parameters
				 //autonomously. So there could be a race condition if the client and the device update the parameters simultaneously

			PostCompletionEvent(CONNECTION_UPDATE_EVT, connectionHandle, 0); //if the connection already existed then post an Updated event with the connectionhandle.

		}

	}

}

//==============================================================================
void BLED112::BleEvtConnectionDisconnect(char* msgData, uint32_t length) { //throws exception
//==============================================================================
	DebugPrint("%s\n", "Event: Disconnect");
	tpl_node *tn;
	uint16_t commandIdx = 0;
	ble_msg_connection_disconnected_evt_t msg;

	if ((tn = tpl_map((char*) "vcv", &commandIdx, &(msg.connection),
			&(msg.reason))) == NULL) {

		DebugPrint("error %d, %d:%s:%d could not deserialize data: %s\n ",
				errno, GetClientSocketDesc(), GetIPv4Address(), GetClientPort(),
				strerror(errno));

		throw BLED112Exception(
				"BLED112 Error: ConnectionDisconnect Deserialization failed");
	}
	tpl_load(tn, TPL_MEM, msgData, length);
	tpl_unpack(tn, 0);
	tpl_free(tn);

	PostCompletionEvent(CONNECTION_DISCONNECT_EVT, msg.connection, 0); //if the device was still waiting for procedure complete, it is stopped here. Error indicates that procedure could not complete because of disconnect (f.e. discover services).
	//This is important because the server app would just continue waiting for an procedurecompleted event. However, since the device could have been disconnected prematurely it just waits forever (since no procedure completed event ever arrives).
	//It would still process the packets, however the main application logic would not be executed anymore i.e. no discovery would take place and no new connects
	//note msg.reason is not used, because 0 is used to signal a successful disconnect, the reason is ignored here


	connectedDevices.erase(msg.connection);

}

//==============================================================================
void BLED112::BleEvtGattServiceFound(char* msgData, uint32_t length) { //throws exception //is needed because it also saves the service UUID, AttributeInfoFound does not do this, so both are needed for full info
//==============================================================================

	//TODO: This function must be changed (its name is badly chosen!) if other read_by_group_type commands are executed. For now, this event function is only called,
	//when primary services are requested. If f.e. the cmd findCharacteristics is implemented, then this
	//function must differentiate between the different group types and not just assume that the type is 0x2800 (primary service) (see below)

	DebugPrint("%s\n", "Event: ServiceFound");
	tpl_node *tn;
	tpl_bin tb;
	uint16_t commandIdx = 0, start = 0, end = 0;
	uint8 connectionHandle = 0;

	if ((tn = tpl_map((char*) "vcvvB", &commandIdx, &connectionHandle, &start,
			&end, &tb)) == NULL) {

		DebugPrint("error %d, %d:%s:%d could not deserialize data: %s\n ",
				errno, GetClientSocketDesc(), GetIPv4Address(), GetClientPort(),
				strerror(errno));

		throw BLED112Exception(
				"BLED112 Error: ServiceFound Deserialization failed");
	}
	tpl_load(tn, TPL_MEM, msgData, length);
	tpl_unpack(tn, 0);
	tpl_free(tn);

	Uuid serviceUuid;
	try {
		ParseUUID((uint8*) tb.addr, tb.sz, &serviceUuid);
	} catch (BLED112Exception& e) {
		DebugPrint("%s \n", e.what());

		free(tb.addr);
		throw BLED112Exception("Failed to process service found event");

	}


	Uuid typeUuid;
	typeUuid.length = 2;
	typeUuid.addr[0] = 0x28; //primary service type
	typeUuid.addr[1] = 0x00;

	if (connectedDevices.count(connectionHandle) > 0) {
		//since events can arrive randomly, it is necessary to check whether the device is still connected, otherwise this would create a new entry in the map even though no device is actually connected anymore
		//note, there is only one thread, so no race conditions occur. All functions that act on connected devices need this!
		//printf("%s\n", "Service Found and device is connected");

		connectedDevices[connectionHandle].attributeMap[start].timestamp =
				boost::posix_time::second_clock::universal_time();

		connectedDevices[connectionHandle].attributeMap[start].connectionHandle =
				connectionHandle;
		connectedDevices[connectionHandle].attributeMap[start].serviceUuid =
				serviceUuid;
		connectedDevices[connectionHandle].attributeMap[start].uuid = typeUuid;
		connectedDevices[connectionHandle].attributeMap[start].attributeHandle =
				start;
		connectedDevices[connectionHandle].attributeMap[start].endHandle = end;
		connectedDevices[connectionHandle].attributeMap[start].description = GetGATTDescriptions(0x2800);

	}

	free(tb.addr); //tpl mallocs databuffer when deserializing

}

//==============================================================================
void BLED112::BleEvtGattAttributeValue(char* msgData, uint32_t length) { //throws exception  //value returned by read, differentiate attr by type
//==============================================================================

	DebugPrint("%s\n", "Event: AttributeValue");

	tpl_node *tn;
	tpl_bin tb;
	uint16_t commandIdx = 0, attributeHandle = 0;
	uint8 connectionHandle = 0, attributeType = 0;

	if ((tn = tpl_map((char*) "vcvcB", &commandIdx, &connectionHandle,
			&attributeHandle, &attributeType, &tb)) == NULL) {

		DebugPrint("error %d, %d:%s:%d could not deserialize data: %s\n ",
				errno, GetClientSocketDesc(), GetIPv4Address(), GetClientPort(),
				strerror(errno));
		PostCompletionEvent(ATTCLIENT_ATTRIBUTE_FOUND_EVT, connectionHandle,
				0x0001);
		throw BLED112Exception(
				"BLED112 Error: AttributeValue Deserialization failed");
	}
	tpl_load(tn, TPL_MEM, msgData, length);
	tpl_unpack(tn, 0);
	tpl_free(tn);

	uint8* pointer = (uint8*) tb.addr;

	if (connectedDevices.count(connectionHandle) > 0) { //since events can arrive randomly, it is necessary to check whether the device is still connected, otherwise this would create a new entry in the map even though no device is actually connected anymore
		//note, there is only one thread, so no race conditions occur. All functions that act on connected devices need this! A Disconnected event could have arrived before this attribute value event

		//If a device disconnects here, it poses no problem because logically the device is still connected until a disconnect event arrives. Since no TCP messages are processed here, it cannot happen that a device is deleted from the list and then added again by the following reference to the map. So the connectedDevice map won't have an inconsistent state
		//Remember it would be problematic if a device is added to the connectedDevices map after an disconnect event (where it was removed) while an attribute value event is still in transmission. Note, map[xyz] adds a new entry if it doesn't find one!

		//Update rawData of Attribute

		connectedDevices[connectionHandle].attributeMap[attributeHandle].timestamp =
				boost::posix_time::second_clock::universal_time();

		connectedDevices[connectionHandle].attributeMap[attributeHandle].connectionHandle =
				connectionHandle;
		connectedDevices[connectionHandle].attributeMap[attributeHandle].attributeHandle =
				attributeHandle;

		connectedDevices[connectionHandle].attributeMap[attributeHandle].attributeType =
						attributeType;

		std::vector<uint8_t>* dataVect =
				&connectedDevices[connectionHandle].attributeMap[attributeHandle].rawData;
		dataVect->clear(); //clear old value

		for (unsigned int i = 0; i < tb.sz; i++) {
			dataVect->push_back(*pointer);

			pointer++;

		}

	}

	free(tb.addr);  //tpl mallocs databuffer when deserializing

	PostCompletionEvent(ATTCLIENT_ATTRIBUTE_FOUND_EVT, connectionHandle, 0);

}

//==============================================================================
void BLED112::BleEvtGattProcedureCompleted(char* msgData, uint32_t length) { //throws exception
//==============================================================================
	DebugPrint("%s\n", "Event: ProcedureCompleted");
	tpl_node *tn;
	uint16_t commandIdx = 0;
	ble_msg_attclient_procedure_completed_evt_t msg;

	if ((tn = tpl_map((char*) "vcvv", &commandIdx, &msg.connection, &msg.result,
			&msg.chrhandle)) == NULL) {

		DebugPrint("error %d, %d:%s:%d could not deserialize data: %s\n ",
				errno, GetClientSocketDesc(), GetIPv4Address(), GetClientPort(),
				strerror(errno));

		throw BLED112Exception(
				"BLED112 Error: ProcedureCompleted Deserialization failed");
	}
	tpl_load(tn, TPL_MEM, msgData, length);
	tpl_unpack(tn, 0);
	tpl_free(tn);

	PostCompletionEvent(GATT_PROCEDURE_COMPLETE_EVT, msg.connection,
			msg.result); //post + deregisters the events
//msg.result = 0 if successful

}

//==============================================================================
void BLED112::BleEvtGattAttributeInfoFound(char* msgData, uint32_t length) { //throws exception //value returned by read, differentiate attr by type, if no more to be found, procedure completed event.
//==============================================================================
	DebugPrint("%s\n", "Event: AttributeInfoFound");
	tpl_node *tn;
	tpl_bin tb;
	uint16_t commandIdx = 0, attributeHandle = 0;
	uint8 connectionHandle = 0;

	if ((tn = tpl_map((char*) "vcvB", &commandIdx, &connectionHandle,
			&attributeHandle, &tb)) == NULL) {

		DebugPrint("error %d, %d:%s:%d could not deserialize data: %s\n ",
				errno, GetClientSocketDesc(), GetIPv4Address(), GetClientPort(),
				strerror(errno));

		throw BLED112Exception(
				"BLED112 Error: AttributeInfoFound Deserialization failed");
	}
	tpl_load(tn, TPL_MEM, msgData, length);
	tpl_unpack(tn, 0);
	tpl_free(tn);

	Uuid uuid;
	try {
		ParseUUID(((uint8_t*) tb.addr), tb.sz, &uuid); // to little endian
	} catch (BLED112Exception& e) {
		DebugPrint("%s \n", e.what());
		free(tb.addr);
		throw BLED112Exception("Failed to process service found event");

	}

	if (connectedDevices.count(connectionHandle) > 0) { //since events can arrive randomly, it is necessary to check whether the device is still connected, otherwise this would create a new entry in the map even though no device is actually connected anymore
		//note, there is only one thread, so no race conditions occur. All functions that act on connected devices need this!
		//Do not check for attribute handle because non service attributes are created here
		connectedDevices[connectionHandle].attributeMap[attributeHandle].connectionHandle =
				connectionHandle;

		if(uuid.length ==2){

			unsigned short ui = 0;
			ui |= uuid.addr[0] << 8;
			ui |= uuid.addr[1];
			connectedDevices[connectionHandle].attributeMap[attributeHandle].description = GetGATTDescriptions(ui);
		}



		connectedDevices[connectionHandle].attributeMap[attributeHandle].uuid =
				uuid;

		connectedDevices[connectionHandle].attributeMap[attributeHandle].timestamp =
					boost::posix_time::second_clock::universal_time();
		connectedDevices[connectionHandle].attributeMap[attributeHandle].attributeHandle =
						attributeHandle;
	}

	free(tb.addr);  //tpl mallocs databuffer when deserializing
}

//==============================================================================
void BLED112::ParseUUID(uint8* rawData, uint32_t length, Uuid* uuid) { //throws exception
//==============================================================================
		//Data in packet arrives big endian, encode UUID in little endian as in BGAPI GUI Program
	if (length <= UUID_SIZE && length > 0) {

		uint8* pointer = rawData;
		rawData = rawData + length - 1;

		uuid->length = length; //there are 2 to 16 byte UUIDs

		int i = 0;

		for (; rawData >= pointer; rawData--) { //big endian to little endian

			uuid->addr[i] = *rawData;
			i++;
		}

	} else {
		throw BLED112Exception("UUID has illegal length");

	}

}

//---------- Helper Functions --------------------------------------------------

//==============================================================================
void BLED112::ParseBGAPIAdvertisementPacket(uint8* rawData, uint32_t dataLength,
		AdvDataPayloadMap &dataMap) { //throws exception
//==============================================================================

	//Packet needs to be at least 1 byte long. Or they are discarded

	dataMap.clear();
	for (uint i = 0; i < dataLength;) {

		uint8 length = rawData[i]; //first byte denotes length of following data, including type byte

		if (i + length >= dataLength) { //overflow
			throw BLED112Exception(
					"BLED112Error: Data in advertisement packet invalid: overflow");
		} else if (length <= 0) { //it is possible that the length byte is 0

			throw BLED112Exception(
					"BLED112Error: Data in advertisement packet invalid: zero length byte");
		}
		uint8 type = *(rawData + i + 1);

		AdvertisementPayload dataArray;
		dataArray.length = length - 1; //data without the type byte

		memcpy(dataArray.data, (rawData + i + 2), (length - 1) * sizeof(uint8));

		dataMap[type] = dataArray;

		i = i + length + 1;

	}

}

//==============================================================================
void BLED112::SendSimpleCommand(uint16_t command_index,
		const char* serializeFormat) { //throws exception
//==============================================================================

//serializing data for transmission over sockets and little-endian to big-endian conversion
	uint16_t id = command_index;
	char* buffer;
	size_t length = 0;
	tpl_node* tn;
	if ((tn = tpl_map((char*) serializeFormat, &id)) == NULL) { //serialize to "id|NULL" bytestream

		DebugPrint("error %d, %d:%s:%d could not serialize data: %s \n", errno,
				GetClientSocketDesc(), GetIPv4Address(), GetClientPort(),
				strerror(errno));

		throw BLED112Exception("BLED112 Error: Serialization failed");

	}
	tpl_pack(tn, 0);
	tpl_dump(tn, TPL_MEM, &buffer, &length);
	tpl_free(tn);


	WriteTCPMsgToSocket(buffer, length);

	free(buffer); //tpl allocates data in heap with malloc, so must be freed

}

//==============================================================================
bool BLED112::CheckConnectionParameters(uint16_t& connIntervalMin,
		uint16_t& connIntervalMax, uint16_t& timeout, uint16_t& slaveLatency) {
//==============================================================================

	bool correct = true;

	if (connIntervalMin < 6 || connIntervalMax > 3200
			|| connIntervalMax - 0x10 < connIntervalMin) { //For some reason BlueGiga always uses  difference of 0x10

		connIntervalMax = CONNECT_INTERVAL;
		connIntervalMin = CONNECT_INTERVAL - 0x10;
		correct = false;

	}
	if (timeout < 10 || timeout > 3200
			|| timeout * 10 < connIntervalMax * 1.25) {

		timeout = CONNECT_TIMEOUT;
		connIntervalMax = CONNECT_INTERVAL;
		connIntervalMin = CONNECT_INTERVAL;

		correct = false;
	}
	if (slaveLatency < 0 || slaveLatency > 500
			|| slaveLatency * connIntervalMax * 1.25 > timeout * 10) {

		timeout = CONNECT_TIMEOUT;
		connIntervalMax = CONNECT_INTERVAL;
		connIntervalMin = CONNECT_INTERVAL;
		slaveLatency = CONNECT_SAVE_LATENCY;
		correct = false;
	}

	return correct;
}

//==============================================================================
bool BLED112::CheckDiscoverParameters(gap_discover_mode& discoverMode,
		unsigned short& scanInterval, unsigned short& scanWindow,
		bool& activeScan) {
//==============================================================================

//units of 625 microseconds
//scanInterval  0x4 - 0x4000, Default 0x4B 75ms
//scanWindow  0x4 - 0x4000, Default 0x32  50ms
//active positive or 0

//scanWindow <= scanInterval

	bool correct = true;

	if (!(scanInterval >= 0x4 && scanInterval <= 0x4000 && scanWindow >= 0x4
			&& scanWindow <= scanInterval)) {

		scanInterval = DISCOVERY_SCAN_INTERVAL;
		scanWindow = DISCOVERY_SCAN_WINDOW;

		correct = false;
	}

	if (!(discoverMode >= 0 && discoverMode <= 3)) {

		discoverMode = gap_discover_generic;
		correct = false;

	}

	return correct;

}

//==============================================================================
bool BLED112::MACToConnectionHandle(bd_addr& mac, uint8_t& connectionHandle) {
//==============================================================================

	for (auto c : connectedDevices) { //O(n) however maximum nr of connections is 8 per dongle

		if (c.second.macAddress == mac) {

			connectionHandle = c.first;
			return true;

		}

	}

	return false; //not found

}

//==============================================================================
void BLED112::RegisterForCompletionEvent(int ce, ConnectionEventStruct* es) { //not thread-safe
//==============================================================================
	completionEventRegister.push_back(std::make_pair(ce, es));
}

//==============================================================================
ConnectionEventStruct BLED112::WaitForCompletionEvent(int ce, uint8 ch,
		unsigned short waitTime) {
//==============================================================================
//ch can be = 0 if only one event can take place at a time. Z.b connect. The dongle needs to finish connecting before another connect can take place
//so setting it to 0 is not problematic
	int eventOccurred = 0;

	ConnectionEventStruct conEvStr;
	conEvStr.connectionHandle = ch;
	conEvStr.eventHandle = &eventOccurred;

	RegisterForCompletionEvent(ce, &conEvStr);

	time_t begTime = std::time(NULL);
	while (!eventOccurred && isConnected
			&& std::difftime(time(NULL), begTime) < waitTime) {

		try {
			ProcessTCPMsgFromSocket(); //continue processing data from sensors. if procedure completed event occurs, event occured is set to true.
		} catch (BLED112Exception& e) {
			DebugPrint("%s \n", e.what());
		}
	}

	if (!eventOccurred) { //if event has not occured yet
		PostCompletionEvent(ce, ch, 0x0002); //release event with timeout error
	}

	return conEvStr;

}

//==============================================================================
void BLED112::PostCompletionEvent(int ce, uint8 ch, int code) {
//==============================================================================

	int i = 0;
	for (auto &e : completionEventRegister) {

		if ((e.first & ce) && e.second->connectionHandle == ch) { //CE form bitflags, allows for combination of events, bitwise AND checks if at least one flag is set
			*(e.second->eventHandle) = 1; //release from spin wait
			e.second->errorCode = code;

			completionEventRegister.erase(completionEventRegister.begin() + i);

		}
		i++;

	}

}

}

