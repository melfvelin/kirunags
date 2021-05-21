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
         void EncapsulateTM(uint8_t *rnOut, uint32_t nMsgType, uint32_t nScrambler, uint32_t nFEC, 
         uint32_t nFrameFormat, uint32_t nCaduSize, uint8_t *rnData);
         void DecapsulateTM(uint8_t *pnPacket, uint32_t& nMsglen, uint64_t& nTimeTag, uint32_t& nMsgType,
         uint32_t& nScrambler, uint32_t& nFEC, uint32_t& nFrameFormat, uint32_t& nCaduSize, uint8_t *pnData);
   		//uint8_t *encapsulate(const char *message, uint32_t message_len);	
   		//int decapsulate(uint8_t *frame_ptr);
   		uint64_t GenIRIGBTag();
   		int DecimalToBCD(int decimal);
   		int BCDToDecimal(int bcd);
   	}
   	
   	namespace telecommand
   	{ 
         void EncapsulateTC(uint8_t *pnOut, uint32_t nMsgType, uint32_t nCltuSize, uint8_t *pnData);
         void DecapsulateTC(uint8_t *pnPacket, uint32_t& nMsglen, uint64_t& nTimeTag, uint32_t& nMsgType,
         uint32_t& nCltuSize, uint8_t *pnData);
   		//uint8_t *encapsulate(const char *message, uint32_t message_len);	
   		//int decapsulate(uint8_t *tc_ptr, uint32_t data_size);
   	}
  }





#endif /* TABLES_H */
