/* This is the main file that calls all other files and functions
*
*/
#include <bitset>
#include <iostream>
#include <iomanip>

#include "server.h"
#include "utils.h"
#include "tables.h"

// global variable declaration
UL_TABLE sUlTable;
DL_TABLE sDlTable;


/* main() - assigns values to global tables and starts the TCP server
* 	inputs: none
*	outputs: none
*	function calls: server::SetupServer()
*	author: Martin Elfvelin
*/
int main()
{
	// assigning values to global configs (uplink and downlink tables)
	
	sDlTable.nPreamble = PREAMBLE;
    sDlTable.nMsglen = sizeof(DL_TABLE);
    sDlTable.nMsgType = 10;
    sDlTable.nTabType = 0;
    sDlTable.nScrambler = 1;
    sDlTable.nFEC = 1;
    sDlTable.nFrameFormat = 1;
    sDlTable.nLineCode = 1;
    sDlTable.nModScheme = 1;
    sDlTable.fBitRate = 9600;
    sDlTable.nPostamble = POSTAMBLE;

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

    std::cout << "Uplink and downlink tables set" << std::endl;
    std::cout << "sizeof(UL_TABLE) = " << sizeof(DL_TABLE) << std::endl;

    uint8_t *syncPointer = (uint8_t *)malloc(sizeof(DL_TABLE));
    memcpy(syncPointer, &sDlTable, sizeof(DL_TABLE));
    // server::FindAsmOne(syncPointer);
    free(syncPointer);
    if(syncPointer == nullptr)
    {
		std::cout << "Pointer is free" << std::endl;    	
    }
    
    TEST_HEADER m_sTestHeader;
    m_sTestHeader.nTrash = 0xD5A1134E;
    m_sTestHeader.nPreamble = PREAMBLE;
    m_sTestHeader.nMsglen = 5 * sizeof(uint32_t);
    m_sTestHeader.nMsgType = 12;
    m_sTestHeader.nTabType = 0;
    m_sTestHeader.nPostamble = POSTAMBLE;
    std::cout << "TEST_HEADER Msglen = " << m_sTestHeader.nMsglen << std::endl;
    std::cout << "TEST_HEADER MsgType = " << m_sTestHeader.nMsgType << std::endl;
    syncPointer = (uint8_t *)malloc(sizeof(TEST_HEADER));
    memcpy(syncPointer, &m_sTestHeader, sizeof(TEST_HEADER));
    // server::FindAsmOne(syncPointer);
    // server::FindAsmTwo(syncPointer, 20);
    // server::FindAsmThree(syncPointer);
    server::SetupServer();

	// std::cout << "Parsed string: " << utils::parse::ParseMsgType(nMsgType) << std::endl;
	// std::cout << "Parsed string: " << utils::parse::ParseScrambler(nScrambler) << std::endl;
	// std::cout << "Parsed string: " << utils::parse::ParseFEC(nFEC) << std::endl;
	// std::cout << "Parsed string: " << utils::parse::ParseFrameFormat(nFrameFormat) << std::endl;	

	return 0;
}