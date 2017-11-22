
#ifndef CH_ETHZ_AH_BLEG_CLIENT_H_
#define CH_ETHZ_AH_BLEG_CLIENT_H_



#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


#include "uart_driver.h"
#include "cmd_def.h"
#include "tpl.h"

//TCP Socket Protocol:

#define CONNECTION_RETRY 2 //no. of connection retries in main function
#define RECEIVE_HEADER_SIZE 4 // length of header in bytes received by server

//




int connect_to_server();
int read_message_from_serial_port();
void send_message_to_serial_port(uint8 header_length, uint8* header, uint16 data_length, uint8* data);
int process_message_from_tcpsocket();
int read_bytes_from_tcpsocket(int length, unsigned char* data_buffer);
int decode_incoming_tcp_message(unsigned char* message, uint32_t length);
void send_message_to_tcpsocket(char* buffer,size_t length);
void* command_thread_handler();
void* response_thread_handler();
int read_abort_threadsafe();
int signal_abort_threadsafe();



#endif /* CH_ETHZ_AH_BLEDG_CLIENT_H_ */
