/*
 * Copyright (c) Barco Inc.
 * Use, modification, and distribution subject to terms of license.
 */
/*
 * PlayerSelection - Class definition 
 * - registered as a jQuery plugin
 */
var PlayerSelection = Class.extend({
  init: function(options, elem) {
    // Mix in the passed-in options with the default options
    this.options = jQuery.extend({}, this._defaults, options);
    this.log(bDEBUG, "init", "call", "");
    // Init state
    // Save the element reference, both as a jQuery
    // reference and a normal reference
    this.elem = elem;
    this.$elem = jQuery(elem);
    this.cssInsertPos = 0;
    this.message = null;
    this.requiredPlayerVersion = '%PLAYER.VERSION2%';
    // Build the DOM's initial structure
    var os = 'other';
    os = navigator.userAgent.match(/(Windows NT).*/i) == null ? os : 'PC';
    os = navigator.userAgent.match(/(iPad|iPhone|iPod touch);.*/i) == null ? os : 'iOS';
    os = navigator.userAgent.match(/(Intel Mac OS X).*/i) == null ? os : 'Mac';
    os = navigator.userAgent.match(/Android .*/i) == null ? os : 'Android';
    this.options.platform = os;
    this.browserCapabilities = PluginDetect.getBrowserCapabilities('AUTO' /* this.options.playerPref */);
    this._build();
  },
  _defaults: {
  },
  _initPlayerSelectionRules: function() {
    this.log(bDEBUG, "_initPlayerSelectionRules", "call", "");
    // Missing properties are treated as don't-cares
    this._playerSelectionRules = {
      start: {
        nextIfPass: "isIE9x64"
      },
      isIE9x64: {
        conditions: {
          isMediaIPVPlayerCompatible: true,
          isMediaChromelessCompatible: false,
          isIE9x64: true,
        },
        actions: ["UseIE9x32"],
        nextIfPass: "IPVOnly"
      },
      IPVOnly: {
        conditions: {
          isMediaIPVPlayerCompatible: true,
          isMediaChromelessCompatible: false,
          isIE9x64: false,
          isIPVPlayer32Installable: true,
        },
        actions: ["MaybeInstallIPV", "LoadIPV"],
        nextIfPass: "IPV"
      },
      IPV: {
        conditions: {
          playerPref: "AUTO",
          isMediaIPVPlayerCompatible: true,
          isIE9x64: false,
          isIPVPlayer32Installable: true,
        },
        actions: ["MaybeInstallIPV", "LoadIPV"],
        nextIfPass: "Flash"
      },
      Flash: {
        conditions: {
          isMediaChromelessCompatible: true,
          isFlashInstallable: true,
        },
        actions: ["MaybeInstallFlash", "LoadChromeless"],
        nextIfPass: "HTML5hls"
      },
      HTML5hls: {
        conditions: {
          isMediaHTML5hlsCompatible: true,
          isHTML5hlsInstallable: true,
        },
        actions: ["LoadHTML5"],
        nextIfPass: "IPVMediaUnsupportedWin"
      },
      IPVMediaUnsupportedWin: {
        conditions: {
          platforms: ["PC"],
          isMediaChromelessCompatible: false,
          isIPVPlayer32Installable: false,
        },
        actions: ["IPVMediaUnsupportedWin"],
        nextIfPass: "IPVMediaUnsupportedOther"
      },
      IPVMediaUnsupportedOther: {
        conditions: {
          platforms: ["Mac","iOS","Android", "other"],
          isMediaHTML5hlsCompatible: false,
        },
        actions: ["IPVMediaUnsupportedOther"],
        nextIfPass: "done"
      },
    };
  },
  _build: function() {
    this.log(bDEBUG, "_build", "call", "");
    this._initPlayerSelectionRules();
    this.log(bINFO, "_build", "return", "Done");
  },
  getPlatform: function() {
    this.log(bDEBUG, "getPlatform", "call", "");
    return this.options.platform;
  },
  isIE9x64: function() {
    this.log(bDEBUG, "isIE9x64", "call", "");
    return (navigator.userAgent.indexOf(" MSIE 9.") > 0 &&
            navigator.userAgent.indexOf(" x64;") > 0);
  },
  isFlashInstallable: function() {
    this.log(bDEBUG, "isFlashInstallable", "call", "");
    return ((this.options.platform === 'PC') ||
            (this.options.platform === 'Mac'));
  },
  isIPVPlayer32Installable: function() {
    this.log(bDEBUG, "isIPVPlayer32Installable", "call", "");
    return this.browserCapabilities.activeXSupported && !this.isIE9x64();
  },
  isHTML5hlsInstallable: function() {
    this.log(bDEBUG, "isHTML5hlsInstallable", "call", "");
    // The Chrome UA also lists Safari so we have to filter it out
    var safari = ((this.options.platform === 'iOS' || this.options.platform === 'Mac')) &&
                   navigator.userAgent.match(/Safari\/.*/i) != null && 
                   navigator.userAgent.match(/CriOS\/.*/i) == null &&
                   navigator.userAgent.match(/Chrome\/.*/i) == null;
    // The Andriod test needs more refinement.
    return safari || this.options.platform === 'Android';
  },
  isMediaIPVPlayerCompatible: function() {
    this.log(bDEBUG, "isMediaIPVPlayerCompatible", "call", "");
    return (this.options.playerURL.indexOf(".m3u8") < 0);
  },
  isMediaChromelessCompatible: function() {
    this.log(bDEBUG, "isMediaChromelessCompatible", "call", "");
    return (this.options.playerURL.indexOf(".m3u8") > 0 || this.options.playerURL.indexOf("fallback=true") > 0);
  },
  isMediaHTML5hlsCompatible: function() {
    this.log(bDEBUG, "isMediaHTML5hlsCompatible", "call", "");
    return (this.options.playerURL.indexOf(".m3u8") > 0 || this.options.playerURL.indexOf("fallback=true") > 0);
  },
  isFlashLoadable: function() {
    this.log(bDEBUG, "isFlashLoadable", "call", "");
    return this.browserCapabilities.flashPlayerInstalled;
  },
  isIPVPlayer32Loadable: function() {
    this.log(bDEBUG, "isIPVPlayer32Loadable", "call", "");
    return this.browserCapabilities.iPVPlayerInstalled;
  },
  isIPVPlayer32UpToDate: function() {
    if (this.browserCapabilities.iPVPlayerInstalled) {
      var matchVersion = this.requiredPlayerVersion.replace(/,/g, '.');
      if (matchVersion === '%PLAYER.' + 'VERSION2%') { // for localhost debugging
        matchVersion = this.browserCapabilities.iPVPlayerVersion;
      }
      return (this.browserCapabilities.iPVPlayerVersion === matchVersion);
    }
    return false;
  },
  isHTML5hlsLoadable: function() {
    this.log(bDEBUG, "isHTML5hlsLoadable", "call", "");
    return this.isHTML5hlsInstallable();
  },
  isActiveXFilteringOn: function() {
    this.log(bDEBUG, "isActiveXFilteringOn", "call", "");
    return this.browserCapabilities.activeXSupported && this.browserCapabilities.activeXFiltering;
  },
  isActiveXDisabled: function() {
    this.log(bDEBUG, "isActiveXDisabled", "call", "");
    return this.browserCapabilities.activeXSupported && !this.browserCapabilities.activeXEnabled;
  },
  _matchInputsToRuleConditions: function(conditions) {
    this.log(bDEBUG, "_doConditionsMatch", "call", "");
    var matches = true; 
    var i = 0;
    if (matches && conditions.playerPref != undefined) {
      matches = conditions.playerPref === this.options.playerPref;
    }
    if (matches && conditions.platforms !== undefined) {
      var platformMatch = false;
      for (i = 0; i < conditions.platforms.length; i++) {
        if (this.options.platform === conditions.platforms[i]) {
          platformMatch = true;
          break;
        }
      }
      matches = platformMatch;
    }
    if (matches && conditions.isIE9x64 !== undefined) {
      matches = conditions.isIE9x64 === this.isIE9x64();
    }
    if (matches && conditions.isIPVPlayer32Installable !== undefined) {
      matches = conditions.isIPVPlayer32Installable === this.isIPVPlayer32Installable();
    }
    if (matches && conditions.isFlashInstallable !== undefined) {
      matches = conditions.isFlashInstallable === this.isFlashInstallable();
    }
    if (matches && conditions.isHTML5hlsInstallable !== undefined) {
      matches = conditions.isHTML5hlsInstallable === this.isHTML5hlsInstallable();
    }
    if (matches && conditions.isMediaIPVPlayerCompatible !== undefined) {
      matches = conditions.isMediaIPVPlayerCompatible === this.isMediaIPVPlayerCompatible();
    }
    if (matches && conditions.isMediaChromelessCompatible !== undefined) {
      matches = conditions.isMediaChromelessCompatible === this.isMediaChromelessCompatible();
    }
    if (matches && conditions.isMediaHTML5hlsCompatible !== undefined) {
      matches = conditions.isMediaHTML5hlsCompatible === this.isMediaHTML5hlsCompatible();
    }
    return matches;
  },
  _getPlayerActions: function() {
    this.log(bDEBUG, "_getPlayerAction", "call", "");
    var currentRuleName = this._playerSelectionRules.start.nextIfPass;
    while (true) {
      var rule = this._playerSelectionRules[currentRuleName];
      var matches = this._matchInputsToRuleConditions(rule.conditions);
      if (matches) return rule.actions;
      else {
        currentRuleName = rule.nextIfPass;
        if (currentRuleName == "done") return ["MoMatchingRules"];
      }
    }
  },
  _showError: function(text) {
    this.message = jQuery('<p style="padding-top: 100px">' + text + '</p>').addClass('PlayerDetection');
  },
  _doOtherActiveXActions: function() {
    if (this.isActiveXFilteringOn()) {
      this.message = jQuery('<p>ActiveX filtering is on. If you want to use any of<br>' +
        'the video player add-ons this setting needs to be turned off:<br>' +
        'go to Tools > Internet Options > Safety > Active Filtering to turn off.</p>').addClass('PlayerDetection');
    }
    if (this.isActiveXDisabled()) {
      this.message = jQuery('<p">ActiveX add-on use is disabled.<br>' +
        'If you want to use any of the video player add-ons<br>' +
        'this setting needs to be turned on:<br>' +
        'go to Tools > Internet Options > Security > Local internet > Custom level...<br>' +
        'Scroll down to the ActiveX Controls and plug-ins and ' +
        'Enable each option that makes sense for your environment.</p>').addClass('PlayerDetection');
    }
  },
  _doUseIE9x32Action: function() {
    this.log(bDEBUG, "_doUseIE9x32Action", "call", "");
    this.message = jQuery('<p>Media not supported in this browser.<br>Try 32-bit IE 9 or any edition of IE 10+.</p>').addClass('PlayerDetection');
    this.errorEvent = {type: "Error", message: ' IE 9 x64 cannot load IPVPlayer '};
    return true;
  },
  _doMaybeInstallIPVAction: function() {
    this.log(bDEBUG, "_doMaybeInstallIPVAction", "call", "");
    if (!this.isIPVPlayer32Loadable()) {
      this._doOtherActiveXActions();
      this.message = jQuery('<p>This browser supports a Barco add-on that can<br>' +
        'play this media. Install with the following steps:<br>' + 
        'Login as Administrator. Reload this page.<br>' +
        'Then click on the icon below to install the IPVPlayer add-on.<br>' +
        '<a id="InstallIPV" href="js/LoadIPVPlayerObject.html?url=' + encodeURIComponent(document.URL) + '">' +
        '<img src="resource/barco60x40.png" border="0" alt="Get Barco ActiveX IPV Player plug-in" />' +
        '</a><br>Or, recheck that it has not been disabled<br>in the Manage Add-ons dialog under Tools.</p>').addClass('PlayerDetection');
      this.errorEvent = {type: "Error", message: ' Need to install IPVPlayer '};
      return true;
    } else if (this.isIPVPlayer32Loadable() && !this.isIPVPlayer32UpToDate()) {
      this.message = jQuery('<p>A newer version of a Barco add-on that can play this<br>' +
        'media is available. Install with the following steps:<br>' + 
        'Login as Administrator. Reload this page.<br>' +
        'Then click on the icon below to install the IPVPlayer add-on.<br>' +
        '<a id="InstallIPV" href="js/LoadIPVPlayerObject.html?url=' + encodeURIComponent(document.URL) + '">' +
        '<img src="resource/barco60x40.png" border="0" alt="Get Barco ActiveX IPV Player plug-in" />' +
        '</a></p>').addClass('PlayerDetection');
      this.errorEvent = {type: "Error", message: ' Need to update IPVPlayer '};
      return true;
    }
    return false;
  },
  _doLoadIPVAction: function() {
    this.log(bDEBUG, "_doLoadIPVAction", "call", "");
    this.options.playerUsed = 'streamPlayerIPV';
    return false;
  },
  _doMaybeInstallFlashAction: function() {
    this.log(bDEBUG, "_doMaybeInstallFlashAction", "call", "");
    if (!this.isFlashLoadable()) {
      this._doOtherActiveXActions();
      this.message = jQuery('<p>This browser supports a Barco Flash Player<br>' +
        'plug-in that can play this media.<br>' +
        'However, the Flash Player plug-in is missing or disabled.<br>' + 
        'Install with the following steps:<br>' +
        'Login as Administrator. Reload this page.<br>' +
        'Then click on the icon below to install the Flash Player plug-in.<br>' +
        '<a href="http://www.adobe.com/go/getflash">' +
        '<img src="http://www.adobe.com/images/shared/download_buttons/get_flash_player.gif" border="0" alt="Get Adobe Flash player" />' +
        '</a><br>Or, recheck that it has not been disabled in your browser.</p>' +
        '<p style="color: grey;">Flash Player version 10.2.0 or higher required.</p>').addClass('PlayerDetection');
      this.errorEvent = {type: "Error", message: ' Need to install Flash player '};
      return true;
    }
    return false;
  },
  _doLoadChromeless: function() {
    this.log(bDEBUG, "_doLoadChromeless", "call", "");
    this.options.playerUsed = 'streamPlayerChromelessHLS';
    return false;
  },
  _doLoadHTML5: function() {
    this.log(bDEBUG, "_doLoadHTML5", "call", "");
    this.options.playerUsed = 'streamPlayerHTML5videoHLS';
    return false;
  },
  _doIPVMediaUnsupportedWinAction: function() {
    this.log(bDEBUG, "_doIPVMediaUnsupportedWinAction", "call", "");
    this._showError("Media not supported in this browser.<br>Try 32-bit IE 9 or any edition of IE 10+.");
    this.errorEvent = {type: "Error", message: ' Media unsupported '};
    return true;
  },
  _doIPVMediaUnsupportedOtherAction: function() {
    this.log(bDEBUG, "_doIPVMediaUnsupportedOtherAction", "call", "");
    this._showError("Media not supported in this browser.");
    this.errorEvent = {type: "Error", message: ' Media unsupported '};
    return true;
  },
  _doMoMatchingRulesAction: function() {
    this.log(bDEBUG, "_doMoMatchingRulesAction", "call", "");
    this.errorEvent = {type: "Error", message: ' No matching player selection rule '};
  },
  doPlayerSelectionActions: function() {
    var abortPlay = false;
    var actions = this._getPlayerActions();
    var len = actions.length;
    this.message = null;
    for (var i = 0; i < len; i++) {
      switch (actions[i]) {
        case 'UseIE9x32':
          abortPlay = this._doUseIE9x32Action();
          break;
        case 'MaybeInstallIPV':
          abortPlay = this._doMaybeInstallIPVAction();
          break;
        case 'LoadIPV':
          abortPlay = this._doLoadIPVAction();
          break;
        case 'MaybeInstallFlash':
          abortPlay = this._doMaybeInstallFlashAction();
          break;
        case 'LoadChromeless':
          abortPlay = this._doLoadChromeless();
          break;
        case 'LoadHTML5':
          abortPlay = this._doLoadHTML5();
          break;
        case 'IPVMediaUnsupportedWin':
          abortPlay = this._doIPVMediaUnsupportedWinAction();
          break;
        case 'IPVMediaUnsupportedOther':
          abortPlay = this._doIPVMediaUnsupportedOtherAction();
          break;
        case 'MoMatchingRules':
          this._doMoMatchingRulesAction();
        default:
          abortPlay = true;
      }
      if (abortPlay) break;
    }
    return abortPlay;
  },
  getPlayerUsed: function() {
    return this.options.playerUsed;
  },
  showMessage: function() {
    this.$elem.append(this.message);
  },
  getErrorEvent: function() {
    return this.errorEvent
  },
  destroy: function(pluginName) {
    this.log(bDEBUG, "destroy", "call", pluginName);
    this.$elem.removeData(pluginName);
  }
});

// expose Objects as jQuery plugins
jQuery.plugin('playerSelection', PlayerSelection);