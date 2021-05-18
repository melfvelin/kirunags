#ifndef TABLES_H
#define TABLES_H
#include <arpa/inet.h>
#define PREAMBLE 0xA1B2C3D4
#define VERBOSE false
/*
 *  DSP Tables
 *  ==========
 */
typedef struct __attribute__ ((packed)) _TC_HEADER {
    uint32_t preamble;       /* Packet preamble */
    uint32_t total_len;      /* Total packet length */
    uint32_t tf_size;        /* Transfer frame size */
} TC_HEADER;

typedef struct __attribute__ ((packed)) _TM_HEADER {
    uint32_t preamble;       /* Packet preamble */
    uint32_t total_len;      /* Total packet length */
    uint64_t time_tag;       /* Telemetry time-tag */
    uint8_t scrambler;       /* CCSDS scrambler */
    uint8_t basis;           /* RS basis */
    uint8_t fec;             /* FEC code */
    uint32_t tf_size;        /* Transfer frame size */
} TM_HEADER;

/*
 *  GUI Tables
 *  ==========
 */
typedef struct __attribute__ ((packed)) _REQUEST_TABLE {
    uint32_t preamble;       /* Packet preamble */
    uint32_t msg_type;       /* Monitoring table type */
    uint32_t postamble;      /* Postamble */
} REQUEST_TABLE;

typedef enum _REQ_TABLE
{
    REQ_ERROR		= 0,
    REQ_USRP_TABLE,
    REQ_UPLINK_TABLE,
    REQ_DOWNLINK_TABLE
} REQ_TABLE;


typedef struct __attribute__ ((packed)) _USRP_TABLE {
    uint32_t preamble;       /* Packet preamble */
    uint32_t total_len;      /* Total packet length */
    uint64_t time_tag;       /* Message time-tag */
    uint32_t msg_type;       /* Monitoring table type */
    uint32_t iq_source;      /* IQ Source */
    uint32_t sub_dev;        /* Sub-device */
    float sps;            /* File samples per symbol */
    uint32_t ipv4_address;   /* IPV4 Address */
    uint64_t rx_freq;        /* Rx Freq */
    uint32_t rx_antenna;     /* Rx Antenna */
    uint32_t rx_gain;        /* Rx Gain*/
    uint64_t tx_freq;        /* Rx Freq */
    uint32_t tx_antenna;     /* Rx Antenna */
    uint32_t tx_gain;        /* Rx Gain */
    uint32_t postamble;      /* Postamble */
} USRP_TABLE;

typedef struct __attribute__ ((packed)) _UPLINK_TABLE {
    uint32_t preamble;       /* Packet preamble */
    uint32_t total_len;      /* Total packet length */
    uint64_t time_tag;       /* Message time-tag */
    uint32_t msg_type;       /* Monitoring table type */
    uint32_t modulation;     /* TC Modulation */
    uint32_t bit_rate;       /* Uplink bit rate */
    uint32_t line_code;      /* Uplink line code */
    uint32_t car_freq;       /* Uplink mod car frequency */
    uint32_t subcar_freq;    /* Uplink sub car frequency */
    uint32_t subcar_type;    /* Subcar type */
    uint32_t plop_version;   /* Physical layer operations procedures */
    uint32_t scramble;       /* CCSDS Scrambler */
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

typedef struct __attribute__ ((packed)) _DOWNLINK_TABLE {
    uint32_t preamble;       /* Packet preamble */
    uint32_t total_len;      /* Total packet length */
    uint64_t time_tag;       /* Message time-tag */
    uint32_t msg_type;       /* Monitoring table type */
    uint32_t modulation;     /* TC Modulation */
    uint32_t bit_rate;       /* Uplink bit rate */
    uint32_t line_code;      /* Uplink line code */
    uint32_t filter_type;    /* Filter type */
    float rolloff;           /* Filter rollof */
    float loop_bw;           /* Loop bandwidth */
    uint32_t cadu_size;      /* CADU Size */
    uint32_t syncword;       /* Sync word */
    uint32_t syncword_len;   /* Sync word length */
    uint32_t fec_type;       /* FEC Type */
    uint32_t rs_depth;       /* RS interleave depth */
    uint32_t rs_mode;        /* RS encoding mode */
    uint32_t scramble;       /* CCSDS Scrambler */
    uint32_t SYN;
    uint32_t CTL;
    uint32_t LTS;
    uint32_t SLIP;
    uint32_t postamble;      /* Postamble */
} DOWNLINK_TABLE;

typedef struct __attribute__((packed)) _ERROR_TABLE
{
    uint32_t preamble;       /* Packet preamble */
    uint32_t total_len;      /* Total packet length */
    uint64_t time_tag;       /* Message time-tag */
    uint32_t msg_type;       /* Monitoring table type */
    uint32_t error;          /* Error Code */
    uint32_t postamble;      /* Packet postamble */
} ERROR_TABLE;

// Time Tagging
inline int
DecimalToBCD (int Decimal)
{
   return (((Decimal/10) << 4) | (Decimal % 10));
}

inline int
BCDToDecimal(int BCD)
{
   return (((BCD>>4)*10) + (BCD & 0xF));
}

inline uint64_t
GenIRIGBTag()
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
//    if (VERBOSE)
//    {
//        std::cout << "gmtm->tm_yday = " << gmtm->tm_yday << std::endl;
//        std::cout << "gmtm->tm_yday%100 = " << (gmtm->tm_yday%100) << std::endl;
//        std::cout << "gmtm->tm_yday/100 = " << (gmtm->tm_yday/100) << std::endl;
//    }

    return IRIGB_Code;

}
#endif // TABLE
