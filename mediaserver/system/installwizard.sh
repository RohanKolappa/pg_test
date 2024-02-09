#!/bin/sh 

Green='\033[32m'
GreenE='\033[m'
Red='\033[31m'
RedE='\033[m'

echo -e "${Red} installwizard will allow to set IP Address, Netmask & Gateway ${RedE}"  
echo -e -n "${Red} want to configure [y/n]: ${RedE}"
read result 

if [ `echo $result | tr [:upper:] [:lower:]` !=  "y" ]; then
    echo "Aborting..."
    return 1
fi
                                                                       

echo -n -e  "${Green} Enter IP Address : ${GreenE}"
read static_ip

if [ "$static_ip" = "" ]; then 
   echo  -e  "${Red} Error:Required Ip Address ${RedE}"
   return 1
fi

echo -n  -e "${Green} Enter Netmask    :${GreenE} "
read netmask

if [ "$netmask" = "" ]; then 
   echo  -e  "${Red} Error:Required Netmask ${RedE}"
   return 1
fi

echo -n -e  "${Green} Enter Gateway    :${GreenE} "
read gateway

if [ "$gateway" = "" ]; then 
   echo  -e  "${Red} Error:Required Gateway ${RedE}"
   return 1
fi
                                                            
index=`get_route_table | grep index | wc -l`


if [ "$index" -gt  "1" ]; then 
 echo  " More than one entry found"
 exit
fi

val=`/V2O/bin/mod_if_list 1  -usedhcp "No" -ipaddress $static_ip -netmask $netmask | grep -ri "Success" |  sed -n "s/.*value=\"\(.*\)\".*/\1/p"`
 if [ "$val" != "Success" ]; then 
   echo  -e  "${Red} Error in setting IP Address/Netmask check the Paramters ${RedE} "
   exit 1
 fi


if [ "$index" = "1" ]; then
  val=`/V2O/bin/mod_route_entry 1 -dst "0.0.0.0" -mask "0.0.0.0" -gw $gateway | grep -ri "Success" |  sed -n "s/.*value=\"\(.*\)\".*/\1/p"`
else
  val=`/V2O/bin/add_route_entry -dst "0.0.0.0" -mask "0.0.0.0" -gw $gateway | grep -ri "Success" |  sed -n "s/.*value=\"\(.*\)\".*/\1/p"`
fi


ret=$val | grep -ri "Success" |  sed -n "s/.*value=\"\(.*\)\".*/\1/p"
 if [ $val != "Success" ]; then 
   echo  -e  "${Red} Error in setting Gateway check the Paramter  ${RedE}"
   exit 1
 fi
 
 echo  " Done Applying configuration..."
