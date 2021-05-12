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
   	int decapsulate(uint8_t *tc_ptr, uint32_t data_size);
   	int parse_tm_header(uint8_t *frame_ptr);
 	  int extract_data(uint8_t *frame_ptr);
    uint64_t GenIRIGBTag();
    int DecimalToBCD(int decimal);
   	int BCDToDecimal(int bcd);
    void sendTC_frame();
  }





#endif /* TABLES_H */
