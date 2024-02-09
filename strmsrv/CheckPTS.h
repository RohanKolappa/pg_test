/*
 * CCheckPTS.h
 *
 *  Created on: Jun 4, 2013
 *      Author: durga
 */

#ifndef CCHECKPTS_H_
#define CCHECKPTS_H_

#include <iostream>

class AVCodecContext;
class AVFormatContext;

class CCheckPTS {
public:
    CCheckPTS(std::string a_strInputFile);
    virtual ~CCheckPTS();

    void RetrievePTSAndDTS(bool a_bVideo = true, bool a_bAudio = true);

private:
    void Initialize();

    AVFormatContext *m_pInputFmtContext;

    AVCodecContext *m_pVideoCodecCtx;
    AVCodecContext *m_pAudioCodecCtx;

    unsigned int m_nVideoIndex;
    unsigned int m_nAudioIndex;

    std::string m_strInputFileName;
};

#endif /* CCHECKPTS_H_ */
