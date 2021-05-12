#include <stdio.h>
#include <stdint.h>

/* icd.cpp
*	this is a small exercise to learn how to handle tm and tc headers
*	using struct pointers and structs
*/

typedef struct __attribute__ ((packed)) _ICD_HEADER {
            uint32_t preamble;       /* Packet preamble */
            uint32_t total_len;      /* Total packet length */
            uint64_t time_tag;       /* Telemetry time-tag */
            uint8_t scrambler;       /* CCSDS scrambler */
            uint8_t basis;           /* RS basis */
            uint8_t fec;             /* FEC code */
            uint32_t tf_size;        /* Transfer frame size */
        } ICD_HEADER;

typedef struct __attribute__ ((packed)) _TC_HEADER {
            uint32_t preamble;       /* Packet preamble */
            uint32_t total_len;      /* Total packet length */
            uint32_t tf_size;        /* Transfer frame size */
        } TC_HEADER;

int print_tc_header(TC_HEADER *ptr)
{
	printf("From print: %x\n", ptr->preamble);	
	printf("| %i | %i | %i |\n", ptr->preamble, ptr->total_len, ptr->tf_size);
	return 0;
}

int main(void)
{
	ICD_HEADER tm_header;
	TC_HEADER tc_header;

	tc_header.preamble = 10000;
	tc_header.total_len =  50;
	tc_header.tf_size = 100;
	printf("preamble = %x\n", tc_header.preamble);
	print_tc_header(&tc_header);	

	return 0;
}
