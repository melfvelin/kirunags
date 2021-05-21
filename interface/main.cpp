/* This is the main file that calls all other files and functions
*
*/
#include <bitset>
#include <iostream>
#include "server.h"
#include "tmtc.h"
#include "tables.h"


int main()
{
	//tmtc::encapsulate("hello", sizeof("hello"));
	uint8_t nDataSize = 8;
	uint8_t *pnPacket = (uint8_t *)malloc(1000);
	uint8_t nInData[nDataSize];
	uint8_t nOutData[nDataSize];
	nInData[0] = 'A';
	std::cout << "Sending TM: " << nInData[0] << std::endl;
	tmtc::telemetry::EncapsulateTM(pnPacket, 0, 0, 0, 3, nDataSize, nInData);

	static uint32_t nMsglen;
	static uint64_t nTimeTag;
	static uint32_t nMsgType;
	static uint32_t nScrambler;
	static uint32_t nFEC;
	static uint32_t nFrameFormat;
	static uint32_t nCaduSize;

	tmtc::telemetry::DecapsulateTM(pnPacket, nMsglen, nTimeTag, nMsgType, nScrambler, nFEC, nFrameFormat, nCaduSize, nOutData);
	std::cout << "Decapsulated TM: " << nInData[0] << std::endl;

	nInData[0] = 'C';
	std::cout << "Sending TC: " << nInData[0] << std::endl;
	tmtc::telecommand::EncapsulateTC(pnPacket, 1, nDataSize, nInData);
	tmtc::telecommand::DecapsulateTC(pnPacket, nMsglen, nTimeTag, nMsgType, nCaduSize, nInData);
	std::cout << "Decapsulated TC: " << nInData[0] << std::endl;
	return 0;
}