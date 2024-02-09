#!/bin/sh
IPTABLES=/sbin/iptables
IPLOOPBACK=127.0.0.1
DSSPORT=9191
HTTPPORT=80
HTTPSPORT=443
SNMPPORT=161
SNMPTRAPPORT=162
SSHPORT=22
DNSPORT=53
TELNETPORT=23
DHCPPORT=67

WWWSERVER=
NOCSERVER=
LOCALWEBSERVER=
SECONDARY=
ROUTER=
MASQUERADE=

${IPTABLES} -F INPUT	
${IPTABLES} -F FORWARD	
${IPTABLES} -F OUTPUT	
${IPTABLES} -t nat -F


${IPTABLES} -P INPUT	ACCEPT
${IPTABLES} -P FORWARD	ACCEPT
${IPTABLES} -P OUTPUT	ACCEPT

${IPTABLES} -A INPUT -s ${IPLOOPBACK} -j ACCEPT
${IPTABLES} -A OUTPUT -d ${IPLOOPBACK} -j ACCEPT

${IPTABLES} -A INPUT -p icmp -j ACCEPT

${IPTABLES} -A INPUT -m state --state ESTABLISHED,RELATED  -j ACCEPT
${IPTABLES} -A FORWARD -m state --state ESTABLISHED,RELATED  -j ACCEPT
for port in ${SSHPORT} ${HTTPPORT} ${HTTPSPORT} ${DSSPORT}
do
${IPTABLES} -A INPUT -p tcp -m tcp --dport $port -m state --state NEW -j ACCEPT
done
for port in ${DHCPPORT} ${SNMPPORT} ${SNMPTRAPPORT}
do
${IPTABLES} -A INPUT -p udp -m udp --dport $port -m state --state NEW -j ACCEPT
done

# Following configurations are for the router case
if [ x${ROUTER} = "x" ];
then
	exit 0
fi

/sbin/sysctl -w net.ipv4.ip_forward=1

for port in ${SSHPORT} ${HTTPSPORT} ${HTTPPORT} ${HTTPSPORT} ${DSSPORT} ${SNMPPORT} ${DNSPORT}
do
${IPTABLES} -A FORWARD -p tcp -m tcp --dport $port -m state --state NEW -j ACCEPT
done

 
for port in ${DNSPORT}
do
${IPTABLES} -A FORWARD -p udp -m udp --dport $port -m state --state NEW -j ACCEPT
done
# Masquerade rules for NAT
if [ x${MASQUERADE} != "x" ];
then
	${IPTABLES} -A POSTROUTING -t nat -o eth0 -j MASQUERADE
fi
#Redirect rules for captive portal do not redirect traffic to noc and www
for server in ${WWWSERVER} ${NOCSERVER} ${SECONDARY} ${LOCALWEBSERVER}
do
	${IPTABLES} -I PREROUTING -t nat -d $server -j ACCEPT
done
#${IPTABLES} -A PREROUTING -t nat -p tcp -j REDIRECT --to 80
CUSTOMRULES=
