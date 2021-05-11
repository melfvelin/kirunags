/*  tmtc.cpp
*   Author: Martin Elfvelin
*   Contains encapsulate() and decapsulate()
*
*/

#include "tmtc.h"


namespace tmtc
{

	int encapsulate(const char *data, uint32_t data_size)
	{
		// First we set the TM Header parameters to correct values
		// preamble and postable are defined in tables.h
		tm_header.preamble = PREAMBLE;
		postamble = POSTAMBLE;
		// total TCP packet length is header + data + postamble
		uint32_t total_len = (sizeof(tm_header) + sizeof(data) + sizeof(postamble));
		tm_header.total_len = total_len;
	 	tm_header.time_tag = GenIRIGBTag();
	 	// boolean value for scrambler (on/off)
		tm_header.scrambler = 0;
		// boolean value for basis conventional/dual-basis RS
		tm_header.basis = 0;
		// 0 - 8 for different FEC modes
		tm_header.fec = 1;
		tm_header.tf_size = data_size;

		// this pointer will point to the location of the start of the frame in memory
		// now we can use memcpy to copy binary data from the tm_header to *frame_ptr
		uint8_t *frame_ptr = (uint8_t*)malloc(total_len * sizeof(uint8_t));

		// This kind of if-statement could be used for all memcpy
		if(memcpy(frame_ptr, &tm_header, sizeof(tm_header)) == NULL)
		{
			std::cout << "memcpy returned empty address!" << std::endl;
		}

		// next step is to copy the data into the memory following the header
		memcpy(frame_ptr + sizeof(tm_header), &data, sizeof(data));
		
		// now we add the postamble after the data
		memcpy(frame_ptr + sizeof(tm_header) + sizeof(data), &postamble, sizeof(postamble));

		tmtc::parse_tm_header(frame_ptr);
		free(frame_ptr);
		return 0;
	}

	int parse_tm_header(uint8_t *frame_ptr)
	{
		// tm_frame = (TM_FRAME)frame_ptr;
		memcpy(&tm_frame, frame_ptr, sizeof(TM_HEADER));
		std::cout << "Preamble: " << std::hex << (tm_frame.preamble & 0xFFFFFFFF) << std::endl;
		std::cout << "Total length: " << std::dec << (tm_frame.total_len & 0xFFF) << std::endl;
		std::cout << "Time tag: " << std::dec << tm_frame.time_tag << std::endl;
		std::cout << "Scrambler: " << std::dec << (tm_frame.scrambler & 0x1) << std::endl;
		std::cout << "Basis: " << std::dec << (tm_frame.basis & 0x1) << std::endl;
		std::cout << "FEC: " << std::dec << (tm_frame.fec & 0xF) << std::endl;
		std::cout << "Payload Length: " << std::dec << tm_frame.tf_size << std::endl;

		memcpy(&tm_frame.data, (frame_ptr + 32), tm_frame.tf_size);
		std::cout << "Data is: " << tm_frame.data << std::endl;
		memcpy(&tm_frame.postamble, (frame_ptr +32 + tm_frame.tf_size), sizeof(uint32_t));

		// bit shifting is less good than memcpy, probably
		//tm_frame.total_len = (frame_ptr[4] << 24) | (frame_ptr[5] << 16) | (frame_ptr[6] << 8) | (frame_ptr[7]);
		//tm_frame.tf_size =  (frame_ptr[28] << 24) | (frame_ptr[29] << 16) | (frame_ptr[30] << 8) | (frame_ptr[31]);
		//uint8_t data = (frame_ptr[32]);

		return 0;
	}
	
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
        std::cout << "gmtm->tm_yday = " << gmtm->tm_yday << std::endl;
        std::cout << "gmtm->tm_yday%100 = " << (gmtm->tm_yday%100) << std::endl;
        std::cout << "gmtm->tm_yday/100 = " << (gmtm->tm_yday/100) << std::endl;
        std::cout << "gmtm->tm_min = " << (gmtm->tm_min) << std::endl;
    
       	return IRIGB_Code;

	}

	int DecimalToBCD(int decimal)
	{
		return (((decimal/10) << 4 ) | (decimal % 10));
	}

	int BCDToDecimal(int bcd)
	{
		return (((bcd >> 4) * 10) + (bcd & 0xF));
	}

	/*	decapsulate - decapsulates a TC packet down to payload data
	*	in:
	*	out:
	*	author:
	*/
	int decapsulate(uint8_t *tc_ptr, uint32_t data_size)
	{
		// memcpy(packet, tc_ptr, data_size);
		// save TC header first
		memcpy(&tc_header, tc_ptr, sizeof(TC_HEADER));
		// use tf_size from TC header to malloc memory for payload
		uint8_t *payload = (uint8_t *)malloc(tc_header.tf_size);
		// copy data from TC pointer to payload
		memcpy(payload, (tc_ptr + sizeof(TC_HEADER)), tc_header.tf_size);
		// print payload
		std::cout << "Payload: " << payload << std::endl;

		return 0;
	}

	void sendTC_frame()
	{
		

		TC_HEADER tx_tc_frame;
		tx_tc_frame.preamble = 0xA1B2C3D4;
		const char message[] = "TC TEST MESSAGE";
		uint32_t total_len = (sizeof(TC_HEADER) + sizeof(message));
		tx_tc_frame.total_len = total_len;
		tx_tc_frame.tf_size = sizeof(message);
		// adding message after tc header
		//memcpy((tx_tc_frame + sizeof(TC_HEADER)), &message, tx_tc_frame.tf_size);

		uint8_t *tc_ptr = (uint8_t *)malloc(tx_tc_frame.total_len);
		memcpy(tc_ptr, &tx_tc_frame, sizeof(TC_HEADER));
		memcpy((tc_ptr + sizeof(TC_HEADER)), &message, tx_tc_frame.tf_size);
		if(tmtc::decapsulate(tc_ptr, total_len) == 0)
		{
			std::cout << "Successfully decapsulated tc frame!" << std::endl;
		}

		return;
	}
} /* tmtc */
/* main() - makes function calls to other methods
*	takes data as input from user: word[64] or hard coded data: inData
*	inData is of variable length, word is 64 bytes
*	sends data to encapsulate() 
*/
int main()
	{
    	//const char *inData = (const char *)0b01010101;
    	tmtc::sendTC_frame();
    	char word[64];
    	std::cout << "Please enter data (<64 byte): " << std::endl;
    	std::cin >> word;
    	std::cout << "Input: " << word << std::endl;
		const char inData[] = "123456789190201321238123";
		// std::cout << "Payload length before packing is: " << std::dec << sizeof(inData) << std::endl;
		std::cout << "Payload length before packing is: " << std::dec << sizeof(word) << std::endl;
    	// if(tmtc::encapsulate(inData, sizeof(inData)/sizeof(inData[0]) ) == 0)
    	if(tmtc::encapsulate(word, sizeof(word)/sizeof(word[0]) ) == 0)
    	{
    		std::cout << "encapsulate returned 0\n";
    	}
    
    	return 0;
	}













