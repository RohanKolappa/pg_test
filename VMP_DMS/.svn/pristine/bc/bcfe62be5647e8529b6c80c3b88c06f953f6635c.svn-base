// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
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
// Filename:
//	TBFC.Base.SPtrDecl.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _TBFC_Base_SPtrDecl_H_
#define _TBFC_Base_SPtrDecl_H_

// ============================================================================

#include "TBFC.Light.DLL.h"

// ============================================================================

#include "TBFC.Base.FastCS.h"
#include "TBFC.Base.SPtrHub.h"

// ============================================================================

namespace TBFC {

// ============================================================================

class SObject;
// class SPtrHub;

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
/// To overcome these problems, TBFC provides a so-called 'Smart Pointer'
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
/// \ingroup TBFC_Base

class TBFC_Light_DLL SPtr {

public :

	/// \brief Creates a new Master SPtr, which doesn't point to any
	///	SObject.

	SPtr(
	);

	/// \brief Creates a new Master SPtr, which points to SObject \a final.

	SPtr(
			SObject		* final
	);

	/// \brief Creates a new SPtr of the same type (Master or Slave) and
	///	pointing to the same SObject than \a other.

	SPtr(
		const	SPtr		& other
	);

	/// \brief Destroys this object, also destroying the SObject this SPtr
	///	points to (if any) if this object was the last Master SPtr
	///	pointing to it.

	virtual ~SPtr(
	);

	/// \brief Detaches this object from the SObject it points to (if any),
	///	and attaches it to SObject \a final.
	///
	/// This object keeps using the same type (Master or Slave) w.r.t.
	/// \a final.
	///
	/// If this object was the last Master SPtr object pointing to the
	/// (old) SObject (if any), then this object is destroyed first.

	SPtr & operator = (
			SObject		* final
	);

	/// \brief Detaches this object from the SObject it points to (if any),
	///	and attaches it to the SObject \a other points to (if any).
	///
	/// This object will use the same type (Master or Slave) as \a other
	/// does with the SObject it will get attached to.

	SPtr & operator = (
		const	SPtr		& other
	);

	/// \brief Returns an OS-dependent description of the type of the
	///	object pointed to by this SPtr.
	///
	/// \sa SPtrHub::typeName()

	const char * typeName(
	) const;

	/// \brief Returns true iff this object points to \a ptr.

	Bool operator == (
		const	SObject		* ptr
	) const;

	/// \brief Returns true iff this object doesn't point to \a ptr.

	Bool operator != (
		const	SObject		* ptr
	) const;

	/// \brief Returns true iff this object points to the same SObject
	///	than \a other.

	Bool operator == (
		const	SPtr		& other
	) const;

	/// \brief Returns true iff this object doesn't point to the same
	///	SObject than \a other.

	Bool operator != (
		const	SPtr		& other
	) const;

	/// \brief Returns true iff this object points to some SObject.

	operator Bool (
	) const;

	/// \brief Returns true iff this object doesn't point to any SObject.

	Bool operator ! (
	) const;

	/// \brief Detaches this object from the SObject it points to (if any).
	///
	/// If this object was the last Master SPtr pointing to the SObject,
	/// then this SObject is destroyed.

	void kill(
	);

	/// \brief Turns the type of this object to Master if \a mst is true,
	///	or Slave if \a mst is false.
	///
	/// If this object was the last Master, and is now turned into a
	/// Slave, then the SObject is NOT destroyed!!!

	void setMaster(
			Bool		mst
	);

protected :

	static FastCS	* getCriticalSection();

	SPtrHub		* hub;		///< Shared hub.
	Bool		mst;		///< Master or Slave.

	/// \brief Returns the SObject pointed to by \a other.

	SObject * getRoot(
		const	SPtr		& other
	) const;

	/// \brief Returns the SObject pointed to by this object.

	SObject * getRoot(
	) const {
		return ( hub ? hub->getRoot() : 0 );
	}

};

// ============================================================================

} // namespace TBFC

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
/// \ingroup TBFC_Base

#define TBFC_PTR_DECL(__N__,__P__)					\
class __P__;								\
									\
class __N__ __P__ ## Ptr : public TBFC::SPtr {				\
public :								\
	__P__ ## Ptr();							\
	__P__ ## Ptr(__P__ *final);					\
	__P__ ## Ptr(const __P__ ## Ptr& o);				\
	__P__ ## Ptr(const TBFC::SPtr& o);				\
	virtual ~__P__ ## Ptr();					\
	__P__ ## Ptr& operator = (__P__ *final);			\
	__P__ ## Ptr& operator = (const __P__ ## Ptr& o);		\
	__P__ ## Ptr& operator = (const TBFC::SPtr& o);			\
	__P__ * operator -> () const;					\
	__P__ & operator * () const;					\
};

// ============================================================================

#endif // _TBFC_Base_SPtrDecl_H_

// ============================================================================

