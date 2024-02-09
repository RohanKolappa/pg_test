#!/usr/bin/haserl -u 10000
<?
source /usr/share/bweb/localui/bweb_localui.sh
#bweb_header 1
bweb_header 
?>
<? 
	if [ $bweb_auth_level -ge 1 ]; then
		bweb_cli
	fi
?>
<form class="form-horizontal"  action="/licenseservice/put" enctype="multipart/form-data" method="GET">
  
<!-- Top Row -->
<div class="row-fluid" style="height:50px;">
  <div class="section-title">License Information</div>
</div>
<!-- End Top Row -->
<!-- Middel Row -->
<div class="row-fluid" style="height:290px"><br>
  <fieldset>
	<div class="control-group">
	  <label class="control-label">Project Name</label>
	  <div class="controls">
		<input id="projectname" type="text" value="Project Name" readonly>
	  </div>
	</div>
	
	<div class="control-group">
	  <label class="control-label">Licensed To</label>
	  <div class="controls">
		<input id="licensedto" type="text" value="Licensed To" readonly>
	  </div>
	</div>

	<div class="control-group">
	  <label class="control-label">License Status</label>
	  <div class="controls">
		<input id="licensestatus" type="text" value="License Status" readonly>
	  </div>
	</div>
	
	<div class="control-group">
	  <label class="control-label">Days Left</label>
	  <div class="controls">
		<input id="daysleft" type="text" value="Days Left" readonly>
	  </div>
	</div>
	
	<div class="control-group">
	  <label class="control-label">Pincode</label>
	  <div class="controls">
		<input id="pincode" type="text" value="Pincode" readonly>
	  </div>
	</div>
	
	<div class="control-group">
	  <label class="control-label">License Key</label>
		<div class="controls">
		<input id="licensekey" type="text" value="License Key" readonly>
	  </div>
	</div>
	
  </fieldset>
</div>			
</form>
<form class="form-horizontal"  action="/licenseservice/put" enctype="multipart/form-data" method="POST">
  
<!-- Top Row -->
<div class="row-fluid" style="height:50px;">
  <div class="section-title">Upload License</div>
</div>
<!-- End Top Row -->

<div class="row-fluid" style="height:140px"><br>
  <fieldset>

	<div class="control-group">
	  <label class="control-label">Get License File</label>
		<div class="controls">
		<a href="http://productregistration.barco.com">To download your license click here</a>
	  </div>
	</div>

	<div class="control-group" style="margin-bottom:0px">
	  <label class="control-label">Upload License on Server</label>
	  <div class="controls">
		<input id="browse" name="license" type="file" accept=".lic" style="width:80; border-width:10";">
	  </div>
	</div>
	<div class="control-group">
	  <div class="controls">
		<input id="uploadinput" type="submit" value="Upload">
	  </div>
	</div>

  </fieldset>
</div>			
</form>

<? bweb_footer ?>