//==============================================================================
// Project   :  BLE Gateway
// File      :  HTTP_HELPER.cpp
// Author    :  Arthur Habicht
// Compiler  :  GCC C++
// Brief     :  HTTP/HTML/JSON Parsing/Writing
//==============================================================================

#ifndef CH_ETHZ_AH_HTTP_GATEWAY_H_
#define CH_ETHZ_AH_HTTP_GATEWAY_H_

#include <stdio.h>
#include "fcgio.h"
#include <exception>
#include <vector>
#include <set>
#include <map>
#include <unordered_map>
#include <iostream>
#include <tuple>
#include <boost/algorithm/string.hpp>
#include "boost/variant.hpp"
#include "BLE_DRIVER.h"
#include "json/json.h"

namespace BLEGateway {

//forward declaration
class HTMLPrintVisitor;
class JSONPrintVisitor;

// Map Keys for result/request parameter map
static const std::string COMMAND_ID = "COMMAND_ID";
static const std::string HTTP_BODY_STRING = "HTTP_BODY_STRING";
static const std::string HTTP_STATUSCODE = "HTTP_STATUSCODE"; //actual http status codes
static const std::string HTTP_CODE = "HTTP_CODE"; //internal http codes -> enum
static const std::string MAC_STRING = "MAC_STRING";
static const std::string ATTR_HANDLE_STR = "ATTR_HANDLE_STR";
static const std::string ACCEPTED_MIME = "ACCEPTED_MIME";

//Querystring inputs -> transformed to upper case by ParseQueryString
//Discover
static const std::string QUERY_DISCOVER_MODE = "DISCOVERMODE";
static const std::string QUERY_SCAN_INTERVAL = "SCANINTERVAL";
static const std::string QUERY_SCAN_WINDOW = "SCANWINDOW";
static const std::string QUERY_ACTIVE_SCAN = "ACTIVESCAN";
static const std::string QUERY_DISCOVER_TIME = "DISCOVERTIME";

//Connect
static const std::string QUERY_ADDRESS_TYPE = "ADDRESSTYPE";
static const std::string QUERY_CONNECTION_INTERVAL = "CONNECTIONINTERVAL";
static const std::string QUERY_TIMEOUT = "TIMEOUT";
static const std::string QUERY_SLAVE_LATENCY = "SLAVELATENCY";

static const std::string QUERY_LAST_UPDATE = "LASTUPDATE"; //in seconds.

static const std::string QUERY_WRITE_VALUE = "VALUE";

enum CommandID {

	GW_OVERVIEW = 0, //overview of gateway (link to dongles)
	DONGLE_OVERVIEW = 1, //overview of specific Dongle (Version, build, link to discovered/connected)
	BLEDEV_OVERVIEW = 2, // overview of specific BLE Device (Attribute list)
	CONNECTED = 3, // Show Connected devices
	DISCOVERED = 4, //Show Discovered devices / start discovery
	CONNECT = 5, //connect to specific device
	DISCONNECT = 6, //disconnect specific device
	WRITE_HANDLE = 7, //write to an attribute
	READ_HANDLE = 8, // read from an attribute
	ERROR = 9, //errors, use HTTP CODE and HTTP ERROR MESSAGE to specify
	OPTIONS=10
};

//HTTP Header strings
static const std::string HEADER_END = "\r\n";
static const std::string CONNECTION_CLOSE = "Connection: close\r\n";
static const std::string CONNECTION_KA = "Connection: keep-alive\r\n";
//MIME-Types
static const std::string HTML_CONTENT_TYPE = "text/html";
static const std::string JSON_CONTENT_TYPE = "application/json";
static const std::string TEXTWC_CONTENT_TYPE = "text/*";
static const std::string APPWC_CONTENT_TYPE = "application/*";
static const std::string WCWC_CONTENT_TYPE = "*/*";

enum HTTPID {

	HTTP_100_CONTINUE = 0,
	HTTP_101_SWITCHING_P = 1,
	HTTP_102_PROCESSING = 2,
	HTTP_200_OK = 3,
	HTTP_201_CREATED = 4,
	HTTP_202_ACCEPTED = 5,
	HTTP_203_NON_A_I = 6,
	HTTP_204_NO_CONTENT = 7,
	HTTP_205_RESET_CONTENT = 8,
	HTTP_206_PARTIAL_CONTENT = 9,
	HTTP_207_MULTI_STAT = 10,
	HTTP_208_ALREADY_REP = 11,
	HTTP_226_IM_USED = 12,
	HTTP_400_BAD_REQ = 13,
	HTTP_401_UNAUTH = 14,
	HTTP_402_PAY_REQ = 15,
	HTTP_403_FORBIDDEN = 16,
	HTTP_404_NOT_FOUND = 17,
	HTTP_405_M_NOT_A = 18,
	HTTP_406_NOT_ACC = 19,
	HTTP_407_PROXY_A_REQ = 20,
	HTTP_408_REQ_T_O = 21,
	HTTP_409_CONFLICT = 22,
	HTTP_410_GONE = 23,
	HTTP_411_LENGTH_REQ = 24,
	HTTP_423_LOCKED = 25,
	HTTP_429_TOO_MANY_REQ = 26,
	HTTP_500_INT_S_ERROR = 27,
	HTTP_501_NOT_IMPL = 28,
	HTTP_502_BAD_GATEWAY = 29,
	HTTP_503_SERVICE_UNAVAIL = 30,
	HTTP_504_GATEWAY_TO = 31,
	HTTP_505_HTTP_V_NOT_S = 32,
};

static const std::vector<std::string> httpVariableNames = { "DOCUMENT_ROOT",
		"GATEWAY_INTERFACE", "HTTP_ACCEPT", "HTTP_ACCEPT_ENCODING",
		"HTTP_ACCEPT_LANGUAGE", "HTTP_CACHE_CONTROL", "HTTP_CONNECTION",
		"HTTP_HOST", "HTTP_PRAGMA", "HTTP_RANGE", "HTTP_REFERER", "HTTP_TE",
		"HTTP_USER_AGENT", "HTTP_X_FORWARDED_FOR", "HTTP_COOKIE", "PATH",
		"QUERY_STRING", "REMOTE_ADDR", "REMOTE_HOST", "REMOTE_PORT",
		"REQUEST_METHOD", "REQUEST_URI", "SCRIPT_FILENAME", "SCRIPT_NAME",
		"SERVER_ADDR", "SERVER_ADMIN", "SERVER_NAME", "SERVER_PORT",
		"SERVER_PROTOCOL", "SERVER_SIGNATURE", "SERVER_SOFTWARE", "DATE_LOCAL" };

//==============================================================================
/**
 * @brief Process incoming HTTP Requests
 * Decodes requested commands and parameters. Enforces RESTful API
 * The Request URI is assumed to have at least a correct Dongle Nr as first argument (checked by FCGI Thread)
 *
 * For now, the implementation only uses GET/PUT/DELETE Methods
 * GET (Read): Receive data from a resource specified by the URI, can contain Query_String. F.e. GET /DongleId/Discovered?window=..& ...
 * starts a Discovery and returns the discovered MAC Addresses and additional info (f.e. RSSI etc..)
 * starts
 * PUT (Write/Update/Connect): Add a resource or update it. Note that POST is not appropriate here because
 * it is assumed that the Client knows the full URI when he adds a resource (Mac Address)
 * the MAC can be retrieved through discovering BLE Devices. POST is not idempotent and is generally used
 * to address the resource which is responsible for adding another resource (z.b. POST /Company/Employees would
 * mean that the Server is responsible for creating a new Employee ID, This URI would be illegal for PUT.
 * PUT /Company/Employees/Employee1 would either add or Update Employee1). For ease of parsing, this implementation
 * uses QUERY_STRING to supply additional parameters (f.e. ?slaveLatency=..) when connecting
 * PUT is used to Connect/Update a BLE Sensor: PUT /dongleId/MACAddress?slaveLatency=..&timeout=..? ...
 * DELETE (Disconnect): Used to delete a resource. In this case this means Disconnecting a BLE Device by deleting its
 * URI. Note that only BLE Devices can be added/deleted not the Dongles
 *
 * OPTIONS might be added later. It would return possible inputs for the Query String so clients can query possible
 * parameter inputs for each URI
 *
 * The QUERY_STRING parameters don't need to be supplied in a specific order. Some parameters are optional and default parameters are used
 * when they are missing or invalid.
 *
 * HTTP_ACCEPT can be used to notify the Gateway whether it should return a JSON (application/json) or HTML (text/html) format in the HTTP Body
 *
 * The URI is processed case-insensitive
 *
 * @param[in] wr FastCGI Request
 * @param[out] result contains command id and parameters for execution of commands
 */
//==============================================================================
void ProcessHTTPRequest(FCGX_Request* wr,
		std::unordered_map<std::string, std::string>& result);

//==============================================================================
/**
 * @brief Parses URI
 * Case-insentitive, Determines which command is requested
 * @param[in] requestURI Uri to be parsed
 * @param[in] requestMethod
 * @param[out] result contains command id and parameters for execution of commands
 */
//==============================================================================
void ParseURI(std::string requestURI, std::string requestMethod,
		std::unordered_map<std::string, std::string>& result);

//==============================================================================
/**
 * @brief Parses Query String
 * Case-insentitive, parameters can be in any order. Determines additional parameters
 * for specific URIs.
 * @param[in] queryString Query String to be parsed
 * @param[out] result contains command id and parameters for execution of commands
 */
//==============================================================================
void ParseQueryString(std::string queryString,
		std::unordered_map<std::string, std::string>& result);

//==============================================================================
/**
 * @brief Parses Accept Header
 *
 * Parses Accept Header according RFC 7231 5.3.2 for content negotiation
 *
 * Recognized media types: JSON (application/ * , application/json)
 * HTML (text/ *, text/html) default
 *
 * also parses Quality parameter q.  0.0<=q<=1.0
 *
 * @param[in] acceptHeader
 * @param[out] result contains command id and parameters for execution of commands
 */
//==============================================================================
void ParseAcceptHeader(std::string acceptHeader,
		std::unordered_map<std::string, std::string>&result);

//==============================================================================
/**
 * @brief Parses Quality value in Accept Header
 *
 * if no q parameter, then q = 1.0
 *
 * @param[in] str
 * @result quality parameter as float
 */
//==============================================================================
float GetQualityParam(std::string str);
//HTTP Response Functions

//==============================================================================
/**
 * @brief Prints Struct Data as JSON or HTML and sends it to the client
 *
 * Creates HTTP Header and Body and uses FCGI Socket to send data to client.
 *
 * @param[in] wr FCGI Request
 * @param[in] data pointers to struct data to be printed
 * @param[in] parameters contains data for compiling header and body
 */
//==============================================================================
void PostHTTPStructData(FCGX_Request* wr, std::vector<GatewayBaseStruct*> data,
		std::unordered_map<std::string, std::string>& parameters);

//==============================================================================
/**
 * @brief Prints Vector Data (pair of key and value strings) as JSON or HTML and sends it to the client
 *
 * Creates HTTP Header and Body and uses FCGI Socket to send data to client.
 *
 * @param[in] wr FCGI Request
 * @param[in] data pointers to vector data to be printed
 * @param[in] parameters contains data for compiling header and body
 */
//==============================================================================
void PostHTTPVectorData(FCGX_Request* wr,
		std::vector<std::pair<std::string, std::string>> data,
		std::unordered_map<std::string, std::string>& parameters);

//==============================================================================
/**
 * @brief Posts HTTP Message to client
 *
 * Creates HTTP Header and Body. Mainly used for sending various HTTP Status codes to
 * the client. F.e. in case of errors.
 *
 * @param[in] wr FCGI Request
 * @param[in] parameters contains data for compiling header and body
 */
//==============================================================================
void PostHTTPMessage(FCGX_Request* wr,
		std::unordered_map<std::string, std::string>& parameters);

//==============================================================================
/**
 * @brief Creates HTTP Body for error messages
 *
 * @param[in] body message in body
 * @param[in] type JSON or HTML
 * @param[in] parameters contains data for compiling header and body
 */
//==============================================================================
void CreateHTTPErrorBody(std::stringstream& body, std::string type,
		std::unordered_map<std::string, std::string>& parameters);

//==============================================================================
/**
 * @brief Get Current GMT Time for Header according to RFC 7231 7.1.1.1.
 * @result Time as string
 */
//==============================================================================
std::string GetGMTDateTime();

//==============================================================================
/**
 * @brief Calculates Content-Length Header input accordingt to RFC 7230 3.3.2.
 *
 * @param[in] str message in body
 * @param[in] parameters contains data for compiling header and body
 * @result Content-Length size in number of octets
 */
//==============================================================================
std::string GetContentLengthStr(std::string str);

//==============================================================================
/**
 * @brief Returns HTTP Date Header according to RFC 7231 7.1.1.2.
 *
 * @result Date as string
 */
//==============================================================================
std::string GetDateStr();

//==============================================================================
/**
 * @brief Returns Content-Type header according to RFC 7231 3.1.1.5.
 * @param[in] type JSON or HTML
 * @result Content-Type as string
 */
//==============================================================================
std::string GetContentTypeStr(std::string type);

//==============================================================================
/**
 * @brief Returns Allow Header field according to RFC 7231 7.4.1
 * When returning a 405 Method Not Allowed, the Header MUST contain "Allow"
 * @param[in] allowedMethods GET/DELETE/PUT depending on URI
 * @result Allow Header as string
 */
//==============================================================================
std::string GetAllowStr(std::string allowedMethods);

//==============================================================================
/**
 * @brief Returns Status string for FCGI
 * FCGI requires a "Status:..." string to change HTTP Status Header in HTTP Response
 * @param[in] status  requested http status
 * @result Status string
 */
//==============================================================================
std::string GetStatusStr(std::string status);

//Print functions----------------------------------------------------------------------
//==============================================================================
/**
 * @brief Print simple HTML from vector
 * @param[in] printableLines  key-value pairs. Vector instead of Map to avoid reordering of inputs
 * @result HTML string
 */
//==============================================================================
std::string PrintHTML(
		std::vector<std::pair<std::string, std::string> > printableLines);

//==============================================================================
/**
 * @brief Print simple HTML from vector of struct pointers
 * Uses HTML Visitor to visit the data of structs and to create a HTML representation
 * @param[in] printableStructs  pointer to structs
 * @result HTML string
 */
//==============================================================================
std::string PrintHTML(std::vector<GatewayBaseStruct*> printableStructs);

//==============================================================================
/**
 * @brief Print simple JSON from vector
 * FCGI requires a "Status:..." string to change HTTP Status Header in HTTP Response
 * @param[in] printableLines  key-value pairs. Vector instead of Map to avoid reordering of inputs
 * @result JSON string
 */
//==============================================================================
std::string PrintJSON(
		std::vector<std::pair<std::string, std::string> > printableLines);

//==============================================================================
/**
 * @brief Print simple JSON from vector of struct pointers
 * Uses JSON Visitor to visit the data of structs and to create a JSON representation
 * @param[in] printableStructs  pointer to structs
 * @result JSON string
 */
//==============================================================================
std::string PrintJSON(std::vector<GatewayBaseStruct*> printableStructs);

// Visitors, based on boost::variant
class HTMLPrintVisitor: public boost::static_visitor<std::string> {

public:

	//if input is a string, use it
	std::string operator()(std::string s) const {

		return s;
	}

	//if value of variant is a struct, use PrintStructHTML recursively
	std::string operator()(GatewayBaseStruct* gp) const {

		if (gp != nullptr) {
			return PrintStructHTML(gp, this);
		} else {
			return "EMPTY";
		}
	}

	std::string PrintStructHTML(GatewayBaseStruct* printableStruct,
			const HTMLPrintVisitor* visitor) const

			{

		std::stringstream ss;
		ss << "<b>" << printableStruct->GetId() << "</b><br>";
		ss << "<table border=\"1\">\r\n";

		for (auto &v : printableStruct->ToString()) {

			ss << "<tr><td>" << v.first << "</td>";
			ss << "<td>" << v.second.apply_visitor(*visitor) << "</td>"
					<< "</tr>\r\n";
		}

		ss << "</table><br>\r\n";

		return ss.str();

	}

};

class JSONPrintVisitor: public boost::static_visitor<Json::Value> {

public:

	Json::Value operator()(std::string s) const {
		return s;
	}

	Json::Value operator()(GatewayBaseStruct* gp) const {

		if (gp != nullptr) {
			return PrintStructJSON(gp, this);
		} else {
			return "EMPTY";
		}
	}

	Json::Value PrintStructJSON(GatewayBaseStruct* printableStruct,
			const JSONPrintVisitor* visitor) const {

		Json::Value root;
		Json::Value values;

		for (auto v : printableStruct->ToString()) {

			values[v.first] = v.second.apply_visitor(*visitor);

		}

		root[printableStruct->GetId()] = values;

		return root;

	}

};

}

#endif /* CH_ETHZ_AH_HTTP_GATEWAY_H_ */
