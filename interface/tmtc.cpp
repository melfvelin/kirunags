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

	int GetMsgType(uint8_t *pnData, uint32_t nValread)
	{
		GEN_HEADER m_sGenHeader;

		memcpy(&m_sGenHeader, pnData, sizeof(GEN_HEADER));

		if(m_sGenHeader.nMsgType >= 0 && m_sGenHeader.nMsgType <= 11)
		{
			return m_sGenHeader.nMsgType;
		}
		else
		{
			return -1;
		}
		return -1;
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














