#include "ClientFuncs.h"

namespace ClientFuncs
{
	void DecapsulateTM(uint8_t *pnPacket, uint32_t& nMsglen, uint64_t& nTimeTag, uint32_t& nMsgType, uint32_t& nScrambler, 
		uint32_t& nFEC, uint32_t& nFrameFormat, uint32_t& nCaduSize, uint8_t *pnData)
	{
		TM_HEADER m_sTMheader;
		// Copy TM header to local struct
		memcpy(&m_sTMheader, pnPacket, sizeof(TM_HEADER));
		nMsglen = (m_sTMheader.nMsglen & 0x3FF);
		nTimeTag = (m_sTMheader.nTimeTag);
		nMsgType = (m_sTMheader.nMsgType & 0x3);
		nScrambler = (m_sTMheader.nScrambler & 0x1);
		nFEC = (m_sTMheader.nFEC & 0XF);
		nFrameFormat = (m_sTMheader.nFrameFormat & 0x7);
		nCaduSize = (m_sTMheader.nCaduSize & 0x3FF);
		// copy data to local address pointed to by pnData using CADU Size
		memcpy(pnData, (pnPacket + sizeof(TM_HEADER)) , m_sTMheader.nCaduSize);
		return;
	}

	void EncapsulateTC(uint8_t *pnOut, uint32_t nCltuSize, uint8_t *pnData)
	{
		// Local variables: Preamble, Postamble, TimeTag add m_ to all locals
		TC_HEADER sTCheader;
		sTCheader.nPreamble = PREAMBLE;			// PREAMBLE defined in tables.h
		sTCheader.nMsgType = 1;
		sTCheader.nCltuSize = nCltuSize;
		sTCheader.nMsglen = sizeof(TC_HEADER) + nCltuSize + sizeof(uint32_t);
		// copies tc header to output ref/ptr
		memcpy(pnOut, &sTCheader, sizeof(TC_HEADER));
		// copies data from input reference to output reference
		memcpy((pnOut + sizeof(TC_HEADER)), pnData, nCltuSize);
		// copies postable from local variable to output reference
		memcpy(pnOut + sizeof(TC_HEADER) + nCltuSize, &sTCheader.nPreamble, sizeof(uint32_t));
		return;
	}

	void DecapsulateTCACK(uint8_t *pnPacket, uint32_t& nAckCode, uint8_t *pnData)
	{	
		TCACK_HEADER m_sTcAckHeader;
		memcpy(&m_sTcAckHeader, pnPacket, sizeof(TCACK_HEADER));
		memcpy(pnData, pnPacket, m_sTcAckHeader.nCltuSize);
		nAckCode = m_sTcAckHeader.nAckCode;
		return;
	}

	void EncapsulateTrack(uint8_t *pnOut, uint32_t nPayloadSize, uint8_t *pnPayload)
	{
		TR_HEADER m_sTRheader;
		m_sTRheader.nPreamble = PREAMBLE;
		m_sTRheader.nMsglen = sizeof(TR_HEADER) + nPayloadSize + sizeof(uint32_t);
		m_sTRheader.nTimeTag = utils::GenIRIGBTag();
		m_sTRheader.nMsgType = 3;
		uint32_t m_nPostamble = POSTAMBLE;

		memcpy(pnOut, &m_sTRheader, sizeof(TR_HEADER));
		memcpy((pnOut + sizeof(TR_HEADER)), pnPayload, nPayloadSize);
		memcpy((pnOut + sizeof(TR_HEADER) + nPayloadSize), &m_nPostamble, sizeof(uint32_t)); 
		return;
	}

	void EncapsulateAntenna(uint8_t *pnOut, float fSatID, float fAzimuth, float fElevation)
	{
		ANT_HEADER m_sANTheader;
		
		m_sANTheader.nPreamble = PREAMBLE;
		m_sANTheader.nMsglen = sizeof(ANT_HEADER) + sizeof(uint32_t);
		// debug
		std::cout << "Message lenght: " << m_sANTheader.nMsglen << std::endl;
		m_sANTheader.nTimeTag = utils::GenIRIGBTag();
		m_sANTheader.nMsgType = 4;
		m_sANTheader.fSatID = fSatID;
		m_sANTheader.fAzimuth = fAzimuth;
		m_sANTheader.fElevation = fElevation;

		uint32_t m_nPostamble = POSTAMBLE;

		memcpy(pnOut, &m_sANTheader, sizeof(ANT_HEADER));
		memcpy((pnOut + sizeof(ANT_HEADER)), &m_nPostamble, sizeof(m_nPostamble));

		return;
	}

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

	void DecapsulateDecoder(uint8_t *pnPacket, uint32_t& nMsglen, uint32_t& nPayloadLen, uint8_t *pnPayload)
	{
		DEC_HEADER m_sDECheader;

		memcpy(&m_sDECheader, pnPacket, sizeof(DEC_HEADER));

		nMsglen = m_sDECheader.nMsglen;
		nPayloadLen = m_sDECheader.nPayloadLen;
		memcpy(pnPayload, (pnPacket + sizeof(DEC_HEADER)), nPayloadLen);

		return;
	}

	void EncapsulateStatus(uint8_t *pnOut, uint32_t nTabType)
	{
		STATUS_HEADER m_sStatusHeader;

		m_sStatusHeader.nPreamble = PREAMBLE;
		m_sStatusHeader.nMsglen = sizeof(STATUS_HEADER);
		m_sStatusHeader.nMsgType = 11;
		m_sStatusHeader.nTabType = nTabType;
		m_sStatusHeader.nPostamble = POSTAMBLE;

		memcpy(pnOut, &m_sStatusHeader, m_sStatusHeader.nMsglen);

		return;
	}

} /* ClientFuncs */