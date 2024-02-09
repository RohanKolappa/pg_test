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
//	BFC.Base.SPtr.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_SPtr_H_
#define _BFC_SPtr_H_

// ============================================================================

#include "BFC.Base.Exception.h"

// ============================================================================

namespace BFC {

// ============================================================================

class SObject;
class SPtrHub;

// ============================================================================

/// Base class of all Smart Pointer objects.

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
/// \ingroup Common_Base

class SPtr {

public :

	SPtr(
	);

	SPtr(
			SObject		*final
	);

	SPtr(
		const	SPtr&		o
	);

	virtual ~SPtr(
	);

	SPtr& operator = (
			SObject		*final
	);

	SPtr& operator = (
		const	SPtr&		o
	);

	/// \brief Returns an OS-dependent description of the type of the
	///	object pointed to by this SPtr.

	const char *typeName(
	) const;

	Bool operator == (
		const	SObject	*ptr
	) const;

	Bool operator != (
		const	SObject	*ptr
	) const;

	Bool operator == (
		const	SPtr&	o
	) const;

	Bool operator != (
		const	SPtr&	o
	) const;

	operator Bool () const;

	Bool operator ! () const;

	void kill();

	void setMaster(Bool);

	BASE_CLASS_GEN_EXCEPTION("SPtr");

	CLASS_EXCEPTION(BadCast, "Bad cast");
	CLASS_EXCEPTION(NullPointer, "Null pointer");

protected :

	SPtrHub		*hub;
	Bool		mst;

	static Uint32	currCount;
	static Uint32	peakCount;

	SObject *getRoot(
		const	SPtr&	o
	) const;

	SObject *getRoot(
	) const;

};

// ============================================================================

/// Macro to declare a class inheriting BFC::SPtr.

/// This macro can be used to easily and rapidly declare a new Smart
/// Pointer class to some user class. See the dummy BFC::SampleClass
/// and BFC::SampleClassPtr
/// classes documentation to further understand the Smart Pointer
/// mechanism.
/// \param __N__ A string to be placed between the token "class" and
///	the class name. If this Smart Pointer is to be defined in a DLL,
///	you should use some mixture of the JBS_DLL_EXPORT and JBS_DLL_IMPORT macros.
///	If this Smart Pointer is not to be exported by/imported from some
///	DLL, you could simply use the JBS_DLL_NONE macro as argument.
///	Note that JBS (JFG's Building System) takes care of all this
///	automatically (see appropriate documentation).
/// \param __P__ The user class name. This name is used to form the final
///	Smart Pointer class name, which is the concatenation between the
///	user class name and the string "Ptr".
///
/// Example: use JBS's C++ class template instantiator to see how to use
///	this macro.
///
/// \ingroup Common_Base

#define BFC_PTR_DECL(__N__,__P__)					\
									\
class __P__;								\
									\
class __N__ __P__ ## Ptr : public BFC::SPtr {				\
public :								\
	__P__ ## Ptr();							\
	__P__ ## Ptr(__P__ *final);					\
	__P__ ## Ptr(const __P__ ## Ptr& o);				\
	__P__ ## Ptr(const BFC::SPtr& o);				\
	virtual ~__P__ ## Ptr();					\
	__P__ ## Ptr& operator = (__P__ *final);			\
	__P__ ## Ptr& operator = (const __P__ ## Ptr& o);		\
	__P__ ## Ptr& operator = (const BFC::SPtr& o);			\
	__P__ * operator -> () const;					\
	__P__ & operator * () const;					\
};

/// Macro to define (implement) a class inheriting BFC::SPtr.

/// This macro can be used to implement the class declared using a
/// corresponding BFC_PTR_DECL macro.
/// \param __P__ The user class name. This name is used to form the final
///	Smart Pointer class name, which is the concatenation between the
///	user class name and the string "Ptr".
///
/// Example: use JBS's C++ class template instantiator to see how to use
///	this macro.
///
/// \ingroup Common_Base

#define BFC_PTR_IMPL(__P__)						\
									\
__P__ ## Ptr::__P__ ## Ptr() : BFC::SPtr() {}				\
									\
__P__ ## Ptr::__P__ ## Ptr(__P__ *final) : BFC::SPtr(final) {}		\
									\
__P__ ## Ptr::__P__ ## Ptr(const __P__ ## Ptr& o) : BFC::SPtr(o) {}	\
									\
__P__ ## Ptr::__P__ ## Ptr(const BFC::SPtr& o) : BFC::SPtr(o) {		\
	BFC::SObject *ot = getRoot();					\
	if ( ot ) {							\
		__P__ *nt = dynamic_cast<__P__ *>( ot );		\
		if ( ! nt ) {						\
			const char *tn = typeName();			\
			operator = ( (BFC::SObject *)0);		\
			throw BadCast(					\
				BFC::Buffer("Casting ") + tn		\
				+ " to " # __P__ );			\
		}							\
	}								\
}									\
									\
__P__ ## Ptr::~ __P__ ## Ptr() {}					\
									\
__P__ ## Ptr& __P__ ## Ptr::operator = (__P__ *final) {			\
	SPtr::operator = (final);					\
	return *this;							\
}									\
									\
__P__ ## Ptr& __P__ ## Ptr::operator = (const __P__ ## Ptr& o) {	\
	SPtr::operator = (o);						\
	return *this;							\
}									\
									\
__P__ ## Ptr& __P__ ## Ptr::operator = (const BFC::SPtr& o) {		\
	SPtr::operator = (o);						\
	BFC::SObject *ot = getRoot();					\
	if ( ot ) {							\
		__P__ *nt = dynamic_cast<__P__ *>( ot );		\
		if ( ! nt ) {						\
			const char *tn = typeName();			\
			operator = ( (BFC::SObject *)0);		\
			throw BadCast(					\
				BFC::Buffer("Casting ") + tn		\
				+ " to " # __P__ );			\
		}							\
	}								\
	return *this;							\
}									\
									\
__P__ * __P__ ## Ptr::operator -> () const {				\
	BFC::SObject *ot = getRoot();					\
	if ( ! ot ) {throw NullPointer( "Type: " # __P__ );}		\
	__P__ *nt = dynamic_cast<__P__ *>(ot);				\
	if ( ! nt ) {							\
		throw BadCast(						\
			BFC::Buffer("Casting ") + typeName()		\
			+ " to " # __P__ );				\
	}								\
	return nt;							\
}									\
									\
__P__ & __P__ ## Ptr::operator * () const {				\
	BFC::SObject *ot = getRoot();					\
	if ( ! ot ) {throw NullPointer( "Type: " # __P__ );}		\
	__P__ *nt = dynamic_cast<__P__ *>(ot);				\
	if ( ! nt ) {							\
		throw BadCast(						\
			BFC::Buffer("Casting ") + typeName()		\
			+ " to " # __P__ );				\
	}								\
	return (*nt);							\
}

#define BFC_PTR_IMPL_NS(__NS__,__P__)					\
									\
__NS__::__P__ ## Ptr::__P__ ## Ptr() : BFC::SPtr() {}			\
									\
__NS__::__P__ ## Ptr::__P__ ## Ptr(__NS__::__P__ *final) : BFC::SPtr(final) {}		\
									\
__NS__::__P__ ## Ptr::__P__ ## Ptr(const __NS__::__P__ ## Ptr& o) : BFC::SPtr(o) {}	\
									\
__NS__::__P__ ## Ptr::__P__ ## Ptr(const BFC::SPtr& o) : BFC::SPtr(o) {		\
	BFC::SObject *ot = getRoot();					\
	if ( ot ) {							\
		__P__ *nt = dynamic_cast<__P__ *>( ot );		\
		if ( ! nt ) {						\
			const char *tn = typeName();			\
			operator = ( (BFC::SObject *)0);		\
			throw BadCast(					\
				BFC::Buffer("Casting ") + tn		\
				+ " to " # __P__ );			\
		}							\
	}								\
}									\
									\
__NS__::__P__ ## Ptr::~ __P__ ## Ptr() {}					\
									\
__NS__::__P__ ## Ptr& __NS__::__P__ ## Ptr::operator = (__NS__::__P__ *final) {			\
	SPtr::operator = (final);					\
	return *this;							\
}									\
									\
__NS__::__P__ ## Ptr& __NS__::__P__ ## Ptr::operator = (const __NS__::__P__ ## Ptr& o) {	\
	SPtr::operator = (o);						\
	return *this;							\
}									\
									\
__NS__::__P__ ## Ptr& __NS__::__P__ ## Ptr::operator = (const BFC::SPtr& o) {		\
	SPtr::operator = (o);					\
	BFC::SObject *ot = getRoot();					\
	if ( ot ) {							\
		__P__ *nt = dynamic_cast<__P__ *>( ot );		\
		if ( ! nt ) {						\
			const char *tn = typeName();			\
			operator = ( (BFC::SObject *)0);		\
			throw BadCast(					\
				BFC::Buffer("Casting ") + tn		\
				+ " to " # __P__ );			\
		}							\
	}								\
	return *this;							\
}									\
									\
__NS__::__P__ * __NS__::__P__ ## Ptr::operator -> () const {				\
	BFC::SObject *ot = getRoot();					\
	if ( ! ot ) {throw NullPointer( "Type: " # __P__ );}		\
	__P__ *nt = dynamic_cast<__P__ *>(ot);				\
	if ( ! nt ) {							\
		throw BadCast(						\
			BFC::Buffer("Casting ") + typeName()		\
			+ " to " # __P__ );				\
	}								\
	return nt;							\
}									\
									\
__NS__::__P__ & __NS__::__P__ ## Ptr::operator * () const {				\
	BFC::SObject *ot = getRoot();					\
	if ( ! ot ) {throw NullPointer( "Type: " # __P__ );}		\
	__P__ *nt = dynamic_cast<__P__ *>(ot);				\
	if ( ! nt ) {							\
		throw BadCast(						\
			BFC::Buffer("Casting ") + typeName()		\
			+ " to " # __P__ );				\
	}								\
	return (*nt);							\
}

// ============================================================================

} // namespace BFC

// ============================================================================

#endif // _BFC_SPtr_H_

// ============================================================================

