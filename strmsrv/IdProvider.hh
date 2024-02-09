/*
 * IdProvider.hh
 *
 *  Created on: Apr 4, 2013
 *      Author: rkale
 */

#ifndef IDPROVIDER_HH_
#define IDPROVIDER_HH_

#include <list>

class CIdProvider {

public:
    CIdProvider(int a_startValue)
    {
        m_startValue = a_startValue;
    }
    ~CIdProvider()
    {

    }

    int GetId() {
        int proposedValue = m_startValue;
        while (InUse(proposedValue))
            proposedValue++;
        m_idList.push_back(proposedValue);
        return proposedValue;
    }

    bool InUse(int value) {
        std::list<int>::iterator iter;
        for (iter = m_idList.begin();
             iter != m_idList.end(); ++iter) {
            if (value == (*iter))
                return true;
        }
        return false;
    }

    void ReleaseId(int value)
    {
        // Just remove the element from our list if we find it
        std::list<int>::iterator iter;
        for (iter = m_idList.begin();
             iter != m_idList.end(); ++iter) {
            if (value == (*iter)) {
                m_idList.erase(iter);
                return;
            }
        }
    }

private:
    int m_startValue;
    std::list<int>  m_idList;
};

#endif /* IDPROVIDER_HH_ */
