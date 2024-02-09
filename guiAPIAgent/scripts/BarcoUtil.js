/**
 *
 *  Base64 encode / decode
 *  http://www.webtoolkit.info/
 *
 **/

window.bERROR = 0;
window.bWARNING = 1;
window.bINFO = 2;
window.bDEBUG = 3;
window.bLEVEL = bINFO;

if (!window.console) window.console = {};
if (!window.console.log) window.console.log = function () { };

window.barco = window.barco || {};
barco.playerDispatch = jQuery({});

jQuery.publish = function() {
  barco.playerDispatch.trigger.apply(barco.playerDispatch, arguments);
};

jQuery.subscribe = function() {
  barco.playerDispatch.on.apply(barco.playerDispatch, arguments);
};

jQuery.unsubscribe = function() {
  barco.playerDispatch.off.apply(barco.playerDispatch, arguments);
};

jQuery.isBlank = function(obj) {
  return (typeof obj == 'undefined' || jQuery.trim(obj) === "");
};
  
jQuery.log = function (data) {
    if (window.console && window.console.log)
        console.log(data);
};

BarcoUtil = function() {
};

BarcoUtil.b64encode = function(text) {
	if (/([^\u0000-\u00ff])/.test(text)) {
		throw new Error("Can't base64 encode non-ASCII characters.");
	}
	var digits = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/",
		i = 0,
		cur, prev, byteNum, result = [];
	while (i < text.length) {
		cur = text.charCodeAt(i);
		byteNum = i % 3;
		switch (byteNum) {
			case 0:
				//first byte
				result.push(digits.charAt(cur >> 2));
				break;
			case 1:
				//second byte
				result.push(digits.charAt((prev & 3) << 4 | (cur >> 4)));
				break;
			case 2:
				//third byte
				result.push(digits.charAt((prev & 0x0f) << 2 | (cur >> 6)));
				result.push(digits.charAt(cur & 0x3f));
				break;
		}
		prev = cur;
		i++;
	}
	if (byteNum == 0) {
		result.push(digits.charAt((prev & 3) << 4));
		result.push("==");
	} else if (byteNum == 1) {
		result.push(digits.charAt((prev & 0x0f) << 2));
		result.push("=");
	}
	return result.join("");
};

BarcoUtil.b64decode = function(text) {
	text = text.replace(/\s/g, "");
	if (!(/^[a-z0-9\+\/\s]+\={0,2}$/i.test(text)) || text.length % 4 > 0) {
		throw new Error("Not a base64-encoded string.");
	}
	//local variables
	var digits = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/",
		cur, prev, digitNum, i = 0,
		result = [];
	text = text.replace(/=/g, "");
	while (i < text.length) {
		cur = digits.indexOf(text.charAt(i));
		digitNum = i % 4;
		switch (digitNum) {
			//case 0: first digit - do nothing, not enough info to work with
			case 1:
				//second digit
				result.push(String.fromCharCode(prev << 2 | cur >> 4));
				break;
			case 2:
				//third digit
				result.push(String.fromCharCode((prev & 0x0f) << 4 | cur >> 2));
				break;
			case 3:
				//fourth digit
				result.push(String.fromCharCode((prev & 3) << 6 | cur));
				break;
		}
		prev = cur;
		i++;
	}
	return result.join("");
};

BarcoUtil.log = function(data) {
	// var callerRef = (new Error).stack.split("\n")[2];
	if (window.console && window.console.log)
		console.log(data /* + callerRef */);
};

BarcoUtil.logger = function(traceLevel, divId, pluginName, methodName, whereInMethod, comments) {
  if (traceLevel <= bLEVEL) {
    var clientTimeStamp = (new Date()).getTime();
    var lev;
    switch (traceLevel) {
      case bERROR:   lev = "ERROR  "; break;
      case bWARNING: lev = "WARNING"; break;
      case bINFO:    lev = "INFO   "; break;
      case bDEBUG:   lev = "DEBUG  "; break;
      default:       lev = "TRACE_" + (traceLevel - bDEBUG);
    }
    var id = (divId && divId.length > 0) ? divId + '.' : "";
    var plugin = (pluginName && pluginName.length > 0) ? pluginName + '.' : "";
    var meth = (methodName && methodName.length > 0) ? methodName + '()' : "";
    var whr = (whereInMethod && whereInMethod.length > 0) ? '<' + whereInMethod + '>' : "";
    var msg = (comments && comments.length > 0) ? ': ' + comments : "";
    var logString = '[' + clientTimeStamp + '] ' + lev + ' ' + id + plugin + meth + whr + msg;
    if (typeof ClientLog === 'object') {
      ClientLog.log(logString); // TODO: create a client log UI
    } else {
      BarcoUtil.log(logString);
    }
  }
};

BarcoUtil.msToHMS = function(playLength) {
  var playLengthStr = "";
  var playLength = parseInt(playLength);
  var OffsetSeconds = parseInt(playLength / 1000);
  var OffsetHr = parseInt(OffsetSeconds / 3600);
  if (OffsetHr < 10)
    OffsetHr = "0" + OffsetHr;
  var OffsetMin = parseInt(OffsetSeconds / 60);
  OffsetMin = OffsetMin % 60;
  if (OffsetMin < 10)
    OffsetMin = "0" + OffsetMin;
  var OffsetSec = OffsetSeconds % 60;
  if (OffsetSec < 10)
    OffsetSec = "0" + OffsetSec;

  playLengthStr = OffsetHr + ":" + OffsetMin + ":" + OffsetSec;
  return playLengthStr;
};

BarcoUtil.msToStr = function(ms) {
  var h, m, s;
  s = Math.floor(ms / 1000);
  m = Math.floor(s / 60);
  s = s % 60;
  h = Math.floor(m / 60);
  m = m % 60;
  var formatedVal = '';
  if (h > 0) {
    var minStr = m >= 10 ? (':' + m) : (':0' + m);
    var secStr = s >= 10 ? (':' + s) : (':0' + s);
    formatedVal = h + minStr + secStr;
  }
  else if (m > 0) {
    var secStr = s >= 10 ? (':' + s) : (':0' + s);
    formatedVal = m + secStr;
  }
  else {
    var minStr = '0';
    var secStr = s >= 10 ? (':' + s) : (':0' + s);
    formatedVal = minStr + secStr;
  }
  return formatedVal;
};

BarcoUtil.convertSizeToStr = function (size) {
  var catSize = Math.log(size) / Math.LN10;
  var formatedVal = '';
  if (catSize < 6) {
    var trimVal = (size / 1024).toFixed(2);
    formatedVal = trimVal + 'K';
  }
  else if (catSize < 9) {
    var trimVal = (size / (1024 * 1024)).toFixed(2);
    formatedVal = trimVal + 'M';
  }
  else if (catSize < 12) {
    var trimVal = (size / (1024 * 1024 * 1024)).toFixed(2);
    formatedVal = trimVal + 'G';
  }
  else if (catSize >= 12) {
    var trimVal = (size / (1024 * 1024 * 1024 * 1024)).toFixed(2);
    formatedVal = trimVal + 'T';
  }
  return formatedVal;
};

BarcoUtil.parseURL = function(url) {
    var queryString = url.split('?');
    if (queryString.length < 2) {
        return {};
    }
    var qObj = {};
    var p = queryString[0].split(':/');
    if (p.length === 2) {
        qObj[p[0]] = p[1].replace(/\+/g, " ");
    }
    var queryArray = queryString[1].split('&');
    if (queryArray.length == 0)
        return {};
    for (var i = 0; i < queryArray.length; ++i) {
        var p = queryArray[i].split('=');
        if (p.length != 2)
            continue;
        qObj[p[0]] = decodeURIComponent(p[1].replace(/\+/g, " "));
    }
    return qObj;
};


/**
* displayMsg: display error Msg
* @param $msgBoardElem {DOM Element} 
* @param msg {String} 
* @param type {String|INFO|WARNING|SUCCESS|ERROR}, no 'type' default ERROR 
*/
BarcoUtil.displayMsg = function ($msgBoardElem, msg, type) {
  var cssClass = 'alert-error';
  if (type == "INFO") {
    cssClass = 'alert-info';
  } else if (type == "WARNING") {
    cssClass = 'alert-warning';
  } else if (type == "SUCCESS") {
    cssClass = 'alert-success';
  }
  $msgBoardElem.removeClass('hidden').addClass(cssClass);
  $msgBoardElem.html(msg).animate({
    opacity: '1'
  }, {
    duration: 6000,
    complete: jQuery.proxy(function () {
      $msgBoardElem.addClass('hidden').removeClass(cssClass);
    }, this)
  });
};

/**
 * Selects string resource to use base on browser language
 * @param {String} defaultLang
 * @param {Object} langs
 * @returns {BarcoUtil.setupLocalization@arr;_langs}
 */
BarcoUtil.setupLocalization = function (langs, defaultLang) {
    var baseLang;
    if (navigator.userLanguage) { // IE case
      baseLang = navigator.userLanguage.substring(0, 2).toLowerCase();
    } else { // All other browsers
      baseLang = navigator.language.substring(0, 2).toLowerCase();
    }
    var resource = langs[baseLang];
    if (resource !== undefined) {
      return resource;
    } else {
      return langs[defaultLang];
    }
  };


