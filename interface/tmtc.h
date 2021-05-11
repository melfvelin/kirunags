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

   	int encapsulate(const char *data, uint32_t data_size);
   	int decapsulate(const char *frame);
   	int parse_tm_header(uint8_t *frame_ptr);
 	int extract_data(uint8_t *frame_ptr);
    uint64_t GenIRIGBTag();
    int DecimalToBCD(int decimal);
   	int BCDToDecimal(int bcd);
   	
   	// 'public' declaration of TM_HEADER
   	TM_HEADER tm_header;
   	TM_FRAME tm_frame;
   	uint32_t postamble;
}





#endif /* TABLES_H */
