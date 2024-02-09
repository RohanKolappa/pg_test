#ifndef __H_UTILS__
#define __H_UTILS__

#include <string>
char * BitrateToString(double bitrate, char *buffer);
char * GetTimeString(timestamp_t a_milliseconds, char *a_buffer);
int ReadFileIntoBuffer(const char * filename, uint8_t * &buffer);
std::string TrimString(std::string a_string);


#endif /*__H_UTILS__*/
