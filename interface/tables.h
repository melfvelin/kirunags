/*  tables.h
*   all typedef structs are done in this file
*   author: Martin Elfvelin
*/

#ifndef TABLES_H
#define TABLES_H

#define PREAMBLE 0xA1B2C3D4
#define POSTAMBLE ~0xA1B2C3D4

typedef struct __attribute__ ((packed)) _ICD_HEADER {
    uint32_t nPreamble;       	/* Packet preamble */
    uint32_t nMsglen;      		/* Total packet length */
    uint64_t nTimeTag;       	/* Telemetry time-tag */
    uint32_t nMsgType;       	/* Message type */
    uint32_t nScrambler;       	/* CCSDS scrambler */
    uint32_t nFEC;           	/* FEC code */
    uint32_t nFrameFormat;      /* FEC code */
    uint32_t nCaduSize;        	/* CADU size */
} TM_HEADER;

typedef struct __attribute__ ((packed)) _TC_HEADER {
    uint32_t nPreamble;       	/* Packet preamble */
    uint32_t nMsglen;      		/* Total packet length */
    uint32_t nTimeTag;        	/* Transfer frame size */
    uint32_t nMsgType;			/* Message type */
    uint32_t nCltuSize;			/* CLTU size */
} TC_HEADER;

typedef struct __attribute__ ((packed)) _TR_HEADER {
    uint32_t nPreamble;         /* Packet preamble */
    uint32_t nMsglen;           /* Total packet length */
    uint32_t nTimeTag;          /* Transfer frame size */
    uint32_t nMsgType;          /* Message type */
} TR_HEADER;

typedef struct __attribute__ ((packed)) _ANT_HEADER {
    uint32_t nPreamble;         /* Packet preamble */
    uint32_t nMsglen;           /* Total packet length */
    uint32_t nTimeTag;          /* Transfer frame size */
    uint32_t nMsgType;          /* Message type */
    float fSatID;               /* Spacecraft ID */
    float fAzimuth;             /* Azimuth */
    float fElevation;           /* Elevation */
} ANT_HEADER;

typedef struct __attribute__ ((packed)) _DOPP_HEADER {
    uint32_t nPreamble;         /* Packet preamble */
    uint32_t nMsglen;           /* Total packet length */
    uint32_t nTimeTag;          /* Transfer frame size */
    uint32_t nMsgType;          /* Message type */
    float fSatID;               /* Spacecraft ID */
    float fRxFreq;              /* Receive Frequency */
    float fRxDoppler;           /* Received Doppler */
    float fTxFreq;              /* Transmit Frequency */
    float fTxDoppler;           /* Transmit Doppler */
} DOPP_HEADER;

typedef struct __attribute__ ((packed)) _DEC_HEADER {
    uint32_t nPreamble;         /* Packet preamble */
    uint32_t nMsglen;           /* Total packet length */
    // uint32_t nMsgType;          /* Message type */
    uint32_t nPayloadLen;       /* Payload length */
} DEC_HEADER;

#endif /* TABLES_H */
