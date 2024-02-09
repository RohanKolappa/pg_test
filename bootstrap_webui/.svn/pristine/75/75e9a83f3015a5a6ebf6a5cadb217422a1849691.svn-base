/* 
 * Copyright 2013 Barco, Inc. All rights reserved.
 */

/* ============================================================================
 * Localization
 * ==========================================================================*/

var barco_ngs_setup_wizard_langs = {
    en: {
      ref: barco_ngs_setup_wizard_en,
      plural: barco_ngs_setup_wizard_en_plural,
      plural_is: barco_ngs_setup_wizard_en_plural_is
      }
};

var barco_ngs_setup_wizard_resource = null;

function barco_setupLocalization(defaultLang) {
  if (navigator.userLanguage) { // IE case
    baseLang = navigator.userLanguage.substring(0,2).toLowerCase();
  } else { // All other browsers
    baseLang = navigator.language.substring(0,2).toLowerCase();
  }
  var resource = barco_ngs_setup_wizard_langs[baseLang];
  if (resource !== undefined) {
    barco_ngs_setup_wizard_resource = resource;
  } else {
    resource = barco_ngs_setup_wizard_langs[defaultLang];
    barco_ngs_setup_wizard_resource = resource;
  }
}

function tJ(id) {
  return barco_ngs_setup_wizard_resource.ref[id];
}

function tJ_plural(num) {
  return barco_ngs_setup_wizard_resource.plural(num);
}

function tJ_plural_is(num) {
  return barco_ngs_setup_wizard_resource.plural_is(num);
}

/* ============================================================================
 * Pane Management
 * ==========================================================================*/

var barco_panes = [
  {pane:'general', wiz:'wiz1', valid: []},
  {pane:'password', wiz:'wiz2', valid: []},
  {pane:'network', wiz:'wiz3', valid: []},
  {pane:'video', wiz:'wiz4', valid: []}
];

var barco_urls = {
  setup:'/cgi-bin/wizard/setup_ngs.cgi',
  success:'/'
  // setup:'/cgi-bin/test_send.cgi',
  // success:'/cgi-bin/test_show.cgi',
  // testNewIPaddress: '10.51.49.124'
};

var barco_devnam_range = {min: 1};
var barco_passwd_range = {min: 8};
var barco_generalSettings = {
  password: ''
};

var barco_video_presets = {
  desktop: {bw: 10, imgQ: 6, profile: 'Remote Desktop Interaction'},
  hires: {bw: 40, imgQ: 7, profile: 'High Resolution Graphics'},
  video: {bw: 20, imgQ: 7, profile: 'Full Motion Video'},
  custom: {bw: 10, imgQ: 6, profile: 'Custom'}
  };
var barco_bandwidth_range = {min: 1, max: 500, step: 1};
var barco_slider_range = {min: 6, max: 10, step: 1};
var barco_rebootTimout1 = 60*1000; // 60 seconds
var barco_rebootTimout2 = 15*1000; // 15 seconds
var barco_rebootFailTimeout = 4*60*1000; // 4 minutes;

var barco_doneBtn = false;

function barco_paneIdToIdx(paneId) {
  var idx = 0;
  var len = barco_panes.length;
  while (barco_panes[idx].pane !== paneId && idx < len) idx++;
  return idx;
}

function barco_getPaneId() {
  return $("div.well .tab-pane.active").attr('id');
}

function barco_allPanesValid() {
  var len = barco_panes.length;
  var all_valid = true;
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
  var valid = barco_panes[prvIdx].valid[0] || prvIdx >= toIdx;
  if (toIdx < 0 || toIdx >= len || !valid) return;
  for (var i = 0; i < len; i++) {
    var paneObj = barco_panes[i];
    if (i === toIdx) {
      $('#' + paneObj.wiz).addClass('current');
      $('#' + paneObj.wiz + ' span.badge').addClass("badge-inverse");
      $("div.well .tab-pane#" + paneObj.pane).addClass('active');
      if (prvIdx !== toIdx)
        $("div.well .tab-pane#" + paneObj.pane + ' input[tabindex="1"]').focus();
    } else {
      $('#' + paneObj.wiz).removeClass('current');
      $('#' + paneObj.wiz + ' span.badge').removeClass("badge-inverse");
      $("div.well .tab-pane#" + paneObj.pane).removeClass('active');
    }
  }
  barco_enablePreviousBtn(toIdx > 0);
  barco_enableNextBtn(toIdx < len-1 && barco_panes[toIdx].valid[0]);
  // Special hack to enable Done btn on last page.
  var i = barco_paneIdToIdx('video');
  if (i === toIdx) barco_panes[i].valid[0] = true;
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

function barco_errPwInvalid(ref) {
  barco_showErrMsg(ref, tJ('invalidPwFormat'), '', '', '', '.');
}

function barco_errPwNoMatch(ref) {
  barco_showErrMsg(ref, tJ('invalidPwMatch1'), '',
    tJ('invalidPwMatch2'), '', '.');
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
var barco_ngs_status = 'UNKNOWN';
var barco_ngs_results = '';
var barco_rebootTimeoutHandle = null;
var barco_rebootWatchdogHandle = null;
var barco_boardtype = "StreamSrc";
var barco_newURL = '';

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

function barco_addSubmitQueryForCombo(prefix, name) {
  
  var item = $('select[name="' + name + '"]');
  var val = encodeURIComponent(item.val());
  var query = prefix + name + '=';
  query += val;
  return query;
}

function barco_addSubmitQueryForMulticast(prefix, name) {
  
  var item = $('input[name="' + name + '"]');
  var val = encodeURIComponent(item.val());
  var query = prefix + name + '=';
  query += val;
  var tmpStr = "&multicast_parm_multicast_to="+val;
  query += tmpStr;
  
  return query;
}

function barco_addSubmitQueryManagedMode(prefix) {
  var query = prefix;
  var mode = $('input[name="mngmode"]:checked').val();
  if (mode === 'managed') {
    var serverip = $('input[name="xmpp_agent_serverip"]').val();
    var device = $('input[name="dns_client_hostname"]').val();
    var password = $('input[name="admin_password"]').val();
    query += 'cli_adminop=1&adminop_checkbox_standalone=off' + 
      '&cli_xmpp_agent=1';
	if (serverip === '0.0.0.0') {
		query += '&xmpp_agent_checkbox_enable=off';
	}
	else {
		query += '&xmpp_agent_checkbox_enable=on';
	}
	query +=
	  '&xmpp_agent_serverip=' + 
      encodeURIComponent(serverip) + '&xmpp_agent_devicename=' +
      encodeURIComponent(device) + '&xmpp_agent_password=' +
      encodeURIComponent(password);
  } else {
    query += 'cli_adminop=1&adminop_checkbox_standalone=on' +
    '&cli_xmpp_agent=1&xmpp_agent_checkbox_enable=off';  
  }
  return query;
}

function barco_addSubmitQueryStreamingMode(prefix) {
  var mode = $('div.btn-group a.active').attr('href');
  var query = prefix + 'board_info_boardtype' + '=';
  // strip off the '#'
  barco_boardtype = mode.substr(1);
  if (barco_boardtype === 'StreamDst') {
    query += encodeURIComponent("StreamDst");
    query += "&cli_board_info=1";
  } else {
    query += encodeURIComponent("StreamSrc");
  }
  return query;
}

function barco_addSubmitQueryVideoProfile(prefix) {
  var query = prefix;
  var input = $('input[name="vidprofile"]:checked');
  var profile = input.val();
  query += 'v2d_streaming_profile_profile=' +
    barco_video_presets[profile].profile +
    barco_addSubmitQuery('&', 'v2d_streaming_profile_checkbox_enable_auth_table') +
    barco_addSubmitQuery('&', 'v2d_streaming_profile_channelNumber') +
    barco_addSubmitQuery('&', 'v2d_streaming_profile_bandwidth') + 'M' +
    barco_addSubmitQuery('&', 'v2d_streaming_profile_high_compression') +
    barco_addSubmitQuery('&', 'v2d_streaming_profile_low_compression') +
    ((profile === 'desktop' || barco_boardtype === "StreamDst") ? '' :
      barco_addSubmitQuery('&', 'cli_v2d_streaming_profile'));
  return query;   
}

function barco_audioModeSetup(prefix, mode) {
  var query = "";
  if (mode === "None") {
    query += prefix + "xp_audio_mixer_local_volume=0&cli_xp_audio_mixer=1";
  } else if (mode !== "Line") {
    query += prefix + 'xp_audio_mixer_audio_input' + '=' + encodeURIComponent(mode);
    query += "&cli_xp_audio_mixer=1";
  }
  return query;
}

function barco_addSubmitQueryAudioMode(prefix) {
  var query = "";
  if (barco_boardtype === 'StreamDst') {
    var mode = $('input[name="decode_audio_input"]:checked').val();
    query += barco_audioModeSetup(prefix, mode);
  } else {
    var mode = $('input[name="encode_audio_input"]:checked').val();
    query += barco_audioModeSetup(prefix, mode);
  }
  return query;
}

function barco_NGSisBack(response, status) {
  window.clearInterval(barco_rebootTimeoutHandle);
  window.clearTimeout(barco_rebootWatchdogHandle);
  $('#myModal').modal('hide');
  window.location.href = barco_newURL + barco_urls.success;
}

function barco_NGSisNotBack(jqXHR, status, error) {
  // Using jsonp to get around cross-domain issue
  if (jqXHR.status === 200) barco_NGSisBack(error, status);
}

/*
 *  Note: The rebooted check is not reliable. If we are using HTTPS with self-
 *  signed certs. where the IP address has changed, the browser will block
 *  access to the new IP address thinking site trust has been compromised.
 *  The Chrome browser won't return anything, not even a error.
 */
function barco_NGSCheckIfRebooted(evt) {
  // Need to use jsonp to get around cross-domain issues with browser and the
  // lighttpd web server that doesn't deal with cross-domain access when IP
  // address of NGS has changed.
  $.ajax({  
    method: "GET",
    dataType: 'jsonp',
    url: barco_newURL + barco_urls.success,
    success: barco_NGSisBack,
    error: barco_NGSisNotBack
  });  
}

function barco_NGSRebootDelay(evt) {
  window.clearTimeout(barco_rebootTimeoutHandle);
  barco_rebootTimeoutHandle =
    window.setInterval(barco_NGSCheckIfRebooted, barco_rebootTimout2);
  barco_NGSCheckIfRebooted(evt);
}

function barco_rebootWatchdog() {
  window.clearTimeout(barco_rebootTimeoutHandle);
  window.clearTimeout(barco_rebootWatchdogHandle);
  $('#myModal h3').html(tJ('statusWatchdog'));
}

function barco_ngsStatusHandler() {
  if (barco_ngs_results.length > 0) {
    barco_newURL = '';
    $('#myModal h3').html(tJ('errorServer1') + '<ul>' + barco_ngs_results +
      '</ul>' + tJ('errorServer2'));  
  } else if (barco_ngs_status === "OK") { // success
    $('#myModal').modal('hide');
    window.location.href = barco_newURL + barco_urls.success;
  } else if (barco_ngs_status === "REBOOT") {
    $('#myModal h3').html(tJ('statusReboot'));
    barco_rebootTimeoutHandle =
      window.setTimeout(barco_NGSRebootDelay, barco_rebootTimout1);
    barco_rebootWatchdogHandle =
      window.setTimeout(barco_rebootWatchdog, barco_rebootFailTimeout);
  } else if (barco_ngs_status === "CHANGEADDR") {
    $('#myModal h3').html(tJ('statusChangeAddr') + '<a href="' +
      barco_newURL + barco_urls.success + '">' + barco_newURL +
      barco_urls.success + '</a>');
    barco_rebootTimeoutHandle =
      window.setTimeout(barco_NGSRebootDelay, barco_rebootTimout1);
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

function barco_findResultsInHTML(response) {
  // Look for <results>...</results> even across multiple lines
  var str = response.match(/<results[^>]*>[\s\S]*?<\/results>/i);
  if (str === null) return 'NO MATCH';
  // console.log(str);
  var xmlDoc = $.parseXML(str[0]);
  var xml = $(xmlDoc);
  var resultsText="";
  xml.find('clicmd').each(function() {
    var clicmd = $(this).attr('name');
    $(this).find('Group').each(function() {
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

function barco_findStatusInHTML(response) {
  // Look for <status>...</status> even across multiple lines
  var str = response.match(/<status[^>]*>[\s\S]*?<\/status>/i);
  if (str === null) return 'NO MATCH';
  var xmlDoc = $.parseXML(str[0]);
  var xml = $(xmlDoc);
  var tag = xml.find('status');
  // Strip all whitespace and line terminators from <status> tag text
  var msg = tag.text().replace(/\s*/g, '');
  return msg;
}

function barco_formResponseGood(response, status) {
  barco_ngs_status = barco_findStatusInHTML(response);
  barco_ngs_results = barco_findResultsInHTML(response);
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

function barco_delayedChangedIPCase(evt) {
  barco_ngs_status = "CHANGEADDR";
  barco_ngsStatusHandler();
}

function barco_handleChangedIPCase() {
  var input = $('input[name="if_list_ipaddress"]');
  if (barco_urls.testNewIPaddress !== undefined)
    barco_newURL = 'https://' + barco_urls.testNewIPaddress;
  else
    barco_newURL = 'https://' + input.val();
  window.setTimeout(barco_delayedChangedIPCase, barco_rebootTimout2);
}

function barco_formSubmit(evt) {
  window.clearTimeout(barco_timeoutHandle);
  var useDHCP = $('input[name="if_list_checkbox_usedhcp"]').prop('checked');
  var ntpSet = $('div#ntpip input').val().length > 0;
  barco_submitQuery = barco_addSubmitQuery('', 'cli_dns_client') +
    barco_addSubmitQuery('&', 'dns_client_hostname') +
    barco_addSubmitQuery('&', 'cli_admin') +
    barco_addSubmitQuery('&', 'admin_password') +
	barco_addSubmitQuery('&', 'cli_tx_kbm_layout') +
	barco_addSubmitQueryForCombo('&', 'tx_kbm_layout_keyboard_layout') +

    barco_addSubmitQueryManagedMode('&') +
    barco_addSubmitQuery('&', 'cli_if_list') +
    barco_addSubmitQuery('&', 'if_list_index') +
    barco_addSubmitQuery('&', 'if_list_checkbox_usedhcp') +
    (useDHCP ? '' : barco_addSubmitQuery('&', 'if_list_ipaddress')) +
    (useDHCP ? '' : barco_addSubmitQuery('&', 'if_list_netmask')) +
    (useDHCP ? '' : barco_addSubmitQuery('&', 'cli_route_entry')) +
    (useDHCP ? '' : barco_addSubmitQuery('&', 'route_entry_index')) +
    (useDHCP ? '' : barco_addSubmitQuery('&', 'route_entry_dst')) +
    (useDHCP ? '' : barco_addSubmitQuery('&', 'route_entry_mask')) +
    (useDHCP ? '' : barco_addSubmitQuery('&', 'route_entry_gw')) +
    barco_addSubmitQuery('&', 'cli_ntp') +
    '&ntp_checkbox_enable=on' +
    barco_addSubmitQuery('&', 'ntp_ipaddress') +
    barco_addSubmitQueryStreamingMode('&') +
	barco_addSubmitQuery('&', 'cli_multicast_parm') +
	barco_addSubmitQueryForMulticast('&', 'multicast_parm_multicast_from') +
	barco_addSubmitQueryVideoProfile('&') +
    barco_addSubmitQueryAudioMode('&');
  // console.log(barco_submitQuery);
  if (!useDHCP) barco_handleChangedIPCase();
  barco_responseReceived = 0;
  barco_ngs_status = "";
  barco_ngs_results = "";
  $('#myModal h3').html(tJ("statusSubmit"));
  $.ajax({  
    type: "POST",  
    url: barco_urls.setup,
    // dataType: 'xml',
    data: barco_submitQuery,  
    success: barco_formResponseGood,
    error: barco_formResponseBad
  });
}

/* ============================================================================
 * Common Pane Button Actions
 * ==========================================================================*/

function barco_doPreviousAction () {
  barco_errCnt = 0;
  var idx = barco_paneIdToIdx(barco_getPaneId());
  barco_changePanes(idx, idx-1);
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
  barco_changePanes(idx, idx+1);
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
    barco_errAtLeastNChars('div#devicename', barco_devnam_range.min);
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
  // Allow _ and - in hostname
  var filter = val.replace(/[^a-z0-9-]/g,
                           function(str) { return ''; } );
  self.val(filter);
  barco_maybeEnableNextforGeneral(self.val());
}

function barco_devicenameValid() {
  var i = barco_paneIdToIdx('general');
  var valid = barco_devicenameRule($(this).val());
  barco_panes[i].valid[0] = valid;
  barco_maybeShowDevErrorMsg(valid);
  barco_setControlGroupState('div#devicename', valid);
  barco_changePanes(i, i);
}

function barco_generalValidation() {
  $('div#devicename input').on("blur", barco_devicenameValid);
  $('div#devicename input').on("keyup", barco_devicenameCharFilter);
  barco_panes[0].valid[0] = true;
  barco_maybeEnableNextforGeneral($('div#devicename input').val());
}

/* ============================================================================
 * Passward Pane Validation
 * ==========================================================================*/

var barco_pw_confirmed = false;

function barco_passwordRule(min, pw1, pw2) {
  var invalidChars = pw1.match(/[^A-Za-z0-9]/g);
  var lowerCase = true; // pw1.match(/[a-z]/g);
  var upperCase = pw1.match(/[A-Z]/g);
  var digits = pw1.match(/[0-9]/g);
  return (pw1.length >= min) && (pw1 === pw2) && lowerCase &&
          !invalidChars && upperCase && digits;
}

function barco_maybeShowPWErrorMsg(min, pw1, pw2) {
  if (pw1.length < min) {
    barco_errAtLeastNChars('div#passwordinput', min);
    barco_setControlGroupState('div#passwordinput', false);
    return;
  }
  if (!barco_passwordRule(min, pw1, pw1)) {
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
  barco_maybeShowPWErrorMsg(barco_passwd_range.min, pw1, pw2);
  barco_panes[i].valid[0] = barco_passwordRule(barco_passwd_range.min, pw1, pw2);
  barco_changePanes(i, i);
  barco_maybeEnableNextforPassword();
}

function barco_maybeEnableNextforPassword() {
  var pw1 = $('div#passwordinput input').val();
  var pw2 = $('div#passwordconfirm input').val();
  var valid = barco_passwordRule(barco_passwd_range.min, pw1, pw2);
  if (valid) {
    barco_setControlGroupState('div#passwordinput', true);
    barco_setControlGroupState('div#passwordconfirm', true);
  }
  barco_enableNextBtn(valid);
}

function barco_passwordFocus(evt) {
  if (evt.type === 'focus' && $(this).val() === barco_generalSettings.password) {
    $(this).val('');
  } else if (evt.type === "blur" && $(this).val() === "") {
    $(this).val(barco_generalSettings.password);
  }
}

function barco_passwordValidation() {
  barco_generalSettings.password = $('div#passwordinput input').val();
  if (barco_generalSettings.password.length >= barco_passwd_range.min) {
    var i = barco_paneIdToIdx('password');
    barco_panes[i].valid[0] = true;
  }
  $('div#passwordinput input').on("focus", barco_passwordFocus);
  $('div#passwordinput input').on("blur", barco_passwordFocus);
  $('div#passwordconfirm input').on("focus", barco_passwordFocus);
  $('div#passwordconfirm input').on("blur", barco_passwordFocus);
  $('div#passwordinput input').on("blur", barco_passwordValidate);
  $('div#passwordconfirm input').on("blur", barco_passwordValidate);
  $('div#passwordconfirm input').on("keyup", barco_maybeEnableNextforPassword);
}

/* ============================================================================
 * Network Pane Validation
 * ==========================================================================*/

function barco_networkRule(i) {
  var managedMode = ($('input[name="mngmode"]:checked').val()) === 'managed';
  var dhcpCheck = $('div#dhcp input').prop('checked');
  var strScp = $('div#scipaddress input').val();
  var strNtp = $('div#ntpip input').val();
  if (strScp.match(/^[0-9]/)) {
    barco_panes[i].valid[5] = strScp.match(barco_ipv4_re8);
  } else {
    barco_panes[i].valid[5] = strScp.length > 0;
  }
  if (strNtp.match(/^[0-9]/)) {
    barco_panes[i].valid[4] = strNtp.match(barco_ipv4_re8);
  } else {
    barco_panes[i].valid[4] = true;
  }
  var vRef = barco_panes[i].valid;
  if (dhcpCheck) {
    barco_panes[i].valid[0] = vRef[4];
    if (managedMode) barco_panes[i].valid[0] = vRef[4] && vRef[5]; 
  } else {
    barco_panes[i].valid[0] = vRef[1] && vRef[2] && vRef[3] && vRef[4];
    if (managedMode)
      barco_panes[i].valid[0] = vRef[1] && vRef[2] && vRef[3] && vRef[4] && vRef[5];
  }
  return barco_panes[i].valid[0];
}

function barco_networkPaneValid(i) {
  var standaloneMode = ($('input[name="mngmode"]:checked').val()) === 'standalone';
  var dhcpCheck = $('div#dhcp input').prop('checked');
  if (standaloneMode) {
    $('div#scipaddress').removeClass('error');
    $('div#scipaddress p.error-msg').addClass('hidden').html('');
  }
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

function barco_managedMode(evt) {
  var self = $(this);
  var val = self.val();
  if (val === 'standalone') {
    $('div#scipaddress input').prop('disabled', true);
  } else {
    $('div#scipaddress input').prop('disabled', false);
  }
  var i = barco_paneIdToIdx('network');
  barco_networkPaneValid(i);
}

function barco_networkDHCPenable(evt) {
  var init = !(evt instanceof Object);
  if (init) $('div#dhcp input').prop('checked', true);
  var check = $('div#dhcp input').prop('checked');
  if (check) {
    $('div#ipaddress input').prop('disabled', true);
    $('div#netmask input').prop('disabled', true);
    $('div#gateway input').prop('disabled', true);
  } else {
    $('div#ipaddress input').prop('disabled', false);
    $('div#netmask input').prop('disabled', false);
    $('div#gateway input').prop('disabled', false);
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

function barco_scpCharFilter() {
  var self = $(this); // jQuery call
  var val = self.val();
  var filter = null;
  if (val.match(/^[0-9]/)) {
    filter = barco_ipv4AddrCharReplace(val);
  } else {
    // filter = self.val().replace(/^[^a-z]/,
    //                             function(str) { return ''; } );
    if (val.match(/^[^a-z]/)) {
      self.val('');
      return;
    }
    var filter = val.replace(/[^a-z0-9\.]/g,
                             function(str) { return ''; } );
  }
  self.val(filter);
}

function barco_ntpCharFilter() {
  var self = $(this); // jQuery call
  var val = self.val();
  var filter = null;
  if (val.match(/^[0-9]/)) {
    filter = barco_ipv4AddrCharReplace(val);
  } else {
    // filter = self.val().replace(/^[^a-z]/,
    //                             function(str) { return ''; } );
    if (val.match(/^[^a-z]/)) {
      self.val('');
      return;
    }
    var filter = val.replace(/[^a-z0-9\.]/g,
                             function(str) { return ''; } );
  }
  if (filter.length === 0) {
    barco_warnNtpBlank('div#ntpip');
  } else {
    barco_hideErrMsg('div#ntpip');
  }
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

function barco_ipv4SCPAddrValidateArg(self) {
  var val = self.val();
  if (val.match(/^[0-9]/)) {
    var valid = barco_ipv4AddrValidateArg(self, 'div#scipaddress'); // jQuery call
  } else {
    valid = true;
  }
  var i = barco_paneIdToIdx('network');
  barco_panes[i].valid[4] = valid;
  if (val.length === 0) {
    barco_errIPv4Incomplete('div#scipaddress');
  } else if (valid) {
    barco_hideErrMsg('div#scipaddress');
  }
  barco_networkPaneValid(i);
  barco_setControlGroupState('div#scipaddress', valid);
}

function barco_ipv4SCPAddrValidate() {
  var self = $(this);
  barco_ipv4SCPAddrValidateArg(self);
}

function barco_ipv4GWAddrValidate() {
  var valid = barco_ipv4AddrValidateArg($(this), 'div#gateway'); // jQuery call
  var i = barco_paneIdToIdx('network');
  barco_panes[i].valid[3] = valid;
  barco_networkPaneValid(i);
  barco_setControlGroupState('div#gateway', valid);
}

function barco_ipv4NTPAddrValidateArg(self) {
  var val = self.val();
  if (val.match(/^[0-9]/)) {
    var valid = barco_ipv4AddrValidateArg(self, 'div#ntpip'); // jQuery call
  } else {
    valid = true;
  }
  var i = barco_paneIdToIdx('network');
  barco_panes[i].valid[4] = valid;
  if (val.length === 0) {
    // barco_warnNtpBlank('div#ntpip');
  } else if (valid) {
    barco_hideErrMsg('div#ntpip');
  }
  barco_networkPaneValid(i);
  barco_setControlGroupState('div#ntpip', valid);
}

function barco_ipv4NTPAddrValidate() {
  var self = $(this);
  barco_ipv4NTPAddrValidateArg(self);
}

function barco_ipv4MulticastValidateArg(self) {
  var val = self.val();
  if (val.match(/^[0-9]/)) {
    var valid = barco_ipv4AddrValidateArg(self, 'div#multicast'); // jQuery call
  } else {
    valid = true;
  }
  if (val.length === 0) {
    // barco_warnNtpBlank('div#multicast');
  } else if (valid) {
    barco_hideErrMsg('div#multicast');
  }
  barco_setControlGroupState('div#multicast', valid);
}

function barco_ipv4MulticastValidate() {
	var self = $(this);
	barco_ipv4MulticastValidateArg(self);
}

function barco_maybeEnableNextforNetwork() {
  barco_ipv4SCPAddrValidateArg($('div#scipaddress input'));
  barco_ipv4NTPAddrValidateArg($('div#ntpip input'));
  var i = barco_paneIdToIdx('network');
  var valid = barco_networkRule(i);
  barco_enableNextBtn(valid);
}

function barco_networkValidation() {
  var i = barco_paneIdToIdx('network');
  barco_panes[i].valid[0] = true;
  $('div#network input:radio').on("change", barco_managedMode);
  $('div#scipaddress input').on("keyup", barco_scpCharFilter);
  $('div#ipaddress input').on("keyup", barco_ipv4AddrCharFilter);
  $('div#netmask input').on("keyup", barco_ipv4AddrCharFilter);
  $('div#gateway input').on("keyup", barco_ipv4AddrCharFilter);
  $('div#ntpip input').on("keyup", barco_ntpCharFilter);
  $('div#scipaddress input').on("blur", barco_ipv4SCPAddrValidate);
  $('div#dhcp input').on("change", barco_networkDHCPenable);
  $('div#ipaddress input').on("blur", barco_ipv4IPAddrValidate);
  $('div#netmask input').on("blur", barco_ipv4MaskValidate);
  $('div#gateway input').on("blur", barco_ipv4GWAddrValidate);
  $('div#ntpip input').on("blur", barco_ipv4NTPAddrValidate);
}

function barco_streamingValidation() {
  var i = barco_paneIdToIdx('video');
  barco_panes[i].valid[0] = true;
  $('div#multicast input').on("keyup", barco_ipv4AddrCharFilter);
  $('div#multicast input').on("blur", barco_ipv4MulticastValidate);
}

/* ============================================================================
 * Video Pane Validation
 * ==========================================================================*/

var barco_videoTabs = [
  {btn:'sendVideo', tab:'StreamSrc'},
  {btn:'receiveVideo', tab:'StreamDst'}
];

var barco_old_slider_val = 0;

function barco_bwRule(intVal) {
  return intVal >= barco_bandwidth_range.min &&
         intVal <= barco_bandwidth_range.max;
}

// Not used in favor of barco_bwCharFilter2
function barco_bwCharFilter1 () {
  var self = $(this);
  var filter = self.val().replace(/[^0-9]/g,
                                    function(str) { return ''; } );
  self.val(filter);
}

function barco_bwCharFilter2() {
  var self = $(this); // jQuery call
  var val = self.val();
  if (val !== '') {
    var intVal = parseInt(val);
    if (isNaN(intVal)) intVal = 1;
    if (intVal < barco_bandwidth_range.min) intVal = barco_bandwidth_range.min;
    if (intVal > barco_bandwidth_range.max) intVal = barco_bandwidth_range.max;
    val = intVal.toString(10);
  }
  self.val(val);
}

function barco_bwValidate() {
  var i = barco_paneIdToIdx('video');
  var val = $(this).val();
  var intVal = parseInt($(this).val());
  if (intVal < barco_bandwidth_range.min) {
    $(this).val(barco_bandwidth_range.min);
  } else if (intVal > barco_bandwidth_range.max) {
    $(this).val(barco_bandwidth_range.max);
  }
  barco_panes[i].valid[0] = true;
  // barco_setControlGroupState('div#bandwidth', valid);
  // barco_changePanes(i, i);
  barco_video_presets.custom.bw = $(this).val();
}

function barco_imageQualityVal(evt, ui) {
  var val = ui.value;
  if (val !== barco_old_slider_val) {
    $('div#image-quality input').val(val);
    barco_old_slider_val = val;
  }
  barco_video_presets.custom.imgQ = val;
}

function barco_imageQualitySet(evt, ui) {
  $("#slider").slider({value: barco_old_slider_val});
}

function barco_videoPresets(evt) {
  var self = $(this);
  var val = self.val();
  var presets = barco_video_presets[val];
  if (val === 'custom') {
    $('div#bandwidth input').prop('disabled', false);
    $('#slider').slider('enable');
  } else {
    $('div#bandwidth input').prop('disabled', true);
    $('#slider').slider('disable');
  }
  // != used because presets could be null or undefined
  if (presets != null) {
    var cliInput = $('input[name="cli_v2d_streaming_profile"]');
    barco_setCLIActiveVal(cliInput);
    $('div#bandwidth input').val(presets.bw);
    $("#slider").slider({value: presets.imgQ});
    $('div#image-quality input').val(presets.imgQ);
  }
}

function barco_videoValidation() {
  var i = barco_paneIdToIdx('video');
  barco_panes[i].valid[0] = false;
  var presets = barco_video_presets['desktop'];
  $('div#bandwidth input').val(presets.bw);
  $("#slider").slider({
    slide: barco_imageQualityVal,
    stop: barco_imageQualitySet,
    min: barco_slider_range.min,
    max: barco_slider_range.max,
    step: barco_slider_range.step,
    value: presets.imgQ
  });
  $("#slider").slider('disable');
  $('div#bandwidth input').on("keyup", barco_bwCharFilter2);
  $('div#bandwidth input').on("blur", barco_bwValidate);
  $('div#image-quality input').val(presets.imgQ);
  $('div#StreamSrc input:radio').on("change", barco_videoPresets);
  // barco_changePanes(i, i);
}

function barco_addItemsToKBMLayoutDropdown() {

	var available_locales = document.getElementsByName('tx_kbm_layout')[0].value;
	var localeArray = available_locales.split(",");
	var arrLength = localeArray.length;
	
	if (localeArray[arrLength-1] == "")
		localeArray.splice(arrLength-1, 1);
	
	var defaultLocale = document.getElementsByName('tx_kbm_layout_default_value')[0].value;
	var newDropdown = document.getElementById('keyboard_locale');
	
	for (i=0; i<localeArray.length;i++)
	{
		var option=document.createElement("option");
		option.text=localeArray[i].replace(/[^a-zA-Z ]/g, "");
		newDropdown.add(option,newDropdown.options[null]);
		
		if (option.text == defaultLocale)
			newDropdown.selectedIndex = i;
	}
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
  // Commented as there is no other select element on the setup.html except the one added for keyboard locale. Not required for it.
  //$("select").on("change", barco_setCLIActive);
  $("input").on("keyup", barco_setCLIActive);
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

function barco_ngs_init() {
  barco_setupLocalization('en');
  barco_initPaneState();
  barco_changePanes(1, 0);
  barco_enableNextBtn(false);
  barco_enableDoneBtn(false);
  barco_addItemsToKBMLayoutDropdown();
  barco_networkDHCPenable(true);
  barco_generalValidation();
  barco_passwordValidation();
  barco_networkValidation();
  barco_streamingValidation();
  barco_videoValidation();
  barco_setupCLISetInvocation();
  $('input[tabindex="1"]').focus();
  $("input[data-toggle=popover]")
    .popover()
    .click(function(e) {
      e.preventDefault();
    });
}
