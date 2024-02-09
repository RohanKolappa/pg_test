#include "v2dlib_compatibility.h"

#ifndef __HH_V2D_H__
#include "v2d.h"
#endif

unsigned short get_protocol_value_short(unsigned char *cp) 
{ 
    unsigned short sval; 
    memcpy(&sval, cp, sizeof(short)); 
    return ntohs(sval); 
}

void set_protocol_value_short(unsigned char *cp, unsigned short val) 
{ 
    unsigned short sval = htons(val); 
    memcpy(cp, &sval, sizeof(short)); 
}

unsigned long get_protocol_value_long(unsigned char *cp) 
{ 
    unsigned long lval; 
    memcpy(&lval, cp, sizeof(long)); 
    return ntohl(lval); 
}

void set_protocol_value_long(unsigned char *cp, unsigned long val) 
{ 
    unsigned long lval = htonl(val); 
    memcpy(cp, &lval, sizeof(long)); 
}

unsigned short get_protocol_packet_length(unsigned char *cp) 
{ 
    unsigned short sval; 
    memcpy(&sval, cp, sizeof(short)); 
    return ntohs(sval); 
}

void set_protocol_packet_length(unsigned char *cp, int len)
{ 
    unsigned short sval = htons(len); 
    memcpy(cp, &sval, sizeof(short)); 
}

void fill_protocol_unit(int blen, int id, unsigned char *buf, unsigned char *cp)
{ 
    unsigned short sval = htons(blen + 4); 
    memcpy(cp, &sval, sizeof(short)); 
    sval = htons(id); 
    memcpy(cp + 2, &sval, sizeof(short)); 
    memcpy(cp + 4, buf, blen); 
} 

void fill_protocol_unit_short(int id, unsigned short val, unsigned char *cp) 
{
    unsigned short sval = htons((unsigned short) val); 
    memcpy(cp + 4, &sval, sizeof(short)); 
    sval = htons(6); 
    memcpy(cp, &sval, sizeof(short)); 
    sval = htons(id); 
    memcpy(cp + 2, &sval, sizeof(short)); 
} 

void fill_protocol_unit_long(int id, unsigned long val, unsigned char *cp) 
{
    unsigned short sval = htons(8); 
    unsigned long lval = htonl(val); 
    memcpy(cp + 4, &lval, sizeof(long)); 
    memcpy(cp, &sval, sizeof(short)); 
    sval = htons(id); 
    memcpy(cp + 2, &sval, sizeof(short)); 
}

void fill_protocol_unit_string(int slen, int id, char *str, unsigned char *cp)
{
    unsigned short sval = htons(slen + 4);
    memcpy(cp + 4, str, slen); 
    memcpy(cp, &sval, sizeof(short)); 
    sval = htons(id); 
    memcpy(cp + 2, &sval, sizeof(short)); 
}

unsigned short get_protocol_unit_id(unsigned char *cp)
{
    unsigned short sval; 
    memcpy(&sval, cp + 2, sizeof(short)); 
    return ntohs(sval); 
} 

void set_protocol_unit_id(unsigned char *cp, unsigned short id)
{ 
    unsigned short sval = htons(id); 
    memcpy(cp + 2, &sval, sizeof(short)); 
} 

unsigned short get_protocol_unit_short(unsigned char *cp) 
{ 
    unsigned short sval; 
    memcpy(&sval, cp + 4, sizeof(short)); 
    return ntohs(sval); 
}

unsigned long get_protocol_unit_long(unsigned char *cp) 
{
    unsigned long lval; 
    memcpy(&lval, cp + 4, sizeof(long)); 
    return ntohl(lval); 
} 

