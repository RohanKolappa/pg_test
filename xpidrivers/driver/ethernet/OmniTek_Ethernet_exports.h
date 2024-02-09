#pragma once
/*******************************************************************************
 *
 * Copyright (c) 2007 Image Processing Techniques, Ltd. www.imageproc.com.
 * 
 * Image Processing Techniques Ltd. licenses this software under specific 
 * terms and conditions.  Use of any of the software or derviatives thereof 
 * in any product without a Image Processing Techniques is strictly prohibited. 
 * 
 * This file is provided without any express or implied warranties, including, 
 * but not limited to, the implied warranties of merchantability and fitness 
 * for a particular purpose. It is not intended for use in life support 
 * appliances, devices, or systems. Use in such applications is expressly 
 * prohibited.
 *
 * Image Processing Techniques makes no guarantee or representations regarding
 * the use of the results of the use of, the software and documentation in 
 * terms of correctness, accuracy, reliability, currentness, or otherwise; 
 * and you rely on the software, documentation and results solely at your
 * own risk.
 * 
 * IN NO EVENT SHALL IMAGE PROCESSING TECHNIQUES LTD. BE LIABLE FOR ANY LOSS 
 * OF USE, LOSS OF BUSINESS, LOSS OF PROFITS, INDIRECT, INCIDENTAL, SPECIAL 
 * OR CONSEQUENTIAL DAMAGES OF ANY KIND.
 *
 *****************************************************************************/

#include "OmniTek_Ethernet.h"
extern struct net_device * OmniTek_Ethernet_get_device(int index);
extern int  OmniTek_Ethernet_set_multicast_list_func(OmniTek_ext_ndo_set_multicast_list *, struct net_device *);
extern int  OmniTek_Ethernet_clear_multicast_list_func(OmniTek_ext_clear_multicast_list *, struct net_device *);
#if 0
extern void OmniTek_Ethernet_set_mdio_read_func(OmniTek_mdio_read *, 
                                                struct net_device *);
extern void OmniTek_Ethernet_set_mdio_write_func(OmniTek_mdio_write *, 
                                                 struct net_device *);
extern void OmniTek_Ethernet_set_mdio_speed_func(OmniTek_mdio_set_speed *, 
                                                 struct net_device *);
#endif
extern void OmniTek_Ethernet_set_ext_stats_func(OmniTek_ext_stats_func *, 
                                                 struct net_device *);

