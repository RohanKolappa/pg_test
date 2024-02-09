#!/usr/bin/haserl -u 10000 -s /bin/ash
Content-type: text/html
<?
echo ""
#export _DEBUG="on"
# change > to >> in the line below to log multiple cgi's
eval 'exec 2>/tmp/bweb_trace.log'
if [ "$_DEBUG" == "on" ]; then
  echo "================== ${SCRIPT_NAME} =================" >&2
  env | sort >&2
  set -x
fi
?>

<!DOCTYPE html>
<html>
  <head>
    <title>License Upload</title>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <link rel="stylesheet" href="/css/bootstrap.css" type="text/css">
    <link rel="stylesheet" href="/css/barco-nd.css" type="text/css">
    <link rel="stylesheet" href="/css/barco-setup.css" type="text/css">
  </head>
  <body class="form-group">
<?
ac=1
if [ -n "${FORM_licenseFile_path}" ]; then
  sz=`du -k "${FORM_licenseFile_path}" | cut -f1`
  if [ $sz -lt 1 ]; then
    msg="File missing or too small."
  else
    mv ${FORM_licenseFile_path} /tmp/barcoLicense.txt
    stat=`/V2O/bin/set_license_file`
    msg="$stat"
  fi
fi
?>
    <label>Use 
<?
/V2O/bin/get_license_pin
?>
    when requesting a license file. Then, upload here:</label>
    <form action="/cgi-bin/license-upload.cgi" enctype="multipart/form-data" method="post">
      <input type="file" name="licenseFile"><input type="submit" value="Upload">
      <p class="error-msg">
<?
echo $msg
?>
      </p>
    </form>
    <label>Current Licenses</label>
    <pre>
<?
if [ $ac -eq 1 ]; then
  /V2O/bin/verify_license
fi
?>
    </pre>
  </body>
</html>
