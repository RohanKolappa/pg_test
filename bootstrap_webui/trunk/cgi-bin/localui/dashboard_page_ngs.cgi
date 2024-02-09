#!/usr/bin/haserl
<?
source /usr/share/bweb/localui/bweb_localui.sh
bweb_header
?>
<script>
<?
local system_health_template=`tr -d '\n' < /var/www/templates/localui/ngs/system-health.txt`
local system_info_template=`tr -d '\n' < /var/www/templates/localui/ngs/system-info.txt`
local tx_adjustments_template=`tr -d '\n' < /var/www/templates/localui/ngs/tx_adjustments.txt`
local tx_statistics_template=`tr -d '\n' < /var/www/templates/localui/ngs/tx_statistics.txt`
local rx_adjustments_template=`tr -d '\n' < /var/www/templates/localui/ngs/rx_adjustments.txt`
local rx_statistics_template=`tr -d '\n' < /var/www/templates/localui/ngs/rx_statistics.txt`
echo "var system_health_template = '${system_health_template}';"
echo "var system_info_template = '${system_info_template}';"
echo "var tx_adjustments_template = '${tx_adjustments_template}';"
echo "var tx_statistics_template = '${tx_statistics_template}';"
echo "var rx_adjustments_template = '${rx_adjustments_template}';"
echo "var rx_statistics_template = '${rx_statistics_template}';"
?>
</script>
<h1 class="title">Dashboard</h1>
<div class="row-fluid">
  <div class="box">
    <div class="box-header">
      <span class="title">Status</span>
    </div>
    <div class="box-content">
      <div id="section1" class="span6"></div>
      <div id="section2" class="span6"></div>
    </div> <!-- /box-content -->
  </div> <!-- /box -->
</div> <!-- /fluid-row -->
<div class="row-fluid spacer">
  <div id="section3" class="span6"></div>
  <div id="section4" class="span6"></div>
</div> <!-- /row-fluid -->
<div class="row-fluid spacer"></div>
<div class="row-fluid spacer"></div>
<?
bweb_footer
?>
