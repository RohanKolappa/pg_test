#include "ebml/StdIOCallback.h"
#include <ebml/EbmlHead.h>
#include <ebml/EbmlSubHead.h>
#include <ebml/EbmlVersion.h>
#include <ebml/EbmlVoid.h>

#include <matroska/FileKax.h>
#include <matroska/KaxAttached.h>
#include <matroska/KaxAttachments.h>
#include <matroska/KaxBlock.h>
#include <matroska/KaxChapters.h>
#include <matroska/KaxCluster.h>
#include <matroska/KaxClusterData.h>
#include <matroska/KaxCues.h>
#include <matroska/KaxCuesData.h>
#include <matroska/KaxInfo.h>
#include <matroska/KaxInfoData.h>
#include <matroska/KaxSeekHead.h>
#include <matroska/KaxSegment.h>
#include <matroska/KaxTags.h>
#include <matroska/KaxTag.h>
#include <matroska/KaxTracks.h>
#include <matroska/KaxTrackEntryData.h>
#include <matroska/KaxTrackAudio.h>
#include <matroska/KaxTrackVideo.h>
#include <matroska/KaxVersion.h>

using namespace LIBEBML_NAMESPACE;
using namespace LIBMATROSKA_NAMESPACE;

#include "MatroskaUtils.hh"

namespace mediaindex
{

    void FixMandatoryTagElements(EbmlElement *a_pEbmlElement) {
        if (dynamic_cast<KaxTag *>(a_pEbmlElement) != NULL) {
            KaxTag &t = *static_cast<KaxTag *>(a_pEbmlElement);
            GetChild<KaxTagTargets>(t);
            GetChild<KaxTagSimple>(t);

        } else if (dynamic_cast<KaxTagSimple *>(a_pEbmlElement) != NULL) {
            KaxTagSimple &s                       = *static_cast<KaxTagSimple *>(a_pEbmlElement);
            KaxTagName &n                         = GetChild<KaxTagName>(s);
            *static_cast<EbmlUnicodeString *>(&n) = UTFstring(n);
            KaxTagLangue &l                       = GetChild<KaxTagLangue>(s);
            *static_cast<EbmlString *>(&l)        = std::string(l);
            KaxTagDefault &d                      = GetChild<KaxTagDefault>(s);
            *static_cast<EbmlUInteger *>(&d)      = uint64(d);

        } else if (dynamic_cast<KaxTagTargets *>(a_pEbmlElement) != NULL) {
            KaxTagTargets &t = *static_cast<KaxTagTargets *>(a_pEbmlElement);
            GetChild<KaxTagTargetTypeValue>(t);
            KaxTagTargetTypeValue &v         = GetChild<KaxTagTargetTypeValue>(t);
            *static_cast<EbmlUInteger *>(&v) = uint64(v);

        }

        if (dynamic_cast<EbmlMaster *>(a_pEbmlElement) != NULL) {

            EbmlMaster *m = static_cast<EbmlMaster *>(a_pEbmlElement);
            for (size_t i = 0; i < m->ListSize(); i++)
                FixMandatoryTagElements((*m)[i]);
        }
    }

}

