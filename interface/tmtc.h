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
         void EncapsulateTM(uint8_t *rnOut, uint32_t nScrambler, uint32_t nFEC, 
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
         void EncapsulateTC(uint8_t *pnOut, uint32_t nCltuSize, uint8_t *pnData);
         void DecapsulateTC(uint8_t *pnPacket, uint32_t& nMsglen, uint32_t& nMsgType,
         uint32_t& nCltuSize, uint8_t *pnData);
         void EncapsulateTCACK(uint8_t *pnOut, uint32_t nCltuSize, uint32_t nAckCode, uint8_t *pnData);
         void DecapsulateTCACK(uint8_t *pnPacket, uint32_t& nAckCode, uint8_t *pnData);
   	}

      void EncapsulateTrack(uint8_t *pnOut, uint32_t nPayloadSize, uint8_t *pnPayload);
      void DecapsulateTrack(uint8_t *pnPacket, uint32_t& nMsglen, uint64_t& nTimeTag, uint32_t& nMsgType, uint8_t *pnPayload);
      void EncapsulateAntenna(uint8_t *pnOut, float fSatID, float fAzimuth, float fElevation);
      void DecapsulateAntenna(uint8_t *pnPacket, uint32_t& nMsglen, uint64_t& nTimeTag, uint32_t& nMsgType, float& fSatID, float& fAzimuth, float& fElevation);
      void EncapsulateDoppler(uint8_t *pnOut, float fSatID, float fRxFreq, float fRxDoppler, float fTxFreq, float fTxDoppler);
      void DecapsulateDoppler(uint8_t *pnPacket, uint32_t& nMsglen, uint64_t& nTimeTag, uint32_t& nMsgType, float& fSatID,
      float& fRxFreq, float& fRxDoppler, float& fTxFreq, float& fTxDoppler);
      void EncapsulateDecoder(uint8_t *pnOut, uint32_t nPayloadLen, uint8_t *pnPayload);
      void DecapsulateDecoder(uint8_t *pnPacket, uint32_t& nMsglen, uint32_t& nPayloadLen, uint8_t *pnPayload);
      void EncapsulateStatus(uint8_t *pnOut, uint32_t nTabType);
      void DecapsulateStatus(uint8_t *pnPacket, uint32_t& nMsglen, uint32_t& nMsgType, uint32_t& nTabType);

      namespace parse
      {
         std::string ParseMsgType(uint32_t& nMsgType);
         std::string ParseScrambler(uint32_t& nScrambler);
         std::string ParseFEC(uint32_t& nFEC);
         std::string ParseFrameFormat(uint32_t& nFrameFormat);
      }
  }

// global variable declaration 
extern UL_TABLE sUlTable;



#endif /* TABLES_H */
