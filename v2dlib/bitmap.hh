/*
 * Windows Bitmap File Loader
 * Version 1.2.1 (20070430)
 *
 * Supported Formats: 1, 4, 8, 16, 24, 32 Bit Images
 * Alpha Bitmaps are also supported.
 * Supported compression types: RLE 8, BITFIELDS
 *
 * Created by: Benjamin Kalytta, 2006 - 2007
 *
 * Licence: Free to use
 * Source can be found at http://www.kalytta.com/bitmap.h
 */
#ifndef __HH_BITMAPLOADER_HH__
#define __HH_BITMAPLOADER_HH__

#ifdef V2DLIBRARY_EXPORTS
#define DLLEXPORT __declspec(dllexport)
#else
#define DLLEXPORT
#endif

#pragma pack(1)


typedef struct {
	unsigned short int Signature;
	unsigned int Size;
	unsigned int Reserved;
	unsigned int BitsOffset;
} BITMAP_FILEHEADER_INT;

#define BITMAP_FILEHEADER_SIZE_INT 14

typedef struct {
	unsigned int HeaderSize;
	int Width;
	int Height;
	unsigned short int Planes;
	unsigned short int BitCount;
	unsigned int Compression;
	unsigned int SizeImage;
	int PelsPerMeterX;
	int PelsPerMeterY;
	unsigned int ClrUsed;
	unsigned int ClrImportant;
	unsigned int RedMask;
	unsigned int GreenMask;
	unsigned int BlueMask;
	unsigned int AlphaMask;
	unsigned int CsType;
	unsigned int Endpoints[9]; // see http://msdn2.microsoft.com/en-us/library/ms536569.aspx
	unsigned int GammaRed;
	unsigned int GammaGreen;
	unsigned int GammaBlue;
} BITMAP_HEADER_INT;

typedef struct {
	unsigned char Red;
	unsigned char Green;
	unsigned char Blue;
	unsigned char Alpha;
} RGBA_INT;

typedef struct {
	unsigned char Blue;
	unsigned char Green;
	unsigned char Red;
	unsigned char Alpha;
} BGRA_INT;

typedef struct {
	unsigned char Blue;
	unsigned char Green;
	unsigned char Red;
} BGR_INT;

typedef struct {
	unsigned short int Blue:5;
	unsigned short int Green:5;
	unsigned short int Red:5;
	unsigned short int Reserved:1;
} BGR16_INT;

#pragma pack()

#if 0

#define RIFF_SIGNATURE	0x46464952
#define RIFF_TYPE		0x204c4150
#define PAL_SIGNATURE	0x61746164
#define PAL_UNKNOWN		0x01000300

typedef struct {
	unsigned int Signature;
	unsigned int FileLength;
	unsigned int Type;
	unsigned int PalSignature;
	unsigned int ChunkSize;
	unsigned int Unkown;
} PAL;

#endif

class DLLEXPORT CBitmap {
private:
	BITMAP_FILEHEADER_INT m_BitmapFileHeader;
	BITMAP_HEADER_INT m_BitmapHeader;
	RGBA_INT *m_BitmapData;
	unsigned int m_BitmapSize;
	BGRA_INT *m_ColorTable;
	unsigned int m_ColorTableSize;

    unsigned int ShiftRightByMask(unsigned int Color, unsigned int Mask, unsigned int DistributeToBits = 8);
    void Dispose();

public:

	CBitmap();
	CBitmap(const char* Filename);
	~CBitmap();

    int LoadFromBuffer(unsigned char *pBuffer, int nSize);
    int Load(const char *Filename);
    bool Save(const char* Filename, unsigned int BitCount = 32);

	unsigned int GetWidth() {
		return m_BitmapHeader.Width < 0 ? -m_BitmapHeader.Width : m_BitmapHeader.Width;
	}

	unsigned int GetHeight() {
		return m_BitmapHeader.Height < 0 ? -m_BitmapHeader.Height : m_BitmapHeader.Height;
	}

	unsigned int GetBitCount() {
		return m_BitmapHeader.BitCount;
	}

    bool GetBits(void* Buffer, unsigned int &Size);
    void* GetBits();
    bool GetBits(void* Buffer, unsigned int &Size, unsigned int BitCount);
    bool GetBitsWithPalette(void* Buffer, unsigned int &Size, unsigned int BitCount, BGRA_INT* &Palette,
            unsigned int &PaletteSize, bool OptimalPalette = false);
    bool GetBitsBGRA(void* Buffer, unsigned int &Size);
    void SetBitsBGRA(void* Buffer, unsigned int Width, unsigned int Height);
    void SetBitsRGBA(void* Buffer, unsigned int Width, unsigned int Height);
    void SetBitsBGR(void* Buffer, unsigned int Width, unsigned int Height);
    void SetBits(void* Buffer, unsigned int Width, unsigned int Height,
            unsigned int RedMask, unsigned int GreenMask, unsigned int BlueMask, unsigned int AlphaMask = 0);
    void SetAlphaBits(unsigned char Alpha);

};

#endif //__HH_BITMAPLOADER_HH__
