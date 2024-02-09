#include <iostream>
#include <gtest/gtest.h>
#include <gtest_utils/gtest_utils.hh>
#include "system_utils.h"
#include "commonutils.h"
#include "text_utils.h"

using namespace std;

class SystemUtilsTest : public testing::Test {
protected:
    virtual void SetUp() {
        m_sTempAreaDir = "temp_test_area/";
        GTestUtils::CreateDirectory(m_sTempAreaDir);
    }
    virtual void TearDown() {
        GTestUtils::DeleteDirectory(m_sTempAreaDir);
    }

    void CreateHexFile(string testfile, unsigned char testdat[], 
            int testdatlen, int longformat, int oneline) {
        char line[80], *buffer;

        for (int i = 0; i < testdatlen; i += 8) {
            if (longformat) { 
                unsigned long longdat[2];
                if (i < testdatlen - 7) {
                    longdat[0] = (testdat[i+3] << 24) | (testdat[i+2] << 16) |
                        (testdat[i+1] << 8) | testdat[i];
                    longdat[1] = (testdat[i+7] << 24) | (testdat[i+6] << 16) |
                        (testdat[i+5] << 8) | testdat[i+4];
                    sprintf(line, "%08lx %08lx", longdat[0], longdat[1]);
                }
                else if (i < testdatlen - 6) {
                    longdat[0] = (testdat[i+3] << 24) | (testdat[i+2] << 16) |
                        (testdat[i+1] << 8) | testdat[i];
                    longdat[1] =  (testdat[i+6] << 16) | (testdat[i+5] << 8) | 
                        testdat[i+4];
                    sprintf(line, "%08lx %08lx", longdat[0], longdat[1]);
                }
                else if (i < testdatlen - 5) {
                    longdat[0] = (testdat[i+3] << 24) | (testdat[i+2] << 16) |
                        (testdat[i+1] << 8) | testdat[i];
                    longdat[1] =  (testdat[i+5] << 8) | testdat[i+4];
                    sprintf(line, "%08lx %08lx", longdat[0], longdat[1]);
                }
                else if (i < testdatlen - 4) {
                    longdat[0] = (testdat[i+3] << 24) | (testdat[i+2] << 16) |
                        (testdat[i+1] << 8) | testdat[i];
                    sprintf(line, "%08lx", longdat[0]);
                }
                else if (i < testdatlen - 3) {
                    longdat[0] = (testdat[i+2] << 16) | (testdat[i+1] << 8) | 
                        testdat[i];
                    sprintf(line, "%08lx", longdat[0]);
                }
                else if (i < testdatlen - 2) {
                    longdat[0] = (testdat[i+1] << 8) | testdat[i];
                    sprintf(line, "%08lx", longdat[0]);
                }
                else if (i < testdatlen - 1) {
                    longdat[0] = testdat[i];
                    sprintf(line, "%08lx", longdat[0]);
                }
                else 
                    break;
            }
            else if (i < testdatlen - 7) 
                sprintf(line, "%02x %02x %02x %02x %02x %02x %02x %02x",
                    testdat[i], testdat[i+1], testdat[i+2], testdat[i+3], 
                    testdat[i+4], testdat[i+5], testdat[i+6], testdat[i+7]);
            else if (i < testdatlen - 6) 
                sprintf(line, "%02x %02x %02x %02x %02x %02x %02x",
                    testdat[i], testdat[i+1], testdat[i+2], testdat[i+3], 
                    testdat[i+4], testdat[i+5], testdat[i+6]);
            else if (i < testdatlen - 5) 
                sprintf(line, "%02x %02x %02x %02x %02x %02x",
                    testdat[i], testdat[i+1], testdat[i+2], testdat[i+3], 
                    testdat[i+4], testdat[i+5]);
            else if (i < testdatlen - 4) 
                sprintf(line, "%02x %02x %02x %02x %02x",
                    testdat[i], testdat[i+1], testdat[i+2], testdat[i+3],
                    testdat[i+4]); 
            else if (i < testdatlen - 3) 
                sprintf(line, "%02x %02x %02x %02x",
                    testdat[i], testdat[i+1], testdat[i+2], testdat[i+3]);
            else if (i < testdatlen - 2) 
                sprintf(line, "%02x %02x %02x",
                    testdat[i], testdat[i+1], testdat[i+2]);
            else if (i < testdatlen - 1) 
                sprintf(line, "%02x %02x", testdat[i], testdat[i+1]);
            else if (i < testdatlen)
                sprintf(line, "%02x", testdat[i]);
            else
                break;

            if (!oneline)
                strcat(line, "\n");
            if (i == 0) 
                buffer = strdup((const char *) line);
            else 
                buffer = strgrowcat(buffer, line);
        }
        GTestUtils::CreateFileFromBuffer(testfile, (unsigned char *) buffer, 
                strlen((const char *) buffer));
        free(buffer);
    }

    void CheckReadValidHexFromFile(string testfile, int testdatlen, 
            char testmd5sum[], int longformat) {
        FILE *file = fopen((const char *) (testfile.c_str()), "r");
        if (file) {
            int rc;
            unsigned char *memblock;
            char md5sum[40];
            memblock = (unsigned char *) malloc(testdatlen);
            rc = read_hex_from_file(file, memblock, testdatlen, longformat);
            fclose(file);
            EXPECT_EQ(testdatlen, rc);
            GTestUtils::MD5Sum(memblock, rc, md5sum);
            EXPECT_STREQ(testmd5sum, md5sum);
            free(memblock);
        }
    }

    void CheckReadInvalidHexFromFile(string testfile, int testdatlen,
            int longformat) {
        FILE *file = fopen((const char *) (testfile.c_str()), "r");
        if (file) {
            int rc;
            unsigned char *memblock;
            if (longformat)
                memblock = (unsigned char *) malloc(testdatlen + 
                        (testdatlen + 3) / 4);
            else
                memblock = (unsigned char *) malloc(testdatlen);
            rc = read_hex_from_file(file, memblock, testdatlen, longformat);
            fclose(file);
            free(memblock);
            EXPECT_LE(rc, 0);
        }
    }

protected:
    string m_sTempAreaDir;
};

TEST_F(SystemUtilsTest, ReadValidHexFile) {
    unsigned char defaultedid[] = {
        0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00,
        0x26, 0x16, 0x02, 0x01, 0x01, 0x00, 0x00, 0x00,
        0x10, 0x12, 0x01, 0x03, 0x80, 0x34, 0x20, 0x78,
        0x2b, 0xb0, 0xa6, 0xa3, 0x56, 0x4a, 0x9c, 0x23,
        0x11, 0x4f, 0x53, 0x03, 0xcf, 0x80, 0x81, 0xcd,
        0x81, 0x40, 0x95, 0x00, 0xb3, 0x00, 0x01, 0x01,
        0x81, 0x80, 0xa9, 0x40, 0x71, 0x40, 0xe2, 0x68,
        0x00, 0xa0, 0xa0, 0x40, 0x2e, 0x60, 0x30, 0x20,
        0x36, 0x00, 0xc4, 0x8e, 0x21, 0x00, 0x00, 0x1e,
        0x28, 0x3c, 0x80, 0xa0, 0x70, 0xb0, 0x23, 0x40,
        0x30, 0x20, 0x36, 0x00, 0xc4, 0x8e, 0x21, 0x00,
        0x00, 0x1e, 0x02, 0x3a, 0x80, 0x18, 0x71, 0x38,
        0x2d, 0x40, 0x58, 0x30, 0x45, 0x00, 0xc4, 0x8e,
        0x21, 0x00, 0x00, 0x1e, 0x00, 0x00, 0x00, 0xfc,
        0x00, 0x58, 0x50, 0x32, 0x30, 0x30, 0x2d, 0x44,
        0x20, 0x20, 0x20, 0x0a, 0x20, 0x20, 0x00, 0xb2
    };
    unsigned char meiedid[] = {
        0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00,
        0x34, 0xa9, 0x75, 0xd0, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x11, 0x01, 0x03, 0x80, 0x00, 0x00, 0x78,
        0x0a, 0x69, 0xbe, 0xa6, 0x57, 0x53, 0xa6, 0x23,
        0x0c, 0x48, 0x55, 0x00, 0x00, 0x00, 0x01, 0x01,
        0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
        0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x1d,
        0x80, 0x18, 0x71, 0x1c, 0x16, 0x20, 0x58, 0x2c,
        0x25, 0x00, 0xc4, 0x8e, 0x21, 0x00, 0x00, 0x9e,
        0x01, 0x1d, 0x80, 0xd0, 0x72, 0x1c, 0x16, 0x20,
        0x10, 0x2c, 0x25, 0x80, 0xc4, 0x8e, 0x21, 0x00,
        0x00, 0x9e, 0x00, 0x00, 0x00, 0xfc, 0x00, 0x41,
        0x45, 0x2d, 0x32, 0x30, 0x30, 0x30, 0x0a, 0x20,
        0x20, 0x20, 0x20, 0x20, 0x00, 0x00, 0x00, 0xfd,
        0x00, 0x18, 0x3d, 0x1c, 0x44, 0x0f, 0x00, 0x0a,
        0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x01, 0xd6,
        0x02, 0x03, 0x1b, 0x31, 0x4a, 0x05, 0x14, 0x04,
        0x13, 0x20, 0x02, 0x11, 0x01, 0x10, 0x1f, 0xe3,
        0x05, 0x03, 0x01, 0x67, 0x03, 0x0c, 0x00, 0x30,
        0x00, 0x00, 0x00, 0x01, 0x1d, 0x80, 0x3e, 0x73,
        0x38, 0x2d, 0x40, 0x7e, 0x2c, 0x45, 0x80, 0xc4,
        0x8e, 0x21, 0x00, 0x00, 0x1e, 0x8c, 0x0a, 0xd0,
        0x8a, 0x20, 0xe0, 0x2d, 0x10, 0x10, 0x3e, 0x96,
        0x00, 0x13, 0x8e, 0x21, 0x00, 0x00, 0x18, 0x8c,
        0x0a, 0xd0, 0x90, 0x20, 0x40, 0x31, 0x20, 0x0c,
        0x40, 0x55, 0x00, 0x13, 0x8e, 0x21, 0x00, 0x00,
        0x18, 0x01, 0x1d, 0x00, 0x72, 0x51, 0xd0, 0x1e,
        0x20, 0x6e, 0x28, 0x55, 0x00, 0xc4, 0x8e, 0x21,
        0x00, 0x00, 0x1e, 0x01, 0x1d, 0x00, 0xbc, 0x52,
        0xd0, 0x1e, 0x20, 0xb8, 0x28, 0x55, 0x40, 0xc4,
        0x8e, 0x21, 0x00, 0x00, 0x1e, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x94
    };
    unsigned char testdat[] = {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 
        0x10, 
    };
    string testfile = m_sTempAreaDir + "testdat";
    char testmd5sum[40];

    // Create hex file with 128 bytes of data 
    CreateHexFile(testfile, defaultedid, sizeof(defaultedid), 0, 0);
    GTestUtils::MD5Sum(defaultedid, sizeof(defaultedid), testmd5sum);
    CheckReadValidHexFromFile(testfile, sizeof(defaultedid), testmd5sum, 0);
    remove(testfile.c_str());

    // Create hex file with 256 bytes of data 
    CreateHexFile(testfile, meiedid, sizeof(meiedid), 0, 0);
    GTestUtils::MD5Sum(meiedid, sizeof(meiedid), testmd5sum);
    CheckReadValidHexFromFile(testfile, sizeof(meiedid), testmd5sum, 0);
    remove(testfile.c_str());

    // Create hex file with # bytes not multiples of 4
    CreateHexFile(testfile, testdat, sizeof(testdat), 0, 0);
    GTestUtils::MD5Sum(testdat, sizeof(testdat), testmd5sum);
    CheckReadValidHexFromFile(testfile, sizeof(testdat), testmd5sum, 0);
    remove(testfile.c_str());

    // Create long hex file with 128 bytes of data 
    CreateHexFile(testfile, defaultedid, sizeof(defaultedid), 1, 0);
    GTestUtils::MD5Sum(defaultedid, sizeof(defaultedid), testmd5sum);
    CheckReadValidHexFromFile(testfile, sizeof(defaultedid), testmd5sum, 1);
    remove(testfile.c_str());
}

TEST_F(SystemUtilsTest, ReadInvalidHexFile) {
    unsigned char testdat[] = {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 
        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
        0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 
        0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
        0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f, 
        0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
        0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f, 
        0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47,
        0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f, 
        0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57,
        0x58, 0x59, 0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f, 
    };

    string testfile = m_sTempAreaDir + "testdat";
    unsigned char buffer[80];

    // Create hex file with lines > max allowed length (128)
    CreateHexFile(testfile, testdat, sizeof(testdat), 0, 1);
    CheckReadInvalidHexFromFile(testfile, sizeof(testdat), 0);
    remove(testfile.c_str());
    
    // Create hex file with invalid characters
    strcpy((char *) buffer, "01 02 this is not a hex file");
    GTestUtils::CreateFileFromBuffer(testfile, buffer, 
            strlen((const char *)buffer));
    CheckReadInvalidHexFromFile(testfile, sizeof(buffer), 0);
    remove(testfile.c_str());
}

TEST_F(SystemUtilsTest, GetSystemTemperature) {
    int rc, buflen;
    unsigned char buffer[80];
    string filename;

    filename = m_sTempAreaDir + "temperature";
    strcpy((char *) buffer, "temperature:             40 C\n");
    GTestUtils::CreateFileFromBuffer(filename, buffer, 
            strlen((const char *) buffer));
    rc = get_sysfs_attribute(filename.c_str(), "temperature: %d C\n");
    EXPECT_EQ(40, rc);

    strcpy((char *) buffer, "temperature:             20 C\n");
    GTestUtils::CreateFileFromBuffer(filename, buffer, 
            strlen((const char *) buffer));
    rc = get_sysfs_attribute(filename.c_str(), "temperature: %d C\n");
    EXPECT_EQ(20, rc);

    strcpy((char *) buffer, "temperature:             100 C\n");
    GTestUtils::CreateFileFromBuffer(filename, buffer, 
            strlen((const char *) buffer));
    rc = get_sysfs_attribute(filename.c_str(), "temperature: %d C\n");
    EXPECT_EQ(100, rc);

    strcpy((char *) buffer, "temperature:             0 C\n");
    GTestUtils::CreateFileFromBuffer(filename, buffer, 
            strlen((const char *) buffer));
    rc = get_sysfs_attribute(filename.c_str(), "temperature: %d C\n");
    EXPECT_EQ(0, rc);

    strcpy((char *) buffer, "temperature:             -10 C\n");
    GTestUtils::CreateFileFromBuffer(filename, buffer, 
            strlen((const char *) buffer));
    rc = get_sysfs_attribute(filename.c_str(), "temperature: %d C\n");
    EXPECT_EQ(-10, rc);

    strcpy((char *) buffer, "");
    GTestUtils::CreateFileFromBuffer(filename, buffer, 
            strlen((const char *) buffer));
    rc = get_sysfs_attribute(filename.c_str(), "temperature: %d C\n");
    EXPECT_EQ(0, rc);

    remove(filename.c_str());
}
