/*  tables.h
*   all typedef structs are done in this file
*   author: Martin Elfvelin
*/

#ifndef TABLES_H
#define TABLES_H

#define PREAMBLE 0xA1B2C3D4
#define POSTAMBLE ~0xA1B2C3D4

typedef struct __attribute__ ((packed)) _TM_HEADER {
    uint32_t nPreamble;       	/* Packet preamble */
    uint32_t nMsglen;      		/* Total packet length */
    uint64_t nTimeTag;       	/* Telemetry time-tag */
    uint32_t nMsgType;       	/* Message type */
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
    uint64_t nTimeTag;          /* Telemetry time-tag */
    uint32_t nMsgType;          /* Message type */
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
    uint32_t nTabLen;           /* Table length */
/*  uint8_t* nPayload;             Table data
    uint32_t nPostamble;        */ 
} SESH_HEADER;

/*  REQ TYPES
    10 - DL TABLE
    20 - UL TABLE
    30 - ETC

    With this format the session and status messages can use the same numbers, status request uses the number
    the user wants to retrieve, status response uses the same number as "Message type" (should maybe be called table type not to conflict)
    for that table. Session can use it by specifying what table it is uploading. It could be easier to manage the session payload data
    if the pointer used was a uint32_t pointer instead

*/ 
// No uplink and downlink tables!
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
    uint32_t nFilterType;       /* Filter type */
    uint32_t nFilterRolloff;    /* Filter roll-off */
    uint32_t nLoopBW;           /* Loop bandwidth */
    uint32_t nSynchWord;        /* Hexadecimal sync word */
    uint32_t nSyncWordLen;      /* Sync word length (bits) */
    uint32_t nIntLeavDept;      /* Interleave depth */
    uint32_t nBasis;            /* RS basis */
    uint32_t nPostamble;        /* Packet postamble */
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
    uint32_t nCarFreq;          /* Carrier frequency */
    uint32_t nSubCarFreq;       /* Sub-carrier frequency */
    uint32_t nSubCarType;       /* Sub-carrier type */
    uint32_t nIdlePat;          /* Idle pattern */
    uint32_t nIdlePatLen;       /* Idle pattern length */
    uint32_t nCmm1Check;        /* CMM1 check */
    uint32_t nCmm1SetTime;      /* CMM1 RF set time */
    uint32_t nCmm1Offset;       /* CMM1 byte number */
    uint32_t nCmm1Mask;         /* CMM1 bit mask */
    uint32_t nCmm2Check;        /* CMM2 check */
    uint32_t nCmm2SetTime;      /* CMM2 RF set time */
    uint32_t nCmm2Offset;       /* CMM2 byte number */
    uint32_t nCmm2Mask;         /* CMM2 bit mask */
    uint32_t nPostAmble;        /* Postamble */
} UL_TABLE;


#endif /* TABLES_H */
