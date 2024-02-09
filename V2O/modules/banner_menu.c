#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <crypt.h>

#define CUSTOM_MENU_DEFS

#include "custmenu.h"
#include "menuutils.h"
#include "commonutils.h"
#include "system_utils.h"
#include "banner_menu_gen.h"
#include "vlog.h"
#include "urlutils.h"
#include "vutils.h"
#include "text_utils.h"

#define MAX_BANNER_LEN   2500

static int isAsciiString(char * str) {
    char *pstr = str;
    while (*pstr) {
        if (!isascii(*pstr))
            return 0;
        pstr++;
    }
    return 1;
}

static int validateBanner(struct banner_login_params *params,
                          struct banner_login_params *orig_params,
                          char *error_string) 
{
    // Ensure decoded banner string length is within limits
    char *banner_str = url_decode(params->banner_string_value);
    if (strlen(banner_str) > MAX_BANNER_LEN) {
        sprintf(error_string, "Banner string exceeds limit of %d characters.", 
            MAX_BANNER_LEN);
        free(banner_str);
        return RC_INVALID_VALUE;
    }

    // Enusre only ASCII characters configured for banner
    if (!isAsciiString(banner_str)) {
        sprintf(error_string, "Only ASCII characters supported for banner.");
        free(banner_str);
        return RC_INVALID_VALUE;
    }

    free(banner_str);
    return RC_NORMAL;

}

int applyBanner(struct banner_login_params *params,
                       struct banner_login_params *orig_params)
{
    FILE *fp;

    // The banner string is url encoded. Decode the string first.
    char *banner_str = url_decode(params->banner_string_value);

    // Already validated but check decoded banner string 
    // length is within limits anyway
    if (strlen(banner_str) > MAX_BANNER_LEN) {
        free(banner_str);
        return RC_ERROR;
    }

    // Write decoded banner string to file
    fp = fopen("/etc/issue.net", "w");
    if (fp == NULL) {
        free(banner_str);
        return RC_ERROR ;
    } 
    
    fwrite(banner_str, 1, strlen(banner_str), fp);
    
    fclose(fp);
    
    free(banner_str);

    return RC_NORMAL;
}

void BannerInit(void)
{
    registerBannerValidateFunction(validateBanner);
}


// This configuration is only exposed via CLI (not shown in menu)
// Implement wrappers around the standard auto generated cli commands
int configure_banner_login_main(int argc, char **argv)
{
    char strbuffer[MAX_BANNER_LEN + 1];
    char bannerbuf[MAX_BANNER_LEN + 1];
    char printbuffer[MAX_BANNER_LEN * 3 + 128];
    char *line;
    int banner_too_long = 0;
    int invalid_banner = 0;

    printf("Type in new banner string.\n");
    printf("Use CTRL-D when done (or CTRL-C to abort).\n");


    // Read standard in one line at a time
    strcpy(bannerbuf, "");
    while (true) {
        line = fgets(strbuffer, MAX_BANNER_LEN, stdin);
        if (line == NULL) 
            break; // EOF

        if (!isAsciiString(line)) {
            invalid_banner = 1;
            break;
        }

        if (strlen(line) + strlen(bannerbuf) > MAX_BANNER_LEN) {
            banner_too_long = 1;
            break;
        }

        // Append new line to current buffer
        strcat(bannerbuf, line);

        if (strlen(bannerbuf) >= MAX_BANNER_LEN)
            break;
    }

    if (invalid_banner) {
        printf("Error: Invalid Banner string.\n");
        printf("Only ASCII characters are supported.\n");
        printf("Banner configuration aborted.\n");
        return 1;
    }

    if (banner_too_long) {
        printf("Error: Banner string is too long.\n");
        printf("Maximum supported length is 2500 characters.\n");
        printf("Banner configuration aborted.\n");
        return 1;
    }

    // Encode the banner string
    char *encoded_banner_str = url_encode(bannerbuf);

    // Need to fix system_command to support really long strings.
    // Using system() here directly for now
    sprintf(printbuffer, 
            "set_banner_login -banner_string \"%s\" > /dev/null 2>&1", 
            encoded_banner_str);
    system(printbuffer);

    free(encoded_banner_str);

    printf("\n");
    printf("Banner string configured sucessfully.\n");

    return 0;
}


int show_banner_login_main(int argc, char **argv)
{
    struct banner_login_params params;

    // Get current configured value
    retrieveBannerValues(&params);

    // URL Decode
    char *banner_str = url_decode(params.banner_string_value);

    // And show
    printf("%s", banner_str);

    free(banner_str);

    return 0;
}

// Read standard in one line at a time
static char *read_stdin(void) {
    char strbuffer[MAX_BANNER_LEN + 1];
    char * line;

    char * buffer = strdup("");
    while (true) {
        line = fgets(strbuffer, MAX_BANNER_LEN, stdin);
        if (line == NULL)
            break; // EOF

        if (!isAsciiString(line)) {
        	return buffer;
        }

        // Append new line to current buffer
        buffer = strgrowcat(buffer, line);
    }
    return buffer;
}

// Exposes simple url encoder that encodes only text strings
int urlencoder_main(int argc, char **argv)
{

    char * inputbuffer;
    char * outputbuffer;

    if (argc > 1) {
    	printf("When run without any arguments, this utility urlencodes \nstandard input to standard output\n");
    	exit(1);
    }
    // Read input
    inputbuffer = read_stdin();

    // Encode the input
    outputbuffer = url_encode(inputbuffer);

    // Dump output
    printf("%s", outputbuffer);

    free(outputbuffer);
    free(inputbuffer);

    return 0;
}


// Exposes simple url decoder that decodes only text strings
int urldecoder_main(int argc, char **argv)
{
    char * inputbuffer;
    char * outputbuffer;

    if (argc > 1) {
    	printf("When run without any arguments, this utility urldecodes \nstandard input to standard output.\n");
    	exit(1);
    }

    // Read input
    inputbuffer = read_stdin();

    // Encode the input
    outputbuffer = url_decode(inputbuffer);

    // Dump output
    printf("%s", outputbuffer);

    free(outputbuffer);
    free(inputbuffer);

    return 0;
}

