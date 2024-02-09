#!/usr/bin/haserl -u 10000
Content-type: text/html

<?
local deviceType=`/V2O/bin/get_version | grep 'name="hardware"' | cut -f4 -d'"'`
deviceType=`echo $deviceType | cut -f1 -d"-"`
deviceType=`echo $deviceType | tr '[:punct:]' '-' | tr '[A-Z]' '[a-z]'`

source /usr/share/bweb/localui/bweb_localui.sh

IFS='=&'
set -- $QUERY_STRING
local reconfigure=$2
        
if [ -n "$QUERY_STRING" ]; then
	QUERY_STRING=?$QUERY_STRING
fi
bweb_locate_res 'localui/' 'dashboard_page' '.cgi'
if [ -n "$bweb_resource" ]; then
	echo "<html><meta http-equiv=\"refresh\" content=\"0;URL=/cgi-bin/$bweb_resource${QUERY_STRING}\"></html>"
else
	echo "<html><head></head><body><pre>Device is not configured properly. Please \"Revert to Default\" and setup from Bootstrap Wizard</pre></body></html>"
fi

?>
