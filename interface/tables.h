/*  tables.h
*   all typedef structs are done in this file
*   author: Martin Elfvelin
*/

#ifndef TABLES_H
#define TABLES_H

#define PREAMBLE 0xA1B2C3D4
#define POSTAMBLE ~0xA1B2C3D4

#define TRUE   1 
#define FALSE  0 
#define PORT 8888

typedef struct __attribute__ ((packed)) _TM_HEADER {
    uint32_t nPreamble;       	/* Packet preamble */
    uint32_t nMsglen;      		/* Total packet length */
    uint32_t nMsgType;       	/* Message type */
    uint64_t nTimeTag;       	/* Telemetry time-tag */
    uint32_t nScrambler;       	/* CCSDS scrambler */
    uint32_t nFEC;           	/* FEC code */
    uint32_t nFrameFormat;      /* Frame format */
    uint32_t nLineCode;         /* Line code */
    uint32_t nModScheme;        /* Modulation scheme */
    float fBitRate;            /* Bit rate */
    uint32_t nCaduSize;        	/* CADU size */
} TM_HEADER;

typedef struct __attribute__ ((packed)) _TC_HEADER {
    uint32_t nPreamble;       	/* Packet preamble */
    uint32_t nMsglen;      		/* Total packet length */
    uint32_t nMsgType;			/* Message type */
    uint32_t nCltuSize;			/* CLTU size */
} TC_HEADER;

typedef struct __attribute__ ((packed)) _TCACK_HEADER {
    uint32_t nPreamble;         /* Packet preamble */
    uint32_t nMsglen;           /* Total packet length */
    uint32_t nMsgType;          /* Message type */
    uint64_t nTimeTag;          /* Telemetry time-tag */
    uint32_t nScrambler;        /* CCSDS scrambler */
    uint32_t nFEC;              /* FEC code */
    uint32_t nFrameFormat;      /* Frame format */
    uint32_t nLineCode;         /* Line code */
    uint32_t nModScheme;        /* Modulation scheme */
    float fBitRate;            /* Bit rate */
    uint32_t nPlopVersion;      /* PLOP version */
    uint32_t nAckCode;          /* Ack code */
    uint32_t nCltuSize;         /* CADU size */
} TCACK_HEADER;

typedef struct __attribute__ ((packed)) _TR_HEADER {
    uint32_t nPreamble;         /* Packet preamble */
    uint32_t nMsglen;           /* Total packet length */
    uint32_t nMsgType;          /* Message type */
    uint32_t nTimeTag;          /* Transfer frame size */
} TR_HEADER;

typedef struct __attribute__ ((packed)) _ANT_HEADER {
    uint32_t nPreamble;         /* Packet preamble */
    uint32_t nMsglen;           /* Total packet length */
    uint32_t nMsgType;          /* Message type */
    uint32_t nTimeTag;          /* Transfer frame size */
    float fSatID;               /* Spacecraft ID */
    float fAzimuth;             /* Azimuth */
    float fElevation;           /* Elevation */
} ANT_HEADER;

typedef struct __attribute__ ((packed)) _DOPP_HEADER {
    uint32_t nPreamble;         /* Packet preamble */
    uint32_t nMsglen;           /* Total packet length */
    uint32_t nMsgType;          /* Message type */
    uint32_t nTimeTag;          /* Transfer frame size */
    float fSatID;               /* Spacecraft ID */
    float fRxFreq;              /* Receive Frequency */
    float fRxDoppler;           /* Received Doppler */
    float fTxFreq;              /* Transmit Frequency */
    float fTxDoppler;           /* Transmit Doppler */
} DOPP_HEADER;

typedef struct __attribute__ ((packed)) _DEC_HEADER {
    uint32_t nPreamble;         /* Packet preamble */
    uint32_t nMsglen;           /* Total packet length */
    uint32_t nMsgType;          /* Message type */
    uint32_t nPayloadLen;       /* Payload length */
} DEC_HEADER;

typedef struct __attribute__ ((packed)) _STATUS_HEADER {
    uint32_t nPreamble;         /* Packet preamble */
    uint32_t nMsglen;           /* Total packet length */
    uint32_t nMsgType;          /* Message type */
    uint32_t nTabType;          /* Table type */
    uint32_t nPostamble;        /* Packet postamble */
} STATUS_HEADER;

typedef struct __attribute__ ((packed)) _SESH_HEADER {
    uint32_t nPreamble;         /* Packet preamble */
    uint32_t nMsglen;           /* Total packet length */
    uint32_t nMsgType;          /* Message type */
    uint32_t nTabType;          /* Table type */
} SESH_HEADER;  // replaced for now by sending a conf table on the sesh port but used for decapsulation purposes

typedef struct __attribute__ ((packed)) _GEN_HEADER {
    uint32_t nPreamble;         /* Packet preamble */
    uint32_t nMsglen;           /* Total packet length */
    uint32_t nMsgType;          /* Message type */
    uint32_t nTabType;          /* Table type */
} GEN_HEADER;  // replaced for now by sending a conf table on the sesh port but used for decapsulation purposes

typedef struct __attribute__ ((packed)) _DL_TABLE {
    uint32_t nPreamble;         /* Packet preamble */
    uint32_t nMsglen;           /* Total packet length, fixed */
    uint32_t nMsgType;          /* Message type */
    uint32_t nTabType;          /* Table type */
    uint32_t nScrambler;        /* CCSDS scrambler */
    uint32_t nFEC;              /* FEC code */
    uint32_t nFrameFormat;      /* Frame format */
    uint32_t nLineCode;         /* Line code */
    uint32_t nModScheme;        /* Modulation scheme */
    float fBitRate;             /* Bit rate */
    uint32_t nPostamble;        /* Postamble */
} DL_TABLE;

typedef struct __attribute__ ((packed)) _UL_TABLE {
    uint32_t nPreamble;         /* Packet preamble */
    uint32_t nMsglen;           /* Total packet length, fixed*/
    uint32_t nMsgType;          /* Message type */
    uint32_t nTabType;          /* Table type */
    uint32_t nScrambler;        /* CCSDS scrambler */
    uint32_t nFEC;              /* FEC code */
    uint32_t nFrameFormat;      /* Frame format */
    uint32_t nLineCode;         /* Line code */
    uint32_t nModScheme;        /* Modulation scheme */
    float fBitRate;             /* Bit rate */
    uint32_t nPlopVersion;      /* PLOP version */
    uint32_t nPostamble;        /* Postamble */
} UL_TABLE;


#endif /* TABLES_H */
