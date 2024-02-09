#include "CommonIncludes.hh"
#include <unistd.h>
#include <dirent.h>
#include <sys/time.h>

#include <string>
#include <list>

using namespace std;

typedef struct hdr
{
    unsigned long long h_nTimestamp;
    int		   h_nFlags;
    int		   h_nLen;
} hdr_t;

typedef unsigned long long timestamp_t;

static void usage(char *command) {
    printf("Usage: %s [OPTIONS]\n", command);
    printf("\nOptions:\n");
    printf("\t-f file\t\tMedia blob file to index\n");
    printf("\t-d directory\t\tMedia blob directory to index\n");
    printf("\t-h\t\tCreate index (otherwise check index)\n");
    printf("\t-h\t\tShow this help\n");
    exit(1);
}

static timestamp_t gettime() {
    struct timeval ts;
    gettimeofday(&ts, NULL);
    return (timestamp_t) (((unsigned long long) ts.tv_sec) * 1000 +
                                 (ts.tv_usec/1000));
}

// Main routine that indexes a given file
// Or just, confirms that the current indexing is correct
static int index_file(char *filename, bool check_index) {

    int buffersize = 750000;
    char buffer[buffersize+1];
    unsigned long readbuffersize = 0;
    unsigned long readoffset = 0;
    char indexfilename[256];
    FILE *fp;
    unsigned long fileoffset = 0;
    unsigned long lastfileoffset = 0;
    unsigned long nextfileoffset = 0;
    int index_count = 0;
    int read_new_index = 1;
    int check_for_next_ts = 0;

    timestamp_t index_timestamp = 0;

    int fd =  open(filename, O_RDONLY, 00666);
    if (fd <= 0)
        return -1;

    if (!check_index) {
        printf("Indexing file %s...\n", filename);
    }

    snprintf(indexfilename, 255, "%s.idx", filename);

    if (!check_index) {
        if ((fp = fopen(indexfilename, "w")) == NULL) {
            printf("Cannot create file %s\n", indexfilename);
            close (fd);
            return -1;
        }
    }
    else {
        if ((fp = fopen(indexfilename, "r")) == NULL) {
            printf("Cannot read file %s\n", indexfilename);
            close (fd);
            return -1;
        }
    }


    timestamp_t now = gettime();
    while (1) {
        // Read a chunk of data
        if (lseek (fd, (int)fileoffset, SEEK_SET) < 0) {
            break;
        }
        int length = read(fd,
                          buffer + readbuffersize,
                          buffersize - readbuffersize);
        if (length <= 0)
            break;

        //        printf("Chunk size = %d %lu\n", length, readbuffersize);
        readbuffersize += length;
        fileoffset += length;

        // process the chunk
        while (1) {
            char *data = buffer + readoffset;
            
            // Do we have a complete frame starting at current offset in the
            // current chunk?
            if (readoffset + sizeof(hdr_t) > readbuffersize)
                break; // too short to fit the header
            
            hdr_t * pHdr = (hdr_t *) data;
            if (readoffset + sizeof(hdr_t) + pHdr->h_nLen > readbuffersize)
                break; // too short to fit the data
            
            // Frame found

            if (check_index) {
                if (read_new_index == 1) {
                    char buf[256];
                    if (fgets(buf, 32, fp) == NULL) {
                        goto BREAK_OUT;
                    }
                    sscanf(buf, "%"SCNu64" %lu", &index_timestamp, &nextfileoffset);
                    //printf("Read new Index at %llu %lu\n", index_timestamp, nextfileoffset);
                    read_new_index = 0;
                    index_count++;
                    check_for_next_ts = 0;
                }
                if (pHdr->h_nTimestamp == index_timestamp) {
                    if (lastfileoffset + readoffset != nextfileoffset) {
                        check_for_next_ts = 1;  // potential match if next timestamp is the same
                    }
                    else {
                        //printf("Indexing at %llu %lu confirmed\n", index_timestamp, nextfileoffset);
                        read_new_index = 1;
                        check_for_next_ts = 0;
                    }
                }
                else if (check_for_next_ts) {
                    printf("Bad Index at %"PRIu64": Offset mismatch.\n", index_timestamp);
                    exit(1);
                }
                else if (pHdr->h_nTimestamp > index_timestamp) {
                    printf("Bad Index at %"PRIu64": Unexptected timestamp.\n", index_timestamp);
                    exit(1);
                }
            }
            else {
                if (index_timestamp == 0) {
                    // index first packet in every chunk
                    index_timestamp = pHdr->h_nTimestamp;
                    fprintf(fp, "%"PRIu64" %lu\n", index_timestamp, lastfileoffset);
                }
                if (pHdr->h_nTimestamp - index_timestamp > 1000) {
                    // index every second within a chunk so we break to
                    // to read another chunk if more than 1 sec within a chunk

                    break;
                }
            }
            // Adjust the current read offset
            readoffset += pHdr->h_nLen + sizeof(hdr_t);
        }
        
        // get ready for next chunk read
        int offset = readbuffersize - readoffset;
        readoffset = 0;
        readbuffersize = 0;
        fileoffset -= offset;

        if (!check_index)
            index_timestamp = 0;
        lastfileoffset = fileoffset;

    }
  BREAK_OUT:
    if (check_index) {
        if (index_count == 0 ) {
            printf("Index file %s OK [Empty]\n", indexfilename);
        }
        else {
            printf("Index file %s OK\n", indexfilename);
        }
    }
    else {
        printf ("Time taken = %"PRIu64"\n", gettime() - now);
    }
    close(fd);
    fclose(fp);
    return 0;
}

int main(int argc, char *argv[]) {
    
    DIR *dirp;
    FILE *fp;
    char filename[256];
    char *directory = NULL;
    char *singlefile = NULL;
    char ch;
    list<char *> file_list;
    struct dirent *direntp;
    struct stat statbuf;
    bool check_index = true;

    while( (ch=getopt(argc, argv, "d:f:Ch")) != -1) {
        switch(ch) {
        case 'd':
            directory = strdup(optarg);
            break;
        case 'f':
            singlefile = strdup(optarg);
            break;
        case 'C':
            check_index = false;
            break;
        case 'h':
        default:
            usage(argv[0]);
            break;
        }
    }
    
    // make sure either directory or file has been specified on
    // command line
    if (directory == NULL && singlefile == NULL) {
        printf("Need to specify directory (using -d) or file (using -f).\n");
        exit(1);
    }

    // If file is specified just index it and return
    if (singlefile != NULL) {
        if (stat(singlefile, &statbuf) != 0) {
            printf("File not found: %s\n", singlefile);
            exit (1);
        }
        if (!S_ISREG(statbuf.st_mode)) {
            printf("Not a regular file: %s\n", singlefile);
            exit(1);
        }

        if (index_file(singlefile, check_index) != 0)
            exit(1);
        exit (0);
    }

    // Directory is specified. open the directory
    if ((dirp = opendir(directory)) == NULL) {
        printf("Cannot open directory %s.\n", directory);
        exit(1);
    }

    // make sure INDEX file exists in this directory and media is not "live"
    snprintf(filename, 255, "%s/INDEX", directory);
    if ((fp = fopen(filename, "r")) == NULL) {
        printf("Did not find valid media (INDEX file missing).\n");
        exit(1);
    }
    unsigned long long ts;
    int busy;
    if (fscanf(fp, "%"SCNu64"-%d\n", &ts, &busy) != 2) {
        printf("Did not find valid media (INDEX file corrupted).\n");
        exit(1);
    }
    if (busy == 1) {
        printf("Aborting since media seems to be busy.\n");
        exit(1);
    }
    fclose(fp);

    // Create list of files to index
    while ((direntp = readdir(dirp)) != NULL) {

        if (strcmp(direntp->d_name, "INDEX") == 0)
            continue;
        if (strstr(direntp->d_name, ".idx") != NULL)
            continue;
        if (strstr(direntp->d_name, ".xml") != NULL)
            continue;
        snprintf(filename, 255, "%s/%s", directory, direntp->d_name);
        if (stat(filename, &statbuf) != 0)
            continue;
        if (!S_ISREG(statbuf.st_mode)) // skip directories
            continue;

        file_list.push_back(strdup(filename));
    }
    closedir(dirp);

    // Index all files in the list
    while (!file_list.empty()) {
        singlefile = file_list.front();
        file_list.pop_front();
        index_file(singlefile, check_index);
    }

    free(directory);
    free(singlefile);
    return 0;
}

