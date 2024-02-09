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
//	BFC.Base.SPtrImpl.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_Base_SPtrImpl_H_
#define _BFC_Base_SPtrImpl_H_

// ============================================================================

#include "BFC.Light.DLL.h"

// ============================================================================

#include "BFC.Base.Buffer.h"
#include "BFC.Base.Exception.h"
#include "BFC.Base.SPtrDecl.h"

// ============================================================================

/// \brief Helper macro.
///
/// \ingroup BFC_Base

#define BFC_PTR_IMPL_PRIV( FQ, NQ )					\
									\
FQ ## CPtr::NQ ## CPtr(							\
	const	FQ *		pFinal,					\
	const	BFC::Bool	pMaster,				\
	const	BFC::Bool	pSticky ) :				\
									\
	BFC::SCPtr( pFinal, pMaster, pSticky ) {			\
									\
}									\
									\
FQ ## CPtr::NQ ## CPtr(							\
	const	BFC::SCPtr &	pOther,					\
	const	BFC::Bool	pMaster,				\
	const	BFC::Bool	pSticky ) :				\
									\
	BFC::SCPtr( pOther, pMaster, pSticky ) {			\
									\
	checkCastPtr< NQ >( # NQ );					\
									\
}									\
									\
FQ ## CPtr & FQ ## CPtr::operator = (					\
	const	FQ *		pFinal ) {				\
									\
	BFC::SCPtr::operator = ( pFinal );				\
	return *this;							\
									\
}									\
									\
FQ ## CPtr & FQ ## CPtr::operator = (					\
	const	BFC::SCPtr &	pOther ) {				\
									\
	BFC::SCPtr::operator = ( pOther );				\
	checkCastPtr< NQ >( # NQ );					\
	return *this;							\
									\
}									\
									\
const FQ * FQ ## CPtr::operator -> () const {				\
									\
	return getCastPtr< NQ >( # NQ );				\
									\
}									\
									\
const FQ & FQ ## CPtr::operator * () const {				\
									\
	return *getCastPtr< NQ >( # NQ );				\
									\
}									\
									\
FQ ## Ptr::NQ ## Ptr(							\
		FQ *		pFinal,					\
	const	BFC::Bool	pMaster,				\
	const	BFC::Bool	pSticky ) :				\
									\
	BFC::SPtr( pFinal, pMaster, pSticky ) {				\
									\
}									\
									\
FQ ## Ptr::NQ ## Ptr(							\
	const	BFC::SPtr &	pOther,					\
	const	BFC::Bool	pMaster,				\
	const	BFC::Bool	pSticky ) :				\
									\
	BFC::SPtr( pOther, pMaster, pSticky ) {				\
									\
	checkCastPtr< NQ >( # NQ );					\
									\
}									\
									\
FQ ## Ptr & FQ ## Ptr::operator = (					\
		FQ *		pFinal ) {				\
									\
	BFC::SPtr::operator = ( pFinal );				\
	return *this;							\
									\
}									\
									\
FQ ## Ptr & FQ ## Ptr::operator = (					\
	const	BFC::SPtr &	pOther ) {				\
									\
	BFC::SPtr::operator = ( pOther );				\
	checkCastPtr< NQ >( # NQ );					\
	return *this;							\
									\
}									\
									\
FQ * FQ ## Ptr::operator -> () const {					\
									\
	return getCastPtr< NQ >( # NQ );				\
									\
}									\
									\
FQ & FQ ## Ptr::operator * () const {					\
									\
	return *getCastPtr< NQ >( # NQ );				\
									\
}

// ============================================================================

/// \brief Macro to define (implement) a class inheriting SPtr.
///
/// This macro can be used to implement the class declared using a
/// corresponding BFC_PTR_DECL macro.
///
/// \param __P__ The user class name. This name is used to form the final
///	Smart Pointer class name, which is the concatenation between the
///	user class name and the string "Ptr".
///
/// Example: use JBS's C++ class template instantiator to see how to use
///	this macro.
///
/// \ingroup BFC_Base

#define BFC_PTR_IMPL( __P__ ) BFC_PTR_IMPL_PRIV( __P__, __P__ )

// ============================================================================

/// \brief Macro to define (implement) a class inheriting SPtr, and belonging
///	to some namespace.
///
/// This macro can be used to implement the class declared using a
/// corresponding BFC_PTR_DECL macro.
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
/// \ingroup BFC_Base

#define BFC_PTR_IMPL_NS( __NS__, __P__ )				\
	BFC_PTR_IMPL_PRIV( __NS__::__P__, __P__ )

// ============================================================================

/// \brief Macro to define (implement) a class inheriting SPtr, and belonging
///	to 2 nested namespaces.
///
/// This macro can be used to implement the class declared using a
/// corresponding BFC_PTR_DECL macro.
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
/// \ingroup BFC_Base

#define BFC_PTR_IMPL_NS2( __NS1__, __NS2__, __P__ )			\
	BFC_PTR_IMPL_PRIV( __NS1__::__NS2__::__P__, __P__ )

// ============================================================================

/// \brief Macro to define (implement) a class inheriting SPtr, and belonging
///	to 3 nested namespaces.
///
/// This macro can be used to implement the class declared using a
/// corresponding BFC_PTR_DECL macro.
///
/// \param __NS1__
///	The outer namespace class \a __P__ belongs to.
///
/// \param __NS2__
///	The inner namespace class \a __P__ belongs to.
///
/// \param __NS3__
///	The inner namespace class \a __P__ belongs to.
///
/// \param __P__ The user class name. This name is used to form the final
///	Smart Pointer class name, which is the concatenation between the
///	user class name and the string "Ptr".
///
/// Example: use JBS's C++ class template instantiator to see how to use
///	this macro.
///
/// \ingroup BFC_Base

#define BFC_PTR_IMPL_NS3( __NS1__, __NS2__, __NS3__, __P__ )		\
	BFC_PTR_IMPL_PRIV( __NS1__::__NS2__::__NS3__::__P__, __P__ )

// ============================================================================

/// \brief Macro to define (implement) an inner class inheriting SPtr.
///
/// This macro can be used to implement the class declared using a
/// corresponding BFC_PTR_DECL macro.
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
/// \ingroup BFC_Base

#define BFC_PTR_IMPL_CC( __CC__, __P__ )				\
	BFC_PTR_IMPL_NS( __CC__, __P__ )

// ============================================================================

/// \brief Macro to define (implement) an inner class inheriting SPtr, and
///	belonging to some namespace.
///
/// This macro can be used to implement the class declared using a
/// corresponding BFC_PTR_DECL macro.
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
/// \ingroup BFC_Base

#define BFC_PTR_IMPL_NS_CC( __NS__, __CC__, __P__ )			\
	BFC_PTR_IMPL_NS2( __NS__, __CC__, __P__ )

// ============================================================================

#define BFC_PTR_IMPL_NS2_CC( __NS1__, __NS2__, __CC__, __P__ )		\
	BFC_PTR_IMPL_NS3( __NS1__, __NS2__, __CC__, __P__ )

// ============================================================================

#endif // _BFC_Base_SPtrImpl_H_

// ============================================================================

