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
//	BFC.TL.RBTree.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include <stdlib.h>

#include "BFC.TL.RBTree.h"

// ============================================================================

using namespace BFC;

// ============================================================================

BFC_PTR_IMPL( RBTree )

// ============================================================================

// Dummy (sentinel) node, so that we can make X->left->up = X
// We then use this instead of NULL to mean the top or bottom
// end of the rb tree. It is a black node.

extern RBCell rb_null;

RBCell rb_null( &rb_null, &rb_null, &rb_null, false, 0 );

#define RBNULL (&rb_null)

// ============================================================================

RBCell::~RBCell() {

	if ( left != RBNULL ) {
		delete left;
	}

	if ( right != RBNULL ) {
		delete right;
	}

}

// ============================================================================

RBCell * RBCell::getSucc() {

	if ( right != RBNULL ) {
		RBCell * y = right;
		while ( y->left != RBNULL ) {
			y = y->left;
		}
		return y;
	}
	else {
		RBCell * x = this;
		RBCell * y = up;
		while ( y != RBNULL && x == y->right ) {
			x = y;
			y = y->up;
		}
		return y;
	}

}

RBCell * RBCell::getPred() {

	if ( left != RBNULL ) {
		RBCell * y = left;
		while ( y->right != RBNULL ) {
			y = y->right;
		}
		return y;
	}
	else {
		RBCell * x = this;
		RBCell * y = up;
		while ( y != RBNULL && x == y->left ) {
			x = y;
			y = y->up;
		}
		return y;
	}

}

// ============================================================================

RBTree::RBTree() :

	rb_root( RBNULL ) {

}

RBTree::~RBTree() {

	if ( rb_root != RBNULL ) {
		delete rb_root;
	}
	
}

// ============================================================================

const void * RBTree::get(
	const	void *	key,
	const	Bool		ins ) {

	RBCell * x = rb_traverse( ins, key );

	return ( x == RBNULL
		? 0
		: x->key );

}

const void * RBTree::del(
	const	void *	key ) {

	RBCell * x = rb_traverse( false, key );

	if ( x == RBNULL ) {
		return 0;
	}

	const void * y = x->key;

	rb_delete( x );

	return y;

}

const void * RBTree::lookup(
	const	int		mode,
	const	void *	key ) {

	RBCell * x = rb_lookup( mode, key );

	return ( x == RBNULL
		? 0
		: x->key );

}

// ============================================================================

RBCell * RBTree::rb_traverse(
	const	Bool		insert,
	const	void *		key ) {

	RBCell *x,*y,*z;
	int cmp;
	int found=0;

	y=RBNULL; /* points to the parent of x */
	x=rb_root;

	/* walk x down the tree */
	while(x!=RBNULL && found==0)
	{
		y=x;
		/* printf("key=%s, x->key=%s\n", key, x->key); */
		cmp=rb_cmp(key, x->key);

		if (cmp<0)
			x=x->left;
		else if (cmp>0)
			x=x->right;
		else
			found=1;
	}

	if (found || !insert)
		return(x);

	z = new RBCell( RBNULL, RBNULL, y, true, key );

	if ( y == RBNULL ) {
		rb_root=z;
	}
	else {
		cmp=rb_cmp(z->key, y->key);
		if (cmp<0)
			y->left=z;
		else
			y->right=z;
	}

	/* Having added a red node, we must now walk back up the tree balancing
	** it, by a series of rotations and changing of cols
	*/
	x = z;

	/* While we are not at the top and our parent node is red
	** N.B. Since the root node is garanteed black, then we
	** are also going to stop if we are the child of the root
	*/

	while(x != rb_root && x->up->red )
	{
		/* if our parent is on the left side of our grandparent */
		if (x->up == x->up->up->left)
		{
			/* get the right side of our grandparent (uncle?) */
			y=x->up->up->right;
			if ( y->red )
			{
				/* make our parent black */
				x->up->red = false;
				/* make our uncle black */
				y->red = false;
				/* make our grandparent red */
				x->up->up->red = true;

				/* now consider our grandparent */
				x=x->up->up;
			}
			else
			{
				/* if we are on the right side of our parent */
				if (x == x->up->right)
				{
					/* Move up to our parent */
					x=x->up;
					rb_left_rotate( x );
				}

				/* make our parent black */
				x->up->red = false;
				/* make our grandparent red */
				x->up->up->red = true;
				/* right rotate our grandparent */
				rb_right_rotate( x->up->up );
			}
		}
		else
		{
			/* everything here is the same as above, but
			** exchanging left for right
			*/

			y=x->up->up->left;
			if (y->red)
			{
				x->up->red = false;
				y->red = false;
				x->up->up->red = true;

				x=x->up->up;
			}
			else
			{
				if (x == x->up->left)
				{
					x=x->up;
					rb_right_rotate( x );
				}

				x->up->red = false;
				x->up->up->red = true;
				rb_left_rotate( x->up->up );
			}
		}
	}

	/* Set the root node black */
	rb_root->red = false;

	return z;

}

// ============================================================================

void RBTree::rb_delete(
		RBCell *	z ) {

	RBCell *x, *y;

	if (z->left == RBNULL || z->right == RBNULL)
		y=z;
	else
		y=z->getSucc();

	if (y->left != RBNULL)
		x=y->left;
	else
		x=y->right;

	x->up = y->up;

	if (y->up == RBNULL)
	{
		rb_root=x;
	}
	else
	{
		if (y==y->up->left)
			y->up->left = x;
		else
			y->up->right = x;
	}

	if (y!=z)
	{
		z->key = y->key;
	}

	if ( ! y->red )
		rb_delete_fix( x );

	if ( y != RBNULL ) {
		delete y;
	}

}

void RBTree::rb_delete_fix(
		RBCell *	x ) {

	RBCell *w;

	while (x!=rb_root && ! x->red )
	{
		if (x==x->up->left)
		{
			w=x->up->right;
			if (w->red)
			{
				w->red = false;
				x->up->red = true;
				rb_left_rotate( x->up );
				w=x->up->right;
			}

			if ( ! w->left->red && ! w->right->red )
			{
				w->red = true;
				x=x->up;
			}
			else
			{
				if ( ! w->right->red )
				{
					w->left->red = false;
					w->red = true;
					rb_right_rotate( w );
					w=x->up->right;
				}


				w->red = x->up->red;
				x->up->red = false;
				w->right->red = false;
				rb_left_rotate( x->up);
				x=rb_root;
			}
		}
		else
		{
			w=x->up->left;
			if (w->red)
			{
				w->red = false;
				x->up->red = true;
				rb_right_rotate( x->up);
				w=x->up->left;
			}

			if ( ! w->right->red && ! w->left->red )
			{
				w->red = true;
				x=x->up;
			}
			else
			{
				if ( ! w->left->red )
				{
					w->right->red = false;
					w->red = true;
					rb_left_rotate( w);
					w=x->up->left;
				}

				w->red = x->up->red;
				x->up->red = false;
				w->left->red = false;
				rb_right_rotate( x->up);
				x=rb_root;
			}
		}
	}

	x->red = false;

}

// ============================================================================

void RBTree::rb_left_rotate(
		RBCell *	x ) {

	RBCell *y;

	y=x->right; /* set Y */

	/* Turn Y's left subtree into X's right subtree (move B)*/
	x->right = y->left;

	/* If B is not null, set it's parent to be X */
	if (y->left != RBNULL)
		y->left->up = x;

	/* Set Y's parent to be what X's parent was */
	y->up = x->up;

	/* if X was the root */
	if (x->up == RBNULL)
	{
		rb_root=y;
	}
	else
	{
		/* Set X's parent's left or right pointer to be Y */
		if (x == x->up->left)
		{
			x->up->left=y;
		}
		else
		{
			x->up->right=y;
		}
	}

	/* Put X on Y's left */
	y->left=x;

	/* Set X's parent to be Y */
	x->up = y;

}

void RBTree::rb_right_rotate(
		RBCell *	y ) {

	RBCell *x;

	x=y->left; /* set X */

	/* Turn X's right subtree into Y's left subtree (move B) */
	y->left = x->right;

	/* If B is not null, set it's parent to be Y */
	if (x->right != RBNULL)
		x->right->up = y;

	/* Set X's parent to be what Y's parent was */
	x->up = y->up;

	/* if Y was the root */
	if (y->up == RBNULL)
	{
		rb_root=x;
	}
	else
	{
		/* Set Y's parent's left or right pointer to be X */
		if (y == y->up->left)
		{
			y->up->left=x;
		}
		else
		{
			y->up->right=x;
		}
	}

	/* Put Y on X's right */
	x->right=y;

	/* Set Y's parent to be X */
	y->up = x;

}

// ============================================================================

RBCell * RBTree::rb_lookup(
	const	int		mode,
	const	void *	key ) {

	RBCell *x,*y;
	int cmp = 0;
	int found=0;

	y=RBNULL; /* points to the parent of x */
	x=rb_root;

	if (mode==RB_LUFIRST)
	{
		/* Keep going left until we hit a NULL */
		while(x!=RBNULL)
		{
			y=x;
			x=x->left;
		}

		return(y);
	}
	else if (mode==RB_LULAST)
	{
		/* Keep going right until we hit a NULL */
		while(x!=RBNULL)
		{
			y=x;
			x=x->right;
		}

		return(y);
	}

	/* walk x down the tree */
	while(x!=RBNULL && found==0)
	{
		y=x;
		/* printf("key=%s, x->key=%s\n", key, x->key); */
		cmp=rb_cmp(key, x->key);


		if (cmp<0)
			x=x->left;
		else if (cmp>0)
			x=x->right;
		else
			found=1;
	}

	if (found && (mode==RB_LUEQUAL || mode==RB_LUGTEQ || mode==RB_LULTEQ))
		return(x);
	
	if (!found && (mode==RB_LUEQUAL || mode==RB_LUNEXT || mode==RB_LUPREV))
		return(RBNULL);
	
	if (mode==RB_LUGTEQ || (!found && mode==RB_LUGREAT))
	{
		if (cmp>0)
			return(y->getSucc());
		else
			return(y);
	}

	if (mode==RB_LULTEQ || (!found && mode==RB_LULESS))
	{
		if (cmp<0)
			return(y->getPred());
		else
			return(y);
	}

	if (mode==RB_LUNEXT || (found && mode==RB_LUGREAT))
		return(x->getSucc());

	if (mode==RB_LUPREV || (found && mode==RB_LULESS))
		return(x->getPred());
	
	/* Shouldn't get here */
	return(RBNULL);

}

// ============================================================================

//rblists * rbopenlist(const rbtree *tree) {
//	if (tree==NULL)
//		return(NULL);
//
//	return(rb_openlist(tree->rb_root));
//}
//
//const void * rbreadlist(rblists *rblistp) {
//	if (rblistp==NULL)
//		return(NULL);
//
//	return(rb_readlist(rblistp));
//}
//
//void rbcloselist(rblists *rblistp) {
//	if (rblistp==NULL)
//		return;
//
//	rb_closelist(rblistp);
//}

// ============================================================================

//rblists * rb_openlist(const RBCell *rootp) {
//	rblists *rblistp;
//
//	rblistp=(rblists *) malloc(sizeof(rblists));
//	if (!rblistp)
//		return(NULL);
//
//	rblistp->rootp=rootp;
//	rblistp->nextp=rootp;
//
//	if (rootp!=RBNULL)
//	{
//		while(rblistp->nextp->left!=RBNULL)
//		{
//			rblistp->nextp=rblistp->nextp->left;
//		}
//	}
//
//	return(rblistp);
//}
//
//const void * rb_readlist(rblists *rblistp) {
//	const void *key=NULL;
//
//	if (rblistp!=NULL && rblistp->nextp!=RBNULL)
//	{
//		key=rblistp->nextp->key;
//		rblistp->nextp=rb_successor(rblistp->nextp);
//	}
//
//	return(key);
//}
//
//void rb_closelist(rblists *rblistp) {
//	if (rblistp)
//		free(rblistp);
//}

// ============================================================================

