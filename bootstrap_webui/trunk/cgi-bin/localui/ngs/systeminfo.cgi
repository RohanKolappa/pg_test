#!/usr/bin/haserl -u 10000
<?
source /usr/share/bweb/localui/bweb_localui.sh
#bweb_header 1
bweb_header 
?>
	<form class="form-panel" method="post" >
	<h1 class="title">System Information</h1>


	<? bweb_listdata get_software_image_status "Software Information" ?>

	<? bweb_listdata get_xpi_firmware_image_status "Firmware Information" ?>

	<? bweb_listdata get_cpu_status "System Information"?>

	<? bweb_listdata get_memory_status "Memory Utilization"?>

	<? bweb_listdata get_system_clock_status "System Time "?>
	<? bweb_listdata get_xpi_fan_status "Temperature And Fan Information "?>
	</form>

<? bweb_footer ?>
