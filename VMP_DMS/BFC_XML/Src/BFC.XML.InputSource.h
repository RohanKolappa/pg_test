// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "BFC".
// 
// "BFC" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "BFC" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "BFC"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	BFC.XML.InputSource.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _BFC_XML_InputSource_H_
#define _BFC_XML_InputSource_H_

// ============================================================================

#include "BFC.XML.DLL.h"

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace BFC {
namespace XML {

BFC_PTR_DECL( BFC_XML_DLL, InputSource )

} // namespace XML
} // namespace BFC

// ============================================================================

#include "BFC.Base.Buffer.h"
#include "BFC.Base.Exception.h"

#include "BFC.Debug.DObject.h"

#include "BFC.IO.Device.h"

// ============================================================================

namespace BFC {
namespace XML {

// ============================================================================

/// \brief The InputSource class provides the input data for the
///	Reader subclasses.
/// 
/// \ingroup BFC_XML

class BFC_XML_DLL InputSource : virtual public DObject {

public :

	/// \brief Constructs a new InputSource that gets its data from buffer
	///	\a pBuffer.

	InputSource(
		const	Buffer &	pBuffer
	);

	/// \brief Constructs a new InputSource that gets its data from device
	///	\a pDevice.

	InputSource(
			IO::DevicePtr	pDevice
	);

	/// \brief Destroys this object.

	virtual ~InputSource(
	);

	/// \brief Returns all the data the input source contains.

	const Buffer & getData(
	) const;

	/// \brief Returns the next character of the input source.
	///
	/// If this function reaches the end of available data, it returns
	/// InputSource::EndOfData. If you call getNext() after that, it
	/// tries to fetch more data by calling fetchData(). If the
	/// fetchData() call results in new data, this function returns the
	/// first character of that data; otherwise it returns
	/// InputSource::EndOfDocument.
	///
	/// Readers, such as SimpleReader, will assume that the end of
	/// the XML document has been reached if the this function returns
	/// InputSource::EndOfDocument, and will check that the
	/// supplied input is well-formed. Therefore, when reimplementing
	/// this function, it is important to ensure that this behavior is
	/// duplicated.

	Uchar getNext(
	);

	BASE_CLASS_GEN_EXCEPTION( "BFC.XML.InputSource" );

	CLASS_EXCEPTION( EndOfStream, "End of stream" );

protected:

	void safeFetch(
	);

private:

	IO::DevicePtr	inputDevice;

	Buffer		buffer;
	const Uchar *	bufPtr;
	Uint32		bufLen;
	Uint32		bufPos;

	// Disable the copy constructor and assignment by default so you will get
	// compiler errors instead of unexpected behaviour if you pass objects
	// by value or assign objects.

	InputSource(
	);

	InputSource(
		const	InputSource&
	);

	InputSource& operator = (
		const	InputSource&
	);

};

// ============================================================================

} // namespace XML
} // namespace BFC

// ============================================================================

#endif // _BFC_XML_InputSource_H_

// ============================================================================

