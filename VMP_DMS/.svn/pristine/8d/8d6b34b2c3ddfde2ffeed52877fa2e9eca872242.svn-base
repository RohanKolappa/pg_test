// ============================================================================

#ifndef _BufQString_H_
#define _BufQString_H_

#include <QtCore/QString>

#include "Buffer.h"

// ============================================================================

class BufQString : public QString {

public :

	BufQString() : QString() {}

	BufQString(const QString& s) : QString(s) {}

	BufQString(
		const Buffer&	src
	) {
		if (!src.isEmpty()) {
#if QT_VERSION >= 0x040000
			QByteArray a(
				(const char *)src.getBase(),
				src.length()
			);
#else
			QByteArray a;
			a.duplicate(
				(const char *)src.getBase(),
				src.length()
			);
#endif
			append( a );
		}
	}

	Buffer toBuffer() {
#if QT_VERSION >= 0x040000
		QByteArray a = toLatin1();
		return Buffer(
			( const char *)a.constData(),
			( int )a.count() );
#else
		return Buffer((const char *)latin1(), (int)length());
#endif
	}

protected :

private :

};

// ============================================================================

#endif // _BufQString_H_

// ============================================================================

