/*  tmtc.cpp
*   Author: Martin Elfvelin
*   Contains encapsulate() and decapsulate()
*
*/

#include "server.h"
#include "tmtc.h"
#include "tables.h"


namespace tmtc
{
	namespace telemetry
	{
			/*	encapsulate() - creates a TM network packet made up of a TM header, payload data and postamble
		*	in: const char pointer to payload data, 32-bit unsigned data size
		*	out: uintb_t pointer to TM network packet sent to parse function
		*	returns 0 at end of function
		*	function calls: GenIRIGBTag() - generates time tag. parse_tm_header() - prints out packet header and data
		*/	
		uint8_t *encapsulate(const char *message, uint32_t message_len)
		{
			#ifdef debug
			std::cout << "[tmtc.cpp:encapsulate] Payload data before packing: " << message << std::endl;
			std::cout << "[tmtc.cpp:encapsulate] Sizeof(data) = " << sizeof(message) << std::endl;
			std::cout << "[tmtc.cpp:encapsulate] Strlen(data) = " << strlen(message) << std::endl;
			std::cout << "[tmtc.cpp:encapsulate] data_size = " << message_len << std::endl;
			#endif /* debug */

			TM_HEADER tm_header;
			// preamble and postable are defined in tables.h
			tm_header.preamble = PREAMBLE;
			uint32_t postamble = POSTAMBLE;
			uint32_t total_len = (sizeof(tm_header) + message_len + sizeof(postamble));
			tm_header.total_len = total_len;
		 	tm_header.time_tag = GenIRIGBTag();
			tm_header.scrambler = 0;
			tm_header.basis = 0;
			tm_header.fec = 1;
			tm_header.tf_size = message_len;

			// create 8-bit pointer to allocated memory
			uint8_t *frame_ptr = (uint8_t*)malloc(total_len * sizeof(uint8_t));

			// put tm_header in allocated memory 
			if(memcpy(frame_ptr, &tm_header, sizeof(tm_header)) == NULL)
			{
				std::cout << "memcpy returned empty address!" << std::endl;
			}

			// next step is to copy the payload data into the memory following the header
			if(memcpy(frame_ptr + sizeof(tm_header), message, message_len) == NULL)
			{
				std::cout << "memcpy returned empty address!" << std::endl;	
			}

			// now we add the postamble after the data
			if(memcpy(frame_ptr + sizeof(tm_header) + message_len, &postamble, sizeof(postamble)) == NULL)
			{
				std::cout << "memcpy returned empty address!" << std::endl;
			}
		
			return frame_ptr;
		}

		/*	decapsulate() - receives pointer to tm packet and prints out packet preamble, payload data and postamble
		*	input: uint8_t pointer to memory adress of TM packet
		*	output: none
		*	returns 0 at end
		*	function calls: none
		*/
		int decapsulate(uint8_t *frame_ptr)
			{
			// Init local TM_FRAME struct to put packet in
			TM_HEADER tm_frame;
			// copy data from address in frame_ptr to local struct for processing
			if(memcpy(&tm_frame, frame_ptr, sizeof(TM_HEADER)) == NULL)
			{
				std::cout << "memcpy returned empty address!" << std::endl;	
			}
			// use TM_HEADER to print out parameters
			std::cout << "[tmtc.cpp:parse_tm_header] Preamble: " << std::hex << (tm_frame.preamble & 0xFFFFFFFF) << std::endl;
			std::cout << "[tmtc.cpp:parse_tm_header] Total length: " << std::dec << (tm_frame.total_len & 0xFFF) << std::endl;
			std::cout << "[tmtc.cpp:parse_tm_header] Time tag: " << std::dec << tm_frame.time_tag << std::endl;
			std::cout << "[tmtc.cpp:parse_tm_header] Scrambler: " << std::dec << (tm_frame.scrambler & 0x1) << std::endl;
			std::cout << "[tmtc.cpp:parse_tm_header] Basis: " << std::dec << (tm_frame.basis & 0x1) << std::endl;
			std::cout << "[tmtc.cpp:parse_tm_header] FEC: " << std::dec << (tm_frame.fec & 0xF) << std::endl;
			std::cout << "[tmtc.cpp:parse_tm_header] Payload Length: " << std::dec << tm_frame.tf_size << std::endl;
			// copy payload data from adress in frame_ptr to local struct for processing
			uint8_t *payload_ptr = (uint8_t *)malloc(tm_frame.tf_size);
			if(memcpy(payload_ptr, (frame_ptr + sizeof(TM_HEADER)), tm_frame.tf_size) ==NULL)
			{
				std::cout << "[tmtc.cpp:parse_tm_header] memcpy returned empty address!" << std::endl;
			}
			// when sending payload as characters it can be necessary to add a trailing null char
			// payload_ptr[tm_frame.tf_size] = '\0';
			//std::cout << "[tmtc.cpp:parse_tm_header] Data is: " << payload_ptr << std::endl;

			// with a for loop printing out data we avoid printing unwanted extra chars
			std::cout << "[tmtc.cpp:parse_tm_header] Data is: ";
			for(int i = 0; i < tm_frame.tf_size; i++)
			{
				std::cout << payload_ptr[i];
			}
			std::cout << "\n";

			// copy postamble from adress in frame_ptr to local struct for processing
			uint32_t postamble = 0;
			if(memcpy(&postamble, (frame_ptr +sizeof(TM_HEADER) + tm_frame.tf_size), sizeof(uint32_t)) == NULL)
			{
				std::cout << "[tmtc.cpp:parse_tm_header] memcpy returned empty address!" << std::endl;
			}
			std::cout << "[tmtc.cpp:parse_tm_header] Postamble: " << std::hex << postamble << std::endl;
			return 0;
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
		/*	encapsulate - encapsulates a message in a telecommand header and returns the pointer to the packet in memory
		*	in: char pointer to message and length of message, e.g strlen(message)
		*	out: creates a telecommand packet in memory
		*	returns pointer to start of packet
		*/
		uint8_t *encapsulate(const char *message, uint32_t message_len)
    	{
        
	        TC_HEADER tx_tc_frame;
	        tx_tc_frame.preamble = PREAMBLE;
	        uint32_t total_len = (sizeof(TC_HEADER) + message_len);
	        tx_tc_frame.total_len = total_len;
	        tx_tc_frame.tf_size = message_len;
	        
	        // allocate memory space for TC packet
	        uint8_t *tc_ptr = (uint8_t *)malloc(tx_tc_frame.total_len);
	        // copy TC header to allocated memory
	        if(memcpy(tc_ptr, &tx_tc_frame, sizeof(TC_HEADER)) == NULL)
	        {
	            std::cout << "memcpy returned empty address!" << std::endl;
	        }

	        // adding message after tc header
	        if(memcpy((tc_ptr + sizeof(TC_HEADER)), message, tx_tc_frame.tf_size) == NULL)
	        {
	            std::cout << "memcpy returned empty address!" << std::endl;
	        }

	        // return the pointer containing the adress to the constructed TC frame
	        return tc_ptr; 
    	}

		/*	decapsulate - decapsulates a TC packet and prints the payload data
		*	in: uint8_t pointer to TC packet memory address, uint32_t data_size (not used)
		*	out: prints out payload data from TC packet
		*	returns 0
		*/
		int decapsulate(uint8_t *tc_ptr, uint32_t data_size)
		{
			// Init struct TC_HEADER locally
			TC_HEADER tc_header;
			// save TC header first
			memcpy(&tc_header, tc_ptr, sizeof(TC_HEADER));
			// use tf_size from TC header to malloc memory for payload
			uint8_t *payload = (uint8_t *)malloc(tc_header.tf_size);
			// copy data from TC pointer to payload
			memcpy(payload, (tc_ptr + sizeof(TC_HEADER)), tc_header.tf_size);
			// print payload
			std::cout << "[tmtc.cpp:decapsulate] Payload: " << payload << std::endl;

			return 0;
		}
	} /* telecommand */
}	/* tmtc */














