#!/usr/bin/haserl -u 10000
Content-type: text/html

<?
local deviceType=`/V2O/bin/get_version | grep 'name="hardware"' | cut -f4 -d'"'`
deviceType=`echo $deviceType | cut -f1 -d"-"`
deviceType=`echo $deviceType | tr '[:punct:]' '-' | tr '[A-Z]' '[a-z]'`

#if the factory file exists, the /wizard/bweb_wizard.sh is loaded and the user is redirected to the Bootstrap Wizard 
#if the factory file does not exist, the /localui/bweb_localui.sh is loaded and the user is redirected to the Local UI 

case "$deviceType" in
  "ngs") 
    if [ ! -f /mnt/cf/factory ]; then
      source /usr/share/bweb/localui/bweb_localui.sh
      if [ -n "$QUERY_STRING" ]; then
        QUERY_STRING=?$QUERY_STRING
      fi
      bweb_locate_res 'localui/' 'dashboard_page' '.cgi'
      echo "<html><meta http-equiv=\"refresh\" content=\"0;URL=/cgi-bin/$bweb_resource${QUERY_STRING}\"></html>"
    else
      source /usr/share/bweb/wizard/bweb_wizard.sh
      bweb_setup
    fi
  ;;

  *)
    if [ ! -f /etc/config/factory ]; then
      source /usr/share/bweb/localui/bweb_localui.sh
      IFS='=&'
      set -- $QUERY_STRING
      local reconfigure=$2
      if [ $reconfigure ]; then
        bweb_setup
      else
        if [ -n "$QUERY_STRING" ]; then
          QUERY_STRING=?$QUERY_STRING
        fi
        local scenable=`/V2O/bin/get_xmpp_server | grep 'name="enable"' | cut -f4 -d'"'`
        if [ "$scenable" = "Yes" ]; then
          echo "<html><meta http-equiv=\"refresh\" content=\"0;URL=/clients/gui/index.html${QUERY_STRING}\"></html>"
        else
          bweb_locate_res 'localui/' 'dashboard_page' '.cgi'
          if [ -n "$bweb_resource" ]; then
            echo "<html><meta http-equiv=\"refresh\" content=\"0;URL=/cgi-bin/$bweb_resource${QUERY_STRING}\"></html>"
          else
            echo "<html><head></head><body><pre>Device is not configured properly. Please \"Revert to Default\" and setup from Bootstrap Wizard</pre></body></html>"
          fi
        fi
      fi
    else
      source /usr/share/bweb/wizard/bweb_wizard.sh
      bweb_setup
    fi
  ;;
esac
?>
