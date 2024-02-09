/*
* Copyright (c) IP Video Systems Inc. 2012
* 
* internal function begin with prefix '_', like _play();
*
*/
///<reference path="jquery-1.7.1-vsdoc">
(function ($) {

  /** 
  * util function to pause javascript excution a specific time (default ms) 
  **/
  var pausetime = function (ms) {
    ms += new Date().getTime();
    while (new Date() < ms) { }
  };
  /** 
  * util function isBlank to check whhether a empty string 
  **/
  $.isBlank = function (obj) {
    return (!obj || $.trim(obj) === "");
  };
  /** 
  * util function to publish/subscribe event 
  **/
  var topics = $({});
  $.subscribe = function () {
    topics.on.apply(topics, arguments);
  };

  $.unsubscribe = function () {
    topics.off.apply(topics, arguments);
  };

  $.publish = function () {
    topics.trigger.apply(topics, arguments);
  };

  $.log = function (data) {
    if (window.console && window.console.log)
      console.log(data);
  };

  $.getScript("sessionapi.js", function () {
    $.log("sessionapi.js is loaded and executed.");
  });

  /** 
  * API functions
  **/
  var methods = {
    conf: function (opts) {
      //default config
      var defaultConf = {
        fps: 10,
        // http 'true' for rtp
        http: true,

        //player default dimensions
        width: 200,
        height: 200,

        //clip 10000 ms
        skipOffset: 10000,

        //internal properties 
        playerVersion: '3.8.r3.b0',
        loadingPause: 20,
        maxLoadTry: 5
      };
      // repeat over each element in selector
      return this.each(function () {
        var root = $(this);
        conf = root.data('conf') || defaultConf;
        $.extend(conf, opts);
        root.data({ 'conf': conf });
      });
    },
    init: function (opts) {

      return this.each(function () {
        var root = $(this);
		root.data({ 'isMute': false, 'isLoaded': false, 'isHidden': false, 'isPaused': false, 'isPlaying': false });
		methods['conf'].call(root, opts);
		//read player.version and set to conf data
		$.get('player.version',function(data){
				var playerVersion = $.trim(data);
				var versionArr =  $.trim(playerVersion).match(/[1-9].[1-9].r[1-9][0-9].b[1-9][0-9]/) ||
								  $.trim(playerVersion).match(/[1-9].[1-9].r[1-9][0-9].b[0-9]/) ||
								  $.trim(playerVersion).match(/[1-9].[1-9].r[0-9].b[1-9][0-9]/) || 				
								  $.trim(playerVersion).match(/[1-9].[1-9].r[0-9].b[0-9]/);
				if(typeof versionArr != 'undefined' && versionArr != null) {
					var version = versionArr[0];
					//alert('>>>> version[0]'+version);
					methods['conf'].call(root,{ playerVersion: version});
				}
		});
      });
    },
    load: function () {
      return this.each(function () {
        var root = $(this),
					conf = root.data('conf');

        if (!conf) {
          //in case of load - unload - load
          methods['init'].call(root);
          conf = root.data('conf');
        }

        //player dimension can be defined config
        if ($.isBlank(root.attr('style'))) {
          root.css({ width: conf.width, height: conf.height });
        }

        var id = Math.floor((Math.random() * 100000) + 1),
					playerId = 'player_' + id,
					divId = this.id ? this.id : 'div_' + id;
        //keep div and player id
        root.data({ 'id': divId, 'playerid': playerId });
        // objectdiv is nessesary otherwise <script> tag can't directely append to parent div 
        this.innerHTML =
				'<div id="objectdivid" class="objectdiv" >' +
				'	<OBJECT id=' + playerId + ' codeBase=IPVPlayerCtrl.cab#Version=' + conf.playerVersion + ' classid=CLSID:A2A6189E-44F4-4D69-9CE4-8329312D2E3A width="100%" height="100%"> ' +
				'		<PARAM NAME=\"BackColor\" VALUE=\"7874560\"><PARAM NAME=\"BorderColor\" VALUE=\"0\"><PARAM NAME=\"AutoSize\" VALUE=\"0\"> ' +
				'	    <PARAM NAME=\"AutoStart\" VALUE=\"0\"><PARAM NAME=\"SessionId\" VALUE=\"0\"><PARAM NAME=\"Windowless\"  VALUE=\"1\"> ' +
				'	</OBJECT>' +
				'</div> ' +
				'<script FOR="' + playerId + '" EVENT="OnFullScreen(a_bFullScreen)" LANGUAGE="JavaScript">' +
				'	$.publish("onFullscreen",{id:"' + divId + '",type:"Player",message:"Toggle Fullscreen",playerId:"' + playerId + '",flag:a_bFullScreen} );' +
				'</script>';
        //'<script FOR="'+playerId+'" EVENT="OnNativeSizeChange(a_lWidth,a_lHeight)" LANGUAGE="JavaScript">'+
        //'	$.log(" OnNativeSizeChange call back time : "+new Date().getTime());'+
        //'	$.publish("OnNativeSizeChange",{id:"'+divId+'",type:"Player",message:"NativeSizeChange",playerId:"'+playerId+'",width:a_lWidth,height:a_lHeight} );'+
        //'</script>';

        //a little bit more efforts for player loading in case it takes time
        var player = function () {
          var tempRef = null,
			  maxLoadTry = conf.maxLoadTry;

          //Event onLoad
          $.publish("onLoad", { id: divId, playerId: playerId, type: "Event", message: 'On Load' });
          while (!tempRef && maxLoadTry-- >= 0) {
            //ideally document.getElementById should equivalent to $('#mediavideo') but here not! 
            //ref. http://stackoverflow.com/questions/6790948/jquery-inserted-activex-object-tag-having-different-behavior
            tempRef = document.getElementById(playerId);
            pausetime(conf.loadingPause);
          }
          var loaded = tempRef ? true : false;
          root.data({ 'isLoaded': loaded });
          return tempRef;
        } ();

        if (player) {
          root.data({ 'player': player });
          player.Fps = conf.fps;
          player.useHTTP = conf.http;

          //comparing two versions from activeX and configuration
          var currVerionArr = player.Version.split('.');
          var confVerionArr = root.data('conf').playerVersion.split('.');
          if (currVerionArr[0] != confVerionArr[0] ||
						currVerionArr[1] != confVerionArr[1] ||
						currVerionArr[2] != confVerionArr[2] ||
						currVerionArr[3] != confVerionArr[3]) {
            $.publish("onError", { id: divId, playerId: playerId, type: "Player", message: 'Player version config error: run time version different with the config one. ' });
          }

        } else {
          //raise up and handle player loading error		
          var errObj = { id: divId, playerId: playerId, type: "Player", message: 'Fail to Load ActiveX Object' };
          $.publish("onError", errObj);
        }

        /* PlayState used to test activeX alive ready to call player.Play()
        while (typeof player.PlayState == 'undefined')
        {
        pausetime(conf.loadingPause);
        }
        */

      });
    },
    unload: function () {
      return this.each(function () {
        //reset local data 
        var root = $(this),
			sessionInstance = root.data('sessioninstance');
        if (sessionInstance) {
          //stop the sessionInstance if have
          methods['stop'].call(root);
        }
        $.publish("onUnload", { id: root.data('id'), playerId: root.data('playerid'), type: "Event", message: 'On Unload' });
        root.removeData();
        this.innerHTML = '';

        //remove all <script> which src contains '&clientData=' (JSONP nodes)
        $("head script").each(function () {
          if (this.getAttribute("src").search('&clientData=') > 0) {
            this.removeNode();
          };
        })

      });
    },
    play: function (inputURL) {
      return this.each(function () {
        var root = $(this); 
		if(root.data('isPaused')){
			methods['resume'].call(root);
			return;
		};
		if(!root.data('isPlaying')){
			var handleStartedCallback = function (response) { methods['_handleStartedCallback'].call(root, response); },
				handleResponseCallback = function (response) { methods['_handleResponseCallback'].call(root, response); },
				handleEventCallback = function (evtObj) { methods['_handleEventCallback'].call(root, evtObj); },
				handleErrorCallback = function (errObj) { methods['_handleErrorCallback'].call(root, errObj); },
				lcConf = root.data('conf'),
				sessionURL = inputURL ? inputURL : lcConf.controlURL;

			//update inputURL to local 'data'  
			if (!$.isBlank(inputURL)) {
			  $.extend(lcConf, { controlURL: inputURL });
			}
			//load player if it is not being loaded
			if (!root.data('isLoaded')) {
			  methods['load'].call(root);
			};
			//no URL input publish error
			if ($.isBlank(sessionURL)) {
			  var errObj = { id: root.data('id'), playerId: root.data('playerid'), type: "Error", message: "No URL input error." };
			  $.publish("onError", errObj);
			}
			else {
			  var controlURL = methods['_convertURL'].call(root, sessionURL),
				  sessionInstance = new Session(handleStartedCallback, handleResponseCallback, handleEventCallback, handleErrorCallback);

			   root.data({ sessioninstance: sessionInstance, controlurl: controlURL });
			   $.log("Call sessionapi.doStart time: " + new Date().getTime() + " ||  (" + controlURL + ")");
			   sessionInstance && sessionInstance.doStart(controlURL);
			}
		}
		else{
			$.publish("onError", { id: root.data('id'), playerId: root.data('playerid'), type: "Error", message: ' Player is already playing '});
		}
      });
    },
    _play: function (streamURL) {
      //call from _handleStartedCallback when session started
      return this.each(function () {
        var root = $(this),
			sessionInstance = root.data('sessioninstance'),
			player = root.data('player');
        root.data('mediaurl', streamURL);

        //get URL when session started
        player.Filename = streamURL;

        $.log(" Play MediaURL: " + new Date().getTime() + "  URL: " + streamURL + ".");
        player.Play();
      });
    },
    stop: function () {
      return this.each(function () {
        var root = $(this),
			player = root.data('player'),
			sessionInstance = root.data('sessioninstance');
        player && player.Stop();
        sessionInstance && sessionInstance.doStop();
        //reset 'isPaused' & 'isPlaying' for STOP
        root.data({ 'isPaused': false, 'isPlaying': false });
        //clean sessioninstance after STOP
        root.removeData('sessioninstance');
        $.publish("onStop", { id: root.data('id'), playerId: root.data('playerid'), type: "Event", message: "On Stop" });
      });

    },
    skipforward: function (skipoffset) {
      return this.each(function () {
        var root = $(this),
			conf = root.data('conf'),
			offset = skipoffset ? skipoffset : conf ? conf.skipOffset : 0,
			parameter = "SkipFwd.skipOffset=" + offset,
			sessionInstance = root.data('sessioninstance');

        root.data('skipoffset', offset);
        sessionInstance && sessionInstance.sendSessionRequest('SkipFwd', parameter);
      });
    },
    skipbackward: function (skipoffset) {
      return this.each(function () {
        var root = $(this),
			conf = root.data('conf'),
			offset = skipoffset ? skipoffset : conf ? conf.skipOffset : 0,
			parameter = "SkipBack.skipOffset=" + offset,
			sessionInstance = root.data('sessioninstance');

        root.data('skipoffset', offset);
        sessionInstance && sessionInstance.sendSessionRequest('SkipBack', parameter);
      });
    },
    pause: function () {
      return this.each(function () {
        var root = $(this),
			sessionInstance = root.data('sessioninstance');
        sessionInstance && sessionInstance.sendSessionRequest('Pause');
      });
    },
    resume: function () {
      return this.each(function () {
        var root = $(this),
			sessionInstance = root.data('sessioninstance');
        sessionInstance && sessionInstance.sendSessionRequest('Resume');
      });
    },
    catchup: function () {
      return this.each(function () {
        var root = $(this),
			sessionInstance = root.data('sessioninstance');
        sessionInstance && sessionInstance.sendSessionRequest('Catchup');
      });
    },
    fullscreen: function () {
      return this.each(function () {
        var root = $(this),
			player = root.data('player');
        player && player.ToggleDisplay();
      });
    },
    mute: function () {
      return this.each(function () {
        var root = $(this),
			player = root.data('player');
        if (player) {
          player.Mute = true;
          root.data({ 'isMute': true });
          $.publish("onMute", { id: root.data('id'), playerId: root.data('playerid'), type: "Event", message: "Mute " });
        }
      });
    },
    unmute: function () {
      return this.each(function () {
        var root = $(this),
			player = root.data('player');
        if (player) {
          player.Mute = false;
          root.data({ 'isMute': false });
          $.publish("onUnmute", { id: root.data('id'), playerId: root.data('playerid'), type: "Event", message: "Unmute " });
        }
      });
    },
    show: function () {
      var root = $(this),
		  playerdiv = methods['getParent'].call(root);
      playerdiv.show();
      root.data({ 'isHidden': false });
    },
    hide: function () {
      var root = $(this),
		  playerdiv = methods['getParent'].call(root);
      playerdiv.hide();
      root.data({ 'isHidden': true });
    },
    getplayer: function () {
      var root = $(this);
      return root.data('player');
    },
    getcliplength: function () {
      var root = $(this);
      var rawLen = root.data('cliplength');
      return methods['_convertClipLength'].call(root, rawLen);
    },
    id: function () {
      var root = $(this);
      return root.data('id');
    },
    getParent: function () {
      var root = $(this);
      return root;
    },
    getVersion: function () {
      var root = $(this),
		  player = root.data('player'),
		  version = player ? player.Version : 'UNKNOWN';
      return version;
    },
	setVersion: function (Obj) {
      var root = $(this);
	   if(Obj && Obj.version) {
			methods['conf'].call(root,{ playerVersion: Obj.version});
			return true;
	   }
	   else{
			return false;
	   }
    },
    getPlayerId: function () {
      var root = $(this);
      return root.data('playerid');
    },
    getMediaURL: function () {
      var root = $(this);
      return root.data('mediaurl');
    },
    isLoaded: function () {
      var root = $(this);
      return root.data('isLoaded');
    },
    isPaused: function () {
      var root = $(this);
      return root.data('isPaused');
    },
    isPlaying: function () {
      var root = $(this);
      return root.data('isPlaying');
    },
    isHidden: function () {
      var root = $(this);
      return root.data('isHidden');
    },
    _handleStartedCallback: function (response) {
      return this.each(function () {
        var root = $(this),
			playerid = root.data('playerid'),
			controlURL = root.data('controlurl'),
			streamURL = response.mediaURL;
        //started a session and call '_play' with a streamURL in response 
        methods['_play'].call(root, streamURL);
        $.publish("onStart", { id: root.data('id'), playerId: playerid, type: "Event", message: "On Start" });
        root.data({ 'isPlaying': true });

        //parse session XML
        var sessionXml = response.data,
					sessionXmlDoc = $.parseXML(sessionXml),
					$sessionXml = $(sessionXmlDoc).find('Session');

        //Get session Timeline
        var sessionTimelineId = $sessionXml.attr('timelineId');

        $sessionXml.find('Timeline').each(function () {
          var timelinId = $(this).attr('id');
          if (timelinId == sessionTimelineId) {
            var startElement = $(this).find('Start'),
				//get 'WC' and 'offset' from 'Start' element							
				wc = startElement.attr('WC'),
				offset = startElement.attr('Offset'),
				//get 'timeScale' and 'requestedAction' from 'Last' element
				lastElement = $(this).find('Last'),
				timeScale = lastElement.attr('TimeScale'),
				requestedAction = lastElement.attr('requestedAction');
            //pub 'timeLine' event at start time
            $.publish("onTimeLine", { id: id, playerId: playerid, type: "Event", message: { wc: wc, offset: offset, timeScale: timeScale, requestedAction: requestedAction} });
          }
        });

        //Get current playback file length
        $sessionXml.find('File').each(function () {
          var resourceNID = $(this).attr('resourceNID');
          if (controlURL.search(resourceNID) >= 0) {
            var clipLength = $(this).attr('playLength');
            //keep file length
            root.data('cliplength', clipLength);
          }
        });
      });
    },
    _handleResponseCallback: function (response) {
      return this.each(function () {
        //get general info. of player
        var root = $(this),
			id = root.data('id'),
			playerid = root.data('playerid'),
			sessionInstance = root.data('sessioninstance'),
			//error info. of session response 
			responseXmlDoc = $.parseXML(response.data),
			$responseXml = $(responseXmlDoc),
			sessionId = sessionInstance ? sessionInstance.getSessionId() : null,
			error = sessionId ? '' : " There isn't any session ";

        $responseXml.find('ipvserror').each(function () {
          var errorcode = $(this).attr('code'),
						description = $(this).find('description').text();
          error += ' Error code: ' + errorcode + '; Description: ' + description + '.';
        });

        //publish error in response error (not fatal so continue)
        if (!$.isBlank(error)) {
          $.publish("onError", { id: id, playerId: playerid, type: "Error", message: error });
        };

        /** Timeline sample
        * <SkipBack>
        *   <Timeline sessionId="948eba60-01c5-46c6-bfe3-b6c91180632e" connGroupId="" connId="" id="fbce5277-4bf2-4da3-a626-397ed6d17969.StreamTrack">
        *       <Start WC="1353519733891" Offset="0" parentTimelineId=""/>
        *       <Last WC="1353519759596" Offset="15705" TimeScale="1.0" requestedAction="SkipBackward"/>
        *   </Timeline>
        * </SkipBack>
        **/
        $responseXml.find('Timeline').each(function () {
          //by default only one PVR response per 'Timeline' element 
          if ($(this).attr('sessionId') == sessionId) {
            if (response.requestName == "Pause" || response.requestName == "Resume" || response.requestName == "Catchup") {
              //set 'isPaused' true and publish 'onPause' event
              root.data({ 'isPaused': (response.requestName == "Pause"), 'isPlaying': (response.requestName != "Pause") });
              $.publish("on" + response.requestName, { id: id, playerId: playerid, type: "Event", message: response.requestName });
            }
            else if (response.requestName == "SkipFwd" || response.requestName == "SkipBack") {
              root.data({ 'isPaused': false, 'isPlaying': true });
              var skipoffset = root.data('skipoffset');
              $.publish("onSeek", { id: root.data('id'), playerId: root.data('playerid'), type: "Event", message: response.requestName + ', ' + skipoffset + ' ms' });
            }

            // 'offset' from 'last' element 
            var lastElement = $(this).find('Last'),
								wc = lastElement.attr('WC'),
								offset = lastElement.attr('Offset'),
								timeScale = lastElement.attr('TimeScale'),
								requestedAction = lastElement.attr('requestedAction');
            //pub 'timeLine' event for slider sync.
            $.publish("onTimeLine", { id: id, playerId: playerid, type: "Event", message: { wc: wc, offset: offset, timeScale: timeScale, requestedAction: requestedAction} });

          }
        });


      });
    },
    _handleEventCallback: function (eventResponse) {
      return this.each(function () {
        //process event response
        var root = $(this),
					responseXmlDoc = $.parseXML(eventResponse.data),
					$responseXml = $(responseXmlDoc);

        /**
        * <notificationList>
        *  <x xmlns='com.ipvs.session'>
        *      <ConnState sessionId="59eda28c-e35e-4462-8674-8972aa3422b6" connId="myConn5678" value="END_OF_MEDIA">
        *          <Timeline id="bdcb148d-9dc6-429b-9101-fd42d286f464">
        *              <Start Offset="" TC="1352423287577" WC="1352735070525" parentTimelineId=""/>
        *              <Last Offset="1154" TC="1352423287577" TimeScale="1.0" WC="1352735071258" requestedAction="Start"/>
        *         </Timeline>
        *      </ConnState>
        *  </x>
        * </notificationList>
        **/
        //END_OF_MEDIA event 
        $responseXml.find('ConnState').each(function () {
          var sessionId = $(this).attr('sessionId'),
						id = root.data('id'),
						playerid = root.data('playerid'),
						connId = $(this).attr('connId'),
						value = $(this).attr('value'),
						timeLineLast = $(this).find('Last'),
						offset = timeLineLast ? timeLineLast.attr('Offset') : 0,
						tc = timeLineLast ? timeLineLast.attr('TC') : 0,
						timeScale = timeLineLast ? timeLineLast.attr('TimeScale') : "0.0",
						wc = timeLineLast ? timeLineLast.attr('WC') : 0,
						requestedAction = timeLineLast ? timeLineLast.attr('requestedAction') : "UNKOWN";
          if (!timeLineLast) {
            var errObj = { id: id, playerId: playerid, sessionId: sessionId, connId: connId, type: "Error", message: "END_OF_MEDIA timeLine element error." };
            $.publish("onError", errObj);
          }
          else {
            //  <Last Offset="1154" TC="1352423287577" TimeScale="1.0" WC="1352735071258" requestedAction="Start"/> plus 'value' from <ConnState>
            $.publish("onConnState", { id: id, playerId: playerid, sessionId: sessionId, connId: connId, type: "Event", message: { offset: offset, tc: tc, timeScale: timeScale, wc: wc, requestedAction: requestedAction, value: value} });
          }
        });

      });
    },
    _handleErrorCallback: function (response) {
      return this.each(function () {
        var root = $(this);
        var playerid = root.data('playerid'),
					id = root.data('id'),
					errObj = { id: id, playerId: playerid, type: "Error", code: response.errorCode, message: response.data };
        $.publish("onError", errObj);
      });
    },
    _convertURL: function (rawURL) {
      //convert URL from getFile/getPort to proper mediaURL like
      //http://10.51.51.151/clients/player?requestName=Playback&amp;SrcFile.fileNID=...
      //TO: http://10.51.51.151/controller/session?requestName=Playback&amp;SrcFile.fileNID=...
      var mediaURL = rawURL;
      if (rawURL.search("/clients/player?") >= 0) {
        mediaURL = rawURL.replace("/clients/player?", "/controller/session?");
      }
      return mediaURL;
    },
    _convertClipLength: function (cliplength) {
      var playLengthStr = "00:00:00";
      var playLength = parseInt(cliplength, 10);
      var plSeconds = parseInt(playLength / 1000, 10);
      var plHr = parseInt(plSeconds / 3600, 10);
      if (plHr < 10)
        plHr = "0" + plHr;
      var plMin = parseInt(plSeconds / 60, 10);
      plMin = plMin % 60;
      if (plMin < 10)
        plMin = "0" + plMin;
      var plSeconds = plSeconds % 60;
      if (plSeconds < 10)
        plSeconds = "0" + plSeconds;

      playLengthStr = plHr + ":" + plMin + ":" + plSeconds;
      return playLengthStr;
    }

  };


  /** 
  * player plugin 
  **/
  $.fn.player = function () {
    //instance varible 
    // Grab our method, sadly if we used function(method){}, it ruins it all
    var method = arguments[0];
    var opts = arguments[1];

    // Check if the passed method exists
    if (methods[method]) {
      method = methods[method];

      // If the method is not found, check if the method is an object (JSON Object) or one was not sent.
    } else if (typeof (method) == 'object' || !method) {

      // If we passed parameters as the first object or no arguments, just use the "init" methods
      method = methods.init;
    } else {

      // Not a method and not parameters, so return an error.  Something wasn't called correctly.
      $.error('Method "' + method + '" does not exist in jQuery.player');
      return this;
    }

    // Call our selected method
    // Once again, note how we move the "each()" from here to the individual methods
    return opts ? method.call(this, opts) : method.call(this);

  };


})(jQuery);

