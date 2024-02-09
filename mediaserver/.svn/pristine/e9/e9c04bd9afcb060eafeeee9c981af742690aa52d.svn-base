#!/bin/sh
IPTABLES=/sbin/iptables

# TCP PORTS to block
SEDNA_PORTS="5050 5151"
RESERVED_TCP_PORTS="10000:10100 7070 7443 5223"

# UDP ports to block
SYSLOG_PORTS=514

TCP_PORTS="${RESERVED_TCP_PORTS} ${SEDNA_PORTS}"
UDP_PORTS="${SYSLOG_PORTS}"

# Clean all existing rules
${IPTABLES} -F
${IPTABLES} -t nat -F

# Set default policies for all chains on the filter table:
# Allow everything unless some other rule matches
${IPTABLES} -P INPUT	ACCEPT
${IPTABLES} -P FORWARD	ACCEPT
${IPTABLES} -P OUTPUT	ACCEPT

# Block incoming TCP traffic on all external interfaces (interfaces
# starting with "eth") for the listed ports
for port in ${TCP_PORTS}
do
    ${IPTABLES} -A INPUT -i eth+ -p tcp -m tcp --dport $port -m state --state NEW -j REJECT
done


# Block incoming UDP traffic on all external interfaces (interfaces
# starting with "eth") for the listed ports
for port in ${UDP_PORTS}
do
    ${IPTABLES} -A INPUT -i eth+ -p udp --dport $port -j DROP
done
${IPTABLES} -A INPUT -p ICMP --icmp-type timestamp-request -j DROP              
${IPTABLES} -A INPUT -p ICMP --icmp-type timestamp-reply -j DROP   


