/*  ServerFuncs.h
*
*  author: Martin Elfvelin
*/

#ifndef SERVERFUNCS_H
#define SERVERFUNCS_H

#include <iostream>
#include <stdint.h>
#include <cstring>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include "tables.h"
#include "utils.h"

namespace ServerFuncs
{

   	
   void EncapsulateTM(uint8_t *rnOut, uint32_t nScrambler, uint32_t nFEC, uint32_t nFrameFormat, uint32_t nCaduSize, uint8_t *rnData); 
   void DecapsulateTC(uint8_t *pnPacket, uint32_t& nMsglen, uint32_t& nMsgType, uint32_t& nCltuSize, uint8_t *pnData);
   void EncapsulateTCACK(uint8_t *pnOut, uint32_t nCltuSize, uint32_t nAckCode, uint8_t *pnData);
   void DecapsulateTrack(uint8_t *pnPacket, uint32_t& nMsglen, uint64_t& nTimeTag, uint32_t& nMsgType, uint8_t *pnPayload);
   void DecapsulateAntenna(uint8_t *pnPacket, uint32_t& nMsglen, uint64_t& nTimeTag, uint32_t& nMsgType, float& fSatID, float& fAzimuth, float& fElevation);
   void EncapsulateDoppler(uint8_t *pnOut, float fSatID, float fRxFreq, float fRxDoppler, float fTxFreq, float fTxDoppler);
   void DecapsulateDoppler(uint8_t *pnPacket, uint32_t& nMsglen, uint64_t& nTimeTag, uint32_t& nMsgType, float& fSatID, float& fRxFreq, float& fRxDoppler, 
      float& fTxFreq, float& fTxDoppler);
   void EncapsulateDecoder(uint8_t *pnOut, uint32_t nPayloadLen, uint8_t *pnPayload);
   void DecapsulateStatus(uint8_t *pnPacket, uint32_t& nMsglen, uint32_t& nMsgType, uint32_t& nTabType);
   void DecapsulateSession(uint8_t *pnPacket, uint32_t& nMsglen, uint32_t& nMsgType, uint32_t& nTabType);
   int MsgHandler(int nSockDesc, uint8_t *pnData, uint32_t& nValread);
   void SetConfTable(uint8_t *pnData, uint32_t nMsglen, uint32_t nTabType);

} /* ServerFuncs */

// global variable declaration 
extern UL_TABLE sUlTable;
extern DL_TABLE sDlTable;


#endif /* ServerFuncs_H */