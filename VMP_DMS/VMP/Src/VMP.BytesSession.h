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
//	VMP.BytesSession.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_BytesSession_H_
#define _VMP_BytesSession_H_

// ============================================================================

#include "VMP.Libs.Base.DLL.h"

// ============================================================================

#include "VMP.BytesSessionPtr.h"

// ============================================================================

#include "VMP.ConfigInfoPtr.h"
#include "VMP.Session.h"

// ============================================================================

namespace VMP {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup VMP_Libs_Base

class VMP_Libs_Base_DLL BytesSession : public Session {

public :

	/// \brief Unique DL::Type object identifying this class.

	static const BFC::DL::TypeCPtr & getClassType(
	);

	/// \brief Creates a new BytesSession.
	///
	/// \param pFlags
	///	Type of session this object represents (default is to create
	///	a Pausable session).

	BytesSession(
		const	Flags		pFlags = Pausable
	);

	/// \brief Copy oeprator.

	BytesSession(
		const	BytesSession &	pOther
	);

	virtual SessionPtr clone(
	) const;

	virtual BFC::Bool isCompatibleWith(
			SessionCPtr	pSession
	) const;

	/// \brief Sets the unit index to \a p.
	///
	/// \param p
	///	The new units index of this session.

	void setUnitsIndex(
			UnitsIndexPtr	p
	) {
		index = p;
		setLength( index->getByteOffset( index->getSize() ) );
	}

	/// \brief Resets the units index of this session.

	void resetUnitsIndex(
	) {
		index.kill();
	}

	/// \brief Returns true iff the units index has been set.

	BFC::Bool hasUnitsIndex(
	) const {
		return index;
	}

	/// \brief Returns the units index of this frame.

	UnitsIndexPtr getUnitsIndex(
	) const {
		return index;
	}

	void setBitRate(
		const	BFC::Uint32	i
	) {
		bitRate = i;
	}

	BFC::Bool hasBitRate(
	) const {
		return ( bitRate != 0 );
	}

	BFC::Uint32 getBitRate(
	) const {
		return bitRate;
	}

	void setGOPSize(
		const	BFC::Uint32	i
	) {
		gopSize = i;
	}

	BFC::Bool hasGOPSize(
	) const {
		return ( gopSize != 0 );
	}

	BFC::Uint32 getGOPSize(
	) const {
		return gopSize;
	}

	/// \brief Sets the content to \a p.
	///
	/// \param p
	///	The new content of this session.

	void setContent(
			StreamPtr	p
	) {
		setStream( p );
	}

	/// \brief Resets the content of this session.

	void resetContent(
	) {
		resetStream();
	}

	/// \brief Returns true iff the content has been set.

	BFC::Bool hasContent(
	) const {
		return hasStream();
	}

	/// \brief Returns the content of this session.

	StreamPtr getContent(
	) {
		return getStream();
	}

	/// \brief Returns the content of this session.

	StreamCPtr getContent(
	) const {
		return getStream();
	}

	BFC::Bool hasConfigInfo(
	) const {
		return ( BFC::Bool )cnfgInfo;
	}

	ConfigInfoCPtr getConfigInfo(
	) const {
		return cnfgInfo;
	}

	void setConfigInfo(
			ConfigInfoCPtr	pCnfgInfo
	) {
		cnfgInfo = pCnfgInfo;
	}

	void resetConfigInfo(
	) {
		cnfgInfo.kill();
	}

	virtual BFC::Buffer toBuffer(
	) const;

	virtual void doSerialize(
			BFC::IO::ByteOutputStreamPtr
	) const;

	virtual void unSerialize(
			BFC::IO::ByteInputStreamPtr
	);

private :

	BFC::Uint32		bitRate;	///< Bitrate.
	BFC::Uint32		gopSize;	///< GOP size.
	UnitsIndexPtr		index;		///< Units index.
	ConfigInfoCPtr		cnfgInfo;	///< Extra configuration info.

	// others...

	BytesSession& operator = (
		const	BytesSession&
	);

};

// ============================================================================

} // namespace VMP

// ============================================================================

#endif // _VMP_BytesSession_H_

// ============================================================================

