#ifndef __CLI_UTILS_H__
#define __CLI_UTILS_H__

#define XML_GROUP       "<Group name=\"%s\">\n"
#define XML_GROUPEND    "</Group>\n"
#define XML_TABLE       "<Table name=\"%s\">\n"
#define XML_TABEND      "</Table>\n"
#define XML_OBJENTRY    "<ObjectEntry name=\"%s\">\n"
#define XML_OBJINDEX    "<ObjectEntry index=\"%d\" name=\"%s\">\n"
#define XML_OBJEND      "</ObjectEntry>\n"
#define XML_FIELD_START "<Attribute "
#define XML_FIELD_END 	"/>\n"
#define XML_FIELD     "name=\"%s\" label=\"%s\" type=\"%s\" size=\"%s\" defaultvalue=\"%s\" "
#define XML_FIELD_RANGE     " validation=\"%s\" min=\"%s\" max=\"%s\" "
#define XML_FIELD_NETMASK     " validation=\"%s\""
#define XML_FIELD_IPADDRESS     " validation=\"%s\""
#define XML_FIELD_ENUMERATION     " validation=\"%s\" values=\"%s\""
#define XML_FIELD_REGEX     " validation=\"%s\" regex=\"%s\" "
#define XML_ATTRINT     "<Attribute name=\"%s\" value=\"%d\"></Attribute>\n"
#define XML_ATTRFLOAT   "<Attribute name=\"%s\" value=\"%f\"></Attribute>\n"
#define XML_ATTRSTR     "<Attribute name=\"%s\" value=\"%s\"></Attribute>\n"
#define XML_ATTRSTR_DETAIL     "<Attribute name=\"%s\" value=\"%s\">%s</Attribute>\n"
#define XML_RESPONSE    "<Response>\n"
#define XML_RESPONSEEND "</Response>\n"

#define XML_MENUDESCR_ROOTBEGIN           "<MenuDescription>\n"
#define XML_MENUDESCR_ROOTEND             "</MenuDescription>\n"
#define XML_MENUDESCR_TYPE               "<Type>%s</Type>\n"
#define XML_MENUDESCR_TITLE               "<Title>%s</Title>\n"
#define XML_MENUDESCR_DESCRIPTION         "<Description>%s</Description>\n"
#define XML_MENUDESCR_BOARDACCESS         "<BoardAccess>%s</BoardAccess>\n"
#define XML_MENUDESCR_CLICOMMANDNAME      "<CliCommandName>%s</CliCommandName>\n"
#define XML_MENUDESCR_CONFIGFILENAME      "<ConfigFile>%s</ConfigFile>\n"
#define XML_MENUDESCR_ITEMSROOTBEGIN      "<MenuItems>\n"
#define XML_MENUDESCR_ITEMSROOTEND        "</MenuItems>\n"
#define XML_MENUDESCR_ITEMROOTBEGIN       "<MenuItem>\n"
#define XML_MENUDESCR_ITEMROOTEND         "</MenuItem>\n"
#define XML_MENUDESCR_ITEMNAME            "<Name>%s</Name>\n"
#define XML_MENUDESCR_ITEMLABEL           "<Label>%s</Label>\n"
#define XML_MENUDESCR_ITEMTYPE            "<Type>%s</Type>\n"
#define XML_MENUDESCR_ITEMHELP            "<Help>%s</Help>\n"
#define XML_MENUDESCR_ITEMALIGN           "<Align>%s</Align>\n"
#define XML_MENUDESCR_ITEMFIELDLENGTH     "<FieldLength>%s</FieldLength>\n"
#define XML_MENUDESCR_ITEMDEFAULTVALUE    "<DefaultValue>%s</DefaultValue>\n"
#define XML_MENUDESCR_ITEMVALIDATIONBEGIN "<Validation>\n"
#define XML_MENUDESCR_ITEMVALIDATIONEND   "</Validation>\n"
#define XML_MENUDESCR_ITEMVALIDATIONTYPE  "<Type>%s</Type>\n"
#define XML_MENUDESCR_ITEMVALIDATIONMIN  "<Min>%s</Min>\n"
#define XML_MENUDESCR_ITEMVALIDATIONMAX  "<Max>%s</Max>\n"
#define XML_MENUDESCR_ITEMVALIDATIONVALUE  "<Value>%s</Value>\n"
#define XML_MENUDESCR_ITEMVALIDATIONVALUES  "<Values>%s</Values>\n"

#define CLI_STATUS_NAME                 "Status"
#define CLI_REASON_NAME                 "Reason"
#define CLI_STATUS_SUCCESS              "Success"
#define CLI_STATUS_FAILURE              "Failure"
#define CLI_REASON_NA                   "N/A"
#define CLI_REASON_WRONG_HW             "Wrong HW"
#define CLI_REASON_NO_INPUT             "No Input Video"
#define CLI_REASON_BAD_FRAMECNT         "Bad Frame Count"
#define CLI_REASON_BAD_USAGE            "Bad Usage"
#define CLI_REASON_BAD_VALUE            "Bad Parameter Value"
#define CLI_REASON_NO_PERMISSION        "Permission Violation"
#define CLI_REASON_TOOMANY_LUT          "Too Many LUT Entries"
#define CLI_REASON_EMPTY_LUT            "No LUT Entries"
#define CLI_REASON_NOSUCH_LUT           "No Such LUT Entry"
#define CLI_REASON_SYSTEM_ERR           "Linux System Error"
#define CLI_REASON_BAD_INDEX            "Bad Index"
#define CLI_REASON_DUPLICATE            "Duplicate Entry"
#define CLI_REASON_EMPTY_EDID           "No EDID Entries"
#define CLI_REASON_EMPTY_RULELIST       "No Rulelist Entries"
#define CLI_REASON_CANNOT_COMM_DRIVER   "Cannot Communicate With Driver"
#define CLI_REASON_ALLOCATE_MEM_RULES   "Allocate Memory to Rules"
#define CLI_REASON_NO_RULES             "No Rules"
#define CLI_REASON_SYSTEM_BUSY          "System Busy"

extern char *yesnochecklist[]; /* Used by code generator */
extern char regexp_bandwidth[];
extern char regexp_password[];
extern char regexp_hostname[];
extern char regexp_any[];
extern char regexp_anynonempty[];

#ifdef __cplusplus
extern "C" {
#endif
extern void print_cli_failure(const char *groupname, const char *reason);
extern void print_cli_failure_with_detail(const char *groupname,
        const char *reason, const char *detail);
extern void print_cli_success(const char *groupname);

extern int view_only(void);
#ifdef __cplusplus
}
#endif

#endif // __CLI_UTILS_H__
