/*  ClientFuncs.h
*
*
*/

#ifndef CLIENTFUNCS_H
#define CLIENTFUNCS_H

#include <iostream>
#include <stdint.h>
#include <cstring>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include "tables.h"
#include "tmtc.h"

namespace ClientFuncs
{

   	
   void DecapsulateTM(uint8_t *pnPacket, uint32_t& nMsglen, uint64_t& nTimeTag, uint32_t& nMsgType,
      uint32_t& nScrambler, uint32_t& nFEC, uint32_t& nFrameFormat, uint32_t& nCaduSize, uint8_t *pnData);
   void EncapsulateTC(uint8_t *pnOut, uint32_t nCltuSize, uint8_t *pnData);
   void DecapsulateTCACK(uint8_t *pnPacket, uint32_t& nAckCode, uint8_t *pnData);
   void EncapsulateTrack(uint8_t *pnOut, uint32_t nPayloadSize, uint8_t *pnPayload);
   void EncapsulateAntenna(uint8_t *pnOut, float fSatID, float fAzimuth, float fElevation);
   void EncapsulateDoppler(uint8_t *pnOut, float fSatID, float fRxFreq, float fRxDoppler, float fTxFreq, float fTxDoppler);
   void DecapsulateDecoder(uint8_t *pnPacket, uint32_t& nMsglen, uint32_t& nPayloadLen, uint8_t *pnPayload);
   void EncapsulateStatus(uint8_t *pnOut, uint32_t nTabType);
} /* ClientFuncs */

// global variable declaration 
extern UL_TABLE sUlTable;
extern DL_TABLE sDlTable;


#endif /* TMTC_H */
