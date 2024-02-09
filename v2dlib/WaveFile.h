
#if !defined __WAVE_FILE_HH__
#define __WAVE_FILE_HH__

#if (defined(__WIN32__) || defined(_WIN32))
#define WINDOWS
#else
#ifdef WINDOWS
#undef WINDOWS
#endif
#endif

#ifdef WINDOWS

#include <windows.h>
#include <assert.h>
#include <wchar.h>
#include <mmsystem.h>
#include <commctrl.h> // for InitCommonControls() 
#include <shellapi.h> // for ExtractIcon()
#include <math.h>      
#include <ctime>     
#include <limits.h>      
#include <stdio.h>
//-----------------------------------------------------------------------------
// Name: class CWaveFile
// Desc: Encapsulates reading or writing sound data to or from a wave file
//-----------------------------------------------------------------------------
class CWaveFile
{
public:
    WAVEFORMATEX* m_pwfx;        // Pointer to WAVEFORMATEX structure
    HMMIO         m_hmmio;       // MM I/O handle for the WAVE
    MMCKINFO      m_ck;          // Multimedia RIFF chunk
    MMCKINFO      m_ckRiff;      // Use in opening a WAVE file
    DWORD         m_dwSize;      // The size of the wave file
    MMIOINFO      m_mmioinfoOut;
    DWORD         m_dwFlags;
    BOOL          m_bIsReadingFromMemory;
    BYTE*         m_pbData;
    BYTE*         m_pbDataCur;
    ULONG         m_ulDataSize;
    CHAR*         m_pResourceBuffer;

protected:
    HRESULT ReadMMIO();
    HRESULT WriteMMIO( WAVEFORMATEX *pwfxDest );

public:
    CWaveFile();
    ~CWaveFile();

    HRESULT Open( LPWSTR strFileName, WAVEFORMATEX* pwfx, DWORD dwFlags );
    HRESULT OpenFromMemory( BYTE* pbData, ULONG ulDataSize, WAVEFORMATEX* pwfx, DWORD dwFlags );
    HRESULT Close();

    HRESULT Read( BYTE* pBuffer, DWORD dwSizeToRead, DWORD* pdwSizeRead );
    HRESULT Write( UINT nSizeToWrite, BYTE* pbData, UINT* pnSizeWrote );

    DWORD   GetSize();
    HRESULT ResetFile();
    WAVEFORMATEX* GetFormat();
};
#else

typedef long                HRESULT;
typedef wchar_t*            LPWSTR;
typedef unsigned short      WORD;
typedef unsigned long       DWORD;
typedef unsigned char       BYTE;
typedef unsigned int        UINT;
typedef unsigned long       ULONG;

#define WAVE_FORMAT_PCM 1
#define FAILED(hr) (((HRESULT)(hr)) < 0)

typedef struct tWAVEFORMATEX
{
    WORD        wFormatTag;         
    WORD        nChannels;          
    DWORD       nSamplesPerSec;     
    DWORD       nAvgBytesPerSec;    
    WORD        nBlockAlign;        
    WORD        wBitsPerSample;     
    WORD        cbSize;             
                                    
} WAVEFORMATEX;

class CWaveFile
{
private:
    WAVEFORMATEX* m_pwfx; 

public:
    CWaveFile();
    ~CWaveFile();

    HRESULT Open( LPWSTR strFileName, WAVEFORMATEX* pwfx, DWORD dwFlags );
    HRESULT OpenFromMemory( BYTE* pbData, ULONG ulDataSize, WAVEFORMATEX* pwfx, DWORD dwFlags );
    HRESULT Close();

    HRESULT Read( BYTE* pBuffer, DWORD dwSizeToRead, DWORD* pdwSizeRead );
    HRESULT Write( UINT nSizeToWrite, BYTE* pbData, UINT* pnSizeWrote );

    DWORD   GetSize();
    HRESULT ResetFile();
    WAVEFORMATEX* GetFormat();
};

#endif

#endif // __WAVE_FILE_HH__
