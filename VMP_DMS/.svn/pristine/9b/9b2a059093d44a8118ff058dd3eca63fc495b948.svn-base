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
//	BFC.TL.RBTree.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_TL_RBTree_H_
#define _BFC_TL_RBTree_H_

// ============================================================================

#include "BFC.Light.DLL.h"

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace BFC {

BFC_PTR_DECL( BFC_Light_DLL, RBTree )

} // namespace BFC

// ============================================================================

#include "BFC.Base.SObject.h"

// ============================================================================

namespace BFC {

// ============================================================================

/// \brief Individual cell in a RBTree.
///
/// \ingroup BFC_TL

class BFC_Light_DLL RBCell {

public :

	RBCell(
			RBCell *	__left,
			RBCell *	__right,
			RBCell *	__up,
			Bool		__red,
		const	void *		__key
	) :
		left( __left ),
		right( __right ),
		up( __up ),
		red( __red ),
		key( __key ) {
	}

	~RBCell(
	);

	RBCell * getSucc(
	);

	RBCell * getPred(
	);

	RBCell *	left;		///< Left down.
	RBCell *	right;		///< Right down.
	RBCell *	up;		///< Up.
	Bool		red;		///< Node col is red ?
	const void *	key;		///< Pointer to user's key (and data).

private :

	RBCell(
	);

};

// ============================================================================

/// \brief Red-black tree.
///
/// The algorithm is the fairly standard red/black taken from "Introduction to
/// Algorithms" by Cormen, Leiserson & Rivest.
///
/// Basically a red/black balanced tree has the following properties:
/// -	every node is either red or black (col is RED or BLACK),
/// -	a leaf (RBNULL pointer) is considered black,
/// -	if a node is red then its children are black,
/// -	every path from a node to a leaf contains the same no of black nodes.
///
/// The last 2 points above guarantee that the longest path (alternating red
/// and black nodes) is only twice as long as the shortest path (all black
/// nodes). Thus the tree remains fairly balanced.
///
/// \ingroup BFC_TL

class BFC_Light_DLL RBTree : virtual public SObject {

public :

//	typedef int ( * CompFunc )(
//		const	void *		key1,
//		const	void *		key2
//	) const;

	/// \brief Creates a new RBTree.

	RBTree(
	);

	/// \brief Destroys this object.

	virtual ~RBTree(
	);

	virtual int rb_cmp(
		const	void *		key1,
		const	void *		key2
	) const = 0;

	const void * get(
		const	void *	key,
		const	Bool		ins = false
	);

	const void * del(
		const	void *	key
	);

#define RB_NONE -1	    /* None of those below */
#define RB_LUEQUAL 0	/* Only exact match */
#define RB_LUGTEQ 1		/* Exact match or greater */
#define RB_LULTEQ 2		/* Exact match or less */
#define RB_LULESS 3		/* Less than key (not equal to) */
#define RB_LUGREAT 4	/* Greater than key (not equal to) */
#define RB_LUNEXT 5		/* Next key after current */
#define RB_LUPREV 6		/* Prev key before current */
#define RB_LUFIRST 7	/* First key in index */
#define RB_LULAST 8		/* Last key in index */

	const void * lookup(
		const	int		mode,
		const	void *	key
	);

protected :

	/// \brief Searches for and if not found and \a ins is true, adds a
	///	new node in this tree.

	RBCell * rb_traverse(
		const	Bool		ins,
		const	void *	key
	);

	/// \brief Deletes \a node, and frees up the space.

	void rb_delete(
			RBCell *	node
	);

	/// \brief Restores the reb-black properties after a delete.

	void rb_delete_fix(
			RBCell *	node
	);

	// Rotate our tree thus:
	// 
	//             X        rb_left_rotate(X)--->            Y
	//           /   \                                     /   \.
	//          A     Y     <---rb_right_rotate(Y)        X     C
	//              /   \                               /   \.
	//             B     C                             A     B
	// 
	// N.B. This does not change the ordering.
	// 
	// We assume that neither X or Y is NULL

	void rb_left_rotate(
			RBCell *	node
	);

	void rb_right_rotate(
			RBCell *	node
	);

	/// \brief Searches for a key according to mode.

	RBCell * rb_lookup(
		const	int		mode,
		const	void *	key
	);

//	rblists *rb_openlist(const RBCell *);
//	const void * rb_readlist(rblists *);
//	void rb_closelist(rblists *);

private :

	RBCell *	rb_root;

	/// \brief Forbidden copy constructor.

	RBTree(
		const	RBTree&
	);

	/// \brief Forbidden copy operator.

	RBTree& operator = (
		const	RBTree&
	);

};

// ============================================================================

//struct rblists {
//	const RBCell *rootp; 
//	const RBCell *nextp; 
//}; 

//rblists *rbopenlist(const struct rbtree *); 
//const void *rbreadlist(rblists *); 
//void rbcloselist(rblists *); 

// ============================================================================

} // namespace BFC

// ============================================================================

#endif // _BFC_TL_RBTree_H_

// ============================================================================

