/*
 * test_xml_utils.cpp
 *
 *  Created on: Jun 18, 2013
 *      Author: rkale
 */

#include "gtest/gtest.h"
#include <gtest_utils/gtest_utils.hh>


#include "xmlUtils.h"

class XMLUtilsTest : public testing::Test {
protected:
    virtual void SetUp() {
        m_sTempAreaDir = "temp_test_area/";
        GTestUtils::DeleteDirectory(m_sTempAreaDir);
        GTestUtils::CreateDirectory(m_sTempAreaDir);

        m_XMLParser = NULL;
    }
    virtual void TearDown() {
        GTestUtils::DeleteDirectory(m_sTempAreaDir);

        if (m_XMLParser != NULL)
            destroyXMLParser(m_XMLParser);
    }

    void CreateXMLParser(std::string fileName) {
        std::string actualFileName = m_sTempAreaDir + fileName;
        m_XMLParser = createXMLParser(actualFileName.c_str());
    }

    void CreateFileFromString(std::string fileName, std::string contents) {
        GTestUtils::CreateFileFromBuffer(m_sTempAreaDir + fileName,
                (unsigned char *)contents.c_str(), contents.length());
    }

protected:
    std::string m_sTempAreaDir;

    XMLParser *m_XMLParser;
};


TEST_F(XMLUtilsTest, NonexstingFile_ReturnsError) {
    CreateXMLParser("no_such_file.xml");

    EXPECT_EQ(1, XMLParserHasError(m_XMLParser));
}

TEST_F(XMLUtilsTest, InValidXMLFile_ReturnsError) {
    CreateFileFromString("invalidfile.xml", "asdf\n");

    CreateXMLParser("invalid_file.xml");

    EXPECT_EQ(1, XMLParserHasError(m_XMLParser));
}


TEST_F(XMLUtilsTest, ValidXMLFile_DoesNotHaveError) {
    CreateFileFromString("xml_file.xml", "<e>text</e>\n");

    CreateXMLParser("xml_file.xml");

    EXPECT_EQ(0, XMLParserHasError(m_XMLParser));
}

TEST_F(XMLUtilsTest, GetElementText_WithSingleRootElement_Succeeds) {
    CreateFileFromString("xml_file.xml", "<E>Some Text</E>\n");

    CreateXMLParser("xml_file.xml");

    const char * elementText = XMLParserGetElementText(m_XMLParser, "E");
    EXPECT_STREQ("Some Text", elementText);
}

TEST_F(XMLUtilsTest, GetElementText_WithNonExistingElement_ReturnsNull) {
    CreateFileFromString("xml_file.xml", "<E>some text</E>\n");

    CreateXMLParser("xml_file.xml");

    const char * elementText = XMLParserGetElementText(m_XMLParser, "NoSuchElement");
    EXPECT_TRUE(elementText == NULL);
}

TEST_F(XMLUtilsTest, GetElementText_WithTwoElements_Succeeds) {
    CreateFileFromString("xml_file.xml", "<root><A>text A</A> <B>text B</B></root>\n");

    CreateXMLParser("xml_file.xml");

    const char * elementText = XMLParserGetElementText(m_XMLParser, "A");
    EXPECT_STREQ("text A", elementText);
    elementText = XMLParserGetElementText(m_XMLParser, "B");
    EXPECT_STREQ("text B", elementText);

}

TEST_F(XMLUtilsTest, GetElementText_WithNestedElement_Succeeds) {
    CreateFileFromString("xml_file.xml", "<root><A><B>text B in A</B></A></root>\n");

    CreateXMLParser("xml_file.xml");

    const char * elementText = XMLParserGetElementText(m_XMLParser, "B");
    EXPECT_STREQ("text B in A", elementText);
}

TEST_F(XMLUtilsTest, GetElement_WithDuplicateElements_ReturnsFirstText) {
    CreateFileFromString("xml_file.xml", "<root><A>Some Text</A> <A>Some Other Text</A></root>\n");

    CreateXMLParser("xml_file.xml");

    const char * elementText = XMLParserGetElementText(m_XMLParser, "A");
    EXPECT_STREQ("Some Text", elementText);
}

TEST_F(XMLUtilsTest, GetElementText_WithDuplicateNestedElement_ReturnsFirstText) {
    CreateFileFromString("xml_file.xml", "<root><A><B>text B in A</B></A> <B>Some Other Text</B></root>\n");

    CreateXMLParser("xml_file.xml");

    const char * elementText = XMLParserGetElementText(m_XMLParser, "B");
    EXPECT_STREQ("text B in A", elementText);
}
