/*  tables.h
*   all typedef structs are done in this file
*   author: Martin Elfvelin
*/

#ifndef TABLES_H
#define TABLES_H

typedef struct __attribute__ ((packed)) _ICD_HEADER {
    uint32_t preamble;       /* Packet preamble */
    uint32_t total_len;      /* Total packet length */
    uint64_t time_tag;       /* Telemetry time-tag */
    uint32_t scrambler;       /* CCSDS scrambler */
    uint32_t basis;           /* RS basis */
    uint32_t fec;             /* FEC code */
    uint32_t tf_size;        /* Transfer frame size */
} ICD_HEADER;

typedef struct __attribute__ ((packed)) _TC_HEADER {
    uint32_t preamble;       /* Packet preamble */
    uint32_t total_len;      /* Total packet length */
    uint32_t tf_size;        /* Transfer frame size */
} TC_HEADER;

#endif /* TABLES_H */
