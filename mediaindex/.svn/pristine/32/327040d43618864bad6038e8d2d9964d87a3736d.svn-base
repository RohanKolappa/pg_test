#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "MediaIndexUtils.hh"

using namespace mediaindex;

int main(int argc, char *argv[]) {
    
    char * homedir;
    if(argc < 2) {
        printf("Usage: %s <blobdir>\n", argv[0]);
        exit(1);
    }
    homedir = argv[1];
    
    SourceIndexInterface* si = createSourceIndex(homedir, NULL);
    if(si == NULL) {
        printf("Failed creating Source Index for media in %s", homedir);
        return -1;
    }

    int status = si->start(0, 0);
    if(status == INDEX_ERROR) {
        char *desc = si->getLastError();
        printf("Error : %s\n", desc);
        delete si;
        return -1;
    }
    

    si->dumpGapInfo();

    return 0;
}

