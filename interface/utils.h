/*  utils.h - function declarations for both client and server side
*  
*  author: Martin Elfvelin
*/

#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include <stdint.h>
#include <cstring>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include "tables.h"

namespace utils
{
  		uint64_t GenIRIGBTag();
  		int DecimalToBCD(int decimal);
  		int BCDToDecimal(int bcd); 
      int GetMsgType(uint8_t *pnData);      
      void PrintBinary(uint32_t nIn, int bigendian);
      
      namespace parse
      {
         std::string ParseMsgType(uint32_t& nMsgType);
         std::string ParseScrambler(uint32_t& nScrambler);
         std::string ParseFEC(uint32_t& nFEC);
         std::string ParseFrameFormat(uint32_t& nFrameFormat);
      }
} /* utils */

#endif /* TMTC_H */
