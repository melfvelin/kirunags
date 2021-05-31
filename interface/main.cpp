/* This is the main file that calls all other files and functions
*
*/
#include <bitset>
#include <iostream>
#include "server.h"
#include "tmtc.h"
#include "tables.h"
#include <iomanip>


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
	static float satid;
	static float az = 0.0;
	static float el = 0.0;
	static float rxfreq = 0;
	static float rxdopp = 0;
	static float txfreq = 0;
	static float txdopp = 0;

	nInData[0] = 'A';
	std::cout << "Sending TM: " << nInData[0] << std::endl;
	tmtc::telemetry::EncapsulateTM(pnPacket, 0, 0, 3, nDataSize, nInData);
	tmtc::telemetry::DecapsulateTM(pnPacket, nMsglen, nTimeTag, nMsgType, nScrambler, nFEC, nFrameFormat, nCaduSize, nOutData);
	std::cout << "Decapsulated TM: " << nOutData[0] << std::endl;
	std::cout << "Message type: " << nMsgType << std::endl;

	nInData[0] = 'C';
	std::cout << "Sending TC: " << nInData[0] << std::endl;
	tmtc::telecommand::EncapsulateTC(pnPacket, nDataSize, nInData);
	tmtc::telecommand::DecapsulateTC(pnPacket, nMsglen, nTimeTag, nMsgType, nCaduSize, nOutData);
	std::cout << "Decapsulated TC: " << nOutData[0] << std::endl;
	std::cout << "Message type: " << nMsgType << std::endl;

	nInData[0] = 'T';
	std::cout << "Sending tracking frame containing: " << nInData[0] << std::endl;
	tmtc::EncapsulateTrack(pnPacket, sizeof(uint8_t), nInData);
	tmtc::DecapsulateTrack(pnPacket, nMsglen, nTimeTag, nMsgType, nOutData);
	std::cout << "Decapsulated tracking frame: " << nOutData[0] << std::endl;
	std::cout << "Message type: " << nMsgType << std::endl;

	tmtc::EncapsulateAntenna(pnPacket, 1001, 27.7, 120.1);
	tmtc::DecapsulateAntenna(pnPacket, nMsglen, nTimeTag, nMsgType, satid, az, el);

	std::cout << "Azimuth: " << std::fixed << std::setprecision(2) << az;
	std::cout << "Elevation: " << std::fixed << std::setprecision(2) << el << std::endl;
	std::cout << "Message type: " << nMsgType << std::endl;

	tmtc::EncapsulateDoppler(pnPacket, 1000, 5000, 10, 5100, 10);
	tmtc::DecapsulateDoppler(pnPacket, nMsglen, nTimeTag, nMsgType, satid, rxfreq, rxdopp, txfreq, txdopp);

	std::cout << "Rx Freq: " << std::fixed << std::setprecision(2) << rxfreq;
	std::cout << "Tx dopp: " << std::fixed << std::setprecision(2) << txdopp << std::endl;
	std::cout << "Message type: " << nMsgType << std::endl;	

	nInData[0] = 'D';
	tmtc::EncapsulateDecoder(pnPacket, 1, nInData);
	tmtc::DecapsulateDecoder(pnPacket, nMsglen, nCaduSize, nOutData);
	std::cout << "Decapsulated decoder frame: " << nOutData[0] << std::endl;
	std::cout << "Decapsulated decoder CADU: " << nCaduSize << std::endl;
	// std::cout << "String returns: " << tmtc::parse::func(5) << std::endl;
	free(pnPacket);

	std::cout << "Parsed string: " << tmtc::parse::ParseMsgType(nMsgType) << std::endl;
	std::cout << "Parsed string: " << tmtc::parse::ParseScrambler(nScrambler) << std::endl;
	std::cout << "Parsed string: " << tmtc::parse::ParseFEC(nFEC) << std::endl;
	std::cout << "Parsed string: " << tmtc::parse::ParseFrameFormat(nFrameFormat) << std::endl;

	return 0;
}