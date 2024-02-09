lutid=$1
#lutid=1
#while(true); do
 #if [ ${lutid} -gt 33 ]; then
   #exit
 #fi
name=`cat /V2O/config/lutlistconf_0.txt |grep "NAME.${lutid} " |cut -c27-`
hres=`cat /V2O/config/lutlistconf_0.txt |grep "HRES.${lutid} " |cut -c27-`
vres=`cat /V2O/config/lutlistconf_0.txt |grep "VRES.${lutid} " |cut -c27-`
refresh=`cat /V2O/config/lutlistconf_0.txt |grep "REFRESH.${lutid} " |cut -c27-33`
pclk=`cat /V2O/config/lutlistconf_0.txt |grep "PCR.${lutid} " |cut -c27-33`
hfp=`cat /V2O/config/lutlistconf_0.txt |grep "HFP.${lutid} " |cut -c27-`
vfp=`cat /V2O/config/lutlistconf_0.txt |grep "VFP.${lutid} " |cut -c27-`
hbp=`cat /V2O/config/lutlistconf_0.txt |grep "HBP.${lutid} " |cut -c27-`
vbp=`cat /V2O/config/lutlistconf_0.txt |grep "VBP.${lutid} " |cut -c27-`
hswidth=`cat /V2O/config/lutlistconf_0.txt |grep "HSYNC.${lutid} " |cut -c27-`
vswidth=`cat /V2O/config/lutlistconf_0.txt |grep "VSYNC.${lutid} " |cut -c27-`
echo Setting Rx splash to ${name}
echo ${hres} ${vres} ${refresh} ${pclk} ${hfp} ${vfp} ${hbp} ${vbp} ${hswidth} ${vswidth}
lutid=`expr ${lutid} + 1`
#done
/V2O/bin/set_rx_splash_parm -splash_screen "Grid Lines" -enable_splash Yes -stereo No -horiz_resolution ${hres} -vert_resolution ${vres} -refresh_rate ${refresh} -pixel_clock_rate ${pclk} -horiz_front_porch ${hfp} -vert_front_porch ${vfp} -horiz_back_porch ${hbp} -vert_back_porch ${vbp} -horiz_synch ${hswidth} -vert_synch ${vswidth}
/V2O/bin/get_rx_splash_parm
