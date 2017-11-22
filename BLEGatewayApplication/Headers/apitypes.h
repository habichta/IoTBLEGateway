
#ifndef APITYPES_H_
#define APITYPES_H_

#ifdef __GNUC__

#define PACKSTRUCT( decl ) decl __attribute__((__packed__))
#define ALIGNED __attribute__((aligned(0x4)))

#else //msvc

#define PACKSTRUCT( decl ) __pragma( pack(push, 1) ) decl __pragma( pack(pop) )
#define ALIGNED

#endif


typedef enum CONSTANTS{

	BGAPI_PAYLOAD_SIZE = 60,
	BGAPI_READ_PAYLOAD = 22,
	BGAPI_WRITE_PAYLOAD = 20,
	GATT_MIN_HANDLE= 1,
	GATT_MAX_HANDLE = 65535,
	UUID_SIZE = 16,
	MAC_SIZE = 6,
	GAP_CON_HANDLE_DEFAULT = 0,
	DISCOVERY_INTERVAL = 6, // seconds
	SOCKET_POLL_TIMEOUT = 3000, //ms
	RECEIVE_HEADER_SIZE = 4 //length of header in bytes received by server
}Constants;




typedef unsigned char  uint8;
typedef unsigned short uint16;
typedef signed short   int16;
typedef unsigned long  uint32;
typedef signed char    int8;

typedef struct bd_addr_t
{
    uint8 addr[6];

}bd_addr;

typedef bd_addr hwaddr;
typedef struct
{
    uint8 len;
    uint8 data[];
}uint8array;


typedef struct
{
    uint8 len;
    int8 data[];
}string;


#endif
