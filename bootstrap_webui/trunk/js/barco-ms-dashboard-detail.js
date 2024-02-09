/* 
 * Copyright 2013 Barco, Inc. All rights reserved.
 */

/* ============================================================================
 * Localization
 * ==========================================================================*/

var barco_ngs_setup_detail_langs = {
    en: { ref: barco_ngs_setup_detail_en }
};

var barco_ngs_setup_detail_resource = null;

function barco_setupLocalization(defaultLang) {
  if (navigator.userLanguage) { // IE case
    baseLang = navigator.userLanguage.substring(0,2).toLowerCase();
  } else { // All other browsers
    baseLang = navigator.language.substring(0,2).toLowerCase();
  }
  var resource = barco_ngs_setup_detail_langs[baseLang];
  if (resource !== undefined && resource.ref !== undefined) {
    barco_ngs_setup_detail_resource = resource.ref;
  } else {
    barco_ngs_setup_detail_resource =
      barco_ngs_setup_detail_langs[defaultLang].ref;
  }
}

function tK(id) {
  return barco_ngs_setup_detail_resource[id];
}

/* ============================================================================
 * Dashboard handling
 * ==========================================================================*/
var barco_video_presets = {
  desktop: {bw: '10.00M', imgQ: 6, profile: 'Remote Desktop Interaction'},
  hires: {bw: '40.00M', imgQ: 7, profile: 'High Resolution Graphics'},
  video: {bw: '20.00M', imgQ: 7, profile: 'Full Motion Video'},
  custom: {bw: '10.00M', imgQ: 6, profile: 'Custom'}
  };
var barco_bandwidth_range = {min: 1, max: 500, step: 1};
var barco_slider_range = {min: 0, max: 10, step: 1};
var barco_old_slider_val = 0;
var barco_dashboardData = {
  system_health: {
    uptime_full: '',
    uptime: '',
    memory_total: 0,
    memory_free: 0,
    memory_percent: 0,
    fan_speed: 0,
    fan_speed_good: '',
    cpu_temp: 0,
    cpu_temp_good: ''
  },
  system_info: {
    boardType: 'StreamSrc',
    streaming_mode: '',
    firmware_ver: '',
    software_ver: '',
    serial_no: '',
    device_model: ''
  },
  adjustments: {
    timestamp: 0.0,
    frames: 0.0,
    fps: '',
    videoBytes: 0.0,
    videoBW: '',
    audioBytes: 0.0,
    audioBW: '',
    revAudioBytes: 0.0,
    revAudioBW: '',
    audioSource: '',
    vidQuality: ''        
  },
  statistics: {
    status: '',
    client: '',
    server: '',
    lut: ''
  }
};
var barco_dashboardState = {
  system_health: {
    changed: true,
    user_override: false
  },
  system_info: {
    changed: true,
    user_override: false
  },
  adjustments: {
    changed: true,
    user_override: false
  },
  statistics: {
    changed: true,
    user_override: false
  }
};

/*
function barco_imageQualityVal(evt, ui) {
  var val = ui.value;
  if (val !== barco_old_slider_val) {
    $('div#image-quality input').val(val);
    barco_dashboardState.adjustments.user_override = true;
    barco_old_slider_val = val;
  }
}

function barco_imageQualitySet(evt, ui) {
  barco_dashboardState.adjustments.user_override = true;
  $("#slider").slider({value: barco_old_slider_val});
}

function barco_videoPresets(evt) {
  var self = $(this);
  var val = self.val();
  var presets = barco_video_presets[val];
  // != used because presets could be null or undefined
  if (presets != null) {
    barco_dashboardState.adjustments.user_override = true;
    $('div#bandwidth input').val(presets.bw);
    $("#slider").slider({value: presets.imgQ});
    $('div#image-quality input').val(presets.imgQ);
  }
}
*/

function barco_findParmData(xml) {
  var data = {};
  xml.find('Parameters').each(function (){
    var timestamp = $(this).attr('timestamp');
    if (timestamp != null || timestamp !== '')
      data['timestamp'] = timestamp;
    $(this).find('Parameter').each(function (){
      var name = $(this).attr('name');
      var text = $(this).text();
      data[name] = text;
    });
  });
  return data; 
}

function barco_getCpuStatus(xml) {
  var data = barco_findParmData(xml);
  var display = barco_dashboardData.system_health;
  var state = barco_dashboardState.system_health;
  for (var name in data) {
    switch (name) {
      case 'Uptime':
        display.uptime_full = data[name];
        break;
    }
  }
  var part = display.uptime_full.split(', ');
  var uptime = part[0];
  if (part.length > 1) uptime = part[0] + ', ' + part[1];
  if (display.uptime !== uptime) {
    display.uptime = uptime;
    state.changed = true;
  }
}

function barco_getMemStatus(xml) {
  var data = barco_findParmData(xml);
  var display = barco_dashboardData.system_health;
  var state = barco_dashboardState.system_health;
  for (var name in data) {
    switch (name) {
      case 'Total':
        if (display.memory_total !== data[name]) {
          display.memory_total = parseInt(data[name]);
          if (isNaN(display.memory_total)) display.memory_total = 1;
        }
        break;
      case 'Free':
        if (display.memory_free !== data[name]) {
          display.memory_free = parseInt(data[name]);
          if (isNaN(display.memory_free)) display.memory_free = 0;
        }
        break;
    }
    var total = display.memory_total;
    var free = display.memory_free;
    var percent = Math.round(100*(total - free)/total);
    if (display.memory_percent !== percent) {
      display.memory_percent = percent;
      state.changed = true;
    }
  }
}

function barco_getSoftStatus(xml) {
  var data = barco_findParmData(xml);
  var display = barco_dashboardData.system_info;
  var state = barco_dashboardState.system_info;
  for (var name in data) {
    switch (name) {
      case 'Version':
        if (display.software_ver !== data[name]) {
          display.software_ver = data[name];
          state.changed = true;
        }
        break;
      case 'Serial Number':
        if (display.serial_no !== data[name]) {
          display.serial_no = data[name];
          state.changed = true;
        }
        break;
      case 'Model':
        if (display.device_model !== data[name]) {
          display.device_model = data[name];
          state.changed = true;
        }
        break;
    }
  }
}

function barco_getFanStatus(xml) {
  var data = barco_findParmData(xml);
  var display = barco_dashboardData.system_health;
  var state = barco_dashboardState.system_health;
  for (var name in data) {
    switch (name) {
      case 'CPU':
        var temp = parseInt(data[name]);
        if (isNaN(temp)) temp = 0;
        if (display.cpu_temp !== temp) {
          display.cpu_temp = temp;
        }
        break;
      case 'Fan RPM':
        var rpm = parseInt(data[name]);
        if (isNaN(rpm)) rpm = 0;
        if (display.fan_speed !== rpm) {
          display.fan_speed = rpm;
        }
        break;
    }
  }
  var cpu_temp_good = display.cpu_temp < 100 ? 'Normal' : 'Hot';
  if (display.cpu_temp_good !== cpu_temp_good) {
    display.cpu_temp_good = cpu_temp_good;
    state.changed = true;
  }
  var fan_speed_good = 'Normal';
  if (display.fan_speed === 0 ) fan_speed_good = '?';
  else if (display.fan_speed > 120) fan_speed_good = 'High';
  if (display.fan_speed_good !== fan_speed_good) {
    display.fan_speed_good = fan_speed_good;
    state.changed = true;
  }
}

function barco_getNetPortsStatus(xml) {
  var data = barco_findParmData(xml);
  var display = barco_dashboardData.system_info;
  var state = barco_dashboardState.system_info;
  for (var name in data) {
    switch (name) {
      case 'IPAddress':
        if (display.ipaddr !== data[name]) {
          display.ipaddr = data[name];
          state.changed = true;
        }
        break;
      case 'Name':
        if (display.name !== data[name]) {
          display.name = data[name];
          state.changed = true;
        }
        break;
    }
  }
}

function barco_getFirmwareStatus(xml) {
  var data = barco_findParmData(xml);
  var display = barco_dashboardData.system_info;
  var state = barco_dashboardState.system_info;
  for (var name in data) {
    switch (name) {
      case 'Firmware Version':
        if (display.firmware_ver !== data[name]) {
          display.firmware_ver = data[name];
          state.changed = true;
        }
        break;
    }
  }
}

function barco_getIfList(xml) {
  /*
  var display = barco_dashboardData.statistics;
  var state = barco_dashboardState.statistics;
  xml.find('Attribute').each(function (){
    var name = $(this).attr('name');
    var value = $(this).attr('value');
    switch (name) {
      case 'usedhcp':
        if (display.dhcp_state !== value) {
          display.dhcp_state = value;
          state.changed = true;
        }
        return; // 1st entry only
    }
  });
  */
}

function barco_getBoardInfo(xml) {
  var display = barco_dashboardData.system_info;
  var state = barco_dashboardState.system_info;
  xml.find('Attribute').each(function (){
    var name = $(this).attr('name');
    var value = $(this).attr('value');
    switch (name) {
      case 'boardtype':
        if (display.boardType !== value) {
          display.boardType = value;
          state.changed = true;
        }
        break;
    }
  });
  var mode = '?';
  if (display.boardType === 'StreamSrc') mode = 'Encoder';
  else if (display.boardType === 'StreamDst') mode = 'Decoder';
  if (display.streaming_mode !== mode) {
    display.streaming_mode = mode;
    state.changed = true;
    if (!barco_dashboardState.adjustments.user_override)
      barco_refreshDashboardSection3(display.boardType);
    if (!barco_dashboardState.statistics.user_override)
      barco_refreshDashboardSection4(display.boardType);
    barco_sendAJAXDashboardRequest('get_rx_connection_status');
  }
}

function barco_getAuthTable(xml) {
  var display = barco_dashboardData.adjustments;
  var state = barco_dashboardState.adjustments;
  xml.find('Attribute').each(function (){
    var name = $(this).attr('name');
    var value = $(this).attr('value');
    switch (name) {
      case 'highCompression':
        if (display.vidQuality !== value) {
          display.vidQuality = value;
          state.changed = true;
        }
        return; // 1st entry only
    }
  });
}

var barco_k = 1000.0;
var barco_K = 1024.0;
var barco_m = barco_k * barco_k;
var barco_M = barco_K * barco_K;
var barco_g = barco_k * barco_m;
var barco_G = barco_K * barco_M;

function barco_setMultipler(countStr, mult) {
  var multiplier = 1.0;
  var lastChar = countStr.charAt(countStr.length - 1);
  switch (lastChar) {
    case 'k': multiplier = barco_k; break;
    case 'K': multiplier = barco_K; break;  
    case 'm': multiplier = barco_m; break;  
    case 'M': multiplier = barco_M; break;  
    case 'g': multiplier = barco_g; break;  
    case 'G': multiplier = barco_G; break;  
  }
  return multiplier * mult;  
}

function barco_toFixedDecimal(value, precision) {
  var neg = value < 0;
  var power = Math.pow(10, precision);
  var value = Math.round(value * power);
  var integral = String((neg ? Math.ceil : Math.floor)(value / power));
  var fraction = String((neg ? -value : value) % power);
  var padding = new Array(Math.max(precision - fraction.length, 0) + 1).join('0');
  return precision > 0 ? integral + '.' +  padding + fraction : integral;
}

function barco_formatNumber(value, precision) {
  var str;
  var suffix;
  if (value > barco_G) {
    value /= barco_G;
    suffix = ' G';
  } else if (value > barco_M) {
    value /= barco_M;
    suffix = ' M';
  } else if (value > barco_K) {
    value /= barco_K;
    suffix = ' k';
  } else {
    suffix = ' ';
  }
  str = barco_toFixedDecimal(value, precision);
  return str + suffix;
}

function barco_setRate(obj, countProp, rateProp, countStr, newTS, mult) {
  var rate = 0;
  if (newTS === obj['timestamp']) return false;
  if (countStr == null || countStr === '') countStr = '0';
  var count = 0;
  var multiplier = barco_setMultipler(countStr, mult);
  if (multiplier > 10.0) {
    count = parseFloat(countStr.substring(0, countStr.length - 1)) * multiplier;
  } else {
    count = parseFloat(countStr) * multiplier;
  }
  rate = (count - obj[countProp]) / (newTS - obj['timestamp']);
  obj[countProp] = count;
  if (rate < 0) return false;
  var rateStr = barco_formatNumber(rate, 2);
  var changed = rateStr !== obj[rateProp];
  if (changed) {
    obj[rateProp] = rateStr;
  }
  return changed;
}

/*
function DEBUG_DATA(obj, countProp, intv, min, max, suffix) {
  var inc = intv*(min + Math.random()*(max-min));
  switch (suffix) {
    case 'M': inc *= barco_M; break;
    case 'K': inc *= barco_K; break;
  }
  var count = obj[countProp] + inc;
  switch (suffix) {
    case 'M': count = Math.round(count/barco_M); break;
    case 'K': count = Math.round(count/barco_K); break;
  }
  return count.toString() + suffix;
}
*/

function barco_RXTXConnStatus(data, name, display, state, timestamp) {
  var changed = false;
  switch (name) {
    case 'Video Bytes':
      // data[name] = DEBUG_DATA(display, 'videoBytes',5,29,31,'M');
      changed = barco_setRate(display, 'videoBytes', 'videoBW',
        data[name], timestamp, 8);
      break;
    case 'Audio Bytes':
      // data[name] = DEBUG_DATA(display,'audioBytes',5,39,41,'K');
      changed = barco_setRate(display, 'audioBytes', 'audioBW',
        data[name], timestamp, 8);
      break;
    case 'Frames':
    case 'Video Frames':
      // data[name] = DEBUG_DATA(display,'frames',5,59,61,'');
      changed = barco_setRate(display, 'frames', 'fps',
        data[name], timestamp, 1);
      break;
    case 'Rev Audio Bytes':
    case 'Reverse Audio Bytes':
      // data[name] = DEBUG_DATA(display,'revAudioBytes',5,9000,11000,'');
      changed = barco_setRate(display, 'revAudioBytes', 'revAudioBW',
        data[name], timestamp, 8);
      break;
  }
  if (!state.changed && changed) state.changed = changed;
}

function barco_getTXConnStatus(xml) {
  var data = barco_findParmData(xml);
  var displayA = barco_dashboardData.adjustments;
  var displayS = barco_dashboardData.statistics;
  var stateA = barco_dashboardState.adjustments;
  var stateS = barco_dashboardState.statistics;
  var timestamp = parseFloat(data['timestamp'])/barco_k;
  var timestamp = parseFloat(data['timestamp'])/barco_k;
  if (isNaN(timestamp)) timestamp = displayA.timestamp;
  for (var name in data) {
    switch (name) {
      case 'Client':
        if (displayS.client !== data[name]) {
          displayS.client = data[name];
          stateS.changed = true;
        }
        break;
      case 'Status':
        if (displayS.status !== data[name]) {
          displayS.status = data[name];
          stateS.changed = true;
        }
        break;
      case 'LUT':
        if (displayS.lut !== data[name]) {
          displayS.lut = data[name];
          stateS.changed = true;
        }
        break;
      default:
        barco_RXTXConnStatus(data, name, displayA, stateA, timestamp);
    }
  }
  displayA.timestamp = timestamp;
}

function barco_getRXConnStatus(xml) {
  var data = barco_findParmData(xml);
  var displayA = barco_dashboardData.adjustments;
  var displayS = barco_dashboardData.statistics;
  var stateA = barco_dashboardState.adjustments;
  var stateS = barco_dashboardState.statistics;
  var timestamp = parseFloat(data['timestamp'])/barco_k;
  if (isNaN(timestamp)) timestamp = displayA.timestamp;
  for (var name in data) {
    switch (name) {
      case 'Server':
        if (displayS.server !== data[name]) {
          displayS.server = data[name];
          stateS.changed = true;
        }
        break;
      case 'Status':
        if (displayS.status !== data[name]) {
          displayS.status = data[name];
          stateS.changed = true;
        }
        break;
      case 'Remote LUT':
        if (displayS.lut !== data[name]) {
          displayS.lut = data[name];
          stateS.changed = true;
        }
        break;
      default:
        barco_RXTXConnStatus(data, name, displayA, stateA, timestamp);
    }
  }
  displayA.timestamp = timestamp;
}

function barco_specialCliCmdCase(xml) {
  var id = xml.find('Table').attr('name');
  if (id == null || id === '') id = xml.find('ObjectEntry').attr('name');
  // if (id === 'IFList') return 'get_if_list';
  if (id === 'AuthorizationTable') return 'get_auth_table';
  if (id === 'BoardInfo') return 'get_board_info';
  return '';
}

function barco_dashboardResponseGood(response, status) {
  var xml = $(response);
  var clicmd = xml.find('CLICommand').text();
  if (clicmd == null || clicmd === "") clicmd = barco_specialCliCmdCase(xml);
  switch (clicmd) {
    case 'get_cpu_status': barco_getCpuStatus(xml); break;
    case 'get_memory_status': barco_getMemStatus(xml); break;
    case 'get_software_image_status': barco_getSoftStatus(xml); break;
    case 'get_xpi_fan_status': barco_getFanStatus(xml); break;
    case 'get_network_ports_status': barco_getNetPortsStatus(xml); break;
    case 'get_xpi_firmware_image_status': barco_getFirmwareStatus(xml); break;
    // case 'get_if_list': barco_getIfList(xml); break;
    case 'get_auth_table': barco_getAuthTable(xml); break;
    case 'get_board_info': barco_getBoardInfo(xml); break;
    case 'get_tx_connection_status': barco_getTXConnStatus(xml); break;
    case 'get_rx_connection_status': barco_getRXConnStatus(xml); break;
  } 
}

function barco_dashboardResponseBad(request, status, error) {
  // alert('barco_dashboardResponseBad ' + status + ': ' + error);
}

function barco_sendAJAXDashboardRequest(data) {
  $.ajax({  
    type: "GET",  
    url: "/cgi-bin/localui/dashboard.cgi",
    dataType: 'xml',
    data: 'cli=' + encodeURI(data),  
    success: barco_dashboardResponseGood,
    error: barco_dashboardResponseBad
  });
}

function barco_refreshDashboardSection1(boardType) {
  if (barco_dashboardState.adjustments.changed) {
    var template = tx_adjustments_template;
    if (boardType === 'StreamDst') template = rx_adjustments_template;
    var section1_html =
      Mustache.render(template, barco_dashboardData.adjustments);
    barco_dashboardState.adjustments.changed = false;
    $('#section1').html(section1_html);
    $("#slider").slider({
      // slide: barco_imageQualityVal,
      // stop: barco_imageQualitySet,
      min: barco_slider_range.min,
      max: barco_slider_range.max,
      step: barco_slider_range.step,
      value: parseInt(barco_dashboardData.adjustments.vidQuality)
    });
    $('#slider').slider('disable');
  }
}

function barco_refreshDashboardSection2(boardType) {
  if (barco_dashboardState.statistics.changed) {
    var template = tx_statistics_template;
    if (boardType === 'StreamDst') template = rx_statistics_template;
    var section2_html =
      Mustache.render(template, barco_dashboardData.statistics);
    barco_dashboardState.statistics.changed = false;
    $('#section2').html(section2_html);
  }
}

function barco_refreshDashboardSection3() {
  if (barco_dashboardState.system_health.changed) {
    var section3_html =
      Mustache.render(system_health_template, barco_dashboardData.system_health);
    barco_dashboardState.system_health.changed = false;
    $('#section3').html(section3_html);
  }
}

function barco_refreshDashboardSection4() {
  if (barco_dashboardState.system_info.changed) {
    var section4_html =
      Mustache.render(system_info_template, barco_dashboardData.system_info);
    barco_dashboardState.system_info.changed = false;
    $('#section4').html(section4_html);
  }
}

function barco_refreshDashboardDisplay() {
  var boardType = barco_dashboardData.system_info.boardType;
  if (!barco_dashboardState.adjustments.user_override)
    barco_refreshDashboardSection1(boardType);
  if (!barco_dashboardState.statistics.user_override)
    barco_refreshDashboardSection2(boardType);
  if (!barco_dashboardState.system_health.user_override)
    barco_refreshDashboardSection3();
  if (!barco_dashboardState.system_info.user_override)
    barco_refreshDashboardSection4();
}

function barco_populateDashboardFast() {
  var boardType = barco_dashboardData.system_info.boardType;
  if (boardType === 'StreamSrc')
    barco_sendAJAXDashboardRequest('get_tx_connection_status');
  else if (boardType === 'StreamDst')
    barco_sendAJAXDashboardRequest('get_rx_connection_status');
}

function barco_populateDashboardMedium() {
  barco_sendAJAXDashboardRequest('get_memory_status');
}

function barco_populateDashboardSlow() {
  barco_sendAJAXDashboardRequest('get_software_image_status');
  barco_sendAJAXDashboardRequest('get_xpi_fan_status');
  barco_sendAJAXDashboardRequest('get_cpu_status');
}

function barco_populateDashboardOnce() {
  barco_sendAJAXDashboardRequest('get_board_info');
  barco_sendAJAXDashboardRequest('get_network_ports_status');
  barco_sendAJAXDashboardRequest('get_xpi_firmware_image_status');
  // barco_sendAJAXDashboardRequest('get_if_list');
  barco_sendAJAXDashboardRequest('get_auth_table');
}

/* ============================================================================
 * Created jQuery validation class strings from XML attributes
 * ==========================================================================*/

/**
 * @param {type} xml
 * @returns {String}
 */
function barco_validateFromXMLschema(xml)
{
  var classStr = 'validate[';
  var inpType = xml.attr('type');
  var validation = xml.attr('validation');
  var inpSize = xml.attr('size');
  var inpMin = xml.attr('min');
  var inpMax = xml.attr('max');
  var separator = '';
  if (validation !== 'enumeration') {
    if (inpType === 'integer') {
      classStr += separator + 'custom[integer]';
      if (separator === '') separator = ',';
    }
    if (inpSize !== undefined) {
      classStr += separator + 'maxSize[' + inpSize + ']';
      if (separator === '') separator = ',';
    }
  }
  if (validation !== undefined) {
    if (validation.match(/range/i)) {
      var minVal = parseFloat(inpMin);
      var maxVal = parseFloat(inpMax);
      if (!(isNaN(minVal) || isNaN(maxVal))) {
        classStr += separator + 'custom[number]' +
          'min[' + inpMin + '],max[' + inpMax + ']';
      }
    }
    if (validation.match(/ipaddress/i)) {
      classStr += separator + 'custom[ipv4]';
    }
    if (validation.match(/netmask/i)) {
      classStr += separator + 'custom[netmask]';
    }
  }
  classStr += ']';
  return classStr;
}

/* ============================================================================
 * Refresh certain status tables periodically
 * ==========================================================================*/

var barco_status_url = "/cgi-bin/localui/ms/get_status.cgi?cmd=";
var barco_refresh_cmds = [];
var barco_hStatusRefresh = null;
var barco_timeoutErrorHandle = null;
var barco_errorPriority = 3; // 1 highest; 3 lowest

function barco_addCmdToRefreshList(cmd) {
  barco_refresh_cmds.push(cmd);
}

function barco_errorDisplay(status, error) {
  // If a higher priority error is showning, don't overwrite it.
  if (barco_errorPriority < 2) return; 
  barco_errorPriority = 2;
  var statStr = "";
  if (status === "error") {
    if (error.length === 0)
      statStr = tK('errorLost');
    else
      statStr = tK('errorDev');
  }
  else if (status === "timeout")
    statStr = tK('errorTimeout');
  else if (status === "abort")
    statStr = tK('errorAbort');
  else if (status === "parseerror")
    statStr = tK('errorParse');
  $('div.servererror').removeClass('hidden')
    .html('<strong>' + statStr +
           error + '</strong>');
}

function barco_statusResponseGood(response, status) {
  if (barco_errorPriority >= 2)
    $('div.servererror').addClass('hidden').html('');
  var xmlDOM = $(response);
  var cliName = null;
  xmlDOM.find('CLICommand').each(function () {
    cliName = $(this).text();
  });
  var tabletext = createStatusHTML(xmlDOM, cliName);
  var tablehtml = $.parseHTML(tabletext);
  $(tablehtml).replaceAll('#' + cliName);
}

function barco_statusResponseBad(request, status, error) {
  barco_timeoutErrorHandle = window.setTimeout(function () {
    window.clearTimeout(barco_timeoutErrorHandle);
    barco_errorDisplay(status, error);
  }, 5000);
}

function barco_refreshStatusGet(cmd) {
  $.ajax({  
    type: "GET",  
    url: barco_status_url + cmd,
    dataType: 'xml',
    success: barco_statusResponseGood,
    error: barco_statusResponseBad
  });  
}  

function barco_refreshStatusDisplay(evt) {
  var len = barco_refresh_cmds.length;
  for (var i = 0; i < len; i++) {
    var cmd = barco_refresh_cmds[i];
    barco_refreshStatusGet(cmd);
  }
}

/* ============================================================================
 * List form Add/Edit/Delete Handling
 * ==========================================================================*/
var barco_animTime = 300;

var barco_rowDeletes = [];
var barco_rowEdits = [];
var barco_rowAdds = [];

// Begin Special Connection Table Handling

function barco_breakConn(evt) {
  barco_sendAJAXCmdRequest('id=break_conn');
}

function barco_makeConn(evt) {
  var self = $(this);
  var tr = self.parent().parent();
  var id = tr.attr('id');
  var parms = id.split('-row-');
  barco_sendAJAXCmdRequest('id=make_conn&idx=' + parms[1]);
}

// End Special Connection Table Handling

function barco_prefillListFormWithDefaults(cliName) {
  prefillListForm(cliName); // defined in barco-xml-utils.js
}

function barco_getCorrespondingForm(self, op) {
  var cliName = null;
  if (op === 'add') {
    var addIdParts = self.attr('id').split('add_');
    cliName = addIdParts[1];
    barco_rowAdds.push([cliName,'0']);
    var cliInput = $('input[name="cli_' + cliName + '"]');
    barco_setCLIActiveVal(cliInput);
    barco_prefillListFormWithDefaults(cliName);
  } else {
    var href = self.attr('href');
    var hrefParts = href.substring(1, href.length).split('-row-');
    var cliName = hrefParts[0];
  }
  var form = $('div#' + cliName).children('div.collapsible-form');
  return form;
}

function barco_collapsibleFormShow(self, op) {
  var form = barco_getCorrespondingForm(self, op);
  form.slideDown(barco_animTime, function() {
      $('ul.row-context-menu').hide(0);
    });
  $('input.addbutton').prop('disabled', true);
  $('a.bicon-settings').addClass('disabled');
}

function barco_rowUndoDelete(self) {
  self.removeClass('bicon-undo').addClass('bicon-settings');
  var tr = self.parent().parent().parent();
  tr.removeClass('row-delete');
  if ($('input.addbutton').prop('disabled')) {
    self.addClass('disabled');
  }
}

function barco_toggleRowEditMenuArg(self) {
  if (self.hasClass('disabled')) return false;
  var menu = $(self).next();
  if (self.hasClass('bicon-undo') && menu.is(':hidden')) {
    barco_rowUndoDelete(self);
    return false;
  }
  var allActiveCogs = $('a.bicon-settings.active');
  var allMenus = $('ul.row-context-menu');
  if (menu.is(':hidden')) {
    allActiveCogs.removeClass('active');
    allMenus.hide(0);
    self.addClass('active');
  }
  menu.stop(true,true).slideToggle('fast', function() {
    if (menu.is(':hidden')) self.removeClass('active');
    else self.addClass('active');
  });
  return false;
}

function barco_toggleRowEditMenu(evt) {
  var self = $(this);
  barco_toggleRowEditMenuArg(self);
  // evt.stopPropagation();
  return false;
}

function barco_collapsibleFormHide() {
  $('div.collapsible-form').slideUp(barco_animTime, function() {
      $('input.addbutton').prop('disabled', false);
      barco_rowAdds = [];
      $('a.bicon-undo').removeClass('bicon-undo').addClass('bicon-settings');
      $('a.bicon-settings').removeClass('disabled');
      $('a.bicon-settings.active').removeClass('active');
      $('tr.row-edit').removeClass('row-edit');
      $('tr.row-delete').removeClass('row-delete');
      $('div.servererror').addClass('hidden');
      barco_errorPriority = 3;
    });
}

function barco_setupListForms(contextMenus) {
  $('#break_conn').on('click', barco_breakConn);
  $('input.addbutton').on('click', function () {
      barco_collapsibleFormShow($(this), 'add');
    });
  $('input.connectbut').on('click', barco_makeConn);
  $('a.bicon-settings').on('click', barco_toggleRowEditMenu);
  for (var i in contextMenus) {
    var contextMenu = contextMenus[i];
    for (var j in contextMenu) {
      var menuItem = contextMenu[j];
      $('a.' + menuItem.cls).on('click', menuItem.func);
    }
  }
}

function barco_populateListFormForIdx(cliname, dataHTML, idx) {
  var groupDoc = $.parseXML(dataHTML);
  var groupXML = $(groupDoc);
  var keyvalues = null;
  groupXML.find('objectentry').each(function() {
    var objectentry = $(this);
    var attrIdx = objectentry.attr('index');
    // == used because we need type coersion here
    if (attrIdx == idx) {
      populateFormForEntry(cliname, objectentry);
    }
  });
}

function barco_populateListFormAtIdIdx(descHTML, dataHTML, id, idx) {
  var cliname="";
  var descDoc = $.parseXML(descHTML);
  var descXML = $(descDoc);
  var cliXML = descXML.find("clicommandname");
  // != used because cliXML might be null or undefined.
  if (cliXML != null) {
    cliname = cliXML.text();
  }
  if (id.substr(1, cliname.length) !== cliname) return;
  barco_populateListFormForIdx(cliname, dataHTML, idx);
}

function barco_collectRowDeletes(idx) {
  var tr = $(this);
  var id = tr.attr('id');
  var parms = id.split('-row-');
  barco_rowDeletes.push(parms);
}

function barco_collectRowEdits(idx) {
  var tr = $(this);
  var id = tr.attr('id');
  var parms = id.split('-row-');
  barco_rowEdits.push(parms);
}

function barco_buildIndexStringsForSubmit(list) {
  var cliIndexes = {};
  for (var i in list) {
    var cliname = list[i][0];
    var index = list[i][1];
    if (cliname in cliIndexes) {
      var str = cliIndexes[cliname];
      str += ',' + index;
      cliIndexes[cliname] = str;
    } else {
      cliIndexes[cliname] = '' + index;
    }
  }
  return cliIndexes;
}

function barco_setInputValuesForSubmit(cliIndexes, op) {
  for (var cli in cliIndexes) {
    var idxStr = cliIndexes[cli];
    if (op === 'del') {
      $('input#' + cli + '_deletes').val(idxStr);
      var cliInput = $('input[name="cli_' + cli + '"]');
      barco_setCLIActiveVal(cliInput);
    }
    if (op === 'edit' || op === 'add') {
      $('input#' + cli + '_index').val(idxStr);
    }
  }
}

function barco_collectUserData() {
  barco_rowDeletes = [];
  barco_rowEdits = [];
  $('tr.row-delete').each(barco_collectRowDeletes);
  $('tr.row-edit').each(barco_collectRowEdits);
  var cliIndexes = null;
  if (barco_rowDeletes.length > 0) {
    barco_rowDeletes.reverse();
    cliIndexes = barco_buildIndexStringsForSubmit(barco_rowDeletes);
    barco_setInputValuesForSubmit(cliIndexes, 'del');
  }
  if (barco_rowEdits.length > 0) {
    cliIndexes = barco_buildIndexStringsForSubmit(barco_rowEdits);
    barco_setInputValuesForSubmit(cliIndexes, 'edit');
  }
  if (barco_rowAdds.length > 0) {
    cliIndexes = barco_buildIndexStringsForSubmit(barco_rowAdds);
    barco_setInputValuesForSubmit(cliIndexes, 'add');
  }
}

function barco_rowEdit() {
  var id = $(this).attr('href');
  var idx = $(this).data('index');
  $(id).addClass('row-edit');
  $(".listform").each (function (index) {
    var formdata = $(this).children().eq(1).html();
    var formdesc = $(this).children().eq(0).html();
    barco_populateListFormAtIdIdx(formdesc, formdata, id, idx);
    });
  barco_collapsibleFormShow($(this), 'edit');
  return false;
}

function barco_rowDelete() {
  var self = $(this);
  var id = self.attr('href');
  var idx = self.data('index');
  $(id).addClass('row-delete');
  var td = self.parent().parent().parent();
  var a = td.children('a');
  a.removeClass('bicon-settings').addClass('bicon-undo');
  barco_toggleRowEditMenuArg(a);
  return false;
}

/* ============================================================================
 * Streaming.cgi connection profile handling
 * ==========================================================================*/
function barco_videoPresets(evt) {
  var use_auth = $('input#v2d_streaming_profile_checkbox_enable_auth_table');
  if (use_auth == null) return;
  var use_auth_checked = use_auth.prop('checked');
  var profile = $('select#v2d_streaming_profile_profile');
  var selection = profile.val();
  var found = false;
  var preset;
  var val;
  for (val in barco_video_presets) {
    preset = barco_video_presets[val];
    if (preset.profile == selection) {
      found = true;
      break;
    } 
  }
  if (use_auth_checked) {
    profile.prop('disabled', true);
    $('input#v2d_streaming_profile_bandwidth').prop('disabled', true);
    $('input#v2d_streaming_profile_low_compression').prop('disabled', true);
    $('input#v2d_streaming_profile_high_compression').prop('disabled', true);
    return;
  } else {
    profile.prop('disabled', false);
    $('input#v2d_streaming_profile_bandwidth').prop('disabled', false);
    $('input#v2d_streaming_profile_low_compression').prop('disabled', false);
    $('input#v2d_streaming_profile_high_compression').prop('disabled', false);
  }
  if (found && val === 'custom') {
    $('input#v2d_streaming_profile_bandwidth').prop('readonly', false);
    $('input#v2d_streaming_profile_low_compression').prop('readonly', false);
    $('input#v2d_streaming_profile_high_compression').prop('readonly', false);
  } else {
    $('input#v2d_streaming_profile_bandwidth').prop('readonly', true);
    $('input#v2d_streaming_profile_low_compression').prop('readonly', true);
    $('input#v2d_streaming_profile_high_compression').prop('readonly', true);
    $('input#v2d_streaming_profile_bandwidth').val(preset.bw);
    $('input#v2d_streaming_profile_low_compression').val('6');
    $('input#v2d_streaming_profile_high_compression').val(preset.imgQ);
  }
}

/* ============================================================================
 * Admin.cgi command button handling
 * ==========================================================================*/

var barco_rebootTimout1 = 15*1000; // 15 seconds
var barco_rebootTimout2 = 15*1000; // 15 seconds
var barco_rebootFailTimeout = 3*60*1000; // 3 minutes;

var barco_modalShowing = false;
var barco_responseReceived = 0;
var barco_ngs_status = 'UNKNOWN';
var barco_ngs_results = '';
var barco_rebootTimeoutHandle = null;
var barco_rebootWatchdogHandle = null;

function barco_findStatusInHTML(response) {
  // Look for <status>...</status> even across multiple lines
  var str = response.match(/<status[^>]*>[\s\S]*?<\/status>/i);
  // == used because we need to detect both null and undefined
  if (str == null) return 'NO MATCH';
  var xmlDoc = $.parseXML(str[0]);
  var xml = $(xmlDoc);
  var tag = xml.find('status');
  // Strip all whitespace and line terminators from <status> tag text
  var msg = tag.text().replace(/\s*/g, '');
  return msg;
}

function barco_findResultsInHTML(response) {
  // Look for <results>...</results> even across multiple lines
  var str = response.match(/<results[^>]*>[\s\S]*?<\/results>/i);
  // == used because we need to detect both null and undefined
  if (str == null) return '';
  var xmlDoc = $.parseXML(str[0]);
  var xml = $(xmlDoc);
  var tag = xml.find('results');
  return tag.text();
}

function barco_NGSisBack(response, status) {
  window.clearInterval(barco_rebootTimeoutHandle);
  window.clearTimeout(barco_rebootWatchdogHandle);
  $('#myModal').modal('hide');
  // Sometimes web server is ready before the rest of the system causing the
  // template substitutions to fail. Adding a couple of seconds to make that
  // less likely. Not an ideal solution but this only happens occasionally.
  window.setTimeout(function () {
    window.location.href = "/";
  }, 2000);
}

function barco_NGSisDown(request, status, error) {
  window.clearInterval(barco_rebootTimeoutHandle);
  window.clearTimeout(barco_rebootWatchdogHandle);
  $('#myModal h3').html(tK('shutdownAfter'));
}

/*
 *  Note: The rebooted check is not reliable. If we are using HTTPS with self-
 *  signed certs. where the IP address has changed, the browser will block
 *  access to the new IP address thinking site trust has been compromised.
 *  The Chrome browser won't return anything, not even a error.
 */
function barco_NGSCheckIfRebooted(evt) {
  if (barco_ngs_status !== "REVERT")
    pingUrl = "/clients/gui/index.html"; 
  else 
    pingUrl = "/";
  $.ajax({  
    type: "HEAD",  
    url: pingUrl,
    success: barco_NGSisBack
  });  
}

function barco_NGSCheckIfShutdown(evt) {
  $.ajax({  
    type: "HEAD",  
    url: "/",
    error: barco_NGSisDown
  });  
}

function barco_NGSRebootDelay(evt) {
  window.clearTimeout(barco_rebootTimeoutHandle);
  barco_rebootTimeoutHandle =
    window.setInterval(barco_NGSCheckIfRebooted, barco_rebootTimout2);
  barco_NGSCheckIfRebooted(evt);
}

function barco_NGSShutdownDelay(evt) {
  window.clearTimeout(barco_rebootTimeoutHandle);
  barco_rebootTimeoutHandle =
    window.setInterval(barco_NGSCheckIfShutdown, barco_rebootTimout2);
  barco_NGSCheckIfShutdown(evt);
}

function barco_rebootWatchdog() {
  window.clearTimeout(barco_rebootTimeoutHandle);
  window.clearTimeout(barco_rebootWatchdogHandle);
  $('#myModal h3').html(tK('watchdogTimeout'));
}

function barco_ngsStatusHandler() {
  if (barco_ngs_status === "OK") { // success
    $('#myModal').modal('hide');
  } else if (barco_ngs_status === "REFRESH") { // success
    $('#myModal').modal('hide');
    window.location.href = document.URL;
  } else if (barco_ngs_status === "NO MATCH") {
    $('#myModal h3').html(tK('statusNoMatch'));
  } else if (barco_ngs_status === "TIMEDOUT") {
    $('#myModal h3').html(tK('statusTimedOut'));
  } else if (barco_ngs_status === "ERROR") {
    $('#myModal h3').html(tK('statusError') + '<br>' + barco_ngs_results);
  } else if (barco_ngs_status === "REVERT") {
    $('#myModal h3').html(tK('statusRevert'));
    barco_rebootTimeoutHandle =
      window.setTimeout(barco_NGSRebootDelay, barco_rebootTimout1);
    barco_rebootWatchdogHandle =
      window.setTimeout(barco_rebootWatchdog, barco_rebootFailTimeout);
  } else if (barco_ngs_status === "REBOOT") {
    $('#myModal h3').html(tK('statusReboot'));
    barco_rebootTimeoutHandle =
      window.setTimeout(barco_NGSRebootDelay, barco_rebootTimout1);
    barco_rebootWatchdogHandle =
      window.setTimeout(barco_rebootWatchdog, barco_rebootFailTimeout);
  } else if (barco_ngs_status === "SHUTDOWN") {
    $('#myModal h3').html(tK('statusShutdown'));
      barco_rebootTimeoutHandle =
        window.setTimeout(barco_NGSShutdownDelay, barco_rebootTimout1);
  }
}

function barco_ngsSystemErrorHandler() {
  $('#myModal h3').html(tK('statusLost'));
}

function barco_processCmdGood(response, status) {
  barco_ngs_status = barco_findStatusInHTML(response);
  if (barco_ngs_status === "ERROR") {
    barco_ngs_results = barco_findResultsInHTML(response);
  }
  barco_responseReceived = 1;
  if (barco_modalShowing) {
    barco_ngsStatusHandler();
  }
  // $('div.servererror').removeClass('hidden')
  //   .html('RESPONSE FROM /cgi-bin/processcmd.cgi: ' + barco_ngs_status);
}

function barco_processCmdBad(request, status, error) {
  barco_responseReceived = 2;
  if (barco_modalShowing)
    barco_ngsSystemErrorHandler();
  // $('div.servererror').removeClass('hidden')
  //   .html('BAD STATUS FROM /cgi-bin/processcmd.cgi ' + status + '/' + error);
}

function barco_modalShowingEvt(evt) {
  $('#myModal').off('shown', barco_modalShowing);
  $('#myModal h3').html(tK('statusSubmit'));
  barco_modalShowing = true;
  if (barco_responseReceived === 1) {
    barco_ngsStatusHandler();
  }
  if (barco_responseReceived === 2) { // error
    barco_ngsSystemErrorHandler();
  }
}

function barco_processCmdData(id, self) {
  var data = "id=" + id;
  var showPrompt=false;
  var confirmationMsg="";
  
  if (id === "upgrade") {
	showPrompt=true;
	confirmationMsg="This utility will allow you to upgrade software on this MS. The MS will reboot automatically. Proceed with software upgrade?";
    var url = $('input#upgrade_url').val();
    data += "&url=" + encodeURI(url);
  } else if (id === "reboot" || id === "shutdown") {
	showPrompt=true;
	confirmationMsg="In case any settings have been changed, please save them before continuing as any unsaved changes will be lost across reboots.";
  }
  else if (id === "rebuildDisk") {
    var diskId = $('input#disk_id').val();
    data += "&diskid=" + diskId;
  } else if (id === "revert") {
	showPrompt=true;
	var reset_network_checked = $('input#reset_network').prop('checked');
	data += '&reset_network=' + (reset_network_checked ? "yes" : "no");
	confirmationMsg="All settings will be lost except the network settings. If \"Reset network settings\" is also checked then the MS will revert back to a DHCP assigned IP address that this client may not be able to reach. Do you wish to continue?";
  }
  
  if(showPrompt)
	if (confirm(confirmationMsg) ) {
		// Do nothing just go back
	} else 
		throw new Error();
  
  return data;
}

function barco_sendAJAXCmdRequest(data) {
  barco_dms_status = "";
  barco_dms_results = "";
  $.ajax({  
    type: "GET",  
    url: "/cgi-bin/localui/ms/processcmd.cgi",
    // dataType: 'xml',
    data: data,  
    success: barco_processCmdGood,
    error: barco_processCmdBad
  });
  // $('div.servererror').removeClass('hidden')
  //   .html('/cgi-bin/processcmd.cgi?id=' + id + ' REQUEST SENT');
  barco_modalShowing = false;
  $('#myModal').on('shown', barco_modalShowingEvt);
  $('#myModal').modal({show:true, backdrop:'static'});
}

function barco_processCmd() {
  barco_responseReceived = 0;
  var self = $(this);
  var id = self.attr('id');
  try {
	var data = barco_processCmdData(id, self);
	barco_sendAJAXCmdRequest(data);
  }
  catch (e) {
	// user doesn't want to proceed with factory reser. just do nothing
  }
}

/* ============================================================================
 * Subsection submits
 * ==========================================================================*/

function barco_sectionInput(input, sectionId) {
  var name = input.attr('name');
  if (name === undefined) return '';
  var inputType = input.attr('type');
  var val = encodeURIComponent(input.val());
  var query = '&' + name + '=';
  if (inputType === 'checkbox') {
    var checked = input.prop('checked');
    if (val.length === 0) val = 'on';
    query +=  checked ? val : '';
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

function barco_subsectionSubmit(evt) {
  var self = $(this);
  var id = self.attr('id');
  var parts = id.split('_ajax_submit');
  var sectionId = parts[0];
  var query = 'id=ajax_submit&cli_' + sectionId + '=1';
  $('[id^=' + sectionId + '_]').each(function (i, elt) {
      query += barco_sectionInput($(this), sectionId);
    });
  barco_sendAJAXCmdRequest(query);
}

function barco_setupSubsectionSubmits() {
  $('input.ajax_submit').on('click', barco_subsectionSubmit);
}

/* ============================================================================
 * Collapsible fieldsets
 * ==========================================================================*/
var barco_expandOpenList = null;
var barco_expandOpenCnt = 0;

function barco_identifyCollapsedSets(i) {
  var self = $(this);
  var closed = self.hasClass('collapse-close');
  barco_expandOpenCnt++;
  if (closed) {
    var tableDiv = self.parent().parent().next();
    barco_expandOpenList.push({icon:self, section:tableDiv, done:false});
  }
}

function barco_identifyOpenedSets(i) {
  var self = $(this);
  var opened = self.hasClass('collapse-open');
  barco_expandOpenCnt++;
  if (opened) {
    var tableDiv = self.parent().parent().next();
    barco_expandOpenList.push({icon:self, section:tableDiv, done:false});
  }
}

function barco_maybeUpdateCollapseAllUI(collapse) {
  var allCollapseRef = $('#collapse_expand_all a');
  var allCollapsed = allCollapseRef.hasClass('collapse-all-close');
  if (collapse && allCollapsed) {
    barco_expandOpenList = [];
    barco_expandOpenCnt = 0;
    $('.table-header-icon a').each(barco_identifyOpenedSets);
    var len = barco_expandOpenList.length;
    if (barco_expandOpenCnt === 0) {
      $('#collapse_expand_all').remove();
    } else if (len === barco_expandOpenCnt) {
      allCollapseRef.addClass('collapse-all-open');
      allCollapseRef.removeClass('collapse-all-close');
      allCollapseRef.html(tK('expandAll'));
    }
  } else if (!collapse && !allCollapsed) {
    barco_expandOpenList = [];
    barco_expandOpenCnt = 0;
    $('.table-header-icon a').each(barco_identifyCollapsedSets);
    var len = barco_expandOpenList.length;
    if (barco_expandOpenCnt === 0) {
      $('#collapse_expand_all').remove();
    } else if (len === barco_expandOpenCnt) {
      allCollapseRef.addClass('collapse-all-close');
      allCollapseRef.removeClass('collapse-all-open');
      allCollapseRef.html(tK('collapseAll'));
    }
  }
}

function barco_collapsibleFieldSet(evt) {
  var self = $(this);
  var tableDiv = self.parent().parent().next();
  var collapse = self.hasClass('collapse-close');
  if (collapse) {
    tableDiv.stop(true,true)
      .slideUp(barco_animTime, function() {
        self.removeClass('collapse-close');
        self.addClass('collapse-open');
        barco_maybeUpdateCollapseAllUI(true);
      });
  } else {
    tableDiv.stop(true,true)
      .slideDown(barco_animTime, function() {
        self.addClass('collapse-close');
        self.removeClass('collapse-open');
        barco_maybeUpdateCollapseAllUI(false);
      });
  }
  return false;
}

function barco_collapseAllSets(self) {
  var len = barco_expandOpenList.length;
  var allDone = true;
  for (var i= 0; i < len; i++) {
    var obj = barco_expandOpenList[i];
    if (!obj.done) {
      obj.section.stop(true,true)
        .slideUp(barco_animTime/5, function() {
          obj.icon.addClass('collapse-open');
          obj.icon.removeClass('collapse-close');
          obj.done = true;
          barco_collapseAllSets(self);
        });
      allDone = false;
      break;
    }
  }
  if (allDone) {
    self.addClass('collapse-all-open');
    self.removeClass('collapse-all-close');
    self.html(tK('expandAll'));
  }
}

function barco_expandAllSets(self) {
  var len = barco_expandOpenList.length;
  var allDone = true;
  for (var i= 0; i < len; i++) {
    var obj = barco_expandOpenList[i];
    if (!obj.done) {
      obj.section.stop(true,true)
        .slideDown(barco_animTime/5, function() {
          obj.icon.addClass('collapse-close');
          obj.icon.removeClass('collapse-open');
          obj.done = true;
          barco_expandAllSets(self);
        });
      allDone = false;
      break;
    }
  }
  if (allDone) {
    self.addClass('collapse-all-close');
    self.removeClass('collapse-all-open');
    self.html(tK('collapseAll'));
  }
}

function barco_collapsibleAllSet(evt) {
  var self = $(this);
  var collapse = self.hasClass('collapse-all-close');
  barco_expandOpenList = [];
  barco_expandOpenCnt = 0;
  if (collapse) {
    $('.table-header-icon a').each(barco_identifyCollapsedSets);
    barco_collapseAllSets(self);
  } else {
    $('.table-header-icon a').each(barco_identifyOpenedSets);
    barco_expandAllSets(self);
  }
}

function barco_setupCollapsibleFieldSets() {
  $('.table-header-icon a').on('click', barco_collapsibleFieldSet);
  $('#collapse_expand_all a').on('click', barco_collapsibleAllSet);
  barco_maybeUpdateCollapseAllUI(true);
}

/* ============================================================================
 * Select which CLIs to invoke based on which fields the user changed
 * ==========================================================================*/

function barco_setCLIActiveVal(cliInput) {
  var cliValue = cliInput.val();
  if (cliValue.length > 0) return;
  cliInput.val('1');
}

function barco_setCLIActive() {
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
  $("input").on("change", barco_setCLIActive);
}

/* ============================================================================
 * Initialization
 * ==========================================================================*/

var barco_generalSettings = {
  password: ''
};

function barco_passwordFocus(evt) {
  if (evt.type === 'focus' && $(this).val() === barco_generalSettings.password) {
    $(this).val('');
  } else if (evt.type === "blur" && $(this).val() === "") {
    $(this).val(barco_generalSettings.password);
  }
}

function barco_ngsSetupDashboard(statusRefreshInterval) {
  barco_setupLocalization('en');
  barco_populateDashboardOnce();
  barco_populateDashboardSlow();
  barco_populateDashboardMedium();
  barco_populateDashboardFast();
  window.setInterval(barco_populateDashboardSlow, 10*statusRefreshInterval);
  window.setInterval(barco_populateDashboardMedium, 3*statusRefreshInterval);
  window.setInterval(barco_populateDashboardFast, statusRefreshInterval);
  barco_refreshDashboardDisplay();
  barco_hStatusRefresh =
    window.setInterval(barco_refreshDashboardDisplay, 537);
  $("input[data-toggle=popover]")
    .popover().click(function(e) { e.preventDefault(); });
}

function barco_listFormMenuFunctions() {
  barco_contextMenuA[0].func = barco_rowEdit;
  barco_contextMenuA[1].func = barco_rowDelete;
  return [barco_contextMenuA];
}

function barco_ngsSetupDetail(statusRefreshInterval) {
  barco_hStatusRefresh =
    window.setInterval(barco_refreshStatusDisplay, statusRefreshInterval);
  $("input[data-toggle=popover]")
    .popover().click(function(e) { e.preventDefault(); });
  var contextMenus = barco_listFormMenuFunctions();
  barco_setupLocalization('en');
  barco_setupListForms(contextMenus);   
  barco_setupCollapsibleFieldSets();
  barco_setupSubsectionSubmits();
  barco_setupCLISetInvocation();
}

function barco_setupAdminButtons() {
  $('input.cmdbutton').on('click', barco_processCmd);
}

function barco_setupProfileHandling() {
  barco_videoPresets();
  $('select#v2d_streaming_profile_profile').on('change', barco_videoPresets);
  $('input#v2d_streaming_profile_checkbox_enable_auth_table').on('change', barco_videoPresets);
}

function barco_killStatusRefresh() {
  window.clearInterval(barco_hStatusRefresh);
}

  
function barco_setControlsState () {
	var $adminPwd = $('input[name="adminpwd"]');
	var $retypePwd = $('input[name="retypeadminpwd"]');
	if ($adminPwd.length > 0) {
		barco_generalSettings.password = $adminPwd.val();
		$adminPwd.on('focus', barco_passwordFocus);
		$adminPwd.on('blur', barco_passwordFocus);
		$retypePwd.on('focus', barco_passwordFocus);
		$retypePwd.on('blur', barco_passwordFocus);
	}
	var useNTP = $('input[name="ms_server_options_checkbox_enablentp"]');
	useNTP.on('change', changeNTPState);
	changeNTPState();
	
	var enableHA = $('input[name="ha_configuration_checkbox_enableha"]');
	enableHA.on('change', changeHAState);
	changeHAState();
	
	var enableVirtualIP = $('input[name="ha_configuration_checkbox_enable_virtual_ip"]');
	enableVirtualIP.on('change', changeVirtualIPState);
	
	var enabledns = $('input[name="ha_configuration_checkbox_enabledns"]');
	enabledns.on('change', changeDnsState);
	
	var enableLDAP = $('input[name="ldap_configuration_checkbox_enableldap"]');
	enableLDAP.on('change', changeLDAPState);
	changeLDAPState();
	
	var enableDailyBackup = $('input[name="db_management_checkbox_daily_backup"]');
	enableDailyBackup.on('change', changeDailyBackupState);
	changeDailyBackupState();
	
	var enableFTP = $('input[name="db_management_checkbox_enable_export"]');
	enableFTP.on('change', changeFTPState);
} 

function changeNTPState() {
	useNTP= (document.getElementById('ms_server_options_checkbox_enablentp').checked ? true : false);
	$('input[name="ms_server_options_ntpip"]').prop("disabled", useNTP);
}

function changeHAState () {
	enableHA= (document.getElementById('ha_configuration_checkbox_enableha').checked ? true : false);
	$('input[name="ha_configuration_peerip"]').prop("disabled", !enableHA);
	$('input[name="ha_configuration_peername"]').prop("disabled", !enableHA);
	$('select[name="ha_configuration_interface"]').prop("disabled", !enableHA);
	$('input[name="ha_configuration_checkbox_enable_virtual_ip"]').prop("disabled", !enableHA);
	$('input[name="ha_configuration_virtualip"]').prop("disabled", !enableHA);
	$('input[name="ha_configuration_virtualip2"]').prop("disabled", !enableHA);
	$('input[name="ha_configuration_checkbox_enable_as_secondary"]').prop("disabled", !enableHA);
	$('input[name="ha_configuration_checkbox_enabledns"]').prop("disabled", !enableHA);

	if (enableHA)
	{	
		changeVirtualIPState();
		changeDnsState();
	}
}

function changeVirtualIPState() {
	enableVirtualIP= (document.getElementById('ha_configuration_checkbox_enable_virtual_ip').checked ? true : false);
	$('input[name="ha_configuration_virtualip"]').prop("disabled", !enableVirtualIP);
	$('input[name="ha_configuration_virtualip2"]').prop("disabled", !enableVirtualIP);
	$('input[name="ha_configuration_checkbox_enabledns"]').prop("checked", !enableVirtualIP);
}

function changeDnsState() {
	enableDns= (document.getElementById('ha_configuration_checkbox_enabledns').checked ? true : false);
	$('input[name="ha_configuration_checkbox_enable_virtual_ip"]').prop("checked", !enableDns);
	$('input[name="ha_configuration_virtualip"]').prop("disabled", enableDns);
	$('input[name="ha_configuration_virtualip2"]').prop("disabled", enableDns);
}

function changeLDAPState () {
	enableLDAP= (document.getElementById('ldap_configuration_checkbox_enableldap').checked ? true : false);
	$('input[name="ldap_configuration_ldaphost"]').prop("disabled", !enableLDAP);
	$('input[name="ldap_configuration_admindn"]').prop("disabled", !enableLDAP);
	$('input[name="ldap_configuration_adminpassword"]').prop("disabled", !enableLDAP);
	$('input[name="ldap_configuration_basedn"]').prop("disabled", !enableLDAP);
	$('input[name="ldap_configuration_userfield"]').prop("disabled", !enableLDAP);
}

function changeDailyBackupState () {
	enableDailyBackup= (document.getElementById('db_management_checkbox_daily_backup').checked ? true : false);
	$('input[name="db_management_when"]').prop("disabled", !enableDailyBackup);
	$('input[name="db_management_checkbox_enable_export"]').prop("disabled", !enableDailyBackup);
	$('input[name="db_management_export_url"]').prop("disabled", !enableDailyBackup);
	$('select[name="db_management_recovery_mode"]').prop("disabled", !enableDailyBackup);
	if (enableDailyBackup)
		changeFTPState();		
}

function changeFTPState() {
	enableVirtualIP= (document.getElementById('db_management_checkbox_enable_export').checked ? true : false);
	$('input[name="db_management_export_url"]').prop("disabled", !enableVirtualIP);
}

function barco_showHideExitAdminLink() {
	exitAdminField= document.getElementById('exitadmin');
	exitAdminField.style.display = "block";
}	

