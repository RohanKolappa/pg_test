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
//	BFC.Crypto.Khazad.cpp
//
// Description:
//	...
//
// Author(s):
//	Jean-Francois GOBBERS
//
// ============================================================================

#include "BFC.Crypto.Khazad.h"
#include "BFC.Crypto.CipherRegisterer.h"

// ============================================================================

using namespace BFC;

// ============================================================================

BFC_PTR_IMPL_NS( Crypto, Khazad )

// ============================================================================

const DL::TypeCPtr & Crypto::Khazad::getClassType() {

	static DL::TypeCPtr i = new DL::Type(
		UUId( "19037821-22bb-4898-90e6-76fb1260de12" ),
		"BFC.Crypto.Khazad",
		Crypto::Cipher::getClassType(),
		new CipherDescriptor(
			DL::Descr(
				"khazad",
				"Cipher",
				"Khazad Cipher" ),
			16, 16, 8, 8 ) );

	return i;

}

// ============================================================================

Crypto::Khazad::Khazad() :

	Cipher( getClassType() ) {

}

// ============================================================================

Uint32 Crypto::Khazad::getKeySize(
	const	Uint32		pKeySize ) const {

	if ( pKeySize < 16 ) {
		throw InvalidArgument();
	}

	return 16;

}

// ============================================================================

void Crypto::Khazad::setKey(
	const	Buffer &	pKey ) {

	const Uchar * key = pKey.getCstPtr();
	Uint32 keylen = pKey.getLength();

	if ( keylen != 16 ) {
		throw InvalidArgument( "Invalid key!" );
	}

	const Uint64 *	S;
	Uint64		K2, K1;

	/* use 7th table */
	S = T7;

	 /*
	 * map unsigned char array cipher key to initial key state (mu):
	 */
	K2 =
		((Uint64)key[ 0] << 56) ^
		((Uint64)key[ 1] << 48) ^
		((Uint64)key[ 2] << 40) ^
		((Uint64)key[ 3] << 32) ^
		((Uint64)key[ 4] << 24) ^
		((Uint64)key[ 5] << 16) ^
		((Uint64)key[ 6] <<  8) ^
		((Uint64)key[ 7]		);
	K1 =
		((Uint64)key[ 8] << 56) ^
		((Uint64)key[ 9] << 48) ^
		((Uint64)key[10] << 40) ^
		((Uint64)key[11] << 32) ^
		((Uint64)key[12] << 24) ^
		((Uint64)key[13] << 16) ^
		((Uint64)key[14] <<  8) ^
		((Uint64)key[15]		);

	/*
	 * compute the round keys:
	 */
	for ( Uint32 r = 0 ; r <= R ; r++ ) {
		/*
		 * K[r] = rho(c[r], K1) ^ K2;
		 */
		roundKeyEnc[r] =
			T0[(int)(K1 >> 56)		 ] ^
			T1[(int)(K1 >> 48) & 0xff] ^
			T2[(int)(K1 >> 40) & 0xff] ^
			T3[(int)(K1 >> 32) & 0xff] ^
			T4[(int)(K1 >> 24) & 0xff] ^
			T5[(int)(K1 >> 16) & 0xff] ^
			T6[(int)(K1 >>  8) & 0xff] ^
			T7[(int)(K1		) & 0xff] ^
			c[r] ^ K2;
		K2 = K1; K1 = roundKeyEnc[r];
	}
	/*
	 * compute the inverse key schedule:
	 * K'^0 = K^R, K'^R = K^0, K'^r = theta(K^{R-r})
	 */
	roundKeyDec[0] = roundKeyEnc[R];
	for ( Uint32 r = 1 ; r < R ; r++ ) {
		K1 = roundKeyEnc[R - r];
		roundKeyDec[r] =
			T0[(int)S[(int)(K1 >> 56)		 ] & 0xff] ^
			T1[(int)S[(int)(K1 >> 48) & 0xff] & 0xff] ^
			T2[(int)S[(int)(K1 >> 40) & 0xff] & 0xff] ^
			T3[(int)S[(int)(K1 >> 32) & 0xff] & 0xff] ^
			T4[(int)S[(int)(K1 >> 24) & 0xff] & 0xff] ^
			T5[(int)S[(int)(K1 >> 16) & 0xff] & 0xff] ^
			T6[(int)S[(int)(K1 >>  8) & 0xff] & 0xff] ^
			T7[(int)S[(int)(K1		) & 0xff] & 0xff];
	}

	roundKeyDec[R] = roundKeyEnc[0];

}

// ============================================================================

void Crypto::Khazad::encrypt(
	const	Buffer &	pPlainText,
		Buffer &	pCipherText ) {

	if ( pPlainText.getLength() != 8 ) {
		throw InvalidArgument();
	}

	if ( pCipherText.getLength() != 8 ) {
		pCipherText.resize( 8 );
	}

	const Uchar *	pt = pPlainText.getCstPtr();
	Uchar *		ct = pCipherText.getVarPtr();

	process( pt, ct, roundKeyEnc );

}

void Crypto::Khazad::decrypt(
	const	Buffer &	pCipherText,
		Buffer &	pPlainText ) {

	if ( pCipherText.getLength() != 8 ) {
		throw InvalidArgument();
	}

	if ( pPlainText.getLength() != 8 ) {
		pPlainText.resize( 8 );
	}

	const Uchar *	ct = pCipherText.getCstPtr();
	Uchar *		pt = pPlainText.getVarPtr();

	process( ct, pt, roundKeyDec );

}

// ============================================================================

void Crypto::Khazad::test() {

	static const struct {
		Uchar	pt[ 8 ];
		Uchar	ct[ 8 ];
		Uchar	key[ 16 ];
	} tests[] = {
		{
			{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
			{ 0x49, 0xA4, 0xCE, 0x32, 0xAC, 0x19, 0x0E, 0x3F },
			{ 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
			  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }
		}, {
			{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
			{ 0x64, 0x5D, 0x77, 0x3E, 0x40, 0xAB, 0xDD, 0x53 },
			{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
			  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01 }
		}, {
			{ 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
			{ 0x9E, 0x39, 0x98, 0x64, 0xF7, 0x8E, 0xCA, 0x02 },
			{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
			  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }
		}, {
			{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01 },
			{ 0xA9, 0xDF, 0x3D, 0x2C, 0x64, 0xD3, 0xEA, 0x28 },
			{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
			  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }
		}
	};

	for ( Uint32 x = 0; x < (sizeof(tests)/sizeof(tests[0])); x++) {

		Buffer	key( tests[ x ].key, 16 );
		Buffer	pt( tests[ x ].pt, 8 );
		Buffer	ct( tests[ x ].ct, 8 );

		setKey( key );

		Buffer	nct = Cipher::encrypt( pt );
		Buffer	npt = Cipher::decrypt( nct );

		if ( nct != ct
		  || npt != pt ) {
			throw InternalError( "Khazad failed (pass 1)!" );
		}

		Buffer tmp = ct;

		for ( Uint32 y = 0 ; y < 1000 ; y++ ) {
			tmp = Cipher::encrypt( tmp );
		}
		for ( Uint32 y = 0 ; y < 1000 ; y++ ) {
			tmp = Cipher::decrypt( tmp );
		}

		if ( tmp != ct ) {
			throw InternalError( "Khazad failed (pass 2)!" );
		}

	}

}

// ============================================================================

void Crypto::Khazad::process(
	const	Uchar *		pt,
		Uchar *		ct,
	const	Uint64 *	roundKey ) {

	Uint64 state;
	 /*
	 * map pt block to cipher state (mu)
	 * and add initial round key (sigma[K^0]):
	 */
	state =
		((Uint64)pt[0] << 56) ^
		((Uint64)pt[1] << 48) ^
		((Uint64)pt[2] << 40) ^
		((Uint64)pt[3] << 32) ^
		((Uint64)pt[4] << 24) ^
		((Uint64)pt[5] << 16) ^
		((Uint64)pt[6] <<  8) ^
		((Uint64)pt[7]		) ^
		roundKey[0];

	 /*
	 * R - 1 full rounds:
	 */
	 for ( Uint32 r = 1; r < R; r++) {
		state =
			T0[(int)(state >> 56)		 ] ^
			T1[(int)(state >> 48) & 0xff] ^
			T2[(int)(state >> 40) & 0xff] ^
			T3[(int)(state >> 32) & 0xff] ^
			T4[(int)(state >> 24) & 0xff] ^
			T5[(int)(state >> 16) & 0xff] ^
			T6[(int)(state >>  8) & 0xff] ^
			T7[(int)(state		) & 0xff] ^
			roundKey[r];
	 }

	 /*
	 * last round:
	 */
	state =
		(T0[(int)(state >> 56)		 ] & CONST64(0xff00000000000000)) ^
		(T1[(int)(state >> 48) & 0xff] & CONST64(0x00ff000000000000)) ^
		(T2[(int)(state >> 40) & 0xff] & CONST64(0x0000ff0000000000)) ^
		(T3[(int)(state >> 32) & 0xff] & CONST64(0x000000ff00000000)) ^
		(T4[(int)(state >> 24) & 0xff] & CONST64(0x00000000ff000000)) ^
		(T5[(int)(state >> 16) & 0xff] & CONST64(0x0000000000ff0000)) ^
		(T6[(int)(state >>  8) & 0xff] & CONST64(0x000000000000ff00)) ^
		(T7[(int)(state		) & 0xff] & CONST64(0x00000000000000ff)) ^
		roundKey[R];

	/*
	 * map cipher state to ct block (mu^{-1}):
	 */
	ct[0] = (Uchar)(state >> 56);
	ct[1] = (Uchar)(state >> 48);
	ct[2] = (Uchar)(state >> 40);
	ct[3] = (Uchar)(state >> 32);
	ct[4] = (Uchar)(state >> 24);
	ct[5] = (Uchar)(state >> 16);
	ct[6] = (Uchar)(state >>  8);
	ct[7] = (Uchar)(state		);

}

// ============================================================================

const Uint64 Crypto::Khazad::T0[256] = {
	 CU64(0xbad3d268bbb96a01), CU64(0x54fc4d19e59a66b1), CU64(0x2f71bc93e26514cd), CU64(0x749ccdb925871b51),
	 CU64(0x53f55102f7a257a4), CU64(0xd3686bb8d0d6be03), CU64(0xd26b6fbdd6deb504), CU64(0x4dd72964b35285fe),
	 CU64(0x50f05d0dfdba4aad), CU64(0xace98a26cf09e063), CU64(0x8d8a0e83091c9684), CU64(0xbfdcc679a5914d1a),
	 CU64(0x7090ddad3da7374d), CU64(0x52f65507f1aa5ca3), CU64(0x9ab352c87ba417e1), CU64(0x4cd42d61b55a8ef9),
	 CU64(0xea238f65460320ac), CU64(0xd56273a6c4e68411), CU64(0x97a466f155cc68c2), CU64(0xd16e63b2dcc6a80d),
	 CU64(0x3355ccffaa85d099), CU64(0x51f35908fbb241aa), CU64(0x5bed712ac7e20f9c), CU64(0xa6f7a204f359ae55),
	 CU64(0xde7f5f81febec120), CU64(0x48d83d75ad7aa2e5), CU64(0xa8e59a32d729cc7f), CU64(0x99b65ec771bc0ae8),
	 CU64(0xdb704b90e096e63b), CU64(0x3256c8faac8ddb9e), CU64(0xb7c4e65195d11522), CU64(0xfc19d72b32b3aace),
	 CU64(0xe338ab48704b7393), CU64(0x9ebf42dc63843bfd), CU64(0x91ae7eef41fc52d0), CU64(0x9bb056cd7dac1ce6),
	 CU64(0xe23baf4d76437894), CU64(0xbbd0d66dbdb16106), CU64(0x41c319589b32f1da), CU64(0x6eb2a5cb7957e517),
	 CU64(0xa5f2ae0bf941b35c), CU64(0xcb400bc08016564b), CU64(0x6bbdb1da677fc20c), CU64(0x95a26efb59dc7ecc),
	 CU64(0xa1febe1fe1619f40), CU64(0xf308eb1810cbc3e3), CU64(0xb1cefe4f81e12f30), CU64(0x0206080a0c10160e),
	 CU64(0xcc4917db922e675e), CU64(0xc45137f3a26e3f66), CU64(0x1d2774694ee8cf53), CU64(0x143c504478a09c6c),
	 CU64(0xc3582be8b0560e73), CU64(0x63a591f2573f9a34), CU64(0xda734f95e69eed3c), CU64(0x5de76934d3d2358e),
	 CU64(0x5fe1613edfc22380), CU64(0xdc79578bf2aed72e), CU64(0x7d87e99413cf486e), CU64(0xcd4a13de94266c59),
	 CU64(0x7f81e19e1fdf5e60), CU64(0x5aee752fc1ea049b), CU64(0x6cb4adc17547f319), CU64(0x5ce46d31d5da3e89),
	 CU64(0xf704fb0c08ebefff), CU64(0x266a98bed42d47f2), CU64(0xff1cdb2438abb7c7), CU64(0xed2a937e543b11b9),
	 CU64(0xe825876f4a1336a2), CU64(0x9dba4ed3699c26f4), CU64(0x6fb1a1ce7f5fee10), CU64(0x8e8f028c03048b8d),
	 CU64(0x192b647d56c8e34f), CU64(0xa0fdba1ae7699447), CU64(0xf00de7171ad3deea), CU64(0x89861e97113cba98),
	 CU64(0x0f113c332278692d), CU64(0x07091c1b12383115), CU64(0xafec8629c511fd6a), CU64(0xfb10cb30208b9bdb),
	 CU64(0x0818202830405838), CU64(0x153f54417ea8976b), CU64(0x0d1734392e687f23), CU64(0x040c101418202c1c),
	 CU64(0x0103040506080b07), CU64(0x64ac8de94507ab21), CU64(0xdf7c5b84f8b6ca27), CU64(0x769ac5b329970d5f),
	 CU64(0x798bf9800bef6472), CU64(0xdd7a538ef4a6dc29), CU64(0x3d47f4c98ef5b2b3), CU64(0x163a584e74b08a62),
	 CU64(0x3f41fcc382e5a4bd), CU64(0x3759dcebb2a5fc85), CU64(0x6db7a9c4734ff81e), CU64(0x3848e0d890dd95a8),
	 CU64(0xb9d6de67b1a17708), CU64(0x7395d1a237bf2a44), CU64(0xe926836a4c1b3da5), CU64(0x355fd4e1beb5ea8b),
	 CU64(0x55ff491ce3926db6), CU64(0x7193d9a83baf3c4a), CU64(0x7b8df18a07ff727c), CU64(0x8c890a860f149d83),
	 CU64(0x7296d5a731b72143), CU64(0x88851a921734b19f), CU64(0xf607ff090ee3e4f8), CU64(0x2a7ea882fc4d33d6),
	 CU64(0x3e42f8c684edafba), CU64(0x5ee2653bd9ca2887), CU64(0x27699cbbd2254cf5), CU64(0x46ca0543890ac0cf),
	 CU64(0x0c14303c28607424), CU64(0x65af89ec430fa026), CU64(0x68b8bdd56d67df05), CU64(0x61a399f85b2f8c3a),
	 CU64(0x03050c0f0a181d09), CU64(0xc15e23e2bc46187d), CU64(0x57f94116ef827bb8), CU64(0xd6677fa9cefe9918),
	 CU64(0xd976439aec86f035), CU64(0x58e87d25cdfa1295), CU64(0xd875479fea8efb32), CU64(0x66aa85e34917bd2f),
	 CU64(0xd7647bacc8f6921f), CU64(0x3a4ee8d29ccd83a6), CU64(0xc84507cf8a0e4b42), CU64(0x3c44f0cc88fdb9b4),
	 CU64(0xfa13cf35268390dc), CU64(0x96a762f453c463c5), CU64(0xa7f4a601f551a552), CU64(0x98b55ac277b401ef),
	 CU64(0xec29977b52331abe), CU64(0xb8d5da62b7a97c0f), CU64(0xc7543bfca876226f), CU64(0xaeef822cc319f66d),
	 CU64(0x69bbb9d06b6fd402), CU64(0x4bdd317aa762bfec), CU64(0xabe0963ddd31d176), CU64(0xa9e69e37d121c778),
	 CU64(0x67a981e64f1fb628), CU64(0x0a1e28223c504e36), CU64(0x47c901468f02cbc8), CU64(0xf20bef1d16c3c8e4),
	 CU64(0xb5c2ee5b99c1032c), CU64(0x226688aacc0d6bee), CU64(0xe532b356647b4981), CU64(0xee2f9f715e230cb0),
	 CU64(0xbedfc27ca399461d), CU64(0x2b7dac87fa4538d1), CU64(0x819e3ebf217ce2a0), CU64(0x1236485a6c90a67e),
	 CU64(0x839836b52d6cf4ae), CU64(0x1b2d6c775ad8f541), CU64(0x0e1238362470622a), CU64(0x23658cafca0560e9),
	 CU64(0xf502f30604fbf9f1), CU64(0x45cf094c8312ddc6), CU64(0x216384a5c61576e7), CU64(0xce4f1fd19e3e7150),
	 CU64(0x49db3970ab72a9e2), CU64(0x2c74b09ce87d09c4), CU64(0xf916c33a2c9b8dd5), CU64(0xe637bf596e635488),
	 CU64(0xb6c7e25493d91e25), CU64(0x2878a088f05d25d8), CU64(0x17395c4b72b88165), CU64(0x829b32b02b64ffa9),
	 CU64(0x1a2e68725cd0fe46), CU64(0x8b80169d1d2cac96), CU64(0xfe1fdf213ea3bcc0), CU64(0x8a8312981b24a791),
	 CU64(0x091b242d3648533f), CU64(0xc94603ca8c064045), CU64(0x879426a1354cd8b2), CU64(0x4ed2256bb94a98f7),
	 CU64(0xe13ea3427c5b659d), CU64(0x2e72b896e46d1fca), CU64(0xe431b75362734286), CU64(0xe03da7477a536e9a),
	 CU64(0xeb208b60400b2bab), CU64(0x90ad7aea47f459d7), CU64(0xa4f1aa0eff49b85b), CU64(0x1e22786644f0d25a),
	 CU64(0x85922eab395ccebc), CU64(0x60a09dfd5d27873d), CU64(0x0000000000000000), CU64(0x256f94b1de355afb),
	 CU64(0xf401f70302f3f2f6), CU64(0xf10ee3121cdbd5ed), CU64(0x94a16afe5fd475cb), CU64(0x0b1d2c273a584531),
	 CU64(0xe734bb5c686b5f8f), CU64(0x759fc9bc238f1056), CU64(0xef2c9b74582b07b7), CU64(0x345cd0e4b8bde18c),
	 CU64(0x3153c4f5a695c697), CU64(0xd46177a3c2ee8f16), CU64(0xd06d67b7dacea30a), CU64(0x869722a43344d3b5),
	 CU64(0x7e82e59b19d75567), CU64(0xadea8e23c901eb64), CU64(0xfd1ad32e34bba1c9), CU64(0x297ba48df6552edf),
	 CU64(0x3050c0f0a09dcd90), CU64(0x3b4decd79ac588a1), CU64(0x9fbc46d9658c30fa), CU64(0xf815c73f2a9386d2),
	 CU64(0xc6573ff9ae7e2968), CU64(0x13354c5f6a98ad79), CU64(0x060a181e14303a12), CU64(0x050f14111e28271b),
	 CU64(0xc55233f6a4663461), CU64(0x113344556688bb77), CU64(0x7799c1b62f9f0658), CU64(0x7c84ed9115c74369),
	 CU64(0x7a8ef58f01f7797b), CU64(0x7888fd850de76f75), CU64(0x365ad8eeb4adf782), CU64(0x1c24706c48e0c454),
	 CU64(0x394be4dd96d59eaf), CU64(0x59eb7920cbf21992), CU64(0x1828607850c0e848), CU64(0x56fa4513e98a70bf),
	 CU64(0xb3c8f6458df1393e), CU64(0xb0cdfa4a87e92437), CU64(0x246c90b4d83d51fc), CU64(0x206080a0c01d7de0),
	 CU64(0xb2cbf2408bf93239), CU64(0x92ab72e04be44fd9), CU64(0xa3f8b615ed71894e), CU64(0xc05d27e7ba4e137a),
	 CU64(0x44cc0d49851ad6c1), CU64(0x62a695f751379133), CU64(0x103040506080b070), CU64(0xb4c1ea5e9fc9082b),
	 CU64(0x84912aae3f54c5bb), CU64(0x43c511529722e7d4), CU64(0x93a876e54dec44de), CU64(0xc25b2fedb65e0574),
	 CU64(0x4ade357fa16ab4eb), CU64(0xbddace73a9815b14), CU64(0x8f8c0689050c808a), CU64(0x2d77b499ee7502c3),
	 CU64(0xbcd9ca76af895013), CU64(0x9cb94ad66f942df3), CU64(0x6abeb5df6177c90b), CU64(0x40c01d5d9d3afadd),
	 CU64(0xcf4c1bd498367a57), CU64(0xa2fbb210eb798249), CU64(0x809d3aba2774e9a7), CU64(0x4fd1216ebf4293f0),
	 CU64(0x1f217c6342f8d95d), CU64(0xca430fc5861e5d4c), CU64(0xaae39238db39da71), CU64(0x42c61557912aecd3),
};

const Uint64 Crypto::Khazad::T1[256] = {
	 CU64(0xd3ba68d2b9bb016a), CU64(0xfc54194d9ae5b166), CU64(0x712f93bc65e2cd14), CU64(0x9c74b9cd8725511b),
	 CU64(0xf5530251a2f7a457), CU64(0x68d3b86bd6d003be), CU64(0x6bd2bd6fded604b5), CU64(0xd74d642952b3fe85),
	 CU64(0xf0500d5dbafdad4a), CU64(0xe9ac268a09cf63e0), CU64(0x8a8d830e1c098496), CU64(0xdcbf79c691a51a4d),
	 CU64(0x9070addda73d4d37), CU64(0xf6520755aaf1a35c), CU64(0xb39ac852a47be117), CU64(0xd44c612d5ab5f98e),
	 CU64(0x23ea658f0346ac20), CU64(0x62d5a673e6c41184), CU64(0xa497f166cc55c268), CU64(0x6ed1b263c6dc0da8),
	 CU64(0x5533ffcc85aa99d0), CU64(0xf3510859b2fbaa41), CU64(0xed5b2a71e2c79c0f), CU64(0xf7a604a259f355ae),
	 CU64(0x7fde815fbefe20c1), CU64(0xd848753d7aade5a2), CU64(0xe5a8329a29d77fcc), CU64(0xb699c75ebc71e80a),
	 CU64(0x70db904b96e03be6), CU64(0x5632fac88dac9edb), CU64(0xc4b751e6d1952215), CU64(0x19fc2bd7b332ceaa),
	 CU64(0x38e348ab4b709373), CU64(0xbf9edc428463fd3b), CU64(0xae91ef7efc41d052), CU64(0xb09bcd56ac7de61c),
	 CU64(0x3be24daf43769478), CU64(0xd0bb6dd6b1bd0661), CU64(0xc3415819329bdaf1), CU64(0xb26ecba5577917e5),
	 CU64(0xf2a50bae41f95cb3), CU64(0x40cbc00b16804b56), CU64(0xbd6bdab17f670cc2), CU64(0xa295fb6edc59cc7e),
	 CU64(0xfea11fbe61e1409f), CU64(0x08f318ebcb10e3c3), CU64(0xceb14ffee181302f), CU64(0x06020a08100c0e16),
	 CU64(0x49ccdb172e925e67), CU64(0x51c4f3376ea2663f), CU64(0x271d6974e84e53cf), CU64(0x3c144450a0786c9c),
	 CU64(0x58c3e82b56b0730e), CU64(0xa563f2913f57349a), CU64(0x73da954f9ee63ced), CU64(0xe75d3469d2d38e35),
	 CU64(0xe15f3e61c2df8023), CU64(0x79dc8b57aef22ed7), CU64(0x877d94e9cf136e48), CU64(0x4acdde132694596c),
	 CU64(0x817f9ee1df1f605e), CU64(0xee5a2f75eac19b04), CU64(0xb46cc1ad477519f3), CU64(0xe45c316ddad5893e),
	 CU64(0x04f70cfbeb08ffef), CU64(0x6a26be982dd4f247), CU64(0x1cff24dbab38c7b7), CU64(0x2aed7e933b54b911),
	 CU64(0x25e86f87134aa236), CU64(0xba9dd34e9c69f426), CU64(0xb16fcea15f7f10ee), CU64(0x8f8e8c0204038d8b),
	 CU64(0x2b197d64c8564fe3), CU64(0xfda01aba69e74794), CU64(0x0df017e7d31aeade), CU64(0x8689971e3c1198ba),
	 CU64(0x110f333c78222d69), CU64(0x09071b1c38121531), CU64(0xecaf298611c56afd), CU64(0x10fb30cb8b20db9b),
	 CU64(0x1808282040303858), CU64(0x3f154154a87e6b97), CU64(0x170d3934682e237f), CU64(0x0c04141020181c2c),
	 CU64(0x030105040806070b), CU64(0xac64e98d074521ab), CU64(0x7cdf845bb6f827ca), CU64(0x9a76b3c597295f0d),
	 CU64(0x8b7980f9ef0b7264), CU64(0x7add8e53a6f429dc), CU64(0x473dc9f4f58eb3b2), CU64(0x3a164e58b074628a),
	 CU64(0x413fc3fce582bda4), CU64(0x5937ebdca5b285fc), CU64(0xb76dc4a94f731ef8), CU64(0x4838d8e0dd90a895),
	 CU64(0xd6b967dea1b10877), CU64(0x9573a2d1bf37442a), CU64(0x26e96a831b4ca53d), CU64(0x5f35e1d4b5be8bea),
	 CU64(0xff551c4992e3b66d), CU64(0x9371a8d9af3b4a3c), CU64(0x8d7b8af1ff077c72), CU64(0x898c860a140f839d),
	 CU64(0x9672a7d5b7314321), CU64(0x8588921a34179fb1), CU64(0x07f609ffe30ef8e4), CU64(0x7e2a82a84dfcd633),
	 CU64(0x423ec6f8ed84baaf), CU64(0xe25e3b65cad98728), CU64(0x6927bb9c25d2f54c), CU64(0xca4643050a89cfc0),
	 CU64(0x140c3c3060282474), CU64(0xaf65ec890f4326a0), CU64(0xb868d5bd676d05df), CU64(0xa361f8992f5b3a8c),
	 CU64(0x05030f0c180a091d), CU64(0x5ec1e22346bc7d18), CU64(0xf957164182efb87b), CU64(0x67d6a97ffece1899),
	 CU64(0x76d99a4386ec35f0), CU64(0xe858257dfacd9512), CU64(0x75d89f478eea32fb), CU64(0xaa66e38517492fbd),
	 CU64(0x64d7ac7bf6c81f92), CU64(0x4e3ad2e8cd9ca683), CU64(0x45c8cf070e8a424b), CU64(0x443cccf0fd88b4b9),
	 CU64(0x13fa35cf8326dc90), CU64(0xa796f462c453c563), CU64(0xf4a701a651f552a5), CU64(0xb598c25ab477ef01),
	 CU64(0x29ec7b973352be1a), CU64(0xd5b862daa9b70f7c), CU64(0x54c7fc3b76a86f22), CU64(0xefae2c8219c36df6),
	 CU64(0xbb69d0b96f6b02d4), CU64(0xdd4b7a3162a7ecbf), CU64(0xe0ab3d9631dd76d1), CU64(0xe6a9379e21d178c7),
	 CU64(0xa967e6811f4f28b6), CU64(0x1e0a2228503c364e), CU64(0xc9474601028fc8cb), CU64(0x0bf21defc316e4c8),
	 CU64(0xc2b55beec1992c03), CU64(0x6622aa880dccee6b), CU64(0x32e556b37b648149), CU64(0x2fee719f235eb00c),
	 CU64(0xdfbe7cc299a31d46), CU64(0x7d2b87ac45fad138), CU64(0x9e81bf3e7c21a0e2), CU64(0x36125a48906c7ea6),
	 CU64(0x9883b5366c2daef4), CU64(0x2d1b776cd85a41f5), CU64(0x120e363870242a62), CU64(0x6523af8c05cae960),
	 CU64(0x02f506f3fb04f1f9), CU64(0xcf454c091283c6dd), CU64(0x6321a58415c6e776), CU64(0x4fced11f3e9e5071),
	 CU64(0xdb49703972abe2a9), CU64(0x742c9cb07de8c409), CU64(0x16f93ac39b2cd58d), CU64(0x37e659bf636e8854),
	 CU64(0xc7b654e2d993251e), CU64(0x782888a05df0d825), CU64(0x39174b5cb8726581), CU64(0x9b82b032642ba9ff),
	 CU64(0x2e1a7268d05c46fe), CU64(0x808b9d162c1d96ac), CU64(0x1ffe21dfa33ec0bc), CU64(0x838a9812241b91a7),
	 CU64(0x1b092d2448363f53), CU64(0x46c9ca03068c4540), CU64(0x9487a1264c35b2d8), CU64(0xd24e6b254ab9f798),
	 CU64(0x3ee142a35b7c9d65), CU64(0x722e96b86de4ca1f), CU64(0x31e453b773628642), CU64(0x3de047a7537a9a6e),
	 CU64(0x20eb608b0b40ab2b), CU64(0xad90ea7af447d759), CU64(0xf1a40eaa49ff5bb8), CU64(0x221e6678f0445ad2),
	 CU64(0x9285ab2e5c39bcce), CU64(0xa060fd9d275d3d87), CU64(0x0000000000000000), CU64(0x6f25b19435defb5a),
	 CU64(0x01f403f7f302f6f2), CU64(0x0ef112e3db1cedd5), CU64(0xa194fe6ad45fcb75), CU64(0x1d0b272c583a3145),
	 CU64(0x34e75cbb6b688f5f), CU64(0x9f75bcc98f235610), CU64(0x2cef749b2b58b707), CU64(0x5c34e4d0bdb88ce1),
	 CU64(0x5331f5c495a697c6), CU64(0x61d4a377eec2168f), CU64(0x6dd0b767ceda0aa3), CU64(0x9786a4224433b5d3),
	 CU64(0x827e9be5d7196755), CU64(0xeaad238e01c964eb), CU64(0x1afd2ed3bb34c9a1), CU64(0x7b298da455f6df2e),
	 CU64(0x5030f0c09da090cd), CU64(0x4d3bd7ecc59aa188), CU64(0xbc9fd9468c65fa30), CU64(0x15f83fc7932ad286),
	 CU64(0x57c6f93f7eae6829), CU64(0x35135f4c986a79ad), CU64(0x0a061e183014123a), CU64(0x0f051114281e1b27),
	 CU64(0x52c5f63366a46134), CU64(0x33115544886677bb), CU64(0x9977b6c19f2f5806), CU64(0x847c91edc7156943),
	 CU64(0x8e7a8ff5f7017b79), CU64(0x887885fde70d756f), CU64(0x5a36eed8adb482f7), CU64(0x241c6c70e04854c4),
	 CU64(0x4b39dde4d596af9e), CU64(0xeb592079f2cb9219), CU64(0x28187860c05048e8), CU64(0xfa5613458ae9bf70),
	 CU64(0xc8b345f6f18d3e39), CU64(0xcdb04afae9873724), CU64(0x6c24b4903dd8fc51), CU64(0x6020a0801dc0e07d),
	 CU64(0xcbb240f2f98b3932), CU64(0xab92e072e44bd94f), CU64(0xf8a315b671ed4e89), CU64(0x5dc0e7274eba7a13),
	 CU64(0xcc44490d1a85c1d6), CU64(0xa662f79537513391), CU64(0x30105040806070b0), CU64(0xc1b45eeac99f2b08),
	 CU64(0x9184ae2a543fbbc5), CU64(0xc54352112297d4e7), CU64(0xa893e576ec4dde44), CU64(0x5bc2ed2f5eb67405),
	 CU64(0xde4a7f356aa1ebb4), CU64(0xdabd73ce81a9145b), CU64(0x8c8f89060c058a80), CU64(0x772d99b475eec302),
	 CU64(0xd9bc76ca89af1350), CU64(0xb99cd64a946ff32d), CU64(0xbe6adfb577610bc9), CU64(0xc0405d1d3a9dddfa),
	 CU64(0x4ccfd41b3698577a), CU64(0xfba210b279eb4982), CU64(0x9d80ba3a7427a7e9), CU64(0xd14f6e2142bff093),
	 CU64(0x211f637cf8425dd9), CU64(0x43cac50f1e864c5d), CU64(0xe3aa389239db71da), CU64(0xc64257152a91d3ec),
};

const Uint64 Crypto::Khazad::T2[256] = {
	 CU64(0xd268bad36a01bbb9), CU64(0x4d1954fc66b1e59a), CU64(0xbc932f7114cde265), CU64(0xcdb9749c1b512587),
	 CU64(0x510253f557a4f7a2), CU64(0x6bb8d368be03d0d6), CU64(0x6fbdd26bb504d6de), CU64(0x29644dd785feb352),
	 CU64(0x5d0d50f04aadfdba), CU64(0x8a26ace9e063cf09), CU64(0x0e838d8a9684091c), CU64(0xc679bfdc4d1aa591),
	 CU64(0xddad7090374d3da7), CU64(0x550752f65ca3f1aa), CU64(0x52c89ab317e17ba4), CU64(0x2d614cd48ef9b55a),
	 CU64(0x8f65ea2320ac4603), CU64(0x73a6d5628411c4e6), CU64(0x66f197a468c255cc), CU64(0x63b2d16ea80ddcc6),
	 CU64(0xccff3355d099aa85), CU64(0x590851f341aafbb2), CU64(0x712a5bed0f9cc7e2), CU64(0xa204a6f7ae55f359),
	 CU64(0x5f81de7fc120febe), CU64(0x3d7548d8a2e5ad7a), CU64(0x9a32a8e5cc7fd729), CU64(0x5ec799b60ae871bc),
	 CU64(0x4b90db70e63be096), CU64(0xc8fa3256db9eac8d), CU64(0xe651b7c4152295d1), CU64(0xd72bfc19aace32b3),
	 CU64(0xab48e3387393704b), CU64(0x42dc9ebf3bfd6384), CU64(0x7eef91ae52d041fc), CU64(0x56cd9bb01ce67dac),
	 CU64(0xaf4de23b78947643), CU64(0xd66dbbd06106bdb1), CU64(0x195841c3f1da9b32), CU64(0xa5cb6eb2e5177957),
	 CU64(0xae0ba5f2b35cf941), CU64(0x0bc0cb40564b8016), CU64(0xb1da6bbdc20c677f), CU64(0x6efb95a27ecc59dc),
	 CU64(0xbe1fa1fe9f40e161), CU64(0xeb18f308c3e310cb), CU64(0xfe4fb1ce2f3081e1), CU64(0x080a0206160e0c10),
	 CU64(0x17dbcc49675e922e), CU64(0x37f3c4513f66a26e), CU64(0x74691d27cf534ee8), CU64(0x5044143c9c6c78a0),
	 CU64(0x2be8c3580e73b056), CU64(0x91f263a59a34573f), CU64(0x4f95da73ed3ce69e), CU64(0x69345de7358ed3d2),
	 CU64(0x613e5fe12380dfc2), CU64(0x578bdc79d72ef2ae), CU64(0xe9947d87486e13cf), CU64(0x13decd4a6c599426),
	 CU64(0xe19e7f815e601fdf), CU64(0x752f5aee049bc1ea), CU64(0xadc16cb4f3197547), CU64(0x6d315ce43e89d5da),
	 CU64(0xfb0cf704efff08eb), CU64(0x98be266a47f2d42d), CU64(0xdb24ff1cb7c738ab), CU64(0x937eed2a11b9543b),
	 CU64(0x876fe82536a24a13), CU64(0x4ed39dba26f4699c), CU64(0xa1ce6fb1ee107f5f), CU64(0x028c8e8f8b8d0304),
	 CU64(0x647d192be34f56c8), CU64(0xba1aa0fd9447e769), CU64(0xe717f00ddeea1ad3), CU64(0x1e978986ba98113c),
	 CU64(0x3c330f11692d2278), CU64(0x1c1b070931151238), CU64(0x8629afecfd6ac511), CU64(0xcb30fb109bdb208b),
	 CU64(0x2028081858383040), CU64(0x5441153f976b7ea8), CU64(0x34390d177f232e68), CU64(0x1014040c2c1c1820),
	 CU64(0x040501030b070608), CU64(0x8de964acab214507), CU64(0x5b84df7cca27f8b6), CU64(0xc5b3769a0d5f2997),
	 CU64(0xf980798b64720bef), CU64(0x538edd7adc29f4a6), CU64(0xf4c93d47b2b38ef5), CU64(0x584e163a8a6274b0),
	 CU64(0xfcc33f41a4bd82e5), CU64(0xdceb3759fc85b2a5), CU64(0xa9c46db7f81e734f), CU64(0xe0d8384895a890dd),
	 CU64(0xde67b9d67708b1a1), CU64(0xd1a273952a4437bf), CU64(0x836ae9263da54c1b), CU64(0xd4e1355fea8bbeb5),
	 CU64(0x491c55ff6db6e392), CU64(0xd9a871933c4a3baf), CU64(0xf18a7b8d727c07ff), CU64(0x0a868c899d830f14),
	 CU64(0xd5a77296214331b7), CU64(0x1a928885b19f1734), CU64(0xff09f607e4f80ee3), CU64(0xa8822a7e33d6fc4d),
	 CU64(0xf8c63e42afba84ed), CU64(0x653b5ee22887d9ca), CU64(0x9cbb27694cf5d225), CU64(0x054346cac0cf890a),
	 CU64(0x303c0c1474242860), CU64(0x89ec65afa026430f), CU64(0xbdd568b8df056d67), CU64(0x99f861a38c3a5b2f),
	 CU64(0x0c0f03051d090a18), CU64(0x23e2c15e187dbc46), CU64(0x411657f97bb8ef82), CU64(0x7fa9d6679918cefe),
	 CU64(0x439ad976f035ec86), CU64(0x7d2558e81295cdfa), CU64(0x479fd875fb32ea8e), CU64(0x85e366aabd2f4917),
	 CU64(0x7bacd764921fc8f6), CU64(0xe8d23a4e83a69ccd), CU64(0x07cfc8454b428a0e), CU64(0xf0cc3c44b9b488fd),
	 CU64(0xcf35fa1390dc2683), CU64(0x62f496a763c553c4), CU64(0xa601a7f4a552f551), CU64(0x5ac298b501ef77b4),
	 CU64(0x977bec291abe5233), CU64(0xda62b8d57c0fb7a9), CU64(0x3bfcc754226fa876), CU64(0x822caeeff66dc319),
	 CU64(0xb9d069bbd4026b6f), CU64(0x317a4bddbfeca762), CU64(0x963dabe0d176dd31), CU64(0x9e37a9e6c778d121),
	 CU64(0x81e667a9b6284f1f), CU64(0x28220a1e4e363c50), CU64(0x014647c9cbc88f02), CU64(0xef1df20bc8e416c3),
	 CU64(0xee5bb5c2032c99c1), CU64(0x88aa22666beecc0d), CU64(0xb356e5324981647b), CU64(0x9f71ee2f0cb05e23),
	 CU64(0xc27cbedf461da399), CU64(0xac872b7d38d1fa45), CU64(0x3ebf819ee2a0217c), CU64(0x485a1236a67e6c90),
	 CU64(0x36b58398f4ae2d6c), CU64(0x6c771b2df5415ad8), CU64(0x38360e12622a2470), CU64(0x8caf236560e9ca05),
	 CU64(0xf306f502f9f104fb), CU64(0x094c45cfddc68312), CU64(0x84a5216376e7c615), CU64(0x1fd1ce4f71509e3e),
	 CU64(0x397049dba9e2ab72), CU64(0xb09c2c7409c4e87d), CU64(0xc33af9168dd52c9b), CU64(0xbf59e63754886e63),
	 CU64(0xe254b6c71e2593d9), CU64(0xa088287825d8f05d), CU64(0x5c4b1739816572b8), CU64(0x32b0829bffa92b64),
	 CU64(0x68721a2efe465cd0), CU64(0x169d8b80ac961d2c), CU64(0xdf21fe1fbcc03ea3), CU64(0x12988a83a7911b24),
	 CU64(0x242d091b533f3648), CU64(0x03cac94640458c06), CU64(0x26a18794d8b2354c), CU64(0x256b4ed298f7b94a),
	 CU64(0xa342e13e659d7c5b), CU64(0xb8962e721fcae46d), CU64(0xb753e43142866273), CU64(0xa747e03d6e9a7a53),
	 CU64(0x8b60eb202bab400b), CU64(0x7aea90ad59d747f4), CU64(0xaa0ea4f1b85bff49), CU64(0x78661e22d25a44f0),
	 CU64(0x2eab8592cebc395c), CU64(0x9dfd60a0873d5d27), CU64(0x0000000000000000), CU64(0x94b1256f5afbde35),
	 CU64(0xf703f401f2f602f3), CU64(0xe312f10ed5ed1cdb), CU64(0x6afe94a175cb5fd4), CU64(0x2c270b1d45313a58),
	 CU64(0xbb5ce7345f8f686b), CU64(0xc9bc759f1056238f), CU64(0x9b74ef2c07b7582b), CU64(0xd0e4345ce18cb8bd),
	 CU64(0xc4f53153c697a695), CU64(0x77a3d4618f16c2ee), CU64(0x67b7d06da30adace), CU64(0x22a48697d3b53344),
	 CU64(0xe59b7e82556719d7), CU64(0x8e23adeaeb64c901), CU64(0xd32efd1aa1c934bb), CU64(0xa48d297b2edff655),
	 CU64(0xc0f03050cd90a09d), CU64(0xecd73b4d88a19ac5), CU64(0x46d99fbc30fa658c), CU64(0xc73ff81586d22a93),
	 CU64(0x3ff9c6572968ae7e), CU64(0x4c5f1335ad796a98), CU64(0x181e060a3a121430), CU64(0x1411050f271b1e28),
	 CU64(0x33f6c5523461a466), CU64(0x44551133bb776688), CU64(0xc1b6779906582f9f), CU64(0xed917c84436915c7),
	 CU64(0xf58f7a8e797b01f7), CU64(0xfd8578886f750de7), CU64(0xd8ee365af782b4ad), CU64(0x706c1c24c45448e0),
	 CU64(0xe4dd394b9eaf96d5), CU64(0x792059eb1992cbf2), CU64(0x60781828e84850c0), CU64(0x451356fa70bfe98a),
	 CU64(0xf645b3c8393e8df1), CU64(0xfa4ab0cd243787e9), CU64(0x90b4246c51fcd83d), CU64(0x80a020607de0c01d),
	 CU64(0xf240b2cb32398bf9), CU64(0x72e092ab4fd94be4), CU64(0xb615a3f8894eed71), CU64(0x27e7c05d137aba4e),
	 CU64(0x0d4944ccd6c1851a), CU64(0x95f762a691335137), CU64(0x40501030b0706080), CU64(0xea5eb4c1082b9fc9),
	 CU64(0x2aae8491c5bb3f54), CU64(0x115243c5e7d49722), CU64(0x76e593a844de4dec), CU64(0x2fedc25b0574b65e),
	 CU64(0x357f4adeb4eba16a), CU64(0xce73bdda5b14a981), CU64(0x06898f8c808a050c), CU64(0xb4992d7702c3ee75),
	 CU64(0xca76bcd95013af89), CU64(0x4ad69cb92df36f94), CU64(0xb5df6abec90b6177), CU64(0x1d5d40c0fadd9d3a),
	 CU64(0x1bd4cf4c7a579836), CU64(0xb210a2fb8249eb79), CU64(0x3aba809de9a72774), CU64(0x216e4fd193f0bf42),
	 CU64(0x7c631f21d95d42f8), CU64(0x0fc5ca435d4c861e), CU64(0x9238aae3da71db39), CU64(0x155742c6ecd3912a),
};

const Uint64 Crypto::Khazad::T3[256] = {
	 CU64(0x68d2d3ba016ab9bb), CU64(0x194dfc54b1669ae5), CU64(0x93bc712fcd1465e2), CU64(0xb9cd9c74511b8725),
	 CU64(0x0251f553a457a2f7), CU64(0xb86b68d303bed6d0), CU64(0xbd6f6bd204b5ded6), CU64(0x6429d74dfe8552b3),
	 CU64(0x0d5df050ad4abafd), CU64(0x268ae9ac63e009cf), CU64(0x830e8a8d84961c09), CU64(0x79c6dcbf1a4d91a5),
	 CU64(0xaddd90704d37a73d), CU64(0x0755f652a35caaf1), CU64(0xc852b39ae117a47b), CU64(0x612dd44cf98e5ab5),
	 CU64(0x658f23eaac200346), CU64(0xa67362d51184e6c4), CU64(0xf166a497c268cc55), CU64(0xb2636ed10da8c6dc),
	 CU64(0xffcc553399d085aa), CU64(0x0859f351aa41b2fb), CU64(0x2a71ed5b9c0fe2c7), CU64(0x04a2f7a655ae59f3),
	 CU64(0x815f7fde20c1befe), CU64(0x753dd848e5a27aad), CU64(0x329ae5a87fcc29d7), CU64(0xc75eb699e80abc71),
	 CU64(0x904b70db3be696e0), CU64(0xfac856329edb8dac), CU64(0x51e6c4b72215d195), CU64(0x2bd719fcceaab332),
	 CU64(0x48ab38e393734b70), CU64(0xdc42bf9efd3b8463), CU64(0xef7eae91d052fc41), CU64(0xcd56b09be61cac7d),
	 CU64(0x4daf3be294784376), CU64(0x6dd6d0bb0661b1bd), CU64(0x5819c341daf1329b), CU64(0xcba5b26e17e55779),
	 CU64(0x0baef2a55cb341f9), CU64(0xc00b40cb4b561680), CU64(0xdab1bd6b0cc27f67), CU64(0xfb6ea295cc7edc59),
	 CU64(0x1fbefea1409f61e1), CU64(0x18eb08f3e3c3cb10), CU64(0x4ffeceb1302fe181), CU64(0x0a0806020e16100c),
	 CU64(0xdb1749cc5e672e92), CU64(0xf33751c4663f6ea2), CU64(0x6974271d53cfe84e), CU64(0x44503c146c9ca078),
	 CU64(0xe82b58c3730e56b0), CU64(0xf291a563349a3f57), CU64(0x954f73da3ced9ee6), CU64(0x3469e75d8e35d2d3),
	 CU64(0x3e61e15f8023c2df), CU64(0x8b5779dc2ed7aef2), CU64(0x94e9877d6e48cf13), CU64(0xde134acd596c2694),
	 CU64(0x9ee1817f605edf1f), CU64(0x2f75ee5a9b04eac1), CU64(0xc1adb46c19f34775), CU64(0x316de45c893edad5),
	 CU64(0x0cfb04f7ffefeb08), CU64(0xbe986a26f2472dd4), CU64(0x24db1cffc7b7ab38), CU64(0x7e932aedb9113b54),
	 CU64(0x6f8725e8a236134a), CU64(0xd34eba9df4269c69), CU64(0xcea1b16f10ee5f7f), CU64(0x8c028f8e8d8b0403),
	 CU64(0x7d642b194fe3c856), CU64(0x1abafda0479469e7), CU64(0x17e70df0eaded31a), CU64(0x971e868998ba3c11),
	 CU64(0x333c110f2d697822), CU64(0x1b1c090715313812), CU64(0x2986ecaf6afd11c5), CU64(0x30cb10fbdb9b8b20),
	 CU64(0x2820180838584030), CU64(0x41543f156b97a87e), CU64(0x3934170d237f682e), CU64(0x14100c041c2c2018),
	 CU64(0x05040301070b0806), CU64(0xe98dac6421ab0745), CU64(0x845b7cdf27cab6f8), CU64(0xb3c59a765f0d9729),
	 CU64(0x80f98b797264ef0b), CU64(0x8e537add29dca6f4), CU64(0xc9f4473db3b2f58e), CU64(0x4e583a16628ab074),
	 CU64(0xc3fc413fbda4e582), CU64(0xebdc593785fca5b2), CU64(0xc4a9b76d1ef84f73), CU64(0xd8e04838a895dd90),
	 CU64(0x67ded6b90877a1b1), CU64(0xa2d19573442abf37), CU64(0x6a8326e9a53d1b4c), CU64(0xe1d45f358beab5be),
	 CU64(0x1c49ff55b66d92e3), CU64(0xa8d993714a3caf3b), CU64(0x8af18d7b7c72ff07), CU64(0x860a898c839d140f),
	 CU64(0xa7d596724321b731), CU64(0x921a85889fb13417), CU64(0x09ff07f6f8e4e30e), CU64(0x82a87e2ad6334dfc),
	 CU64(0xc6f8423ebaafed84), CU64(0x3b65e25e8728cad9), CU64(0xbb9c6927f54c25d2), CU64(0x4305ca46cfc00a89),
	 CU64(0x3c30140c24746028), CU64(0xec89af6526a00f43), CU64(0xd5bdb86805df676d), CU64(0xf899a3613a8c2f5b),
	 CU64(0x0f0c0503091d180a), CU64(0xe2235ec17d1846bc), CU64(0x1641f957b87b82ef), CU64(0xa97f67d61899fece),
	 CU64(0x9a4376d935f086ec), CU64(0x257de8589512facd), CU64(0x9f4775d832fb8eea), CU64(0xe385aa662fbd1749),
	 CU64(0xac7b64d71f92f6c8), CU64(0xd2e84e3aa683cd9c), CU64(0xcf0745c8424b0e8a), CU64(0xccf0443cb4b9fd88),
	 CU64(0x35cf13fadc908326), CU64(0xf462a796c563c453), CU64(0x01a6f4a752a551f5), CU64(0xc25ab598ef01b477),
	 CU64(0x7b9729ecbe1a3352), CU64(0x62dad5b80f7ca9b7), CU64(0xfc3b54c76f2276a8), CU64(0x2c82efae6df619c3),
	 CU64(0xd0b9bb6902d46f6b), CU64(0x7a31dd4becbf62a7), CU64(0x3d96e0ab76d131dd), CU64(0x379ee6a978c721d1),
	 CU64(0xe681a96728b61f4f), CU64(0x22281e0a364e503c), CU64(0x4601c947c8cb028f), CU64(0x1def0bf2e4c8c316),
	 CU64(0x5beec2b52c03c199), CU64(0xaa886622ee6b0dcc), CU64(0x56b332e581497b64), CU64(0x719f2feeb00c235e),
	 CU64(0x7cc2dfbe1d4699a3), CU64(0x87ac7d2bd13845fa), CU64(0xbf3e9e81a0e27c21), CU64(0x5a4836127ea6906c),
	 CU64(0xb5369883aef46c2d), CU64(0x776c2d1b41f5d85a), CU64(0x3638120e2a627024), CU64(0xaf8c6523e96005ca),
	 CU64(0x06f302f5f1f9fb04), CU64(0x4c09cf45c6dd1283), CU64(0xa5846321e77615c6), CU64(0xd11f4fce50713e9e),
	 CU64(0x7039db49e2a972ab), CU64(0x9cb0742cc4097de8), CU64(0x3ac316f9d58d9b2c), CU64(0x59bf37e68854636e),
	 CU64(0x54e2c7b6251ed993), CU64(0x88a07828d8255df0), CU64(0x4b5c39176581b872), CU64(0xb0329b82a9ff642b),
	 CU64(0x72682e1a46fed05c), CU64(0x9d16808b96ac2c1d), CU64(0x21df1ffec0bca33e), CU64(0x9812838a91a7241b),
	 CU64(0x2d241b093f534836), CU64(0xca0346c94540068c), CU64(0xa1269487b2d84c35), CU64(0x6b25d24ef7984ab9),
	 CU64(0x42a33ee19d655b7c), CU64(0x96b8722eca1f6de4), CU64(0x53b731e486427362), CU64(0x47a73de09a6e537a),
	 CU64(0x608b20ebab2b0b40), CU64(0xea7aad90d759f447), CU64(0x0eaaf1a45bb849ff), CU64(0x6678221e5ad2f044),
	 CU64(0xab2e9285bcce5c39), CU64(0xfd9da0603d87275d), CU64(0x0000000000000000), CU64(0xb1946f25fb5a35de),
	 CU64(0x03f701f4f6f2f302), CU64(0x12e30ef1edd5db1c), CU64(0xfe6aa194cb75d45f), CU64(0x272c1d0b3145583a),
	 CU64(0x5cbb34e78f5f6b68), CU64(0xbcc99f7556108f23), CU64(0x749b2cefb7072b58), CU64(0xe4d05c348ce1bdb8),
	 CU64(0xf5c4533197c695a6), CU64(0xa37761d4168feec2), CU64(0xb7676dd00aa3ceda), CU64(0xa4229786b5d34433),
	 CU64(0x9be5827e6755d719), CU64(0x238eeaad64eb01c9), CU64(0x2ed31afdc9a1bb34), CU64(0x8da47b29df2e55f6),
	 CU64(0xf0c0503090cd9da0), CU64(0xd7ec4d3ba188c59a), CU64(0xd946bc9ffa308c65), CU64(0x3fc715f8d286932a),
	 CU64(0xf93f57c668297eae), CU64(0x5f4c351379ad986a), CU64(0x1e180a06123a3014), CU64(0x11140f051b27281e),
	 CU64(0xf63352c5613466a4), CU64(0x5544331177bb8866), CU64(0xb6c1997758069f2f), CU64(0x91ed847c6943c715),
	 CU64(0x8ff58e7a7b79f701), CU64(0x85fd8878756fe70d), CU64(0xeed85a3682f7adb4), CU64(0x6c70241c54c4e048),
	 CU64(0xdde44b39af9ed596), CU64(0x2079eb599219f2cb), CU64(0x7860281848e8c050), CU64(0x1345fa56bf708ae9),
	 CU64(0x45f6c8b33e39f18d), CU64(0x4afacdb03724e987), CU64(0xb4906c24fc513dd8), CU64(0xa0806020e07d1dc0),
	 CU64(0x40f2cbb23932f98b), CU64(0xe072ab92d94fe44b), CU64(0x15b6f8a34e8971ed), CU64(0xe7275dc07a134eba),
	 CU64(0x490dcc44c1d61a85), CU64(0xf795a66233913751), CU64(0x5040301070b08060), CU64(0x5eeac1b42b08c99f),
	 CU64(0xae2a9184bbc5543f), CU64(0x5211c543d4e72297), CU64(0xe576a893de44ec4d), CU64(0xed2f5bc274055eb6),
	 CU64(0x7f35de4aebb46aa1), CU64(0x73cedabd145b81a9), CU64(0x89068c8f8a800c05), CU64(0x99b4772dc30275ee),
	 CU64(0x76cad9bc135089af), CU64(0xd64ab99cf32d946f), CU64(0xdfb5be6a0bc97761), CU64(0x5d1dc040ddfa3a9d),
	 CU64(0xd41b4ccf577a3698), CU64(0x10b2fba2498279eb), CU64(0xba3a9d80a7e97427), CU64(0x6e21d14ff09342bf),
	 CU64(0x637c211f5dd9f842), CU64(0xc50f43ca4c5d1e86), CU64(0x3892e3aa71da39db), CU64(0x5715c642d3ec2a91),
};

const Uint64 Crypto::Khazad::T4[256] = {
	 CU64(0xbbb96a01bad3d268), CU64(0xe59a66b154fc4d19), CU64(0xe26514cd2f71bc93), CU64(0x25871b51749ccdb9),
	 CU64(0xf7a257a453f55102), CU64(0xd0d6be03d3686bb8), CU64(0xd6deb504d26b6fbd), CU64(0xb35285fe4dd72964),
	 CU64(0xfdba4aad50f05d0d), CU64(0xcf09e063ace98a26), CU64(0x091c96848d8a0e83), CU64(0xa5914d1abfdcc679),
	 CU64(0x3da7374d7090ddad), CU64(0xf1aa5ca352f65507), CU64(0x7ba417e19ab352c8), CU64(0xb55a8ef94cd42d61),
	 CU64(0x460320acea238f65), CU64(0xc4e68411d56273a6), CU64(0x55cc68c297a466f1), CU64(0xdcc6a80dd16e63b2),
	 CU64(0xaa85d0993355ccff), CU64(0xfbb241aa51f35908), CU64(0xc7e20f9c5bed712a), CU64(0xf359ae55a6f7a204),
	 CU64(0xfebec120de7f5f81), CU64(0xad7aa2e548d83d75), CU64(0xd729cc7fa8e59a32), CU64(0x71bc0ae899b65ec7),
	 CU64(0xe096e63bdb704b90), CU64(0xac8ddb9e3256c8fa), CU64(0x95d11522b7c4e651), CU64(0x32b3aacefc19d72b),
	 CU64(0x704b7393e338ab48), CU64(0x63843bfd9ebf42dc), CU64(0x41fc52d091ae7eef), CU64(0x7dac1ce69bb056cd),
	 CU64(0x76437894e23baf4d), CU64(0xbdb16106bbd0d66d), CU64(0x9b32f1da41c31958), CU64(0x7957e5176eb2a5cb),
	 CU64(0xf941b35ca5f2ae0b), CU64(0x8016564bcb400bc0), CU64(0x677fc20c6bbdb1da), CU64(0x59dc7ecc95a26efb),
	 CU64(0xe1619f40a1febe1f), CU64(0x10cbc3e3f308eb18), CU64(0x81e12f30b1cefe4f), CU64(0x0c10160e0206080a),
	 CU64(0x922e675ecc4917db), CU64(0xa26e3f66c45137f3), CU64(0x4ee8cf531d277469), CU64(0x78a09c6c143c5044),
	 CU64(0xb0560e73c3582be8), CU64(0x573f9a3463a591f2), CU64(0xe69eed3cda734f95), CU64(0xd3d2358e5de76934),
	 CU64(0xdfc223805fe1613e), CU64(0xf2aed72edc79578b), CU64(0x13cf486e7d87e994), CU64(0x94266c59cd4a13de),
	 CU64(0x1fdf5e607f81e19e), CU64(0xc1ea049b5aee752f), CU64(0x7547f3196cb4adc1), CU64(0xd5da3e895ce46d31),
	 CU64(0x08ebeffff704fb0c), CU64(0xd42d47f2266a98be), CU64(0x38abb7c7ff1cdb24), CU64(0x543b11b9ed2a937e),
	 CU64(0x4a1336a2e825876f), CU64(0x699c26f49dba4ed3), CU64(0x7f5fee106fb1a1ce), CU64(0x03048b8d8e8f028c),
	 CU64(0x56c8e34f192b647d), CU64(0xe7699447a0fdba1a), CU64(0x1ad3deeaf00de717), CU64(0x113cba9889861e97),
	 CU64(0x2278692d0f113c33), CU64(0x1238311507091c1b), CU64(0xc511fd6aafec8629), CU64(0x208b9bdbfb10cb30),
	 CU64(0x3040583808182028), CU64(0x7ea8976b153f5441), CU64(0x2e687f230d173439), CU64(0x18202c1c040c1014),
	 CU64(0x06080b0701030405), CU64(0x4507ab2164ac8de9), CU64(0xf8b6ca27df7c5b84), CU64(0x29970d5f769ac5b3),
	 CU64(0x0bef6472798bf980), CU64(0xf4a6dc29dd7a538e), CU64(0x8ef5b2b33d47f4c9), CU64(0x74b08a62163a584e),
	 CU64(0x82e5a4bd3f41fcc3), CU64(0xb2a5fc853759dceb), CU64(0x734ff81e6db7a9c4), CU64(0x90dd95a83848e0d8),
	 CU64(0xb1a17708b9d6de67), CU64(0x37bf2a447395d1a2), CU64(0x4c1b3da5e926836a), CU64(0xbeb5ea8b355fd4e1),
	 CU64(0xe3926db655ff491c), CU64(0x3baf3c4a7193d9a8), CU64(0x07ff727c7b8df18a), CU64(0x0f149d838c890a86),
	 CU64(0x31b721437296d5a7), CU64(0x1734b19f88851a92), CU64(0x0ee3e4f8f607ff09), CU64(0xfc4d33d62a7ea882),
	 CU64(0x84edafba3e42f8c6), CU64(0xd9ca28875ee2653b), CU64(0xd2254cf527699cbb), CU64(0x890ac0cf46ca0543),
	 CU64(0x286074240c14303c), CU64(0x430fa02665af89ec), CU64(0x6d67df0568b8bdd5), CU64(0x5b2f8c3a61a399f8),
	 CU64(0x0a181d0903050c0f), CU64(0xbc46187dc15e23e2), CU64(0xef827bb857f94116), CU64(0xcefe9918d6677fa9),
	 CU64(0xec86f035d976439a), CU64(0xcdfa129558e87d25), CU64(0xea8efb32d875479f), CU64(0x4917bd2f66aa85e3),
	 CU64(0xc8f6921fd7647bac), CU64(0x9ccd83a63a4ee8d2), CU64(0x8a0e4b42c84507cf), CU64(0x88fdb9b43c44f0cc),
	 CU64(0x268390dcfa13cf35), CU64(0x53c463c596a762f4), CU64(0xf551a552a7f4a601), CU64(0x77b401ef98b55ac2),
	 CU64(0x52331abeec29977b), CU64(0xb7a97c0fb8d5da62), CU64(0xa876226fc7543bfc), CU64(0xc319f66daeef822c),
	 CU64(0x6b6fd40269bbb9d0), CU64(0xa762bfec4bdd317a), CU64(0xdd31d176abe0963d), CU64(0xd121c778a9e69e37),
	 CU64(0x4f1fb62867a981e6), CU64(0x3c504e360a1e2822), CU64(0x8f02cbc847c90146), CU64(0x16c3c8e4f20bef1d),
	 CU64(0x99c1032cb5c2ee5b), CU64(0xcc0d6bee226688aa), CU64(0x647b4981e532b356), CU64(0x5e230cb0ee2f9f71),
	 CU64(0xa399461dbedfc27c), CU64(0xfa4538d12b7dac87), CU64(0x217ce2a0819e3ebf), CU64(0x6c90a67e1236485a),
	 CU64(0x2d6cf4ae839836b5), CU64(0x5ad8f5411b2d6c77), CU64(0x2470622a0e123836), CU64(0xca0560e923658caf),
	 CU64(0x04fbf9f1f502f306), CU64(0x8312ddc645cf094c), CU64(0xc61576e7216384a5), CU64(0x9e3e7150ce4f1fd1),
	 CU64(0xab72a9e249db3970), CU64(0xe87d09c42c74b09c), CU64(0x2c9b8dd5f916c33a), CU64(0x6e635488e637bf59),
	 CU64(0x93d91e25b6c7e254), CU64(0xf05d25d82878a088), CU64(0x72b8816517395c4b), CU64(0x2b64ffa9829b32b0),
	 CU64(0x5cd0fe461a2e6872), CU64(0x1d2cac968b80169d), CU64(0x3ea3bcc0fe1fdf21), CU64(0x1b24a7918a831298),
	 CU64(0x3648533f091b242d), CU64(0x8c064045c94603ca), CU64(0x354cd8b2879426a1), CU64(0xb94a98f74ed2256b),
	 CU64(0x7c5b659de13ea342), CU64(0xe46d1fca2e72b896), CU64(0x62734286e431b753), CU64(0x7a536e9ae03da747),
	 CU64(0x400b2babeb208b60), CU64(0x47f459d790ad7aea), CU64(0xff49b85ba4f1aa0e), CU64(0x44f0d25a1e227866),
	 CU64(0x395ccebc85922eab), CU64(0x5d27873d60a09dfd), CU64(0x0000000000000000), CU64(0xde355afb256f94b1),
	 CU64(0x02f3f2f6f401f703), CU64(0x1cdbd5edf10ee312), CU64(0x5fd475cb94a16afe), CU64(0x3a5845310b1d2c27),
	 CU64(0x686b5f8fe734bb5c), CU64(0x238f1056759fc9bc), CU64(0x582b07b7ef2c9b74), CU64(0xb8bde18c345cd0e4),
	 CU64(0xa695c6973153c4f5), CU64(0xc2ee8f16d46177a3), CU64(0xdacea30ad06d67b7), CU64(0x3344d3b5869722a4),
	 CU64(0x19d755677e82e59b), CU64(0xc901eb64adea8e23), CU64(0x34bba1c9fd1ad32e), CU64(0xf6552edf297ba48d),
	 CU64(0xa09dcd903050c0f0), CU64(0x9ac588a13b4decd7), CU64(0x658c30fa9fbc46d9), CU64(0x2a9386d2f815c73f),
	 CU64(0xae7e2968c6573ff9), CU64(0x6a98ad7913354c5f), CU64(0x14303a12060a181e), CU64(0x1e28271b050f1411),
	 CU64(0xa4663461c55233f6), CU64(0x6688bb7711334455), CU64(0x2f9f06587799c1b6), CU64(0x15c743697c84ed91),
	 CU64(0x01f7797b7a8ef58f), CU64(0x0de76f757888fd85), CU64(0xb4adf782365ad8ee), CU64(0x48e0c4541c24706c),
	 CU64(0x96d59eaf394be4dd), CU64(0xcbf2199259eb7920), CU64(0x50c0e84818286078), CU64(0xe98a70bf56fa4513),
	 CU64(0x8df1393eb3c8f645), CU64(0x87e92437b0cdfa4a), CU64(0xd83d51fc246c90b4), CU64(0xc01d7de0206080a0),
	 CU64(0x8bf93239b2cbf240), CU64(0x4be44fd992ab72e0), CU64(0xed71894ea3f8b615), CU64(0xba4e137ac05d27e7),
	 CU64(0x851ad6c144cc0d49), CU64(0x5137913362a695f7), CU64(0x6080b07010304050), CU64(0x9fc9082bb4c1ea5e),
	 CU64(0x3f54c5bb84912aae), CU64(0x9722e7d443c51152), CU64(0x4dec44de93a876e5), CU64(0xb65e0574c25b2fed),
	 CU64(0xa16ab4eb4ade357f), CU64(0xa9815b14bddace73), CU64(0x050c808a8f8c0689), CU64(0xee7502c32d77b499),
	 CU64(0xaf895013bcd9ca76), CU64(0x6f942df39cb94ad6), CU64(0x6177c90b6abeb5df), CU64(0x9d3afadd40c01d5d),
	 CU64(0x98367a57cf4c1bd4), CU64(0xeb798249a2fbb210), CU64(0x2774e9a7809d3aba), CU64(0xbf4293f04fd1216e),
	 CU64(0x42f8d95d1f217c63), CU64(0x861e5d4cca430fc5), CU64(0xdb39da71aae39238), CU64(0x912aecd342c61557),
};

const Uint64 Crypto::Khazad::T5[256] = {
	 CU64(0xb9bb016ad3ba68d2), CU64(0x9ae5b166fc54194d), CU64(0x65e2cd14712f93bc), CU64(0x8725511b9c74b9cd),
	 CU64(0xa2f7a457f5530251), CU64(0xd6d003be68d3b86b), CU64(0xded604b56bd2bd6f), CU64(0x52b3fe85d74d6429),
	 CU64(0xbafdad4af0500d5d), CU64(0x09cf63e0e9ac268a), CU64(0x1c0984968a8d830e), CU64(0x91a51a4ddcbf79c6),
	 CU64(0xa73d4d379070addd), CU64(0xaaf1a35cf6520755), CU64(0xa47be117b39ac852), CU64(0x5ab5f98ed44c612d),
	 CU64(0x0346ac2023ea658f), CU64(0xe6c4118462d5a673), CU64(0xcc55c268a497f166), CU64(0xc6dc0da86ed1b263),
	 CU64(0x85aa99d05533ffcc), CU64(0xb2fbaa41f3510859), CU64(0xe2c79c0fed5b2a71), CU64(0x59f355aef7a604a2),
	 CU64(0xbefe20c17fde815f), CU64(0x7aade5a2d848753d), CU64(0x29d77fcce5a8329a), CU64(0xbc71e80ab699c75e),
	 CU64(0x96e03be670db904b), CU64(0x8dac9edb5632fac8), CU64(0xd1952215c4b751e6), CU64(0xb332ceaa19fc2bd7),
	 CU64(0x4b70937338e348ab), CU64(0x8463fd3bbf9edc42), CU64(0xfc41d052ae91ef7e), CU64(0xac7de61cb09bcd56),
	 CU64(0x437694783be24daf), CU64(0xb1bd0661d0bb6dd6), CU64(0x329bdaf1c3415819), CU64(0x577917e5b26ecba5),
	 CU64(0x41f95cb3f2a50bae), CU64(0x16804b5640cbc00b), CU64(0x7f670cc2bd6bdab1), CU64(0xdc59cc7ea295fb6e),
	 CU64(0x61e1409ffea11fbe), CU64(0xcb10e3c308f318eb), CU64(0xe181302fceb14ffe), CU64(0x100c0e1606020a08),
	 CU64(0x2e925e6749ccdb17), CU64(0x6ea2663f51c4f337), CU64(0xe84e53cf271d6974), CU64(0xa0786c9c3c144450),
	 CU64(0x56b0730e58c3e82b), CU64(0x3f57349aa563f291), CU64(0x9ee63ced73da954f), CU64(0xd2d38e35e75d3469),
	 CU64(0xc2df8023e15f3e61), CU64(0xaef22ed779dc8b57), CU64(0xcf136e48877d94e9), CU64(0x2694596c4acdde13),
	 CU64(0xdf1f605e817f9ee1), CU64(0xeac19b04ee5a2f75), CU64(0x477519f3b46cc1ad), CU64(0xdad5893ee45c316d),
	 CU64(0xeb08ffef04f70cfb), CU64(0x2dd4f2476a26be98), CU64(0xab38c7b71cff24db), CU64(0x3b54b9112aed7e93),
	 CU64(0x134aa23625e86f87), CU64(0x9c69f426ba9dd34e), CU64(0x5f7f10eeb16fcea1), CU64(0x04038d8b8f8e8c02),
	 CU64(0xc8564fe32b197d64), CU64(0x69e74794fda01aba), CU64(0xd31aeade0df017e7), CU64(0x3c1198ba8689971e),
	 CU64(0x78222d69110f333c), CU64(0x3812153109071b1c), CU64(0x11c56afdecaf2986), CU64(0x8b20db9b10fb30cb),
	 CU64(0x4030385818082820), CU64(0xa87e6b973f154154), CU64(0x682e237f170d3934), CU64(0x20181c2c0c041410),
	 CU64(0x0806070b03010504), CU64(0x074521abac64e98d), CU64(0xb6f827ca7cdf845b), CU64(0x97295f0d9a76b3c5),
	 CU64(0xef0b72648b7980f9), CU64(0xa6f429dc7add8e53), CU64(0xf58eb3b2473dc9f4), CU64(0xb074628a3a164e58),
	 CU64(0xe582bda4413fc3fc), CU64(0xa5b285fc5937ebdc), CU64(0x4f731ef8b76dc4a9), CU64(0xdd90a8954838d8e0),
	 CU64(0xa1b10877d6b967de), CU64(0xbf37442a9573a2d1), CU64(0x1b4ca53d26e96a83), CU64(0xb5be8bea5f35e1d4),
	 CU64(0x92e3b66dff551c49), CU64(0xaf3b4a3c9371a8d9), CU64(0xff077c728d7b8af1), CU64(0x140f839d898c860a),
	 CU64(0xb73143219672a7d5), CU64(0x34179fb18588921a), CU64(0xe30ef8e407f609ff), CU64(0x4dfcd6337e2a82a8),
	 CU64(0xed84baaf423ec6f8), CU64(0xcad98728e25e3b65), CU64(0x25d2f54c6927bb9c), CU64(0x0a89cfc0ca464305),
	 CU64(0x60282474140c3c30), CU64(0x0f4326a0af65ec89), CU64(0x676d05dfb868d5bd), CU64(0x2f5b3a8ca361f899),
	 CU64(0x180a091d05030f0c), CU64(0x46bc7d185ec1e223), CU64(0x82efb87bf9571641), CU64(0xfece189967d6a97f),
	 CU64(0x86ec35f076d99a43), CU64(0xfacd9512e858257d), CU64(0x8eea32fb75d89f47), CU64(0x17492fbdaa66e385),
	 CU64(0xf6c81f9264d7ac7b), CU64(0xcd9ca6834e3ad2e8), CU64(0x0e8a424b45c8cf07), CU64(0xfd88b4b9443cccf0),
	 CU64(0x8326dc9013fa35cf), CU64(0xc453c563a796f462), CU64(0x51f552a5f4a701a6), CU64(0xb477ef01b598c25a),
	 CU64(0x3352be1a29ec7b97), CU64(0xa9b70f7cd5b862da), CU64(0x76a86f2254c7fc3b), CU64(0x19c36df6efae2c82),
	 CU64(0x6f6b02d4bb69d0b9), CU64(0x62a7ecbfdd4b7a31), CU64(0x31dd76d1e0ab3d96), CU64(0x21d178c7e6a9379e),
	 CU64(0x1f4f28b6a967e681), CU64(0x503c364e1e0a2228), CU64(0x028fc8cbc9474601), CU64(0xc316e4c80bf21def),
	 CU64(0xc1992c03c2b55bee), CU64(0x0dccee6b6622aa88), CU64(0x7b64814932e556b3), CU64(0x235eb00c2fee719f),
	 CU64(0x99a31d46dfbe7cc2), CU64(0x45fad1387d2b87ac), CU64(0x7c21a0e29e81bf3e), CU64(0x906c7ea636125a48),
	 CU64(0x6c2daef49883b536), CU64(0xd85a41f52d1b776c), CU64(0x70242a62120e3638), CU64(0x05cae9606523af8c),
	 CU64(0xfb04f1f902f506f3), CU64(0x1283c6ddcf454c09), CU64(0x15c6e7766321a584), CU64(0x3e9e50714fced11f),
	 CU64(0x72abe2a9db497039), CU64(0x7de8c409742c9cb0), CU64(0x9b2cd58d16f93ac3), CU64(0x636e885437e659bf),
	 CU64(0xd993251ec7b654e2), CU64(0x5df0d825782888a0), CU64(0xb872658139174b5c), CU64(0x642ba9ff9b82b032),
	 CU64(0xd05c46fe2e1a7268), CU64(0x2c1d96ac808b9d16), CU64(0xa33ec0bc1ffe21df), CU64(0x241b91a7838a9812),
	 CU64(0x48363f531b092d24), CU64(0x068c454046c9ca03), CU64(0x4c35b2d89487a126), CU64(0x4ab9f798d24e6b25),
	 CU64(0x5b7c9d653ee142a3), CU64(0x6de4ca1f722e96b8), CU64(0x7362864231e453b7), CU64(0x537a9a6e3de047a7),
	 CU64(0x0b40ab2b20eb608b), CU64(0xf447d759ad90ea7a), CU64(0x49ff5bb8f1a40eaa), CU64(0xf0445ad2221e6678),
	 CU64(0x5c39bcce9285ab2e), CU64(0x275d3d87a060fd9d), CU64(0x0000000000000000), CU64(0x35defb5a6f25b194),
	 CU64(0xf302f6f201f403f7), CU64(0xdb1cedd50ef112e3), CU64(0xd45fcb75a194fe6a), CU64(0x583a31451d0b272c),
	 CU64(0x6b688f5f34e75cbb), CU64(0x8f2356109f75bcc9), CU64(0x2b58b7072cef749b), CU64(0xbdb88ce15c34e4d0),
	 CU64(0x95a697c65331f5c4), CU64(0xeec2168f61d4a377), CU64(0xceda0aa36dd0b767), CU64(0x4433b5d39786a422),
	 CU64(0xd7196755827e9be5), CU64(0x01c964ebeaad238e), CU64(0xbb34c9a11afd2ed3), CU64(0x55f6df2e7b298da4),
	 CU64(0x9da090cd5030f0c0), CU64(0xc59aa1884d3bd7ec), CU64(0x8c65fa30bc9fd946), CU64(0x932ad28615f83fc7),
	 CU64(0x7eae682957c6f93f), CU64(0x986a79ad35135f4c), CU64(0x3014123a0a061e18), CU64(0x281e1b270f051114),
	 CU64(0x66a4613452c5f633), CU64(0x886677bb33115544), CU64(0x9f2f58069977b6c1), CU64(0xc7156943847c91ed),
	 CU64(0xf7017b798e7a8ff5), CU64(0xe70d756f887885fd), CU64(0xadb482f75a36eed8), CU64(0xe04854c4241c6c70),
	 CU64(0xd596af9e4b39dde4), CU64(0xf2cb9219eb592079), CU64(0xc05048e828187860), CU64(0x8ae9bf70fa561345),
	 CU64(0xf18d3e39c8b345f6), CU64(0xe9873724cdb04afa), CU64(0x3dd8fc516c24b490), CU64(0x1dc0e07d6020a080),
	 CU64(0xf98b3932cbb240f2), CU64(0xe44bd94fab92e072), CU64(0x71ed4e89f8a315b6), CU64(0x4eba7a135dc0e727),
	 CU64(0x1a85c1d6cc44490d), CU64(0x37513391a662f795), CU64(0x806070b030105040), CU64(0xc99f2b08c1b45eea),
	 CU64(0x543fbbc59184ae2a), CU64(0x2297d4e7c5435211), CU64(0xec4dde44a893e576), CU64(0x5eb674055bc2ed2f),
	 CU64(0x6aa1ebb4de4a7f35), CU64(0x81a9145bdabd73ce), CU64(0x0c058a808c8f8906), CU64(0x75eec302772d99b4),
	 CU64(0x89af1350d9bc76ca), CU64(0x946ff32db99cd64a), CU64(0x77610bc9be6adfb5), CU64(0x3a9dddfac0405d1d),
	 CU64(0x3698577a4ccfd41b), CU64(0x79eb4982fba210b2), CU64(0x7427a7e99d80ba3a), CU64(0x42bff093d14f6e21),
	 CU64(0xf8425dd9211f637c), CU64(0x1e864c5d43cac50f), CU64(0x39db71dae3aa3892), CU64(0x2a91d3ecc6425715),
};

const Uint64 Crypto::Khazad::T6[256] = {
	 CU64(0x6a01bbb9d268bad3), CU64(0x66b1e59a4d1954fc), CU64(0x14cde265bc932f71), CU64(0x1b512587cdb9749c),
	 CU64(0x57a4f7a2510253f5), CU64(0xbe03d0d66bb8d368), CU64(0xb504d6de6fbdd26b), CU64(0x85feb35229644dd7),
	 CU64(0x4aadfdba5d0d50f0), CU64(0xe063cf098a26ace9), CU64(0x9684091c0e838d8a), CU64(0x4d1aa591c679bfdc),
	 CU64(0x374d3da7ddad7090), CU64(0x5ca3f1aa550752f6), CU64(0x17e17ba452c89ab3), CU64(0x8ef9b55a2d614cd4),
	 CU64(0x20ac46038f65ea23), CU64(0x8411c4e673a6d562), CU64(0x68c255cc66f197a4), CU64(0xa80ddcc663b2d16e),
	 CU64(0xd099aa85ccff3355), CU64(0x41aafbb2590851f3), CU64(0x0f9cc7e2712a5bed), CU64(0xae55f359a204a6f7),
	 CU64(0xc120febe5f81de7f), CU64(0xa2e5ad7a3d7548d8), CU64(0xcc7fd7299a32a8e5), CU64(0x0ae871bc5ec799b6),
	 CU64(0xe63be0964b90db70), CU64(0xdb9eac8dc8fa3256), CU64(0x152295d1e651b7c4), CU64(0xaace32b3d72bfc19),
	 CU64(0x7393704bab48e338), CU64(0x3bfd638442dc9ebf), CU64(0x52d041fc7eef91ae), CU64(0x1ce67dac56cd9bb0),
	 CU64(0x78947643af4de23b), CU64(0x6106bdb1d66dbbd0), CU64(0xf1da9b32195841c3), CU64(0xe5177957a5cb6eb2),
	 CU64(0xb35cf941ae0ba5f2), CU64(0x564b80160bc0cb40), CU64(0xc20c677fb1da6bbd), CU64(0x7ecc59dc6efb95a2),
	 CU64(0x9f40e161be1fa1fe), CU64(0xc3e310cbeb18f308), CU64(0x2f3081e1fe4fb1ce), CU64(0x160e0c10080a0206),
	 CU64(0x675e922e17dbcc49), CU64(0x3f66a26e37f3c451), CU64(0xcf534ee874691d27), CU64(0x9c6c78a05044143c),
	 CU64(0x0e73b0562be8c358), CU64(0x9a34573f91f263a5), CU64(0xed3ce69e4f95da73), CU64(0x358ed3d269345de7),
	 CU64(0x2380dfc2613e5fe1), CU64(0xd72ef2ae578bdc79), CU64(0x486e13cfe9947d87), CU64(0x6c59942613decd4a),
	 CU64(0x5e601fdfe19e7f81), CU64(0x049bc1ea752f5aee), CU64(0xf3197547adc16cb4), CU64(0x3e89d5da6d315ce4),
	 CU64(0xefff08ebfb0cf704), CU64(0x47f2d42d98be266a), CU64(0xb7c738abdb24ff1c), CU64(0x11b9543b937eed2a),
	 CU64(0x36a24a13876fe825), CU64(0x26f4699c4ed39dba), CU64(0xee107f5fa1ce6fb1), CU64(0x8b8d0304028c8e8f),
	 CU64(0xe34f56c8647d192b), CU64(0x9447e769ba1aa0fd), CU64(0xdeea1ad3e717f00d), CU64(0xba98113c1e978986),
	 CU64(0x692d22783c330f11), CU64(0x311512381c1b0709), CU64(0xfd6ac5118629afec), CU64(0x9bdb208bcb30fb10),
	 CU64(0x5838304020280818), CU64(0x976b7ea85441153f), CU64(0x7f232e6834390d17), CU64(0x2c1c18201014040c),
	 CU64(0x0b07060804050103), CU64(0xab2145078de964ac), CU64(0xca27f8b65b84df7c), CU64(0x0d5f2997c5b3769a),
	 CU64(0x64720beff980798b), CU64(0xdc29f4a6538edd7a), CU64(0xb2b38ef5f4c93d47), CU64(0x8a6274b0584e163a),
	 CU64(0xa4bd82e5fcc33f41), CU64(0xfc85b2a5dceb3759), CU64(0xf81e734fa9c46db7), CU64(0x95a890dde0d83848),
	 CU64(0x7708b1a1de67b9d6), CU64(0x2a4437bfd1a27395), CU64(0x3da54c1b836ae926), CU64(0xea8bbeb5d4e1355f),
	 CU64(0x6db6e392491c55ff), CU64(0x3c4a3bafd9a87193), CU64(0x727c07fff18a7b8d), CU64(0x9d830f140a868c89),
	 CU64(0x214331b7d5a77296), CU64(0xb19f17341a928885), CU64(0xe4f80ee3ff09f607), CU64(0x33d6fc4da8822a7e),
	 CU64(0xafba84edf8c63e42), CU64(0x2887d9ca653b5ee2), CU64(0x4cf5d2259cbb2769), CU64(0xc0cf890a054346ca),
	 CU64(0x74242860303c0c14), CU64(0xa026430f89ec65af), CU64(0xdf056d67bdd568b8), CU64(0x8c3a5b2f99f861a3),
	 CU64(0x1d090a180c0f0305), CU64(0x187dbc4623e2c15e), CU64(0x7bb8ef82411657f9), CU64(0x9918cefe7fa9d667),
	 CU64(0xf035ec86439ad976), CU64(0x1295cdfa7d2558e8), CU64(0xfb32ea8e479fd875), CU64(0xbd2f491785e366aa),
	 CU64(0x921fc8f67bacd764), CU64(0x83a69ccde8d23a4e), CU64(0x4b428a0e07cfc845), CU64(0xb9b488fdf0cc3c44),
	 CU64(0x90dc2683cf35fa13), CU64(0x63c553c462f496a7), CU64(0xa552f551a601a7f4), CU64(0x01ef77b45ac298b5),
	 CU64(0x1abe5233977bec29), CU64(0x7c0fb7a9da62b8d5), CU64(0x226fa8763bfcc754), CU64(0xf66dc319822caeef),
	 CU64(0xd4026b6fb9d069bb), CU64(0xbfeca762317a4bdd), CU64(0xd176dd31963dabe0), CU64(0xc778d1219e37a9e6),
	 CU64(0xb6284f1f81e667a9), CU64(0x4e363c5028220a1e), CU64(0xcbc88f02014647c9), CU64(0xc8e416c3ef1df20b),
	 CU64(0x032c99c1ee5bb5c2), CU64(0x6beecc0d88aa2266), CU64(0x4981647bb356e532), CU64(0x0cb05e239f71ee2f),
	 CU64(0x461da399c27cbedf), CU64(0x38d1fa45ac872b7d), CU64(0xe2a0217c3ebf819e), CU64(0xa67e6c90485a1236),
	 CU64(0xf4ae2d6c36b58398), CU64(0xf5415ad86c771b2d), CU64(0x622a247038360e12), CU64(0x60e9ca058caf2365),
	 CU64(0xf9f104fbf306f502), CU64(0xddc68312094c45cf), CU64(0x76e7c61584a52163), CU64(0x71509e3e1fd1ce4f),
	 CU64(0xa9e2ab72397049db), CU64(0x09c4e87db09c2c74), CU64(0x8dd52c9bc33af916), CU64(0x54886e63bf59e637),
	 CU64(0x1e2593d9e254b6c7), CU64(0x25d8f05da0882878), CU64(0x816572b85c4b1739), CU64(0xffa92b6432b0829b),
	 CU64(0xfe465cd068721a2e), CU64(0xac961d2c169d8b80), CU64(0xbcc03ea3df21fe1f), CU64(0xa7911b2412988a83),
	 CU64(0x533f3648242d091b), CU64(0x40458c0603cac946), CU64(0xd8b2354c26a18794), CU64(0x98f7b94a256b4ed2),
	 CU64(0x659d7c5ba342e13e), CU64(0x1fcae46db8962e72), CU64(0x42866273b753e431), CU64(0x6e9a7a53a747e03d),
	 CU64(0x2bab400b8b60eb20), CU64(0x59d747f47aea90ad), CU64(0xb85bff49aa0ea4f1), CU64(0xd25a44f078661e22),
	 CU64(0xcebc395c2eab8592), CU64(0x873d5d279dfd60a0), CU64(0x0000000000000000), CU64(0x5afbde3594b1256f),
	 CU64(0xf2f602f3f703f401), CU64(0xd5ed1cdbe312f10e), CU64(0x75cb5fd46afe94a1), CU64(0x45313a582c270b1d),
	 CU64(0x5f8f686bbb5ce734), CU64(0x1056238fc9bc759f), CU64(0x07b7582b9b74ef2c), CU64(0xe18cb8bdd0e4345c),
	 CU64(0xc697a695c4f53153), CU64(0x8f16c2ee77a3d461), CU64(0xa30adace67b7d06d), CU64(0xd3b5334422a48697),
	 CU64(0x556719d7e59b7e82), CU64(0xeb64c9018e23adea), CU64(0xa1c934bbd32efd1a), CU64(0x2edff655a48d297b),
	 CU64(0xcd90a09dc0f03050), CU64(0x88a19ac5ecd73b4d), CU64(0x30fa658c46d99fbc), CU64(0x86d22a93c73ff815),
	 CU64(0x2968ae7e3ff9c657), CU64(0xad796a984c5f1335), CU64(0x3a121430181e060a), CU64(0x271b1e281411050f),
	 CU64(0x3461a46633f6c552), CU64(0xbb77668844551133), CU64(0x06582f9fc1b67799), CU64(0x436915c7ed917c84),
	 CU64(0x797b01f7f58f7a8e), CU64(0x6f750de7fd857888), CU64(0xf782b4add8ee365a), CU64(0xc45448e0706c1c24),
	 CU64(0x9eaf96d5e4dd394b), CU64(0x1992cbf2792059eb), CU64(0xe84850c060781828), CU64(0x70bfe98a451356fa),
	 CU64(0x393e8df1f645b3c8), CU64(0x243787e9fa4ab0cd), CU64(0x51fcd83d90b4246c), CU64(0x7de0c01d80a02060),
	 CU64(0x32398bf9f240b2cb), CU64(0x4fd94be472e092ab), CU64(0x894eed71b615a3f8), CU64(0x137aba4e27e7c05d),
	 CU64(0xd6c1851a0d4944cc), CU64(0x9133513795f762a6), CU64(0xb070608040501030), CU64(0x082b9fc9ea5eb4c1),
	 CU64(0xc5bb3f542aae8491), CU64(0xe7d49722115243c5), CU64(0x44de4dec76e593a8), CU64(0x0574b65e2fedc25b),
	 CU64(0xb4eba16a357f4ade), CU64(0x5b14a981ce73bdda), CU64(0x808a050c06898f8c), CU64(0x02c3ee75b4992d77),
	 CU64(0x5013af89ca76bcd9), CU64(0x2df36f944ad69cb9), CU64(0xc90b6177b5df6abe), CU64(0xfadd9d3a1d5d40c0),
	 CU64(0x7a5798361bd4cf4c), CU64(0x8249eb79b210a2fb), CU64(0xe9a727743aba809d), CU64(0x93f0bf42216e4fd1),
	 CU64(0xd95d42f87c631f21), CU64(0x5d4c861e0fc5ca43), CU64(0xda71db399238aae3), CU64(0xecd3912a155742c6),
};

const Uint64 Crypto::Khazad::T7[256] = {
	 CU64(0x016ab9bb68d2d3ba), CU64(0xb1669ae5194dfc54), CU64(0xcd1465e293bc712f), CU64(0x511b8725b9cd9c74),
	 CU64(0xa457a2f70251f553), CU64(0x03bed6d0b86b68d3), CU64(0x04b5ded6bd6f6bd2), CU64(0xfe8552b36429d74d),
	 CU64(0xad4abafd0d5df050), CU64(0x63e009cf268ae9ac), CU64(0x84961c09830e8a8d), CU64(0x1a4d91a579c6dcbf),
	 CU64(0x4d37a73daddd9070), CU64(0xa35caaf10755f652), CU64(0xe117a47bc852b39a), CU64(0xf98e5ab5612dd44c),
	 CU64(0xac200346658f23ea), CU64(0x1184e6c4a67362d5), CU64(0xc268cc55f166a497), CU64(0x0da8c6dcb2636ed1),
	 CU64(0x99d085aaffcc5533), CU64(0xaa41b2fb0859f351), CU64(0x9c0fe2c72a71ed5b), CU64(0x55ae59f304a2f7a6),
	 CU64(0x20c1befe815f7fde), CU64(0xe5a27aad753dd848), CU64(0x7fcc29d7329ae5a8), CU64(0xe80abc71c75eb699),
	 CU64(0x3be696e0904b70db), CU64(0x9edb8dacfac85632), CU64(0x2215d19551e6c4b7), CU64(0xceaab3322bd719fc),
	 CU64(0x93734b7048ab38e3), CU64(0xfd3b8463dc42bf9e), CU64(0xd052fc41ef7eae91), CU64(0xe61cac7dcd56b09b),
	 CU64(0x947843764daf3be2), CU64(0x0661b1bd6dd6d0bb), CU64(0xdaf1329b5819c341), CU64(0x17e55779cba5b26e),
	 CU64(0x5cb341f90baef2a5), CU64(0x4b561680c00b40cb), CU64(0x0cc27f67dab1bd6b), CU64(0xcc7edc59fb6ea295),
	 CU64(0x409f61e11fbefea1), CU64(0xe3c3cb1018eb08f3), CU64(0x302fe1814ffeceb1), CU64(0x0e16100c0a080602),
	 CU64(0x5e672e92db1749cc), CU64(0x663f6ea2f33751c4), CU64(0x53cfe84e6974271d), CU64(0x6c9ca07844503c14),
	 CU64(0x730e56b0e82b58c3), CU64(0x349a3f57f291a563), CU64(0x3ced9ee6954f73da), CU64(0x8e35d2d33469e75d),
	 CU64(0x8023c2df3e61e15f), CU64(0x2ed7aef28b5779dc), CU64(0x6e48cf1394e9877d), CU64(0x596c2694de134acd),
	 CU64(0x605edf1f9ee1817f), CU64(0x9b04eac12f75ee5a), CU64(0x19f34775c1adb46c), CU64(0x893edad5316de45c),
	 CU64(0xffefeb080cfb04f7), CU64(0xf2472dd4be986a26), CU64(0xc7b7ab3824db1cff), CU64(0xb9113b547e932aed),
	 CU64(0xa236134a6f8725e8), CU64(0xf4269c69d34eba9d), CU64(0x10ee5f7fcea1b16f), CU64(0x8d8b04038c028f8e),
	 CU64(0x4fe3c8567d642b19), CU64(0x479469e71abafda0), CU64(0xeaded31a17e70df0), CU64(0x98ba3c11971e8689),
	 CU64(0x2d697822333c110f), CU64(0x153138121b1c0907), CU64(0x6afd11c52986ecaf), CU64(0xdb9b8b2030cb10fb),
	 CU64(0x3858403028201808), CU64(0x6b97a87e41543f15), CU64(0x237f682e3934170d), CU64(0x1c2c201814100c04),
	 CU64(0x070b080605040301), CU64(0x21ab0745e98dac64), CU64(0x27cab6f8845b7cdf), CU64(0x5f0d9729b3c59a76),
	 CU64(0x7264ef0b80f98b79), CU64(0x29dca6f48e537add), CU64(0xb3b2f58ec9f4473d), CU64(0x628ab0744e583a16),
	 CU64(0xbda4e582c3fc413f), CU64(0x85fca5b2ebdc5937), CU64(0x1ef84f73c4a9b76d), CU64(0xa895dd90d8e04838),
	 CU64(0x0877a1b167ded6b9), CU64(0x442abf37a2d19573), CU64(0xa53d1b4c6a8326e9), CU64(0x8beab5bee1d45f35),
	 CU64(0xb66d92e31c49ff55), CU64(0x4a3caf3ba8d99371), CU64(0x7c72ff078af18d7b), CU64(0x839d140f860a898c),
	 CU64(0x4321b731a7d59672), CU64(0x9fb13417921a8588), CU64(0xf8e4e30e09ff07f6), CU64(0xd6334dfc82a87e2a),
	 CU64(0xbaafed84c6f8423e), CU64(0x8728cad93b65e25e), CU64(0xf54c25d2bb9c6927), CU64(0xcfc00a894305ca46),
	 CU64(0x247460283c30140c), CU64(0x26a00f43ec89af65), CU64(0x05df676dd5bdb868), CU64(0x3a8c2f5bf899a361),
	 CU64(0x091d180a0f0c0503), CU64(0x7d1846bce2235ec1), CU64(0xb87b82ef1641f957), CU64(0x1899fecea97f67d6),
	 CU64(0x35f086ec9a4376d9), CU64(0x9512facd257de858), CU64(0x32fb8eea9f4775d8), CU64(0x2fbd1749e385aa66),
	 CU64(0x1f92f6c8ac7b64d7), CU64(0xa683cd9cd2e84e3a), CU64(0x424b0e8acf0745c8), CU64(0xb4b9fd88ccf0443c),
	 CU64(0xdc90832635cf13fa), CU64(0xc563c453f462a796), CU64(0x52a551f501a6f4a7), CU64(0xef01b477c25ab598),
	 CU64(0xbe1a33527b9729ec), CU64(0x0f7ca9b762dad5b8), CU64(0x6f2276a8fc3b54c7), CU64(0x6df619c32c82efae),
	 CU64(0x02d46f6bd0b9bb69), CU64(0xecbf62a77a31dd4b), CU64(0x76d131dd3d96e0ab), CU64(0x78c721d1379ee6a9),
	 CU64(0x28b61f4fe681a967), CU64(0x364e503c22281e0a), CU64(0xc8cb028f4601c947), CU64(0xe4c8c3161def0bf2),
	 CU64(0x2c03c1995beec2b5), CU64(0xee6b0dccaa886622), CU64(0x81497b6456b332e5), CU64(0xb00c235e719f2fee),
	 CU64(0x1d4699a37cc2dfbe), CU64(0xd13845fa87ac7d2b), CU64(0xa0e27c21bf3e9e81), CU64(0x7ea6906c5a483612),
	 CU64(0xaef46c2db5369883), CU64(0x41f5d85a776c2d1b), CU64(0x2a6270243638120e), CU64(0xe96005caaf8c6523),
	 CU64(0xf1f9fb0406f302f5), CU64(0xc6dd12834c09cf45), CU64(0xe77615c6a5846321), CU64(0x50713e9ed11f4fce),
	 CU64(0xe2a972ab7039db49), CU64(0xc4097de89cb0742c), CU64(0xd58d9b2c3ac316f9), CU64(0x8854636e59bf37e6),
	 CU64(0x251ed99354e2c7b6), CU64(0xd8255df088a07828), CU64(0x6581b8724b5c3917), CU64(0xa9ff642bb0329b82),
	 CU64(0x46fed05c72682e1a), CU64(0x96ac2c1d9d16808b), CU64(0xc0bca33e21df1ffe), CU64(0x91a7241b9812838a),
	 CU64(0x3f5348362d241b09), CU64(0x4540068cca0346c9), CU64(0xb2d84c35a1269487), CU64(0xf7984ab96b25d24e),
	 CU64(0x9d655b7c42a33ee1), CU64(0xca1f6de496b8722e), CU64(0x8642736253b731e4), CU64(0x9a6e537a47a73de0),
	 CU64(0xab2b0b40608b20eb), CU64(0xd759f447ea7aad90), CU64(0x5bb849ff0eaaf1a4), CU64(0x5ad2f0446678221e),
	 CU64(0xbcce5c39ab2e9285), CU64(0x3d87275dfd9da060), CU64(0x0000000000000000), CU64(0xfb5a35deb1946f25),
	 CU64(0xf6f2f30203f701f4), CU64(0xedd5db1c12e30ef1), CU64(0xcb75d45ffe6aa194), CU64(0x3145583a272c1d0b),
	 CU64(0x8f5f6b685cbb34e7), CU64(0x56108f23bcc99f75), CU64(0xb7072b58749b2cef), CU64(0x8ce1bdb8e4d05c34),
	 CU64(0x97c695a6f5c45331), CU64(0x168feec2a37761d4), CU64(0x0aa3cedab7676dd0), CU64(0xb5d34433a4229786),
	 CU64(0x6755d7199be5827e), CU64(0x64eb01c9238eeaad), CU64(0xc9a1bb342ed31afd), CU64(0xdf2e55f68da47b29),
	 CU64(0x90cd9da0f0c05030), CU64(0xa188c59ad7ec4d3b), CU64(0xfa308c65d946bc9f), CU64(0xd286932a3fc715f8),
	 CU64(0x68297eaef93f57c6), CU64(0x79ad986a5f4c3513), CU64(0x123a30141e180a06), CU64(0x1b27281e11140f05),
	 CU64(0x613466a4f63352c5), CU64(0x77bb886655443311), CU64(0x58069f2fb6c19977), CU64(0x6943c71591ed847c),
	 CU64(0x7b79f7018ff58e7a), CU64(0x756fe70d85fd8878), CU64(0x82f7adb4eed85a36), CU64(0x54c4e0486c70241c),
	 CU64(0xaf9ed596dde44b39), CU64(0x9219f2cb2079eb59), CU64(0x48e8c05078602818), CU64(0xbf708ae91345fa56),
	 CU64(0x3e39f18d45f6c8b3), CU64(0x3724e9874afacdb0), CU64(0xfc513dd8b4906c24), CU64(0xe07d1dc0a0806020),
	 CU64(0x3932f98b40f2cbb2), CU64(0xd94fe44be072ab92), CU64(0x4e8971ed15b6f8a3), CU64(0x7a134ebae7275dc0),
	 CU64(0xc1d61a85490dcc44), CU64(0x33913751f795a662), CU64(0x70b0806050403010), CU64(0x2b08c99f5eeac1b4),
	 CU64(0xbbc5543fae2a9184), CU64(0xd4e722975211c543), CU64(0xde44ec4de576a893), CU64(0x74055eb6ed2f5bc2),
	 CU64(0xebb46aa17f35de4a), CU64(0x145b81a973cedabd), CU64(0x8a800c0589068c8f), CU64(0xc30275ee99b4772d),
	 CU64(0x135089af76cad9bc), CU64(0xf32d946fd64ab99c), CU64(0x0bc97761dfb5be6a), CU64(0xddfa3a9d5d1dc040),
	 CU64(0x577a3698d41b4ccf), CU64(0x498279eb10b2fba2), CU64(0xa7e97427ba3a9d80), CU64(0xf09342bf6e21d14f),
	 CU64(0x5dd9f842637c211f), CU64(0x4c5d1e86c50f43ca), CU64(0x71da39db3892e3aa), CU64(0xd3ec2a915715c642),
};

// ============================================================================

const Uint64 Crypto::Khazad::c[R + 1] = {
	 CU64(0xba542f7453d3d24d),
	 CU64(0x50ac8dbf70529a4c),
	 CU64(0xead597d133515ba6),
	 CU64(0xde48a899db32b7fc),
	 CU64(0xe39e919be2bb416e),
	 CU64(0xa5cb6b95a1f3b102),
	 CU64(0xccc41d14c363da5d),
	 CU64(0x5fdc7dcd7f5a6c5c),
	 CU64(0xf726ffede89d6f8e),
};

// ============================================================================

static Crypto::CipherRegisterer< Crypto::Khazad > khazadRegisterer;

// ============================================================================

