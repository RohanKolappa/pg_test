//
// Copyright 2003 Lab X Technologies, LLC

// #define _LABXDEBUG

// Library headers
#include "Debug.h"
#include "XmlParser.h"
#include "LabX.h"
#include <ios>

// Namespace using directives
using LabX::toString;
using std::ios;
using std::istream;
using std::string;

// Implementation of class XmlParser

// Constructor / Destructor

LabX::XmlParser::XmlParser(void) :
  state(PARSE_CHARS),
  atts(),
  elemStack(),
  gotXMLVersion(false),
  gotRootElement(false),
  documentEnded(false),
  lineNumber(1),
  stripWhitespace(true) {}

LabX::XmlParser::~XmlParser(void) {}

// Public interface methods

void LabX::XmlParser::setStripWhitespace(const bool& stripWhitespace) {
  LabX::XmlParser::stripWhitespace = stripWhitespace;
}

const bool LabX::XmlParser::parse(istream& xmlStream, XmlHandler& handler) {
  char ch;
  string buf;
  string rsvdBuf;
  string element;
  string attrName;
  string attrValue;
  bool inQuotes = false;
  bool fatalError = false;

  // Initialize state attributes
  state = PARSE_CHARS;
  atts.clear();
  elemStack.clear();
  gotXMLVersion = false;
  gotRootElement = false;
  documentEnded = false;
  lineNumber = 1;

  DebugNote("Beginning to parse document");

  // Ensure that the input stream will not skip whitespace - we rely upon it for parsing
  // per XML's semantics
  xmlStream >> std::noskipws;

  // Loop, parsing characters off of the input stream
  xmlStream >> ch;
  while((xmlStream.fail() == false) && (fatalError == false) && (documentEnded == false)) {
    // Update state based upon previous state and the new character
    switch(state) {
    case PARSE_CHARS:
      {
        switch(ch) {
        case '<':
          {
            // About to start processing a tag or comment, flush any
            // buffered characters in one gulp
            if(buf.empty() == false) {
              handler.characters(buf);
              buf.clear();
            }
            state = PARSE_GOT_LT;
            firstCharacter = true;
            possibleComment = false;
            break;
          }
          
        case '&':
          {
            state = PARSE_RSVD_CHAR;
            break;
          }
          
        default:
          {
            // Make sure the call to isWhitespace() is not short-circuited to
            // update line numbers regardless
            if((isWhitespace(ch) == false) || 
               (buf.empty() == false) || 
               (stripWhitespace == false)) {
              buf.append(1, ch);
            }
          } // default:
        } // switch(ch)
        
        break;
      } // case PARSE_CHARS
          
    case PARSE_RSVD_CHAR:
      {
        // Dummy call to update line number
        isWhitespace(ch);
        switch(ch) {
        case ';':
          {
            if(rsvdBuf == "amp") {
              buf.append(1, '&');
            } else if(rsvdBuf == "apos") {
              buf.append(1, '\'');
            } else if(rsvdBuf == "quot") {
              buf.append(1, '\"');
            } else if(rsvdBuf == "lt") {
              buf.append(1, '<');
            } else if(rsvdBuf == "gt") {
              buf.append(1, '>');
            } else {
              handler.fatalError("Unrecognized XML escape sequence \"&" +
                                 rsvdBuf +
                                 ";\" on line " +
                                 toString(lineNumber));
              fatalError = true;
            }
            
            state = PARSE_CHARS;
            
            break;
          } // case ';':
          
        default:
          {
            if(isWhitespace(ch) || (rsvdBuf.size() >= 4)) {
              handler.fatalError("XML escape sequence \"&" +
                                 rsvdBuf +
                                 "\" missing semicolon on line " +
                                 toString(lineNumber));
              fatalError = true;
            } else {
              rsvdBuf.append(1, ch);
            }
          } // default:
        } // switch(ch)
            
        break;
      } // case PARSE_RSVD_CHAR:
          
    case PARSE_GOT_LT:
      {
        switch(ch) {
        case '!':
          element.append(1, ch);
          possibleComment = firstCharacter;
          commentDashes = 0;
          break;

        case '-':
          // Fully-qualify comment starts
          element.append(1, ch);
          if(possibleComment) {
            if(++commentDashes == 2) {
              state = PARSE_COMMENT;
              element.clear();
              commentDashes = 0;
            }
          } else {
            state = PARSE_ELEM_NAME;
          }
          break;
          
        case '/':
          if(firstCharacter) {
            state = PARSE_END_ELEM;
          } else {
            element.append(1, ch);
            state = PARSE_ELEM_NAME;
          }
          break;
          
        default:
          possibleComment = false;
          if(isWhitespace(ch) == false) {
            element.append(1, ch);
            state = PARSE_ELEM_NAME;
          } else {
            handler.fatalError("Empty tag name on line " + toString(lineNumber));
            fatalError = true;
          }
        } // switch(ch)
        
        // No longer the first character in the element
        firstCharacter = false;
        break;
      } // case PARSE_GOT_LT:
      
    case PARSE_ELEM_NAME:
      {
        switch(ch) {
        case '>':
          {
            addElement(element, atts, handler);
            state = PARSE_CHARS;
            break;
          } // case '>':
          
        default:
          {
            if(isWhitespace(ch) == false) {
              element.append(1, ch);
            } else {
              // Specially treat the <?xml ...> tag
              if(element == "?xml") {
                if(gotXMLVersion) {
                  handler.fatalError("XML version tag \"<?xml ...>\" specified a second time on line " +
                                     toString(lineNumber));
                  fatalError = true;
                } else {
                  gotXMLVersion = true;
                  state = PARSE_XML_VERSION;
                }
              } else {
                state = PARSE_ATTR_NAME;
              }
            }
          } // default:
        } // switch(ch)
        
        break;
      } // case PARSE_ELEM_NAME:
      
    case PARSE_XML_VERSION:
      {
        // For now, just ignore the XML version tag's attributes, we just don't want to
        // process it as a normal element tag.
        if(ch == '>') {
          element = "";
          state = PARSE_CHARS;
        }
        break;
      }
          
    case PARSE_ATTR_NAME:
      {
        switch(ch) {
        case '>':
          {
            addElement(element, atts, handler);
            state = PARSE_CHARS;
            break;
          } // case '>':
          
        default:
          {
            if(isWhitespace(ch) == false) {
              if(ch != '=') {
                  attrName.append(1, ch);
              } else {
                state = PARSE_ATTR_VALUE;
              }
            } else if(attrName.empty() == false) {
              handler.fatalError("Malformed attribute for element " + 
                                 element +
                                 " on line " +
                                 toString(lineNumber));
              fatalError = true;
            }
          } // default:
        } // switch(ch)
        
        break;
      } // case PARSE_ATTR_NAME:
          
    case PARSE_ATTR_VALUE:
      {
        switch(ch) {
        case '>':
          {
            addElement(element, atts, handler);
            state = PARSE_CHARS;
            break;
          } // case '>':
          
        case '\"':
          {
            if(attrValue.empty() && !inQuotes) {
              inQuotes = true;
            } else {
              atts.setProperty(attrName, attrValue);
              attrName = attrValue = "";
              inQuotes = false;
              state = PARSE_ATTR_NAME;
            }
                  
            break;
          } // case '\"':
          
        default:
          {
            // Dummy call to update line number
            isWhitespace(ch);
            if(inQuotes == true) {
              attrValue.append(1, ch);
            } else {
              handler.fatalError("Attribute value for element \"" +
                                 element +
                                 "\", attribute \"" +
                                 attrName +
                                 "\" missing quotes on line " +
                                 toString(lineNumber));
              fatalError = true;
            }
          } // default:
        } // switch(ch)
        
        break;
      } // case PARSE_ATTR_VALUE:
      
    case PARSE_END_ELEM:
      {
        switch(ch) {
        case '>':
          {
            if(elemStack.empty() == false) {
              if(elemStack.back() == element) {
                handler.endElement(element);
                elemStack.pop_back();
                element = "";
                if(elemStack.empty()) {
                  handler.endDocument();
                  documentEnded = true;
                }
                state = PARSE_CHARS;
              } else {
                handler.fatalError("Tag / end tag mismatch: \"<" +
                                   elemStack.back() +
                                   ">\" vs. \"</" +
                                   element +
                                   ">\" on line " +
                                   toString(lineNumber));
                fatalError = true;
              }
            } else {
              handler.fatalError("Unmatched end tag </" +
                                 element +
                                 "> on line " +
                                 toString(lineNumber));
              fatalError = true;
            }
                  
            break;
          } // case '>':
          
        default:
          {
            if(isWhitespace(ch) == false) {
              element.append(1, ch);
            } else {
              handler.fatalError("Whitespace in end tag on line " +
                                 toString(lineNumber));
              fatalError = true;
            }
          } // default:
        } // switch(ch)
            
        break;
      } // case PARSE_END_ELEM:
          
    case PARSE_COMMENT:
      {
        // Dummy call to update line number
        isWhitespace(ch);

        switch(ch) {
        case '-':
          commentDashes++;
          break;

        case '>':
          if(commentDashes >= 2) {
            state = PARSE_CHARS;
          }
          break;

        default:
          // Continuously reset the dash count
          commentDashes = 0;
        }
        
        break;
      } // case PARSE_COMMENT
          
    default:
      {
        // Invalid state, should not happen!
        state = PARSE_CHARS;
      }
    } // switch(state)
    
    // Grab the next character unless we know the loop will fall through
    if(!(fatalError || documentEnded)) {
      xmlStream >> ch;
    }
  } // while(xmlStream.fail() == false)
  
  // Ensure that a root element was found
  if(gotRootElement == false) {
    handler.fatalError("No root element found");
  } else if(documentEnded == false) {
    handler.fatalError("Stream exhausted before root element ended");
  }

  // Return true upon success
  return(!fatalError);
}

// Private helper methods

void LabX::XmlParser::addElement(string& element, PropertyTable& atts, 
                                 XmlHandler& handler) {
  if(documentEnded == true) {
    handler.fatalError("Element \"" +
                       element +
                       "\" detected outside of root element on line " +
                       toString(lineNumber));
  } else {
    if(elemStack.empty()) {
	  handler.startDocument();
	  gotRootElement = true;
	}
    elemStack.push_back(element);
    handler.startElement(element, atts);
    element.clear();
    atts.clear();
  } // !if(documentEnded == true)
}

const bool LabX::XmlParser::isWhitespace(const char ch) {
  if(ch == '\n') lineNumber++;
  return((ch == ' ') || (ch == '\t') || (ch == '\r') || (ch == '\n'));
}
