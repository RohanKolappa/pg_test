// ============================================================================
//
// Copyright (c) 2003-2014 Barco N.V.
//
// ============================================================================
//
// Filename:
//	VMP.L2.Engine.h
//
// Description:
//	...
//
// Author(s):
//	Jean-Francois GOBBERS
//
// ============================================================================

#ifndef _VMP_L2_Engine_H_
#define _VMP_L2_Engine_H_

// ============================================================================

#include "VMP.L2.DLL.h"

// ============================================================================

#include "VMP.L2.EnginePtr.h"

// ============================================================================

#include "BFC.Base.UUId.h"

#include "BFC.DL.Object.h"
#include "BFC.DL.Type.h"

#include "BFC.Dyn.VarUser.h"

#include "BFC.Msg.Observable.h"
#include "BFC.Msg.Observer.h"

#include "VMP.DynVar.h"

#include "VMP.L2.AlreadyInSession.h"
#include "VMP.L2.BrokenSession.h"
#include "VMP.L2.EndOfStream.h"
#include "VMP.L2.NotInSession.h"
#include "VMP.L2.NotSuitable.h"

#include "VMP.L3.Backend.h"

#include "VMP.UI.IElement.h"
#include "VMP.UI.UObject.h"

// ============================================================================

namespace VMP {
namespace L2 {

// ============================================================================

/// \brief Base class of all L2 Engine objects.
///
/// An Engine represents an abstract data processing object.
///
/// An Engine can work in one of two modes: Pull or Push.
///
/// An Engine can get its input data from 0, 1, or more input Engine object(s),
/// and send its output data to 0, 1, or more output Engine object(s). The
/// exact number of inputs and outputs, and the type of data, are
/// coded in subclass constructors.
///
/// An Engine is a state machine, which at any point in time must be in
/// exactly one of the predefined states:
/// -	\c Inexistent: The engine doesn't exist at all.
/// -	\c Created: The engine has been created, but is not initialized.
/// -	\c Stopped: The engine has been created and initialized, but cannot
///	process data.
/// -	\c Started: The engine is fully functional.
///
/// The following state transitions are available:
/// -	\c Inexistent to \c Created:
///	The ctor is called. The subclass must set its specific class type, and the
///	number and types of inputs and outputs (see setType(),
///	setNbrInputs(), setInputType(), setNbrOutputs(),
///	setOutputType()). If it uses an L3::Backend, it must attach it
///	now too (see setBackend()).
/// -	\c Created to \c Stopped:
///	The user interface is first attached using setUserInterface(), then
///	the settings are restored (if any) using setUserSettings().
/// -	\c Stopped to \c Started:
///	The engine is started by a call to startEngine().
/// -	\c Started to \c Stopped: 
///	The engine is stopped by a call to stopEngine().
/// -	\c Stopped to \c Inexistent:
///	The dtor is called.
///
/// \ingroup VMP_L2

class VMP_L2_DLL Engine :

	public UI::UObject,
	public BFC::Msg::Observable,
	public BFC::Msg::Observer,
	public BFC::Dyn::VarUser {

public :

	/// \brief Returns the unique identifier of this class type.

	static const BFC::DL::TypeCPtr & getClassType(
	);

	/// \brief List of modes supported by Engine objects.

	enum Mode {

		/// \brief Pull mode (i.e. Producer mode).
		///
		/// An Engine working in Pull mode can be asked to \b produce
		/// Session objects or Frame objects.
		///
		/// \sa PullEngine

		Pull = 0,

		/// \brief Push mode (i.e. Consumer mode).
		///
		/// An Engine working in Push mode can be asked to \b consume
		/// Session objects or Frame objects.
		///
		/// \sa PushEngine

		Push,

		Prod,

		Cons

	};

protected :

	/// \brief Creates a new Engine working in mode \a pMode.

	Engine(
		const	Mode			pMode
	);

	/// \brief Creates a new Engine working in mode \a pMode.

	Engine(
		const	Mode			pMode,
		const	BFC::DL::TypeCPtr &	pType
	);

public :

//	virtual void setObjectName(
//		const	BFC::Buffer &		pName
//	);

	/// \brief Returns the unique id assigned to this Engine.

	const BFC::UUId & getUUId(
	) const;

	/// \brief Returns the Mode this engine works in.

	Mode getMode(
	) const;

	/// \brief Returns the number of inputs this Engine communicates with.

	BFC::Uint32 getNbrInputs(
	) const;

	/// \brief Returns the data type the input with index \a pIndex should
	///	produce.

	const BFC::DL::TypeCPtr & getInputType(
		const	BFC::Uint32		pIndex
	) const;

	/// \brief Returns the number of outputs this Engine communicates with.

	BFC::Uint32 getNbrOutputs(
	) const;

	/// \brief Returns the data type the output with index \a pIndex should
	///	consume.

	const BFC::DL::TypeCPtr & getOutputType(
		const	BFC::Uint32		pIndex
	) const;

	/// \brief Attaches a User Interface to this Engine.
	///
	/// If this Engine uses an L3::Backend, then \a pUserInterface is
	/// attached to this backend via a call to L3::Backend::setUserInterface().
	/// Otherwise, \a pUserInterface is attached to this object, and the
	/// initUICallback() method is called.

	virtual void setUserInterface(
			UI::IElementPtr		pUserInterface
	);

	/// \brief Returns the User Interface this Engine is attached to, if
	///	any.
	///
	/// If this Engine uses an L3::Backend, then the call is transfered to
	/// this backend via the L3::Backend::getUserInterface() method.
	/// Otherwise, the user interface set by a previous call to
	/// setUserInterface() is returned (can be null).

	virtual UI::IElementPtr getUserInterface(
	) const;

protected :

//	/// \brief Initializes the user interface.
//	///
//	/// Default is to print a warning message.
//
//	virtual void initUICallback(
//			UI::IElementPtr		ui
//	);

public :

	/// \brief Returns a snapshot of the Engine status.
	///
	/// This method should be reimplemented in subclasses where status
	/// information can be saved and later restored.
	///
	/// Default is to transfer this call to the attached L3::Backend,
	/// if one has been previously attached,
	/// or to return an empty DynVar.

	virtual DynVar getUserSettings(
	);

	/// \brief Restores the status of this Engine, based on \a pSettings.
	///
	/// This method should be reimplemented in subclasses where status
	/// information can be saved and later restored.
	///
	/// Default it to transfer this call to the attached L3::Backend,
	/// if one has been previously attached,
	/// or to print a warning message.

	virtual void setUserSettings(
		const	DynVar&			pSettings
	);

	/// \brief "Starts" this Engine.
	///
	/// This method should be reimplemented by subclasses to perform
	/// additional initialization (see lifecycle).
	///
	/// Default is to call the L3::Backend::startBackend() method of
	/// the attached L3::Backend, if one has been previously attached.

	virtual void startEngine(
	);

	/// \brief "Stops" this Engine.
	///
	/// This method should be reimplemented by subclasses to perform
	/// additional finalization (see lifecycle diagram).
	///
	/// Default is to call the L3::Backend::stopBackend() method of
	/// the attached L3::Backend, if one has been previously attached.

	virtual void stopEngine(
	);

	BFC::Bool isInSession(
	) const {
		return inSession;
	}

	L3::BackendPtr getBackend(
	) {
		return backend;
	}

protected :

	/// \brief Attaches the L3::Backend \a pBackend to this Engine.
	///
	/// If this Engine should an L3::Backend at all, it should attach it
	/// at object creation time (i.e. in the constructor(s)).
	///
	/// When an L3::Backend is attached this way, the following calls are
	/// automatically transfered to it:
	/// -	startEngine() / stopEngine()
	/// -	setUserInterface() / getUserInterface()
	/// -	setUserSettings() / getUserSettings()

	void setBackend(
			L3::BackendPtr		pBackend
	);

	/// \brief Sets the number of inputs this Engine will communicate with
	///	to pSize.
	///
	/// This method must be called at object creation time (i.e. in the
	/// constructor(s)).

	virtual void setNbrInputs(
		const	BFC::Uint32		pSize
	);

	/// \brief Sets the data type the input with index \a pIndex should
	///	produce to \a pType.
	///
	/// This method must be called at object creation time (i.e. in the
	/// constructor(s)).

	void setInputType(
		const	BFC::Uint32		pIndex,
		const	BFC::DL::TypeCPtr &	pType
	);

	/// \brief Sets the number of outputs this Engine will communicate with
	///	to \a pSize.
	///
	/// This method must be called at object creation time (i.e. in the
	/// constructor(s)).

	virtual void setNbrOutputs(
		const	BFC::Uint32		pSize
	);

	/// \brief Sets the data type the output with index \a pIndex should
	///	consume to \a pType.
	///
	/// This method must be called at object creation time (i.e. in the
	/// constructor(s)).

	void setOutputType(
		const	BFC::Uint32		pIndex,
		const	BFC::DL::TypeCPtr &	pType
	);

	void setInSession(
		const	BFC::Bool		pFlag
	) {
		inSession = pFlag;
	}

private :

	typedef BFC::Array< BFC::DL::TypeCPtr >	ClassTypeArray;

	BFC::UUId		uuid;		///< Unique id.
	L3::BackendPtr		backend;	///< L3 Backend.
	Mode			mode;		///< Working mode.
	ClassTypeArray		iTypes;		///< Input data types.
	ClassTypeArray		oTypes;		///< Output data types.
	BFC::Bool		inSession;	///< Is in session ?

	/// \brief Forbidden copy constructor.

	Engine(
		const	Engine &
	);

	/// \brief Forbidden copy operator.

	Engine & operator = (
		const	Engine &
	);

};

// ============================================================================

} // namespace L2
} // namespace VMP

// ============================================================================

#endif // _VMP_L2_Engine_H_

// ============================================================================

