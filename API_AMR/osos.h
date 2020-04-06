//vvolkanunal@gmail.com wrote this file.

#ifndef __OSOS__H
#define __OSOS__H

#include "stdint.h"
#include "bsp.h"


#ifdef __cplusplus
extern "C" {
#endif

#define DEBUG

#ifdef DEBUG
#define DEBUG_LOG(...) do{printf(__VA_ARGS__);}while(0);
#else
#define DEBUG_LOG(...) do{ }while(0);
#endif

#define POLL_TIME_MS 60000


typedef enum amr_process
{
	AMR_IDLE,

	AMR_START_MESSAGE_SEND,
	AMR_START_MESSAGE_RECEIVED,
	
	AMR_READOUT_MESSAGE_SEND,
	AMR_READOUT_MESSAGE_SEND_COMPLETED,
	AMR_READOUT_MESSAGE_RECEIVING,
	AMR_READOUT_MESSAGE_RECEIVED,

	OBIS_PARSE_PROCESS,
	OBIS_PARSE_PROCESS_END,

}e_amr_process;


extern e_amr_process amr_process;

typedef enum error
{
    RETURN_OK = 0x00,
    INIT_ERROR = 0x01,

}err;

struct amr_config
{
  void (*write) (const unsigned char* data, uint8_t size);

	const char* amr_type;

  const char* SerialNumber;
	
	uint32_t bufsize;
	
	char *buf;
	
	void (*uart_init)(uint32_t baud);
	
};

void 
amr_get_readout(const struct amr_config*, const char**, unsigned int table_size);

err 
amr_init_config(const struct amr_config*);

void sys_process(struct amr_config*);


#ifdef __cplusplus
}
#endif



#endif
