#include "stdio.h"
#include "string.h"
#include "stdbool.h"
#include "stdlib.h"

#include "osos.h"
#include "bsp.h"



#define AMR_START_BAUD 300
#define AMR_READOUT_BAUD 9600

static bool makel_brand = false;

extern uint32_t systick_tick;
/*
static const unsigned char Start_Message[] = {  '\x2f',
                                                '\x3f',
                                                '\x21',
                                                '\x0D',
                                                '\x0A',

};
*/

static const  char Readout_Message_b9600[] = {  '\x06',
                                                        '\x30', 
                                                        '\x35', 
                                                        '\x30',
                                                        '\x0D',
                                                        '\x0A',
};

typedef enum amr_process
{
    AMR_IDLE,

    AMR_START_MESSAGE_SEND,
    AMR_START_MESSAGE_RECEIVED,
    
    AMR_READOUT_MESSAGE_SEND,
    AMR_READOUT_MESSAGE_RECEIVED,

    OBIS_PARSE_PROCESS,
	OBIS_PARSE_PROCESS_END,

}e_amr_process;


e_amr_process amr_process = AMR_IDLE;

static void 
parse_process(char* buffer, const char **table, unsigned int size)
{
	if(amr_process == OBIS_PARSE_PROCESS)
	{
		char *p =  NULL;
		for(int i = 0; i < size; i++)
		{
			
			p = strstr(buffer, table[i]);
			float fvalue = 0;
			if(p)
			{
				p = strstr(p, "(");
				if(p)
				{
					char buf[20];
					int i = 0;
				  
					while(*p++ != '*')
					{
						buf[i] = *p;
						i++;
					}
				  
					buf[i-1] = '\0';
					fvalue = atof(buf);
					
				}
			}
			
			DEBUG_LOG("\r\n Obis Code = %s -> ", table[i]);
			DEBUG_LOG("Value = %f \r\n", fvalue);
		}
		
		
		amr_process = OBIS_PARSE_PROCESS_END;
	}
}

static void 
send_start_message(const struct amr_config* self)
{
	if (amr_process == AMR_IDLE)
	{
		char buf[20] = {0};
		self -> uart_init(AMR_START_BAUD);
		memset(self -> buf, 0, self -> bufsize);

		const  char* message_start = "/?";
		const  char* message_end = "!\r\n";
		const  char* makel_req = "MSY";

		if (!makel_brand && self -> SerialNumber != NULL)
		{
			snprintf(buf, 20, "%s%s%s", message_start, self -> SerialNumber, message_end);
		}
		
		else if (self -> SerialNumber == NULL)
		{
			snprintf(buf, 20, "%s%s", message_start, message_end);
			
		}

		else if (makel_brand)
		{
			snprintf(buf, 20, "%s%s%s%s", message_start, makel_req, self -> SerialNumber, message_end);
		}

		self -> write(buf, strlen(buf));
		DEBUG_LOG("Start Message Send\r\n");
		DEBUG_LOG("Send data = %s", buf);


		amr_process = AMR_START_MESSAGE_SEND;
	}
}

static void
send_readout_message(const struct amr_config* self, const char* buffer)
{
	if (amr_process == AMR_START_MESSAGE_RECEIVED)
	{
		DEBUG_LOG("Readout Message Send\r\n");

		memset(self -> buf, 0, self -> bufsize);
		self -> write(buffer, strlen(buffer));

		amr_process = AMR_READOUT_MESSAGE_SEND;
		self -> uart_init(9600);
	}
}

void 
amr_get_readout(const struct amr_config* self, const char** obis_table, unsigned int table_size)
{
    send_start_message(self);
	
	send_readout_message(self, Readout_Message_b9600);
	
	parse_process(self -> buf, obis_table, table_size);
}

err 
amr_init_config(const struct amr_config* self)
{
    err ret = RETURN_OK;

    if (self -> write == NULL )
    {
        ret = INIT_ERROR;
    }

    else if (strcmp(self -> amr_type, "MAKEL") == 0 && self -> SerialNumber != NULL)
    {
        makel_brand = true;
    }

    return ret;
}

void sys_process(struct amr_config* self)
{
	systick_tick++;
	
	if (strlen(self -> buf) != 0 && systick_tick > 1000 && amr_process == AMR_START_MESSAGE_SEND)
	{
		amr_process = AMR_START_MESSAGE_RECEIVED;
		systick_tick = 0;
	}
	
	else if (strlen(self -> buf) != 0 && systick_tick > 1500 && amr_process == AMR_READOUT_MESSAGE_SEND)
	{
		amr_process = OBIS_PARSE_PROCESS;
		systick_tick = 0;
	}
	
	
}

