// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// Filename:
//	VMP.UI.IListView.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _VMP_UI_IListView_H_
#define _VMP_UI_IListView_H_

// ============================================================================

#include "VMP.UI.DLL.h"

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace VMP {
namespace UI {

BFC_PTR_DECL(VMP_UI_DLL,IListView)

} // namespace UI
} // namespace VMP

// ============================================================================

#include "VMP.UI.IElement.h"

// ============================================================================

namespace VMP {
namespace UI {

// ============================================================================

/// \brief Abstract list view.
///
/// \ingroup VMP_UI

class VMP_UI_DLL IListView : virtual public IElement {

public :

	IListView(
	);

	// Sample API...

//	virtual int addColumn ( const QString & label, int width = -1 )
//	virtual int addColumn ( const QIconSet & iconset, const QString & label, int width = -1 )
//	virtual void removeColumn ( int index )
//	virtual void setColumnText ( int column, const QString & label )
//	virtual void setColumnText ( int column, const QIconSet & iconset, const QString & label )
//	virtual QString columnText ( int c ) const

	virtual void setProperty(
		const	BFC::Buffer&		propertyName,
		const	BFC::Buffer&		propertyValue
	);

	virtual BFC::Buffer getProperty(
		const	BFC::Buffer&		propertyName
	) const;

protected :

	virtual void sendEvent(
	) const;

private :

	// internal state...

//	static BFC::Buffer	textTag;

	// others...

	IListView(
		const	IListView&
	);

	IListView& operator = (
		const	IListView&
	);

};

// ============================================================================

} // namespace UI
} // namespace VMP

// ============================================================================

#endif // _VMP_UI_IListView_H_

// ============================================================================

