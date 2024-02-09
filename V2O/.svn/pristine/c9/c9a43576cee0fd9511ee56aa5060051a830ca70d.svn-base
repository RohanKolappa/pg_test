#include "gtest/gtest.h"
#include "validationroutines.h"
#include "custmenu.h"

TEST(ValidationTest, CheckIntorAny) {

    int rc;
    rc = check_integerorany("AnY");
    EXPECT_EQ(RC_NORMAL,rc); 

    rc = check_integerorany("any");
    EXPECT_EQ(RC_NORMAL,rc);

    rc = check_integerorany("any1");
    EXPECT_EQ(RC_ERROR,rc);

    rc = check_integerorany("1w");
    EXPECT_EQ(RC_ERROR, rc); 
 
    rc = check_integerorany("3");
    EXPECT_EQ(RC_NORMAL, rc);

    rc = check_integerorany("5555");
    EXPECT_EQ(RC_NORMAL, rc);

}

TEST(ValidationTest, CheckIntoranyrange) {

    int rc;
    rc = check_integeroranyrange("ANY", 0  ,4 );
    EXPECT_EQ(RC_NORMAL, rc);

    rc = check_integeroranyrange("any", 45 , 49);
    EXPECT_EQ(RC_NORMAL, rc);

    rc = check_integeroranyrange("A546", 5, 577);
    EXPECT_EQ(RC_ERROR, rc);

    rc = check_integeroranyrange("45", 40, 55);
    EXPECT_EQ(RC_NORMAL, rc);

    rc = check_integeroranyrange("6", 5, 579);
    EXPECT_EQ(RC_NORMAL, rc);

    rc = check_integeroranyrange("578", 5, 577);
    EXPECT_EQ(RC_ERROR, rc);

    rc = check_integeroranyrange("579", 5 , 579);
    EXPECT_EQ(RC_NORMAL, rc);

    rc = check_integeroranyrange("5", 5 , 579);
    EXPECT_EQ(RC_NORMAL, rc);

    rc = check_integeroranyrange("54", 55, 51);
    EXPECT_EQ(RC_ERROR, rc);

}

TEST(ValidationTest, CheckIntListRange) {

    int rc;
    rc = check_integerlist_range("3,13,22,23", 3, 25);
    EXPECT_EQ(RC_NORMAL, rc);

    rc = check_integerlist_range("3,13,22,23", 3, 25);
    EXPECT_EQ(RC_NORMAL, rc);
  
    rc = check_integerlist_range("224", 223, 224);
    EXPECT_EQ(RC_NORMAL, rc);

    rc = check_integerlist_range("220,221,222,224,223",220, 224);
    EXPECT_EQ(RC_NORMAL, rc);

    rc = check_integerlist_range("220,221, 222, 224, 223, -223", 220, 224);
    EXPECT_EQ(RC_ERROR, rc);

    rc = check_integerlist_range(".3, 4444", 0, 23);
    EXPECT_EQ(RC_ERROR, rc);
 
    rc = check_integerlist_range("2.3, 23.", 0, 23);
    EXPECT_EQ(RC_ERROR, rc);
  
    rc = check_integerlist_range(",", 0, 23);
    EXPECT_EQ(RC_ERROR, rc);

    rc = check_integerlist_range(",,,,,", 0, 23);
    EXPECT_EQ(RC_ERROR, rc);

    rc = check_integerlist_range(",,,3 4444", 0, 23);
    EXPECT_EQ(RC_ERROR, rc);

    rc = check_integerlist_range(",24,2", 5, 25);
    EXPECT_EQ(RC_ERROR, rc);

    rc = check_integerlist_range("224,", 223, 224);
    EXPECT_EQ(RC_ERROR, rc);

    rc = check_integerlist_range("224,223.", 223, 224);
    EXPECT_EQ(RC_ERROR, rc);

    rc = check_integerlist_range("a,a,a2", 7, 13);
    EXPECT_EQ(RC_ERROR, rc);
  
    rc = check_integerlist_range("a,11,12", 7, 13);
    EXPECT_EQ(RC_ERROR, rc);

    rc = check_integerlist_range("3, 24", 0, 23);
    EXPECT_EQ(RC_ERROR, rc);
 
    rc = check_integerlist_range("12,24,2", 0, 23);
    EXPECT_EQ(RC_ERROR, rc);

}

TEST(ValidationTest, CheckIP) {

    int rc;
    rc = check_ip("0.0.0.0");
    EXPECT_EQ(RC_NORMAL, rc);

    rc = check_ip("0.a.a.0");
    EXPECT_EQ(RC_ERROR, rc);

    rc = check_ip("");
    EXPECT_EQ(RC_ERROR,rc);

    rc = check_ip("12...");
    EXPECT_EQ(RC_ERROR,rc);

    rc = check_ip("s.2.3.3");
    EXPECT_EQ(RC_ERROR,rc);

    rc = check_ip("256.2.3.3");
    EXPECT_EQ(RC_ERROR,rc);

    rc = check_ip("192.2.6.67.");
    EXPECT_EQ(RC_ERROR,rc);

    rc = check_ip("192. 3.4.34");
    EXPECT_EQ(RC_ERROR,rc);

    rc = check_ip("192.168.1.216");
    EXPECT_EQ(RC_NORMAL,rc);

}

TEST(ValidationTest, CheckIPorBlank) {

    int rc;
    rc = check_ipaddorblank("0.0.0.0");
    EXPECT_EQ(RC_NORMAL, rc);

    rc = check_ipaddorblank("");
    EXPECT_EQ(RC_NORMAL,rc);

    rc = check_ipaddorblank("0.12.a.0");
    EXPECT_EQ(RC_ERROR, rc);

    rc = check_ipaddorblank("a...");
    EXPECT_EQ(RC_ERROR,rc);

    rc = check_ipaddorblank("s.4.4.3");
    EXPECT_EQ(RC_ERROR,rc);

    rc = check_ipaddorblank("192.168.1.216");
    EXPECT_EQ(RC_NORMAL,rc);

}

TEST(ValidationTest, CheckNetmask) {

    int rc;
    rc = check_netmask("0.0.0.0");
    EXPECT_EQ(RC_NORMAL, rc);

    rc = check_netmask("0.a.a.0");
    EXPECT_EQ(RC_ERROR, rc);

    rc = check_netmask("255.255.255.0");
    EXPECT_EQ(RC_NORMAL,rc);

    rc = check_netmask("255.255.0.0");
    EXPECT_EQ(RC_NORMAL,rc);

    rc = check_netmask("255.240.0.0");
    EXPECT_EQ(RC_NORMAL,rc);

    rc = check_netmask("255.255.255.90");
    EXPECT_EQ(RC_ERROR,rc);

    rc = check_netmask("");
    EXPECT_EQ(RC_ERROR,rc);

}

TEST(ValidationTest, CheckMulticastIP) {

    int rc;
    rc = check_multicastip("239.255.255.255");
    EXPECT_EQ(RC_NORMAL,rc);

    rc = check_multicastip("224.168.1.216");
    EXPECT_EQ(RC_NORMAL,rc);

    rc = check_multicastip("0.0.0.0");
    EXPECT_EQ(RC_ERROR, rc);

    rc = check_multicastip("0.a.a.0");
    EXPECT_EQ(RC_ERROR, rc);

    rc = check_multicastip("");
    EXPECT_EQ(RC_ERROR,rc);

    rc = check_multicastip("240.0.0.0");
    EXPECT_EQ(RC_ERROR,rc);

    rc = check_multicastip("192.168.1.12");
    EXPECT_EQ(RC_ERROR,rc);
}

TEST(ValidationTest, CheckInetname) {

    int rc;
    rc = check_inetname("0.0.0.0");
    EXPECT_EQ(RC_NORMAL, rc);

    rc = check_inetname("0.a.a.0");
    EXPECT_EQ(RC_ERROR, rc);

    rc = check_inetname("192.168.1.216");
    EXPECT_EQ(RC_NORMAL,rc);

    rc = check_inetname("atdms.com");
    EXPECT_EQ(RC_NORMAL,rc);

    rc = check_inetname("at.dms12.com");
    EXPECT_EQ(RC_NORMAL,rc);

    rc = check_inetname("at.ms..com");
    EXPECT_EQ(RC_ERROR,rc);

    rc = check_inetname("0.Bame.com");
    EXPECT_EQ(RC_ERROR,rc);
}

TEST(ValidationTest, CheckDomainname) {

    int rc;
    rc = check_domainname("");
    EXPECT_EQ(RC_NORMAL, rc);

    rc = check_domainname("12233.com");
    EXPECT_EQ(RC_ERROR, rc);

    rc = check_domainname("34abcd.com");
    EXPECT_EQ(RC_ERROR, rc);

    rc = check_domainname("abcd.com.");
    EXPECT_EQ(RC_ERROR, rc);

    rc = check_domainname(".xyx.com");
    EXPECT_EQ(RC_ERROR, rc);

    rc = check_domainname("dmsbuild123.com");
    EXPECT_EQ(RC_NORMAL,rc);

    rc = check_domainname("dms.build.com");
    EXPECT_EQ(RC_NORMAL,rc);

    rc = check_domainname("192.168.1.12");
    EXPECT_EQ(RC_ERROR, rc);

    rc = check_domainname(".com");
    EXPECT_EQ(RC_ERROR, rc);
}

TEST(ValidationTest, CheckHostnameorblank) {

    int rc;
    rc = check_hostnameorblank("");
    EXPECT_EQ(RC_NORMAL, rc);

    rc = check_hostnameorblank("DMS");
    EXPECT_EQ(RC_NORMAL,rc);

    rc = check_hostnameorblank("34abcd");
    EXPECT_EQ(RC_ERROR, rc);

    rc = check_hostnameorblank("DMS22");
    EXPECT_EQ(RC_NORMAL,rc);

    rc = check_hostnameorblank("DMS-22");
    EXPECT_EQ(RC_NORMAL,rc);

    rc = check_hostnameorblank("-DMS-22");
    EXPECT_EQ(RC_ERROR,rc);

    rc = check_hostnameorblank("44444");
    EXPECT_EQ(RC_ERROR, rc);

}

TEST(ValidationTest, CheckHexadecimal) {

    int rc;
    rc = check_hexadecimal("0x");
    EXPECT_EQ(RC_NORMAL,rc);

    rc = check_hexadecimal("88");
    EXPECT_EQ(RC_NORMAL, rc);

    rc = check_hexadecimal("0x12");
    EXPECT_EQ(RC_NORMAL, rc);

    rc = check_hexadecimal("0X12");
    EXPECT_EQ(RC_NORMAL, rc);

    rc = check_hexadecimal("0xA");
    EXPECT_EQ(RC_NORMAL, rc);

    rc = check_hexadecimal("F");
    EXPECT_EQ(RC_ERROR, rc);

    rc = check_hexadecimal("any");
    EXPECT_EQ(RC_ERROR,rc);

    rc = check_hexadecimal("0Xany");
    EXPECT_EQ(RC_ERROR,rc);

    rc = check_hexadecimal("0xx");
    EXPECT_EQ(RC_ERROR,rc);

    rc = check_hexadecimal("");// empty strings are accepted
    EXPECT_EQ(RC_NORMAL,rc);
 
}

TEST(ValidationTest, GetHexadecimal) {

    int rc;
    rc = get_hexadecimal("0x12");
    EXPECT_EQ(rc, 18);

    rc = get_hexadecimal("0X12");
    EXPECT_EQ(rc, 18);

    rc = get_hexadecimal("88");
    EXPECT_EQ(rc, 88);

    rc = get_hexadecimal("F");
    EXPECT_EQ(rc, 0);

    rc = get_hexadecimal("0xAF");
    EXPECT_EQ(rc, 175);

    rc = get_hexadecimal("0XaB");
    EXPECT_EQ(rc, 171);

}

TEST(ValidationTest, CheckOnlyHexadecimal) {

    int rc;
    rc = check_onlyhexadecimal("0x");
    EXPECT_EQ(RC_NORMAL,rc);
    
    rc = check_onlyhexadecimal("88");
    EXPECT_EQ(RC_NORMAL, rc);

    rc = check_onlyhexadecimal("2");
    EXPECT_EQ(RC_NORMAL, rc);

    rc = check_onlyhexadecimal("a");
    EXPECT_EQ(RC_NORMAL, rc);

    rc = check_onlyhexadecimal("E");
    EXPECT_EQ(RC_NORMAL, rc);

    rc = check_onlyhexadecimal("G");
    EXPECT_EQ(RC_ERROR, rc);

    rc = check_onlyhexadecimal("0x12");
    EXPECT_EQ(RC_NORMAL, rc);

    rc = check_onlyhexadecimal("0X12");
    EXPECT_EQ(RC_NORMAL, rc);

    rc = check_onlyhexadecimal("0xany");
    EXPECT_EQ(RC_ERROR,rc);

    rc = check_onlyhexadecimal("0xx");
    EXPECT_EQ(RC_ERROR,rc);

    rc = check_onlyhexadecimal("8FCa");
    EXPECT_EQ(RC_NORMAL, rc);
 
    rc = check_onlyhexadecimal("8WCF");
    EXPECT_EQ(RC_ERROR, rc);

    rc = check_onlyhexadecimal("");
    EXPECT_EQ(RC_NORMAL,rc);

}

TEST(ValidationTest, GetOnlyHexadecimal) {

    int rc;
    rc = get_onlyhexadecimal("0x12");
    EXPECT_EQ(rc, 18);

    rc = get_onlyhexadecimal("12");
    EXPECT_EQ(rc, 18);

    rc = get_onlyhexadecimal("88");
    EXPECT_EQ(rc, 136);

    rc = get_onlyhexadecimal("2");
    EXPECT_EQ(rc, 2);

    rc = get_onlyhexadecimal("E");
    EXPECT_EQ(rc, 14);

    rc = get_onlyhexadecimal("a");
    EXPECT_EQ(rc, 10);

    rc = get_onlyhexadecimal("8FCa");
    EXPECT_EQ(rc, 36810);

    rc = get_onlyhexadecimal("aBCD");
    EXPECT_EQ(rc, 43981);

    rc = get_onlyhexadecimal("0xaBCD");
    EXPECT_EQ(rc, 43981);    

}

TEST(ValidationTest, CheckEnum) {

    int rc;
    char *array[10] = {(char *) "Any", 
                       (char *) "any", 
                       (char *) "NULL", 
                       (char *) "12", 
                       (char *) "14", 
                       NULL};

    rc = check_enum("AnY", array);
    EXPECT_EQ(RC_NORMAL, rc);
    
    rc = check_enum("any", array);
    EXPECT_EQ(RC_NORMAL, rc);

    rc = check_enum("an", array);
    EXPECT_EQ(RC_ERROR, rc);

    rc = check_enum("12", array);
    EXPECT_EQ(RC_NORMAL, rc);

    rc = check_enum("any12", array);
    EXPECT_EQ(RC_ERROR, rc);

    rc = check_enum("any,14", array);
    EXPECT_EQ(RC_ERROR, rc);

    rc = check_enum("NULL", array);
    EXPECT_EQ(RC_NORMAL, rc);

    rc = check_enum("", array);
    EXPECT_EQ(RC_ERROR, rc);

}

TEST(ValidationTest, CheckRegexp) {

    int rc;
    char regexp_any[] = "^.*$";
    char regexp_anynonempty[] = "^[^[:space:]].*$";
    char regexp_alphanum[] = "[0-9a-zA-Z].$";
    char regexp_num[] = "[0-9].$";

    rc = check_regex("AnY", regexp_any);
    EXPECT_EQ(RC_NORMAL, rc);

    rc = check_regex("122", regexp_any);
    EXPECT_EQ(RC_NORMAL, rc);

    rc = check_regex("abCD320", regexp_any);
    EXPECT_EQ(RC_NORMAL, rc);

    rc = check_regex("", regexp_any);
    EXPECT_EQ(RC_NORMAL, rc);

    rc = check_regex("any", regexp_anynonempty);
    EXPECT_EQ(RC_NORMAL, rc);

    rc = check_regex("any123", regexp_anynonempty);
    EXPECT_EQ(RC_NORMAL, rc);

    rc = check_regex("1444A", regexp_anynonempty);
    EXPECT_EQ(RC_NORMAL, rc);

    rc = check_regex("122", regexp_anynonempty);
    EXPECT_EQ(RC_NORMAL, rc);

    rc = check_regex("", regexp_anynonempty);
    EXPECT_EQ(RC_ERROR, rc);
   
    rc = check_regex("12aa", regexp_alphanum);
    EXPECT_EQ(RC_NORMAL, rc);

    rc = check_regex("12", regexp_alphanum);
    EXPECT_EQ(RC_NORMAL, rc);

    rc = check_regex("aa12", regexp_alphanum);
    EXPECT_EQ(RC_NORMAL, rc);

    rc = check_regex("1@45", regexp_alphanum);
    EXPECT_EQ(RC_NORMAL, rc);

    rc = check_regex("", regexp_alphanum);
    EXPECT_EQ(RC_ERROR, rc);

    rc = check_regex("12", regexp_num);
    EXPECT_EQ(RC_NORMAL, rc);

    rc = check_regex("89", regexp_num);
    EXPECT_EQ(RC_NORMAL, rc);

    rc = check_regex("12aa", regexp_num);
    EXPECT_EQ(RC_ERROR, rc);

    rc = check_regex("aac", regexp_num);
    EXPECT_EQ(RC_ERROR, rc);

    rc = check_regex("", regexp_num);
    EXPECT_EQ(RC_ERROR, rc);

}

