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

typedef struct __attribute__ ((packed)) _DL_TABLE {
    uint32_t nPreamble;         /* Packet preamble */
    uint32_t nMsglen;           /* Total packet length */
    uint32_t nMsgType;          /* Payload length */
    uint32_t nScrambler;        /* CCSDS scrambler */
    uint32_t nFEC;              /* FEC code */
    uint32_t nFrameFormat;      /* Frame format */
    uint32_t nLineCode;         /* Line code */
    uint32_t nModScheme;        /* Modulation scheme */
    float fBitRate;            /* Bit rate */
    uint32_t nPlopVersion;      /* PLOP version */
} DL_TABLE;

typedef struct __attribute__ ((packed)) _UL_TABLE {
    uint32_t nPreamble;         /* Packet preamble */
    uint32_t nMsglen;           /* Total packet length */
    uint32_t nMsgType;          /* Payload length */
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

typedef struct __attribute__ ((packed)) _UPLINK_TABLE {

    uint32_t idle_pattern;   /* Idle pattern */
    uint32_t idle_pat_len;   /* Idle pattern length */
    uint32_t cmm1_check;     /* CMM1 Check */
    uint32_t cmm1_set_time;  /* CMM1 RF Set Time */
    uint32_t cmm1_offset;    /* CMM1 byte number */
    uint32_t cmm1_mask;      /* CMM1 Mask */
    uint32_t cmm2_check;     /* CMM1 Check */
    uint32_t cmm2_fall_time; /* CMM1 RF Set Time */
    uint32_t cmm2_offset;    /* CMM1 byte number */
    uint32_t cmm2_mask;      /* CMM1 Mask */
    uint32_t postamble;      /* Postamble */
} UPLINK_TABLE;


#endif /* TABLES_H */
