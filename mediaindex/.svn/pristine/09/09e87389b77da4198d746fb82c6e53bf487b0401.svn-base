/*
 * EditList.cpp
 *
 *  Created on: Nov 18, 2011
 *      Author: rkale
 */


#include "CommonIncludes.hh"
#include "EditList.hh"


namespace mediaindex {

CEditList::CEditList()
{
}

CEditList::~CEditList()
{
}

bool CEditList::Set(std::string aEditList)
{
    m_lEditList.clear();

//    printf("Testing %s\n", aEditList.c_str());
    std::string::size_type pos = 0;
    while (true) {
        std::string::size_type commapos = aEditList.find(',', pos);
        if (commapos == std::string::npos) {
            if (!CheckItem(aEditList.substr(pos)))
                return false;
            break;
        }

        std::string item = aEditList.substr(pos, commapos - pos);
        if (!CheckItem(item))
            return false;
        pos += (commapos - pos) + 1;
        if (pos == aEditList.length())
            break;
    }

    //printf("Item Count %d\n", m_nItemCount);
    if (GetItemCount() == 0)
        return false;
    return true;
}

bool CEditList::CheckItem(std::string aEditList) {
//    printf("Checking %s\n", aEditList.c_str());

    if (aEditList.length() == 0)
        return false;

    std::string::size_type pos;
    pos = aEditList.find(':', 0);
    if (pos == std::string::npos)
        return false;

    if (pos == std::string::npos)
        return false;

    std::string from = aEditList.substr(0, pos);
    if (!IsANumber(from))
        return false;

    std::string::size_type endpos = aEditList.find(',', pos + 1);
    std::string to = aEditList.substr(pos + 1, endpos - (pos + 1));

    if (!IsANumber(to))
        return false;

    AddItem(from, to);
    return true;
}

bool CEditList::IsANumber(std::string aString)
{
    if (aString.length() == 0)
        return false;

    if (aString.find_first_not_of("0123456789") != std::string::npos)
        return false;
    return true;

}

void CEditList::AddItem(std::string from, std::string to)
{
    uint64_t fromVal = STRTOULL(from.c_str(), NULL, 10);
    uint64_t toVal = STRTOULL(to.c_str(), NULL, 10);
    if (toVal <= fromVal) {
        return;
    }

    CEditItem newItem(fromVal, toVal);
    std::list<CEditItem> :: iterator iter;
    for (iter = m_lEditList.begin(); iter != m_lEditList.end(); ++iter) {
        CEditItem item = (*iter);
        if (item.IsAfter(newItem)) {
            m_lEditList.insert(iter, newItem);
            return;
        }
        if (item.OverLaps(newItem)) {
            iter->Merge(newItem);
            ConsolidateItems();
            return;
        }
    }

    m_lEditList.push_back(newItem);
}

void CEditList::ConsolidateItems()
{
    // Travese the list merging two consecutive items if need be
    while (true) {
        bool mergedAnItem = false;
        std::list<CEditItem> :: iterator iter;
        for (iter = m_lEditList.begin(); iter != m_lEditList.end(); ) {
            std::list<CEditItem> :: iterator prevIter = iter;
            ++iter;
            if (iter == m_lEditList.end())
                break;
            if (iter->OverLaps(*prevIter)) {
                prevIter->Merge(*iter);
                m_lEditList.erase(iter);
                mergedAnItem = true;
                break;
            }
        }
        if (!mergedAnItem)
            break;
    }
}

int CEditList::GetItemCount()
{
    return m_lEditList.size();
}

uint64_t CEditList::StartOffset(int index) {
    return GetItemAt(index).GetBeginTime();
}

uint64_t CEditList::EndOffset(int index) {
    return GetItemAt(index).GetEndTime();
}

CEditItem CEditList::GetItemAt(int index) {
    if (index < 0 || index >= (int)m_lEditList.size())
        return CEditItem(0,0);
    std::list<CEditItem> :: iterator iter;
    int currentItem = 0;
    for (iter = m_lEditList.begin(); iter != m_lEditList.end(); ++iter) {
        if (currentItem++ == index) {
            return (*iter);
        }
    }
    return CEditItem(0,0);
}

};
