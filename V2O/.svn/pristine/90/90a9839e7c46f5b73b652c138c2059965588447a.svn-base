#ifndef __TEXT_UTILS_H__
#define __TEXT_UTILS_H__


#ifdef __cplusplus
extern "C" {
#endif
extern int bits_to_bytes(const char *bitstring);
extern char *bytes_to_bits(int bytes, char *bitstring, int maxlen);
extern char *bytes_to_string(char *buf, unsigned long bytes);
extern char *bytes_l_to_string(char *buf, unsigned long long bytes);
extern char *trim_string(char *str);
extern int increment_ip(char *str, int increment);
extern char *parse_status_line(char *buffer, char *parameter);
extern char *strgrowcat(char *dest, const char *src);
#ifdef __cplusplus
}
#endif

#endif // __TEXT_UTILS_H__
