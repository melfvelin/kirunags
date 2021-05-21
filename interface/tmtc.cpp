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
#include <stringstream>


namespace tmtc
{
	namespace telemetry
	{
		void EncapsulateTM(uint8_t *pnOut, uint32_t nMsgType, uint32_t nScrambler, uint32_t nFEC, 
			uint32_t nFrameFormat, uint32_t nCaduSize, uint8_t *pnData)
		{
			// Local variables: Preamble, Postamble, TimeTag add m_ to all locals
			TM_HEADER sTMheader;

			sTMheader.nPreamble = PREAMBLE;		// PREAMBLE defined in tables.h
			sTMheader.nMsgType = nMsgType;
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

			nMsglen = m_sTMheader.nMsglen;
			nTimeTag = m_sTMheader.nTimeTag;
			nMsgType = m_sTMheader.nMsgType;
			nScrambler = m_sTMheader.nScrambler;
			nFEC = m_sTMheader.nFEC;
			nFrameFormat = m_sTMheader.nFrameFormat;
			nCaduSize = m_sTMheader.nCaduSize;
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
		void EncapsulateTC(uint8_t *pnOut, uint32_t nMsgType, uint32_t nCltuSize, uint8_t *pnData)
		{
			// Local variables: Preamble, Postamble, TimeTag add m_ to all locals
			TC_HEADER sTCheader;

			sTCheader.nPreamble = PREAMBLE;			// PREAMBLE defined in tables.h
			sTCheader.nMsgType = nMsgType;
			sTCheader.nCltuSize = nCltuSize;
			sTCheader.nMsglen = sizeof(TC_HEADER) + nCltuSize + sizeof(uint32_t);
			sTCheader.nTimeTag = tmtc::telemetry::GenIRIGBTag();

			// copies tc header to output ref/ptr
			memcpy(pnOut, &sTCheader, sizeof(TC_HEADER));
			// copies data from input reference to output reference
			memcpy((pnOut + sizeof(TC_HEADER)), pnData, nCltuSize);
			// copies postable from local variable to output reference
			memcpy(pnOut + sizeof(TC_HEADER) + nCltuSize, &sTCheader.nPreamble, sizeof(uint32_t));
			return;
		}

		void DecapsulateTC(uint8_t *pnPacket, uint32_t& nMsglen, uint64_t& nTimeTag, uint32_t& nMsgType,
			uint32_t& nCltuSize, uint8_t *pnData)
		{
			TC_HEADER m_sTCheader;
			// Copy TM header to local struct
			memcpy(&m_sTCheader, pnPacket, sizeof(TC_HEADER));

			nMsglen = m_sTCheader.nMsglen;
			nTimeTag = m_sTCheader.nTimeTag;
			nMsgType = m_sTCheader.nMsgType;
			nCltuSize = m_sTCheader.nCltuSize;
			// copy data to local address pointed to by pnData using CADU Size
			memcpy(pnData, (pnPacket + sizeof(TC_HEADER)) , m_sTCheader.nCltuSize);
			return;
		}
		
	} /* telecommand */

	static std::string toBinaryString(const uint8_t& x)
	{
	    std::stringstream ss;
	    ss << std::bitset<sizeof(uint8_t) * 8>(x);
	    return ss.str();
	}
}	/* tmtc */














