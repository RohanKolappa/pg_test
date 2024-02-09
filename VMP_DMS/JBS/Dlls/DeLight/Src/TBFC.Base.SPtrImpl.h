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
//	TBFC.Base.SPtrImpl.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _TBFC_Base_SPtrImpl_H_
#define _TBFC_Base_SPtrImpl_H_

// ============================================================================

#include "TBFC.Light.DLL.h"

// ============================================================================

#include "TBFC.Base.Buffer.h"
#include "TBFC.Base.Exception.h"
#include "TBFC.Base.SPtrDecl.h"

// ============================================================================

/// \brief Macro to define (implement) a class inheriting SPtr.
///
/// This macro can be used to implement the class declared using a
/// corresponding TBFC_PTR_DECL macro.
///
/// \param __P__ The user class name. This name is used to form the final
///	Smart Pointer class name, which is the concatenation between the
///	user class name and the string "Ptr".
///
/// Example: use JBS's C++ class template instantiator to see how to use
///	this macro.
///
/// \ingroup TBFC_Base

#define TBFC_PTR_IMPL(__P__)						\
__P__ ## Ptr::__P__ ## Ptr() : TBFC::SPtr() {}				\
									\
__P__ ## Ptr::__P__ ## Ptr(__P__ *final) : TBFC::SPtr(final) {}		\
									\
__P__ ## Ptr::__P__ ## Ptr(const __P__ ## Ptr& o) : TBFC::SPtr(o) {}	\
									\
__P__ ## Ptr::__P__ ## Ptr(const SPtr& o) : TBFC::SPtr(o) {		\
	TBFC::SObject *ot = getRoot();					\
	if ( ot ) {							\
		__P__ *nt = dynamic_cast<__P__ *>( ot );		\
		if ( ! nt ) {						\
			const char *tn = typeName();			\
			operator = ( (TBFC::SObject *)0);		\
			throw TBFC::BadCast(				\
				TBFC::Buffer("Casting ") + tn		\
				+ " to " # __P__ );			\
		}							\
	}								\
}									\
									\
__P__ ## Ptr::~ __P__ ## Ptr() {}					\
									\
__P__ ## Ptr& __P__ ## Ptr::operator = (__P__ *final) {			\
	TBFC::SPtr::operator = (final);					\
	return *this;							\
}									\
									\
__P__ ## Ptr& __P__ ## Ptr::operator = (const __P__ ## Ptr& o) {	\
	TBFC::SPtr::operator = (o);					\
	return *this;							\
}									\
									\
__P__ ## Ptr& __P__ ## Ptr::operator = (const TBFC::SPtr& o) {		\
	TBFC::SPtr::operator = (o);					\
	TBFC::SObject *ot = getRoot();					\
	if ( ot ) {							\
		__P__ *nt = dynamic_cast<__P__ *>( ot );		\
		if ( ! nt ) {						\
			const char *tn = typeName();			\
			operator = ( (TBFC::SObject *)0);		\
			throw TBFC::BadCast(				\
				TBFC::Buffer("Casting ") + tn		\
				+ " to " # __P__ );			\
		}							\
	}								\
	return *this;							\
}									\
									\
__P__ * __P__ ## Ptr::operator -> () const {				\
	TBFC::SObject *ot = getRoot();					\
	if ( ! ot ) {throw TBFC::NullPointer( "Type: " # __P__ );}	\
	__P__ *nt = dynamic_cast<__P__ *>(ot);				\
	if ( ! nt ) {							\
		throw TBFC::BadCast(					\
			TBFC::Buffer("Casting ") + typeName()		\
			+ " to " # __P__ );				\
	}								\
	return nt;							\
}									\
									\
__P__ & __P__ ## Ptr::operator * () const {				\
	TBFC::SObject *ot = getRoot();					\
	if ( ! ot ) {throw TBFC::NullPointer( "Type: " # __P__ );}	\
	__P__ *nt = dynamic_cast<__P__ *>(ot);				\
	if ( ! nt ) {							\
		throw TBFC::BadCast(					\
			TBFC::Buffer("Casting ") + typeName()		\
			+ " to " # __P__ );				\
	}								\
	return (*nt);							\
}

// ============================================================================

/// \brief Macro to define (implement) a class inheriting SPtr, and belonging
///	to some namespace.
///
/// This macro can be used to implement the class declared using a
/// corresponding TBFC_PTR_DECL macro.
///
/// \param __NS__
///	The namespace class \a __P__ belongs to.
///
/// \param __P__ The user class name. This name is used to form the final
///	Smart Pointer class name, which is the concatenation between the
///	user class name and the string "Ptr".
///
/// Example: use JBS's C++ class template instantiator to see how to use
///	this macro.
///
/// \ingroup TBFC_Base

#define TBFC_PTR_IMPL_NS(__NS__,__P__)					\
__NS__::__P__ ## Ptr::__P__ ## Ptr() : TBFC::SPtr() {}			\
									\
__NS__::__P__ ## Ptr::__P__ ## Ptr(__NS__::__P__ *final) : TBFC::SPtr(final) {}		\
									\
__NS__::__P__ ## Ptr::__P__ ## Ptr(const __NS__::__P__ ## Ptr& o) : TBFC::SPtr(o) {}	\
									\
__NS__::__P__ ## Ptr::__P__ ## Ptr(const TBFC::SPtr& o) : TBFC::SPtr(o) {	\
	TBFC::SObject *ot = getRoot();					\
	if ( ot ) {							\
		__P__ *nt = dynamic_cast<__P__ *>( ot );		\
		if ( ! nt ) {						\
			const char *tn = typeName();			\
			operator = ( (TBFC::SObject *)0);		\
			throw TBFC::BadCast(				\
				TBFC::Buffer("Casting ") + tn		\
				+ " to " # __P__ );			\
		}							\
	}								\
}									\
									\
__NS__::__P__ ## Ptr::~ __P__ ## Ptr() {}				\
									\
__NS__::__P__ ## Ptr& __NS__::__P__ ## Ptr::operator = (__NS__::__P__ *final) {			\
	TBFC::SPtr::operator = (final);					\
	return *this;							\
}									\
									\
__NS__::__P__ ## Ptr& __NS__::__P__ ## Ptr::operator = (const __NS__::__P__ ## Ptr& o) {	\
	TBFC::SPtr::operator = (o);					\
	return *this;							\
}									\
									\
__NS__::__P__ ## Ptr& __NS__::__P__ ## Ptr::operator = (const TBFC::SPtr& o) {\
	TBFC::SPtr::operator = (o);					\
	TBFC::SObject *ot = getRoot();					\
	if ( ot ) {							\
		__P__ *nt = dynamic_cast<__P__ *>( ot );		\
		if ( ! nt ) {						\
			const char *tn = typeName();			\
			operator = ( (TBFC::SObject *)0);		\
			throw TBFC::BadCast(				\
				TBFC::Buffer("Casting ") + tn		\
				+ " to " # __P__ );			\
		}							\
	}								\
	return *this;							\
}									\
									\
__NS__::__P__ * __NS__::__P__ ## Ptr::operator -> () const {		\
	TBFC::SObject *ot = getRoot();					\
	if ( ! ot ) {throw TBFC::NullPointer( "Type: " # __P__ );}	\
	__P__ *nt = dynamic_cast<__P__ *>(ot);				\
	if ( ! nt ) {							\
		throw TBFC::BadCast(					\
			TBFC::Buffer("Casting ") + typeName()		\
			+ " to " # __P__ );				\
	}								\
	return nt;							\
}									\
									\
__NS__::__P__ & __NS__::__P__ ## Ptr::operator * () const {		\
	TBFC::SObject *ot = getRoot();					\
	if ( ! ot ) {throw TBFC::NullPointer( "Type: " # __P__ );}	\
	__P__ *nt = dynamic_cast<__P__ *>(ot);				\
	if ( ! nt ) {							\
		throw TBFC::BadCast(					\
			TBFC::Buffer("Casting ") + typeName()		\
			+ " to " # __P__ );				\
	}								\
	return (*nt);							\
}

// ============================================================================

/// \brief Macro to define (implement) an inner class inheriting SPtr, and
///	belonging to some namespace.
///
/// This macro can be used to implement the class declared using a
/// corresponding TBFC_PTR_DECL macro.
///
/// \param __NS__
///	The namespace class \a __P__ belongs to.
///
/// \param __CC__
///	The outer class class \a __P__ is declared in.
///
/// \param __P__ The user class name. This name is used to form the final
///	Smart Pointer class name, which is the concatenation between the
///	user class name and the string "Ptr".
///
/// Example: use JBS's C++ class template instantiator to see how to use
///	this macro.
///
/// \ingroup TBFC_Base

#define TBFC_PTR_IMPL_NS_CC(__NS__,__CC__,__P__)				\
__NS__::__CC__::__P__ ## Ptr::__P__ ## Ptr() : TBFC::SPtr() {}		\
									\
__NS__::__CC__::__P__ ## Ptr::__P__ ## Ptr(__NS__::__CC__::__P__ *final) : TBFC::SPtr(final) {}		\
									\
__NS__::__CC__::__P__ ## Ptr::__P__ ## Ptr(const __NS__::__CC__::__P__ ## Ptr& o) : TBFC::SPtr(o) {}	\
									\
__NS__::__CC__::__P__ ## Ptr::__P__ ## Ptr(const TBFC::SPtr& o) : TBFC::SPtr(o) {	\
	TBFC::SObject *ot = getRoot();					\
	if ( ot ) {							\
		__P__ *nt = dynamic_cast<__P__ *>( ot );		\
		if ( ! nt ) {						\
			const char *tn = typeName();			\
			operator = ( (TBFC::SObject *)0);		\
			throw TBFC::BadCast(				\
				TBFC::Buffer("Casting ") + tn		\
				+ " to " # __P__ );			\
		}							\
	}								\
}									\
									\
__NS__::__CC__::__P__ ## Ptr::~ __P__ ## Ptr() {}			\
									\
__NS__::__CC__::__P__ ## Ptr& __NS__::__CC__::__P__ ## Ptr::operator = (__NS__::__CC__::__P__ *final) {			\
	TBFC::SPtr::operator = (final);					\
	return *this;							\
}									\
									\
__NS__::__CC__::__P__ ## Ptr& __NS__::__CC__::__P__ ## Ptr::operator = (const __NS__::__CC__::__P__ ## Ptr& o) {	\
	TBFC::SPtr::operator = (o);					\
	return *this;							\
}									\
									\
__NS__::__CC__::__P__ ## Ptr& __NS__::__CC__::__P__ ## Ptr::operator = (const TBFC::SPtr& o) {		\
	TBFC::SPtr::operator = (o);					\
	TBFC::SObject *ot = getRoot();					\
	if ( ot ) {							\
		__P__ *nt = dynamic_cast<__P__ *>( ot );		\
		if ( ! nt ) {						\
			const char *tn = typeName();			\
			operator = ( (TBFC::SObject *)0);		\
			throw TBFC::BadCast(				\
				TBFC::Buffer("Casting ") + tn		\
				+ " to " # __P__ );			\
		}							\
	}								\
	return *this;							\
}									\
									\
__NS__::__CC__::__P__ * __NS__::__CC__::__P__ ## Ptr::operator -> () const {		\
	TBFC::SObject *ot = getRoot();					\
	if ( ! ot ) {throw TBFC::NullPointer( "Type: " # __P__ );}	\
	__P__ *nt = dynamic_cast<__P__ *>(ot);				\
	if ( ! nt ) {							\
		throw TBFC::BadCast(					\
			TBFC::Buffer("Casting ") + typeName()		\
			+ " to " # __P__ );				\
	}								\
	return nt;							\
}									\
									\
__NS__::__CC__::__P__ & __NS__::__CC__::__P__ ## Ptr::operator * () const {		\
	TBFC::SObject *ot = getRoot();					\
	if ( ! ot ) {throw TBFC::NullPointer( "Type: " # __P__ );}	\
	__P__ *nt = dynamic_cast<__P__ *>(ot);				\
	if ( ! nt ) {							\
		throw TBFC::BadCast(					\
			TBFC::Buffer("Casting ") + typeName()		\
			+ " to " # __P__ );				\
	}								\
	return (*nt);							\
}

// ============================================================================

/// \brief Macro to define (implement) a class inheriting SPtr, and belonging
///	to 2 nested namespaces.
///
/// This macro can be used to implement the class declared using a
/// corresponding TBFC_PTR_DECL macro.
///
/// \param __NS1__
///	The outer namespace class \a __P__ belongs to.
///
/// \param __NS2__
///	The inner namespace class \a __P__ belongs to.
///
/// \param __P__ The user class name. This name is used to form the final
///	Smart Pointer class name, which is the concatenation between the
///	user class name and the string "Ptr".
///
/// Example: use JBS's C++ class template instantiator to see how to use
///	this macro.
///
/// \ingroup TBFC_Base

#define TBFC_PTR_IMPL_NS2(__NS1__,__NS2__,__P__)				\
__NS1__::__NS2__::__P__ ## Ptr::__P__ ## Ptr() : TBFC::SPtr() {}		\
									\
__NS1__::__NS2__::__P__ ## Ptr::__P__ ## Ptr(__NS1__::__NS2__::__P__ *final) : TBFC::SPtr(final) {}		\
									\
__NS1__::__NS2__::__P__ ## Ptr::__P__ ## Ptr(const __NS1__::__NS2__::__P__ ## Ptr& o) : TBFC::SPtr(o) {}	\
									\
__NS1__::__NS2__::__P__ ## Ptr::__P__ ## Ptr(const TBFC::SPtr& o) : TBFC::SPtr(o) {	\
	TBFC::SObject *ot = getRoot();					\
	if ( ot ) {							\
		__P__ *nt = dynamic_cast<__P__ *>( ot );		\
		if ( ! nt ) {						\
			const char *tn = typeName();			\
			operator = ( (TBFC::SObject *)0);		\
			throw TBFC::BadCast(				\
				TBFC::Buffer("Casting ") + tn		\
				+ " to " # __P__ );			\
		}							\
	}								\
}									\
									\
__NS1__::__NS2__::__P__ ## Ptr::~ __P__ ## Ptr() {}			\
									\
__NS1__::__NS2__::__P__ ## Ptr& __NS1__::__NS2__::__P__ ## Ptr::operator = (__NS1__::__NS2__::__P__ *final) {			\
	TBFC::SPtr::operator = (final);					\
	return *this;							\
}									\
									\
__NS1__::__NS2__::__P__ ## Ptr& __NS1__::__NS2__::__P__ ## Ptr::operator = (const __NS1__::__NS2__::__P__ ## Ptr& o) {	\
	TBFC::SPtr::operator = (o);					\
	return *this;							\
}									\
									\
__NS1__::__NS2__::__P__ ## Ptr& __NS1__::__NS2__::__P__ ## Ptr::operator = (const TBFC::SPtr& o) {		\
	TBFC::SPtr::operator = (o);					\
	TBFC::SObject *ot = getRoot();					\
	if ( ot ) {							\
		__P__ *nt = dynamic_cast<__P__ *>( ot );		\
		if ( ! nt ) {						\
			const char *tn = typeName();			\
			operator = ( (TBFC::SObject *)0);		\
			throw TBFC::BadCast(				\
				TBFC::Buffer("Casting ") + tn		\
				+ " to " # __P__ );			\
		}							\
	}								\
	return *this;							\
}									\
									\
__NS1__::__NS2__::__P__ * __NS1__::__NS2__::__P__ ## Ptr::operator -> () const {		\
	TBFC::SObject *ot = getRoot();					\
	if ( ! ot ) {throw TBFC::NullPointer( "Type: " # __P__ );}	\
	__P__ *nt = dynamic_cast<__P__ *>(ot);				\
	if ( ! nt ) {							\
		throw TBFC::BadCast(					\
			TBFC::Buffer("Casting ") + typeName()		\
			+ " to " # __P__ );				\
	}								\
	return nt;							\
}									\
									\
__NS1__::__NS2__::__P__ & __NS1__::__NS2__::__P__ ## Ptr::operator * () const {		\
	TBFC::SObject *ot = getRoot();					\
	if ( ! ot ) {throw TBFC::NullPointer( "Type: " # __P__ );}	\
	__P__ *nt = dynamic_cast<__P__ *>(ot);				\
	if ( ! nt ) {							\
		throw TBFC::BadCast(					\
			TBFC::Buffer("Casting ") + typeName()		\
			+ " to " # __P__ );				\
	}								\
	return (*nt);							\
}

// ============================================================================

/// \brief Macro to define (implement) an inner class inheriting SPtr, and
///	belonging to 2 nested namespaces.
///
/// This macro can be used to implement the class declared using a
/// corresponding TBFC_PTR_DECL macro.
///
/// \param __NS1__
///	The outer namespace class \a __P__ belongs to.
///
/// \param __NS2__
///	The inner namespace class \a __P__ belongs to.
///
/// \param __CC__
///	The outer class class \a __P__ is declared in.
///
/// \param __P__ The user class name. This name is used to form the final
///	Smart Pointer class name, which is the concatenation between the
///	user class name and the string "Ptr".
///
/// Example: use JBS's C++ class template instantiator to see how to use
///	this macro.
///
/// \ingroup TBFC_Base

#define TBFC_PTR_IMPL_NS2_CC(__NS1__,__NS2__,__CC__,__P__)		\
__NS1__::__NS2__::__CC__::__P__ ## Ptr::__P__ ## Ptr() : TBFC::SPtr() {}	\
									\
__NS1__::__NS2__::__CC__::__P__ ## Ptr::__P__ ## Ptr(__NS1__::__NS2__::__CC__::__P__ *final) : TBFC::SPtr(final) {}		\
									\
__NS1__::__NS2__::__CC__::__P__ ## Ptr::__P__ ## Ptr(const __NS1__::__NS2__::__CC__::__P__ ## Ptr& o) : TBFC::SPtr(o) {}	\
									\
__NS1__::__NS2__::__CC__::__P__ ## Ptr::__P__ ## Ptr(const TBFC::SPtr& o) : TBFC::SPtr(o) {	\
	TBFC::SObject *ot = getRoot();					\
	if ( ot ) {							\
		__P__ *nt = dynamic_cast<__P__ *>( ot );		\
		if ( ! nt ) {						\
			const char *tn = typeName();			\
			operator = ( (TBFC::SObject *)0);		\
			throw TBFC::BadCast(				\
				TBFC::Buffer("Casting ") + tn		\
				+ " to " # __P__ );			\
		}							\
	}								\
}									\
									\
__NS1__::__NS2__::__CC__::__P__ ## Ptr::~ __P__ ## Ptr() {}		\
									\
__NS1__::__NS2__::__CC__::__P__ ## Ptr& __NS1__::__NS2__::__CC__::__P__ ## Ptr::operator = (__NS1__::__NS2__::__CC__::__P__ *final) {			\
	TBFC::SPtr::operator = (final);					\
	return *this;							\
}									\
									\
__NS1__::__NS2__::__CC__::__P__ ## Ptr& __NS1__::__NS2__::__CC__::__P__ ## Ptr::operator = (const __NS1__::__NS2__::__CC__::__P__ ## Ptr& o) {	\
	TBFC::SPtr::operator = (o);					\
	return *this;							\
}									\
									\
__NS1__::__NS2__::__CC__::__P__ ## Ptr& __NS1__::__NS2__::__CC__::__P__ ## Ptr::operator = (const TBFC::SPtr& o) {		\
	TBFC::SPtr::operator = (o);					\
	TBFC::SObject *ot = getRoot();					\
	if ( ot ) {							\
		__P__ *nt = dynamic_cast<__P__ *>( ot );		\
		if ( ! nt ) {						\
			const char *tn = typeName();			\
			operator = ( (TBFC::SObject *)0);		\
			throw TBFC::BadCast(				\
				TBFC::Buffer("Casting ") + tn		\
				+ " to " # __P__ );			\
		}							\
	}								\
	return *this;							\
}									\
									\
__NS1__::__NS2__::__CC__::__P__ * __NS1__::__NS2__::__CC__::__P__ ## Ptr::operator -> () const {		\
	TBFC::SObject *ot = getRoot();					\
	if ( ! ot ) {throw TBFC::NullPointer( "Type: " # __P__ );}	\
	__P__ *nt = dynamic_cast<__P__ *>(ot);				\
	if ( ! nt ) {							\
		throw TBFC::BadCast(					\
			TBFC::Buffer("Casting ") + typeName()		\
			+ " to " # __P__ );				\
	}								\
	return nt;							\
}									\
									\
__NS1__::__NS2__::__CC__::__P__ & __NS1__::__NS2__::__CC__::__P__ ## Ptr::operator * () const {		\
	TBFC::SObject *ot = getRoot();					\
	if ( ! ot ) {throw TBFC::NullPointer( "Type: " # __P__ );}	\
	__P__ *nt = dynamic_cast<__P__ *>(ot);				\
	if ( ! nt ) {							\
		throw TBFC::BadCast(					\
			TBFC::Buffer("Casting ") + typeName()		\
			+ " to " # __P__ );				\
	}								\
	return (*nt);							\
}

// ============================================================================

#endif // _TBFC_Base_SPtrImpl_H_

// ============================================================================

