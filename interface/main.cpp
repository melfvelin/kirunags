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

	nInData[0] = 'A';
	std::cout << "Sending TM: " << nInData[0] << std::endl;
	ServFuncs::EncapsulateTM(pnPacket, 0, 0, 3, nDataSize, nInData);
	ClientFuncs::DecapsulateTM(pnPacket, nMsglen, nTimeTag, nMsgType, nScrambler, nFEC, nFrameFormat, nCaduSize, nOutData);
	std::cout << "Decapsulated TM: " << nOutData[0] << std::endl;
	std::cout << "Message type: " << nMsgType << std::endl;

	nInData[0] = 'C';
	std::cout << "Sending TC: " << nInData[0] << std::endl;
	ClientFuncs::EncapsulateTC(pnPacket, nDataSize, nInData);
	ServFuncs::DecapsulateTC(pnPacket, nMsglen, nMsgType, nCaduSize, nOutData);
	std::cout << "Decapsulated TC: " << nOutData[0] << std::endl;
	std::cout << "Message type: " << nMsgType << std::endl;

	nInData[0] = 'T';
	std::cout << "Sending tracking frame containing: " << nInData[0] << std::endl;
	ClientFuncs::EncapsulateTrack(pnPacket, sizeof(uint8_t), nInData);
	ServFuncs::DecapsulateTrack(pnPacket, nMsglen, nTimeTag, nMsgType, nOutData);
	std::cout << "Decapsulated tracking frame: " << nOutData[0] << std::endl;
	std::cout << "Message type: " << nMsgType << std::endl;

	ClientFuncs::EncapsulateAntenna(pnPacket, 1001, 27.7, 120.1);
	ServFuncs::DecapsulateAntenna(pnPacket, nMsglen, nTimeTag, nMsgType, satid, az, el);

	std::cout << "Azimuth: " << std::fixed << std::setprecision(2) << az;
	std::cout << "Elevation: " << std::fixed << std::setprecision(2) << el << std::endl;
	std::cout << "Message type: " << nMsgType << std::endl;

	ServFuncs::EncapsulateDoppler(pnPacket, 1000, 5000, 10, 5100, 10);
	ServFuncs::DecapsulateDoppler(pnPacket, nMsglen, nTimeTag, nMsgType, satid, rxfreq, rxdopp, txfreq, txdopp);

	std::cout << "Rx Freq: " << std::fixed << std::setprecision(2) << rxfreq;
	std::cout << "Tx dopp: " << std::fixed << std::setprecision(2) << txdopp << std::endl;
	std::cout << "Message type: " << nMsgType << std::endl;	

	nInData[0] = 'D';
	ServFuncs::EncapsulateDecoder(pnPacket, 1, nInData);
	ClientFuncs::DecapsulateDecoder(pnPacket, nMsglen, nCaduSize, nOutData);
	std::cout << "Decapsulated decoder frame: " << nOutData[0] << std::endl;
	std::cout << "Decapsulated decoder CADU: " << nCaduSize << std::endl;
	// std::cout << "String returns: " << tmtc::parse::func(5) << std::endl;
	free(pnPacket);

	std::cout << "Parsed string: " << tmtc::parse::ParseMsgType(nMsgType) << std::endl;
	std::cout << "Parsed string: " << tmtc::parse::ParseScrambler(nScrambler) << std::endl;
	std::cout << "Parsed string: " << tmtc::parse::ParseFEC(nFEC) << std::endl;
	std::cout << "Parsed string: " << tmtc::parse::ParseFrameFormat(nFrameFormat) << std::endl;


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

    std::cout << "Uplink table set"  << std::endl;

    /* tmtc::telecommand::EncapsulateTCACK(pnPacket, 1, 0, nInData);
    std::cout << "TC ACK Encapsulated"  << std::endl;
    tmtc::telecommand::DecapsulateTCACK(pnPacket, m_nAckCode, nOutData);
	
    std::cout << "Ack code: " << m_nAckCode << std::endl;
	*/

	server::SetupMulti();

	return 0;
}