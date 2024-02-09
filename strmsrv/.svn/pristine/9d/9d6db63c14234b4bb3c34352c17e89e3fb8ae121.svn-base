/*
 * MPEGTSPktParser.cpp
 *
 *  Created on: Jul 25, 2013
 *      Author: durga
 */

#include "strmsrv_compatibility.h"
#include "MPEGTSPktParser.hh"

CMPEGTSPktParser::CMPEGTSPktParser()
{
    Init();
    m_nInPacketCount = 0;
}

CMPEGTSPktParser::~CMPEGTSPktParser()
{
}

void CMPEGTSPktParser::Init()
{
    m_bStartCodeFound = false;
    m_TS_Error_Indicator = 0;
    m_Payload_Start_Indicator = 0;
    m_Transport_Priority = 0;
    m_nPacketID = 0;
    m_Scrambling_Control = 0;
    m_Adaption_Field_Value = 0;
    m_Adaption_Field_Present = false;
    m_Payload_Present = false;
    m_Continuity_Counter = 0;
    m_Adaption_Field_Len = 0;
    m_Discontinuity_Indicator = false;
    m_Random_Access_Indicator = 0;
    m_ES_Priority_Indicator = 0;
    m_PCR_Flag = 0;
    m_OPCR_Flag = 0;
    m_Splicing_Point_Flag = 0;
    m_Private_Data_Flag = 0;
    m_Adaption_Field_Extension = 0;
    m_Stream_ID = 0;
    m_bVideoStream = false;
    m_bAudioStream = false;
    m_PES_Packet_Len = 0;
    m_bPTSPresent = false;
    m_bDTSPresent = false;
    m_PES_Header_Length = 0;
    m_nPTSValue = 0;
    m_nDTSValue = 0;
    m_nPCRValue = 0;

    m_nPTSStartIndex = -1;
    m_nDTSStartIndex = -1;
    m_nDiscontinuityIndex = -1;
    m_nPCRIndex = -1;
}

void CMPEGTSPktParser::ParsePacket(const unsigned char* a_pPacketData,
                                   int a_nPacketLen)
{
    Init();

    if (a_pPacketData == NULL ||
            a_nPacketLen <= 0)
        return;

    m_nInPacketCount++;

    int nIndex = 0;

    ParsePacketHeader(a_pPacketData, nIndex);

    ParseAdaptionFieldHeader(a_pPacketData, nIndex);

    ParsePESHeader(a_pPacketData, nIndex);
}

void CMPEGTSPktParser::ParsePacketHeader(const unsigned char *a_pPacketData,
                                         int & a_nIndex)
{
    char start_code = (a_pPacketData[a_nIndex]); //8-bits should be 0x47
    a_nIndex++;

    if (start_code == 0x47) {
        m_bStartCodeFound = true;
    }

    m_TS_Error_Indicator = ((a_pPacketData[a_nIndex] >> 7) & 0x01); // 1-bit

    m_Payload_Start_Indicator = ((a_pPacketData[a_nIndex] >> 6) & 0x01); // 1-bit. 1 means start of PES or PSI otherwise zero only.

    m_Transport_Priority = ((a_pPacketData[a_nIndex] >> 5) & 0x01); // 1-bit. 1 means higher priority than other packets with the same PID.

    m_nPacketID = ((a_pPacketData[a_nIndex] & 0x1F) << 8); //13-bits. Packet ID
    a_nIndex++;
    m_nPacketID |=  (a_pPacketData[a_nIndex]);
    a_nIndex++;

    m_Scrambling_Control = ((a_pPacketData[a_nIndex] >> 6) & 0x03); // 2-bits. '00'= Not scrambled. '01'= Reserved.'10'=Scrambled with even key. '11'=Scrambled with odd key.

    m_Adaption_Field_Value = ((a_pPacketData[a_nIndex] & 0x30)) >> 4; //2-bits. 01 = No adaption fields, payload only. 10 = adaption field only
                                                                   // 11 = Adaption field and payload
    if (m_Adaption_Field_Value == 1) {
        m_Payload_Present = true;
        m_Adaption_Field_Present = false;
    }
    else if (m_Adaption_Field_Value == 2) {
        m_Payload_Present = false;
        m_Adaption_Field_Present = true;
    }
    else if (m_Adaption_Field_Value == 3) {
        m_Payload_Present = true;
        m_Adaption_Field_Present = true;
    }
    else {
        printf("Error while parsing mpegts adaption field.\n");//Error should never happen.
    }

    m_Continuity_Counter = (a_pPacketData[a_nIndex] & 0x0F); // 4-bits
    a_nIndex++;
}

void CMPEGTSPktParser::ParseAdaptionFieldHeader(const unsigned char *a_pPacketData,
                                                int & a_nIndex)
{
    if (m_Adaption_Field_Present == false)
        return;

    m_Adaption_Field_Len = a_pPacketData[a_nIndex]; //8-bits
    a_nIndex++;

    m_nDiscontinuityIndex = a_nIndex;

    m_Discontinuity_Indicator = ((a_pPacketData[a_nIndex] & 0x80) >> 7); //1-bit
    m_Random_Access_Indicator = ((a_pPacketData[a_nIndex] & 0x40) >> 6); //1-bit
    m_ES_Priority_Indicator = ((a_pPacketData[a_nIndex] & 0x20) >> 5);   //1-bit
    m_PCR_Flag  = ((a_pPacketData[a_nIndex] & 0x10) >> 4);               //1-bit
    m_OPCR_Flag  = ((a_pPacketData[a_nIndex] & 0x08) >> 3);              //1-bit
    m_Splicing_Point_Flag = ((a_pPacketData[a_nIndex] & 0x04) >> 2);     //1-bit
    m_Private_Data_Flag = ((a_pPacketData[a_nIndex] & 0x02) >> 1);       //1-bit
    m_Adaption_Field_Extension = (a_pPacketData[a_nIndex] & 0x01);       //1-bit
    a_nIndex++;

    if (m_PCR_Flag == true) {
        //PCRValue == ?? //48-bits
        m_nPCRIndex = a_nIndex;
        uint64_t nP1 = 0;
        nP1 = (a_pPacketData[a_nIndex]); //8-bits.
        a_nIndex++;

        uint64_t nP2 = 0;
        nP2 = (a_pPacketData[a_nIndex] << 8); //16-bits.
        a_nIndex++;
        nP2 |=  (a_pPacketData[a_nIndex]);
        a_nIndex++;

        uint64_t nP3 = 0;
        nP3 = (a_pPacketData[a_nIndex] << 8); //9-bits
        a_nIndex++;
        nP3 |= (a_pPacketData[a_nIndex] & 0x80);
        a_nIndex++;

        m_nPCRValue = (((nP1 << 25) | (nP2 << 9)) | (nP3 >> 7));
    }

    if (m_OPCR_Flag == true) {
        //OPCRValue = ?? //48-bits.
        a_nIndex += 5;
    }
}

void CMPEGTSPktParser::ParsePESHeader(const unsigned char *a_pPacketData,
                                      int & a_nIndex)
{
    if (m_Payload_Start_Indicator != 1)
        return;

    int nStart = 0;
    if ((FindPESStartCode(&a_pPacketData[a_nIndex], (188 - (8 * a_nIndex)), &nStart)) > 0) {
        a_nIndex += nStart; // 3 bytes -- 0x000001

        m_Stream_ID = a_pPacketData[a_nIndex]; // 8-bits
        a_nIndex++;
        if (m_Stream_ID >= 0xC0 && m_Stream_ID <= 0xDF) {
            m_bVideoStream = false;
            m_bAudioStream = true;
        }
        else if (m_Stream_ID >= 0xE0 && m_Stream_ID <= 0xEF) {
            m_bVideoStream = true;
            m_bAudioStream = false;
        }

        m_PES_Packet_Len = (a_pPacketData[a_nIndex] << 8); //16-bits
        a_nIndex++;
        m_PES_Packet_Len |= (a_pPacketData[a_nIndex]);
        a_nIndex++;

        /*char nReserved = (a_pPacketData[a_nIndex] & 0xC0) >> 6; //2-bits.
        char PES_Scrambling_Control = (a_pPacketData[a_nIndex] & 0x30) >> 4; //2-bits.
        char PES_Priority = (a_pPacketData[a_nIndex] & 0x08) >> 3; //1-bit.
        char Data_Alignment_Indicator = (a_pPacketData[a_nIndex] & 0x04) >> 2; //1-bit.
        char Copyright = (a_pPacketData[a_nIndex] & 0x02) >> 1; //1-bit.
        char Original_Or_Copy = (a_pPacketData[a_nIndex] & 0x01); //1-bit.*/
        a_nIndex++;

        char PTS_DTS_Flag = ((a_pPacketData[a_nIndex] & 0xC0) >> 6); //2-bits
        if (PTS_DTS_Flag == 3) {
            m_bPTSPresent = true;
            m_bDTSPresent = true;
        }
        else if (PTS_DTS_Flag == 2) {
            m_bPTSPresent = true;
            m_bDTSPresent = false;
        }
        else {
            m_bPTSPresent = false;
            m_bDTSPresent = false;
        }

        /*char ESCR_Flag = ((a_pPacketData[a_nIndex] & 0x20) >> 5); //1-bit
        char ES_Rate_Flag = ((a_pPacketData[a_nIndex] & 0x10) >> 4); //1-bit
        char DMS_Trick_Mode_Flag = ((a_pPacketData[a_nIndex] & 0x08) >> 3); //1-bit
        char Additional_Copy_Info_Flag = ((a_pPacketData[a_nIndex] & 0x04) >> 2); //1-bit
        char PES_CRC_Flag = ((a_pPacketData[a_nIndex] & 0x02) >> 1); // 1-bit
        char PES_Extension_Flag = (a_pPacketData[a_nIndex] & 0x01); //1-bit*/
        a_nIndex++;

        m_PES_Header_Length = a_pPacketData[a_nIndex]; //8-bits.
        a_nIndex++;

        //char Reserved = a_pPacketData[a_nIndex] & 0xF0;//4-bits

        if (m_bPTSPresent) {
            m_nPTSStartIndex = a_nIndex;
            m_nPTSValue = ParseTimeStampValue(a_pPacketData, a_nIndex);
        }

        if (m_bDTSPresent) {
            m_nDTSStartIndex = a_nIndex;
            m_nDTSValue = ParseTimeStampValue(a_pPacketData, a_nIndex);
        }
    }
}

uint64_t CMPEGTSPktParser::ParseTimeStampValue(const unsigned char *a_pPacketData,
                                               int & a_nIndex)
{
    uint64_t nP1 = 0;
    nP1 = (a_pPacketData[a_nIndex] & 0x0E) >> 1; //3-bits. PTS/DTS_32_30

    //char MarkerBit = a_pPacketData[nIndex] & 0x01;//1-bit Marker bit.
    a_nIndex++;

    uint64_t nP2 = 0;
    nP2 = (a_pPacketData[a_nIndex] << 8); //15-bits. PTS/DTS_29_15.
    a_nIndex++;
    nP2 |=  (a_pPacketData[a_nIndex] & 0xFE);

    //char MarkerBit2 = (a_pPacketData[nIndex] & 0x01);//1-bit
    a_nIndex++;

    uint64_t nP3 = 0;
    nP3 = (a_pPacketData[a_nIndex] << 8); //15-bits PTS/DTS_14_0
    a_nIndex++;
    nP3 |= (a_pPacketData[a_nIndex] & 0xFE);

    //char MarkerBit3 = (a_pPacketData[nIndex] & 0x01);
    a_nIndex++;

    uint64_t nTSValue = (((nP1 << 30) | (nP2 << 14)) | (nP3 >> 1));

    return nTSValue;
}

bool CMPEGTSPktParser::UpdatePTSValue(unsigned char* a_pPacketData,
                                      uint64_t a_nNewPTS)
{
    if (m_nPTSStartIndex == -1)
        return false;

    int nIndex = m_nPTSStartIndex;

    unsigned char PTS_33_40 = (((a_nNewPTS >> 29) & 0x0E) | (a_pPacketData[nIndex] & 0xF1));
    a_pPacketData[nIndex] = PTS_33_40;
    nIndex++;

    unsigned char PTS_25_32 = (a_nNewPTS >> 22);
    a_pPacketData[nIndex] = PTS_25_32;
    nIndex++;

    unsigned char PTS_17_24 = (((a_nNewPTS >> 14) & 0xFE) | (a_pPacketData[nIndex] & 0x01));
    a_pPacketData[nIndex] = PTS_17_24;
    nIndex++;

    unsigned char PTS_9_16 = (a_nNewPTS >> 7);
    a_pPacketData[nIndex] = PTS_9_16;
    nIndex++;

    unsigned char PTS_0_8 = ((a_nNewPTS << 1) | (a_pPacketData[nIndex] & 0x01));
    a_pPacketData[nIndex] = PTS_0_8;
    nIndex++;

    return true;
}

bool CMPEGTSPktParser::UpdateDTSValue(unsigned char* a_pPacketData,
                                      uint64_t a_nNewDTS)
{
    if (m_nDTSStartIndex == -1)
        return false;

    int nIndex = m_nDTSStartIndex;

    unsigned char PTS_33_40 = (((a_nNewDTS >> 29) & 0x0E) | (a_pPacketData[nIndex] & 0xF1));
    a_pPacketData[nIndex] = PTS_33_40;
    nIndex++;

    unsigned char PTS_25_32 = (a_nNewDTS >> 22);
    a_pPacketData[nIndex] = PTS_25_32;
    nIndex++;

    unsigned char PTS_17_24 = (((a_nNewDTS >> 14) & 0xFE) | (a_pPacketData[nIndex] & 0x01));
    a_pPacketData[nIndex] = PTS_17_24;
    nIndex++;

    unsigned char PTS_9_16 = (a_nNewDTS >> 7);
    a_pPacketData[nIndex] = PTS_9_16;
    nIndex++;

    unsigned char PTS_0_8 = ((a_nNewDTS << 1) | (a_pPacketData[nIndex] & 0x01));
    a_pPacketData[nIndex] = PTS_0_8;
    nIndex++;

    return true;
}

bool CMPEGTSPktParser::UpdateDiscontinuity(unsigned char* a_pPacketData,
                                           int a_nDiscontinuity)
{
    if (m_nDiscontinuityIndex == -1)
        return false;

    int nDiscontinuity_Indicator = ((a_pPacketData[m_nDiscontinuityIndex] & 0x80) >> 7); //1-bit

    if (nDiscontinuity_Indicator != a_nDiscontinuity) {
        a_pPacketData[m_nDiscontinuityIndex] = ((a_pPacketData[m_nDiscontinuityIndex] & 0x7F) | (0x80));
    }

    nDiscontinuity_Indicator = ((a_pPacketData[m_nDiscontinuityIndex] & 0x80) >> 7); //1-bit

    return true;
}

bool CMPEGTSPktParser::UpdatePCRValue(unsigned char* a_pPacketData,
                                      uint64_t a_nNewPCR)
{
    if (m_nPCRIndex <= -1)
        return false;

    int nIndex = m_nPCRIndex;

    unsigned char Ch5 = (a_nNewPCR >> 25);
    a_pPacketData[nIndex] = Ch5;
    nIndex++;

    unsigned char Ch4 = (a_nNewPCR >> 17);
    a_pPacketData[nIndex] = Ch4;
    nIndex++;

    unsigned char Ch3 = (a_nNewPCR >> 9);
    a_pPacketData[nIndex] = Ch3;
    nIndex++;

    unsigned char Ch2 = (a_nNewPCR >> 1);
    a_pPacketData[nIndex] = Ch2;
    nIndex++;

    unsigned char Ch1 = (a_nNewPCR & 0x01);
    a_pPacketData[nIndex] = (((a_pPacketData[nIndex] & 0x7F) | (Ch1 << 7)));
    nIndex++;

    return true;
}

int CMPEGTSPktParser::FindPESStartCode(const unsigned char *a_pBuff, int a_nSize,
                                       int *a_nPES_Start)
{
    int i = 0;
    *a_nPES_Start = 0;

    while((a_pBuff[i] != 0 || a_pBuff[i + 1] != 0 || a_pBuff[i + 2] != 0x01) &&
          (a_pBuff[i] != 0 || a_pBuff[i + 1] != 0 || a_pBuff[i + 2] != 0 ||
                  a_pBuff[i + 3] != 0x01)) {
        i++;
        if (i + 4 >= a_nSize) {
            return 0; //Did not find PES start.
        }
    }

    if (a_pBuff[i] != 0 || a_pBuff[i + 1] != 0 || a_pBuff[i + 2] != 0x01) {
        i++;
    }

    if (a_pBuff[i] != 0 || a_pBuff[i + 1] != 0 || a_pBuff[i + 2] != 0x01) {
        printf("There is some error. Should never happen. \n");
        return 0;//Error, should never happen.
    }

    i += 3;
    *a_nPES_Start = i;

    while((a_pBuff[i] != 0 || a_pBuff[i + 1] != 0 || a_pBuff[i + 2] != 0) &&
          (a_pBuff[i] != 0 || a_pBuff[i + 1] != 0 || a_pBuff[i + 2] != 0x01)) {
        i++;

        if(i + 3 >= a_nSize) {
            return 0;
        }
    }

    return (*a_nPES_Start);
}

bool CMPEGTSPktParser::IsStartCodeFound()
{
    return m_bStartCodeFound;
}

int CMPEGTSPktParser::GetTSErrorIndicator()
{
    return m_TS_Error_Indicator;
}

int CMPEGTSPktParser::GetPayloadStartIndicator()
{
    return m_Payload_Start_Indicator;
}

int CMPEGTSPktParser::GetTransportPriority()
{
    return m_Transport_Priority;
}

int CMPEGTSPktParser::GetPacketID()
{
    return m_nPacketID;
}

int CMPEGTSPktParser::GetScramblingControl()
{
    return m_Scrambling_Control;
}

bool CMPEGTSPktParser::IsAdaptionFieldPresent()
{
    return m_Adaption_Field_Present;
}

bool CMPEGTSPktParser::IsPayloadPresent()
{
    return m_Payload_Present;
}

int CMPEGTSPktParser::GetContinuityCounter()
{
    return m_Continuity_Counter;
}

unsigned char CMPEGTSPktParser::GetAdaptionFieldLength()
{
    return m_Adaption_Field_Len;
}

bool CMPEGTSPktParser::IsDiscontinuityFound()
{
    return m_Discontinuity_Indicator;
}

int CMPEGTSPktParser::GetRandomAccessIndicator()
{
    return m_Random_Access_Indicator;
}

int CMPEGTSPktParser::GetESPriorityIndicator()
{
    return m_ES_Priority_Indicator;
}

bool CMPEGTSPktParser::IsPCRFieldPresent()
{
    return m_PCR_Flag;
}

bool CMPEGTSPktParser::IsOPCRFieldPresent()
{
    return m_OPCR_Flag;
}

bool CMPEGTSPktParser::IsSpliceCountdownFieldPresent()
{
    return m_Splicing_Point_Flag;
}

bool CMPEGTSPktParser::IsPrivateDataPresent()
{
    return m_Private_Data_Flag;
}

bool CMPEGTSPktParser::IsAdaptionFieldExtensionPresent()
{
    return m_Adaption_Field_Extension;
}

unsigned char CMPEGTSPktParser::GetStreamID()
{
    return m_Stream_ID;
}

bool CMPEGTSPktParser::IsItVideoStream()
{
    return m_bVideoStream;
}

bool CMPEGTSPktParser::IsItAudioStream()
{
    return m_bAudioStream;
}

unsigned short CMPEGTSPktParser::GetPESPacketLength()
{
    return m_PES_Packet_Len;
}

bool CMPEGTSPktParser::IsPTSPresent()
{
    return m_bPTSPresent;
}

bool CMPEGTSPktParser::IsDTSPresent()
{
    return m_bDTSPresent;
}

unsigned int CMPEGTSPktParser::GetPESHeaderLength()
{
    return m_PES_Header_Length;
}

uint64_t CMPEGTSPktParser::GetPTSValue()
{
    return m_nPTSValue;
}

uint64_t CMPEGTSPktParser::GetDTSValue()
{
    return m_nDTSValue;
}

uint64_t CMPEGTSPktParser::GetPCRValue()
{
    return m_nPCRValue;
}
