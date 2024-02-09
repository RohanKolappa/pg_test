#ifndef __STRMSRVACCESS_H__
#define __STRMSRVACCESS_H__

#ifdef __cplusplus
extern "C" {
#endif

    extern int strmsrv_xml_create(const char *xml_string);
    extern int strmsrv_xml_destroy(void);
    extern int strmsrv_xml_select_object_by_id(int id);
    extern int strmsrv_xml_select_object_by_port(int ioport);
    extern int strmsrv_xml_select_rev_audio_object_by_port(int ioport);
    extern int strmsrv_xml_select_object_by_queueid(int queueid);
    extern int strmsrv_xml_get_integer_parameter(const char *a_name, int *a_value);
    extern int strmsrv_xml_get_unsigned_long_parameter(const char *a_name, 
                                                       unsigned long *a_value);
    extern int strmsrv_xml_get_unsigned_long_long_parameter(const char *a_name,
                                                       unsigned long long *a_value);
    extern int strmsrv_xml_get_float_parameter(const char *a_name, double *a_value);

    extern int strmsrv_xml_get_text_parameter(const char *a_name, 
                                              char *a_value, 
                                              int a_max_length);
    extern int strmsrv_xml_get_boolean_parameter(const char *a_name, 
                                                 int *a_value);


#ifdef __cplusplus
}
#endif


#endif
