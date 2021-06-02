/*  tmtc.cpp
*   Author: Martin Elfvelin
*   Contains encapsulate() and decapsulate()
*
*/

#include "server.h"
#include "tmtc.h"
#include "tables.h"
#include <bitset>
#include <iostream>
#include <iomanip>
#include <string>


namespace tmtc
{
	namespace telemetry
	{
		void EncapsulateTM(uint8_t *pnOut, uint32_t nScrambler, uint32_t nFEC, 
			uint32_t nFrameFormat, uint32_t nCaduSize, uint8_t *pnData)
		{
			// Local variables: Preamble, Postamble, TimeTag add m_ to all locals
			TM_HEADER sTMheader;

			sTMheader.nPreamble = PREAMBLE;		// PREAMBLE defined in tables.h
			sTMheader.nMsgType = 0;
			sTMheader.nScrambler = nScrambler;
			sTMheader.nFEC = nFEC;
			sTMheader.nFrameFormat = nFrameFormat;
			sTMheader.nCaduSize = nCaduSize;
			sTMheader.nMsglen = sizeof(TM_HEADER) + nCaduSize + sizeof(uint32_t);
			sTMheader.nTimeTag = tmtc::telemetry::GenIRIGBTag();

			// copies tm header to output ref/ptr
			memcpy(pnOut, &sTMheader, sizeof(TM_HEADER));
			// copies data from input reference to output reference
			memcpy((pnOut + sizeof(TM_HEADER)), pnData, nCaduSize);
			// copies postable from local variable to output reference
			memcpy(pnOut + sizeof(TM_HEADER) + nCaduSize, &sTMheader.nPreamble, sizeof(uint32_t));
			return;
		}

		void DecapsulateTM(uint8_t *pnPacket, uint32_t& nMsglen, uint64_t& nTimeTag, uint32_t& nMsgType,
			uint32_t& nScrambler, uint32_t& nFEC, uint32_t& nFrameFormat, uint32_t& nCaduSize, uint8_t *pnData)
		{
			TM_HEADER m_sTMheader;
			// Copy TM header to local struct
			memcpy(&m_sTMheader, pnPacket, sizeof(TM_HEADER));

			nMsglen = (m_sTMheader.nMsglen & 0x3FF);
			nTimeTag = (m_sTMheader.nTimeTag);
			nMsgType = (m_sTMheader.nMsgType & 0x3);
			nScrambler = (m_sTMheader.nScrambler & 0x1);
			nFEC = (m_sTMheader.nFEC & 0XF);
			nFrameFormat = (m_sTMheader.nFrameFormat & 0x7);
			nCaduSize = (m_sTMheader.nCaduSize & 0x3FF);
			// copy data to local address pointed to by pnData using CADU Size
			memcpy(pnData, (pnPacket + sizeof(TM_HEADER)) , m_sTMheader.nCaduSize);
			return;
		}
		
		/*	GenIRIGTag() -   generates unsigned 64-bit IRIG-B time tag
		*	inputs: none
		*	outputs: uint64_t time tag
		*	returns IRIGB_Code
		*	author: Moses Browne Mwakyanjala
		*/
		uint64_t GenIRIGBTag()
		{
	    	timespec spec;
	    	clock_gettime(CLOCK_REALTIME,&spec);
	    	tm* gmtm = gmtime(&spec.tv_sec);
	    	uint8_t IRIGB[8];
	    	IRIGB[0] = 0xFF;
	    	IRIGB[1] = (uint8_t)DecimalToBCD(gmtm->tm_yday%100);
	    	IRIGB[2] = (uint8_t)DecimalToBCD(gmtm->tm_yday/100);
	    	IRIGB[3] = (uint8_t)DecimalToBCD(gmtm->tm_hour);
	    	IRIGB[4] = (uint8_t)DecimalToBCD(gmtm->tm_min);
	    	IRIGB[5] = (uint8_t)DecimalToBCD(gmtm->tm_sec);
	    	long ms  = spec.tv_nsec/(1E6);
	    	IRIGB[6] = (uint8_t)DecimalToBCD(ms/10);
	    	IRIGB[7] = (uint8_t)DecimalToBCD(ms%10);
	    	uint64_t IRIGB_Code;
	    	memcpy(&IRIGB_Code,&IRIGB[0],8);

			//	if (VERBOSE)

	        /* std::cout << "gmtm->tm_yday = " << gmtm->tm_yday << std::endl;
	        std::cout << "gmtm->tm_yday%100 = " << (gmtm->tm_yday%100) << std::endl;
	        std::cout << "gmtm->tm_yday/100 = " << (gmtm->tm_yday/100) << std::endl;
	        std::cout << "gmtm->tm_min = " << (gmtm->tm_min) << std::endl; */

	       	return IRIGB_Code;
		}	

		/*	DecimalToBCD() - Converts decimal to Binary Coded Decimal
		*	inputs: int decimal
		*	outputs: Binary Coded Decimal (int)
		*	author: Moses Browne Mwakyanjala
		*/
		int DecimalToBCD(int decimal)
		{
			return (((decimal/10) << 4 ) | (decimal % 10));
		}

		/*	BCDToDecimal() - Converts Binary Coded Decimal to decimal
		*	inputs: Binary Coded Decimal (int)
		*	outputs: decimal (int)
		*	author: Moses Browne Mwakyanjala
		*/
		int BCDToDecimal(int bcd)
		{
			return (((bcd >> 4) * 10) + (bcd & 0xF));
		}
	} /* telemetry */
	
	namespace telecommand
	{	
		void EncapsulateTC(uint8_t *pnOut, uint32_t nCltuSize, uint8_t *pnData)
		{
			// Local variables: Preamble, Postamble, TimeTag add m_ to all locals
			TC_HEADER sTCheader;

			sTCheader.nPreamble = PREAMBLE;			// PREAMBLE defined in tables.h
			sTCheader.nMsgType = 1;
			sTCheader.nCltuSize = nCltuSize;
			sTCheader.nMsglen = sizeof(TC_HEADER) + nCltuSize + sizeof(uint32_t);

			// copies tc header to output ref/ptr
			memcpy(pnOut, &sTCheader, sizeof(TC_HEADER));
			// copies data from input reference to output reference
			memcpy((pnOut + sizeof(TC_HEADER)), pnData, nCltuSize);
			// copies postable from local variable to output reference
			memcpy(pnOut + sizeof(TC_HEADER) + nCltuSize, &sTCheader.nPreamble, sizeof(uint32_t));
			return;
		}

		void DecapsulateTC(uint8_t *pnPacket, uint32_t& nMsglen, uint32_t& nMsgType,
			uint32_t& nCltuSize, uint8_t *pnData)
		{
			TC_HEADER m_sTCheader;
			// Copy TM header to local struct
			memcpy(&m_sTCheader, pnPacket, sizeof(TC_HEADER));

			nMsglen = (m_sTCheader.nMsglen & 0x3FF);
			nMsgType = (m_sTCheader.nMsgType & 0x15);
			nCltuSize = (m_sTCheader.nCltuSize & 0x3FF);
			// copy data to local address pointed to by pnData using CADU Size
			memcpy(pnData, (pnPacket + sizeof(TC_HEADER)) , m_sTCheader.nCltuSize);
			return;
		}
		// needs to be remade
		void EncapsulateTCACK(uint8_t *pnOut, uint32_t nCltuSize, uint32_t nAckCode, uint8_t *pnData)
		{
			TCACK_HEADER m_sTcAckHeader;

			m_sTcAckHeader.nPreamble = PREAMBLE;		// PREAMBLE defined in tables.h
			m_sTcAckHeader.nMsglen = sizeof(TCACK_HEADER) + nCltuSize;
			m_sTcAckHeader.nTimeTag = tmtc::telemetry::GenIRIGBTag();
			m_sTcAckHeader.nMsgType = 2;
			m_sTcAckHeader.nScrambler = sUlTable.nScrambler;
			m_sTcAckHeader.nFEC = sUlTable.nFEC;
			m_sTcAckHeader.nFrameFormat = sUlTable.nFrameFormat;
			m_sTcAckHeader.nLineCode = sUlTable.nLineCode;
			m_sTcAckHeader.nModScheme = sUlTable.nModScheme;
			m_sTcAckHeader.fBitRate = sUlTable.fBitRate;
			m_sTcAckHeader.nPlopVersion = sUlTable.nPlopVersion;
			m_sTcAckHeader.nAckCode = nAckCode;
			m_sTcAckHeader.nCltuSize = nCltuSize;
			uint32_t m_nPostamble = POSTAMBLE;

			memcpy(pnOut, &m_sTcAckHeader, sizeof(TCACK_HEADER));
			memcpy((pnOut + sizeof(TCACK_HEADER)), pnData, nCltuSize);
			memcpy((pnOut + sizeof(TCACK_HEADER) + nCltuSize), &m_nPostamble, sizeof(m_nPostamble));

			// memcpy(&m_sTcAckHeader, pnPacket, sizeof(TC_HEADER));
			// memcpy((&m_sTcAckHeader + sizeof(TC_HEADER)), (pnPacket + sizeof(TC_HEADER)), m_sTcAckHeader.nCltuSize);

			
			
			
			
			// // copies tm header to output ref/ptr
			// memcpy(pnOut, &m_sTcAckHeader, sizeof(TC_HEADER));
			// // copies data from input reference to output reference
			// memcpy((pnOut + sizeof(TC_HEADER)), (pnPacket + sizeof(TC_HEADER)), m_sTcAckHeader.nCltuSize);
			// memcpy((pnOut + sizeof(TC_HEADER) + m_sTcAckHeader.nCltuSize), &m_nAckCode, sizeof(m_nAckCode));
			// // copies postable from local variable to output reference
			// memcpy((pnOut + sizeof(TC_HEADER) + m_sTcAckHeader.nCltuSize + sizeof(m_nAckCode)), &m_nPostamble, sizeof(uint32_t));
			return;
		}

		void DecapsulateTCACK(uint8_t *pnPacket, uint32_t& nAckCode, uint8_t *pnData)
		{	
			TCACK_HEADER m_sTcAckHeader;

			memcpy(&m_sTcAckHeader, pnPacket, sizeof(TCACK_HEADER));
			memcpy(pnData, pnPacket, m_sTcAckHeader.nCltuSize);
			nAckCode = m_sTcAckHeader.nAckCode;

			return;
		}
	} /* telecommand */

	void EncapsulateTrack(uint8_t *pnOut, uint32_t nPayloadSize, uint8_t *pnPayload)
	{
		TR_HEADER m_sTRheader;
		m_sTRheader.nPreamble = PREAMBLE;
		m_sTRheader.nMsglen = sizeof(TR_HEADER) + nPayloadSize + sizeof(uint32_t);
		m_sTRheader.nTimeTag = tmtc::telemetry::GenIRIGBTag();
		m_sTRheader.nMsgType = 3;
		uint32_t m_nPostamble = POSTAMBLE;


		memcpy(pnOut, &m_sTRheader, sizeof(TR_HEADER));
		memcpy((pnOut + sizeof(TR_HEADER)), pnPayload, nPayloadSize);
		memcpy((pnOut + sizeof(TR_HEADER) + nPayloadSize), &m_nPostamble, sizeof(uint32_t)); 
		return;
	}

	void DecapsulateTrack(uint8_t *pnPacket, uint32_t& nMsglen, uint64_t& nTimeTag, uint32_t& nMsgType, uint8_t *pnPayload)
	{
		TR_HEADER m_sTRheader;
		// copy data from pointer to local struct
		memcpy(&m_sTRheader, pnPacket, sizeof(TR_HEADER));

		// transfer parameters from local struct to references using bit masking
		nMsglen = (m_sTRheader.nMsglen & 0x3FF);
		nTimeTag = m_sTRheader.nTimeTag;
		nMsgType = (m_sTRheader.nMsgType & 0x15);

		// copy payload data from packet pointer to payload pointer
		memcpy(pnPayload, (pnPacket + sizeof(TR_HEADER)), (nMsglen - sizeof(TR_HEADER) - sizeof(uint32_t)));
		return;
	}


	void EncapsulateAntenna(uint8_t *pnOut, float fSatID, float fAzimuth, float fElevation)
	{
		ANT_HEADER m_sANTheader;
		
		m_sANTheader.nPreamble = PREAMBLE;
		m_sANTheader.nMsglen = sizeof(ANT_HEADER) + sizeof(uint32_t);
		// debug
		std::cout << "Message lenght: " << m_sANTheader.nMsglen << std::endl;
		m_sANTheader.nTimeTag = tmtc::telemetry::GenIRIGBTag();
		m_sANTheader.nMsgType = 4;
		m_sANTheader.fSatID = fSatID;
		m_sANTheader.fAzimuth = fAzimuth;
		m_sANTheader.fElevation = fElevation;

		uint32_t m_nPostamble = POSTAMBLE;

		memcpy(pnOut, &m_sANTheader, sizeof(ANT_HEADER));
		memcpy((pnOut + sizeof(ANT_HEADER)), &m_nPostamble, sizeof(m_nPostamble));

		return;
	}

	void DecapsulateAntenna(uint8_t *pnPacket, uint32_t& nMsglen, uint64_t& nTimeTag, uint32_t& nMsgType, float& fSatID, float& fAzimuth, float& fElevation)
	{
		ANT_HEADER m_sANTheader;

		memcpy(&m_sANTheader, pnPacket, sizeof(m_sANTheader));

		nMsglen = (m_sANTheader.nMsglen & 0x3FF);
		nTimeTag = m_sANTheader.nTimeTag;
		nMsgType = (m_sANTheader.nMsgType & 0x15);
		fSatID = m_sANTheader.fSatID;
		fAzimuth = m_sANTheader.fAzimuth;
		fElevation = m_sANTheader.fElevation;

		return;
	}

	void EncapsulateDoppler(uint8_t *pnOut, float fSatID, float fRxFreq, float fRxDoppler, float fTxFreq, float fTxDoppler)
	{
		DOPP_HEADER m_sDOPPheader;
		m_sDOPPheader.nPreamble = PREAMBLE;
		m_sDOPPheader.nMsglen = sizeof(m_sDOPPheader) + sizeof(uint32_t);		
		m_sDOPPheader.nTimeTag = tmtc::telemetry::GenIRIGBTag();
		m_sDOPPheader.nMsgType = 5;
		m_sDOPPheader.fSatID = fSatID;
		m_sDOPPheader.fRxFreq = fRxFreq;
		m_sDOPPheader.fRxDoppler = fRxDoppler;
		m_sDOPPheader.fTxFreq = fTxFreq;
		m_sDOPPheader.fTxDoppler = fTxDoppler;

		uint32_t m_nPostamble = POSTAMBLE;

		memcpy(pnOut, &m_sDOPPheader, sizeof(DOPP_HEADER));
		memcpy((pnOut + sizeof(DOPP_HEADER)), &m_nPostamble, sizeof(uint32_t));

		return;
	}

	void DecapsulateDoppler(uint8_t *pnPacket, uint32_t& nMsglen, uint64_t& nTimeTag, uint32_t& nMsgType, float& fSatID,
		float& fRxFreq, float& fRxDoppler, float& fTxFreq, float& fTxDoppler)
	{
		DOPP_HEADER m_sDOPPheader;

		memcpy(&m_sDOPPheader, pnPacket, sizeof(DOPP_HEADER));

		nMsglen = (m_sDOPPheader.nMsglen & 0x3FF);
		nTimeTag = m_sDOPPheader.nTimeTag;
		nMsgType = (m_sDOPPheader.nMsgType & 0x15);
		fSatID = m_sDOPPheader.fSatID;
		fRxFreq = m_sDOPPheader.fRxFreq;
		fRxDoppler = m_sDOPPheader.fRxDoppler;
		fTxFreq = m_sDOPPheader.fTxFreq;
		fTxDoppler = m_sDOPPheader.fTxDoppler;

		return;
	}

	void EncapsulateDecoder(uint8_t *pnOut, uint32_t nPayloadLen, uint8_t *pnPayload)
	{
		DEC_HEADER m_sDECheader;

		m_sDECheader.nPreamble = PREAMBLE;
		m_sDECheader.nMsglen = sizeof(DEC_HEADER) + nPayloadLen + sizeof(uint32_t);
		m_sDECheader.nMsgType = 6;
		m_sDECheader.nPayloadLen = nPayloadLen;
		uint32_t m_nPostamble = POSTAMBLE;

		memcpy(pnOut, &m_sDECheader, sizeof(DEC_HEADER));
		memcpy((pnOut + sizeof(DEC_HEADER)), pnPayload, nPayloadLen);
		memcpy((pnOut + sizeof(DEC_HEADER) + nPayloadLen), &m_nPostamble, sizeof(uint32_t));

		return;
	}

	void DecapsulateDecoder(uint8_t *pnPacket, uint32_t& nMsglen, uint32_t& nPayloadLen, uint8_t *pnPayload)
	{
		DEC_HEADER m_sDECheader;

		memcpy(&m_sDECheader, pnPacket, sizeof(DEC_HEADER));

		nMsglen = m_sDECheader.nMsglen;
		nPayloadLen = m_sDECheader.nPayloadLen;
		std::cout << "Payload length from DecapsulateDecoder: " << nPayloadLen << std::endl;
		memcpy(pnPayload, (pnPacket + sizeof(DEC_HEADER)), nPayloadLen);

		return;
	}

	void EncapsulateStatus(uint8_t *pnOut, uint32_t nTabType)
	{
		STATUS_HEADER m_sStatusHeader;

		m_sStatusHeader.nPreamble = PREAMBLE;
		m_sStatusHeader.nMsglen = sizeof(STATUS_HEADER);
		m_sStatusHeader.nMsgType = 7;
		m_sStatusHeader.nTabType = nTabType;
		m_sStatusHeader.nPostamble = POSTAMBLE;

		memcpy(pnOut, &m_sStatusHeader, m_sStatusHeader.nMsglen);

		return;
	}

	void DecapsulateStatus(uint8_t *pnPacket, uint32_t& nMsglen, uint32_t& nMsgType, uint32_t& nTabType)
	{
		STATUS_HEADER m_sStatusHeader;

		memcpy(&m_sStatusHeader, pnPacket, sizeof(STATUS_HEADER));

		nMsglen = (m_sStatusHeader.nMsglen & 0x3FF);
		nMsgType = (m_sStatusHeader.nMsgType & 0x15);
		nTabType = (m_sStatusHeader.nTabType & 0x15);

		return;
	}
	/* EncapsulateSession - takes a pointer to the table the user wants to upload to the Expedite server
	*	and encapsulates it in a Session frame
	* Inputs: Pointer to memory where output will go (pnOut), Table type, Table length and 
	pointer to table to upload */
	void EncapsulateSession(uint8_t *pnOut, uint32_t nTabType, uint32_t nTabLen, uint8_t *pnPayload)
	{
		SESH_HEADER m_sSeshHeader;

		m_sSeshHeader.nPreamble = PREAMBLE;
		m_sSeshHeader.nMsglen = sizeof(SESH_HEADER) + nTabLen + sizeof(uint32_t);
		m_sSeshHeader.nMsgType = 8;
		m_sSeshHeader.nTabType = nTabType;
		m_sSeshHeader.nTabLen = nTabLen;
		uint32_t nPostamble = POSTAMBLE;

		memcpy(pnOut, &m_sSeshHeader, sizeof(SESH_HEADER));
		memcpy((pnOut + sizeof(SESH_HEADER)), pnPayload, nTabLen);
		memcpy((pnOut + sizeof(SESH_HEADER) + nTabLen), &nPostamble, sizeof(nPostamble));

		return;
	}

	void DecapsulateSession()
	{
		
		
		return;
	}

	namespace parse
	{
	
		std::string ParseMsgType(uint32_t& nMsgType)
		{	
			switch(nMsgType)
			{
				case 0:
				return "Telemetry";
				
				case 1:
				return "Telecommand";
				
				case 2:
				return "Not defined";
				
				case 3:
				return "Tracking";
				
				case 4:
				return "Antenna";
				
				case 5:
				return "Doppler";
				
				default:
				break;
			}

			// Should never reach here with correct inputs
			return "Invalid message type";
		}

		std::string ParseScrambler(uint32_t& nScrambler)
		{	
			switch(nScrambler)
			{
				case 0:
				return "CCSDS";
				
				case 1:
				return "G3RUH";
				
				default:
				break;
			}

			// Should never reach here with correct inputs
			return "Invalid message type";
		}

		std::string ParseFEC(uint32_t& nFEC)
		{	
			switch(nFEC)
			{
				case 0:
				return "NONE";
				
				case 1:
				return "VITERBI";

				case 2:
				return "INVERTED VITERBI";
				
				case 3:
				return "RS (255, 223)";
				
				case 4:
				return "RS (255, 239)";
				
				case 5:
				return "VITERBI + RS (255, 223)";
				
				case 6:
				return "VITERBI + RS (255, 239)";
				
				case 7:
				return "INV VITERBI + RS (255, 223)";
					
				case 8:
				return "INV VITERBI + RS (255, 239)";

				default:
				break;
			}

			// Should never reach here with correct inputs
			return "Invalid message type";
		}

		std::string ParseFrameFormat(uint32_t& nFrameFormat)
		{	
			switch(nFrameFormat)
			{
				case 0:
				return "NONE";
				
				case 1:
				return "ASM1";
				
				case 2:
				return "ASM2";
				
				case 3:
				return "ASM3";
				
				case 4:
				return "AX25";
				
				default:
				break;
			}

			// Should never reach here with correct inputs
			return "Invalid message type";
		}

	}	/* parse */
}	/* tmtc */














