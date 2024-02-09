/*
 * xmlUtils.h
 *
 *  Created on: Jun 18, 2013
 *      Author: rkale
 */

#ifndef XMLUTILS_H_
#define XMLUTILS_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    void  *handle;
} XMLParser;

extern XMLParser *createXMLParser(const char * filename);
extern void destroyXMLParser(XMLParser * parser);

extern const char * XMLParserGetElementText(XMLParser * parser, const char * elementName);

extern int XMLParserHasError(XMLParser * parser);

#ifdef __cplusplus
}
#endif


#endif /* XMLUTILS_H_ */
