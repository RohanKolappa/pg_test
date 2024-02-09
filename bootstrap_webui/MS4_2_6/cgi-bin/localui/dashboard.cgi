#!/usr/bin/haserl -u 10000
<?
if [ -f /tmp/${GET_cli}.xml ]; then
  cat /tmp/${GET_cli}.xml
else
  /V2O/bin/${GET_cli}
fi
?>