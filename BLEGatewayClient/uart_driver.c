//==============================================================================
// Project   :  BLE Gateway
// File      :  uart_driver.c
// Author    :  Arthur Habicht
// Compiler  :  GCC
// Brief     :  Communication through PTTY port
//==============================================================================

#include "uart_driver.h"

//==============================================================================
int uart_open(const char* serial_port, open_syscall_t open_flags) {
//==============================================================================
	struct termios options;
	int serial_port_desc;
	char* serial_port_name;

	if ((serial_port_desc = open(serial_port, open_flags)) < 0) {

		DebugPrint("error %d: Client: failed attempt to open %s: %s\n", errno,
				serial_port, strerror(errno));

		return -1;
	}

	else {

		if ((serial_port_name = ttyname(serial_port_desc)) == NULL) {

			DebugPrint("error %d, Client: Could not retrieve port name: %s\n",
					errno, strerror(errno));

		}
		fprintf(stdout, "Opened port: %s\n", serial_port_name);

		if (isatty(serial_port_desc)) {
			if (uart_configure(serial_port_desc, &options) != 0) {

				DebugPrint(
						"error %d, Client: Could not configure serial connection for %s: %s\n",
						errno, serial_port, strerror(errno));

				return -1;
			}

			//Locking file, so only one process can connect to the tty file
			int flock_err = flock(serial_port_desc, LOCK_EX | LOCK_NB); //exclusive lock, non-blocking

			if (flock_err != 0) {
				DebugPrint(
						"error %d, Client: Could not obtain lock for %s, might be in use by another process: %s\n",
						errno, serial_port, strerror(errno));

				return -1;

			}
			tcflush(serial_port_desc, TCIFLUSH);
			return serial_port_desc;

		} else {
			DebugPrint("error %d, Client input port %s is not a tty: %s\n",
					errno, serial_port, strerror(errno));

			return -1;
		}
	}
}

//==============================================================================
int uart_close(int serial_port_desc) {
//==============================================================================
	if (close(serial_port_desc)) {

		DebugPrint("error %d, Client: Port could not be closed: %s\n", errno,
				strerror(errno));

		return -1;
	} else {

		flock(serial_port_desc, LOCK_UN);
		DebugPrint("%s\n", "Client: Serial port closed successfully");

		return 0;
	}
}
//==============================================================================
int uart_configure(int serial_port_desc, struct termios* options) {
//==============================================================================

	int ret = tcgetattr(serial_port_desc, options); //get current serial options
	ret |= cfsetispeed(options, B115200); //Baud rate: 115200
	ret |= cfsetospeed(options, B115200);

	/*
	 * Set option flags: refer to termios man page
	 */

	/*Disable: PARENB(parity generation on output and parity checking on input), CSTOPB (set two stop bits)
	 * CSIZE(Character size mask), CRTSCTS(Enable RTS/CTS (hardware) flow control)
	 * HUPCL(Lower modem control lines)
	 */
	options->c_cflag &= ~(PARENB | CSTOPB | CSIZE | CRTSCTS | HUPCL);

	/*Enforce: CS8(Character size 8 bits), CLOCAL (Ignore modem status lines)
	 *CREAD (Enable receiver)
	 */
	options->c_cflag |= (CS8 | CLOCAL | CREAD);

	/*Disable: ICANON (Canonical input), ISIG (Canonical input), ECHO/ECHOE/ECHOK/ECHONL/ECHOCTL/ECHOPRT/ECHOKE (Enable echo)
	 *IEXTEN(Enable extended input character processing)
	 */
	options->c_lflag &= ~(ICANON | ISIG | ECHO | ECHOE | ECHOK | ECHONL
			| ECHOCTL | ECHOPRT | ECHOKE | IEXTEN);

	/*Disable: INPCK(input parity checking), IXON/IXOFF (XON/XOFF flow control on output)
	 * IXANY(Any character can restart stopped output), ICRNL(Translate carriage return to newline on input)
	 */
	options->c_iflag &= ~(INPCK | IXON | IXOFF | IXANY | ICRNL);

	/*Disable: OPOST (Post-process output), ONLCR(Map NL to CR-NL on output)
	 */
	options->c_oflag &= ~(OPOST | ONLCR);

	int i;
	for (i = 0; i < sizeof(options->c_cc); i++)

		/*Since ICANON bit is switched off, raw mode is activated. When reading from a tty channel,
		 * timing issues may occur*/

		options->c_cc[i] = _POSIX_VDISABLE;
	/*
	 * VTIME decides when the read syscall returns. After each received byte, the timer
	 * starts. If it reaches VTIME tenths of a second, read returns. This allows
	 * bursts to be buffered to reduce amount of read calls.
	 */
	options->c_cc[VTIME] = 0; // calls to read wait indefinitely if VTIME=0

	/* If at least VMIN bytes have been received, read returns*/
	options->c_cc[VMIN] = 1; // VTIME waits for first char indefinitely

	/*set new opt for the port: TCSAFLUSH is an optional action.
	 **writes all queued input to tty and flushes the input queue. Then it sets the
	 new options.*/
	ret |= tcsetattr(serial_port_desc, TCSAFLUSH, options);

	return ret;

}

//==============================================================================
int uart_tx(int serial_port_desc, int length, unsigned char *data_buffer) {
//==============================================================================
	int l = length;
	ssize_t bytes_written;

	while (length > 0) {

		if ((bytes_written = write(serial_port_desc, data_buffer, length))
				< 0) {

			DebugPrint("error %d,  Client: Could not write data to port %s, bytes written = %d, remaining bytes = %d: %s\n",
					errno, ttyname(serial_port_desc), l - length, length,
					strerror(errno));

			return -1;
		}

		length -= bytes_written;
		data_buffer += length;
	}

	return l;

}
//==============================================================================
int uart_rx(int serial_port_desc, int length, unsigned char *data_buffer,
		int timeout_ms) {
//==============================================================================

	int l = length;
	ssize_t bytes_read;

	struct termios options;
	tcgetattr(serial_port_desc, &options);
	options.c_cc[VTIME] = timeout_ms / UART_TIMEOUT_ADJ; // Pure timed read. 10ms wait between bytes
	options.c_cc[VMIN] = 0;
	tcsetattr(serial_port_desc, TCSANOW, &options);

	while (length) {

		bytes_read = read(serial_port_desc, data_buffer, length);

		if (bytes_read < 0) {

			DebugPrint("error %d, Client: could not read data from port %s, bytes read = %d, remaining bytes = %d: %s\n",
					errno, ttyname(serial_port_desc), l - length, length,
					strerror(errno));


			return -1;
		} else if (bytes_read == 0) //EOF can happen even with raw data stream
				{
			return 0;
		}

		length -= bytes_read;
		data_buffer += length;
	}
	return l;

}

