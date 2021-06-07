/* This is the main file that calls all other files and functions
*
*/
#include <bitset>
#include <iostream>
#include "server.h"
#include "tmtc.h"
#include "tables.h"
#include "ClientFuncs.h"
#include <iomanip>

// global variable declaration
UL_TABLE sUlTable;
DL_TABLE sDlTable;

int main()
{
	//tmtc::encapsulate("hello", sizeof("hello"));
	uint8_t nDataSize = 8;
	uint8_t *pnPacket = (uint8_t *)malloc(1024); // one KibiByte
	uint8_t nInData[nDataSize];
	uint8_t nOutData[nDataSize];

	static uint32_t nMsglen;
	static uint64_t nTimeTag;
	static uint32_t nMsgType;
	static uint32_t nScrambler;
	static uint32_t nFEC;
	static uint32_t nFrameFormat;
	static uint32_t nCaduSize;
	static uint32_t m_nAckCode;
	static float satid;
	static float az = 0.0;
	static float el = 0.0;
	static float rxfreq = 0;
	static float rxdopp = 0;
	static float txfreq = 0;
	static float txdopp = 0;

	sUlTable.nPreamble = PREAMBLE;
    sUlTable.nMsglen = sizeof(UL_TABLE);
    sUlTable.nMsgType = 10;
    sUlTable.nTabType = 1;
    sUlTable.nScrambler = 1;
    sUlTable.nFEC = 1;
    sUlTable.nFrameFormat = 1;
    sUlTable.nLineCode = 1;
    sUlTable.nModScheme = 1;
    sUlTable.fBitRate = 9600;
    sUlTable.nPlopVersion = 1;
    sUlTable.nPostamble = POSTAMBLE;

    std::cout << "Uplink table set modeschem: " << sUlTable.nModScheme << std::endl;

	server::SetupTest();

	// std::cout << "Parsed string: " << tmtc::parse::ParseMsgType(nMsgType) << std::endl;
	// std::cout << "Parsed string: " << tmtc::parse::ParseScrambler(nScrambler) << std::endl;
	// std::cout << "Parsed string: " << tmtc::parse::ParseFEC(nFEC) << std::endl;
	// std::cout << "Parsed string: " << tmtc::parse::ParseFrameFormat(nFrameFormat) << std::endl;


	

    /* tmtc::telecommand::EncapsulateTCACK(pnPacket, 1, 0, nInData);
    std::cout << "TC ACK Encapsulated"  << std::endl;
    tmtc::telecommand::DecapsulateTCACK(pnPacket, m_nAckCode, nOutData);
	
    std::cout << "Ack code: " << m_nAckCode << std::endl;
	*/

	

	return 0;
}