#ifndef MATROSKA_UTILS_H
#define MATROSKA_UTILS_H

#include "ebml/StdIOCallback.h"
#include <ebml/EbmlHead.h>
#include <ebml/EbmlSubHead.h>
#include <ebml/EbmlVersion.h>
#include <ebml/EbmlVoid.h>
#include "ebml/EbmlMaster.h"
#include "ebml/EbmlUnicodeString.h"

#include <string>

#include <matroska/KaxTags.h>
#include <matroska/KaxTag.h>

#include "libmediaconverter.hh"

using namespace LIBEBML_NAMESPACE;
using namespace LIBMATROSKA_NAMESPACE;


using std::string;
using std::wstring;

#define ISREFID(e, ref) (e->Generic().GlobalId == ref::ClassInfos.GlobalId)

#define ISKAXSEGMENTID(e) ISREFID(e, KaxSegment)
#define ISKAXINFOID(e) ISREFID(e, KaxInfo)
#define ISKAXTRACKSID(e) ISREFID(e, KaxTracks)
#define ISKAXSEEKHEADID(e) ISREFID(e, KaxSeekHead)
#define ISKAXCLUSTERID(e) ISREFID(e, KaxCluster)
#define ISKAXCUESID(e) ISREFID(e, KaxCues)
#define ISKAXATTACHMENTID(e) ISREFID(e, KaxAttachments)
#define ISKAXTAGSID(e) ISREFID(e, KaxTags)
#define ISKAXCHAPTERSID(e) ISREFID(e, KaxChapters)



#define ISEBMLVOID(e) ISREFID(e, EbmlVoid)
#define ISEBMLCRC32(e) ISREFID(e, EbmlCrc32)

#define ISKAXTIMECODESCALEID(e) ISREFID(e, KaxTimecodeScale)
#define ISKAXDURATIONID(e) ISREFID(e, KaxDuration)
#define ISKAXMUXINGAPPID(e) ISREFID(e, KaxMuxingApp)
#define ISKAXWRITINGAPPID(e) ISREFID(e, KaxWritingApp)
#define ISKAXDATEUTCID(e) ISREFID(e, KaxDateUTC)
#define ISKAXSEGMENTUIDID(e) ISREFID(e, KaxSegmentUID)
#define ISKAXSEGMENTFAMILY(e) ISREFID(e, KaxSegmentFamily)
#define ISKAXCHAPTERTRANSLATEID(e) ISREFID(e, KaxChapterTranslate)
#define ISKAXPREVUIDID(e) ISREFID(e, KaxPrevUID)
#define ISKAXPREVFILENAMEID(e) ISREFID(e, KaxPrevFilename)
#define ISKAXNEXTUIDID(e) ISREFID(e, KaxNextUID)
#define ISKAXNEXTFILENAMEID(e) ISREFID(e, KaxNextFilename)
#define ISKAXSEGMENTFILENAMEID(e) ISREFID(e, KaxSegmentFilename)
#define ISKAXTITLEID(e) ISREFID(e, KaxTitle)

#define ISKAXCUETIME(e) ISREFID(e, KaxCueTime)
#define ISKAXCUECLUSTERPOSITION(e) ISREFID(e, KaxCueClusterPosition)
#define ISKAXCUETRACK(e) ISREFID(e, KaxCueTrack)
#define ISKAXCUETRACKPOSITION(e) ISREFID(e, KaxCueTrackPositions)


#define ISKAXTRACKENTRYID(e) ISREFID(e, KaxTrackEntry)
#define ISKAXTRACKNUMBERID(e) ISREFID(e, KaxTrackNumber)
#define ISKAXTRACKUIDID(e) ISREFID(e, KaxTrackUID)
#define ISKAXTRACKTYPEID(e) ISREFID(e, KaxTrackType)
#define ISKAXTRACKFLAGENABLEDID(e) ISREFID(e, KaxTrackFlagEnabled)
#define ISKAXTRACKNAMEID(e) ISREFID(e, KaxTrackName)
#define ISKAXTRACKCODECID(e) ISREFID(e, KaxCodecID)
#define ISKAXTRACKCODECNAMEID(e) ISREFID(e, KaxCodecName)
#define ISKAXTRACKCODECPRIVATEID(e) ISREFID(e, KaxCodecPrivate)


#define ISKAXSEEKPOSITION(e) ISREFID(e, KaxSeekPosition)
#define ISKAXSEEKID(e) ISREFID(e, KaxSeekID)
#define ISKAXSEEK(e) ISREFID(e, KaxSeek)
#define ISKAXCLUSTERTIMECODEID(e) ISREFID(e, KaxClusterTimecode)
#define ISKAXCLUSTERPOSITIONID(e) ISREFID(e, KaxClusterPosition)
#define ISKAXCLUSTERPREVSIZEID(e) ISREFID(e, KaxClusterPrevSize)
#define ISKAXCLUSTERSILENTTRACKSID(e) ISREFID(e, KaxClusterSilentTracks)
#define ISKAXBLOCKGROUPID(e) ISREFID(e, KaxBlockGroup)
#define ISKAXSIMPLEBLOCKID(e) ISREFID(e, KaxSimpleBlock)

#define ISKAXBLOCKID(e) ISREFID(e, KaxBlock)

#define ISKAXTAGID(e)               ISREFID(e, KaxTag)
#define ISKAXTAGTARGETSID(e)        ISREFID(e, KaxTagTargets)
#define ISKAXTARGETTYPEVALUEID(e)   ISREFID(e, KaxTagTargetTypeValue)
#define ISKAXTARGETTYPEID(e)        ISREFID(e, KaxTagTargetType)
#define ISKAXTAGTRACKUIDID(e)       ISREFID(e, KaxTagTrackUID)
#define ISKAXTAGEDITIONUIDID(e)     ISREFID(e, KaxTagEditionUID)
#define ISKAXTAGCHAPTERUIDID(e)     ISREFID(e, KaxTagChapterUID)
#define ISKAXTAGATTACHMENTUIDID(e)  ISREFID(e, KaxTagAttachmentUID)
#define ISKAXSIMPLETAGID(e)         ISREFID(e, KaxTagSimple)
#define ISKAXTAGNAMEID(e)           ISREFID(e, KaxTagName)
#define ISKAXTAGLANGUAGEID(e)       ISREFID(e, KaxTagLangue)
#define ISKAXTAGDEFAULTID(e)        ISREFID(e, KaxTagDefault)
#define ISKAXTAGSTRINGID(e)         ISREFID(e, KaxTagString)
#define ISKAXTAGBINARYID(e)         ISREFID(e, KaxTagBinary)


#define INPARENT(p, in) \
  (!p->IsFiniteSize() || \
   (in->getFilePointer() < \
    (p->GetElementPosition() + p->HeadSize() + p->GetSize())))

#define M_NANOSEC 1e9

#define IPVSTAG "IPVIDEOSYSTEMS_XML"

#define BLOB_METADATA "IPVS_BLOB"
#define INDEX_METADATA "IPVS_DB_INDEX"
#define MSFRL_METADATA "IPVS_DB_MSFRL"

#define INDEX_ELEMENT "index"
#define MSFRL_ELEMENT "MediaStreamFileResource"
#define MSFILE_ELEMENT "MediaStoreFile"
#define BOOKMARK_ELEMENT "Bookmark"

namespace mediaindex
{
    template<typename A, typename B> B &
        GetChildAs(EbmlMaster &m) {
            return GetChild<A>(m);
    }

    template<typename A, typename B> B &
        GetChildAs(EbmlMaster *m) {
            return GetChild<A>(*m);
    }

    inline bool IsGlobalElement(EbmlStream *a_pEs, EbmlElement *a_pEl, 
            int a_nLevel) {
        if (ISEBMLVOID(a_pEl)) {
            return true;

        } else if (ISEBMLCRC32(a_pEl)) {
            return true;
        }

        return false;
    }

    inline std::string GetUTF8StrFromEbmlUnicodeStr(
        EbmlUnicodeString& a_rEbmlUnicodeStr)
    {
        UTFstring utfStr = a_rEbmlUnicodeStr;
        return utfStr.GetUTF8();
    }

    void FixMandatoryTagElements(EbmlElement *a_pEbmlElement);

    inline KaxTagSimple& AddSimpleTag(EbmlMaster& a_rEbmlMaster, 
        const string& a_rStrName, 
        const string& a_rStrVal)
    {
        UTFstring strName, strVal;
        strName.SetUTF8(a_rStrName);
        strVal.SetUTF8(a_rStrVal);

        // TODO: Runtime check to verify that master is KaxTag or KaxTagSimple
        KaxTagSimple& rSimpleTag = AddNewChild<KaxTagSimple>(a_rEbmlMaster);
        KaxTagName &n = GetChild<KaxTagName>(rSimpleTag);
        *static_cast<EbmlUnicodeString *>(&n) = strName;
        KaxTagString &s = GetChild<KaxTagString>(rSimpleTag);
        *static_cast<EbmlUnicodeString *>(&s) = strVal;

        return rSimpleTag;
    }

    inline bool ChangeSimpleTagValue(KaxTagSimple& a_rSimpleTag, 
        const string& a_rStrVal)
    {
        UTFstring strVal;
        strVal.SetUTF8(a_rStrVal);

        KaxTagString* pVal = FindChild<KaxTagString>(a_rSimpleTag);
        if(pVal)
        {
            *static_cast<EbmlUnicodeString *>(pVal) = strVal;
            return true;
        }
        else
        {
            return false;
        }
    }

    inline mediaindex::nameval_pair GetNameValFromSimpleTag(KaxTagSimple& a_rSimpleTag)
    {
        KaxTagName* pName  = FindChild<KaxTagName>(a_rSimpleTag);
        KaxTagString* pVal = FindChild<KaxTagString>(a_rSimpleTag);

        UTFstring strName
            (*static_cast<EbmlUnicodeString *>(pName));
        UTFstring strVal
            (*static_cast<EbmlUnicodeString *>(pVal));

        return mediaindex::nameval_pair(strName.GetUTF8(), strVal.GetUTF8());
    }

    inline bool CheckIfIPVSTagType(KaxTag& a_rKaxTag)
    {
        KaxTagTargets* pTarget = FindChild<KaxTagTargets> (a_rKaxTag);
        if (pTarget) {
            KaxTagTargetType *pType = FindChild<KaxTagTargetType> (*pTarget);
            if (pType) {
                if (string(*dynamic_cast<EbmlString *> (pType)) == IPVSTAG)
                    return true;
            }
        }

        return false;
    }

}

#endif
