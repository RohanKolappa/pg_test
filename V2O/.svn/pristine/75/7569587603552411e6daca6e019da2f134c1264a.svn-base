#include "gtest/gtest.h"
#include "text_utils.h"
#include "custmenu.h"
TEST(TextUtilsTest, BitsToBytes) {

    int rc;
    rc = bits_to_bytes("1K");
    EXPECT_EQ(rc, 128);

    rc = bits_to_bytes("8k");
    EXPECT_EQ(rc, 1024);
    
    rc = bits_to_bytes("1M");
    EXPECT_EQ(rc, 131072 );

    rc = bits_to_bytes("2m");
    EXPECT_EQ(rc, 262144) ;

    rc = bits_to_bytes("1000k");
    EXPECT_EQ(rc, 128000);
    
    rc = bits_to_bytes("65536k");
    EXPECT_EQ(rc, 8388608);

    rc = bits_to_bytes("100");
    EXPECT_EQ(rc, 12);

    rc = bits_to_bytes("1024");
    EXPECT_EQ(rc, 128);

    rc = bits_to_bytes("1048576");
    EXPECT_EQ(rc, 131072);

    rc = bits_to_bytes("0");
    EXPECT_EQ(rc, 0);

    rc = bits_to_bytes("-1k");// It should not be given as -ve value and if given it always returns 0
    EXPECT_EQ(rc, 0);

}


TEST(TextUtilsTest, BytesToBits) {
    
    char bitstring[32];

    bytes_to_bits(0, bitstring, 32);
    EXPECT_STREQ("0.00", bitstring);

    bytes_to_bits(1, bitstring, 32);
    EXPECT_STREQ("8.00", bitstring);    

    bytes_to_bits(1, bitstring, 2);
    EXPECT_STREQ("8", bitstring);

    bytes_to_bits(1024, bitstring, 32);
    EXPECT_STREQ("8.00K", bitstring);

    bytes_to_bits(2, bitstring, 4);
    EXPECT_STREQ("16.", bitstring);

    bytes_to_bits(10, bitstring, 8);
    EXPECT_STREQ("80.00", bitstring);

    bytes_to_bits(11, bitstring, 11);
    EXPECT_STREQ("88.00", bitstring);
  
    bytes_to_bits(1048576, bitstring, 32);
    EXPECT_STREQ("8.00M", bitstring);

    bytes_to_bits(1073741824, bitstring, 32);
    EXPECT_STREQ("8192.00M", bitstring);

    bytes_to_bits(-11110, bitstring, 32); //This is an invalid value for byte, but if given it should return 0
    EXPECT_STREQ("0.00", bitstring);

    bytes_to_bits(14444110, bitstring, 32);
    EXPECT_STREQ("110.20M", bitstring);

    bytes_to_bits(144410000, bitstring, 32);
    EXPECT_STREQ("1101.76M", bitstring);

    sprintf(bitstring, "%s", "zero maxlen");//setting the value of bitstring and returning the same if maxlen is 0 or -ve
    bytes_to_bits(140, bitstring, 0);
    EXPECT_STREQ("zero maxlen", bitstring);

    sprintf(bitstring, "%s", "-ve maxlen");
    bytes_to_bits(10, bitstring, -34);
    EXPECT_STREQ("-ve maxlen", bitstring);
  
    bytes_to_bits(1444410000, bitstring, 256);
    EXPECT_STREQ("11019.97M", bitstring);

}


TEST(TextUtilsTest, BytesToString) {

    char rc[32];

    bytes_to_string(rc, 1);
    EXPECT_STREQ("1", rc);

    bytes_to_string(rc, 1024);
    EXPECT_STREQ("1.00 KB", rc);

    bytes_to_string(rc, 1048576);
    EXPECT_STREQ("1.00 MB", rc);

    bytes_to_string(rc, 34444);
    EXPECT_STREQ("33.64 KB", rc);

    bytes_to_string(rc, 0);
    EXPECT_STREQ("0", rc);

    bytes_to_string(rc, -1); //Not a bug, but the i/p should only be +ve values
    EXPECT_STREQ("4.00 GB", rc); //this value is returned because its signature is "unsigned" and we giving -ve value, so it returns the maximum value in the range

    bytes_to_string(rc, 10000);
    EXPECT_STREQ("9.77 KB", rc);

    bytes_to_string(rc, 99);
    EXPECT_STREQ("99", rc);
   
    bytes_to_string(rc, 100);
    EXPECT_STREQ("100", rc);

    bytes_to_string(rc, 1000);
    EXPECT_STREQ("1000", rc);

    bytes_to_string(rc, 100000);
    EXPECT_STREQ("97.66 KB", rc);



}

TEST(TextUtilsTest, BytesLongToString) {

    
    char rc[32];

    bytes_l_to_string(rc, 23);
    EXPECT_STREQ("23", rc);

    bytes_l_to_string(rc, 0);
    EXPECT_STREQ("0", rc);

    bytes_l_to_string(rc, 100);
    EXPECT_STREQ("100", rc);

    bytes_l_to_string(rc, 230000);
    EXPECT_STREQ("224.61 KB", rc);

    bytes_l_to_string(rc, 100000000);
    EXPECT_STREQ("95.37 MB", rc);

    bytes_l_to_string(rc, 1024);
    EXPECT_STREQ("1.00 KB", rc);

    bytes_l_to_string(rc, 10240);
    EXPECT_STREQ("10.00 KB", rc);

    bytes_l_to_string(rc, 10240000);
    EXPECT_STREQ("9.77 MB", rc);
 
    bytes_l_to_string(rc, 1000000000); //Maximum length for a long type integer, need to be investigated on this.
    EXPECT_STREQ("953.67 MB", rc);

    bytes_l_to_string(rc, 999999999);
    EXPECT_STREQ("953.67 MB", rc);

}

TEST(TextUtilsTest, Strgrowcat) {

    char *dest = strdup("IP");
    strgrowcat(dest, " Video");
    EXPECT_STREQ("IP Video", dest);
    free(dest);

    char *dest1 = strdup("");
    strgrowcat(dest1, "IP Video");
    EXPECT_STREQ("IP Video", dest1);
    free(dest1);

    char *dest2 = strdup("IP Video");
    strgrowcat(dest2, "");
    EXPECT_STREQ("IP Video", dest2);
    free(dest2);

    char *dest3 = strdup("");
    strgrowcat(dest3, "");
    EXPECT_STREQ("", dest3);
    free(dest3);

}
