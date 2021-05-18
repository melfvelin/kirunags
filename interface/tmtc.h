/*  tmtc.h
*
*
*/

#ifndef TMTC_H
#define TMTC_H

#include <iostream>
#include <stdint.h>
#include <cstring>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include "tables.h"

namespace tmtc
{

   	namespace telemetry
   	{
   		uint8_t *encapsulate(const char *message, uint32_t message_len);	
   		int decapsulate(uint8_t *frame_ptr);
   		uint64_t GenIRIGBTag();
   		int DecimalToBCD(int decimal);
   		int BCDToDecimal(int bcd);
   	}
   	
   	namespace telecommand
   	{
   		uint8_t *encapsulate(const char *message, uint32_t message_len);	
   		int decapsulate(uint8_t *tc_ptr, uint32_t data_size);
   	}
  }





#endif /* TABLES_H */
