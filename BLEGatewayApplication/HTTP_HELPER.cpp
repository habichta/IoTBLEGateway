#include "Headers/HTTP_HELPER.h"

namespace BLEGateway {

//==============================================================================
void ProcessHTTPRequest(FCGX_Request* wr,
		std::unordered_map<std::string, std::string>& result) {
//==============================================================================

	std::unordered_map<std::string, std::string> httpValueMap;
	for (auto httpVar : httpVariableNames) {

		const char* value = FCGX_GetParam(httpVar.c_str(), wr->envp);

		if (value != NULL) {
			httpValueMap[httpVar] = std::string(value);
		}
	}

	std::string requestURI = httpValueMap["REQUEST_URI"];
	std::string queryString = httpValueMap["QUERY_STRING"];
	std::string requestMethod = httpValueMap["REQUEST_METHOD"];
	std::string httpAccept = httpValueMap["HTTP_ACCEPT"];

	if (!requestURI.empty()) {
		std::string del;

		//Remove QUERY_STRING from REQUEST_URI
		if (!queryString.empty()) {
			std::vector<std::string> uri;
			ParseQueryString(queryString, result);

			del = std::string("?");
			ConstructSubstrings(requestURI, del, uri);

			ParseURI(uri.at(0), requestMethod, result);

		} else {
			ParseURI(requestURI, requestMethod, result);
		}

		ParseAcceptHeader(httpAccept, result);

	} else {

		//If Request URI is empty, Should not occur
		result[COMMAND_ID] = std::to_string(ERROR);
		result[HTTP_CODE] = std::to_string(HTTP_400_BAD_REQ);
		result[HTTP_BODY_STRING] = requestURI + ": Could not be parsed";

	}

}
//==============================================================================
void ParseURI(std::string requestURI, std::string requestMethod,
		std::unordered_map<std::string, std::string>& result) {
//==============================================================================

//Strings used for parsing the request
	std::string httpGet("GET");
	std::string httpPut("PUT");
	std::string httpDelete("DELETE");
	std::string httpOptions("OPTIONS");
	std::string discoveredDevices("Discovered");
	std::string connectedDevices("Connected");

	std::vector<std::string> uriParameters;
	std::string del = std::string("/");
	ConstructSubstrings(requestURI, del, uriParameters); // this contains ad least the Dongle/Id, FCGI Thread already filtered invalid ids or uris with no id

	//The first element of uriParameters is the dongle id. The second is either
	// Discovered, Connected, MAC or empty. each of these cases

	switch (uriParameters.size()) {

	case 0: {

		if (boost::iequals(requestMethod, httpGet)) {
			result[COMMAND_ID] = std::to_string(GW_OVERVIEW);
		} else if (boost::iequals(requestMethod, httpOptions)) {
			result[COMMAND_ID] = std::to_string(OPTIONS);
			result[HTTP_CODE] = std::to_string(HTTP_200_OK);
			result[HTTP_BODY_STRING] = "METHODS: GET, OPTIONS";
		} else {
			result[COMMAND_ID] = std::to_string(ERROR);
			result[HTTP_CODE] = std::to_string(HTTP_405_M_NOT_A); //RFC2616 section 14.7 MUST have an Allow header
			result[HTTP_BODY_STRING] = "GET, OPTIONS"; //used for allow header
		}
		break;
	}

	case 1: { //TODO: change this if RESTful Interface should also support access to Devices without mentioning the dongle nr

		//only has dongle info, only GET is supported, return Dongle info -> link to connected/discovered HTTP 200
		if (boost::iequals(requestMethod, httpGet)) {
			result[COMMAND_ID] = std::to_string(DONGLE_OVERVIEW);
		} else if (boost::iequals(requestMethod, httpOptions)) {
			result[COMMAND_ID] = std::to_string(OPTIONS);
			result[HTTP_CODE] = std::to_string(HTTP_200_OK);
			result[HTTP_BODY_STRING] = "METHODS: GET, OPTIONS";
		} else {
			result[COMMAND_ID] = std::to_string(ERROR);
			result[HTTP_CODE] = std::to_string(HTTP_405_M_NOT_A); //RFC2616 section 14.7 MUST have an Allow header
			result[HTTP_BODY_STRING] = "GET, OPTIONS"; //used for allow header
		}
		break;
	}
	case 2: {

		std::string uriCommand = uriParameters.at(1);

		if (boost::iequals(uriCommand, discoveredDevices)) {
			//only GET
			if (boost::iequals(requestMethod, httpGet)) {
				result[COMMAND_ID] = std::to_string(DISCOVERED);
			} else if (boost::iequals(requestMethod, httpOptions)) {
				result[COMMAND_ID] = std::to_string(OPTIONS);
				result[HTTP_CODE] = std::to_string(HTTP_200_OK);
				result[HTTP_BODY_STRING] =
						"METHODS: GET, OPTIONS; QUERY GET: ||| discovertime (in seconds) ||| discovermode(0:limited,1:generic,2:observation) ||| scaninterval(in units of 625micros,0x4<=val<=0x4000) ||| scanwindow(in units of 625micros, 0x4<=val<=scaninterval<=0x4000) ||| activescan(0,1)";
			} else {
				result[COMMAND_ID] = std::to_string(ERROR);
				result[HTTP_CODE] = std::to_string(HTTP_405_M_NOT_A); //RFC2616 section 14.7 MUST have an Allow header
				result[HTTP_BODY_STRING] = "GET, OPTIONS"; //used for allow header
			}
		} else if (boost::iequals(uriCommand, connectedDevices)) {

			//only GET
			if (boost::iequals(requestMethod, httpGet)) {

				result[COMMAND_ID] = std::to_string(CONNECTED);
			} else if (boost::iequals(requestMethod, httpOptions)) {
				result[COMMAND_ID] = std::to_string(OPTIONS);
				result[HTTP_CODE] = std::to_string(HTTP_200_OK);
				result[HTTP_BODY_STRING] = "METHODS: GET, OPTIONS";
			} else {
				result[COMMAND_ID] = std::to_string(ERROR);
				result[HTTP_CODE] = std::to_string(HTTP_405_M_NOT_A); //RFC2616 section 14.7 MUST have an Allow header
				result[HTTP_BODY_STRING] = "GET, OPTIONS"; //used for allow header

			}

		} else if (IsValidMAC(uriCommand)) {

			//no atthandle given (only 2 parameters)
			//PUT,GET,DELETE

			if (boost::iequals(requestMethod, httpGet)) {

				//valid mac + Get attributes of ble device
				result[COMMAND_ID] = std::to_string(BLEDEV_OVERVIEW);
				result[MAC_STRING] = uriCommand;

			} else if (boost::iequals(requestMethod, httpPut)) {
				//valid mac + Connect BlE Device
				result[COMMAND_ID] = std::to_string(CONNECT);
				result[MAC_STRING] = uriCommand;

			} else if (boost::iequals(requestMethod, httpDelete)) {
				//valid mac + Disconnect BLE Device
				result[COMMAND_ID] = std::to_string(DISCONNECT);
				result[MAC_STRING] = uriCommand;

			} else if (boost::iequals(requestMethod, httpOptions)) {
				result[COMMAND_ID] = std::to_string(OPTIONS);
				result[HTTP_CODE] = std::to_string(HTTP_200_OK);
				result[HTTP_BODY_STRING] =
						"METHODS: GET, PUT, DELETE, OPTIONS; QUERY PUT: ||| connectioninterval(in units of 1.25ms, 6<=val<=3200) ||| slavelatency(0<=val<=500) ||| timeout(in units of 10ms, 10<=val<=3200)";
			} else {

				result[COMMAND_ID] = std::to_string(ERROR);
				result[HTTP_CODE] = std::to_string(HTTP_405_M_NOT_A); //RFC2616 section 14.7 MUST have an Allow header
				result[HTTP_BODY_STRING] = "GET, PUT, DELETE, OPTIONS"; //used for allow header
			}

		} else {
			result[COMMAND_ID] = std::to_string(ERROR);
			result[HTTP_CODE] = std::to_string(HTTP_400_BAD_REQ);
			result[HTTP_BODY_STRING] = requestURI
					+ ": Invalid Syntax. Could not recognize "
					+ discoveredDevices + ", " + connectedDevices
					+ "or MAC Address.";
		}

		break;
	}

	case 3: {
		//3 parameters, can only be dongle/mac/attributehandle
		std::string uriCommand = uriParameters.at(1);
		std::string attributeId = uriParameters.at(2);

		if (IsValidMAC(uriCommand) && IsValidInteger(attributeId)) {

			//PUT,GET

			if (boost::iequals(requestMethod, httpGet)) {

				//valid mac + attribute handle, get attribute data
				result[COMMAND_ID] = std::to_string(READ_HANDLE);
				result[MAC_STRING] = uriCommand;
				result[ATTR_HANDLE_STR] = attributeId;

			} else if (boost::iequals(requestMethod, httpPut)) {
				//valid mac + attribute handle, write to handle
				result[COMMAND_ID] = std::to_string(WRITE_HANDLE);
				result[MAC_STRING] = uriCommand;
				result[ATTR_HANDLE_STR] = attributeId;

			} else if (boost::iequals(requestMethod, httpOptions)) {
				result[COMMAND_ID] = std::to_string(OPTIONS);
				result[HTTP_CODE] = std::to_string(HTTP_200_OK);
				result[HTTP_BODY_STRING] =
						"METHODS: GET, PUT, OPTIONS; QUERY GET: ||| lastupdate(in seconds); QUERY PUT:||| value(hex string)";
			} else {

				result[COMMAND_ID] = std::to_string(ERROR);
				result[HTTP_CODE] = std::to_string(HTTP_405_M_NOT_A); //RFC2616 section 14.7 MUST have an Allow header
				result[HTTP_BODY_STRING] = "GET, PUT, OPTIONS"; //used for allow header
			}

		} else {

			result[COMMAND_ID] = std::to_string(ERROR);
			result[HTTP_CODE] = std::to_string(HTTP_400_BAD_REQ);
			result[HTTP_BODY_STRING] =
					requestURI
							+ ": Invalid Syntax. Could not recognize MAC Address or Attribute Handle";

		}

		break;
	}

	default: {

		result[COMMAND_ID] = std::to_string(ERROR);
		result[HTTP_CODE] = std::to_string(HTTP_400_BAD_REQ);
		result[HTTP_BODY_STRING] = requestURI + ": Invalid Syntax";

		break;
	}

	}

}

//Parses Query_STRING of format first=value&second=value ..., discards illegal formats
//does not check whether the QUERY_STRING has correct parameters for a given command. This is checked at a later stage
//allows any order of parameters
//implementation is case insenitive
//duplicate keys are overwritten
//==============================================================================
void ParseQueryString(std::string queryString,
		std::unordered_map<std::string, std::string>& result) {
//==============================================================================

	std::string del;
	del = std::string("&");
	std::vector<std::string> parameters;

	ConstructSubstrings(queryString, del, parameters);

	del = std::string("=");
	std::vector<std::string> values;
	for (auto p : parameters) {

		ConstructSubstrings(p, del, values);

		if (values.size() == 2) {

			std::string val = values.at(0);
			boost::to_upper(val); //all upper case so input can be case insensitive

			result[val] = values.at(1);

		}
		//if the QUERY_STRING does not have the right format, discard it
		values.clear();

	}

}

//returns preferred type, parses accept header and decides which type. Also uses q parameter

// if acceptHeader is empty, assume clients accepts all types (default text/html)
// if acceptHeader is not empty but does not contain supported type send 406 Not Acceptable
// choose the most specific type */* is the least specific
// choose according to q= parameter
// choose text/html if both text/html and application/json are acceptable with same q

/*
 *  <field>  =    Accept: <entry> *[ , <entry> ]
 <entry>  =    <content type> *[ ; <param> ]
 <param>  =    <attr> = <float>
 <attr>   =    q / mxs / mxb
 <float>  =    <ANSI-C floating point text represntation>
 *
 */
//==============================================================================
void ParseAcceptHeader(std::string acceptHeader,
		std::unordered_map<std::string, std::string>& result) {
//==============================================================================

	if (!acceptHeader.empty()) {

		namespace ba = boost::algorithm;

		std::vector<std::string> mediaTypes;
		std::vector<std::tuple<std::string, float, int> > typeToPreference; //type,q,specific(0,1,2)
		std::string del = ",";
		int mostSpecific = 0;
		ConstructSubstrings(acceptHeader, del, mediaTypes);

		for (auto s : mediaTypes) {

			if (ba::contains(s, HTML_CONTENT_TYPE)) {

				mostSpecific = 2;
				std::tuple<std::string, float, int> t = std::tuple<std::string,
						float, int>(HTML_CONTENT_TYPE, GetQualityParam(s), 2);
				typeToPreference.push_back(t);

			} else if (ba::contains(s, JSON_CONTENT_TYPE)) {
				mostSpecific = 2;
				std::tuple<std::string, float, int> t = std::tuple<std::string,
						float, int>(JSON_CONTENT_TYPE, GetQualityParam(s), 2);
				typeToPreference.push_back(t);

			} else if (ba::contains(s, APPWC_CONTENT_TYPE)) {

				if (mostSpecific <= 1) {
					mostSpecific = 1;
					std::tuple<std::string, float, int> t = std::tuple<
							std::string, float, int>(APPWC_CONTENT_TYPE,
							GetQualityParam(s), 1);
					typeToPreference.push_back(t);
				}

			} else if (ba::contains(s, TEXTWC_CONTENT_TYPE)) {

				if (mostSpecific <= 1) {
					mostSpecific = 1;
					std::tuple<std::string, float, int> t = std::tuple<
							std::string, float, int>(TEXTWC_CONTENT_TYPE,
							GetQualityParam(s), 1);
					typeToPreference.push_back(t);
				}

			} else if (ba::contains(s, WCWC_CONTENT_TYPE)) {

				if (mostSpecific == 0) {
					std::tuple<std::string, float, int> t = std::tuple<
							std::string, float, int>(TEXTWC_CONTENT_TYPE,
							GetQualityParam(s), 0);
					typeToPreference.push_back(t);
				}

			}

		}

		if (typeToPreference.size() == 0) {
			//accept header string is not empty but no valid type could be accepted
			//server SHOULD send a 406 according to RFC2616
			result[COMMAND_ID] = std::to_string(ERROR);
			result[HTTP_CODE] = std::to_string(HTTP_406_NOT_ACC);
			result[HTTP_BODY_STRING] = "Invalid/Not supported Accept Header";
		} else {

			//go through all accepted headers, choose the most specific one with the highest q value

			float maxq = 0.0;
			std::string chosen = "";

			for (auto t : typeToPreference) {

				if (std::get<2>(t) == mostSpecific && std::get<1>(t) >= maxq) { //only consider the most specific choice which has higher or equal q value. It needs to be >= because if application/json was accepted, a text/html wins if it hase the same q

					if (!((boost::iequals(chosen, HTML_CONTENT_TYPE)
							|| boost::iequals(chosen, TEXTWC_CONTENT_TYPE))
							&& std::get<1>(t) == maxq)) {
						//if text/html has already been chosen with the same q value, don't change it. text/html wins over application/json
						//if the client assignes them the same q value. Also: text/* wins over application/*

						maxq = std::get<1>(t);
						chosen = std::get<0>(t);

					}

				}

			}

			//All values have been evaluated, chosen contains the accepted type

			result[ACCEPTED_MIME] = chosen;

		}

	} else {
		//empty accept header: send html
		result[ACCEPTED_MIME] = HTML_CONTENT_TYPE;

	}

}
//==============================================================================
float GetQualityParam(std::string str) {
//==============================================================================
	namespace ba = boost::algorithm;
	std::vector<std::string> parameters;
	std::string del = ",";
	std::string delp = "=";
	std::string param = "q";

	ConstructSubstrings(str, del, parameters);

	if (parameters.size() > 0) { //if no parameters

		for (auto s : parameters) {

			if (ba::contains(s, param)) {

				std::vector<std::string> pvalues;

				ConstructSubstrings(s, delp, pvalues);

				if (pvalues.size() == 2) {

					std::string value = pvalues.at(1);

					try {
						return std::stof(value, nullptr);
					} catch (std::exception& e) {
						return 1.0;
					}

				} else {
					return 1.0;
				}

			}

		}

	}

	return 1.0;

}

//==============================================================================
void PostHTTPStructData(FCGX_Request* wr, std::vector<GatewayBaseStruct*> data,
		std::unordered_map<std::string, std::string>& parameters) {
	//==============================================================================

	std::string type = parameters[ACCEPTED_MIME];
	std::stringstream body;
	std::stringstream header;

	if (type.empty()) {
		type = HTML_CONTENT_TYPE;
	}

	std::string bodyString = parameters[HTTP_BODY_STRING];
	std::vector<std::pair<std::string, std::string> > status = { std::pair<
			std::string, std::string>("STATUS", bodyString) };

	if (boost::iequals(type, JSON_CONTENT_TYPE)
			|| boost::iequals(type, APPWC_CONTENT_TYPE)) {
		//JSON

		body << PrintJSON(status);
		body << PrintJSON(data);

	} else {
		//HTML body

		body << PrintHTML(status);

		body << PrintHTML(data);
	}

	header << GetStatusStr(parameters[HTTP_STATUSCODE]) << GetDateStr()
			<< CONNECTION_CLOSE << GetContentTypeStr(type)
			<< GetContentLengthStr(body.str()) << HEADER_END;

	std::string httpMessage = header.str() + body.str();

	FCGX_PutS(httpMessage.c_str(), wr->out); //write to outputstream. Sending HTTP Message to Client

}

//==============================================================================
void PostHTTPVectorData(FCGX_Request* wr,
		std::vector<std::pair<std::string, std::string>> data,
		std::unordered_map<std::string, std::string>& parameters) {
//==============================================================================
	std::string type = parameters[ACCEPTED_MIME];
	std::stringstream body;
	std::stringstream header;

	if (type.empty()) {
		type = HTML_CONTENT_TYPE;
	}

	std::string bodyString = parameters[HTTP_BODY_STRING];
	std::vector<std::pair<std::string, std::string> > status = { std::pair<
			std::string, std::string>("STATUS", bodyString) };

	if (boost::iequals(type, JSON_CONTENT_TYPE)
			|| boost::iequals(type, APPWC_CONTENT_TYPE)) {
		//JSON

		body << PrintJSON(status);
		body << PrintJSON(data);

	} else {
		//HTML body

		body << PrintHTML(status);

		body << PrintHTML(data);
	}

	header << GetStatusStr(parameters[HTTP_STATUSCODE]) << GetDateStr()
			<< CONNECTION_CLOSE << GetContentTypeStr(type)
			<< GetContentLengthStr(body.str()) << HEADER_END;

	std::string httpMessage = header.str() + body.str();

	FCGX_PutS(httpMessage.c_str(), wr->out); //write to outputstream. Sending HTTP Message to Client

}

//Todo: Add more cases to switch if needed
//==============================================================================
void PostHTTPMessage(FCGX_Request* wr,
		std::unordered_map<std::string, std::string>& parameters) {
//==============================================================================

	int httpStatusCode = 0;

	std::string httpStatusCodeStr = parameters[HTTP_CODE];

	if (httpStatusCodeStr.empty()) {
		httpStatusCode = HTTP_500_INT_S_ERROR;

	} else {
		httpStatusCode = std::stoi(httpStatusCodeStr, nullptr, 10);
	}

	std::stringstream header;
	std::stringstream body;

	std::string type = parameters[ACCEPTED_MIME];

	if (type.empty()) {
		type = HTML_CONTENT_TYPE;
	}

	switch (httpStatusCode) {
	case HTTP_200_OK: {
		parameters[HTTP_STATUSCODE] = "200";
		CreateHTTPErrorBody(body, type, parameters);
		header << "Status: 200 OK\r\n" << GetDateStr() << CONNECTION_CLOSE
				<< GetContentTypeStr(type) //todo: connection_close ok?
				<< GetContentLengthStr(body.str()) << HEADER_END;

		break;
	}
	case HTTP_202_ACCEPTED: {
		parameters[HTTP_STATUSCODE] = "202";
		CreateHTTPErrorBody(body, type, parameters);
		header << "Status: 202 Accepted\r\n" << GetDateStr() << CONNECTION_CLOSE
				<< GetContentTypeStr(type) //todo: connection_close ok?
				<< GetContentLengthStr(body.str()) << HEADER_END;

		break;
	}

	case HTTP_400_BAD_REQ: {
		//illegal syntax of request
		parameters[HTTP_STATUSCODE] = "400";
		CreateHTTPErrorBody(body, type, parameters);
		header << "Status: 400 Bad Request\r\n" << GetDateStr()
				<< CONNECTION_CLOSE << GetContentTypeStr(type)
				<< GetContentLengthStr(body.str()) << HEADER_END;

		break;

	}
	case HTTP_404_NOT_FOUND: {
		//resource does not exist
		parameters[HTTP_STATUSCODE] = "404";
		CreateHTTPErrorBody(body, type, parameters);
		header << "Status: 404 Not Found\r\n" << GetDateStr()
				<< CONNECTION_CLOSE << GetContentTypeStr(type)
				<< GetContentLengthStr(body.str()) << HEADER_END;

		break;
	}
	case HTTP_405_M_NOT_A: {
		//Method not allowed for this resource, MUST have Allow header

		std::string allowMethods = parameters[HTTP_BODY_STRING];
		parameters[HTTP_STATUSCODE] = "405";

		CreateHTTPErrorBody(body, type, parameters);
		header << "Status: 405 Method Not Allowed\r\n" << GetDateStr()
				<< GetAllowStr(allowMethods) << CONNECTION_CLOSE
				<< GetContentTypeStr(type) << GetContentLengthStr(body.str())
				<< HEADER_END;

		break;
	}
	case HTTP_406_NOT_ACC: {
		//requested type not supported
		parameters[HTTP_STATUSCODE] = "406";
		CreateHTTPErrorBody(body, type, parameters);
		header << "Status: 406 Not Acceptable\r\n" << GetDateStr()
				<< CONNECTION_CLOSE << GetContentTypeStr(type)
				<< GetContentLengthStr(body.str()) << HEADER_END;

		break;

	}
	case HTTP_500_INT_S_ERROR: {
		parameters[HTTP_STATUSCODE] = "500";
		CreateHTTPErrorBody(body, type, parameters);
		header << "Status: 500 Internal Server Error\r\n" << GetDateStr()
				<< CONNECTION_CLOSE << GetContentTypeStr(type)
				<< GetContentLengthStr(body.str()) << HEADER_END;

		break;
	}
	case HTTP_501_NOT_IMPL: {
		parameters[HTTP_STATUSCODE] = "501";
		CreateHTTPErrorBody(body, type, parameters);
		header << "Status: 501 Not Implemented\r\n" << GetDateStr()
				<< CONNECTION_CLOSE << GetContentTypeStr(type)
				<< GetContentLengthStr(body.str()) << HEADER_END;

		break;
	}
	case HTTP_502_BAD_GATEWAY: {
			parameters[HTTP_STATUSCODE] = "502";
			CreateHTTPErrorBody(body, type, parameters);
			header << "Status: 502 Bad Gateway\r\n" << GetDateStr()
					<< CONNECTION_CLOSE << GetContentTypeStr(type)
					<< GetContentLengthStr(body.str()) << HEADER_END;

			break;
		}
	default: {

		//HTTP 500 as default
		parameters[HTTP_STATUSCODE] = "500";
		CreateHTTPErrorBody(body, type, parameters);
		header << "Status: 500 Internal Server Error\r\n" << GetDateStr()
				<< CONNECTION_CLOSE << GetContentTypeStr(type)
				<< GetContentLengthStr(body.str()) << HEADER_END;

		break;

	}

	}

	std::string httpMessage = header.str() + body.str();

	FCGX_PutS(httpMessage.c_str(), wr->out); //write to outputstream. Sending HTTP Message to Client

}
//==============================================================================
void CreateHTTPErrorBody(std::stringstream& body, std::string type,
		std::unordered_map<std::string, std::string>& parameters) {
//==============================================================================
	//HTTP Body

	std::string errorMessage = parameters[HTTP_BODY_STRING];
	std::string httpStatusCode = parameters[HTTP_STATUSCODE];

	if (!errorMessage.empty()) {
		if (boost::iequals(type, JSON_CONTENT_TYPE)
				|| boost::iequals(type, APPWC_CONTENT_TYPE)) {

			std::pair<std::string, std::string> p(httpStatusCode, errorMessage);

			std::vector<std::pair<std::string, std::string> > lines;
			lines.push_back(p);
			body << PrintJSON(lines);

		} else {
			//HTML body
			std::string m1 = httpStatusCode + ": ";
			std::string m2 = errorMessage;

			std::vector<std::pair<std::string, std::string> > lines;
			lines.push_back(std::pair<std::string, std::string>(m1, m2));

			body << PrintHTML(lines);
		}

	}

}

//==============================================================================
std::string GetGMTDateTime() {
//==============================================================================
	namespace pt = boost::posix_time;

	pt::ptime now = pt::second_clock::universal_time();
	std::stringstream ss;
	ss << now.date().day_of_week() << ", " << now.date().day() << " "
			<< now.date().year() << " " << now.time_of_day() << " GMT";

	return ss.str();
}
//==============================================================================
std::string GetContentLengthStr(std::string str) {
//==============================================================================

	std::stringstream ss;
	std::size_t s = str.size(); // in bytes
	ss << "Content-Length: " << s << "\r\n";
	return ss.str();

}
//==============================================================================
std::string GetDateStr() {
//==============================================================================
	return "Date: " + GetGMTDateTime() + "\r\n";
}
//==============================================================================
std::string GetContentTypeStr(std::string type) {
//==============================================================================
	return "Content-Type: " + type + "; charset=utf-8\r\n";
}
//==============================================================================
std::string GetAllowStr(std::string allowedMethods) {
//==============================================================================

	return "Allow: " + allowedMethods + "\r\n";
}
//==============================================================================
std::string GetStatusStr(std::string status) {
//==============================================================================
	if (status.empty()) {

		return "Status: 200 OK\r\n";
	} else {

		return "Status: " + status + "\r\n";
	}

}
//==============================================================================
std::string PrintHTML(
		std::vector<std::pair<std::string, std::string> > printableLines) { //Set with arbitrary inputs
//==============================================================================
	std::stringstream ss;

	ss
			<< "<!DOCTYPE html>\r\n<html><head>\r\n<meta charset=\"utf-8\">\r\n</head>\r\n<body>\r\n<table border=\"1\">\r\n";

	for (auto line : printableLines) {

		ss
				<< "<tr><td>" + line.first + "</td><td>" + line.second
						+ "</td></tr>\r\n";

	}

	ss << "</table></body>\r\n</html>\r\n";

	return ss.str();
}
//==============================================================================
std::string PrintHTML(std::vector<GatewayBaseStruct*> printableStructs) {
//==============================================================================
	std::stringstream ss;

	ss
			<< "<!DOCTYPE html>\r\n<html><head>\r\n<meta charset=\"utf-8\">\r\n</head>\r\n<body>\r\n";

	HTMLPrintVisitor visitor;

	for (auto s : printableStructs) {
		ss << visitor.PrintStructHTML(s, &visitor);

	}
	ss << "</body>\r\n</html>\r\n";

	return ss.str();

}
//==============================================================================
std::string PrintJSON(
		std::vector<std::pair<std::string, std::string> > printableLines) { //rvalue could be non-string. Put string ensures that no exceptions occur. Use pair, not map because it ensures that input order = output order, map changes order
//==============================================================================
	Json::Value root;
	Json::StyledWriter writer;
	int i = 0; //this is necessary, otherwise elements with the same first value tuple overwrite each other
	try {
		if (printableLines.size() > 1) {
			for (auto line : printableLines) {
				root[std::to_string(i)][line.first] = line.second;
				i++;
			}
		} else {
			for (auto line : printableLines) {
				root[line.first] = line.second;

			}
		}
		std::string jsonOutput = writer.write(root);
		return jsonOutput;
	} catch (std::exception& e) {

		return "";
	}

}
//==============================================================================
std::string PrintJSON(std::vector<GatewayBaseStruct*> printableStructs) {
//==============================================================================
	JSONPrintVisitor visitor;
	Json::Value root;
	std::stringstream ss;

	for (auto s : printableStructs) {
		ss << visitor.PrintStructJSON(s, &visitor);
	}

	return ss.str();
}

}
