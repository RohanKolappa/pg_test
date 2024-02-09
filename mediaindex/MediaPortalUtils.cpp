

#include "CommonIncludes.hh"
#include "MediaPortalUtils.hh"

namespace mediaindex {

char *readFile(char *filename)
{
    struct stat statbuf;
    char *buf = NULL;
    FILE *fp;

    fp = fopen(filename, "r");
    if (fp == NULL)
        return NULL;
#ifdef WINDOWS
    stat(filename, &statbuf);
    buf = (char *)malloc(statbuf.st_size + 1);
    fread(buf, statbuf.st_size, 1, fp);
    buf[statbuf.st_size] = '\0';
#else
    int fd = fileno(fp);
    if (flock(fd, LOCK_SH) == 0) { // acquire read lock on the file
        fstat(fd, &statbuf);
        buf = (char *)malloc(statbuf.st_size + 1);
        fread(buf, statbuf.st_size, 1, fp);
        flock(fd, LOCK_UN);
        buf[statbuf.st_size] = '\0';
    }
#endif
    fclose(fp);
    return buf;
}

int generateIndexData(char *mediaID, char *homedir,
                      uint64_t * pbeginTime,
                      uint64_t * pbeginTimeCode,
                      uint64_t * pendTimeCode)
{

    struct stat statbuf;
    int length = 0;

    while (1) {
        char *blobfile = getBlobFile(mediaID, homedir, length);
        int status = LSTAT(blobfile, &statbuf);
        if (status != 0) {
            free(blobfile);
            break;
        }
        if (length == 0) {
            uint64_t tc;
            char *cp;
            int fd = 0;
            cp = (char *) & tc;
            fd = OPEN(blobfile, O_RDONLY);
            if (fd > 0) {
                READ(fd, cp, 8);
            }
            CLOSE(fd);
            *pbeginTimeCode = tc;

        }
        length = length + 1;
        free(blobfile);
    }

    *pbeginTime = *pbeginTimeCode;
    *pendTimeCode = *pbeginTimeCode + length * 60 * 1000;

    return 1;
}

// If home directory is not null, file path is ${homedir}/blob/${mediaID}
// Else mediaID is assumed to be the full path to the media
// within the blob directory and the Index file is supposed to exist there.
char *getIndexFile(char *mediaID, char *homedir)
{
    // This function allocates memory that the client is supposed to free
    char *indexFile = NULL;
    if (homedir != NULL) {
        indexFile = (char *)malloc(strlen(homedir) + strlen(BLOBDIR) + strlen(mediaID) + strlen(INDEX) + 4);
        sprintf(indexFile, "%s" FILESEPERATOR "%s" FILESEPERATOR "%s" FILESEPERATOR "%s", homedir, BLOBDIR, mediaID, INDEX);
    }
    else {
        indexFile = (char *)malloc(strlen(mediaID) + strlen(INDEX) + 4);
        sprintf(indexFile, "%s" FILESEPERATOR "%s", mediaID, INDEX);
    }

    return indexFile;

}

char *getBlobFile(char *mediaID, char *homedir, int minutes)
{

    char *blobFile;
    if (homedir != NULL) {
        blobFile = (char *)malloc(strlen(homedir) + strlen(BLOBDIR) + strlen(mediaID) + 256);
        sprintf(blobFile, "%s" FILESEPERATOR "%s" FILESEPERATOR "%s" FILESEPERATOR "%d", homedir, BLOBDIR, mediaID, minutes);
    }
    else {
        blobFile = (char *)malloc(strlen(mediaID) + 256);
        sprintf(blobFile, "%s" FILESEPERATOR "%d", mediaID, minutes);
    }
    return blobFile;

}

}
