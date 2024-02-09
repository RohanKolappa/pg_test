/* 
 * Copyright 2013 Barco, Inc. All rights reserved.
 */

/* ============================================================================
 * Localization
 * ==========================================================================*/

var barco_dms_setup_wizard_langs = {
    en: {
      ref: barco_dms_setup_wizard_en,
      plural: barco_dms_setup_wizard_en_plural,
      plural_is: barco_dms_setup_wizard_en_plural_is
      }
};

var barco_dms_setup_wizard_resource = null;

function barco_setupLocalization(defaultLang) {
  if (navigator.userLanguage) { // IE case
    baseLang = navigator.userLanguage.substring(0,2).toLowerCase();
  } else { // All other browsers
    baseLang = navigator.language.substring(0,2).toLowerCase();
  }
  var resource = barco_dms_setup_wizard_langs[baseLang];
  if (resource !== undefined) {
    barco_dms_setup_wizard_resource = resource;
  } else {
    resource = barco_dms_setup_wizard_langs[defaultLang];
    barco_dms_setup_wizard_resource = resource;
  }
}

function tJ(id) {
  return barco_dms_setup_wizard_resource.ref[id];
}

function tJ_plural(num) {
  return barco_dms_setup_wizard_resource.plural(num);
}

function tJ_plural_is(num) {
  return barco_dms_setup_wizard_resource.plural_is(num);
}

/* ============================================================================
 * Pane Management
 * ==========================================================================*/

var barco_panes = [
  {pane:'general', wiz:'wiz1', valid: []},
  {pane:'password', wiz:'wiz2', valid: []},
  {pane:'network', wiz:'wiz3', valid: []},
  {pane:'msconn', wiz:'wiz4', valid: []},
  {pane:'other', wiz:'wiz4', valid: []}
];

var barco_urls = {
  setup:'/cgi-bin/wizard/setup_dms.cgi',
  success:'/',
  controllerLink: '/',
  clientLink: '/clients/web/IPVSClient-Index.htm'
  // setup:'/cgi-bin/test_send.cgi',
  // success:'/cgi-bin/test_show.cgi',
  // testNewIPaddress: '10.51.51.75'
};

var barco_timeIntervals = {
  checkLinkInterval:2*1000, // 2 seconds
  checkLinkTimeout:2*1000, // 2 seconds
  rebootTimout1: 60*1000, // 60 seconds
  rebootTimout2: 15*1000, // 15 seconds
  rebootFailTimeout: 4*60*1000 // 4 minutes
};

var barco_devnam_range = {min: 1};
var barco_xmpp_server = {enabled: false, enabled_now: false};
var barco_passwd_range = {
	min: 8,
	max: 14
};
var barco_networkSettings = {
  dhcp: false, ipaddress: '', netmask: '', gateway: ''
};
var barco_xmpp_agent = {
  dmsdevicename: '', dmspassword: '', scipaddress: '', autoMode: true
};

var barco_dms_mode = {
  dms: '', combo: ''
};

var barco_MsSettings_dms = {
  scipaddress: '', autoDetect: false, ntpClientIp: ''
};

var barco_MsSettings_combo = {
  discovery: false, accounts: false, ntpClientIp: '', ntp: false
};

var barco_generalSettings = {
  password: ''
};

var barco_isAutoDetectDmsCheckboxEnabled = false;
var barco_isFirstTimeExecuted = true;
var barco_isDms = false;
var barco_doneBtn = false;
var barco_submitResponse = "";
var barco_checkLinkTimer = null;
var barco_isFactoryDefault = false;

function barco_paneIdToIdx(paneId) {
  var idx = 0;
  var len = barco_panes.length;
  while (barco_panes[idx].pane !== paneId && idx < len) idx++;
  return idx;
}

function barco_getPaneId() {
  return $("div.well .outer-pane.active").attr('id');
}

function barco_allPanesValid() {
  var len = barco_panes.length;
  var all_valid = true;
  // Temporary Hack for the New DMS Wizard. Now we have only 4 panes instead of 5
  var i = barco_paneIdToIdx('other');
  barco_panes[i].valid[0] = true;
  
  for (var i = 0; i < len; i++) {
    if (!barco_panes[i].valid[0]) {
      all_valid = false;
      break;
    }
  }
  return all_valid;  
}

function barco_enablePreviousBtn(enable) {
  if (enable) {
    $('#btn_previous').removeClass('disabled');
  } else {
    $('#btn_previous').addClass('disabled');
  }
}

function barco_enableNextBtn(enable) {
  if (enable) {
    $('#btn_next').removeClass('disabled');
  } else {
    $('#btn_next').addClass('disabled');
  }
}

// Shows enabled/disabled Done Btn
function barco_showEnabledDoneBtn(enable) {
    $('#btn_next').addClass('btn-primary');

  if (enable) {
    $('#btn_next').removeClass('disabled');
    $('#btn_next').html('Done');
    barco_doneBtn = true;
  } else {
    $('#btn_next').addClass('disabled');
    $('#btn_next').html('Done');
    barco_doneBtn = false;
  }
}

function barco_enableDoneBtn(enable) {
  if (enable) {
    $('#btn_next').addClass('btn-primary');
    $('#btn_next').removeClass('disabled');
    $('#btn_next').html('Done');
    barco_doneBtn = true;
  } else {
    $('#btn_next').removeClass('btn-primary');
    $('#btn_next').html('Next');
    barco_doneBtn = false;
  }
}

function barco_changePanes(prvIdx, toIdx) {
  var len = barco_panes.length;
  if (!barco_xmpp_server.enabled_now) len--; // ignore other pane
  var valid = barco_panes[prvIdx].valid[0] || prvIdx >= toIdx;
  
  if (toIdx < 0 || toIdx >= len || !valid) return;
  for (var i = 0; i < len; i++) {
    var paneObj = barco_panes[i];
    if (i === toIdx) {
      $('#' + paneObj.wiz).addClass('current');
      $('#' + paneObj.wiz + ' span.badge').addClass("badge-inverse");
      $("div.well .outer-pane#" + paneObj.pane).addClass('active');
      if (prvIdx !== toIdx)
        $("div.well .outer-pane#" + paneObj.pane + ' input[tabindex="1"]').focus();
    } else {
      $('#' + paneObj.wiz).removeClass('current');
      $('#' + paneObj.wiz + ' span.badge').removeClass("badge-inverse");
      $("div.well .outer-pane#" + paneObj.pane).removeClass('active');
    }
  }
  barco_enablePreviousBtn(toIdx > 0);
  barco_enableNextBtn(toIdx < len-1 && barco_panes[toIdx].valid[0]);
   
  // Special hack to enable Done btn on last page.
  var i = barco_paneIdToIdx('msconn');
  var j = barco_paneIdToIdx('other');
  barco_enableDoneBtn(barco_allPanesValid() && (i === toIdx || j === toIdx));
  
  if (i === toIdx) 
  {
	// First time validation, when the user lands on the "Other" Pane.
	checkPaneState();
	barco_showEnabledDoneBtn(barco_panes[i].valid[0]);
  }
  else
	barco_enableDoneBtn(barco_allPanesValid() && i === toIdx);
  
  
}

/* ============================================================================
 * Error Messages
 * ==========================================================================*/

var barco_errCnt = 0;

function barco_setControlGroupState(ref, good) {
  if (good) {
    $(ref).removeClass('error');
  } else {
    $(ref).addClass('error');
  }
}

function barco_showErrMsg(ref, str, arg1, arg2, arg3, arg4) {
  $(ref + ' p.error-msg').removeClass('hidden')
    .html(str + arg1 + arg2 + arg3 + arg4);
  barco_errCnt++;
  if (barco_errCnt === 1) $(ref + ' input').focus();
}

function barco_hideErrMsg(ref) {
  $(ref + ' p.error-msg').addClass('hidden').html('');
}

function barco_errAtLeastNChars(ref, min) {
  barco_showErrMsg(ref, tJ('invalidMinChar1'), min,
    tJ('invalidMinChar2'), tJ_plural(min), '.');
}

function barco_errAtMaxNChars(ref, max) {
  barco_showErrMsg(ref, tJ('invalidMaxChar1'), max,
    tJ('invalidMaxChar2'), tJ_plural(max), '.');
}

function barco_errDevInitialLowerCase(ref) {
  barco_showErrMsg(ref, tJ('invalidDevInitialLowercase'), '', '', '', '.');
}

function barco_errDevnameInvalid(ref, min) {
  barco_showErrMsg(ref, tJ('invalidMinChar1'), min,
    tJ('invalidMinChar2'), tJ_plural(min), tJ('invalidDevLowercase'));
}

function barco_errPwInvalid(ref) {
  barco_showErrMsg(ref, tJ('invalidPwFormat'), '', '', '', '.');
}

function barco_errPwNoMatch(ref) {
  barco_showErrMsg(ref, tJ('invalidPwMatch1'), '',
    tJ('invalidPwMatch2'), '', '.');
}

function barco_errPwIncorrect(ref) {
  barco_showErrMsg(ref, tJ('invalidPwd'), '',
    tJ('invalidPwMatch2'), '', '.');
}

function barco_errPwEmpty(ref) {
  barco_showErrMsg(ref, tJ('invalidPwEmpty'), '', '', '', '.');
}

function barco_warnNtpBlank(ref) {
  barco_showErrMsg(ref, tJ('warningNtpBlank1'), tJ('warningNtpBlank2'),
    tJ('warningNtpBlank3'), '.', '');
}

function barco_errIPv4Incomplete(ref) {
  barco_showErrMsg(ref, tJ('invalidIPv4Incomp1'), tJ('invalidIPv4Incomp2'),
    tJ('invalidIPv4Incomp3'), tJ('invalidIPv4Incomp4'), '.');
}

function barco_errIPv4OutOfRange(ref, cnt) {
  barco_showErrMsg(ref, cnt, tJ('invalidIPv4Range1'), tJ_plural_is(cnt),
    tJ('invalidIPv4Range2'), tJ('invalidIPv4Range3'));
}

function barco_errIPv4MaskGeneric(ref) {
  barco_showErrMsg(ref, tJ('invalidMaskGeneric1'), tJ('invalidMaskGeneric2'),
    tJ('invalidMaskGeneric3'), tJ('invalidMaskGeneric4'), '.');
}

function barco_errIPv4MaskAllOnes(ref, binaryFmt) {
  barco_errIPv4MaskGeneric(ref);
//  barco_showErrMsg(ref, 'Network mask has ',
//    'no host part ', '(no 0\'s): ', binaryFmt, '.');
}

function barco_errIPv4MaskAllZeros(ref, binaryFmt) {
  barco_errIPv4MaskGeneric(ref);
//  barco_showErrMsg(ref, 'Network mask has ',
//    'no subnet part ', '(no 1\'s): ', binaryFmt, '.');
}

function barco_errIPv4MaskHasHoles(ref, binaryFmt) {
  barco_errIPv4MaskGeneric(ref);
 // barco_showErrMsg(ref, 'Network mask contains ',
 //   'holes: ', '', binaryFmt, '.');
}

/* ============================================================================
 * Form submit and response error handling
 * ==========================================================================*/

var barco_timeoutHandle = null;
var barco_submitQuery = '';
var barco_modalShowing = false;
var barco_responseReceived = 0;
var barco_dms_status = 'UNKNOWN';
var barco_dms_results = '';
var barco_rebootTimeoutHandle = null;
var barco_rebootWatchdogHandle = null;
var barco_boardtype = "StreamSrc";
var barco_newURL = '';
var dmsAutoMode = true;

function barco_addSubmitQuery(prefix, name) {
  var input = $('input[name="' + name + '"]');
  var inputType = input.attr('type');
  var val = encodeURIComponent(input.val());
  var query = prefix + name + '=';
  if (inputType === 'checkbox') {
    var checked = input.prop('checked');
    if (val.length === 0) val = 'on';
    query +=  checked ? val : 'off';
  }
  else if (inputType === 'radio') {
    var input = $('input[name="' + name + '"]:checked');
    val = encodeURIComponent(input.val());
    query += val;
  } else {
    query += val;
  }
  return query;
}

function barco_delayedChangedIPCase(evt) {
  barco_dms_status = "CHANGEADDR";
  barco_ngsStatusHandler();
}

function barco_handleChangedIPCase() {
  var input = $('input[name="if_list_ipaddress"]');
  if (barco_urls.testNewIPaddress !== undefined)
    barco_newURL = window.location.protocol + '//' + barco_urls.testNewIPaddress;
  else
    barco_newURL = window.location.protocol + '//' + input.val();
  window.setTimeout(barco_delayedChangedIPCase, barco_timeIntervals.rebootTimout2);
}

function barco_addSubmitQueryForDeviceName(prefix, name) {
  var item = $('input[name="' + name + '"]');
  var val = encodeURIComponent(item.val());
  var query = prefix + name + '=';
  query += val;
  var tmpStr = "&cli_xmpp_agent=1&xmpp_agent_devicename="+val;
  query += tmpStr;
  return query;
}

function barco_addSubmitQueryPassword() {
  var query = "";
  var password = $('div#passwordinput input').val();

  if (barco_generalSettings.password !== password) {
    query += "&cli_admin=1&admin_password=" + password;
	var tmpStr = "&cli_xmpp_agent=1&xmpp_agent_password="+password;
	query += tmpStr;
  }

  return query;
}

function barco_addSubmitQueryNetwork(prefix) {
  var query = "";
  var inputCLI = $('input[name="cli_if_list"]');
  var inputDHCP = $('div#dhcp input');
  var inputIPAddress = $('div#ipaddress input');
  var inputNetmask = $('div#netmask input');
  var inputGateway = $('div#gateway input');
  var useDHCP = inputDHCP.prop('checked');
  if (barco_networkSettings.dhcp !== useDHCP) {
    barco_setCLIActiveVal(inputCLI);
  }
  query += barco_addSubmitQuery(prefix, 'cli_if_list') +
    barco_addSubmitQuery('&', 'if_list_index') +
    barco_addSubmitQuery('&', 'if_list_checkbox_usedhcp');
  if (!useDHCP) {
    var ipaddress = inputIPAddress.val();
    var netmask = inputNetmask.val();
    var gateway = inputGateway.val();
    if (barco_networkSettings.ipaddress !== ipaddress ||
        barco_networkSettings.netmask !== netmask) {
      barco_setCLIActiveVal(inputCLI);
      query += barco_addSubmitQuery('&', 'if_list_ipaddress') +
        barco_addSubmitQuery('&', 'if_list_netmask');
    }
    if (barco_networkSettings.gateway !== gateway) {
      query += barco_addSubmitQuery('&', 'cli_route_entry') +
        barco_addSubmitQuery('&', 'route_entry_dst') +
        barco_addSubmitQuery('&', 'route_entry_mask') +
        barco_addSubmitQuery('&', 'route_entry_gw');
      if (barco_networkSettings.gateway === 'GATEWAY' ||
          barco_networkSettings.gateway === '') {
        query += '&route_entry_index=0'; // add
      } else {
        query += '&route_entry_index=1'; // modify 1st entry
      }
    }
    if (barco_networkSettings.ipaddress !== ipaddress ||
        barco_networkSettings.dhcp === true)
      barco_handleChangedIPCase();
  }
  return query;
}

function barco_addSubmitQueryMSConn(prefix) {
  var query = "";
  
  if(barco_isDms)
	query += createDmsQuery();
  else
	query += createComboQuery();

  return query;
}

function createDmsQuery()
{
	var query="" ;
	var prefix = "&" ;
	query += prefix + 'cli_xmpp_server=1&xmpp_server_checkbox_enable=off';
	var scipaddress = $('div#scipaddress input').val();
	var scipaddressChanged = scipaddress !== barco_MsSettings_dms.scipaddress;
	var ntpclientdms = $('div#ntpclientdms input').val();
	var autoDetect = $('div#msautodetect input').prop('checked');
	var adminPassword = $('div#passwordinput input').val();
	
	var ntpClientIpChanged = ntpclientdms !== barco_MsSettings_dms.ntpClientIp; 
	var autoDetectChanged = autoDetect != barco_MsSettings_dms.autoDetect;
	
	query += prefix + 'cli_xmpp_agent';
	
	if(scipaddressChanged)
	{
		query += prefix + 'xmpp_agent_checkbox_enable=on' + prefix + 'xmpp_agent_serverip=' + scipaddress; 
	}

	if(autoDetect)
	{
		query += prefix + 'xmpp_agent_checkbox_enable=off';  
	}
	
	// update the device password same as the admin password.
	//query += prefix + "xmpp_agent_password=" +adminPassword;
	
	if(ntpClientIpChanged)
	{
	  var disableNtpServer = '&cli_ntp_server=1&ntp_server_checkbox_enable=off';
	  var enableNtp = '&cli_ntp_client=1&ntp_client_checkbox_enable=on';
	  
	  query += disableNtpServer + enableNtp;
	  query += '&ntp_client_ipaddress=' + ntpclientdms;
	  
	  // Fix for the reboot of Device if NTP IP is changed
	  query +="&ntp_dmsipaddressforreboot="+ntpclientdms;
	}
	  
	return query;
}

function createComboQuery()
{
  var discovery = $('div#xmpp_server input').prop('checked');
  var accounts = $('div#accounts input').prop('checked');
  var ntp = $('div#ntpserver input').prop('checked');
  var ntpClientIp = $('div#ntpclientcombo input').val();
  
  var query = "" ;
  var prefix = "&" ;
  var discoveryChanged = discovery !== barco_MsSettings_combo.discovery;
  var accountsChanged = accounts !== barco_MsSettings_combo.accounts;
  var ntpChanged = ntp !== barco_MsSettings_combo.ntp;
  var ntpClientIpChanged = ntpClientIp !==   barco_MsSettings_combo.ntpClientIp;
  
  //if (discoveryChanged || accountsChanged) {
  // Fix for VNG0001-1071: Device name set in name of device tab for MS doesn't show up the same name in Admin UI. The set_xmpp_server call should be placed always.
  query += '&cli_xmpp_agent=1&xmpp_agent_checkbox_enable=on&xmpp_agent_serverip=127.0.0.1';
  query += prefix + 'cli_xmpp_server=1&xmpp_server_checkbox_enable=on';
  //}
  if (discoveryChanged) {
    query += 
      barco_addSubmitQuery('&', 'xmpp_server_checkbox_enablediscovery');
  }
  if (accountsChanged) {
    query += 
      barco_addSubmitQuery('&', 'xmpp_server_checkbox_enableuserregistry');
  }
  
  if (ntp) {
	if (ntpChanged) {
	  var enableNtpServer = '&cli_ntp_server=1';
	  enableNtpServer += barco_addSubmitQuery('&', 'ntp_server_checkbox_enable');
	  var disableNtp = '&cli_ntp_client=1&ntp_client_checkbox_enable=off'
	  query += enableNtpServer + disableNtp;
	}
  }
  else {
	// If user updates the ipaddress
	  var enableNtp = '&cli_ntp_client=1&ntp_client_checkbox_enable=on';
	  var disableNtpServer = '&cli_ntp_server=1&ntp_server_checkbox_enable=off';
	  query += disableNtpServer + enableNtp;
	  
	  if (ntpClientIpChanged)
	  { 
		query += '&ntp_client_ipaddress=' + ntpClientIp;
		
		// Fix for the reboot of Device if NTP IP is changed
		query +="&ntp_comboipaddressforreboot="+ntpClientIp;
	  }
    }
	
  return query;	
}

/** 
function barco_addSubmitQueryMSConn(prefix) {
  var query = "";
  var msConnModeChanged = dmsAutoMode !== barco_xmpp_agent.autoMode;

  if (dmsAutoMode) {
    if (msConnModeChanged) {
      query += prefix + 'cli_xmpp_agent=1&xmpp_agent_checkbox_enable=off';
    }
  } else {
    var dmsdevicename = $('div#dmsdevicename input').val();
    var dmspassword = $('div#dmspassword input').val();
    var scipaddress = $('div#scipaddress input').val();
    var dmsdevicenameChanged = dmsdevicename !== barco_xmpp_agent.dmsdevicename;
    var dmspasswordChanged = dmspassword !== barco_xmpp_agent.dmspassword;
    var scipaddressChanged = scipaddress !== barco_xmpp_agent.scipaddress;
    if (msConnModeChanged || dmsdevicenameChanged ||
        dmspasswordChanged || scipaddressChanged) {
      query += prefix + 'cli_xmpp_agent=1';
    }
    if (msConnModeChanged) {
      query += '&xmpp_agent_checkbox_enable=on';
    }
    if (dmsdevicenameChanged) {
      query += 
        barco_addSubmitQuery('&', 'xmpp_agent_devicename');
    }
    if (dmspasswordChanged) {
     query += 
        barco_addSubmitQuery('&', 'xmpp_agent_password');
     }
    if (scipaddressChanged) {
     query += 
        barco_addSubmitQuery('&', 'xmpp_agent_serverip');
    }
  }
  return query;
}

function barco_addSubmitQueryOtherMode(prefix) {
  var discovery = $('div#xmpp_server input').prop('checked');
  var accounts = $('div#accounts input').prop('checked');
  var query = "";
  var xmppServEnableChanged =
    barco_xmpp_server.enabled_now !== barco_xmpp_server.enabled;
  var discoveryChanged = discovery !== barco_otherSettings.discovery;
  var accountsChanged = accounts !== barco_otherSettings.accounts;
  if (xmppServEnableChanged || discoveryChanged || accountsChanged) {
    query += prefix + 'cli_xmpp_server=1';
  }
  if (xmppServEnableChanged) {
    query += '&xmpp_server_checkbox_enable=' +
      (barco_xmpp_server.enabled_now ? "on" : "off");
    if (barco_xmpp_server.enabled_now) {
      query += '&cli_xmpp_agent=1&xmpp_agent_checkbox_enable=on&xmpp_agent_serverip=127.0.0.1';
    }
  }
  if (discoveryChanged) {
    query += 
      barco_addSubmitQuery('&', 'xmpp_server_checkbox_enablediscovery');
  }
  if (accountsChanged) {
    query += 
      barco_addSubmitQuery('&', 'xmpp_server_checkbox_enableuserregistry');
  }
  return query;
}
} */

function barco_NGSisBack(response, status) {
  window.clearInterval(barco_rebootTimeoutHandle);
  window.clearTimeout(barco_rebootWatchdogHandle);

  if (!barco_isDms)
	checkLink(barco_newURL + barco_urls.success);
  else
  {
	$('#myModal').modal('hide');
	window.location.href = barco_newURL + barco_urls.controllerLink;
  }
}

function barco_NGSCheckIfRebooted(evt) {
  $.ajax({  
    type: "HEAD",  
    url: barco_newURL + barco_urls.success,
    success: barco_NGSisBack
  });  
}

function barco_NGSRebootDelay(evt) {
  window.clearTimeout(barco_rebootTimeoutHandle);
  barco_rebootTimeoutHandle =
    window.setInterval(barco_NGSCheckIfRebooted, barco_timeIntervals.rebootTimout2);
  barco_NGSCheckIfRebooted(evt);
}

function barco_rebootWatchdog() {
  window.clearTimeout(barco_rebootTimeoutHandle);
  window.clearTimeout(barco_rebootWatchdogHandle);
  $('#myModal h3').html(tJ('statusWatchdog'));
}

function barco_ngsStatusHandler() {
  if (barco_dms_results.length > 0) 
  {
    $('#myModal h3').html(tJ('errorServer1') + '<ul>' + barco_dms_results +
      '</ul>' + tJ('errorServer2'));  
  } 
  else if (barco_dms_status === "OK") 
  { 
	// success
	if (!barco_isDms)
	  checkLink(barco_newURL + barco_urls.success);
	else
	{
	  $('#myModal').modal('hide');
	  window.location.href = barco_newURL + barco_urls.controllerLink;
	}
  }	
  else if (barco_dms_status === "REBOOT") 
  {
    $('#myModal h3').html(tJ('statusReboot'));
    barco_rebootTimeoutHandle =
      window.setTimeout(barco_NGSRebootDelay, barco_timeIntervals.rebootTimout1);
    barco_rebootWatchdogHandle =
      window.setTimeout(barco_rebootWatchdog, barco_timeIntervals.rebootFailTimeout);
  } 
  else if (barco_dms_status === "CHANGEADDR") 
  {
    if (!barco_isDms) 
	{
      $('#myModal h3').html(tJ('statusChangeAddr') + '<a href="' +
        barco_newURL + barco_urls.success + '">' + barco_newURL +
        barco_urls.success + '</a>');
    } 
	else 
	{
      $('#myModal h3').html(tJ('statusChangeAddr') + '<a href="' +
        barco_newURL + barco_urls.controllerLink + '">' + barco_newURL +
        barco_urls.controllerLink + '</a>');
    }
    barco_rebootTimeoutHandle =
      window.setTimeout(barco_NGSRebootDelay, barco_timeIntervals.rebootTimout1);
  }
}

function barco_ngsSystemErrorHandler() {
  $('#myModal h3').html(tJ('errorLost'));
}

function barco_modalShowingEvt(evt) {
  $('#myModal').off('shown', barco_modalShowing);
  barco_modalShowing = true;
  if (barco_responseReceived === 1) {
    barco_ngsStatusHandler();
  }
  if (barco_responseReceived === 2) { // error
    barco_ngsSystemErrorHandler();
  }
}

function checkLink(redirectionUrl)
{
   //barco_checkLinkTimer = setInterval(checkControllerLink, barco_timeIntervals.checkLinkInterval);
   barco_checkLinkTimer = setInterval( function() { checkControllerLink(redirectionUrl); }, barco_timeIntervals.checkLinkInterval );
}

function checkControllerLink(redirectionUrl)
{
  var pingUrl;
  
  if (!barco_isDms) 
	pingUrl = barco_urls.clientLink; 
  else 
	pingUrl = barco_urls.controllerLink;

	$.ajax({
	url: pingUrl,
	type : "HEAD" ,
	timeout: barco_timeIntervals.checkLinkTimeout,
        success: function (response) {
			// Stop checking the link once available
			clearInterval(barco_checkLinkTimer);	
			$('#myModal').modal('hide');
			window.location.href = redirectionUrl;
		},
		
		error: function (response) {
			// Continue trying
        }      
	});
}

function barco_findResultsInHTML() {
  // Look for <results>...</results> even across multiple lines
  var str = barco_submitResponse.match(/<results[^>]*>[\s\S]*?<\/results>/i);
  if (str === null) return 'NO MATCH';
  // console.log(str);
  var xmlDoc = $.parseXML(str[0]);
  var xml = $(xmlDoc);
  var resultsText="";
  xml.find('clicmd').each(function() {
    var clicmd = $(this).attr('name');
    $(this).find('group').each(function() {
      var title = $(this).attr('name');
      var gResponse = $(this).children().eq(0);
      var statusAttr = gResponse.children().eq(0);
      var reasonAttr = gResponse.children().eq(1);
      var status = statusAttr.attr('value');
      var reason = reasonAttr.attr('value');
      if (status !== "Success") {
        resultsText += '<li>' + title + ' ' + status + ": " + reason + '.</li>';
      }
    });
  });
  return resultsText;
}

function barco_findStatusInHTML() {
  // Look for <status>...</status> even across multiple lines
  var str = barco_submitResponse.match(/<status[^>]*>[\s\S]*?<\/status>/i);
  if (str === null) return 'NO MATCH';
  var xmlDoc = $.parseXML(str[0]);
  var xml = $(xmlDoc);
  var tag = xml.find('status');
  // Strip all whitespace and line terminators from <status> tag text
  var msg = tag.text().replace(/\s*/g, '');
  return msg;
}

function barco_formResponseGood() {
  barco_dms_status = barco_findStatusInHTML();
  barco_dms_results = barco_findResultsInHTML();
  barco_responseReceived = 1;

  if (barco_modalShowing) {
    barco_ngsStatusHandler();
  }
}

function barco_formResponseBad(request, status, error) {
  barco_responseReceived = 2;
  if (barco_modalShowing)
    barco_ngsSystemErrorHandler();
}

function barco_formSubmit(evt) {
  window.clearTimeout(barco_timeoutHandle);
  barco_submitQuery = barco_addSubmitQuery('', 'cli_dns_client') +
	barco_addSubmitQueryForDeviceName('&', 'dns_client_hostname')+
    barco_addSubmitQueryPassword() +
    barco_addSubmitQueryNetwork('&') +
    barco_addSubmitQueryMSConn('&');
    //barco_addSubmitQueryOtherMode('&');
    //console.log(barco_submitQuery);
  barco_responseReceived = 0;
  $.ajax({  
    type: "POST",  
    url: barco_urls.setup,
    // dataType: 'xml',
    data: barco_submitQuery,  
    success: function(response, status){
		barco_submitResponse = response;
		barco_formResponseGood();
    },
	error: barco_formResponseBad
  });
}

/* ============================================================================
 * Common Pane Button Actions
 * ==========================================================================*/

function barco_doPreviousAction () {
  barco_errCnt = 0;
  
  var idx = barco_paneIdToIdx(barco_getPaneId());
  var j = barco_paneIdToIdx('other');
  var idxPrev = idx === j ? idx-2 : idx-1;
  barco_changePanes(idx, idxPrev);
}

function barco_doNextAction () {
  barco_errCnt = 0;

  if (barco_allPanesValid() && barco_doneBtn) {
    barco_timeoutHandle = window.setTimeout(barco_formSubmit, 0);
    barco_modalShowing = false;
    $('#myModal').on('shown', barco_modalShowingEvt);
    $('#myModal').modal({show:true, backdrop:'static'});
  }
  var idx = barco_paneIdToIdx(barco_getPaneId());
  var j = barco_paneIdToIdx('network');
  var idxNext = barco_xmpp_server.enabled_now && idx === j ? idx+2 : idx+1;
  barco_changePanes(idx, idxNext);
}

/* ============================================================================
 * General Pane Validation
 * ==========================================================================*/

function barco_devicenameRule(val) {
  return val.length >= barco_devnam_range.min;
}

function barco_maybeShowDevErrorMsg(valid) {
  if (valid) {
    barco_hideErrMsg('div#devicename');
  } else {
    barco_errDevnameInvalid('div#devicename', barco_devnam_range.min);
  }
}

function barco_maybeShowInitialDevErrorMsg(valid) {
  if (valid) {
    barco_hideErrMsg('div#devicename');
  } else {
    barco_errDevInitialLowerCase('div#devicename');
  }
}

function barco_maybeEnableNextforGeneral(val) {
  var valid = barco_devicenameRule(val);
  barco_maybeShowDevErrorMsg(valid);
  barco_enableNextBtn(valid);
  barco_setControlGroupState('div#devicename', valid);
}

function barco_devicenameCharFilter() {
  var self = $(this);
  var val = self.val();
  if (val.match(/^[^a-z]/)) {
    self.val('');
    return;
  }
  var filter = val.replace(/[^a-z0-9]/g,
                           function(str) { return ''; } );
  self.val(filter);
  barco_maybeEnableNextforGeneral(self.val());
}

function barco_devicenameInitialValid(i) {
  var devName = $('div#devicename input').val();
  var upperCase = devName.match(/[A-Z]/g);
  var valid = barco_devicenameRule(devName) && (upperCase == null);
  barco_panes[i].valid[0] = valid;
  barco_maybeShowInitialDevErrorMsg(valid);
  barco_enableNextBtn(valid);
  barco_setControlGroupState('div#devicename', valid);
  barco_changePanes(i, i);
}

function barco_devicenameValid() {
  var i = barco_paneIdToIdx('general');
  var devName = $(this).val();
  var upperCase = devName.match(/[A-Z]/g);
  var valid = barco_devicenameRule(devName) && (upperCase == null);
  barco_panes[i].valid[0] = valid;
  barco_maybeShowDevErrorMsg(valid);
  barco_setControlGroupState('div#devicename', valid);
  barco_changePanes(i, i);
}

function barco_maybeChangeNavigation() {
  var xmpp_on = $('select#device_type option:selected').val() === 'on';
  barco_xmpp_server.enabled_now = xmpp_on;
  if (barco_xmpp_server.enabled_now) {
    $('li#wiz4 span.hidden-phone').html('Other Options&nbsp;&nbsp;');
  } else {
    $('li#wiz4 span.hidden-phone').html('Management Server&nbsp;&nbsp;');
  }
}

function barco_generalValidation() {
  barco_xmpp_server.enabled = $('select#device_type option:selected').val() === 'on';
  barco_maybeChangeNavigation();
  $('div#devicename input').on("blur", barco_devicenameValid);
  $('div#devicename input').on("keyup", barco_devicenameCharFilter);
  $('select#device_type').on("change", barco_maybeChangeNavigation);
  barco_maybeEnableNextforGeneral($('div#devicename input').val());
  barco_panes[0].valid[0] = barco_devicenameInitialValid(0);
}


/* ============================================================================
 * Passward Pane Validation
 * ==========================================================================*/

var barco_pw_confirmed = false;

function barco_passwordRule(min, max, pw1, pw2) {
  var invalidChars = pw1.match(/[^A-Za-z0-9]/g) != null;
  var lowerCase = true; // pw1.match(/[a-z]/g) != null;
  var upperCase = pw1.match(/[A-Z]/g) != null;
  var digits = pw1.match(/[0-9]/g) != null
  return (pw1.length >= min) && (pw1.length <= max) && (pw1 === pw2) && lowerCase &&
          !invalidChars && upperCase && digits;
}

function barco_maybeShowPWErrorMsg(min, max, pw1, pw2) {
  if (pw1.length < min) {
    barco_errAtLeastNChars('div#passwordinput', min);
    barco_setControlGroupState('div#passwordinput', false);
    return;
  }
  if (pw1.length > max) {
    barco_errAtMaxNChars('div#passwordinput', max);
    barco_setControlGroupState('div#passwordinput', false);
    return;
  }
  if (!barco_passwordRule(min, max, pw1, pw1)) {
    barco_errPwInvalid('div#passwordinput');
    barco_setControlGroupState('div#passwordinput', false);
    return;
  }
  if (pw1 !== pw2 && barco_pw_confirmed) {
    barco_errPwNoMatch('div#passwordconfirm');
    barco_setControlGroupState('div#passwordconfirm', false);
    return;
  }
  barco_hideErrMsg('div#passwordconfirm');
  barco_hideErrMsg('div#passwordinput');
  barco_setControlGroupState('div#passwordinput', true);
}

function barco_passwordValidate() {
  var pw1 = $('div#passwordinput input').val();
  var pw2 = $('div#passwordconfirm input').val();
  // if confirm hasn't been filled-in yet we don't show an error
  if (pw2.length > 0) barco_pw_confirmed = true;
  var i = barco_paneIdToIdx('password');
  barco_maybeShowPWErrorMsg(barco_passwd_range.min, barco_passwd_range.max, pw1, pw2);
  barco_panes[i].valid[0] = barco_passwordRule(barco_passwd_range.min, barco_passwd_range.max, pw1, pw2);
  //barco_changePanes(i, i);
  barco_maybeEnableNextforPassword();
}

function barco_maybeEnableNextforPassword() {
  var pw1 = $('div#passwordinput input').val();
  var pw2 = $('div#passwordconfirm input').val();
  var valid = barco_passwordRule(barco_passwd_range.min, barco_passwd_range.max, pw1, pw2);
  if (valid) {
    barco_setControlGroupState('div#passwordinput', true);
    barco_setControlGroupState('div#passwordconfirm', true);
  }
  barco_enableNextBtn(valid);
}

function barco_passwordValidation() {
  barco_generalSettings.password = $('div#passwordinput input').val();
  $('div#passwordinput input').on("blur", barco_passwordValidate);
  $('div#passwordconfirm input').on("blur", barco_passwordValidate);
  $('div#passwordconfirm input').on("keyup", barco_maybeEnableNextforPassword);
}

/* ============================================================================
 * Network Pane Validation
 * ==========================================================================*/

function barco_networkRule(i) {
  var dhcpCheck = $('div#dhcp input').prop('checked');
  var vRef = barco_panes[i].valid;
  if (dhcpCheck) {
    barco_panes[i].valid[0] = true;
  } else {
    barco_panes[i].valid[0] = vRef[1] && vRef[2] && vRef[3];
  }
  return barco_panes[i].valid[0];
}

function barco_networkPaneValid(i) {
  var dhcpCheck = $('div#dhcp input').prop('checked');
  if (dhcpCheck) {
    $('div#ipaddress input').val('');
    $('div#netmask input').val('');
    $('div#gateway input').val('');
    $('div#ipaddress').removeClass('error');
    $('div#netmask').removeClass('error');
    $('div#gateway').removeClass('error');
    $('div#ipaddress p.error-msg').addClass('hidden').html('');
    $('div#netmask p.error-msg').addClass('hidden').html('');
    $('div#gateway p.error-msg').addClass('hidden').html('');
  }
  var valid = barco_networkRule(i);
  barco_changePanes(i, i);
}

function barco_networkDHCPenable(evt) {
  var init = !(evt instanceof Object);
  var inputDHCP = $('div#dhcp input');
  var inputIPAddress = $('div#ipaddress input');
  var inputNetmask = $('div#netmask input');
  var inputGateway = $('div#gateway input');
  var check = inputDHCP.prop('checked');
  if (init) {
    barco_networkSettings.dhcp = check;
    barco_networkSettings.ipaddress = inputIPAddress.val();
    barco_networkSettings.netmask = inputNetmask.val();
    barco_networkSettings.gateway = inputGateway.val();
    if (check) {
      inputIPAddress.val('');
      inputNetmask.val('');
      inputGateway.val('');
    }
  }
  if (check) {
    inputIPAddress.prop('disabled', true);
    inputNetmask.prop('disabled', true);
    inputGateway.prop('disabled', true);
  } else {
    inputIPAddress.prop('disabled', false);
    inputNetmask.prop('disabled', false);
    inputGateway.prop('disabled', false);
    inputIPAddress.val(barco_networkSettings.ipaddress);
    inputNetmask.val(barco_networkSettings.netmask);
    inputGateway.val(barco_networkSettings.gateway);
    var i = barco_paneIdToIdx('network');
    barco_panes[i].valid[1] = barco_networkSettings.ipaddress.length > 6;
    barco_panes[i].valid[2] = barco_networkSettings.netmask.length > 6;
    barco_panes[i].valid[3] = barco_networkSettings.netmask.length > 6;
  }
  if (!init) {
    var i = barco_paneIdToIdx('network');
    barco_networkPaneValid(i);
  }
}

var barco_ipv4_re1 = /^[0-9]{0,3}$/;
var barco_ipv4_re2 = /^[0-9]{1,3}\.$/;
var barco_ipv4_re3 = /^[0-9]{1,3}\.[0-9]{0,3}$/;
var barco_ipv4_re4 = /^[0-9]{1,3}\.[0-9]{1,3}\.$/;
var barco_ipv4_re5 = /^[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{0,3}$/;
var barco_ipv4_re6 = /^[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.$/;
var barco_ipv4_re7 = /^[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{0,3}$/;
var barco_ipv4_re8 = /^[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}$/;

function barco_ipv4AddrCharReplace(str) {
  var m1 = str.match(barco_ipv4_re1);
  var m2 = str.match(barco_ipv4_re2);
  var m3 = str.match(barco_ipv4_re3);
  var m4 = str.match(barco_ipv4_re4);
  var m5 = str.match(barco_ipv4_re5);
  var m6 = str.match(barco_ipv4_re6);
  var m7 = str.match(barco_ipv4_re7);
  if (m1 || m2 || m3 || m4 || m5 || m6 || m7) return str;
  return str.substr(0, str.length-1);
}

function barco_ipv4AddrCharFilter() {
  var self = $(this); // jQuery call
  var filter = barco_ipv4AddrCharReplace(self.val());
  self.val(filter);
}

function barco_ipv4AddrValidateArg(self, controlGrp) {
  var str = self.val();

  var ipv4_match = str.match(barco_ipv4_re8);
  if (!ipv4_match) {
    barco_errIPv4Incomplete(controlGrp);
    return false;
  }
  var octets = str.split('.');
  var len = octets.length;
  var bad_octets = 0;
  for (var i = 0; i < len; i++) {
    if (octets[i] > 255) bad_octets++;
  }
  if (bad_octets > 0) {
    barco_errIPv4OutOfRange(controlGrp, bad_octets);
    return false;
  }
  barco_hideErrMsg(controlGrp);
  return true;
}

function barco_ipv4ScAddrValidateArg(value, controlGrp) {
  var str = value;
  var ipv4_match = str.match(barco_ipv4_re8);
  if (!ipv4_match) {
    barco_errIPv4Incomplete(controlGrp);
    return false;
  }
  var octets = str.split('.');
  var len = octets.length;
  var bad_octets = 0;
  for (var i = 0; i < len; i++) {
    if (octets[i] > 255) bad_octets++;
  }
  if (bad_octets > 0) {
    barco_errIPv4OutOfRange(controlGrp, bad_octets);
    return false;
  }
  barco_hideErrMsg(controlGrp);
  return true;
}

function barco_ipv4IPAddrValidate() {
  var valid = barco_ipv4AddrValidateArg($(this), 'div#ipaddress'); // jQuery call
  var i = barco_paneIdToIdx('network');
  barco_panes[i].valid[1] = valid;
  barco_networkPaneValid(i);
  barco_setControlGroupState('div#ipaddress', valid);
}

function barco_markMaskValid(valid) {
  var i = barco_paneIdToIdx('network');
  barco_panes[i].valid[2] = valid;
  barco_networkPaneValid(i);
  barco_setControlGroupState('div#netmask', valid);
}

function barco_ipv4MaskValidate() {
  var self = $(this); // jQuery call
  if (!barco_ipv4AddrValidateArg(self, 'div#netmask')) {
     barco_markMaskValid(false);
     return;
  };
  var str = self.val();
  var octets = str.split('.');
  var len = octets.length;
  var binaryFmt = "";
  for (var i = 0; i < len; i++) {
    var binOctet = parseInt(octets[i]).toString(2);
    binaryFmt += "00000000".substr(0, 8-binOctet.length) + binOctet;
  }
  var lastOne = binaryFmt.lastIndexOf('1');
  var firstZero = binaryFmt.indexOf('0');
  if (firstZero === -1) {
    // barco_errIPv4MaskAllOnes('div#netmask', binaryFmt);
    barco_markMaskValid(true);
    return;
  }
  if (lastOne === -1) {
    // barco_errIPv4MaskAllZeros('div#netmask', binaryFmt);
    barco_markMaskValid(true);
    return;
  }
  if (firstZero - lastOne !== 1) {
    barco_errIPv4MaskHasHoles('div#netmask', binaryFmt);
    barco_markMaskValid(false);
    return;
  }
  // if we get this far, the field syntax is valid
  barco_hideErrMsg('div#netmask');
  barco_markMaskValid(true);
}

function barco_ipv4GWAddrValidate() {
  var valid = barco_ipv4AddrValidateArg($(this), 'div#gateway'); // jQuery call
  var i = barco_paneIdToIdx('network');
  barco_panes[i].valid[3] = valid;
  barco_networkPaneValid(i);
  barco_setControlGroupState('div#gateway', valid);
}

function barco_maybeEnableNextforNetwork() {
  var i = barco_paneIdToIdx('network');
  var valid = barco_networkRule(i);
  barco_enableNextBtn(valid);
}

function barco_networkValidation() {
  var i = barco_paneIdToIdx('network');
  barco_panes[i].valid[0] = true;
  $('div#ipaddress input').on("keyup", barco_ipv4AddrCharFilter);
  $('div#netmask input').on("keyup", barco_ipv4AddrCharFilter);
  $('div#gateway input').on("keyup", barco_ipv4AddrCharFilter);
  $('div#dhcp input').on("change", barco_networkDHCPenable);
  $('div#ipaddress input').on("blur", barco_ipv4IPAddrValidate);
  $('div#netmask input').on("blur", barco_ipv4MaskValidate);
  $('div#gateway input').on("blur", barco_ipv4GWAddrValidate);
}


/* ============================================================================
 * MS Connection Pane Validation
 * ==========================================================================*/

/**function selectMSConnMode() {
  var selectedMode = $(this).attr("id");
  if (selectedMode === 'automode')
    dmsAutoMode = true;
  else if (selectedMode === 'manualmode')
    dmsAutoMode = false;
}

function barco_msConnRule(i) {
  var vRef = barco_panes[i].valid;
  var devName = $('div#dmsdevicename input').val();
  vRef[1] = barco_devicenameRule(devName);
  barco_panes[i].valid[0] = vRef[1] && vRef[2] && vRef[3];
  return barco_panes[i].valid[0];
}

function barco_dmsDevicenameCharFilter() {
  var self = $(this);
  var val = self.val();
  if (val.match(/^[^a-z]/)) {
    self.val('');
    return;
  }
  var filter = val.replace(/[^a-z0-9]/g,
                           function(str) { return ''; } );
  self.val(filter);
  barco_maybeEnableNextforMsConn();
}

function barco_dmsDevicenameValid() {
  var i = barco_paneIdToIdx('msconn');
  var valid = barco_devicenameRule($(this).val());
  barco_panes[i].valid[1] = valid;
  barco_maybeShowDevErrorMsg(valid);
  barco_setControlGroupState('div#dmsdevicename', valid);
  var valid = barco_msConnRule(i);
  barco_changePanes(i, i);
  barco_maybeEnableNextforMsConn();
}

function barco_maybeShowDMSErrorMsg(min, pw) {
  if (pw.length < min) {
    barco_errAtLeastNChars('div#dmspassword', min);
    barco_setControlGroupState('div#dmspassword', false);
    return;
  }
  if (!barco_passwordRule(min, pw, pw)) {
    barco_errPwInvalid('div#dmspassword');
    barco_setControlGroupState('div#dmspassword', false);
    return;
  }
  barco_hideErrMsg('div#dmspassword');
  barco_setControlGroupState('div#dmspassword', true);
}

function barco_dmsPasswordValidate() {
  var pw = $('div#dmspassword input').val();
  var i = barco_paneIdToIdx('msconn');
  var valid = barco_passwordRule(barco_passwd_range.min, pw, pw);
  barco_maybeShowDMSErrorMsg(barco_passwd_range.min, pw);
  barco_panes[i].valid[2] = valid;
  barco_msConnRule(i);
  barco_changePanes(i, i);
  barco_maybeEnableNextforMsConn();
}
*/

function barco_ipv4SCAddrValidate() {
  var srcValue = $('div#scipaddress input').val();
  var valid = barco_ipv4ScAddrValidateArg(srcValue, 'div#scipaddress'); // jQuery call
  var i = barco_paneIdToIdx('msconn');
  barco_setControlGroupState('div#scipaddress', valid);
  
  return valid;
}

/**function barco_maybeEnableNextforMsConn() {
  var i = barco_paneIdToIdx('msconn');
  var valid = barco_msConnRule(i);
  barco_enableNextBtn(valid);
  $('#btn_next').html('Done');
  $('#btn_next').addClass('btn-primary');
}

function barco_msConnValidation() {
  var i = barco_paneIdToIdx('msconn');
  var msConnMode = $('div#MSMode a');
  var dmsdevicename = $('div#dmsdevicename input');
  var dmspassword = $('div#dmspassword input');
  var scipaddress = $('div#scipaddress input');
  barco_xmpp_agent.dmsdevicename = dmsdevicename.val();
  barco_xmpp_agent.dmspassword = dmspassword.val();
  barco_xmpp_agent.scipaddress = scipaddress.val();
  // Mark all fields valid as they come from the server
  barco_panes[i].valid[0] = true;
  barco_panes[i].valid[1] = true;
  barco_panes[i].valid[2] = true;
  barco_panes[i].valid[3] = true;

  // Set the Mode to Auto-Discovery or Manual based on the value retrieved from the server
  barco_xmpp_agent.autoMode = ($('input[name="dms_msmanual"]').val()=="Yes") ? false : true ;
  dmsAutoMode  = barco_xmpp_agent.autoMode;

  if (barco_xmpp_agent.autoMode)
  		$('div#MSMode #automode').trigger('click');
  else
		$('div#MSMode #manualmode').trigger('click');

  msConnMode.on("click", selectMSConnMode);
  dmsdevicename.on("blur", barco_dmsDevicenameValid);
  dmsdevicename.on("keyup", barco_dmsDevicenameCharFilter);
  dmspassword.on("blur", barco_dmsPasswordValidate);
  scipaddress.on("keyup", barco_ipv4AddrCharFilter);
  scipaddress.on("blur", barco_ipv4SCAddrValidate);
}*/

/* ============================================================================
 * Other Options Validation
 * ==========================================================================*/

function barco_MsSettingsValidation() {
  var i = barco_paneIdToIdx('msconn');
  barco_panes[i].valid[0] = true;
  barco_MsSettings_combo.discovery = $('div#xmpp_server input').prop('checked');
  barco_MsSettings_combo.accounts = $('div#accounts input').prop('checked');
  barco_MsSettings_combo.ntpClientIp = $('div#ntpclientcombo input').val();
  barco_MsSettings_combo.ntp = $('div#ntpserver input').prop('checked');
  barco_MsSettings_dms.scipaddress = $('div#scipaddress input').val();
  barco_MsSettings_dms.autoDetect = $('div#msautodetect input').prop('checked');
  barco_MsSettings_dms.ntpClientIp = $('div#ntpclientdms input').val();
  var selectedMode = $('div#dmscheckbox input').attr("id");
  barco_isDms= (document.getElementById('dms').checked ? true : false);
  $('div#dmscheckbox input').on("change", msDmsPreferenceChanged);
  
  // CODE for PURE DMS Validation
  $('div#scipaddress input').on("keyup", barco_ipv4AddrCharFilter);
  $('div#scipaddress input').on("blur", checkPaneState);
  $('div#ntpclientdms input').on("keyup", barco_ipv4AddrCharFilter);
  $('div#ntpclientdms input').on("blur", checkPaneState);
  
  // CODE for MS DMS COMBO Validation
  $('div#ntpserver input').on("change", barco_updateNtpIpAddressField);
  $('div#msautodetect input').on("change", checkPaneState);
  $('div#ntpclientcombo input').on("keyup", barco_ipv4AddrCharFilter);
  $('div#ntpclientcombo input').on("blur", checkPaneState); 

  msDmsPreferenceChanged();
}

function checkPaneState() {
	barco_isDms= (document.getElementById('dms').checked ? true : false);
	
	if(barco_isDms)
		checkDmsState();
	else
		checkComboState();
}

function msDmsPreferenceChanged() {
	barco_isDms= (document.getElementById('dms').checked ? true : false);
	
	if(barco_isDms){
		$('div#MSAuto').addClass("active");
		$('div#MSManual').removeClass("active");
    } 
	else { 
		$('div#MSManual').addClass("active");
		$('div#MSAuto').removeClass("active");
    }
	checkPaneState();
}
// ============================== DMS Section Validation =====================================
function checkDmsState()
{
	var i = barco_paneIdToIdx('msconn');
	var isSCValid = false;
	var isNtpDmsValid = false;
	barco_MsSettings_dms.autoDetect = $('div#msautodetect input').prop('checked');
	$('div#scipaddress input').attr("disabled", barco_MsSettings_dms.autoDetect);
	
	// Check if SC Ipaddress section is valid
	if (barco_MsSettings_dms.autoDetect)
	{
		isSCValid = true;
		barco_hideErrMsg('div#scipaddress');
		barco_setControlGroupState('div#scipaddress', true);
	} else
	{
		
		barco_ipv4AddrCharReplace($('div#scipaddress input').val());
		isSCValid = barco_ipv4SCAddrValidate($('div#scipaddress input'));
	}
	
	// Check if NTP section is valid
	barco_ipv4AddrCharReplace($('div#ntpclientdms input').val());
	isNtpDmsValid = barco_ipv4NtpDmsAddrValidateArg($('div#ntpclientdms input'));

	barco_panes[i].valid[0] = isSCValid && isNtpDmsValid;
	
	if (!barco_isFirstTimeExecuted)
		barco_showEnabledDoneBtn(barco_panes[i].valid[0]);

	barco_isFirstTimeExecuted = false;
}

// Called when the Auto Detect checkbox is enabled or disabled in the DMS Section
function barco_updateScIpAddressField() {
	checkDmsState();
}

// Called when user focuses our of the DMS NTP IP Address text box
function barco_ipv4NtpDmsAddrValidate() {
	var self = $(this);
	barco_ipv4NtpDmsAddrValidateArg(self);
}

function barco_ipv4NtpDmsAddrValidateArg(self) {
  var val = self.val();
  var valid = false;
  var i = barco_paneIdToIdx('msconn');
  
  if (val.match(/^[0-9]/)) 
	valid = barco_ipv4AddrValidateArg(self, 'div#ntpclientdms'); // jQuery call
  else 
	valid = true;
  
  if (val.length === 0) {
	valid = false;
	barco_warnNtpBlank('div#ntpclientdms');
  } 
  else if (valid) {
	barco_hideErrMsg('div#ntpclientdms');
  }

  barco_panes[i].valid[0] = valid;
  barco_setControlGroupState('div#ntpclientdms', barco_panes[i].valid[0]);
  
  return valid;
}

// ============================== COMBO Section Validation =====================================
function checkComboState()
{
	var i = barco_paneIdToIdx('msconn');
	var isNtpChecked = $('div#ntpserver input').prop('checked');
	$('div#ntpclientcombo input').attr("disabled", isNtpChecked);
	
	var isNtpComboValid = false;
	
	if(isNtpChecked) {
		isNtpComboValid = true;
		barco_hideErrMsg('div#ntpclientcombo');
		barco_setControlGroupState('div#ntpclientcombo', true);
	} 
	else {
		barco_ipv4AddrCharReplace($('div#ntpclientcombo input').val());
		isNtpComboValid = barco_ipv4NtpComboAddrValidateArg($('div#ntpclientcombo input'));
	}
	
	barco_panes[i].valid[0] = isNtpComboValid;
	
	if (!barco_isFirstTimeExecuted)
		barco_showEnabledDoneBtn(barco_panes[i].valid[0]);
	
	barco_isFirstTimeExecuted = false;
}

// Called when the Enable NTP checkbox is enabled or disabled in the Combo Section
function barco_updateNtpIpAddressField() {
	checkComboState();
}

// Called when user focuses our of the DMS NTP IP Address text box
function barco_ipv4NtpComboAddrValidate() {
	var self = $(this);
	barco_ipv4NtpComboAddrValidateArg(self);
}

function barco_ipv4NtpComboAddrValidateArg(self) {
  var val = self.val();
  var valid = false;
  var i = barco_paneIdToIdx('msconn');
  barco_MsSettings_combo.ntp = $('div#ntpserver input').prop('checked');  

  if(barco_MsSettings_combo.ntp)
	valid = true;
  else 
  {
	  if (val.match(/^[0-9]/)) 
	    valid = barco_ipv4AddrValidateArg(self, 'div#ntpclientcombo'); // jQuery call
	  else 
		valid = true;
	  
	  if (val.length === 0) 
	  {
		valid = false;
		barco_warnNtpBlank('div#ntpclientcombo');
	  } 
	  else if (valid) 
	  {
		barco_hideErrMsg('div#ntpclientcombo');
	  }
  }
  barco_panes[i].valid[0] = valid;
  barco_setControlGroupState('div#ntpclientcombo', barco_panes[i].valid[0]);
  
  if (!barco_isFirstTimeExecuted)
	barco_showEnabledDoneBtn(barco_panes[i].valid[0]);
	
  return valid;
}

function barco_otherValidation() {
  var i = barco_paneIdToIdx('other');
  barco_panes[i].valid[0] = true;
  barco_otherSettings.discovery = $('div#xmpp_server input').prop('checked');
  barco_otherSettings.accounts = $('div#accounts input').prop('checked');
}

/* ============================================================================
 * Select which CLIs to invoke based on which fields the user changed
 * ==========================================================================*/

function barco_setCLIActiveVal(cliInput) {
  var cliValue = cliInput.val();
  if (cliValue.length > 0) return;
  cliInput.attr('value', '1'); // IE handles this better than .val('1')
}

function barco_setCLIActive(evt) {
  var self = $(this);
  var name = self.attr('name');
  var word = name.split('_');
  var cliname = "cli";
  var cliInput;
  var len = word.length - 1;
  for (var i = 0; i < len; i++) {
    cliname += '_' + word[i];
    var cliInput = $('input[name="' + cliname + '"]');
    if (cliInput.length > 0) break;
  }
  if (cliInput.length > 0) {
    barco_setCLIActiveVal(cliInput);
  }
}

function barco_setupCLISetInvocation() {
  $("select").on("change", barco_setCLIActive);
  $("input").on("keyup", barco_setCLIActive);
}

/* ============================================================================
 * Login Page Validations
 * ==========================================================================*/
function barco_isLoginPasswordEmpty()
{
	var pwd = $('div#loginPassword input').val();
	var isValid = false;
	
	if (pwd.length <1) {
		// If password is empty, disable the login button and show a warning message.
		isValid = false;
		$("#btn_login").addClass('disabled');
		barco_errPwEmpty('div#loginPassword');
	} 
	else {
		// If password is not empty, enable the login button and hide the warning message.
		isValid = true;
		$("#btn_login").removeClass('disabled');
		barco_hideErrMsg('div#loginPassword');
	}
	
	return isValid;
}

// Checks if the password entered by the user matches the existing admin password
function barco_validAdminPassword()
{
	var pwd = $('div#loginPassword input').val();
	var existingPwd = $('div#passwordinput input').val();

	if(pwd==existingPwd)
		return true;
		
	return false;
}

function barco_doLoginAction()
{	
	if ($("#btn_login").hasClass('disabled')) {
        return;
    }
	else
	{
		var isPwdValid=barco_validAdminPassword();
		
		if(isPwdValid)
		{	
			document.getElementById("loginpagecontainer").setAttribute("style", "margin-top:20px; display:none");
			document.getElementById("bootstrapcontainer").setAttribute("style", "margin-top:20px; display:block");
			document.getElementsByName("dns_client_hostname")[0].focus();

		} else
		{
			barco_errPwIncorrect('div#loginPassword');
			$("#btn_login").addClass('disabled');
		}
	}
}

/* ============================================================================
 * Manage Startup Behaviour
 * ==========================================================================*/

function barco_manageStartupBehaviour() {
  // Fix for VNG0001-2074
  var factoryValue = document.getElementById("factoryinfo").value;
  barco_isFactoryDefault = (factoryValue == "true" ? true : false);
  
  if(barco_isFactoryDefault)
  {
  	document.getElementById("landingpagecontainer").setAttribute("style", "margin-top:20px; display:block");
	document.getElementById("loginpagecontainer").setAttribute("style", "margin-top:20px; display:none");
	document.getElementById("bootstrapcontainer").setAttribute("style", "margin-top:20px; display:none");

	$('div#passwordinput input').prop('value',"");
	$('div#passwordconfirm input').prop('value',"");	
  }
  else
  {
  	document.getElementById("landingpagecontainer").setAttribute("style", "margin-top:20px; display:none");
	document.getElementById("loginpagecontainer").setAttribute("style", "margin-top:20px; display:block");
	document.getElementById("bootstrapcontainer").setAttribute("style", "margin-top:20px; display:none");
  }
  
  $('#btn_start').on("click", barco_doStartAction);
  $('#btn_continue').on("click", barco_showBootstrap);
  $('#btn_cancelsetup').on("click", barco_doCancelSetupAction);
  $('#btn_login').on("click", barco_doLoginAction);
  $('div#loginPassword input').on("blur", barco_isLoginPasswordEmpty); 
}

function barco_doStartAction()
{
	// Show Popup
	var healthInputVal = $('input[name="healthinformation"]').val();
	
	if(healthInputVal == "False")
		barco_showBootstrap();
	else
		$('#startModal').modal('show');
}

function barco_showBootstrap()
{
	$('#startModal').modal('hide');
	document.getElementById("bootstrapcontainer").setAttribute("style", "margin-top:20px; display:block");
	document.getElementById("landingpagecontainer").setAttribute("style", "margin-top:20px; display:none");
	document.getElementsByName("dns_client_hostname")[0].focus();
}

function barco_doCancelSetupAction()
{
	$('#startModal').modal('hide');
}

/* ============================================================================
 * Initialization
 * ==========================================================================*/

function barco_initPaneState() {
  var len = barco_panes.length;
  for (var i = 0; i < len; i++) {
    barco_panes[i].valid.push(false, false, false, false, false, false, false);
  }
  
  $('#btn_previous').on("click", barco_doPreviousAction);
  $('#btn_next').on("click", barco_doNextAction);
}

function barco_handlePopovers() {
	$('[data-toggle="popover"]').popover();

	$('body').on('click', function (e) {
		$('[data-toggle="popover"]').each(function () {
			//the 'is' for buttons that trigger popups
			//the 'has' for icons within a button that triggers a popup
			if (!$(this).is(e.target) && $(this).has(e.target).length === 0 && $('.popover').has(e.target).length === 0) {
				$(this).popover('hide');
			}
		});
	});
}
 
function barco_dms_init() {
  barco_setupLocalization('en');
  barco_initPaneState();
  barco_manageStartupBehaviour();
  barco_changePanes(1, 0);
  barco_enableNextBtn(false);
  barco_enableDoneBtn(false);
  barco_handlePopovers();
  barco_passwordValidation();
  barco_networkDHCPenable(true);
  barco_generalValidation();
  barco_networkValidation();
  // barco_msConnValidation();
  barco_MsSettingsValidation();
  // barco_otherValidation();
  barco_setupCLISetInvocation();
  
  $('input[tabindex="1"]').focus();
  $("input[data-toggle=popover]")
    .popover()
    .click(function(e) {
      e.preventDefault();
    });
  $("a[data-toggle=popover]")
    .popover()
    .click(function(e) {
      e.preventDefault();
    });	
}
