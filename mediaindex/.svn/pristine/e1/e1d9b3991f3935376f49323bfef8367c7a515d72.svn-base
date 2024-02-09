/*
 * EditList.h
 *
 *  Created on: Nov 18, 2011
 *      Author: rkale
 */

#ifndef EDITLIST_H_
#define EDITLIST_H_

#include <string>
#include <list>

namespace mediaindex {

class CEditItem {
public:
    CEditItem(uint64_t a_nBegin, uint64_t a_nEnd) {
        m_nBegin = a_nBegin;
        m_nEnd = a_nEnd;
    }
    uint64_t GetBeginTime() {
        return m_nBegin;
    }
    uint64_t GetEndTime() {
        return m_nEnd;
    }
    void SetBeginTime(uint64_t a_nBegin) {
        m_nBegin = a_nBegin;
    }
    void SetEndTime(uint64_t a_nEnd) {
        m_nEnd = a_nEnd;
    }
    uint64_t GetLength() {
        if (m_nEnd < m_nBegin)
            return 0;
        return m_nEnd - m_nBegin;
    }
    bool IsAfter(CEditItem otherItem) {
        if (m_nBegin > otherItem.m_nEnd)
            return true;
        return false;
    }
    bool OverLaps(CEditItem otherItem) {
        if ((m_nBegin <= otherItem.m_nEnd) &&
                (m_nEnd >= otherItem.m_nBegin)) {
            return true;
        }
        return false;
    }
    void Merge(CEditItem otherItem) {
        if (m_nBegin > otherItem.m_nBegin)
            m_nBegin = otherItem.m_nBegin;
        if (m_nEnd < otherItem.m_nEnd)
            m_nEnd = otherItem.m_nEnd;
    }
private:
    uint64_t m_nBegin;
    uint64_t m_nEnd;
};

class CEditList {

public:
    CEditList();
    ~CEditList();
    bool Set(std::string aEditList);
    int GetItemCount();
    uint64_t StartOffset(int index);
    uint64_t EndOffset(int index);

private:
    std::list <CEditItem> m_lEditList;

    bool IsANumber(std::string aString);
    bool CheckItem(std::string aEditList);
    void AddItem(std::string from, std::string to);
    void ConsolidateItems();
    CEditItem GetItemAt(int index);
};

};

#endif /* EDITLIST_H_ */
