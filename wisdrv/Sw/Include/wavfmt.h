#ifndef _wavfmt_h
#define _wavfmt_h
/*
*
*  This header file describes the format for .WAV audio files
*
*/
/*
* WAV format is an unspecfied standard.  Some of the details may vary from
* the structure given in this file.  One way to approach the format is to
* go with the "chunk" notion.  Each of the documented chunks will probably
* occur in all .WAV files.  There may be some additional data in each 
* chunk, but as long as one can locate the beginning of a chunk, 
* the .WAV file may be playable.
*
* Chunk declarations based on description of WAV file format at:
* http://www.technology.niagarac.on.ca/courses/comp630/WavFileFormat.html
*/

/* N.B.: All ints and shorts are little endian */

struct _riffChunk
    {
    char riffLiteral[4];        /* "RIFF" signature */
    unsigned long fileLen;	/* filelength-8     */
    char waveLiteral[4];        /* file type "WAVE" */
    };

struct _fmtChunk
    {
    char fmtLiteral[4];         /* "fmt_" chunk signature */
    unsigned long fmtChunkLen;  /* fmt chunk len - 8 (or always 0x10?) */
    unsigned short formatId;    /* 0x1 for PCM data */
    unsigned short nchan;       /* number of channels */
    unsigned long sampleRate;   /* samples/sec */
    unsigned long byteRate;     /* bytes/sec */
    unsigned short blockSize;   /* bytes/sample * number of channels */
    unsigned short sampleSize;  /* bits per sample */
    };

struct _dataChunk
    {
    char dataLiteral[4];	/* "data" chunk signature */
    unsigned long dataLen; 	/* length of data in bytes */
    };

#define WAV_HEADERSIZE 0x2c

#define WAV_RIFF_LIT 0x0
#define WAV_LEN 0x4
#define WAV_WAV_LIT 0x8
#define WAV_FMT_LIT 0xc
#define WAV_CHUNKLEN 0x10
#define WAV_FORMATID 0x14
#define WAV_NCHAN 0x16
#define WAV_SAMPLERATE 0x18
#define WAV_BYTERATE 0x1c
#define WAV_BLOCKSIZE 0x20
#define WAV_SAMPLESIZE 0x22
#define WAV_DATA_LIT 0x24
#define WAV_DATALEN 0x28

#endif /* _wavfmt_h */
