// This file will act as the main file for the tie-together of all functions

int main()
{
	// init of usrp, returns usrp pointer
	uhd::usrp::multi_usrp::sptr usrp = test_init();

	// init modulation here

	// filtering params
	Firdes Fir;
    Filter CFilter;
    float fRs = 4e6;
    float fRb = 1e6;
    float fSPS = 4;

    std::vector<float> SRRCTaps,SquareTaps, Taps;
    Firdes CFirdes;
    CFirdes.SRRC(1.0,fRs,fRb,0.35,SRRCTaps);
    CFirdes.Square(int(fSPS),SquareTaps);
    Taps = SRRCTaps;// You can choose SRRCTaps or SquareTaps for Upfilter here.

	std::vector<uint8_t> OriginalBits; // use/nouse?
	std::vector<std::complex<float> > fcxUpSRRC;//TRANSMITTER OUTPUT
	Transmitter TransmitterObj;
        
    // Constellation modulation
    CCommon::Arity nBERArity = CCommon::Binary;
    CCommon::LineCode nLineCode = CCommon::BP_S;

    //transmitter initialize
    //Transmitter TransmitterObj;
    TransmitterObj.Initialize(nLineCode, nBERArity, Taps, fSPS);

    server::SetupServer(TransmitterObj);		// will wait forever for activity on the socket and will call Msghandler when activity happens




    //Transmitter execute
    TransmitterObj.Execute(OriginalBits, fcxUpSRRC);
    std::cout << "Modulated output size: " << fcxUpSRRC.size() <<std::endl;
   



	return 0;
}