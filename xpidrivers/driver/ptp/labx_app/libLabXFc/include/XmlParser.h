#ifndef _XML_PARSER_H_
#define _XML_PARSER_H_

// System headers
#include <iostream>
#include <stdint.h>
#include <vector>

// Project headers
#include "XmlHandler.h"

// Locate within the "LabX" namespace
namespace LabX {

  /**
   * This class parses XML from an input stream and triggers callbacks in a 
   * handler in response to parsed XML "events".
   *
   * Copyright (c) 2003, Lab X Technologies, LLC
   * @author Eldridge M. Mount IV
   */
  class XmlParser {
    
    // Constructor / Destructor
  public:

    /**
     * Default constructor
     */
    XmlParser(void);

    /**
     * Virtual destructor
     */
    virtual ~XmlParser(void);

    // Public interface methods
  public:

    /**
     * Sets whether or not leading whitespace is to be ignored
     */
    void setStripWhitespace(const bool& stripWhitespace);

    /**
     * Parses XML from the passes input stream, triggering callbacks within
     * the passed handler.  Returns true if no fatal errors occurred during the
     * parse.  Returns normally when the root element is ended, returns an error
     * if the stream fails before then.
     */
    virtual const bool parse(std::istream& xmlStream, XmlHandler& handler);

    // Private type definitions
  private:

    /**
     * Enumerated type to represent the parser state
     */
    typedef enum
    {
      PARSE_CHARS,
      PARSE_GOT_LT,
      PARSE_ELEM_NAME,
      PARSE_XML_VERSION,
      PARSE_ATTR_NAME,
      PARSE_ATTR_VALUE,
      PARSE_END_ELEM,
      PARSE_COMMENT,
      PARSE_RSVD_CHAR
    } ParserState;

    // Private helper methods
  private:

    /**
     * Adds another element, and informs the handler
     */
    void addElement(std::string& qName, PropertyTable& atts, XmlHandler& handler);

    /**
     * Returns true if the passed character consists of ignorable whitespace
     */
    const bool isWhitespace(const char ch);

    // Private attributes
  private:

    /**
     * Parser state
     */
    ParserState state;

    /**
     * Property table for passing to the handler
     */
    PropertyTable atts;

    /**
     * Stack for pushing nested element tags
     */
    std::vector<std::string> elemStack;

    /**
     * Flag to indicate the XML version has been specified
     */
    bool gotXMLVersion;

    /**
     * Flag to indicate the root element has been found
     */
    bool gotRootElement;

    /**
     * Flag to indicate the document has ended
     */
    bool documentEnded;

    /**
     * Parse line number
     */
    uint32_t lineNumber;

    /**
     * Flag to control whether extra whitespace is stripped
     */
    bool stripWhitespace;

    /**
     * State variables for comment parsing
     */
    bool possibleComment;
    bool firstCharacter;
    uint32_t commentDashes;
  };
}

#endif
