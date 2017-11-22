//==============================================================================
// Project   :  BLE Gateway Daemon
// File      :  ble_daemon.c
// Author    :  Arthur Habicht
// Compiler  :  gcc
// Brief     :  Simple background process which detects add/remove udev events
//==============================================================================

#include "ble_daemon.h"

NodeToPid map[MAX_DEVICE_NR];
int pipes[MAX_DEVICE_NR][2];
fd_set select_master, select_tset;
int nfds;
char buf[MSG_SIZE];
struct timeval timeout;
typedef void (*sighandler_t)(int);
static sighandler_t

handle_signal (int sig_nr, sighandler_t signalhandler) { //used to mask SIGHUP signal
   struct sigaction n_sig, o_sig;
   n_sig.sa_handler = signalhandler; //ignore signal
   sigemptyset (&n_sig.sa_mask); //initialise struct
   n_sig.sa_flags = SA_RESTART; //interruptible function restart if interrupted by signal
   if (sigaction (sig_nr, &n_sig, &o_sig) < 0)
      return SIG_ERR;
   return o_sig.sa_handler;
}


void start_daemon(const char* log_name, int facility){

	int i;
	   pid_t pid;

	   //Daemonizing (child of init process)
	   if ((pid = fork ()) != 0) //kill parent process, now shell thinks that program finshed and child runs in background
	      exit (EXIT_FAILURE);

	   //Process independency
	   if (setsid() < 0) { //create new session, detach from controlling terminal,  new process leader has no controlling terminal
	      printf("%s cannot be session leader\n",
	         log_name);
	      exit (EXIT_FAILURE);
	   }
	   //ignore SIGHUB (sent to process when its controlling terminal is closed, normally shutdown
	   handle_signal (SIGHUP, SIG_IGN); //SIG_IGN pointer to a signal handler with input int = 1

	   if ((pid = fork ()) != 0) //second fork prevents that session leader (daemon) can open controlling terminal
	      exit (EXIT_FAILURE);
	   //change working directory
	   chdir ("/");

	   //no file permissions inherited, change this for security reasons
	   umask (0);
	   //close all file descriptors
	   for (i = sysconf (_SC_OPEN_MAX); i > 0; i--)
	      close (i);
	   //open log with syslog daemon, defined in /etc/rsyslog.conf
	   openlog ( log_name,
	             LOG_PID | LOG_CONS| LOG_NDELAY, facility );

}


void child_exit_handler() {
	int status;
	struct rusage r;
	pid_t pid;


		pid = wait3(&status, 0, &r);
		int i;
		for(i= 0; i<MAX_DEVICE_NR; i++){

			if(map[i].pid == pid){
				map[i].used = 0;
			}
		}

}



	int main(int argc, char *argv[]) {




		if (argc != 6) {
			fprintf(stdout,"%s\n", "Invalid number of arguments" );

			exit(1);
		}

		start_daemon("ble_daemon_log", LOG_LOCAL0);

		char* vendor_id = *(argv + 1); //2458
		char* product_id = *(argv + 2); //0001
		char* server_ip = *(argv + 3);
		char* server_port = *(argv + 4);
		char* binary_path = *(argv + 5);

		signal(SIGCHLD, child_exit_handler);


/*enable piping
//		FD_ZERO(&select_master);
//
//		int i;
//		for (i = 0; i < MAX_DEVICE_NR; i++) {
//
//			if (pipe(pipes[i]) == -1) {
//				fprintf(file, "Pipe error\n");
//				exit(1);
//			} else {
//
//				FD_SET(pipes[i][0], &select_master);
//			}
//		}
//		timeout.tv_sec = 0;
//		timeout.tv_usec = 0;
//		nfds = pipes[MAX_DEVICE_NR - 1][0] + 1;
*/
		struct udev *udev;
		struct udev_device *dev, *dev_info;
		struct udev_monitor* monitor;

		//Create the udev struct
		udev = udev_new();
		if (!udev) {
			syslog( LOG_ERR, "Can't create udev\n");
			exit(1);
		}

		//Setup monitor for tty devices
		monitor = udev_monitor_new_from_netlink(udev, "udev");
		udev_monitor_filter_add_match_subsystem_devtype(monitor, "tty", NULL);
		udev_monitor_enable_receiving(monitor);

		while (1) {
			dev = udev_monitor_receive_device(monitor);
			if (dev) {

				const char* action = udev_device_get_action(dev);
				const char* node = udev_device_get_devnode(dev);
				syslog( LOG_NOTICE, "Action: %s\n", action );



				if (action != NULL && strcmp(action, "add") == 0) {


					dev_info = udev_device_get_parent_with_subsystem_devtype(
							dev, "usb", "usb_device");
					if (!dev_info) {
						syslog( LOG_ERR, "Unable to find parent usb device\n" );


						exit(1);
					}

					const char* v_id = udev_device_get_sysattr_value(dev_info,
							"idVendor");
					const char* p_id = udev_device_get_sysattr_value(dev_info,
							"idProduct");


					if (v_id != NULL && p_id != NULL
							&& strcmp(v_id, vendor_id) == 0
							&& strcmp(p_id, product_id) == 0) {


						int i = 0; //search for next
						while (map[i].used && i < MAX_DEVICE_NR) { //linear search, improve with hashing
							i++;
						}
						if (i < MAX_DEVICE_NR) {
							syslog( LOG_NOTICE, "Fork new child\n" );

							pid_t pID = fork(); //start new client process (child)

							if (pID == 0) {
								//child
								/*enable piping
								//dup2(pipes[i][1], STDOUT_FILENO); copies file descriptors
								//close(pipes[i][0]); //close read
								//write(pipes[i][1], MSG, MSG_SIZE);
								 */
								execv(binary_path, (char *[] ) { binary_path,
												node, server_ip, server_port });


								_exit(1);
							}
							if (pID < 0) {
								syslog( LOG_ERR, "Fork error\n" );


							} else {
								/*enable piping
								//close(pipes[i][1]); //close write
								*/

								map[i].used = 1;
								map[i].pid = pID;
								map[i].key = node;


							}

						} else {
							//max nr of devices exceeded, kill process
						syslog( LOG_WARNING,
								"Maximum number of processes exceeded\n");


						}

					}

				} else if (action != NULL && strcmp(action, "remove") == 0) {

					int i;
					for (i = 0; i < MAX_DEVICE_NR; i++) {

						char* n = map[i].key;

						if (n != NULL) {

							if (map[i].used == 1 && strcmp(n, node) == 0) {

								kill(map[i].pid, SIGKILL);

								map[i].used = 0;


							}
						}

					}

				}

			} else {


				/*enable piping
				select_tset = select_master;


				if (select(nfds, &select_tset, NULL, NULL, &timeout) > 0) {
					fprintf(stdout, "sel\n");
					int i;
					for (i = 0; i < MAX_DEVICE_NR; i++) {
						if (FD_ISSET(pipes[i][0], &select_tset)) {

							if (read(pipes[i][0], buf, MSG_SIZE) > 0) {

									//data from child in buf
								}

							}

						}

					}
				}
				*/

			}
			usleep(400 * 1000);
		}

		syslog( LOG_NOTICE, "Daemon shut down\n");
		   closelog();

		return EXIT_SUCCESS;
	}
