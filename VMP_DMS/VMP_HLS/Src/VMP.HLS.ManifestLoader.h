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
//	VMP.HLS.ManifestLoader.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_HLS_ManifestLoader_H_
#define _VMP_HLS_ManifestLoader_H_

// ============================================================================

#include "VMP.HLS.DLL.h"

// ============================================================================

#include "VMP.HLS.ManifestLoaderPtr.h"

// ============================================================================

#include "BFC.MT.Thread.h"

#include "VMP.HLS.LevelInfoPtr.h"
#include "VMP.HLS.ManifestPtr.h"
#include "VMP.HLS.ProgramInfoPtr.h"

// ============================================================================

namespace VMP {
namespace HLS {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup VMP_HLS

class VMP_HLS_DLL ManifestLoader : public BFC::MT::Thread {

public :

	/// \brief Creates a new ManifestLoader.

	ManifestLoader(
	);

	/// \brief Destroys this object.

	virtual ~ManifestLoader(
	);

	void setForceURLReload(
		const	BFC::Bool	val
	) {

		frceReld = val;

	}

	BFC::Bool getForceURLReload(
	) const {

		return frceReld;

	}

	/// \brief Returns the total stream duration.

	BFC::Double getStreamDuration(
	) const;

	/// \brief Loads the manifest file.

	void load(
		const	BFC::Buffer &	pMnfstUrl
	);

	void kill(
	);

	LevelInfoPtr getLevelInfo(
	) const {

		return curLevel;

	}

	/// \brief Returns the stream type.

	const BFC::Buffer & getType(
	) const {

		return strmType;

	}

protected :

	virtual void run(
	);

	void doLoop(
	);

	void mergeRootManifest(
			ManifestPtr	pManifest
	);
	void mergeLeafManifest(
			ManifestPtr	pManifest
	);

private :

	BFC::Bool	frceReld;	///< Force manifest reloading ?
//	FileLoaderPtr	fileLder;	///< Object that fetches the manifest.
	ProgramInfoPtr	progInfo;	///< Unique ProgramInfo.
	LevelInfoPtr	curLevel;	///< Current LevelInfo.
	BFC::Buffer	currLink;	///< URL being loaded.
	BFC::Buffer	strmType;	///< Streaming type.
//	TimerPtr	chckTmer;	///< Check timer.

	/// \brief Forbidden copy constructor.

	ManifestLoader(
		const	ManifestLoader &
	);

	/// \brief Forbidden copy operator.

	ManifestLoader & operator = (
		const	ManifestLoader &
	);

};

// ============================================================================

} // namespace HLS
} // namespace VMP

// ============================================================================

#endif // _VMP_HLS_ManifestLoader_H_

// ============================================================================

