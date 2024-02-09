/*
 * MPEGTSPktParser.h
 *
 *  Created on: Jul 25, 2013
 *      Author: durga
 */

#ifndef MPEGTSPKTPARSER_H_
#define MPEGTSPKTPARSER_H_

class CMPEGTSPktParser {
public:
    CMPEGTSPktParser();
    virtual ~CMPEGTSPktParser();

    void ParsePacket(const unsigned char* a_pPacketData, int a_nPacketLen);

    bool IsStartCodeFound();
    int  GetTSErrorIndicator();
    int  GetPayloadStartIndicator();
    int  GetTransportPriority();
    int  GetPacketID();
    int  GetScramblingControl();
    bool IsAdaptionFieldPresent();
    bool IsPayloadPresent();
    int  GetContinuityCounter();
    unsigned char  GetAdaptionFieldLength();
    bool IsDiscontinuityFound();
    int  GetRandomAccessIndicator();
    int  GetESPriorityIndicator();
    bool IsPCRFieldPresent();
    bool IsOPCRFieldPresent();
    bool IsSpliceCountdownFieldPresent();
    bool IsPrivateDataPresent();
    bool IsAdaptionFieldExtensionPresent();
    unsigned char  GetStreamID();
    bool IsItAudioStream();
    bool IsItVideoStream();
    unsigned short GetPESPacketLength();
    bool IsPTSPresent();
    bool IsDTSPresent();
    unsigned int GetPESHeaderLength();
    uint64_t GetPTSValue();
    uint64_t GetDTSValue();
    uint64_t GetPCRValue();

    bool UpdatePCRValue(unsigned char* a_pPacketData,
                        uint64_t a_nNewPCR);

    bool UpdatePTSValue(unsigned char* a_pPacketData,
                        uint64_t a_nNewPTS);

    bool UpdateDTSValue(unsigned char* a_pPacketData,
                        uint64_t a_nNewDTS);

    bool UpdateDiscontinuity(unsigned char* a_pPacketData,
                             int a_nDiscontinuity);

private:
    void Init();
    int  FindPESStartCode(const unsigned char *a_pBuff, int a_nSize,
                          int *a_nPES_Start);

    void ParsePacketHeader(const unsigned char *a_pPacketData, int & a_nIndex);
    void ParseAdaptionFieldHeader(const unsigned char *a_pPacketData, int & a_nIndex);
    void ParsePESHeader(const unsigned char *a_pPacketData, int & a_nIndex);
    uint64_t ParseTimeStampValue(const unsigned char *a_pPacketData, int & a_nIndex);

    bool  m_bStartCodeFound;
    int   m_TS_Error_Indicator;
    int   m_Payload_Start_Indicator;
    int   m_Transport_Priority;
    int   m_nPacketID;
    int   m_Scrambling_Control;
    char  m_Adaption_Field_Value;
    bool  m_Adaption_Field_Present;
    bool  m_Payload_Present;
    int   m_Continuity_Counter;
    unsigned char   m_Adaption_Field_Len;
    bool  m_Discontinuity_Indicator;
    int   m_Random_Access_Indicator;
    int   m_ES_Priority_Indicator;
    int   m_PCR_Flag;
    int   m_OPCR_Flag;
    int   m_Splicing_Point_Flag;
    int   m_Private_Data_Flag;
    int   m_Adaption_Field_Extension;
    unsigned char   m_Stream_ID;
    bool  m_bVideoStream;
    bool  m_bAudioStream;
    unsigned short m_PES_Packet_Len;
    bool  m_bPTSPresent;
    bool  m_bDTSPresent;
    unsigned int m_PES_Header_Length;
    uint64_t m_nPTSValue;
    uint64_t m_nDTSValue;
    uint64_t m_nPCRValue;

    //To update PTS/DTS Value
    int   m_nPTSStartIndex;
    int   m_nDTSStartIndex;

    int   m_nDiscontinuityIndex;
    int   m_nPCRIndex;

    int   m_nInPacketCount;
};

#endif /* MPEGTSPKTPARSER_H_ */
