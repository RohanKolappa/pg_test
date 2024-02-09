// ============================================================================

#include "Buffer.h"

// ============================================================================

#ifndef _File_H_
#define _File_H_

// ============================================================================

class File{

public :

	File(
	);

	virtual ~File(
	);

	static bool exists( const Buffer& );

	enum Mode {Read, Write};

	bool open( const Buffer&, const int );

	Buffer dumpToBuffer() const;

	Buffer getBytes() const;
	void putBytes( const Buffer& ) const;

protected :

private :

	// internal state...

	int fd;

	// others...

	File(
		const	File&
	);

	File& operator = (
		const	File&
	);

};

// ============================================================================

#endif // _File_H_

// ============================================================================

