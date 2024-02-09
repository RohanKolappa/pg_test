#include <cassert>
#include <climits>
#if !defined(__GNUC__) || (__GNUC__ > 2)
#include <sstream>
#endif // GCC2

#include "Std64IOCallback.hh"
#include <ebml/Debug.h>
#include <ebml/EbmlConfig.h>

namespace mediaindex {

Std64IOCallback::Std64IOCallback(const char*Path, const open_mode aMode)
{
	assert(Path!=0);
	const char *Mode;
	switch (aMode)
	{
	case MODE_READ:
		Mode = "rb";
		break;
	case MODE_SAFE:
		Mode = "rb+";
		break;
	case MODE_WRITE:
		Mode = "wb";
		break;
	case MODE_CREATE:
		Mode = "wb+";
		break;
	default:
		throw 0;
	}

	File=fopen(Path,Mode);
	if(File==0)
	{
#if !defined(__GNUC__) || (__GNUC__ > 2)
        std::stringstream Msg;
		Msg<<"Can't open stdio file \""<<Path<<"\" in mode \""<<Mode<<"\"";
		throw std::runtime_error(Msg.str());
#endif // GCC2
	}
	mCurrentPosition = 0;
    mBytesRead = 0;
    mBytesWritten = 0;
}


Std64IOCallback::~Std64IOCallback()throw()
{
	close();
}



uint32 Std64IOCallback::read(void*Buffer,size_t Size)
{
	assert(File != 0);
	
	size_t result = fread(Buffer, 1, Size, File);
	mCurrentPosition += result;
	mBytesRead += result;
	return result;
}

void Std64IOCallback::setFilePointer(int64 Offset,seek_mode Mode)
{
	assert(File != 0);

	// There is a numeric cast in the boost library, which would be quite nice for this checking
/* 
	SL : replaced because unknown class in cygwin
	assert(Offset <= numeric_limits<long>::max());
	assert(Offset >= numeric_limits<long>::min());
*/

	assert(Offset <= LLONG_MAX);
	assert(Offset >= LLONG_MIN);

	assert(Mode == SEEK_CUR || Mode == SEEK_END || Mode == SEEK_SET);

	if (FSEEK(File,Offset,Mode) != 0) {
#if !defined(__GNUC__) || (__GNUC__ > 2)
        std::ostringstream Msg;
		Msg<<"Failed to seek file "<<File<<" to offset "<<(unsigned long)Offset<<" in mode "<<Mode;
		throw std::runtime_error(Msg.str());
#else
        mCurrentPosition = FTELL(File);
#endif // GCC2
	}
	else {
        switch (Mode) {
        case SEEK_CUR:
            mCurrentPosition += Offset;
            break;
        case SEEK_END:
            mCurrentPosition = FTELL(File);
            break;
        case SEEK_SET:
            mCurrentPosition = Offset;
            break;
        }
    }
}

size_t Std64IOCallback::write(const void*Buffer,size_t Size)
{
	assert(File!=0);
	uint32 Result = fwrite(Buffer,1,Size,File);
	mCurrentPosition += Result;
    mBytesWritten += Result;
	return Result;
}

uint64 Std64IOCallback::getFilePointer()
{
	assert(File!=0);
	return mCurrentPosition;
}

void Std64IOCallback::close()
{
	if(File==0)
		return;

	if(fclose(File)!=0)
	{
#if !defined(__GNUC__) || (__GNUC__ > 2)
        std::stringstream Msg;
		Msg<<"Can't close file "<<File;
		throw std::runtime_error(Msg.str());
#endif // GCC2
	}

	File=0;
}

};
