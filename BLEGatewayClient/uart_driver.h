//==============================================================================
// Project   :  BLE Gateway
// File      :  uart_driver.h
// Author    :  Arthur Habicht
// Compiler  :  GCC
// Brief     :  Communication through PTTY port
//==============================================================================


#ifndef CH_ETHZ_AH_UART_DRIVER_H_
#define CH_ETHZ_AH_UART_DRIVER_H_


//---------- Includes ----------------------------------------------------------
#include <termios.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
 #include <sys/file.h>
#include "debug.h"

#define UART_TIMEOUT_ADJ 100


#ifdef __cplusplus
extern "C" {
#endif



//---------- Typedefs ----------------------------------------------------------

/*type defines flag given as input parameter for open system call wrapper */
typedef unsigned int open_syscall_t;



//==============================================================================
int uart_open(const char *serial_port, open_syscall_t open_flags);
int uart_configure(int serial_port_desc, struct termios* options);
//==============================================================================
/*
 * Opens and configures a serial connection to a tty port according to uart_configure
 * input: char* serial_port: valid string of tty device
 * 			   open_syscall_t open_flags: valid unsigned int flags for open system call wrapper
 * return: File descriptor of opened serial port, -1 if fatal error
 */


//==============================================================================
int uart_close(int serial_port_desc);
//==============================================================================

/*
 * Closes a previously opened tty port
 * input: char* serial_port_desc: file descriptor of opened tty device
 *
 * return: 0 if successful, -1 if error
 */


//==============================================================================
int uart_tx(int serial_port_desc, int length, unsigned char *data_buffer);
//==============================================================================
/*
 * Transmits data to previously opened tty port
 * input: int serial_port_desc: file descriptor of opened tty device
 *   		   int length: amount of data to be transmitted (in bytes)
 *   		   unsigned char *data_buffer: buffer for data to be transmitted
 *
 * return: number of bytes transmitted if successful, -1 if error
 */



//==============================================================================
int uart_rx(int serial_port_desc, int length, unsigned char *data_buffer,
		int timeout_ms);
//==============================================================================

/*
 * receives data from previously opened tty port
 * input: int serial_port_desc: file descriptor of opened tty device
 *   		   int length: amount of data to be transmitted (in bytes)
 *   		   unsigned char *data_buffer: buffer for data to be transmitted
 *   		   int timeout_ms: read timeout in ms
 *
 * return: number of bytes read if successful, -1 if error, 0 if EOF
 * */


#ifdef __cplusplus
}
#endif

#endif /* CH_ETHZ_AH_UART_DRIVER_H_ */
