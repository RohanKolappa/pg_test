// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "TBFC::Light".
// 
// "TBFC::Light" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "TBFC::Light" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "TBFC::Light"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// FileName:
//	TBFC.Base.Version.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _TBFC_Base_Version_H_
#define _TBFC_Base_Version_H_

// ============================================================================

#include "TBFC.Light.DLL.h"

// ============================================================================

#include "TBFC.Base.Buffer.h"

// ============================================================================

namespace TBFC {

// ============================================================================

/// \brief Manages a version number (maj, med, min).
///
/// \ingroup TBFC_Base

class TBFC_Light_DLL Version {

public :

	Version(
	) {
		v[0] = 0;
		v[1] = 0;
		v[2] = 0;
		prec = 0;
	}

	Version(
		const	Uint32		maj
	) {
		v[0] = maj;
		v[1] = 0;
		v[2] = 0;
		prec = 1;
	}

	Version(
		const	Uint32		maj,
		const	Uint32		med
	) {
		v[0] = maj;
		v[1] = med;
		v[2] = 0;
		prec = 2;
	}

	Version(
		const	Uint32		maj,
		const	Uint32		med,
		const	Uint32		min
	) {
		v[0] = maj;
		v[1] = med;
		v[2] = min;
		prec = 3;
	}

	Version(
		const	Version&	o
	) {
		v[0] = o.v[0];
		v[1] = o.v[1];
		v[2] = o.v[2];
		prec = o.prec;
	}

	virtual ~Version(
	) {
	}

	void parseBuffer(
		const	Buffer&
	);

	Buffer toBuffer(
	) const;

	void setMaj(const Uint32 i) {v[0] = i; if (prec < 1) prec = 1;}
	void setMed(const Uint32 i) {v[1] = i; if (prec < 2) prec = 2;}
	void setMin(const Uint32 i) {v[2] = i; if (prec < 3) prec = 3;}

	void incMaj() {v[0]++; v[1] = 0; v[2] = 0; prec = 3;}
	void incMed() {v[1]++; v[2] = 0; prec = 3;}
	void incMin() {v[2]++; prec = 3;}

	Uint32 getMaj() const {return v[0];}
	Uint32 getMed() const {return v[1];}
	Uint32 getMin() const {return v[2];}

	Uint32 getPrec() const {return prec;}

	Version& operator = (
		const	Version&	o
	) {
		if (this != &o) {
			v[0] = o.v[0];
			v[1] = o.v[1];
			v[2] = o.v[2];
			prec = o.prec;
		}
		return *this;
	}

	Bool operator == (
		const	Version&	o
	) const {
		if ( prec != o.prec ) return false;
		if ( prec == 0 ) return true;
		if ( v[0] != o.v[0] ) return false;
		if ( prec == 1 ) return true;
		if ( v[1] != o.v[1] ) return false;
		if ( prec == 2 ) return true;
		return ( v[2] == o.v[2] );
	}

	Bool operator != (
		const	Version&	o
	) const {
		if ( prec != o.prec ) return true;
		if ( prec == 0 ) return false;
		if ( v[0] != o.v[0] ) return true;
		if ( prec == 1 ) return false;
		if ( v[1] != o.v[1] ) return true;
		if ( prec == 2 ) return false;
		return ( v[2] != o.v[2] );
	}

	Bool operator < (
		const	Version&	o
	) const {
		if ( prec == 0 ) return false;
		if (v[0] < o.v[0]) return true;
		if (v[0] > o.v[0]) return false;
		if ( prec == 1 ) return false;
		if (v[1] < o.v[1]) return true;
		if (v[1] > o.v[1]) return false;
		if ( prec == 2 ) return false;
		return (v[2] < o.v[2]);
	}

	Bool operator <= (
		const	Version&	o
	) const {
		if ( prec == 0 ) return true;
		if (v[0] < o.v[0]) return true;
		if (v[0] > o.v[0]) return false;
		if ( prec == 1 ) return true;
		if (v[1] < o.v[1]) return true;
		if (v[1] > o.v[1]) return false;
		if ( prec == 2 ) return true;
		return (v[2] <= o.v[2]);
	}

	Bool operator > (
		const	Version&	o
	) const {
		if ( prec == 0 ) return false;
		if (v[0] > o.v[0]) return true;
		if (v[0] < o.v[0]) return false;
		if ( prec == 1 ) return false;
		if (v[1] > o.v[1]) return true;
		if (v[1] < o.v[1]) return false;
		if ( prec == 2 ) return false;
		return (v[2] > o.v[2]);
	}

	Bool operator >= (
		const	Version&	o
	) const {
		if ( prec == 0 ) return true;
		if (v[0] > o.v[0]) return true;
		if (v[0] < o.v[0]) return false;
		if ( prec == 1 ) return true;
		if (v[1] > o.v[1]) return true;
		if (v[1] < o.v[1]) return false;
		if ( prec == 2 ) return true;
		return (v[2] >= o.v[2]);
	}

protected :

private :

	Uint32	v[3];
	Uint32	prec;

};

// ============================================================================

// TBFC_Light_DLL std::ostream& operator << (std::ostream&, const Version&);

// ============================================================================

} // namespace TBFC

// ============================================================================

#endif // _TBFC_Base_Version_H_

// ============================================================================

