/*
 * CGenerateUniqueID.cpp
 *
 *  Created on: Mar 25, 2013
 *      Author: durga
 */

#include "GenerateUniqueID.h"

int CGenerateUniqueID::g_nUniqueID = 0;


CGenerateUniqueID::CGenerateUniqueID() {
}

CGenerateUniqueID::~CGenerateUniqueID() {
}

int CGenerateUniqueID::GetID() {
    return ++g_nUniqueID;
}
