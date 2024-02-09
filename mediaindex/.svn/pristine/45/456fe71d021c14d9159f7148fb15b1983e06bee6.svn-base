#ifndef LIBMEDIACONVERTER_STDIOCALLBACK_H
#define LIBMEDIACONVERTER_STDIOCALLBACK_H

#include <ebml/IOCallback.h>
#include <ebml/EbmlConfig.h>

#include <stdexcept>
#include <cerrno>

// ----- Added 10/15/2003 by jcsston from Zen -----
#if defined (__BORLANDC__) //Maybe other compilers?
  #include <errno.h>
  #include <stdio.h>
#endif //__BORLANDC__
// ------------------------------------------------

#include "libmediaconverter.hh"

using namespace LIBEBML_NAMESPACE;

namespace mediaindex {

class Std64IOCallback: public IOCallback
{
    private:
    	FILE*File;
		uint64 mCurrentPosition;
		uint64 mBytesRead;
        uint64 mBytesWritten;
    public:
	Std64IOCallback(const char*Path, const open_mode Mode);
	virtual ~Std64IOCallback()throw();

	virtual uint32 read(void*Buffer,size_t Size);

	// Seek to the specified position. The mode can have either SEEK_SET, SEEK_CUR
	// or SEEK_END. The callback should return true(1) if the seek operation succeeded
	// or false (0), when the seek fails.
	virtual void setFilePointer(int64 Offset,seek_mode Mode=seek_beginning);

	// This callback just works like its read pendant. It returns the number of bytes written.
	virtual size_t write(const void*Buffer,size_t Size);

	// Although the position is always positive, the return value of this callback is signed to
	// easily allow negative values for returning errors. When an error occurs, the implementor
	// should return -1 and the file pointer otherwise.
	//
	// If an error occurs, an exception should be thrown.
	virtual uint64 getFilePointer();

	// The close callback flushes the file buffers to disk and closes the file. When using the stdio
	// library, this is equivalent to calling fclose. When the close is not successful, an exception
	// should be thrown.
	virtual void close();
	uint64 getBytesRead() { return mBytesRead; };
    uint64 getBytesWritten() { return mBytesWritten; };
};

};
#endif // LIBMEDIACONVERTER_STDIOCALLBACK_H
