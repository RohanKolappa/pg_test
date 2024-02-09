// ============================================================================
//
//	Copyright (c) 2003-2014 Barco N.V.
//
// ============================================================================
//
//	Filename: VMP.DynVar.h
//
//	Creation:
//		Author(s):	Jean-Francois GOBBERS
//		Date:		
//		Version:	
//		Description:	
//
// CVS: $Id: signature.class.h,v 1.2 2002/01/15 17:25:40 gobbers Exp $
//
// ============================================================================

#ifndef _VMP_DynVar_H_
#define _VMP_DynVar_H_

// ============================================================================

#include "VMP.Libs.Base.DLL.h"

// ============================================================================

#include "BFC.Base.Exception.h"
#include "BFC.Base.FastCS.h"
#include "BFC.Base.SObject.h"
#include "BFC.Base.SPtr.h"

#include "BFC.Dyn.VBuffer.h"

#include "BFC.TL.Array.h"

// ============================================================================

namespace VMP {

// ============================================================================

BFC_PTR_DECL(VMP_Libs_Base_DLL,DynVar)

// ============================================================================

/// Maintains a list of (tag,value) pairs.

/// A DynVar object maintains a dynamically modifiable list of (tag,value)
/// pairs, where the (tag) part is a simple string, and the (value) can be
/// a string, an integer, a boolean, or a double.
///
/// \ingroup VMP_Libs_Base

class VMP_Libs_Base_DLL DynVar : virtual public BFC::SObject {

public :

	/// \brief Creates an empty DynVar object.

	DynVar(
	);

	/// \brief Creates a deep copy of \a other.

	DynVar(
		const	DynVar&		other
	);

	/// \brief Destroys this object.

	virtual ~DynVar(
	);

	/// \brief Replaces the actual content by a copy of \a other.

	DynVar& operator = (
		const	DynVar&		other
	);

	/// \brief Returns true iff this object is empty.

	BFC::Bool isEmpty(
	) const {
		return vars.isEmpty();
	}

	BFC::Uint32 size(
	) const {
		return vars.getSize();
	}

	BFC::Buffer getTag(
		const	BFC::Uint32	i
	) const {
		return ( vars[ i ]->getName() );
	}

	BFC::Buffer getVal(
		const	BFC::Uint32	i
	) const {
		return ( vars[ i ]->getValue() );
	}

	/// \brief Adds a new (\a tag,\a val) pair to this object.
	///
	/// Multiple entries, tagged with the same \a tag, can coexist, but
	/// it is not specified which will be used upon following calls to
	/// one of the various 'get' methods described below.

	void add(
		const	BFC::Buffer&	tag,
		const	BFC::Buffer&	val
	);

	/// \brief Deletes existing entry, tagged \a tag.
	///
	/// \throws NoSuchVariable
	///	There is no entry, tagged \a tag.

	void del(
		const	BFC::Buffer&	tag
	);

	/// \brief Gives value \a val to the existing entry, tagged \a tag.
	///
	/// \throws NoSuchVariable
	///	There is no entry, tagged \a tag.

	void set(
		const	BFC::Buffer&	tag,
		const	BFC::Buffer&	val
	);

	/// \brief Retrieves the string value of the existing entry, tagged \a tag.
	///
	/// \throws NoSuchVariable
	///	There is no entry, tagged \a tag.

	BFC::Buffer get(
		const	BFC::Buffer&	tag
	) const;

	/// \brief Retrieves the string value of the existing entry, tagged \a tag.
	///
	/// \throws NoSuchVariable
	///	There is no entry, tagged \a tag.

	BFC::Buffer getBuffer(
		const	BFC::Buffer&	tag
	) const;

	/// \brief Retrieves the integer value of the existing entry, tagged \a tag.
	///
	/// \throws NoSuchVariable
	///	There is no entry, tagged \a tag.

	BFC::Uint32 getUint32(
		const	BFC::Buffer&	tag
	) const;

	/// \brief Retrieves the boolean value of the existing entry, tagged \a tag.
	///
	/// \throws NoSuchVariable
	///	There is no entry, tagged \a tag.

	BFC::Bool getBool(
		const	BFC::Buffer&	tag
	) const;

	/// \brief Retrieves the double value of the existing entry, tagged \a tag.
	///
	/// \throws NoSuchVariable
	///	There is no entry, tagged \a tag.

	BFC::Double getDouble(
		const	BFC::Buffer&	tag
	) const;

	//
	//	For the exceptions...
	//

	BASE_CLASS_GEN_EXCEPTION("VMP.DynVar");

	CLASS_EXCEPTION(NoSuchVariable, "No such variable");

protected :

private :

	mutable BFC::FastCS			crit;
	BFC::Array< BFC::Dyn::VBufferPtr >	vars;

};

// ============================================================================

} // namespace VMP

// ============================================================================

#endif // _VMP_DynVar_H_

// ============================================================================

