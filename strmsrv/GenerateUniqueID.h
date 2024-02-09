/*
 * CGenerateUniqueID.h
 *
 *  Created on: Mar 25, 2013
 *      Author: durga
 */

#ifndef CGENERATEUNIQUEID_H_
#define CGENERATEUNIQUEID_H_

class CGenerateUniqueID {
public:
	CGenerateUniqueID();
	virtual ~CGenerateUniqueID();

	static int GetID();
private:
	static int g_nUniqueID;
};

#endif /* CGENERATEUNIQUEID_H_ */
