// ============================================================================

#include "Array.h"

// ============================================================================

#ifndef _BufferParser_H_
#define _BufferParser_H_

// ============================================================================

class BufferParser{

public :

	BufferParser(
	);

	virtual ~BufferParser(
	);

	static BufferArray tokenize( const Buffer&, const Buffer& );

protected :

private :

	// others...

	BufferParser(
		const	BufferParser&
	);

	BufferParser& operator = (
		const	BufferParser&
	);

};

// ============================================================================

#endif // _BufferParser_H_

// ============================================================================

