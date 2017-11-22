#include "Headers/BLE_GATEWAY.h"

std::atomic<bool> globalTerminate(false);
std::atomic<bool> serverTerminate(false);
int serverSocketDesc;

//==============================================================================
int main(int argc, char* args[]) {
//==============================================================================

	DebugPrint("%s\n", "Gateway application start\n");
	//register signal handler
	signal(SIGINT, signalHandler);

	//Start Acceptor Thread
	std::thread server(BLEGateway::ServerThread);

	//Start FCGI Threads

	for (uint i = 0; i < BLEGateway::FCGI_THREADS; i++) {
		std::thread fcgit(BLEGateway::FCGIWorker, i);
		fcgit.detach(); //not joinable
	}

	DebugPrint("%s\n", "Server opened");

	server.join();

	//terminate all detached threads
	globalTerminate = true;
	DebugPrint("%s\n", "Server closed");

	return 1;

}
//==============================================================================
void signalHandler(int signum) {
//==============================================================================
	DebugPrint("Signal: %d received, shut down\n", signum);
	serverTerminate = true;
	globalTerminate = true;
	exit(signum);

}

namespace BLEGateway {

//map with dongle id -> WorkerThread struct. Globally accessible list of all currently connected dongles
WorkerThreads workers;

//shared lock used for read an write of workers map
std::shared_timed_mutex workerMapMut;

//RAII Paradigm to enforce exception safety on locks
//==============================================================================
void AddBLEWorker(BledID bid, WorkerThread wt) {
//==============================================================================

	std::unique_lock<std::shared_timed_mutex> workerLock(workerMapMut,
			std::defer_lock);
	workers[bid] = wt;

	//end of scope removes lock (exception safe)
}
//==============================================================================
void RemoveBLEWorker(BledID bid) {
//==============================================================================
	std::unique_lock<std::shared_timed_mutex> workerLock(workerMapMut,
			std::defer_lock);
	workers.erase(bid);

	//end of scope removes lock (exception safe)
}
//==============================================================================
bool PushToWorkerQueue(BledID bid, FCGX_Request* wr) { //possible cache ping pong among reader Threads -> padding?
//==============================================================================
	bool success = false;
	std::shared_lock<std::shared_timed_mutex> workerLock(workerMapMut,
			std::defer_lock); //shared, we only read from map

	if (workers.count(bid) > 0) { //avoid that [""] adds a new value to the map if bid doesn0t exists (would not be threadsafe!)

		success = workers[bid].workQueue->push(wr); //lockfree thread safe

	}

	return success;
	//end of scope removes lock (exception safe)
}
//==============================================================================
bool PopFromWorkerQueue(BledID bid, FCGX_Request** wr) { //possible cache ping pong among reader Threads -> padding?
//==============================================================================
	bool success = false;
	std::shared_lock<std::shared_timed_mutex> workerLock(workerMapMut,
			std::defer_lock); //shared, we only read from map

	if (workers.count(bid) > 0) { //avoid that [""] adds a new value to the map if bid doesn't exists (would not be threadsafe!)

		success = workers[bid].workQueue->pop(*wr); //lockfree thread safe

	}

	return success;
	//end of scope removes lock (exception safe)
}
//==============================================================================
void ServerThread() {
//==============================================================================
	int clientSocketDesc;
	struct sockaddr_in server, client;
	int addrlen = sizeof(struct sockaddr_in);
	char serverIP4[INET_ADDRSTRLEN];
	BledID bid = 0;

	if ((serverSocketDesc = socket(AF_INET, SOCK_STREAM, 0)) < 0) {

		perror("error: could not create server socket");
	} else {

		//Defines TCP Sockets for BLE Dongles. Could be changed to UNIX Sockets. Also define special IP-Address Requirements here

		server.sin_family = AF_INET;
		server.sin_addr.s_addr = INADDR_ANY; //make all interfaces available (including localhost)
		server.sin_port = htons(SERVER_PORT); //network byte order of port number

		if (bind(serverSocketDesc, (struct sockaddr *) &server, sizeof(server))
				< 0) {
			perror(
					"error: could not bind server socket to an address and port");
		} else {

			inet_ntop(AF_INET, &(server.sin_addr), serverIP4, INET_ADDRSTRLEN); //retrieve server IP address

			listen(serverSocketDesc, MAX_PENDING_CONNECTIONS); //listen to connections

			fprintf(stdout, "Waiting for connections at %s:%d \n ", serverIP4,
					SERVER_PORT);

			while ((clientSocketDesc = accept(serverSocketDesc,
					(struct sockaddr *) &client, (socklen_t*) &addrlen))
					&& !serverTerminate) {

				if (clientSocketDesc > 0) {

					try {
						//create new BLEWorker Thread when Dongle client program tries to connect
						std::thread session(BLEWorker, clientSocketDesc, bid++,
								client);
						session.detach();
					} catch (std::exception& e) {
						close(clientSocketDesc);
						perror(e.what());
					}

				} else {
					perror("error: server could not accept connection");
				}

			}
		}
		close(serverSocketDesc);
	}
}
//==============================================================================
void BLEWorker(int clientSocketDesc, BledID bid, sockaddr_in addressInfo) {
//==============================================================================

	fprintf(stdout, "Worker Thread for Dongle %d started\n ", bid);
	//Create BLED112 Obj. This connects to the BLED112 and initialized the BledInfo struct and stops any ongoing operations
	//like connect/discover. Note that the BLED112 might still be in discovery phase when connecting it to this application
	BLED112 bledObj(clientSocketDesc, addressInfo); //instantiates BLED object, sets isConnected to true
	Device* devObj = &bledObj; //use Interface functions to make it portable to other BLE Dongles

	//create Work Queue
	boost::lockfree::queue<FCGX_Request*> workQueue(WORKER_QUEUE_SZ); //fixed size, push returns false if full
	//note, queue might not be thread-safe if the size is not fixed

	//Add Thread info to workers map
	AddBLEWorker(bid, WorkerThread(devObj, &workQueue)); //Thread safe

	//Loop
	while (!globalTerminate && devObj->isConnected) { //while no global Termination or other party closed socket

		FCGX_Request* request { nullptr };
		if (PopFromWorkerQueue(bid, &request)) {

			//has request

			if (request != nullptr) {

				ExecuteWorkRequest(devObj, request); //Blocking, serializes requests

			}

		}

		try {
			devObj->ProcessTCPMsgFromSocket(); //Receive Events from BLE Devices, might set isConnected to false if EOF is read
		} catch (BLED112Exception& e) {
			DebugPrint("%s\n", e.what());
		}

	}

	//Clean up
	RemoveBLEWorker(bid); //Thread safe, first remove from worker list

	//then finish all requests which are still on the queue, notify clients that the device could not be found

	std::unordered_map<std::string, std::string> requestParameters;
	requestParameters[HTTP_CODE] = std::to_string(HTTP_404_NOT_FOUND);
	requestParameters[HTTP_STATUSCODE] = "404 Not Found";
	requestParameters[HTTP_BODY_STRING] = "Connection to BLE Device lost";

	FCGX_Request* request { nullptr };
	while (workQueue.pop(request)) {

		PostHTTPMessage(request, requestParameters);

		FCGX_Finish_r(request);
	}

	close(devObj->GetClientSocketDesc()); //try to close connection
	fprintf(stdout, "Worker Thread for Dongle %d shut down\n ", bid);
	DebugPrint("Worker Thread shut down, bid: %d\n", bid);
}
//==============================================================================
void ExecuteWorkRequest(Device* devObj, FCGX_Request* wr) {
//==============================================================================

	//Parse HTTP Variables and save in Hash Map

	std::unordered_map<std::string, std::string> requestParameters;

	//Parses URI, HTTP ACCEPT and QUERY_STRING, returns values and commandID in a string -> string map

	ProcessHTTPRequest(wr, requestParameters);

	//execute commands, wait until finished/error (finished can be done in driver using events, make command calls blocking (while still processing events from device!))
	// Device busy message? -> maybe through a State machine!

	std::string commandIdStr = requestParameters[COMMAND_ID];

	if (commandIdStr.empty()) { //in command string incorrect, should not happen
		requestParameters[COMMAND_ID] = std::to_string(ERROR);
		requestParameters[HTTP_CODE] = std::to_string(HTTP_500_INT_S_ERROR);
	}

	int commandId = std::stoi(commandIdStr, nullptr, 10);

	//Execute the command. Blocking

	switch (commandId) {

	case DONGLE_OVERVIEW: {
		//Read values from Dongle BLED112 struct, The values are set when the Dongle is first connected
		// The function for getting the info could be called again. However, for now, this implementation
		// just reads the data. If the info can be updated without reconnecting the dongle, the the data could be
		//out of date and this function SHOULD read it again using the corresponding driver function.



		Result r = devObj->UpdateBledInformation(); //Blocking


					if (r.errorCode) {
						//error when communicating with upstream server
						requestParameters[HTTP_CODE] = std::to_string(
								HTTP_502_BAD_GATEWAY);
						requestParameters[HTTP_STATUSCODE] = "502 Bad Gateway";
						requestParameters[HTTP_BODY_STRING] = r.errorMessage;

						PostHTTPMessage(wr, requestParameters);
					} else {

						requestParameters[HTTP_BODY_STRING] = r.errorMessage;
						requestParameters[HTTP_CODE] = std::to_string(HTTP_200_OK);
						requestParameters[HTTP_STATUSCODE] = "200 OK";
						PostHTTPStructData(wr, r.data, requestParameters);

					}


		break;
	}
	case BLEDEV_OVERVIEW: { //Get all Attributes of a device

		//Create MAC Address from string:
		std::string macStr = requestParameters[MAC_STRING];
		std::vector<uint8> macVec; //note: bd_addr type needs to be in little endian format. So the vector data needs to be read in reverse
		if (!ConstructValidMAC(macStr, macVec)) {
			requestParameters[HTTP_CODE] = std::to_string(HTTP_400_BAD_REQ);
			requestParameters[HTTP_STATUSCODE] = "400 Bad Request";
			requestParameters[HTTP_BODY_STRING] =
					"Not a valid MAC Address format";
			PostHTTPMessage(wr, requestParameters);
			break;
		}

		Result r = devObj->ReadAllAttributes(macVec); //Blocking

		if (r.connectionHandle == -1) {

			requestParameters[HTTP_CODE] = std::to_string(HTTP_404_NOT_FOUND);
			requestParameters[HTTP_STATUSCODE] = "404 Not Found";
			requestParameters[HTTP_BODY_STRING] = r.errorMessage;

			PostHTTPMessage(wr, requestParameters);

		} else {
			if (r.errorCode) {
				//error when communicating with upstream server
				requestParameters[HTTP_CODE] = std::to_string(
						HTTP_502_BAD_GATEWAY);
				requestParameters[HTTP_STATUSCODE] = "502 Bad Gateway";
				requestParameters[HTTP_BODY_STRING] = r.errorMessage;

				PostHTTPMessage(wr, requestParameters);
			} else {

				requestParameters[HTTP_BODY_STRING] = r.errorMessage;
				requestParameters[HTTP_CODE] = std::to_string(HTTP_200_OK);
				requestParameters[HTTP_STATUSCODE] = "200 OK";
				PostHTTPStructData(wr, r.data, requestParameters);

			}
		}

		break;
	}
	case DISCOVERED: { //list of cached discovered devices (discoverTime=0) or initiate discovery for discoverTime seconds

		int discoverMode = 0, scanInterval = 0, scanWindow = 0, activeScan = 0,
				discoverTime = 0;

		std::string discoverModeStr = requestParameters[QUERY_DISCOVER_MODE];
		std::string scanIntervalStr = requestParameters[QUERY_SCAN_INTERVAL];
		std::string scanWindowStr = requestParameters[QUERY_SCAN_WINDOW];
		std::string activeScanStr = requestParameters[QUERY_ACTIVE_SCAN];
		std::string discoverTimeStr = requestParameters[QUERY_DISCOVER_TIME];

		if (!ConstructValidInteger(discoverModeStr, discoverMode)) { //this does not ensure a legal value yet, only if it is an Integer (does not allow negative integers)! The Driver checks if the value is legal
			discoverMode = gap_discover_generic; //default value
		}

		if (!ConstructValidInteger(scanIntervalStr, scanInterval)) {
			scanInterval = DISCOVERY_SCAN_INTERVAL;
		}
		if (!ConstructValidInteger(scanWindowStr, scanWindow)) {
			scanWindow = DISCOVERY_SCAN_WINDOW;
		}

		if (!ConstructValidInteger(activeScanStr, activeScan)) {
			activeScan = DISCOVERY_ACTIVE_SCAN;
		}

		if (!ConstructValidInteger(discoverTimeStr, discoverTime)) {
			discoverTime = DISCOVER_TIME;
		}

		gap_discover_mode mode;
		switch (discoverMode) {

		case 0: {
			mode = gap_discover_limited;
			break;
		}
		case 1: {
			mode = gap_discover_generic;
			break;
		}
		case 2: {
			mode = gap_discover_observation;
			break;
		}
		default: {
			mode = gap_discover_generic; //default
			break;
		}

		}

		Result r = devObj->Discover(mode, scanInterval, scanWindow, activeScan,
				discoverTime); //Blocking, only starts discovery if discoverTime > 0

		if (r.errorCode) {
			//error when communicating with upstream server
			requestParameters[HTTP_CODE] = std::to_string(HTTP_502_BAD_GATEWAY);
			requestParameters[HTTP_STATUSCODE] = "502 Bad Gateway";
			requestParameters[HTTP_BODY_STRING] = r.errorMessage;

			PostHTTPMessage(wr, requestParameters);

		} else {
			//success

			requestParameters[HTTP_CODE] = std::to_string(HTTP_200_OK);
			requestParameters[HTTP_STATUSCODE] = "200 OK";
			requestParameters[HTTP_BODY_STRING] = r.errorMessage;

			PostHTTPStructData(wr, r.data, requestParameters); //r.data can be empty,

		}

		break;
	}
	case CONNECTED: { //Get all connected devices and their cached values

		Result r = devObj->GetConnectedDevices(); //Blocking

		if (r.errorCode) {
			//error when communicating with upstream server
			requestParameters[HTTP_CODE] = std::to_string(HTTP_502_BAD_GATEWAY);
			requestParameters[HTTP_STATUSCODE] = "502 Bad Gateway";
			requestParameters[HTTP_BODY_STRING] = r.errorMessage;

			PostHTTPMessage(wr, requestParameters);
		} else {

			requestParameters[HTTP_BODY_STRING] = r.errorMessage;
			requestParameters[HTTP_CODE] = std::to_string(HTTP_200_OK);
			requestParameters[HTTP_STATUSCODE] = "200 OK";
			PostHTTPStructData(wr, r.data, requestParameters);

		}

		break;
	}
	case CONNECT: { //connect or update a device (if already connected)

		int addressType = 0, connectionInterval = 0, timeout = 0, slaveLatency =
				0;

		std::string addressTypeStr = requestParameters[QUERY_ADDRESS_TYPE];
		std::string connectionIntervalStr =
				requestParameters[QUERY_CONNECTION_INTERVAL];
		std::string timeoutStr = requestParameters[QUERY_TIMEOUT];
		std::string slaveLatencyStr = requestParameters[QUERY_SLAVE_LATENCY];

		if (!ConstructValidInteger(addressTypeStr, addressType)) { //this does not ensure a legal value yet, only if it is an Integer (does not allow negative values)! The Driver checks if the value is legal
			addressType = CONNECT_ADDRESS_TYPE; //default value
		}

		if (!ConstructValidInteger(connectionIntervalStr, connectionInterval)) {
			connectionInterval = CONNECT_INTERVAL;
		}
		if (!ConstructValidInteger(timeoutStr, timeout)) {
			timeout = CONNECT_TIMEOUT;
		}

		if (!ConstructValidInteger(slaveLatencyStr, slaveLatency)) {
			slaveLatency = CONNECT_SAVE_LATENCY;
		}

		//Note: for now it can only be assumed that the values have the correct format. The Connect function of the driver checks if the values are legal

		//Create MAC Address from string:
		std::string macStr = requestParameters[MAC_STRING];

		std::vector<uint8> macVec; //note: bd_addr type needs to be in little endian format. So the vector data needs to be read in reverse

		if (!ConstructValidMAC(macStr, macVec)) {

			requestParameters[HTTP_CODE] = std::to_string(HTTP_400_BAD_REQ);
			requestParameters[HTTP_STATUSCODE] = "400 Bad Request";
			requestParameters[HTTP_BODY_STRING] =
					"Not a valid MAC Address format";

			PostHTTPMessage(wr, requestParameters);
			break;
		}

		Result r = devObj->Connect(macVec, addressType,
				connectionInterval - 0x10, //assume min = max connection Interval, for some reason Bluegiga assumes a difference of 0x10
				connectionInterval, timeout, slaveLatency); //Blocking

		if (r.errorCode) {
			//error when communicating with upstream server
			requestParameters[HTTP_CODE] = std::to_string(HTTP_502_BAD_GATEWAY);
			requestParameters[HTTP_STATUSCODE] = "502 Bad Gateway";
			requestParameters[HTTP_BODY_STRING] = r.errorMessage;

			PostHTTPMessage(wr, requestParameters);

		} else {

			//assume a non empty r.data vector means that an update occured, send 200 OK with device data
			//an empty r.data vector means that a new device was connected. Return a 201 CREATED, does not need a location header
			//the created resource is the effective request URI

			requestParameters[HTTP_BODY_STRING] = r.errorMessage;

			if (r.connectionHandle == -1) {
				DebugPrint("%s\n",
						"Worker Thread execute command: CONNECT created");
				//Connect, since r.data.size is 0 => 201 Create
				requestParameters[HTTP_CODE] = std::to_string(HTTP_201_CREATED);
				requestParameters[HTTP_STATUSCODE] = "201 Created";

			} else {
				DebugPrint("%s\n", "Worker Thread execute command: 200 OK");
				requestParameters[HTTP_CODE] = std::to_string(HTTP_200_OK);
				requestParameters[HTTP_STATUSCODE] = "200 OK";

			}

			PostHTTPStructData(wr, r.data, requestParameters);

		}

		break;
	}
	case DISCONNECT: {
		//Create MAC Address from string:
		std::string macStr = requestParameters[MAC_STRING];
		std::vector<uint8> macVec; //note: bd_addr type needs to be in little endian format. So the vector data needs to be read in reverse
		if (!ConstructValidMAC(macStr, macVec)) {
			requestParameters[HTTP_CODE] = std::to_string(HTTP_400_BAD_REQ);
			requestParameters[HTTP_STATUSCODE] = "400 Bad Request";
			requestParameters[HTTP_BODY_STRING] =
					"Not a valid MAC Address format";
			PostHTTPMessage(wr, requestParameters);
			break;
		}

		Result r = devObj->Disconnect(macVec); //Blocking

		if (r.errorCode) {
			//error when communicating with upstream server
			requestParameters[HTTP_CODE] = std::to_string(HTTP_502_BAD_GATEWAY);
			requestParameters[HTTP_STATUSCODE] = "502 Bad Gateway";
			requestParameters[HTTP_BODY_STRING] = r.errorMessage;

			PostHTTPMessage(wr, requestParameters);
		} else {

			requestParameters[HTTP_BODY_STRING] = r.errorMessage;
			requestParameters[HTTP_CODE] = std::to_string(HTTP_200_OK);
			requestParameters[HTTP_STATUSCODE] = "200 OK";
			PostHTTPStructData(wr, r.data, requestParameters);

		}

		break;
	}
	case READ_HANDLE: {

		std::string macStr = requestParameters[MAC_STRING];
		std::vector<uint8> macVec; //note: bd_addr type needs to be in little endian format. So the vector data needs to be read in reverse
		if (!ConstructValidMAC(macStr, macVec)) {
			requestParameters[HTTP_CODE] = std::to_string(HTTP_400_BAD_REQ);
			requestParameters[HTTP_STATUSCODE] = "400 Bad Request";
			requestParameters[HTTP_BODY_STRING] =
					"Not a valid MAC Address format";
			PostHTTPMessage(wr, requestParameters);
			break;
		}

		uint16 attributeHandle = 0;
		try {
			attributeHandle = std::stoi(requestParameters[ATTR_HANDLE_STR],
					nullptr, 10);
		} catch (std::exception& e) {

			requestParameters[HTTP_CODE] = std::to_string(HTTP_400_BAD_REQ);
			requestParameters[HTTP_STATUSCODE] = "400 Bad Request";
			requestParameters[HTTP_BODY_STRING] =
					"Not a valid Attribute Handle format";
			PostHTTPMessage(wr, requestParameters);
			break;
		}

		int lastUpdate = 0;
		std::string lastUpdateStr = requestParameters[QUERY_LAST_UPDATE];

		ConstructValidInteger(lastUpdateStr, lastUpdate);

		Result r = devObj->ReadAttribute(macVec, attributeHandle, lastUpdate); //Blocking

		if (r.connectionHandle == -1) {

			requestParameters[HTTP_CODE] = std::to_string(HTTP_404_NOT_FOUND);
			requestParameters[HTTP_STATUSCODE] = "404 Not Found";
			requestParameters[HTTP_BODY_STRING] = r.errorMessage;

			PostHTTPMessage(wr, requestParameters);

		} else {
			if (r.errorCode) {
				//error when communicating with upstream server
				requestParameters[HTTP_CODE] = std::to_string(
						HTTP_502_BAD_GATEWAY);
				requestParameters[HTTP_STATUSCODE] = "502 Bad Gateway";
				requestParameters[HTTP_BODY_STRING] = r.errorMessage;

				PostHTTPMessage(wr, requestParameters);
			} else {

				requestParameters[HTTP_BODY_STRING] = r.errorMessage;
				requestParameters[HTTP_CODE] = std::to_string(HTTP_200_OK);
				requestParameters[HTTP_STATUSCODE] = "200 OK";
				PostHTTPStructData(wr, r.data, requestParameters);

			}
		}

		break;
	}
	case WRITE_HANDLE: {

		//Create MAC Address from string:
		std::string macStr = requestParameters[MAC_STRING];
		std::vector<uint8> macVec; //note: bd_addr type needs to be in little endian format. So the vector data needs to be read in reverse
		if (!ConstructValidMAC(macStr, macVec)) {
			requestParameters[HTTP_CODE] = std::to_string(HTTP_400_BAD_REQ);
			requestParameters[HTTP_STATUSCODE] = "400 Bad Request";
			requestParameters[HTTP_BODY_STRING] =
					"Not a valid MAC Address format";
			PostHTTPMessage(wr, requestParameters);
			break;
		}

		uint16 attributeHandle = 0;
		try {
			attributeHandle = std::stoi(requestParameters[ATTR_HANDLE_STR],
					nullptr, 10);
		} catch (std::exception& e) {

			requestParameters[HTTP_CODE] = std::to_string(HTTP_400_BAD_REQ);
			requestParameters[HTTP_STATUSCODE] = "400 Bad Request";
			requestParameters[HTTP_BODY_STRING] =
					"Not a valid Attribute Handle format";
			PostHTTPMessage(wr, requestParameters);
			break;
		}

		std::string data = requestParameters[QUERY_WRITE_VALUE];

		Result r = devObj->WriteAttribute(macVec, attributeHandle, data); //Blocking

		if (r.connectionHandle == -1) {

			requestParameters[HTTP_CODE] = std::to_string(HTTP_404_NOT_FOUND);
			requestParameters[HTTP_STATUSCODE] = "404 Not Found";
			requestParameters[HTTP_BODY_STRING] = r.errorMessage;

			PostHTTPMessage(wr, requestParameters);

		} else {
			if (r.errorCode) {
				//error when communicating with upstream server
				requestParameters[HTTP_CODE] = std::to_string(
						HTTP_502_BAD_GATEWAY);
				requestParameters[HTTP_STATUSCODE] = "502 Bad Gateway";
				requestParameters[HTTP_BODY_STRING] = r.errorMessage;

				PostHTTPMessage(wr, requestParameters);
			} else {

				requestParameters[HTTP_CODE] = std::to_string(HTTP_200_OK);
				requestParameters[HTTP_STATUSCODE] = "200 OK";
				requestParameters[HTTP_BODY_STRING] = r.errorMessage;
				PostHTTPStructData(wr, r.data, requestParameters);

			}
		}

		break;
	}
	case OPTIONS: {

		PostHTTPMessage(wr, requestParameters);
		break;
	}

	case ERROR: {
		PostHTTPMessage(wr, requestParameters);

		break;
	}
	default: {
		requestParameters[HTTP_CODE] = std::to_string(HTTP_500_INT_S_ERROR);
		requestParameters[HTTP_STATUSCODE] = "500 Internal Server Error";
		requestParameters[HTTP_BODY_STRING] = "Unknown Error";
		PostHTTPMessage(wr, requestParameters);
		break;
	}

	}

	FCGX_Finish_r(wr); //Frees the Request object

}
//==============================================================================
void FCGIWorker(FcgiID fid) { //receives HTTP requests and passes them to the respective dongles or executes "global" requests
//==============================================================================
	fprintf(stdout, "FCGI Thread %d started\n ", fid);
	//Initialize FCGI Request object
	FCGX_Init();

	FCGX_Request *request = CreateRequest();

	while (!globalTerminate && FCGX_Accept_r(request) >= 0) {

		//Parse Request
		std::string uri = std::string(
				FCGX_GetParam("REQUEST_URI", request->envp));

		std::vector<std::string> uriElements;
		std::string del("/");

		std::unordered_map<std::string, std::string> requestParameters;

		ConstructSubstrings(uri, del, uriElements); //returns vector of length 0 if all substrings are empty

		int id = 0;

		//Todo: For now, the first element of the URI needs to be a dongle id! Change this to allow
		// f.e. MAC address directly. This needs additional datastructures. F.e. a
		//map that maps MAC address to dongle nr!

		if (uriElements.size() > 0) { //there is ad least one non-empty substring

			if (!ConstructValidInteger(uriElements.at(0), id)) { //it is not an integer

				requestParameters[HTTP_CODE] = std::to_string(HTTP_400_BAD_REQ);
				requestParameters[HTTP_STATUSCODE] = "400 Bad Request";
				requestParameters[HTTP_BODY_STRING] =
						"Invalid dongle number format";
				PostHTTPMessage(request, requestParameters);

				FCGX_Finish_r(request);
			} else { //it is an integer

				if (!PushToWorkerQueue(id, request)) { //try to put it on the work queue of id Worker. If fails (because doesn't exist) return error

					requestParameters[HTTP_CODE] = std::to_string(
							HTTP_404_NOT_FOUND);
					requestParameters[HTTP_STATUSCODE] = "404 Not Found";
					requestParameters[HTTP_BODY_STRING] =
							"Dongle number not found";
					PostHTTPMessage(request, requestParameters);

					FCGX_Finish_r(request);

				}

			}

		} else { //Not dongle specific

			//Process "global commands"
			ProcessHTTPRequest(request, requestParameters);

			std::string commandIdStr = requestParameters[COMMAND_ID];

			if (commandIdStr.empty()) { //in command string incorrect, should not happen
				requestParameters[COMMAND_ID] = std::to_string(ERROR);
				requestParameters[HTTP_CODE] = std::to_string(
						HTTP_500_INT_S_ERROR);
			}

			int commandId = std::stoi(commandIdStr, nullptr, 10);

			switch (commandId) {

			case GW_OVERVIEW: { //overview over connected dongles

				std::vector<BledID> bids = GetConnectedDongleIDs();
				std::vector<std::pair<std::string, std::string> > bidStrs;
				for (auto& i : bids) {

					bidStrs.push_back(
							std::pair<std::string, std::string>("DONGLE_ID",
									std::to_string(i)));

				}

				requestParameters[HTTP_CODE] = std::to_string(HTTP_200_OK);
				requestParameters[HTTP_STATUSCODE] = "200 OK";
				requestParameters[HTTP_BODY_STRING] = "Connected Dongles";

				PostHTTPVectorData(request, bidStrs, requestParameters);

				break;
			}

			default: {
				PostHTTPMessage(request, requestParameters);
				break;
			}

			}

			FCGX_Finish_r(request);
		}
		// create new Request when accepted because each accept removes old Request data
		request = CreateRequest();

	}

	fprintf(stdout, "FCGI Thread %d shut down\n ", fid);

}

FCGX_Request* CreateRequest() { //Accepting a new FCGI request destroys the state of the previous request, so a new Request needs to be allocated
	//all requests need to be "finished" with FCGI_Finish_r

	FCGX_Request* request = new FCGX_Request();
	FCGX_InitRequest(request, 0, 0);
	return request;

}
//==============================================================================
std::vector<BledID> GetConnectedDongleIDs() {
//==============================================================================
	std::shared_lock<std::shared_timed_mutex> workerLock(workerMapMut,
			std::defer_lock);

	std::vector<BledID> bids;

	for (auto d : workers) {

		bids.push_back(d.first);

	}

	return bids;

}

}

