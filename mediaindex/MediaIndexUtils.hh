#ifndef MEDIAINDEXUTILS_HH
#define MEDIAINDEXUTILS_HH

#include "IndexInterface.hh"

namespace mediaindex {

SourceIndexInterface *createSourceIndex(char *mediaID, 
                                        char *homedir);

DestIndexInterface *createDestIndex(char *mediaID, 
                                    char *homedir, 
                                    uint64_t trimLength);

};

#endif
