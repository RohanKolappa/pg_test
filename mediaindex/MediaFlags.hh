#ifndef _MEDIA_FLAGS_HH
#define _MEDIA_FLAGS_HH


#define FRAMETYPE_I         0x0001
#define FRAMETYPE_P         0x0002
#define FRAMETYPE_B         0x0004

// video stream type and codecs
#define FRAMETYPE_VIDEO     0x0100
#define FRAMETYPE_MP2V      0x0010
#define FRAMETYPE_MP4V      0x0020
#define FRAMETYPE_H264      0x0040

// audio stream type and codec
#define FRAMETYPE_AUDIO     0x0200
#define FRAMETYPE_MPA       0x0010
#define FRAMETYPE_AAC       0x0020

// AV encapsulations
#define FRAMETYPE_RAW       0x0000
#define FRAMETYPE_MPEGTS    0x1000
#define FRAMETYPE_RTP       0x2000

// V2D Frame types

#define FRAMETYPE_V2D_CTL   0x0001
#define FRAMETYPE_V2D_VIDEO 0x0002
#define FRAMETYPE_V2D_AUDIO 0x0004


// For V2D ENCAPS TYPE
#define FRAMETYPE_V2D_ENCAPS_SHIFT  12
#define FRAMETYPE_V2D_ENCAPS_MASK   (0xF << FRAMETYPE_V2D_ENCAPS_SHIFT)
#define FRAMETYPE_V2D_ENCAPS_V2D    (0 << FRAMETYPE_V2D_ENCAPS_SHIFT)
#define FRAMETYPE_V2D_ENCAPS_RTP    (2 << FRAMETYPE_V2D_ENCAPS_SHIFT)

// For V2D CHANNEL TYPE
#define FRAMETYPE_V2D_CHAN_SHIFT    0
#define FRAMETYPE_V2D_CHAN_MASK     0xF
#define FRAMETYPE_V2D_CHAN_CTL      1
#define FRAMETYPE_V2D_CHAN_VIDEO    2
#define FRAMETYPE_V2D_CHAN_AUDIO    4

// For RTP ENCAPS TYPE
#define FRAMETYPE_RTP_ENCAPS_SHIFT  12
#define FRAMETYPE_RTP_ENCAPS_MASK   (0xF << FRAMETYPE_RTP_ENCAPS_SHIFT)
#define FRAMETYPE_RTP_ENCAPS_RTP    (2 << FRAMETYPE_RTP_ENCAPS_SHIFT)

// For RTP CHANNEL TYPE
#define FRAMETYPE_RTP_CHAN_SHIFT    8
#define FRAMETYPE_RTP_CHAN_MASK     (0xF << FRAMETYPE_RTP_CHAN_SHIFT)
#define FRAMETYPE_RTP_CHAN_VIDEO    (1 << FRAMETYPE_RTP_CHAN_SHIFT)
#define FRAMETYPE_RTP_CHAN_AUDIO    (2 << FRAMETYPE_RTP_CHAN_SHIFT)

// For WIS ENCAPS TYPE
#define FRAMETYPE_WIS_ENCAPS_SHIFT  12
#define FRAMETYPE_WIS_ENCAPS_MASK   (0xF << FRAMETYPE_WIS_ENCAPS_SHIFT)
#define FRAMETYPE_WIS_ENCAPS_RAW    (0 << FRAMETYPE_WIS_ENCAPS_SHIFT)
#define FRAMETYPE_WIS_ENCAPS_MPEGTS (1 << FRAMETYPE_WIS_ENCAPS_SHIFT)

// For WIS CHANNEL TYPE
#define FRAMETYPE_WIS_CHAN_SHIFT    8
#define FRAMETYPE_WIS_CHAN_MASK     (0xF << FRAMETYPE_WIS_CHAN_SHIFT)
#define FRAMETYPE_WIS_CHAN_VIDEO    (1 << FRAMETYPE_WIS_CHAN_SHIFT)
#define FRAMETYPE_WIS_CHAN_AUDIO    (2 << FRAMETYPE_WIS_CHAN_SHIFT)

// For WIS VIDEO CODEC TYPE
#define FRAMETYPE_WIS_CODEC_SHIFT   4
#define FRAMETYPE_WIS_CODEC_MASK    (0xF << FRAMETYPE_WIS_CODEC_SHIFT)
#define FRAMETYPE_WIS_CODEC_MP2V    (1 << FRAMETYPE_WIS_CODEC_SHIFT)
#define FRAMETYPE_WIS_CODEC_MP4V    (2 << FRAMETYPE_WIS_CODEC_SHIFT)

// For WIS AUDIO CODEC TYPE
#define FRAMETYPE_WIS_CODEC_MPA     (1 << FRAMETYPE_WIS_CODEC_SHIFT)
#define FRAMETYPE_WIS_CODEC_AAC     (2 << FRAMETYPE_WIS_CODEC_SHIFT)

#define FRAME_VERSION_SHIFT     28
#define FRAME_VERSION_MASK      (0xF << FRAME_VERSION_SHIFT)
#define STREAM_TYPE_SHIFT       20
#define STREAM_TYPE_MASK        (0xFF << STREAM_TYPE_SHIFT)
#define STREAM_SUB_TYPE_SHIFT   16
#define STREAM_SUB_TYPE_MASK    (0xF << STREAM_SUB_TYPE_SHIFT)

// mask bits for test ops
#define FRAMETYPE_ENCAPS_MASK  0xF000
#define FRAMETYPE_AV_MASK      0x0F00
#define FRAMETYPE_CODEC_MASK   0x00F0
#define FRAMETYPE_AVCODEC_MASK 0x0FF0
#define FRAMETYPE_VSEQ_MASK    0x000F

// Stream Types
#define STREAMTYPE_WIS         1
#define STREAMTYPE_V2D         2
#define STREAMTYPE_RTP         3
#define STREAMTYPE_UDP         4

// Backwards compatibility note:
// Before Version 2.2 we had frame version 0
// Since Version 2.2 we have frame version 1
// We have room for 16 possible versions in software lifetime
#define FRAMEVERSION_0         0
#define FRAMEVERSION_1         1

// defines for compatability with old versions of oaserver
// should only be used by wisrx to convert to standard defines as above
#define FRAMETYPE_OA_I         0x00000000
#define FRAMETYPE_OA_P         0x01000000
#define FRAMETYPE_OA_B         0x02000000
#define FRAMETYPE_OA_AUDIO     0x04000000
#define FRAMETYPE_OA_AAC       0x08000000
#define FRAMETYPE_OA_H264      0x10000000


#define IMAGETYPE_UNKNOWN  0
#define IMAGETYPE_JPEG     1
#define IMAGETYPE_PNG      2
#define IMAGETYPE_BMP      3
#define IMAGETYPE_GIF      4
#define IMAGETYPE_TIFF     5

#define IMAGETYPE_MASK     0xFF

#define TNTYPE_STREAM    0x0000
#define TNTYPE_PRIMARY   0x0100

#define TNTYPE_MASK      0xFF00

class CMediaFlag {
public:

    CMediaFlag() : m_bFlag(0) {};
    CMediaFlag(unsigned long a_nFlag) : m_bFlag(a_nFlag) {};
    ~CMediaFlag() {};

    unsigned long GetMediaFlag() const {
        return m_bFlag;
    }

    void SetMediaFlag(const unsigned long a_nFlag) {
        m_bFlag = a_nFlag;
    }

private:
    CMediaFlag(const CMediaFlag &) {};

protected:
    unsigned long m_bFlag;

};

class CStreamMediaFlag : public CMediaFlag {
public:
    CStreamMediaFlag() {
        m_bFlag = ((m_bFlag & ~FRAME_VERSION_MASK) | (FRAMEVERSION_1 << FRAME_VERSION_SHIFT));
    };
    CStreamMediaFlag(unsigned long a_nFlag) : CMediaFlag(a_nFlag) {};

	virtual ~CStreamMediaFlag() {};

    unsigned long GetFrameVersion() {return ((m_bFlag >> FRAME_VERSION_SHIFT) &0xF); };
    unsigned long GetStreamType() {return ((m_bFlag >> STREAM_TYPE_SHIFT) &0xFF); };
    unsigned long GetStreamSubType() {return ((m_bFlag >> STREAM_SUB_TYPE_SHIFT) &0xF); };
    virtual short GetEncaps() const {return 0; };
    virtual short GetChannel() const {return 0; };

private:
    CStreamMediaFlag(const CStreamMediaFlag &) : CMediaFlag() {};
};

class CV2DStreamMediaFlag : public CStreamMediaFlag {
public:
    CV2DStreamMediaFlag() {
        m_bFlag = ((((((((m_bFlag & ~STREAM_TYPE_MASK) |
                    (STREAMTYPE_V2D << STREAM_TYPE_SHIFT)) &
                    ~STREAM_SUB_TYPE_MASK) |
                    (0 << STREAM_SUB_TYPE_SHIFT)) &
                    ~ FRAMETYPE_V2D_ENCAPS_MASK) | FRAMETYPE_V2D_ENCAPS_V2D) &
                    ~FRAMETYPE_V2D_CHAN_MASK) | FRAMETYPE_V2D_CHAN_CTL);
    };

    CV2DStreamMediaFlag(int a_nEncaps, int a_nChannel) {
        m_bFlag = ((((((((m_bFlag & ~STREAM_TYPE_MASK) |
                    (STREAMTYPE_V2D << STREAM_TYPE_SHIFT)) &
                    ~STREAM_SUB_TYPE_MASK) |
                    (0 << STREAM_SUB_TYPE_SHIFT)) &
                    ~ FRAMETYPE_V2D_ENCAPS_MASK) | a_nEncaps) &
                    ~FRAMETYPE_V2D_CHAN_MASK) | a_nChannel);
    };

    CV2DStreamMediaFlag(unsigned long a_nFlag) : CStreamMediaFlag(a_nFlag) {};
    virtual ~CV2DStreamMediaFlag() {};

    short GetEncaps() const {
        return m_bFlag & FRAMETYPE_V2D_ENCAPS_MASK;
    }

    short GetChannel() const {
        return m_bFlag & FRAMETYPE_V2D_CHAN_MASK;
    }

    void SetEncaps(int val) {
        SetMediaFlag( (m_bFlag & ~FRAMETYPE_V2D_ENCAPS_MASK) | ((val) & FRAMETYPE_V2D_ENCAPS_MASK));
    }

    void SetChannel(int val) {
        SetMediaFlag( (m_bFlag & ~FRAMETYPE_V2D_CHAN_MASK) | ((val) & FRAMETYPE_V2D_CHAN_MASK));
    }

private:
    CV2DStreamMediaFlag(const CV2DStreamMediaFlag &) : CStreamMediaFlag() {};
};


class CRTPStreamMediaFlag : public CStreamMediaFlag {
public:
    CRTPStreamMediaFlag() {
        m_bFlag = ((((((((m_bFlag & ~STREAM_TYPE_MASK) |
                    (STREAMTYPE_RTP << STREAM_TYPE_SHIFT)) &
                    ~STREAM_SUB_TYPE_MASK) |
                    (0 << STREAM_SUB_TYPE_SHIFT)) &
                    ~ FRAMETYPE_RTP_ENCAPS_MASK) | FRAMETYPE_RTP_ENCAPS_RTP) &
                    ~FRAMETYPE_RTP_CHAN_MASK) | FRAMETYPE_RTP_CHAN_VIDEO);
    };
    CRTPStreamMediaFlag(int a_nEncaps, int a_nChannel) {
        m_bFlag = ((((((((m_bFlag & ~STREAM_TYPE_MASK) |
                    (STREAMTYPE_RTP << STREAM_TYPE_SHIFT)) &
                    ~STREAM_SUB_TYPE_MASK) |
                    (0 << STREAM_SUB_TYPE_SHIFT)) &
                    ~ FRAMETYPE_RTP_ENCAPS_MASK) | a_nEncaps) &
                    ~FRAMETYPE_RTP_CHAN_MASK) | a_nChannel);
    };

    CRTPStreamMediaFlag(unsigned long a_nFlag) : CStreamMediaFlag(a_nFlag) {};
    virtual ~CRTPStreamMediaFlag() {};

    short GetEncaps() const {
        return m_bFlag & FRAMETYPE_RTP_ENCAPS_MASK;
    }

    short GetChannel() const {
        return m_bFlag & FRAMETYPE_RTP_CHAN_MASK;
    }

    void SetEncaps(int val) {
        SetMediaFlag( (m_bFlag & ~FRAMETYPE_RTP_ENCAPS_MASK) | ((val) & FRAMETYPE_RTP_ENCAPS_MASK));
    }

    void SetChannel(int val) {
        SetMediaFlag( (m_bFlag & ~FRAMETYPE_RTP_CHAN_MASK) | ((val) & FRAMETYPE_RTP_CHAN_MASK));
    }

private:
    CRTPStreamMediaFlag(const CRTPStreamMediaFlag &) : CStreamMediaFlag() {};
};

class CWISStreamMediaFlag : public CStreamMediaFlag {
public:
    CWISStreamMediaFlag() {
        m_bFlag = ((((((((m_bFlag & ~STREAM_TYPE_MASK) |
                    (STREAMTYPE_RTP << STREAM_TYPE_SHIFT)) &
                    ~STREAM_SUB_TYPE_MASK) |
                    (0 << STREAM_SUB_TYPE_SHIFT)) &
                    ~ FRAMETYPE_WIS_ENCAPS_MASK) | FRAMETYPE_WIS_ENCAPS_RAW) &
                    ~FRAMETYPE_WIS_CHAN_MASK) | FRAMETYPE_WIS_CHAN_VIDEO);
    };

    CWISStreamMediaFlag(int a_nEncaps, int a_nChannel) {
        m_bFlag = ((((((((m_bFlag & ~STREAM_TYPE_MASK) |
                    (STREAMTYPE_RTP << STREAM_TYPE_SHIFT)) &
                    ~STREAM_SUB_TYPE_MASK) |
                    (0 << STREAM_SUB_TYPE_SHIFT)) &
                    ~ FRAMETYPE_WIS_ENCAPS_MASK) | a_nEncaps) &
                    ~FRAMETYPE_WIS_CHAN_MASK) | a_nChannel);
    };
    CWISStreamMediaFlag(unsigned long a_nFlag) : CStreamMediaFlag(a_nFlag) {};
    //CWISStreamMediaFlag(const CWISStreamMediaFlag &wisSteamMediaFlag) : CStreamMediaFlag(wisSteamMediaFlag) {};
    virtual ~CWISStreamMediaFlag() {};

    short GetEncaps() const {
        return m_bFlag & FRAMETYPE_WIS_ENCAPS_MASK;
    }

    short GetChannel() const {
        return m_bFlag & FRAMETYPE_WIS_CHAN_MASK;
    }

    short GetCodec() const {
        return m_bFlag & FRAMETYPE_WIS_CODEC_MASK;
    }

    void SetEncaps(int val) {
        SetMediaFlag( (m_bFlag & ~FRAMETYPE_WIS_ENCAPS_MASK) | ((val) & FRAMETYPE_WIS_ENCAPS_MASK));
    }

    void SetChannel(int val) {
        SetMediaFlag( (m_bFlag & ~FRAMETYPE_WIS_CHAN_MASK) | ((val) & FRAMETYPE_WIS_CHAN_MASK));
    }

    void SetCodec(int val) {
        SetMediaFlag( (m_bFlag & ~FRAMETYPE_WIS_CODEC_MASK) | ((val) & FRAMETYPE_WIS_CODEC_MASK));
    }

private:
    CWISStreamMediaFlag(const CWISStreamMediaFlag &) : CStreamMediaFlag() {};
};
#endif // _MEDIA_FLAGS_HH
