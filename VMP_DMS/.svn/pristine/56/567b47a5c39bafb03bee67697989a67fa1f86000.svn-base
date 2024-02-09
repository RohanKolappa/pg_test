// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "BFC::Light".
// 
// "BFC::Light" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "BFC::Light" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "BFC::Light"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	BFC.Base.SPtrDecl.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_Base_SPtrDecl_H_
#define _BFC_Base_SPtrDecl_H_

// ============================================================================

#include "BFC.Light.DLL.h"

// ============================================================================

#include "BFC.Base.FastCS.h"
#include "BFC.Base.SPtrHub.h"

// ============================================================================

namespace BFC {

// ============================================================================

class SObject;

// ============================================================================

/// \brief Base class of all Smart Pointer objects.
///
/// A standard pointer is a 32-bits or 64-bits value holding the
/// address of the memory location of another object.
///
/// Having more than 1 standard pointer pointing to an object is dangerous,
/// as 1 pointer could be used to destroy the object, while the other
/// pointers keep the old address value, thus pointing in a deleted area
/// (dangling pointers).
///
/// Additionally, using 1 standard pointer from different threads could
/// lead to unpredictable behaviour in no proper synchronization is
/// implemented.
///
/// To overcome these problems, BFC provides a so-called 'Smart Pointer'
/// mechanism, that alleviates the user from having to deal with those
/// issues.
///
/// Using this mechanism,
/// standard pointers are replaced by instances of the SPtr class, or more
/// precisely by instances of classes inheriting the SPtr class, with the
/// SPtr class keeping track of the address of the object pointed to, as
/// represented on the diagram below.
///
/// \image html SPtr-Classes.jpg
/// \image latex SPtr-Classes.eps
///
/// \ingroup BFC_Base

class BFC_Light_DLL SCPtr {

public :

	/// \brief Creates a new Master SCPtr, which doesn't point to any
	///	SObject.

	SCPtr(
	) :
		hub( 0 ),
		mst( true ),
		stk( false ),
		sfe( true ) {
	}

	/// \brief Creates a new Master SCPtr, which points to SObject \a final.

	SCPtr(
		const	SObject *	final,
		const	Bool		pMaster = true,
		const	Bool		pSticky = false
	);

	/// \brief Creates a new SCPtr of the same type (Master or Slave) and
	///	pointing to the same SObject than \a other.

	SCPtr(
		const	SCPtr &		other,
		const	Bool		pMaster = true,
		const	Bool		pSticky = false
	) :
		stk( pSticky ) {
		other.initClone( hub, mst, sfe, pMaster, pSticky );
	}

	/// \brief Destroys this object, also destroying the SObject this SCPtr
	///	points to (if any) if this object was the last Master SCPtr
	///	pointing to it.

	~SCPtr(
	);

	/// \brief Detaches this object from the SObject it points to (if any),
	///	and attaches it to SObject \a final.
	///
	/// This object keeps using the same type (Master or Slave) w.r.t.
	/// \a final.
	///
	/// If this object was the last Master SCPtr object pointing to the
	/// (old) SObject (if any), then this object is destroyed first.

	SCPtr & operator = (
		const	SObject *	final
	);

	/// \brief Detaches this object from the SObject it points to (if any),
	///	and attaches it to the SObject \a other points to (if any).
	///
	/// This object will use the same type (Master or Slave) as \a other
	/// does with the SObject it will get attached to.

	SCPtr & operator = (
		const	SCPtr &		other
	);

	/// \brief Returns an OS-dependent description of the type of the
	///	object pointed to by this SCPtr.
	///
	/// \sa SPtrHub::typeName()

	const char * typeName(
	) const;

	/// \brief Returns true if the object pointed to by this smart pointer
	///	is of type \a T.

	template < class T >
	Bool isA(
	) const {
		BFC::SObject *	ot = getRoot();
		return ( ot != 0 && dynamic_cast< T * >( ot ) != 0 );
	}

	/// \brief Returns true iff this object points to \a ptr.

	Bool operator == (
		const	SObject *	ptr
	) const;

	/// \brief Returns true iff this object doesn't point to \a ptr.

	Bool operator != (
		const	SObject *	ptr
	) const;

	/// \brief Returns true iff this object points to the same SObject
	///	than \a other.

	Bool operator == (
		const	SCPtr &		other
	) const;

	/// \brief Returns true iff this object doesn't point to the same
	///	SObject than \a other.

	Bool operator != (
		const	SCPtr &		other
	) const;

	/// \brief Returns true iff this object points to some SObject.

	operator Bool (
	) const;

	/// \brief Returns true iff this object doesn't point to any SObject.

	Bool operator ! (
	) const;

	/// \brief Detaches this object from the SObject it points to (if any).
	///
	/// If this object was the last Master SCPtr pointing to the SObject,
	/// then this SObject is destroyed.

	void kill(
	);

	/// \brief Returns whether this SCPtr is a Master pointer or not.
	///
	/// \return
	///	true if this SCPtr is a Master pointer, false otherwise.

	Bool isMaster(
	) const {
		return mst;
	}

	/// \brief Turns the type of this object to Master if \a mst is true,
	///	or Slave if \a mst is false.
	///
	/// If this object was the last Master, and is now turned into a
	/// Slave, then the SObject is NOT destroyed!!!

	void setMaster(
		const	Bool		mst
	);

	Bool isThreadSafe(
	) const {
		return sfe;
	}

	Bool setThreadSafe(
		const	Bool		safe
	);

	Uint32 getRefCount(
	) const;

protected :

	void initClone(
			SPtrHub * &	newHub,
			Bool &		newMst,
			Bool &		newSfe,
		const	Bool		expMst,
		const	Bool		sticky
	) const;

	static FastCS * getCriticalSection(
	);

	SPtrHub *	hub;	///< Shared hub.
	Bool		mst;	///< Master mode (default: true) ?
	Bool		stk;	///< Sticky mode (default: false) ?
	Bool		sfe;	///< Thread safe (cached) (default: true) ?

	/// \brief Returns the SObject pointed to by this object.

	SObject * getRoot(
	) const {
		return ( hub ? hub->getRoot() : 0 );
	}

	/// \brief Returns the SObject pointed to by \a other.

	static SObject * getRoot(
		const	SCPtr		& other
	) {
		return other.getRoot();
	}

	template < class T >
	T * getCastPtr(
		const	char *		dstName	= 0 // ## T
	) const {
		BFC::SObject *	ot = getRoot();
		if ( ! ot ) {
			throwNullPointer( dstName );
		}
		T * nt = dynamic_cast< T * >( ot );
		if ( ! nt ) {
			throwBadCast( typeName(), dstName );
		}
		return nt;
	}

	template < class T >
	void checkCastPtr(
		const	char *		dstName	// ## T
	) {
		BFC::SObject *	ot = getRoot();
		if ( ot && ! dynamic_cast< T * >( ot ) ) {
			// Backup our type name...
			const char * srcName = typeName();
			// Revert to NULL value...
			kill();
			// Throw BadCast exception!
			throwBadCast( srcName, dstName );
		}
	}

	void throwNullPointer(
		const	char *		dstName
	) const;

	void throwBadCast(
		const	char *		pSrcName,
		const	char *		pDstName
	) const;

};

// ============================================================================

class BFC_Light_DLL SPtr : public SCPtr {

public :

	/// \brief Creates a new Master SPtr, which doesn't point to any
	///	SObject.

	SPtr(
	) {
	}

	/// \brief Creates a new Master SPtr, which points to SObject \a final.

	SPtr(
			SObject *	final,
		const	Bool		pMaster = true,
		const	Bool		pSticky = false
	) : SCPtr( final, pMaster, pSticky ) {
	}

	/// \brief Creates a new SPtr of the same type (Master or Slave) and
	///	pointing to the same SObject than \a other.

	SPtr(
		const	SPtr &		other,
		const	Bool		pMaster = true,
		const	Bool		pSticky = false
	) : SCPtr( other, pMaster, pSticky ) {
	}

	/// \brief Detaches this object from the SObject it points to (if any),
	///	and attaches it to SObject \a final.
	///
	/// This object keeps using the same type (Master or Slave) w.r.t.
	/// \a final.
	///
	/// If this object was the last Master SPtr object pointing to the
	/// (old) SObject (if any), then this object is destroyed first.

	SPtr & operator = (
			SObject *	final
	) {
		SCPtr::operator = ( final );
		return *this;
	}

	/// \brief Detaches this object from the SObject it points to (if any),
	///	and attaches it to the SObject \a other points to (if any).
	///
	/// This object will use the same type (Master or Slave) as \a other
	/// does with the SObject it will get attached to.

	SPtr & operator = (
		const	SPtr &		other
	) {
		SCPtr::operator = ( other );
		return *this;
	}

};

// ============================================================================

} // namespace BFC

// ============================================================================

/// \brief Macro to declare a class inheriting SPtr.
///
/// This macro can be used to easily and rapidly declare a new Smart
/// Pointer class to some user class. See the dummy SampleClass
/// and SampleClassPtr
/// classes documentation to further understand the Smart Pointer
/// mechanism.
///
/// \param __N__ A string to be placed between the token "class" and
///	the class name. If this Smart Pointer is to be defined in a DLL,
///	you should use some mixture of the JBS_DLL_EXPORT and JBS_DLL_IMPORT macros.
///	If this Smart Pointer is not to be exported by/imported from some
///	DLL, you could simply use the JBS_DLL_NONE macro as argument.
///	Note that JBS (JFG's Building System) takes care of all this
///	automatically (see appropriate documentation).
///
/// \param __P__ The user class name. This name is used to form the final
///	Smart Pointer class name, which is the concatenation between the
///	user class name and the string "Ptr".
///
/// Example: use JBS's C++ class template instantiator to see how to use
///	this macro.
///
/// \ingroup BFC_Base

#define BFC_PTR_DECL(__N__,__P__)					\
									\
class __P__;								\
									\
class __N__ __P__ ## CPtr : public BFC::SCPtr {				\
									\
public :								\
									\
	__P__ ## CPtr(							\
	) {								\
	}								\
									\
	__P__ ## CPtr(							\
		const	__P__ *		final,				\
		const	BFC::Bool	pMaster = true,			\
		const	BFC::Bool	pSticky = false			\
	);								\
									\
	__P__ ## CPtr(							\
		const	BFC::SCPtr &	other,				\
		const	BFC::Bool	pMaster = true,			\
		const	BFC::Bool	pSticky = false			\
	);								\
									\
	__P__ ## CPtr & operator = (					\
		const	__P__ *		final				\
	);								\
									\
	__P__ ## CPtr & operator = (					\
		const	BFC::SCPtr &	other				\
	);								\
									\
	const __P__ * operator -> (					\
	) const;							\
									\
	const __P__ & operator * (					\
	) const;							\
									\
};									\
									\
class __N__ __P__ ## Ptr : public BFC::SPtr {				\
									\
public :								\
									\
	__P__ ## Ptr(							\
	) {								\
	}								\
									\
	__P__ ## Ptr(							\
			__P__ *		final,				\
		const	BFC::Bool	pMaster = true,			\
		const	BFC::Bool	pSticky = false			\
	);								\
									\
	__P__ ## Ptr(							\
		const	BFC::SPtr &	other,				\
		const	BFC::Bool	pMaster = true,			\
		const	BFC::Bool	pSticky = false			\
	);								\
									\
	__P__ ## Ptr & operator = (					\
			__P__ *		final				\
	);								\
									\
	__P__ ## Ptr & operator = (					\
		const	BFC::SPtr &	other				\
	);								\
									\
	__P__ * operator -> (						\
	) const;							\
									\
	__P__ & operator * (						\
	) const;							\
									\
};

// ============================================================================

#endif // _BFC_Base_SPtrDecl_H_

// ============================================================================

