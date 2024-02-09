#ifndef KBMIPCUTILS_HH_
#define KBMIPCUTILS_HH_

#define KBSFIFOPATH "/tmp/keyboardfifofd"
#define ABSMSFIFOPATH "/tmp/mouseabsfd"

class CKbmIpc {
public:
    CKbmIpc();
    ~CKbmIpc();

    int m_nKeyboardfifofd;
    int m_nMouseAbsfifofd;

    int GetKbData(unsigned int *key, unsigned int *down);
    int GetMsData(int *buttonMask, int *VncAbsWidth, int *VncAbsHeight);
    int GetScreenWidth();
    int GetScreenHeight();

private:
    int m_nWidth;
    int m_nHeight;

    void InitializeVariables();
    void OpenFIFOFile();
    void CloseFIFOFile();
    void FlushKBMData(int fd);

};


#endif /* KBMIPCUTILS_HH_ */
