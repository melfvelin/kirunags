#include "ccommon.h"
#include "constellations.h"
#include "prbs.h"
#include "linecode.h"
#include "transmitter.h"

int
CCommon Common;
    enum Test
    {
        BPSK_NRZ_L = 0,
        BPSK_NRZ_S,
        BPSK_NRZ_M,
        BPSK_BP_L,
        BPSK_BP_S,
        BPSK_BP_M,
        QPSK_NRZ_L,
        QPSK_NRZ_S,
        QPSK_NRZ_M,
        QPSK_BP_L,
        QPSK_BP_S,
        QPSK_BP_M
    };

    // Filtering parameters
    Firdes Fir;
    Filter CFilter;
    float fRs = 4e6;
    float fRb = 1e6;
    float fSPS = 4;
    // SatTrack inits
    std::time_t timeArg = time(0);
    double *doubPtr = nullptr;
    doubPtr = new double[5];
    float doppler_factor = 435/100;
    // Filter objects and taps
    std::vector<float> SRRCTaps,SquareTaps, Taps;
    Firdes CFirdes;
    CFirdes.SRRC(1.0,fRs,fRb,0.35,SRRCTaps);
    CFirdes.Square(int(fSPS),SquareTaps);
    Taps = SRRCTaps;// You can choose SRRCTaps or SquareTaps for Upfilter here.

    //************************************TEST******************************************//
    Test Test = BPSK_BP_M;
    //Test Test = BPSK_BP_S;
    //Test Test = BPSK_NRZ_S;
    //Test Test = QPSK_BP_S;
    //Test Test = QPSK_NRZ_S;
    std::vector<uint8_t> OriginalBits;//TRANSMITTER INPUT
    
    // CADU generator
    int nFrameLen = 255*5*8;
    const char* syncword = "0x1ACFFC1D";
    PRBS PRBSObject;
    PRBSObject.Initialize(CCommon::PRBS20,nFrameLen);
    PRBSObject.GetSeq(OriginalBits);
    PRBSObject.AppendSyncWord(syncword,32,OriginalBits);

    std::vector<std::complex<float> > fcxUpSRRC;//TRANSMITTER OUTPUT
    Transmitter TransmitterObj;

    if (Test == BPSK_BP_S)
    {
        // Constellation modulation
        CCommon::Arity nBERArity = CCommon::Binary;
        CCommon::LineCode nLineCode = CCommon::BP_S;

        //transmitter initialize
        //Transmitter TransmitterObj;
        TransmitterObj.Initialize(nLineCode, nBERArity, Taps, fSPS);

        //Transmitter execute
        TransmitterObj.Execute(OriginalBits, fcxUpSRRC);
        std::cout<<fcxUpSRRC.size()<<std::endl;
    }

    else if (Test == BPSK_BP_M)
    {

        // Constellation modulation
        CCommon::Arity nBERArity = CCommon::Binary;
        CCommon::LineCode nLineCode = CCommon::BP_M;

        //transmitter initialize
        //Transmitter TransmitterObj;
        TransmitterObj.Initialize(nLineCode, nBERArity, Taps, fSPS);

        //Transmitter execute
        TransmitterObj.Execute(OriginalBits, fcxUpSRRC);
        std::cout<<fcxUpSRRC.size()<<std::endl;
    }

    else if (Test == BPSK_NRZ_S)
    {

        // Constellation modulation
        CCommon::Arity nBERArity = CCommon::Binary;
        CCommon::LineCode nLineCode = CCommon::NRZ_S;

        //transmitter initialize
        //Transmitter TransmitterObj;
        TransmitterObj.Initialize(nLineCode, nBERArity, Taps, fSPS);

        //Transmitter execute
        TransmitterObj.Execute(OriginalBits, fcxUpSRRC);
        std::cout<<fcxUpSRRC.size()<<std::endl;
    }

    else if (Test == QPSK_BP_S)
    {

        // Constellation modulation
        CCommon::Arity nBERArity = CCommon::Quaternary;
        CCommon::LineCode nLineCode = CCommon::BP_S;

        //transmitter initialize
        //Transmitter TransmitterObj;
        TransmitterObj.Initialize(nLineCode, nBERArity, Taps, fSPS);

        //Transmitter execute
        TransmitterObj.Execute(OriginalBits, fcxUpSRRC);
        std::cout<<fcxUpSRRC.size()<<std::endl;
    }

    else if (Test == QPSK_NRZ_S)
    {
        // Constellation modulation
        CCommon::Arity nBERArity = CCommon::Quaternary;
        CCommon::LineCode nLineCode = CCommon::NRZ_S;

        //transmitter initialize
        //Transmitter TransmitterObj;
        TransmitterObj.Initialize(nLineCode, nBERArity, Taps, fSPS);

        //Transmitter execute
        TransmitterObj.Execute(OriginalBits, fcxUpSRRC);
        std::cout<<fcxUpSRRC.size()<<std::endl;
    }