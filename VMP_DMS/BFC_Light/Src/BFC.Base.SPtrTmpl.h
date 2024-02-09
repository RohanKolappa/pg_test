// ============================================================================
// 
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
//	BFC.Base.SPtrTmpl.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_Base_SPtrTmpl_H_
#define _BFC_Base_SPtrTmpl_H_

// ============================================================================

#include "BFC.Light.DLL.h"

// ============================================================================

#include "BFC.Base.SPtrDecl.h"

// ============================================================================

/// \brief Macro to declare a template class inheriting SPtr.
///
/// This macro can be used to easily and rapidly declare a new template Smart
/// Pointer class to some template user class. See the dummy SampleClass
/// and SampleClassPtr
/// classes documentation to further understand the Smart Pointer
/// mechanism.
///
/// \param __P__ The user class name. This name is used to form the final
///	Smart Pointer class name, which is the concatenation between the
///	user class name and the string "Ptr".
///
/// Example: use JBS's C++ class template instantiator to see how to use
///	this macro.
///
/// \ingroup BFC_Base

#define BFC_PTR_TMPL( __P__ )						\
									\
template < typename TElt >						\
class __P__;								\
									\
template < typename TElt >						\
class __P__ ## CPtr : public BFC::SCPtr {				\
									\
public :								\
									\
	__P__ ## CPtr(							\
	) {								\
	}								\
									\
	__P__ ## CPtr(							\
		const	__P__< TElt > *		final,			\
		const	BFC::Bool		pMaster = true,		\
		const	BFC::Bool		pSticky = false		\
	);								\
									\
	__P__ ## CPtr(							\
		const	BFC::SCPtr &		other,			\
		const	BFC::Bool		pMaster = true,		\
		const	BFC::Bool		pSticky = false		\
	);								\
									\
	__P__ ## CPtr & operator = (					\
		const	__P__< TElt > *		final			\
	);								\
									\
	__P__ ## CPtr & operator = (					\
		const	BFC::SCPtr &		other			\
	);								\
									\
	const __P__< TElt > * operator -> (				\
	) const;							\
									\
	const __P__< TElt > & operator * (				\
	) const;							\
									\
};									\
									\
template < typename TElt >						\
class __P__ ## Ptr : public BFC::SPtr {					\
									\
public :								\
									\
	__P__ ## Ptr(							\
	) {								\
	}								\
									\
	__P__ ## Ptr(							\
			__P__< TElt > *		final,			\
		const	BFC::Bool		pMaster = true,		\
		const	BFC::Bool		pSticky = false		\
	);								\
									\
	__P__ ## Ptr(							\
		const	BFC::SPtr &		other,			\
		const	BFC::Bool		pMaster = true,		\
		const	BFC::Bool		pSticky = false		\
	);								\
									\
	__P__ ## Ptr & operator = (					\
			__P__< TElt > *		final			\
	);								\
									\
	__P__ ## Ptr & operator = (					\
		const	BFC::SPtr &		other			\
	);								\
									\
	__P__< TElt > * operator -> (					\
	) const;							\
									\
	__P__< TElt > & operator * (					\
	) const;							\
									\
};									\
									\
template < typename TElt >						\
__P__ ## CPtr< TElt >::__P__ ## CPtr(					\
	const	__P__< TElt > *	final,					\
	const	BFC::Bool	pMaster,				\
	const	BFC::Bool	pSticky ) :				\
									\
	BFC::SCPtr( final, pMaster, pSticky ) {				\
									\
}									\
									\
template < typename TElt >						\
__P__ ## CPtr< TElt >::__P__ ## CPtr(					\
	const	BFC::SCPtr &	other,					\
	const	BFC::Bool	pMaster,				\
	const	BFC::Bool	pSticky ) :				\
									\
	BFC::SCPtr( other, pMaster, pSticky ) {				\
									\
	checkCastPtr< __P__< TElt > >( # __P__ );			\
									\
}									\
									\
template < typename TElt >						\
__P__ ## CPtr< TElt > & __P__ ## CPtr< TElt >::operator = (		\
	const	__P__< TElt > *	final ) {				\
									\
	BFC::SCPtr::operator = ( final );				\
	return *this;							\
									\
}									\
									\
template < typename TElt >						\
__P__ ## CPtr< TElt > & __P__ ## CPtr< TElt >::operator = (		\
	const	BFC::SCPtr &			other ) {		\
									\
	BFC::SCPtr::operator = ( other );				\
	checkCastPtr< __P__< TElt > >( # __P__ );			\
	return *this;							\
									\
}									\
									\
template < typename TElt >						\
const __P__< TElt > * __P__ ## CPtr< TElt >::operator -> () const {	\
									\
	return getCastPtr< __P__< TElt > >( # __P__ );			\
									\
}									\
									\
template < typename TElt >						\
const __P__< TElt > & __P__ ## CPtr< TElt >::operator * () const {	\
									\
	return *getCastPtr< __P__< TElt > >( # __P__ );			\
									\
}									\
									\
template < typename TElt >						\
__P__ ## Ptr< TElt >::__P__ ## Ptr(					\
		__P__< TElt > *	final,					\
	const	BFC::Bool	pMaster,				\
	const	BFC::Bool	pSticky ) :				\
									\
	BFC::SPtr( final, pMaster, pSticky ) {				\
									\
}									\
									\
template < typename TElt >						\
__P__ ## Ptr< TElt >::__P__ ## Ptr(					\
	const	BFC::SPtr &	other,					\
	const	BFC::Bool	pMaster,				\
	const	BFC::Bool	pSticky ) :				\
									\
	BFC::SPtr( other, pMaster, pSticky ) {				\
									\
	checkCastPtr< __P__< TElt > >( # __P__ );			\
									\
}									\
									\
template < typename TElt >						\
__P__ ## Ptr< TElt > & __P__ ## Ptr< TElt >::operator = (		\
		__P__< TElt > *	final ) {				\
									\
	BFC::SPtr::operator = ( final );				\
	return *this;							\
									\
}									\
									\
template < typename TElt >						\
__P__ ## Ptr< TElt > & __P__ ## Ptr< TElt >::operator = (		\
	const	BFC::SPtr &			other ) {		\
									\
	BFC::SPtr::operator = ( other );				\
	checkCastPtr< __P__< TElt > >( # __P__ );			\
	return *this;							\
									\
}									\
									\
template < typename TElt >						\
__P__< TElt > * __P__ ## Ptr< TElt >::operator -> () const {		\
									\
	return getCastPtr< __P__< TElt > >( # __P__ );			\
									\
}									\
									\
template < typename TElt >						\
__P__< TElt > & __P__ ## Ptr< TElt >::operator * () const {		\
									\
	return *getCastPtr< __P__< TElt > >( # __P__ );			\
									\
}

// ============================================================================

/// \brief Macro to declare a template class inheriting SPtr.
///
/// This macro can be used to easily and rapidly declare a new template Smart
/// Pointer class to some template user class. See the dummy SampleClass
/// and SampleClassPtr
/// classes documentation to further understand the Smart Pointer
/// mechanism.
///
/// \param __P__ The user class name. This name is used to form the final
///	Smart Pointer class name, which is the concatenation between the
///	user class name and the string "Ptr".
///
/// Example: use JBS's C++ class template instantiator to see how to use
///	this macro.
///
/// \ingroup BFC_Base

#define BFC_PTR_TMPL2( __P__ )						\
									\
template < typename TElt1, typename TElt2 >				\
class __P__;								\
									\
template < typename TElt1, typename TElt2 >				\
class __P__ ## CPtr : public BFC::SCPtr {				\
									\
public :								\
									\
	typedef __P__ ## CPtr< TElt1, TElt2 >		ThisType;	\
									\
	__P__ ## CPtr(							\
	) {								\
	}								\
									\
	__P__ ## CPtr(							\
		const	ThisType *		final,			\
		const	BFC::Bool		pMaster = true,		\
		const	BFC::Bool		pSticky = false		\
	);								\
									\
	__P__ ## CPtr(							\
		const	BFC::SCPtr &		other,			\
		const	BFC::Bool		pMaster = true,		\
		const	BFC::Bool		pSticky = false		\
	);								\
									\
	__P__ ## CPtr & operator = (					\
		const	ThisType *		final			\
	);								\
									\
	__P__ ## CPtr & operator = (					\
		const	BFC::SCPtr &		other			\
	);								\
									\
	const ThisType * operator -> (					\
	) const;							\
									\
	const ThisType & operator * (					\
	) const;							\
									\
};									\
									\
template < typename TElt1, typename TElt2 >				\
class __P__ ## Ptr : public BFC::SPtr {					\
									\
public :								\
									\
	typedef __P__ ## Ptr< TElt1, TElt2 >		ThisType;	\
									\
	__P__ ## Ptr(							\
	) {								\
	}								\
									\
	__P__ ## Ptr(							\
			ThisType *		final,			\
		const	BFC::Bool		pMaster = true,		\
		const	BFC::Bool		pSticky = false		\
	);								\
									\
	__P__ ## Ptr(							\
		const	BFC::SPtr &		other,			\
		const	BFC::Bool		pMaster = true,		\
		const	BFC::Bool		pSticky = false		\
	);								\
									\
	__P__ ## Ptr & operator = (					\
			ThisType *		final			\
	);								\
									\
	__P__ ## Ptr & operator = (					\
		const	BFC::SPtr &		other			\
	);								\
									\
	ThisType * operator -> (					\
	) const;							\
									\
	ThisType & operator * (						\
	) const;							\
									\
};									\
									\
template < typename TElt1, typename TElt2 >				\
__P__ ## CPtr< TElt1, TElt2 >::__P__ ## CPtr(				\
	const	ThisType *	final,					\
	const	BFC::Bool	pMaster,				\
	const	BFC::Bool	pSticky ) :				\
									\
	BFC::SCPtr( final, pMaster, pSticky ) {				\
									\
}									\
									\
template < typename TElt1, typename TElt2 >				\
__P__ ## CPtr< TElt1, TElt2 >::__P__ ## CPtr(				\
	const	BFC::SCPtr &	other,					\
	const	BFC::Bool	pMaster,				\
	const	BFC::Bool	pSticky ) :				\
									\
	BFC::SCPtr( other, pMaster, pSticky ) {				\
									\
	checkCastPtr< ThisType >( # __P__ );				\
									\
}									\
									\
template < typename TElt1, typename TElt2 >				\
__P__ ## CPtr< TElt1, TElt2 > & __P__ ## CPtr< TElt1, TElt2 >::operator = (\
	const	ThisType *	final ) {				\
									\
	BFC::SCPtr::operator = ( final );				\
	return *this;							\
									\
}									\
									\
template < typename TElt1, typename TElt2 >				\
__P__ ## CPtr< TElt1, TElt2 > & __P__ ## CPtr< TElt1, TElt2 >::operator = (\
	const	BFC::SCPtr &			other ) {		\
									\
	BFC::SCPtr::operator = ( other );				\
	checkCastPtr< ThisType >( # __P__ );				\
	return *this;							\
									\
}									\
									\
template < typename TElt1, typename TElt2 >				\
const __P__ ## CPtr< TElt1, TElt2 > * __P__ ## CPtr< TElt1, TElt2 >::operator -> () const {	\
									\
	return getCastPtr< ThisType >( # __P__ );			\
									\
}									\
									\
template < typename TElt1, typename TElt2 >				\
const __P__ ## CPtr< TElt1, TElt2 > & __P__ ## CPtr< TElt1, TElt2 >::operator * () const {	\
									\
	return *getCastPtr< ThisType >( # __P__ );			\
									\
}									\
									\
template < typename TElt1, typename TElt2 >				\
__P__ ## Ptr< TElt1, TElt2 >::__P__ ## Ptr(				\
		ThisType *	final,					\
	const	BFC::Bool	pMaster,				\
	const	BFC::Bool	pSticky ) :				\
									\
	BFC::SPtr( final, pMaster, pSticky ) {				\
									\
}									\
									\
template < typename TElt1, typename TElt2 >				\
__P__ ## Ptr< TElt1, TElt2 >::__P__ ## Ptr(				\
	const	BFC::SPtr &	other,					\
	const	BFC::Bool	pMaster,				\
	const	BFC::Bool	pSticky) :				\
									\
	BFC::SPtr( other, pMaster, pSticky ) {				\
									\
	checkCastPtr< ThisType >( # __P__ );				\
									\
}									\
									\
template < typename TElt1, typename TElt2 >				\
__P__ ## Ptr< TElt1, TElt2 > & __P__ ## Ptr< TElt1, TElt2 >::operator = (\
		ThisType *	final ) {				\
									\
	BFC::SPtr::operator = ( final );				\
	return *this;							\
									\
}									\
									\
template < typename TElt1, typename TElt2 >				\
__P__ ## Ptr< TElt1, TElt2 > & __P__ ## Ptr< TElt1, TElt2 >::operator = (		\
	const	BFC::SPtr &			other ) {		\
									\
	BFC::SPtr::operator = ( other );				\
	checkCastPtr< ThisType >( # __P__ );				\
	return *this;							\
									\
}									\
									\
template < typename TElt1, typename TElt2 >				\
__P__ ## Ptr< TElt1, TElt2 > * __P__ ## Ptr< TElt1, TElt2 >::operator -> () const {		\
									\
	return getCastPtr< ThisType >( # __P__ );			\
									\
}									\
									\
template < typename TElt1, typename TElt2 >				\
__P__ ## Ptr< TElt1, TElt2 > & __P__ ## Ptr< TElt1, TElt2 >::operator * () const {		\
									\
	return *getCastPtr< ThisType >( # __P__ );			\
									\
}

// ============================================================================

#endif // _BFC_Base_SPtrTmpl_H_

// ============================================================================

