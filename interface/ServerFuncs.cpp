#include "ServerFuncs.h"


namespace ServerFuncs
{
	/* EncapsulateTM - Creates a TM frame according to tables.h by encapsulating input parameters and local variables
	*	inputs: uint8_t pointers pnOut and pnData, uint32_t nScrambler, nFec, nFrameFormat, nCaduSize
	*	outputs: writes TM frame directly to memory address pointed to by pnOut (needs to be allocated before calling this function)
	*	function calls: utils::GenIRIGBTab()
	* 	author: Martin Elfvelin
	*/
	void EncapsulateTM(uint8_t *pnOut, uint32_t nScrambler, uint32_t nFEC, uint32_t nFrameFormat, uint32_t nCaduSize, uint8_t *pnData)
	{
		TM_HEADER sTMheader;
		sTMheader.nPreamble = PREAMBLE;		// PREAMBLE defined in tables.h
		sTMheader.nMsgType = 0;
		sTMheader.nScrambler = nScrambler;
		sTMheader.nFEC = nFEC;
		sTMheader.nFrameFormat = nFrameFormat;
		sTMheader.nCaduSize = nCaduSize;
		sTMheader.nMsglen = sizeof(TM_HEADER) + nCaduSize + sizeof(uint32_t);
		sTMheader.nTimeTag = utils::GenIRIGBTag();
	
		// copies tm header to output ref/ptr
		memcpy(pnOut, &sTMheader, sizeof(TM_HEADER));
		// copies data from input reference to output reference
		memcpy((pnOut + sizeof(TM_HEADER)), pnData, nCaduSize);
		// copies postable from local variable to output reference
		memcpy(pnOut + sizeof(TM_HEADER) + nCaduSize, &sTMheader.nPreamble, sizeof(uint32_t));
		return;
	}

	/* DecapsulateTC - strips down TC Frame into its individual parameters
	*	inputs: uint8_t pointers pnPacket (incoming frame) and pnData (decapsulated data), uint32_t references for storing decapsulated parameters
	*	outputs: writes payload data to memory address pointed to by pnData, writes data to parameters referenced in 
	* 		input (needs to be allocated before calling this function)
	*	function calls: utils::GenIRIGBTab()
	* 	author: Martin Elfvelin
	*/
	void DecapsulateTC(uint8_t *pnPacket, uint32_t& nMsglen, uint32_t& nMsgType,
			uint32_t& nCltuSize, uint8_t *pnData)
	{
		TC_HEADER m_sTCheader;
		// Copy TM header to local struct
		memcpy(&m_sTCheader, pnPacket, sizeof(TC_HEADER));
		nMsglen = (m_sTCheader.nMsglen & 0x3FF);
		nMsgType = (m_sTCheader.nMsgType & 0x15);
		nCltuSize = (m_sTCheader.nCltuSize & 0x3FF);

		// copy data to address pointed to by pnData using CADU Size
		memcpy(pnData, (pnPacket + sizeof(TC_HEADER)) , m_sTCheader.nCltuSize);
		return;
	}
	/* EncapsulateTCACK - Creates a TC ACK frame according to tables.h by encapsulating input parameters, local variables and GLOBAL parameters (Uplink table)
	*	inputs: uint8_t pointers pnOut and pnData, uint32_t nCltuSize, nAckCode
	*	outputs: writes TC ACK frame directly to memory address pointed to by pnOut (needs to be allocated before calling this function)
	*	function calls: utils::GenIRIGBTab()
	* 	author: Martin Elfvelin
	*/
	void EncapsulateTCACK(uint8_t *pnOut, uint32_t nCltuSize, uint32_t nAckCode, uint8_t *pnData)
	{
		TCACK_HEADER m_sTcAckHeader;
		// REMEMBER to add extern globals in h file
		m_sTcAckHeader.nPreamble = PREAMBLE;		// PREAMBLE defined in tables.h
		m_sTcAckHeader.nMsglen = sizeof(TCACK_HEADER) + nCltuSize;
		m_sTcAckHeader.nTimeTag = utils::GenIRIGBTag();
		m_sTcAckHeader.nMsgType = 2;
		m_sTcAckHeader.nScrambler = sUlTable.nScrambler;
		m_sTcAckHeader.nFEC = sUlTable.nFEC;
		m_sTcAckHeader.nFrameFormat = sUlTable.nFrameFormat;
		m_sTcAckHeader.nLineCode = sUlTable.nLineCode;
		m_sTcAckHeader.nModScheme = sUlTable.nModScheme;
		m_sTcAckHeader.fBitRate = sUlTable.fBitRate;
		m_sTcAckHeader.nPlopVersion = sUlTable.nPlopVersion;
		m_sTcAckHeader.nAckCode = nAckCode;
		m_sTcAckHeader.nCltuSize = nCltuSize;
		uint32_t m_nPostamble = POSTAMBLE;

		memcpy(pnOut, &m_sTcAckHeader, sizeof(TCACK_HEADER));
		memcpy((pnOut + sizeof(TCACK_HEADER)), pnData, nCltuSize);
		memcpy((pnOut + sizeof(TCACK_HEADER) + nCltuSize), &m_nPostamble, sizeof(m_nPostamble));
	
		return;
	}

	/* DecapsulateTrack - strips down Tracking frame into its individual parameters
	*	inputs: uint8_t pointers pnPacket (incoming frame) and pnData (decapsulated data), uint32_t references for storing decapsulated parameters
	*	outputs: writes payload data to memory address pointed to by pnData, writes data to parameters referenced in 
	* 		input (needs to be allocated before calling this function)
	*	function calls: none
	* 	author: Martin Elfvelin
	*/
	void DecapsulateTrack(uint8_t *pnPacket, uint32_t& nMsglen, uint64_t& nTimeTag, uint32_t& nMsgType, uint8_t *pnData)
	{
		TR_HEADER m_sTRheader;
		// copy data from pointer to local struct
		memcpy(&m_sTRheader, pnPacket, sizeof(TR_HEADER));

		// transfer parameters from local struct to references using bit masking
		nMsglen = (m_sTRheader.nMsglen & 0x3FF);
		nTimeTag = m_sTRheader.nTimeTag;
		nMsgType = (m_sTRheader.nMsgType & 0x15);

		// copy payload data from packet pointer to payload pointer
		memcpy(pnData, (pnPacket + sizeof(TR_HEADER)), (nMsglen - sizeof(TR_HEADER) - sizeof(uint32_t)));
		return;
	}

	/* DecapsulateAntenna - strips down Antenna frame into its individual parameters
	*	inputs: uint8_t pointers pnPacket (incoming frame) and pnData (decapsulated data), uint32_t references for storing decapsulated parameters
	*	outputs: writes data to parameters referenced in innput (needs to be allocated before calling this function)
	*	function calls: none
	* 	author: Martin Elfvelin
	*/
	void DecapsulateAntenna(uint8_t *pnPacket, uint32_t& nMsglen, uint64_t& nTimeTag, uint32_t& nMsgType, float& fSatID, float& fAzimuth, float& fElevation)
	{
		ANT_HEADER m_sANTheader;

		memcpy(&m_sANTheader, pnPacket, sizeof(m_sANTheader));

		nMsglen = (m_sANTheader.nMsglen & 0x3FF);
		nTimeTag = m_sANTheader.nTimeTag;
		nMsgType = (m_sANTheader.nMsgType & 0x15);
		fSatID = m_sANTheader.fSatID;
		fAzimuth = m_sANTheader.fAzimuth;
		fElevation = m_sANTheader.fElevation;

		return;
	}

	/* EncapsulateDoppler - Creates a Doppler frame according to tables.h by encapsulating input parameters and local variables
	*	inputs: uint8_t pointer pnOut and float parameters to be encapsulated
	*	outputs: writes Doppler frame directly to memory address pointed to by pnOut (needs to be allocated before calling this function)
	*	function calls: utils::GenIRIGBTab()
	* 	author: Martin Elfvelin
	*/
	void EncapsulateDoppler(uint8_t *pnOut, float fSatID, float fRxFreq, float fRxDoppler, float fTxFreq, float fTxDoppler)
	{
		DOPP_HEADER m_sDOPPheader;
		m_sDOPPheader.nPreamble = PREAMBLE;
		m_sDOPPheader.nMsglen = sizeof(m_sDOPPheader) + sizeof(uint32_t);		
		m_sDOPPheader.nTimeTag = utils::GenIRIGBTag();
		m_sDOPPheader.nMsgType = 5;
		m_sDOPPheader.fSatID = fSatID;
		m_sDOPPheader.fRxFreq = fRxFreq;
		m_sDOPPheader.fRxDoppler = fRxDoppler;
		m_sDOPPheader.fTxFreq = fTxFreq;
		m_sDOPPheader.fTxDoppler = fTxDoppler;

		uint32_t m_nPostamble = POSTAMBLE;

		memcpy(pnOut, &m_sDOPPheader, sizeof(DOPP_HEADER));
		memcpy((pnOut + sizeof(DOPP_HEADER)), &m_nPostamble, sizeof(uint32_t));

		return;
	}

	/* DecapsulateDoppler - strips down Doppler frame into its individual parameters
	*	inputs: uint8_t pointers pnPacket (incoming frame), uint32_t references and float references for storing decapsulated parameters
	*	outputs: writes data to parameters referenced in innput (needs to be allocated before calling this function)
	*	function calls: none
	* 	author: Martin Elfvelin
	*/
	void DecapsulateDoppler(uint8_t *pnPacket, uint32_t& nMsglen, uint64_t& nTimeTag, uint32_t& nMsgType, float& fSatID,
		float& fRxFreq, float& fRxDoppler, float& fTxFreq, float& fTxDoppler)
	{
		DOPP_HEADER m_sDOPPheader;

		memcpy(&m_sDOPPheader, pnPacket, sizeof(DOPP_HEADER));

		nMsglen = (m_sDOPPheader.nMsglen & 0x3FF);
		nTimeTag = m_sDOPPheader.nTimeTag;
		nMsgType = (m_sDOPPheader.nMsgType & 0x15);
		fSatID = m_sDOPPheader.fSatID;
		fRxFreq = m_sDOPPheader.fRxFreq;
		fRxDoppler = m_sDOPPheader.fRxDoppler;
		fTxFreq = m_sDOPPheader.fTxFreq;
		fTxDoppler = m_sDOPPheader.fTxDoppler;

		return;
	}

	/* EncapsulateDecoder - Creates a Decoder frame according to tables.h by encapsulating input parameters and local variables
	*	inputs: uint8_t pointers pnOut and pnData, uint32_t parameters to be encapsulated
	*	outputs: writes Decoder frame directly to memory address pointed to by pnOut (needs to be allocated before calling this function)
	*	function calls: none
	* 	author: Martin Elfvelin
	*/
	void EncapsulateDecoder(uint8_t *pnOut, uint32_t nPayloadLen, uint8_t *pnData)
	{
		DEC_HEADER m_sDECheader;

		m_sDECheader.nPreamble = PREAMBLE;
		m_sDECheader.nMsglen = sizeof(DEC_HEADER) + nPayloadLen + sizeof(uint32_t);
		m_sDECheader.nMsgType = 6;
		m_sDECheader.nPayloadLen = nPayloadLen;
		uint32_t m_nPostamble = POSTAMBLE;

		memcpy(pnOut, &m_sDECheader, sizeof(DEC_HEADER));
		memcpy((pnOut + sizeof(DEC_HEADER)), pnData, nPayloadLen);
		memcpy((pnOut + sizeof(DEC_HEADER) + nPayloadLen), &m_nPostamble, sizeof(uint32_t));

		return;
	}

	/* DecapsulateStatus - strips down Status frame into its individual parameters
	*	inputs: uint8_t pointers pnPacket (incoming frame), uint32_t references for storing decapsulated parameters
	*	outputs: writes data to parameters referenced in input (needs to be allocated before calling this function)
	*	function calls: none
	* 	author: Martin Elfvelin
	*/
	void DecapsulateStatus(uint8_t *pnPacket, uint32_t& nMsglen, uint32_t& nMsgType, uint32_t& nTabType)
	{
		STATUS_HEADER m_sStatusHeader;

		memcpy(&m_sStatusHeader, pnPacket, sizeof(STATUS_HEADER));

		nMsglen = (m_sStatusHeader.nMsglen & 0x3FF);
		nMsgType = (m_sStatusHeader.nMsgType & 0xFF);
		nTabType = (m_sStatusHeader.nTabType & 0xFF);

		return;
	}

	/* DecapsulateSession - strips down Session frame into its individual parameters
	*	inputs: uint8_t pointers pnPacket (incoming frame), uint32_t references for storing decapsulated parameters
	*	outputs: writes data to parameters referenced in input (needs to be allocated before calling this function)
	*	function calls: none
	* 	author: Martin Elfvelin
	*/
	void DecapsulateSession(uint8_t *pnPacket, uint32_t& nMsglen, uint32_t& nMsgType, uint32_t& nTabType)
 	{	
 		SESH_HEADER m_sSeshHeader;
 		memcpy(&m_sSeshHeader, pnPacket, sizeof(SESH_HEADER));
 		// bit mask these
	 	nMsglen = (m_sSeshHeader.nMsglen & 0x3FF);
	 	nMsgType = (m_sSeshHeader.nMsgType & 0xFF);
	 	nTabType = (m_sSeshHeader.nTabType & 0xFF);
		return;
	 }

	 /*	MsgHandler - identifies and dispatches frames of unknown formats
	 *	inputs: nSockDesc (socket descriptor) from the tcp socket where the message arrived, pointer to the buffer where
	 * 		recevied messages are stored and a reference to a parameter containing number of bytes read from the socket
	 *	outputs: returns -1 on error, otherwise 0
	 *	function calls: GetMsgType(), also alls any Decapsulate function according to what MsgType is returned by GetMsgType()
	 *	author: Martin Elfvelin
	 *	This function is only partially implemented and will be combined with or replaced by the ASM sync function
	 */

	int MsgHandler(uhd::usrp::multi_usrp::sptr usrp, uint8_t *pnBuffer, uint32_t& nValread, Transmitter TransmitterObj)
	{
		static uint32_t m_nMsglen;
		static uint32_t m_nMsgType;
		static uint64_t m_nTimeTag;
		static uint32_t m_nCaduSize;
		static uint32_t m_nCltuSize;
		static uint32_t m_nScrambler;
		static uint32_t m_nFEC;
		static uint32_t m_nFrameFormat;
		static uint32_t m_nAckCode;
		static uint32_t m_nTabType;
		uint8_t *m_pnData;
		uint8_t *m_pnPacket;
		static float satid;
		static float az = 0.0;
		static float el = 0.0;
		static float rxfreq = 0;
		static float rxdopp = 0;
		static float txfreq = 0;
		static float txdopp = 0;
		// uint32_t m_nMsgType = utils::GetMsgType(pnBuffer);
		server::FindAsmThree(pnBuffer, m_nMsgType);		// will write message type from buffer to m_nMsgType

		if(m_nMsgType < 0)
		{
			std::cout << "Invalid message type!" << std::endl;
			return -1;
		}

		switch(m_nMsgType)
		{
			case 0:
			// TM frames are never decapsulated on the server side
			//	std::cout << "TM Frame decode start" << std::endl;
			//	ServerFuncs::DecapsulateTM()
				break;
			
			case 1:
			// TC frame
				std::cout << "TC Frame decode start" << std::endl;
				m_pnData = (uint8_t *)malloc(sizeof(uint64_t));
				ServerFuncs::DecapsulateTC(pnBuffer, m_nMsglen, m_nMsgType, m_nCltuSize, m_pnData);
				// use m_nMsglen to know how much data to transmit with the USRP
				// make a std::vector of uint8 and copy tc to it
				// memcpy is ok if resize first:
				std::vector<uint8_t> OriginalBits;
				std::vector<std::complex<float> > modulatedSamps;	//TRANSMITTER OUTPUT
				OriginalBits.resize(m_nMsglen);
				memcpy(&OriginalBits[0], pnBuffer, m_nMsglen);
				TransmitterObj.Execute(OriginalBits, modulatedSamps);
				test_exec(usrp, modulatedSamps, modulatedSamps.size());



				// send ModulatedBits using ModulatedBits.size()
				// TC accept condition
				if(m_nCltuSize > 0)
				{	
					// Allocate memory for constructing TC ACK frame
					m_pnPacket = (uint8_t *)malloc(sizeof(TCACK_HEADER) + m_nCltuSize + sizeof(uint32_t));
					// TC accepted, encapsulate TC ACK frame  
					ServerFuncs::EncapsulateTCACK(m_pnPacket, m_nCltuSize, 2, m_pnData);

					// sending TC ack through socket where TC was received
					if(send(nSockDesc, m_pnPacket, (sizeof(TCACK_HEADER) + m_nCltuSize + sizeof(uint32_t)), 0) < 0)
					{
						std::cout << "Send returned -1" << std::endl;		
					}
					free(m_pnPacket);
					free(m_pnData);
					std::cout << "TC frame sent" << std::endl;
				}
				break;	

			case 2:
				std::cout << "TC ACK Frame decode start" << std::endl;
				break;

			case 3:
				std::cout << "Tracking Frame decode start" << std::endl;
				break;

			case 4:
				std::cout << "Antenna Frame decode start" << std::endl;
				break;

			case 5:
				std::cout << "Doppler Frame decode start" << std::endl;
				// set lo_offset
				uhd::tune_request_t tune_request;
        		tune_request = uhd::tune_request_t(freq, lo_offset);
        		usrp->set_tx_freq(tune_request, channel_nums[i]);
				break;

			case 6:
				std::cout << "Decoder Frame decode start" << std::endl;
				break;

			case 7:
				std::cout << "RESERVED Frame" << std::endl;
				break;

			case 8:
				std::cout << "RESERVED Frame" << std::endl;
				break;

			case 9:
				std::cout << "RESERVED Frame" << std::endl;
				break;

			case 10:
			// Session frame
				std::cout << "Session Frame decode start" << std::endl;
				ServerFuncs::DecapsulateSession(pnBuffer, m_nMsglen, m_nMsgType, m_nTabType);
				if(m_nTabType == 1)
	    		{
	    			ServerFuncs::SetConfTable(pnBuffer, m_nMsglen, m_nTabType);	
	    		}	
				break;

			case 11:
			// Status frame
				std::cout << "Status Frame decode start" << std::endl;
				ServerFuncs::DecapsulateStatus(pnBuffer, m_nMsglen, m_nMsgType, m_nTabType);
	    		
	    		//  currently only 2 config tables implemented
	    		switch(m_nTabType)
	    		{
	    			case 0:
	    			DL_TABLE *m_spDlTable;
		    		m_spDlTable = &sDlTable;
		    		std::cout << "Attempting to send back DL TABLE" << std::endl;		
		    		send(nSockDesc, m_spDlTable, sizeof(UL_TABLE), 0);		    
		    		break;

		    		case 1:
		    		UL_TABLE *m_spUlTable;
		    		m_spUlTable = &sUlTable;
		    		std::cout << "Attempting to send back UL TABLE" << std::endl;		
		    		send(nSockDesc, m_spUlTable, sizeof(UL_TABLE), 0);		    
		    		break;
	    		}
				break;

			default:
				std::cout << "Frame not reckognized" << std::endl;
				return -1;

		}

		return 0;
	}

	void SetConfTable(uint8_t *pnData, uint32_t nMsglen, uint32_t nTabType)
	{
		switch(nTabType)
		{
			case 0:
			memcpy(&sDlTable, pnData, nMsglen);
			return;
			case 1:
			memcpy(&sUlTable, pnData, nMsglen);
			return;
		}
		return;
	}

} 	/* ServerFuncs */