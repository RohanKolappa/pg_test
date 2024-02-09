#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

#include <sys/file.h>

#include "configfile_parser.h"
#include "v2d.h"
#include "vlog.h"
#include "commonutils.h"
#include "text_utils.h"
#include "custmenu.h"

#define MAX_NAME_LEN      256
#define MAX_VALUE_LEN     8192
#define MAX_LINE_LEN      (MAX_NAME_LEN + MAX_VALUE_LEN + 128)

/* Make sure config file exists. Create an empty file if needed. */
static void check_config_file(const char *filename)
{
    struct stat statbuf;
    FILE *fp;

    if (stat(filename, &statbuf) != 0) {
        if ((fp = fopen(filename, "w")) == NULL) {
            VLOG_WARNING_CAT("config","Unable to create file %s", filename);
            return;
        }
        fclose(fp);
    }
}

int parse_config_file(const char *filename, int(*nv_handler)(char *, char *,
        void *ptr), void *ptr)
{
    char linbuf[300], savebuf[300], *c, *cp, *name, *value;
    FILE *fp;
    int count = 0;

    /********************************
     * Open config file for reading *
     ********************************/
    if (filename == NULL) {
        VLOG_WARNING_CAT("config","parse_config_file: filename = NULL!");
        return -1;
    }

    check_config_file(filename);

    if ((fp = fopen(filename, "r")) == NULL) {
        VLOG_WARNING_CAT("config","Unable to open config file %s", filename);
        return -1;
    }

    /****************************************
     * Read file a line at a time and parse *
     ****************************************/
    do {
        c = fgets(linbuf, 256, fp);

        /*****************************
         * Throw away comment, etc.  *
         *****************************/
        if (c == NULL || *c == '#')
            continue;

        if (strlen(c) <= 3) {
            continue;
        }

        strcpy(savebuf, linbuf);

        cp = c;
        cp = trim_string(cp);

        name = cp;

        if (strlen(name) <= 1) { /* whole line */
            VLOG_WARNING_CAT("config","Parse error at line: %s", savebuf);
            continue;
        }

        while (*cp != '\0' && *cp != ' ' && *cp != '\t')
            cp++;

        if (*cp == '\0') {
            VLOG_WARNING_CAT("config","Missing value at line: %s", savebuf);
            continue;
        }
        *cp = '\0';
        cp++;

        if (strlen(name) <= 1) { /* just the name */
            VLOG_WARNING_CAT("config","Bad parameter \"%s\" ", name);
            continue;
        }

        while (*cp == ' ' || *cp == '\t')
            cp++;

        value = cp;

        if (strlen(value) < 1) {
            VLOG_WARNING_CAT("config","Parse error at line: %s", savebuf);
            continue;
        }

        if ((count = nv_handler(name, value, ptr)) < 0) {
            VLOG_DEBUG_CAT("config","Unknown parameter: Name = %s, Value = %s", name,
                    value);
        }

    } while (c != NULL);

    fclose(fp);

    return count;
}

/* This version is used by auto generated code and allows empty
 * strings to be parameter values. The above version should be
 * deprecated once all code is converted */
int parse_configuration_file(const char *filename, int(*nv_handler)(char *,
        char *, void *ptr), void *ptr)
{
    char * linbuf = NULL;
    char * savebuf = NULL;

    char *c, *cp, *name, *value;
    FILE *fp;
    int count = 0;

    /********************************
     * Open config file for reading *
     ********************************/
    if (filename == NULL) {
        VLOG_WARNING_CAT("config","parse_config_file: filename = NULL!");
        return -1;
    }

    check_config_file(filename);

    if ((fp = fopen(filename, "r")) == NULL) {
        VLOG_WARNING_CAT("config","Unable to open config file %s", filename);
        return -1;
    }

    linbuf = malloc(MAX_LINE_LEN + 1);
    savebuf = malloc(MAX_LINE_LEN + 1);

    if (linbuf == NULL || savebuf == NULL) {
        if (linbuf != NULL) free(linbuf);
        if (savebuf != NULL) free(savebuf);
        return -1;
    }

    /****************************************
     * Read file a line at a time and parse *
     ****************************************/
    do {
        c = fgets(linbuf, MAX_LINE_LEN, fp);

        /*****************************
         * Throw away comment, etc.  *
         *****************************/
        if (c == NULL || *c == '#')
            continue;

        if (strlen(c) <= 3) {
            continue;
        }

        strcpy(savebuf, linbuf);

        cp = c;
        cp = trim_string(cp);

        name = cp;

        if (strlen(name) <= 1) { /* whole line */
            VLOG_WARNING_CAT("config","Parse error at line: %s", savebuf);
            continue;
        }

        while (*cp != '\0' && *cp != ' ' && *cp != '\t')
            cp++;

        value = NULL;
        if (*cp == '\0') {
            value = ""; // Missing value, plugin blank
        }
        else {
            *cp = '\0';
            cp++;
        }

        if (strlen(name) <= 1) { /* just the name */
            VLOG_WARNING_CAT("config","Bad parameter \"%s\" ", name);
            continue;
        }

        if (value == NULL) {
            while (*cp == ' ' || *cp == '\t')
                cp++;

            value = cp;

            if (strlen(value) < 1) {
                VLOG_WARNING_CAT("config","Parse error at line: %s", savebuf);
                continue;
            }
        }

        if ((count = nv_handler(name, value, ptr)) < 0) {
            VLOG_DEBUG_CAT("config","Unknown parameter: Name = %s, Value = %s", name,
                    value);
        }

    } while (c != NULL);

    fclose(fp);

    free(linbuf);
    free(savebuf);

    return count;
}

int rewrite_config_file(const char *filename, int(*nv_handler)(char *, char *,
        void *, char), void *ptr)
{

    char linbuf[300], savebuf[300], *c, *cp, *name, *value;
    char namebuf[256], valuebuf[257];
    FILE *fp, *tp;
    int len, rc, wrote_nv = 0;
    char tmpfilename[64];
    int td;

    /********************************
     * Open config file for writing *
     ********************************/
    if (filename == NULL)
        return -1;

    check_config_file(filename);

    if ((fp = fopen(filename, "r")) == NULL) {
        VLOG_WARNING_CAT("config","Unable to open config file %s for writing.",
                filename);
        return -1;
    }

    sprintf(tmpfilename, "/V2O/config/tmpfile_XXXXXX");
    td = mkstemp(tmpfilename);
    if (td < 0) {
        VLOG_WARNING_CAT("config","Unable to open TEMP file for writing.");
        return -1;
    }

    tp = fdopen(td, "w");
    if ((tp = fdopen(td, "w")) == NULL) {
        VLOG_WARNING_CAT("config","Unable to get stream for TEMP file.");
        close(td);
        return -1;
    }


    /*******************************************************
     * Write parameters till call back function returns -1 *
     *******************************************************/
    do {

        c = fgets(linbuf, 256, fp);

        /*****************************
         * Throw away comment, etc.  *
         *****************************/
        if (c == NULL)
            continue;

        if (*c == '#' || strlen(c) <= 3) {
            fprintf(tp, "%s", savebuf);
            continue;
        }

        strcpy(savebuf, linbuf);

        cp = c;
        cp = trim_string(cp);

        name = cp;

        if (strlen(name) <= 1) { /* whole line */
            VLOG_WARNING_CAT("config","Parse error at line: %s", savebuf);
            continue;
        }

        while (*cp != '\0' && *cp != ' ' && *cp != '\t')
            cp++;

        if (*cp == '\0') {
            VLOG_WARNING_CAT("config","Missing value at line: %s", savebuf);
            continue;
        }
        *cp = '\0';
        cp++;

        if (strlen(name) <= 1) { /* just the name */
            VLOG_WARNING_CAT("config","Bad parameter \"%s\" ", name);
            continue;
        }

        while (*cp == ' ' || *cp == '\t')
            cp++;

        value = cp;

        if (strlen(value) < 1) {
            VLOG_WARNING_CAT("config","Parse error at line: %s", savebuf);
            continue;
        }

        strncpy(namebuf, name, 128);
        strncpy(valuebuf, value, 128);

        if ((rc = nv_handler(namebuf, valuebuf, ptr, 1)) < 0) {
            VLOG_DEBUG_CAT("config"," Unknown parameter: Name = %s, Value = %s", name,
                    value);
        }
        else {
            len = fprintf(tp, "%s", namebuf);
            fprintf(tp, "%*s %s\n", 25 - len, " ", valuebuf);
            if (rc == 1)
                wrote_nv = 1;
        }

    } while (c != NULL);

    if (!wrote_nv) {

        (void) nv_handler(namebuf, valuebuf, ptr, 0);

        len = fprintf(tp, "%s", namebuf);
        fprintf(tp, "%*s %s\n", 25 - len, " ", valuebuf);
    }

    fclose(fp);
    fclose(tp);

    if (rename(tmpfilename, filename) < 0)
        VLOG_WARNING_CAT("config","Could not rename %s to %s: %s",
                tmpfilename, filename, strerror(errno));

    return 0;
}


/* This version is used by auto generated code and allows empty
 * strings to be written as parameter values. The above version should
 * be deprecated once all code is converted */
int rewrite_configuration_file(const char *filename, int(*nv_handler)(char *,
        char *, void *, char), void *ptr)
{

    char * linbuf = NULL;
    char * savebuf = NULL;
    char * namebuf = NULL;
    char * valuebuf = NULL;
    char  *c, *cp, *name, *value;
    FILE *fp, *tp;
    int len, rc, wrote_nv = 0;
    char tmpfilename[64];
    int td;

    /********************************
     * Open config file for writing *
     ********************************/
    if (filename == NULL)
        return -1;

    check_config_file(filename);

    if ((fp = fopen(filename, "r")) == NULL) {
        VLOG_WARNING_CAT("config","Unable to open config file %s for writing.",
                filename);
        return -1;
    }

    sprintf(tmpfilename, "/V2O/config/tmpfile_XXXXXX");
    td = mkstemp(tmpfilename);
    if (td < 0) {
        VLOG_WARNING_CAT("config","Unable to open TEMP file for writing.");
        return -1;
    }

    tp = fdopen(td, "w");
    if ((tp = fdopen(td, "w")) == NULL) {
        VLOG_WARNING_CAT("config","Unable to get stream for TEMP file.");
        close(td);
        return -1;
    }

    linbuf = malloc(MAX_LINE_LEN + 1);
    savebuf = malloc(MAX_LINE_LEN + 1);
    namebuf = malloc(MAX_NAME_LEN + 1);
    valuebuf = malloc(MAX_VALUE_LEN + 1);

    if (linbuf == NULL || savebuf == NULL || namebuf == NULL || valuebuf == NULL) {
        if (linbuf != NULL) free(linbuf);
        if (savebuf != NULL) free(savebuf);
        if (namebuf != NULL) free(namebuf);
        if (valuebuf != NULL) free(valuebuf);
        return -1;
    }



    /*******************************************************
     * Write parameters till call back function returns -1 *
     *******************************************************/
    do {

        c = fgets(linbuf, MAX_LINE_LEN, fp);

        /*****************************
         * Throw away comment, etc.  *
         *****************************/
        if (c == NULL)
            continue;

        if (*c == '#' || strlen(c) <= 3) {
            fprintf(tp, "%s", linbuf);
            continue;
        }

        strcpy(savebuf, linbuf);

        cp = c;
        cp = trim_string(cp);

        name = cp;

        if (strlen(name) <= 1) { /* whole line */
            VLOG_WARNING_CAT("config","Parse error at line: %s", savebuf);
            continue;
        }

        while (*cp != '\0' && *cp != ' ' && *cp != '\t')
            cp++;

        value = NULL;
        if (*cp == '\0') {
            value = ""; // Missing value, plugin empty string
        }
        else {
            *cp = '\0';
            cp++;
        }

        len = strlen(name);
        if (len <= 1) { /* just the name */
            VLOG_WARNING_CAT("config","Bad parameter \"%s\" ", name);
            continue;
        }

        if (len > MAX_NAME_LEN ) { /* name to long? */
            VLOG_WARNING_CAT("config","Bad parameter \"%s\" too long", name);
            continue;
        }

        if (value == NULL) {
            while (*cp == ' ' || *cp == '\t')
                cp++;

            value = cp;
            len = strlen(value);

            if (len < 1) {
                VLOG_WARNING_CAT("config","Parse error at line: %s", savebuf);
                continue;
            }
            if (len > MAX_VALUE_LEN) {
                VLOG_WARNING_CAT("config","Parse error at line: %s", savebuf);
                continue;
            }

        }

        strcpy(namebuf, name);
        strcpy(valuebuf, value);

        if ((rc = nv_handler(namebuf, valuebuf, ptr, 1)) < 0) {
            VLOG_DEBUG_CAT("config"," Unknown parameter: Name = %s, Value = %s", name,
                    value);
        }
        else {
            len = fprintf(tp, "%s", namebuf);
            fprintf(tp, "%*s %s\n", 25 - len, " ", valuebuf);
            if (rc == 1)
                wrote_nv = 1;
        }

    } while (c != NULL);

    if (!wrote_nv) {

        (void) nv_handler(namebuf, valuebuf, ptr, 0);

        len = fprintf(tp, "%s", namebuf);
        fprintf(tp, "%*s %s\n", 25 - len, " ", valuebuf);
    }

    fclose(fp);
    fclose(tp);

    if (rename(tmpfilename, filename) < 0)
        VLOG_WARNING_CAT("config","Could not rename %s to %s: %s",
                tmpfilename, filename, strerror(errno));

    free(linbuf);
    free(savebuf);
    free(namebuf);
    free(valuebuf);

    return 0;
}

int save_config_file(const char *filename, int(*nv_handler)(char *, char *,
        void *ptr), void *ptr)
{
    char name[256], value[257];
    FILE *fp;
    int len;

    /********************************
     * Open config file for writing *
     ********************************/
    if (filename == NULL)
        return -1;

    if ((fp = fopen(filename, "w+")) == NULL) {
        VLOG_WARNING_CAT("config","Unable to open config file %s for writing.",
                filename);
        return -1;
    }

    /*******************************************************
     * Write parameters till call back function returns -1 *
     *******************************************************/
    while (1) {

        if (nv_handler(name, value, ptr) < 0) {
            break;
        }

        len = fprintf(fp, "%s", name);
        fprintf(fp, "%*s %s\n", 25 - len, " ", value);

    };

    fclose(fp);

    return 0;
}

int parse_list_config_file(const char *filename, int(*nv_handler)(char *,
        char *, void *ptr, int index), void *ptr)
{

    char * linbuf = NULL;
    char * savebuf = NULL;

    char *c, *cp, *name = NULL, *value = NULL;
    FILE *fp;
    int count = 0, idx;


    /********************************
     * Open config file for reading *
     ********************************/
    if (filename == NULL) {
        return -1;
    }

    check_config_file(filename);

    if ((fp = fopen(filename, "r")) == NULL) {
        VLOG_WARNING_CAT("config","Unable to open config file %s", filename);
        return -1;
    }

    linbuf = malloc(MAX_LINE_LEN + 1);
    savebuf = malloc(MAX_LINE_LEN + 1);

    if (linbuf == NULL || savebuf == NULL) {
        if (linbuf != NULL) free(linbuf);
        if (savebuf != NULL) free(savebuf);
        return -1;
    }

    /****************************************
     * Read file a line at a time and parse *
     ****************************************/
    do {
        c = fgets(linbuf, 256, fp);

        /*****************************
         * Throw away comment, etc.  *
         *****************************/
        if (c == NULL || *c == '#')
            continue;

        if (strlen(c) <= 3) {
            continue;
        }

        strcpy(savebuf, linbuf);

        cp = c;
        cp = trim_string(cp);

        name = cp;

        if (strlen(name) <= 1) { /* whole line */
            VLOG_WARNING_CAT("config","Parse error at line: %s", savebuf);
            continue;
        }

        while (*cp != '\0' && *cp != ' ' && *cp != '\t')
            cp++;

        if (*cp == '\0') {
            VLOG_WARNING_CAT("config","Missing value at line:: %s", savebuf);
            continue;
        }
        *cp = '\0';
        cp++;

        if (strlen(name) <= 1) { /* just the name */
            VLOG_WARNING_CAT("config","Bad parameter \"%s\" ", name);
            continue;
        }

        while (*cp == ' ' || *cp == '\t')
            cp++;

        value = cp;

        if (strlen(value) < 1) {
            VLOG_WARNING_CAT("config","Parse error at line: %s", savebuf);
            continue;
        }

        cp = name;
        while (*cp != '.' && *cp != '\0')
            cp++;
        if (*cp == '.') {
            *cp = '\0';
            cp++;
            idx = atoi(cp);
        }
        else {
            VLOG_WARNING_CAT("config","Parse error at line %s", savebuf);
            continue;
        }

        if (nv_handler(name, value, ptr, idx) < 0) {
            VLOG_DEBUG_CAT("config","Unknown parameter: Name = %s, Value = %s", name,
                    value);
        }

    } while (c != NULL);

    if (name != NULL && value != NULL)
        nv_handler(name, value, ptr, 0); /* Call this one more time to signify end of file */

    fclose(fp);

    free(linbuf);
    free(savebuf);

    return count;
}

// global file descriptor used for acquiring and releasing lock.
static int config_lock_fd = -1;

/* This function is used to acquire a lock for any config changes.
 * CONFIG_LOCK_FILE is the file that is used to acquire and release a lock. 
 */
int acquire_config_lock() {

    if(config_lock_fd >= 0) {
        VLOG_ERROR_CAT("config.lock","Attempting to acquire lock twice.");
        return RC_ERROR;
    }

    //Open or create CONFIG_LOCK_FILE
    config_lock_fd = open(CONFIG_LOCK_FILE, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    
    if (config_lock_fd == -1) {
        VLOG_ERROR_CAT("config.lock","Open failed on %s.",CONFIG_LOCK_FILE);
        return RC_ERROR;
    }

    //flock acquires a exclusive non blocking lock on CONFIG_LOCK_FILE.
    //Usually u fail to acquire a lock on this if lock already exist.
    if (flock(config_lock_fd, LOCK_EX | LOCK_NB) == -1) {
        VLOG_DEBUG_CAT("config.lock","Lock acquire failed.");
        close(config_lock_fd);
        config_lock_fd = -1;
        return RC_LOCKED_OUT;
    }

    // We also check for existence of REBOOT_LOCK_FILE for commands other than
    // config file changes. Basically commands that run in the background. Like
    // reboot, upgrade & shutdown. For example if reboot is running in 
    // we don't let any config changes.
    struct stat statbuf;
    if (stat(REBOOT_LOCK_FILE, &statbuf) == 0) {
        if ((time(NULL) - statbuf.st_mtime) < 300) { // only if lock file is not too old
            VLOG_DEBUG_CAT("config.lock", "System busy with other command.");
            release_config_lock();
            return RC_SYSTEM_BUSY;
        }
        else {
            unlink(REBOOT_LOCK_FILE);
        }
    }

    //At this point we successfully acquired the lock. 
    VLOG_DEBUG_CAT("config.lock","Acquired lock.");
    return RC_NORMAL;
}

/* Release the above acquired lock on CONFIG_LOCK_FILE once config changes 
 * are done. Note that a release has to be called for every acquire. if not 
 * the system would lock out any config changes. 
 */
int release_config_lock() {
    if (config_lock_fd > 0) {
        //Remove lock on CONFIG_LOCK_FILE.
        flock(config_lock_fd, LOCK_UN);
    
        VLOG_DEBUG_CAT("config.lock","Release lock.");

        //Close the global file descriptor so that we dont have a fd leak. 
        close(config_lock_fd);

        //Reset fd to -1 to acquire lock again.
        config_lock_fd = -1;
    }
    return RC_NORMAL;
}
