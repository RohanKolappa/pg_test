#include "MediaIndexUtils.hh"
#include "ClipSourceIndex.hh"
#include "ClipDestIndex.hh"
#include "MediaPortalUtils.hh"

namespace mediaindex {

SourceIndexInterface *createSourceIndex(char *mediaID, char *homedir)
{

    SourceIndexInterface * pSourceIndex = new ClipSourceIndex();

    // If home directory is null, mediaID is assumed to be the full path to the
    // blob directory and the Index file is supposed to exist
    // The start() method will fail with appropriate error code
    pSourceIndex->init(mediaID, homedir);

    return pSourceIndex;
}

DestIndexInterface *createDestIndex(char *mediaID, char *homedir,
                                    uint64_t trimLength)
{
    // Only support clip for now
    DestIndexInterface * pDestIndex = new ClipDestIndex();
    if (pDestIndex->init(mediaID, homedir, trimLength) == INDEX_ERROR) {
        delete pDestIndex;
        return NULL;
    }
    return pDestIndex;
}

};
