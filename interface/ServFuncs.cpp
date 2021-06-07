#include "ServFuncs.h"


namespace ServFuncs
{
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
		sTMheader.nTimeTag = tmtc::GenIRIGBTag();
	
		// copies tm header to output ref/ptr
		memcpy(pnOut, &sTMheader, sizeof(TM_HEADER));
		// copies data from input reference to output reference
		memcpy((pnOut + sizeof(TM_HEADER)), pnData, nCaduSize);
		// copies postable from local variable to output reference
		memcpy(pnOut + sizeof(TM_HEADER) + nCaduSize, &sTMheader.nPreamble, sizeof(uint32_t));
		return;
	}

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

	void EncapsulateTCACK(uint8_t *pnOut, uint32_t nCltuSize, uint32_t nAckCode, uint8_t *pnData)
	{
		TCACK_HEADER m_sTcAckHeader;
		// REMEMBER to add extern globals in h file
		m_sTcAckHeader.nPreamble = PREAMBLE;		// PREAMBLE defined in tables.h
		m_sTcAckHeader.nMsglen = sizeof(TCACK_HEADER) + nCltuSize;
		m_sTcAckHeader.nTimeTag = tmtc::GenIRIGBTag();
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

	void DecapsulateTrack(uint8_t *pnPacket, uint32_t& nMsglen, uint64_t& nTimeTag, uint32_t& nMsgType, uint8_t *pnPayload)
	{
		TR_HEADER m_sTRheader;
		// copy data from pointer to local struct
		memcpy(&m_sTRheader, pnPacket, sizeof(TR_HEADER));

		// transfer parameters from local struct to references using bit masking
		nMsglen = (m_sTRheader.nMsglen & 0x3FF);
		nTimeTag = m_sTRheader.nTimeTag;
		nMsgType = (m_sTRheader.nMsgType & 0x15);

		// copy payload data from packet pointer to payload pointer
		memcpy(pnPayload, (pnPacket + sizeof(TR_HEADER)), (nMsglen - sizeof(TR_HEADER) - sizeof(uint32_t)));
		return;
	}

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

	void EncapsulateDoppler(uint8_t *pnOut, float fSatID, float fRxFreq, float fRxDoppler, float fTxFreq, float fTxDoppler)
	{
		DOPP_HEADER m_sDOPPheader;
		m_sDOPPheader.nPreamble = PREAMBLE;
		m_sDOPPheader.nMsglen = sizeof(m_sDOPPheader) + sizeof(uint32_t);		
		m_sDOPPheader.nTimeTag = tmtc::GenIRIGBTag();
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

	void EncapsulateDecoder(uint8_t *pnOut, uint32_t nPayloadLen, uint8_t *pnPayload)
	{
		DEC_HEADER m_sDECheader;

		m_sDECheader.nPreamble = PREAMBLE;
		m_sDECheader.nMsglen = sizeof(DEC_HEADER) + nPayloadLen + sizeof(uint32_t);
		m_sDECheader.nMsgType = 6;
		m_sDECheader.nPayloadLen = nPayloadLen;
		uint32_t m_nPostamble = POSTAMBLE;

		memcpy(pnOut, &m_sDECheader, sizeof(DEC_HEADER));
		memcpy((pnOut + sizeof(DEC_HEADER)), pnPayload, nPayloadLen);
		memcpy((pnOut + sizeof(DEC_HEADER) + nPayloadLen), &m_nPostamble, sizeof(uint32_t));

		return;
	}

	void DecapsulateStatus(uint8_t *pnPacket, uint32_t& nMsglen, uint32_t& nMsgType, uint32_t& nTabType)
	{
		STATUS_HEADER m_sStatusHeader;

		memcpy(&m_sStatusHeader, pnPacket, sizeof(STATUS_HEADER));

		nMsglen = (m_sStatusHeader.nMsglen & 0x3FF);
		nMsgType = (m_sStatusHeader.nMsgType & 0xFF);
		nTabType = (m_sStatusHeader.nTabType & 0xFF);

		return;
	}

	void DecapsulateSession(uint8_t *pnPacket, uint32_t& nMsglen, uint32_t& nMsgType, uint32_t& nTabType)
 	{	
 		SESH_HEADER m_sSeshHeader;
 		memcpy(&m_sSeshHeader, pnPacket, sizeof(SESH_HEADER));
 		// bit mask these
	 	nMsglen = m_sSeshHeader.nMsglen;
	 	nMsgType = m_sSeshHeader.nMsgType;
	 	nTabType = m_sSeshHeader.nTabType;
		return;
	 }

	int MsgHandler(int nSockDesc, uint8_t *pnBuffer, uint32_t& nValread)
	{
		static uint32_t m_nMsglen;
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
		uint32_t m_nMsgType = tmtc::GetMsgType(pnBuffer, nValread);

		if(m_nMsgType < 0)
		{
			std::cout << "Invalid message type!" << std::endl;
			return -1;
		}

		switch(m_nMsgType)
		{
			case 0:
			// TM frame
			//ServFuncs::DecapsulateTM()
				std::cout << "TM Frame decode start" << std::endl;
				break;
			case 1:
				std::cout << "TC Frame decode start" << std::endl;
				m_pnData = (uint8_t *)malloc(sizeof(uint64_t));
				ServFuncs::DecapsulateTC(pnBuffer, m_nMsglen, m_nMsgType, m_nCltuSize, m_pnData);
				if(m_nCltuSize > 0)
				{
					m_pnPacket = (uint8_t *)malloc(sizeof(TCACK_HEADER) + m_nCltuSize + sizeof(uint32_t));
					// TC accepted, send back TC ACK
					ServFuncs::EncapsulateTCACK(m_pnPacket, m_nCltuSize, 2, m_pnData);		// Encapsulate should return bytes encapsulated
					memcpy(pnBuffer, m_pnPacket, (sizeof(TCACK_HEADER) + m_nCltuSize + sizeof(uint32_t)) );
					
					nValread = sizeof(TCACK_HEADER) + m_nCltuSize + sizeof(uint32_t);					
					return 3;

					if(send(nSockDesc, m_pnPacket, (sizeof(TCACK_HEADER) + m_nCltuSize + sizeof(uint32_t)), 0) < 0)
					{
						std::cout << "Send returned -1" << std::endl;		
					}
					free(m_pnPacket);
					free(m_pnData);
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
				std::cout << "Session Frame decode start" << std::endl;
				ServFuncs::DecapsulateSession(pnBuffer, m_nMsglen, m_nMsgType, m_nTabType);
				if(m_nTabType == 1)
	    		{
	    			SetConfTable(pnBuffer, m_nMsglen, m_nTabType);	
	    		}	
				break;
			case 11:
				std::cout << "Status Frame decode start" << std::endl;
				ServFuncs::DecapsulateStatus(pnBuffer, m_nMsglen, m_nMsgType, m_nTabType);
	    	
	    		switch(m_nTabType)
	    		{
	    			case 0:
	    			return -1;

		    		case 1:
		    		UL_TABLE *m_spTable;
		    		m_spTable = &sUlTable;
		    		std::cout << "Attempting to send back UL TABLE" << m_nTabType << std::endl;		
		    		send(nSockDesc, m_spTable, sizeof(UL_TABLE), 0);		    
		    		break;
	    		}
				break;

			default:
				std::cout << "Frame not reckognized" << std::endl;
				return -1;

		}

		return 0;
	}

} 	/* ServFuncs */