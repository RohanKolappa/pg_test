/*
 * CDataDropper.cpp
 *
 *  Created on: Apr 10, 2014
 *      Author: durga
 */

#include "DataDropper.hh"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#define MAX_NUM 1000000

CDataDropper::CDataDropper(double a_dPercentage, int a_nDropsRepeat,
                           int a_nRandomSeed)
{
    m_dPercentage = a_dPercentage;
    m_nDropsRepeat = a_nDropsRepeat;
    m_nRandomNumber = 0;
    m_nDropCount = 0;
    m_nTotalDropCount = 0;
    m_bDoDrop = false;

    SetRandomSeed(a_nRandomSeed);
}

CDataDropper::~CDataDropper()
{

}

void CDataDropper::SetRandomSeed(int a_nRandomSeed)
{
    if (a_nRandomSeed == 0)
        srand(time(NULL));
    else
        srand(a_nRandomSeed);
}

bool CDataDropper::ShouldDrop()
{
    if (m_bDoDrop == true)
    {
        if (m_nDropCount < m_nDropsRepeat)
        {
            m_nTotalDropCount++;
            m_nDropCount++;

            return m_bDoDrop;
        }

        m_nDropCount = 0;
        m_bDoDrop = false;
    }

    m_nRandomNumber = (rand() % MAX_NUM) + 1;

    if (m_nRandomNumber < MAX_NUM * (m_dPercentage / 100))
    {
        m_nTotalDropCount++;
        m_nDropCount++;

        m_bDoDrop = true;
    }

    return m_bDoDrop;
}

uint64_t CDataDropper::GetDropCount()
{
    return m_nTotalDropCount;
}
