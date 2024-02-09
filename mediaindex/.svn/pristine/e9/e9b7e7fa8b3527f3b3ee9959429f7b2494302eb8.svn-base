#ifndef _MEDIAPORTALUTILS_HH
#define _MEDIAPORTALUTILS_HH


#define BLOBDIR "blob"
#define INDEX "INDEX"

namespace mediaindex {

char *readFile(char *filename) ;
char *getIndexFile(char *mediaID, char *homedir) ;
char *getBlobFile(char *mediaID, char *homedir, int minutes) ;
int generateIndexData(char *mediaID, char *homedir,
                      uint64_t *pbeginTime,
                      uint64_t *pbeginTimeCode,
                      uint64_t *pendTimeCode);

};

#endif

