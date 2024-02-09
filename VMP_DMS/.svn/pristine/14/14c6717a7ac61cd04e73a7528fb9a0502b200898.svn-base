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
//	BFC.Net.PassAuth.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_Net_PassAuth_H_
#define _BFC_Net_PassAuth_H_

// ============================================================================

#include "BFC.Net.DLL.h"

// ============================================================================

#include "BFC.Net.PassAuthPtr.h"

// ============================================================================

#include "BFC.Base.Exception.h"
#include "BFC.Base.SObject.h"

// ============================================================================

namespace BFC {
namespace Net {

// ============================================================================

/// \brief Data holder that is used by Authenticator. It is simply a repository
///	for a user name and a password.
///
/// \ingroup BFC_Net

class BFC_Net_DLL PassAuth : virtual public SObject {

public :

	/// \brief Creates a new PassAuth object from the given
	///	user name and password.
	///
	/// \param pUserName
	///	The user name.
	///
	/// \param pPassword
	///	The user's password.

	PassAuth(
		const	Buffer &	pUserName,
		const	Buffer &	pPassword
	) :
		userName( pUserName ),
		password( pPassword ) {
	}

	/// \brief Returns the user name.
	///
	/// \returns
	///	The user name.

	const Buffer & getUserName(
	) const {
		return userName;
	}

	/// \brief Returns the user password.
	///
	/// \returns
	///	The password.

	const Buffer & getPassword(
	) const {
		return password;
	}

protected :

private :

	Buffer	userName;
	Buffer	password;

	/// \brief Forbidden default constructor.

	PassAuth(
	);

	/// \brief Forbidden copy constructor.

	PassAuth(
		const	PassAuth &
	);

	/// \brief Forbidden copy operator.

	PassAuth & operator = (
		const	PassAuth &
	);

};

// ============================================================================

} // namespace Net
} // namespace BFC

// ============================================================================

#endif // _BFC_Net_PassAuth_H_

// ============================================================================

