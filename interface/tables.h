/*  tables.h
*   all typedef structs are done in this file
*   author: Martin Elfvelin
*/

#ifndef TABLES_H
#define TABLES_H

#define PREAMBLE 0xA1B2C3D4
#define POSTAMBLE 0xA1B2C3D4

typedef struct __attribute__ ((packed)) _ICD_HEADER {
    uint32_t preamble;       /* Packet preamble */
    uint32_t total_len;      /* Total packet length */
    uint64_t time_tag;       /* Telemetry time-tag */
    uint32_t scrambler;       /* CCSDS scrambler */
    uint32_t basis;           /* RS basis */
    uint32_t fec;             /* FEC code */
    uint32_t tf_size;        /* Transfer frame size */
} TM_HEADER;

typedef struct __attribute__ ((packed)) _TC_HEADER {
    uint32_t preamble;       /* Packet preamble */
    uint32_t total_len;      /* Total packet length */
    uint32_t tf_size;        /* Transfer frame size */
} TC_HEADER;

typedef struct __attribute__ ((packed)) _TM_FRAME {
    uint32_t preamble;          /* Packet preamble */
    uint32_t total_len;         /* Total packet length */
    uint64_t time_tag;          /* Telemetry time-tag */
    uint32_t scrambler;         /* CCSDS scrambler */
    uint32_t basis;             /* RS basis */
    uint32_t fec;               /* FEC code */
    uint32_t tf_size;           /* Transfer frame size */
    uint8_t *data;              /* Payload data */
    uint32_t postamble;         /* Packet postamble */
} TM_FRAME;


#endif /* TABLES_H */
