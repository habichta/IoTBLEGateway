#ifndef CH_ETHZ_AH_BLEG_GATEWAY_H_
#define CH_ETHZ_AH_BLEG_GATEWAY_H_

#include "BLE_DRIVER.h"
#include "HTTP_HELPER.h"
#include "fcgio.h"
#include <boost/algorithm/string.hpp>
#include <csignal>

void signalHandler(int signum);

namespace BLEGateway {

enum Settings {

	SERVER_PORT = 8080, //should not collide with fastcgi lighttpd configuration
	MAX_PENDING_CONNECTIONS = 10,
	FCGI_THREADS = 1,
	WORKER_QUEUE_SZ = 128,
	FCGI_QUEUE_SZ = 128

};

typedef uint BledID;
typedef uint FcgiID;

typedef boost::lockfree::queue<FCGX_Request*>* LockfreeWorkQueue_p;

//Defines the element in workers buffer
//==============================================================================
/**
 * @brief Defines element in the worker queue
 * Contains pointer to a lockfree work queue which is used for communication between
 * FCGI and Worker Threads
 * It also contains a pointer to an BLED112 object in case this object should be accessible
 * from outside (in future implementations)
 */
//==============================================================================
struct WorkerThread {

	Device* bledObj = nullptr; //BLED object contains all the data of the Dongle, not used here but could be useful for later implementations
	LockfreeWorkQueue_p workQueue = nullptr; //Pointer to workqueue of that Thread (to push requests)

	WorkerThread(Device* b, LockfreeWorkQueue_p wq) {
		bledObj = b;
		workQueue = wq;
	}

	WorkerThread() = default; //needed because map operator[] creates a temporary struct with default constructor and copies values of assigned struct into this new struct

};

typedef std::map<BledID, WorkerThread> WorkerThreads;

//==============================================================================
/**
 * @brief Main Server Thread
 * Accepts new TCP connections when new Dongles are connected, spawns Worker Threads
 * and assigns their BLED IDs.
 *
 * Joinable
 */
//==============================================================================
void ServerThread();

//==============================================================================
/**
 * @brief Worker Thread
 * Each Dongle has one Worker Thread. Runs as long as globalTermination = false
 * and isConnected = true. The latter is set to false in case the TCP socket
 * receives an EOF, which means that the Dongle client program closed the TCP
 * connection.
 *
 * The Worker Thread is responsible for processing HTTP Requests on its work queue (
 * and sending commands to the Dongles)
 * as well as handling events and responses from the BlueGiga BLED112s. It maintains
 * the virtual representation of all BLE devices connected to and discovered by this
 * Dongle.
 *
 *@param[in] clientSocketDesc The TCP socket
 *@param[in] bid BLED ID of this thread given by Server Thread
 *@param[in] adressInfo TCP/IP Address info
 *
 * Detached
 */
//==============================================================================
void BLEWorker(int clientSocketDesc, BledID bid, sockaddr_in adressInfo);

//==============================================================================
/**
 * @brief Execute HTTP requests
 *
 * The Worker Thread fetches FCGX_Requests from the work queue and processes them
 * Finishes the request.
 *
 *@param[in] devObj pointer to BLED112 Object Interface
 *@param[in] wr FCGI Request (HTTP Request)
 *
 */
//==============================================================================
void ExecuteWorkRequest(Device* devObj, FCGX_Request* wr);

//==============================================================================
/**
 * @brief FastCGI Thread
 *
 *Communicated with the Webserver and retrieves HTTP requests.
 *Parses the URI and puts the request on the correct work queue or
 *creates a HTTP response to indicate an error
 *
 *@param[in] fid FCGI ID of thread
 *
 *Detached
 */
//==============================================================================
void FCGIWorker(FcgiID fid);

//==============================================================================
/**
 * @brief Creates new request object
 *
 *FCGX_Accept_r deletes data of previously accepted request. To prevent that, a new request object
 *needs to be created.
 *
 */
//==============================================================================
FCGX_Request* CreateRequest();

//Multi-Producer / Multi-Consumer lockfree Queue (threadsafe)

//==============================================================================
/**
 * @brief Add a new worker to global workers map
 *
 *@param[in] bid BLED ID
 *@param[in] wt WorkerThread struct
 *
 *Thread-safe
 */
//==============================================================================
void AddBLEWorker(BledID bid, WorkerThread wt);

//==============================================================================
/**
 * @brief Remove a new worker from global workers map
 *
 *@param[in] bid BLED ID
 *
 *Thread-safe
 */
//==============================================================================
void RemoveBLEWorker(BledID bid);

//==============================================================================
/**
 * @brief Push new FCGX_Request on worker queue
 *
 *@param[in] bid BLED ID
 *@param[in] wr Request
 *
 *Thread-safe
 */
//==============================================================================
bool PushToWorkerQueue(BledID bid, FCGX_Request* wr);

//==============================================================================
/**
 * @brief Pop FCGX_Request from worker queue
 *
 *@param[in] bid BLED ID
 *@param[in] wr Request
 *
 *Thread-safe
 */
//==============================================================================
bool PopFromWorkerQueue(BledID bid, FCGX_Request** wr);


//==============================================================================
/**
 * @brief Get all connected dongle ids
 *
 *@result vector with Ids
 *
 *Thread-safe
 */
//==============================================================================
std::vector<BledID> GetConnectedDongleIDs();

}

#endif //CH_ETHZ_AH_BLEG_GATEWAY_H_
