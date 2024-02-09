var PNameSpace = "IPVSAuthTool";
PNameSpace = eval(PNameSpace);
var clsDefaultSettings = null;

function app_InitOnLoad() {
  clsDefaultSettings = new com.ipvs.gui.authtool.DefaultSettings();
}

function app_OnUnLoad() {
  delete clsDefaultSettings;
  clsDefaultSettings = null;
}

//------------------- UUID FNC -------------------------------------
Math.randRange = function(min, max) {
  return Math.max(Math.min(Math.round(Math.random() * max), max), min);
}

function getUUID() {
  var getIntegerBits = function(val, start, end) {
    var quadArray = val.toString(16).toUpperCase().split('');
    var quadString = '';
    for(var i = Math.floor(start / 4);i <= Math.floor(end / 4);i++) {
      if(!quadArray[i] || quadArray[i] == '') quadString += '0';
      else quadString += quadArray[i];
    }
    return quadString;
  }

  var dg = new Date(1582, 10, 15, 0, 0, 0, 0).getTime();
  var dc = new Date().getTime();
  var t = (dg < 0) ? Math.abs(dg) + dc : dc - dg;
  var h = '-';
  var tl = getIntegerBits(t, 0, 31);
  var tm = getIntegerBits(t, 32, 47);
  var thv = getIntegerBits(t, 48, 59) + '1'; // version 1, security version is 2
  var csar = getIntegerBits(Math.randRange(0, 4095), 0, 7);
  var csl = getIntegerBits(Math.randRange(0, 4095), 0, 7);

  /* if nic or at least an IP can be obtained reliably, that should be put in here instead. */
  var n = getIntegerBits(Math.randRange(0, 8191), 0, 7) +
          getIntegerBits(Math.randRange(0, 8191), 8, 15) +
          getIntegerBits(Math.randRange(0, 8191), 0, 7) +
          getIntegerBits(Math.randRange(0, 8191), 8, 15) +
          getIntegerBits(Math.randRange(0, 8191), 0, 15); // this last number is two octets long
  var id = tl + h + tm + h + thv + h + csar + csl + n;

  return id;
}