//==============================================================================
// Project   :  BLE Gateway Client
// File      :  client.c
// Author    :  Arthur Habicht
// Compiler  :  gcc
// Brief     :  BLED Client functions
//==============================================================================

//TODO: Daemon which notifies if e device is plugged in or not. shut down program if unplugged

#include "client.h"

volatile int tcp_socket_descriptor;
volatile int bled_serial_descriptor;
const char* server_name;
const char* tty_name;
const char* port_number;
pthread_mutex_t lock;
int abort_cmd;

int main(int argc, char *argv[]) {

	DebugPrint("%s","Client Started\n");


	if (argc < 4) {
		DebugPrint("%s",
				"Client: Not enough arguments: tty_path,server_name, server_port\n");

		exit(-1);
	}

	tty_name = argv[1];
	server_name = argv[2];
	port_number = argv[3];
	pthread_t response_thread, command_thread;

//Attempt to create serial connection with BLED112
	if ((bled_serial_descriptor = uart_open(tty_name, O_RDWR | O_NOCTTY)) < 0) {
		fprintf(stdout,
				"error %d, Client: could not establish connection to %s: %s\n",
				errno, tty_name, strerror(errno));

		exit(-1);
	}

//Attempt to create tcp connection with server application
	int retry = 0;

	fprintf(stdout, "Client: Attempting to connect to %s \n", server_name);
	while ((tcp_socket_descriptor = connect_to_server(server_name,
			atoi(port_number))) < 0) {
		fprintf(stdout, "Client Retry: connect to %s \n", server_name);
		sleep(1);
		retry++;

		if (retry > CONNECTION_RETRY) {

			fprintf(stdout,
					"error %d, Client: could not establish connection to %s: %s\n",
					errno, server_name, strerror(errno));

			exit(-1);
		}

	}

//initialize function pointer for sending data to serial port (BGLIB specific)
	bglib_output = send_message_to_serial_port;
	ble_cmd_gap_end_procedure(); //this ensures that running discovery on the BLED is terminated.
//initialize lock
	if (pthread_mutex_init(&lock, NULL) != 0) {
		DebugPrint("error %d, Client: Could not initialize mutex for %s: %s\n",
				errno, tty_name, strerror(errno));

		exit(-1);
	}
//initialize abort command for inter-thread communication
	abort_cmd = 0;

//start worker threads for receiving and transmitting data
	if (pthread_create(&response_thread, NULL, response_thread_handler, NULL)
			< 0) {

		DebugPrint(
				"error %d, Client: Could not create response thread for %s: %s\n",
				errno, tty_name, strerror(errno));

		exit(-1);
	}

	if (pthread_create(&command_thread, NULL, command_thread_handler, NULL)
			< 0) {

		DebugPrint(
				"error %d, Client: Could not create command thread for %s: %s\n",
				errno, tty_name, strerror(errno));

		exit(-1);
	}
//main thread waits for worker threads to exit
	pthread_join(response_thread, NULL);
	pthread_join(command_thread, NULL);

	close(tcp_socket_descriptor);
	uart_close(bled_serial_descriptor);
	fprintf(stderr, "Client Shutdown: shutdown for for %s: %s \n", tty_name,
			strerror(errno));
	return 0;
}

//read from socket, parse, translate to command and send to dongle
//parse: 1byte = command_idx  following bytes according to specification of commands
//socket/uart failure?
void* command_thread_handler() {

	while (1) {

		if (read_abort_threadsafe()) {

			DebugPrint(
					"Client Abort: Shutting down command thread for %s: %s \n",
					tty_name, strerror(errno));

			pthread_exit(NULL);
		}
		if (process_message_from_tcpsocket()) {

			DebugPrint(
					"Client Abort: Shutting down command thread for %s: %s \n",
					tty_name, strerror(errno));

			signal_abort_threadsafe();
			pthread_exit(NULL);

		}
	}
	return 0;

}

//reads data from serial port, calls handler which is callback function, handler translates to bytestream and sends over socket
// read from serial connection, use bglib to parse, callbacks forward a struct to socket
//write to sockets: first byte response/evt idx, following bytes according to specification
void* response_thread_handler() {

	while (1) {
		if (read_abort_threadsafe()) {
			DebugPrint(
					"Client Abort: Shutting down response thread for %s: %s \n",
					tty_name, strerror(errno));

			pthread_exit(NULL);
		}

		if (read_message_from_serial_port()) {
			DebugPrint(
					"Client Abort: Shutting down response thread for %s: %s \n",
					tty_name, strerror(errno));

			signal_abort_threadsafe();
			pthread_exit(NULL);
		}
	}
	return 0;
}

int connect_to_server(const char* server_ip, int server_port) {

	int socket_fd, port_number;
	struct sockaddr_in server; //server information

	port_number = server_port;
	socket_fd = socket(AF_INET, SOCK_STREAM, 0); //create socket
	if (socket_fd < 0) {
		DebugPrint("error %d: Client: Could not open socket for: %s: %s\n",
				errno, server_ip, strerror(errno));

		exit(-1);
	}

	bzero((char *) &server, sizeof(server)); //set memory to zero
	server.sin_family = AF_INET; //Internet Domain
	server.sin_addr.s_addr = inet_addr(server_ip); //server address, already big endian
	server.sin_port = htons(port_number); //Host to network byte order, from little endian to big endian

	if (connect(socket_fd, (struct sockaddr *) &server, sizeof(server)) < 0) {

		DebugPrint("error %d: Client: Could not connect to: %s: %s\n", errno,
				server_ip, strerror(errno));

		return -1;
	} else {
		fprintf(stdout, "Client: connected to: %s: successful\n", server_ip);
		return socket_fd;
	}
}

//BGLIB specific
int read_message_from_serial_port() { //needs to be in a loop
	int rread;
	const struct ble_msg *apimsg;
	struct ble_header apihdr;
	unsigned char data[256]; //enough for BLE

	rread = uart_rx(bled_serial_descriptor, sizeof(apihdr),
			(unsigned char*) &apihdr, 1000);
	if (rread < 0) {
		return errno;
	} else if (rread == 0) {
		return 0;
	}

	DebugPrint("Serial->Client: READ: header %d bytes from %s ;\n ", rread,
			tty_name);

	//read data if needed (lolen > 0)
	if (apihdr.lolen) {
		rread = uart_rx(bled_serial_descriptor, apihdr.lolen, data, 1000);
		if (rread < 0) {
			return errno;
		}
	}

	DebugPrint("Serial->Client: READ: data %d bytes from %s ;\n ", rread,
			tty_name);

	apimsg = ble_get_msg_hdr(apihdr); //parse header of BGAPI packet and set apimsg->handler accordingly
	if (!apimsg) {

		DebugPrint("error: Client: Message not found:%d:%d\n",
				(int ) apihdr.cls, (int ) apihdr.command);

		return -1;
	}
	apimsg->handler(data); //call the callback function/handler that was set before, pass data to handler

	return 0;
}

//recipient needs to transform network byte order to little endian for first send (length)!
void send_message_to_tcpsocket(char* buffer, size_t length) {

	uint32_t length_no = length;
	length_no = htonl(length_no); // 4 byte header for transmission of data size

	//Send size of bytestream first
	DebugPrint("Client->Server: WRITE_HEADER: %zu bytes to %s \n",
			sizeof(length_no), server_name);

	if (send(tcp_socket_descriptor, &length_no, sizeof(length_no), 0) < 0) {

		DebugPrint(
				"error %d: Client: %s could not send buffer length to %s: %s\n",
				errno, tty_name, server_name, strerror(errno));

		signal_abort_threadsafe();
		pthread_exit(NULL);
	}
	//send buffer data (msg structs)
	DebugPrint("Client->Server: WRITE_DATA: %zu bytes to %s \n", length,
			server_name);

	if (send(tcp_socket_descriptor, buffer, length, 0) < 0) {
		DebugPrint(
				"error %d: Client: %s could not send buffer data to %s: %s\n",
				errno, tty_name, server_name, strerror(errno));

		signal_abort_threadsafe();
		pthread_exit(NULL);
	}

}

void send_message_to_serial_port(uint8 header_length, uint8* header,
		uint16 data_length, uint8* data) {

	int written;

	written = uart_tx(bled_serial_descriptor, header_length, header);
	if (written < 0) {

		DebugPrint("error %d: Client: Could not write header to %s: %s\n",
				(int) errno, tty_name, strerror(errno));

		signal_abort_threadsafe();
		pthread_exit(NULL);
	}
	DebugPrint("Client->Serial: WRITE_HEADER: %d bytes to %s \n", written,
			tty_name);

	written = uart_tx(bled_serial_descriptor, data_length, data);
	if (written < 0) {
		DebugPrint("error %d: Client: Could not write data to %s: %s\n", errno,
				tty_name, strerror(errno));

		signal_abort_threadsafe();
		pthread_exit(NULL);
	}
	DebugPrint("Client->Serial: WRITE_DATA: %d bytes to %s \n", written,
			tty_name);

}

//first size must be send using htonl!
//The serialized message must be encoded like this: "command_index | argument_struct (NULL if none)"
int process_message_from_tcpsocket() {

//read 4 byte header uint32_t which denotes size of data in next message

	int rread;

	unsigned char header_b[RECEIVE_HEADER_SIZE];

	rread = read_bytes_from_tcpsocket(RECEIVE_HEADER_SIZE, header_b);

	if (rread < 0) {
		return errno;
	} else if (rread == 0) { //Socket closed by server

		DebugPrint("%s\n", "Client Abort: Server closed socket");

		return -1;
	}

	//uint32_t data_size = ntohl(*((uint32_t*) header_b));

	uint32_t data_size = 0;
	int i;
	for (i = 0; i < RECEIVE_HEADER_SIZE; ++i) {
		data_size = (data_size << 8) + header_b[i];
	}

	DebugPrint("Server->Client: READ: TCP header %d bytes from %s\n ", rread,
			server_name);

	//read data_size bytes  data if more than 0 bytes had been sent
	if (data_size) {
		unsigned char data_b[data_size];

		if ((rread = read_bytes_from_tcpsocket(data_size, data_b)) < 0) {
			return errno;
		}
		DebugPrint("Server->Client: READ: TCP data %d bytes from %s\n ", rread,
				server_name);

//deserialize the incoming message and call corresponding commands

		decode_incoming_tcp_message(data_b, data_size);

	}

	return 0;
}

int read_bytes_from_tcpsocket(int length, unsigned char* data_buffer) {

	int l = length;
	ssize_t bytes_read;

	while (length) {

		bytes_read = read(tcp_socket_descriptor, data_buffer, length);

		if (bytes_read < 0) {
			DebugPrint(
					"error %d, Client: Could not read data from  %s, bytes read = %d, remaining bytes = %d: %s\n",
					errno, server_name, RECEIVE_HEADER_SIZE - length, length,
					strerror(errno));

			return -1;
		} else if (bytes_read == 0) //socket closed by peer/server
				{

			return 0;

		}

		length -= bytes_read;
		data_buffer += length;
	}

	return l;

}

//assumes following layout of data using tpl format strings: "command_index (uint16_t or type v for tpl ) | argument_struct (NULL if none)"
int decode_incoming_tcp_message(unsigned char* message, uint32_t length) {

	uint16_t command_idx;
	if (tpl_peek(TPL_MEM | TPL_DATAPEEK, message, length, "v",
			&command_idx) == NULL) {

		DebugPrint(
				"error, Client: Could not decode incoming TCP message from %s to %s\n",
				server_name, tty_name);

		return -1;
	}

	switch (command_idx) {

	case ble_cmd_system_get_info_idx: {
		DebugPrint("Client Execute: TCP command %d: get_info  from %s \n ",
				command_idx, server_name);


		ble_cmd_system_get_info();

		break;
	}

	case ble_cmd_gap_end_procedure_idx: {
		DebugPrint("Client Execute: TCP command %d: end_procedure  from %s \n ",
				command_idx, server_name);

		ble_cmd_gap_end_procedure();

		break;
	}

	case ble_cmd_system_get_connections_idx: {
		DebugPrint(
				"Client Execute: TCP command %d: get_max_connections  from %s \n ",
				command_idx, server_name);

		ble_cmd_system_get_connections();

		break;
	}
	case ble_cmd_gap_set_scan_parameters_idx: {
		DebugPrint(
				"Client Execute: TCP command %d: set_scan_params  from %s \n ",
				command_idx, server_name);

		//deserialize
		tpl_node *tn;
		uint16_t command_idx, scanInt, scanWin;
		char activeSc;

		if ((tn = tpl_map("vvvc", &command_idx, &scanInt, &scanWin, &activeSc))
				== NULL) {

			DebugPrint("error %d, Client: Could not deserialize data: %s ",
					errno, strerror(errno));

			return -1;
		}
		tpl_load(tn, TPL_MEM, message, length);
		tpl_unpack(tn, 0);
		tpl_free(tn);

		ble_cmd_gap_set_scan_parameters(scanInt, scanWin, activeSc); //BGLIB command

		break;
	}
	case ble_cmd_gap_discover_idx: {

		DebugPrint("Client Execute: TCP command %d: discover  from %s \n ",
				command_idx, server_name);

		//deserialize
		tpl_node *tn;
		uint16_t command_idx, scan_mode;
		if ((tn = tpl_map("vv", &command_idx, &scan_mode)) == NULL) {
			DebugPrint("error %d, Client: Could not deserialize data: %s ",
					errno, strerror(errno));

			return -1;
		}
		tpl_load(tn, TPL_MEM, message, length);
		tpl_unpack(tn, 0);
		tpl_free(tn);

		ble_cmd_gap_discover(scan_mode); //BGLIB command
		break;
	}

	case ble_cmd_gap_connect_direct_idx: {

		DebugPrint("Client Execute: TCP command %d: connect_direct from %s \n ",
				command_idx, server_name);

		//deserialize
		tpl_node *tn;

		uint16_t command_idx = 0, conn_interval_min = 0, conn_interval_max = 0,
				timeout = 0, slave_latency = 0;
		bd_addr mac_address;
		uint8_t address_type = 0;

		if ((tn = tpl_map((char*) "vS(c#)cvvvv", &command_idx, &mac_address,
				MAC_SIZE, &address_type, &conn_interval_min, &conn_interval_max,
				&timeout, &slave_latency)) == NULL) {

			DebugPrint("error %d, Client: Could not deserialize data: %s ",
					errno, strerror(errno));

			return -1;
		}

		tpl_load(tn, TPL_MEM, message, length);
		tpl_unpack(tn, 0);
		tpl_free(tn);

		ble_cmd_gap_connect_direct(&mac_address, address_type,
				conn_interval_min, conn_interval_max, timeout, slave_latency); //BGLIB command

		break;
	}
	case ble_cmd_connection_get_status_idx: {
		DebugPrint("Client Execute: TCP command %d: get_status  from %s\n ",
				command_idx, server_name);

		//deserialize
		tpl_node *tn;
		uint16_t command_idx;
		uint8_t connection_handle;

		if ((tn = tpl_map((char*) "vc", &command_idx, &connection_handle))
				== NULL) {
			DebugPrint("error %d, Client: Could not deserialize data: %s\n ",
					errno, strerror(errno));

			return -1;
		}
		tpl_load(tn, TPL_MEM, message, length);
		tpl_unpack(tn, 0);
		tpl_free(tn);

		ble_cmd_connection_get_status(connection_handle); //BGLIB command

		break;
	}

	case ble_cmd_connection_disconnect_idx: {
		DebugPrint("Client Execute: TCP command %d: disconnect  from %s \n ",
				command_idx, server_name);

		//deserialize
		tpl_node *tn;
		uint16_t command_idx;
		uint8_t connection_handle;

		if ((tn = tpl_map((char*) "vc", &command_idx, &connection_handle))
				== NULL) {

			DebugPrint("error %d, Client: Could not deserialize data: %s ",
					errno, strerror(errno));

			return -1;
		}
		tpl_load(tn, TPL_MEM, message, length);
		tpl_unpack(tn, 0);
		tpl_free(tn);

		ble_cmd_connection_disconnect(connection_handle); //BGLIB command

		break;
	}
	case ble_cmd_connection_update_idx: {
		DebugPrint(
				"Client Execute: TCP command %d: connection_update  from %s \n ",
				command_idx, server_name);

		//deserialize
		tpl_node *tn;
		uint16_t command_idx, conn_interval_min, conn_interval_max, timeout,
				slave_latency;
		uint8_t connection_handle;

		if ((tn = tpl_map((char*) "vcvvvv", &command_idx, &connection_handle,
				&conn_interval_min, &conn_interval_max, &timeout,
				&slave_latency)) == NULL) {

			DebugPrint("error %d, Client: Could not deserialize data: %s\n",
					errno, strerror(errno));

			return -1;
		}
		tpl_load(tn, TPL_MEM, message, length);
		tpl_unpack(tn, 0);
		tpl_free(tn);

		ble_cmd_connection_update(connection_handle, conn_interval_min,
				conn_interval_max, slave_latency, timeout); //BGLIB command

		break;
	}

	case ble_cmd_attclient_read_by_group_type_idx: {
		DebugPrint("Client Execute: TCP command %d: read_by_group  from %s \n ",
				command_idx, server_name);

		//deserialize
		tpl_node *tn;
		tpl_bin tb;
		uint16_t command_idx, start, end;
		uint8_t connection_handle;

		if ((tn = tpl_map((char*) "vcvvB", &command_idx, &connection_handle,
				&start, &end, &tb)) == NULL) {
			DebugPrint("error %d, Client: Could not deserialize data: %s ",
					errno, strerror(errno));

			return -1;
		}
		tpl_load(tn, TPL_MEM, message, length);
		tpl_unpack(tn, 0);
		tpl_free(tn);

		ble_cmd_attclient_read_by_group_type(connection_handle, start, end,
				tb.sz, tb.addr); //BGLIB command

		free(tb.addr);

		break;
	}
	case ble_cmd_attclient_read_by_type_idx: {
		DebugPrint("Client Execute: TCP command %d: read_by_type  from %s \n ",
				command_idx, server_name);

		//deserialize
		tpl_node *tn;
		tpl_bin tb;
		uint16_t command_idx, start, end;
		uint8_t connection_handle;

		if ((tn = tpl_map((char*) "vcvvB", &command_idx, &connection_handle,
				&start, &end, &tb)) == NULL) {
			DebugPrint("error %d, Client: Could not deserialize data: %s ",
					errno, strerror(errno));

			return -1;
		}
		tpl_load(tn, TPL_MEM, message, length);
		tpl_unpack(tn, 0);
		tpl_free(tn);

		ble_cmd_attclient_read_by_type(connection_handle, start, end, tb.sz,
				tb.addr); //BGLIB command

		free(tb.addr);

		break;
	}

	case ble_cmd_attclient_find_information_idx: {
		DebugPrint(
				"Client Execute: TCP command %d: find_information  from %s \n ",
				command_idx, server_name);

		//deserialize
		tpl_node *tn;
		uint16_t command_idx, start, end;
		uint8_t connection_handle;

		if ((tn = tpl_map((char*) "vcvv", &command_idx, &connection_handle,
				&start, &end)) == NULL) {
			DebugPrint("error %d, Client: Could not deserialize data: %s ",
					errno, strerror(errno));

			return -1;
		}
		tpl_load(tn, TPL_MEM, message, length);
		tpl_unpack(tn, 0);
		tpl_free(tn);

		ble_cmd_attclient_find_information(connection_handle, start, end); //BGLIB command

		break;
	}

	case ble_cmd_attclient_read_by_handle_idx: {
		DebugPrint(
				"Client Execute: TCP command %d: read_by_handle  from %s \n ",
				command_idx, server_name);

		//deserialize
		tpl_node *tn;
		uint16_t command_idx, attribute_handle;
		uint8_t connection_handle;

		if ((tn = tpl_map((char*) "vcv", &command_idx, &connection_handle,
				&attribute_handle)) == NULL) {
			DebugPrint("error %d, Client: Could not deserialize data: %s ",
					errno, strerror(errno));

			return -1;
		}
		tpl_load(tn, TPL_MEM, message, length);
		tpl_unpack(tn, 0);
		tpl_free(tn);

		ble_cmd_attclient_read_by_handle(connection_handle, attribute_handle); //BGLIB command

		break;
	}
	case ble_cmd_attclient_attribute_write_idx: {
		DebugPrint("Client Execute: TCP command %d: read_by_type  from %s \n ",
				command_idx, server_name);

		//deserialize
		tpl_node *tn;
		tpl_bin tb;
		uint16_t command_idx, attribute_handle;
		uint8_t connection_handle;

		if ((tn = tpl_map((char*) "vcvB", &command_idx, &connection_handle,
				&attribute_handle, &tb)) == NULL) {
			DebugPrint("error %d, Client: Could not deserialize data: %s\n",
					errno, strerror(errno));

			return -1;
		}
		tpl_load(tn, TPL_MEM, message, length);
		tpl_unpack(tn, 0);
		tpl_free(tn);

		ble_cmd_attclient_attribute_write(connection_handle, attribute_handle,
				tb.sz, tb.addr); //BGLIB command

		free(tb.addr);

		break;
	}

	default: {
		break;
	}

	}

	return 0;

}

int read_abort_threadsafe() {

	int a;

	pthread_mutex_lock(&lock);
	a = abort_cmd;
	pthread_mutex_unlock(&lock);

	return a;

}

int signal_abort_threadsafe() {

	pthread_mutex_lock(&lock);
	abort_cmd = 1;
	pthread_mutex_unlock(&lock);

	return 0;
}

