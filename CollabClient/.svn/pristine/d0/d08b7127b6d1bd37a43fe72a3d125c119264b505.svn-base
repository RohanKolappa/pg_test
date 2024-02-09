<!DOCTYPE html>
<html>
  <head>
    <meta http-equiv="X-UA-Compatible" content="IE=EDGE" />
    <title>Simple Media Player</title>
    <link rel="stylesheet" href="./resource/PlayerPage.css" type="text/css">
    <link rel="stylesheet" href="./resource/PlayerMessagePanel.css" type="text/css">
    <link rel="stylesheet" href="./resource/PlayerControlBar.css" type="text/css">
    <link rel="stylesheet" href="./resource/PlayerScrubBar.css" type="text/css">
    <link rel="stylesheet" href="./resource/PlayerBookmarkPanel.css" type="text/css">
    <link rel="stylesheet" href="./resource/PlayerZoomPanel.css" type="text/css">
    <link rel="stylesheet" href="./resource/PlayerTrimPanel.css" type="text/css">
    <link rel="stylesheet" href="./resource/PlayerControlButtons.css" type="text/css">
    <style id="customSkin"></style>
    <script src="./jquery/jquery-1.11.0.js" type="text/javascript"></script>
    <script src="./jquery/jquery-ui-1.10.4.slider.js" type="text/javascript"></script>	
    <script src="./jquery/jquery.fullscreen-0.4.1.js" type="text/javascript"></script>
    <script src="./guiAPIAgent/scripts/BarcoUtil.js" type="text/javascript"></script>
    <script src="./guiAPIAgent/scripts/Request.js" type="text/javascript"></script>	
    <script src="./guiAPIAgent/scripts/ServiceAgent.js" type="text/javascript"></script>
    <script src="./guiAPIAgent/scripts/SessionAgent.js" type="text/javascript"></script>
    <script src="./guiAPIAgent/scripts/BarcoAPI.js" type="text/javascript"></script>
    <script src="./guiAPIAgent/scripts/SessionInfo.js" type="text/javascript"></script>
    <script src="./guiAPIAgent/scripts/SessionAPI.js" type="text/javascript"></script>
    <script src="./js/plugin-inheritance.js" type="text/javascript"></script>	
    <script src="./js/plugin-detect.js" type="text/javascript"></script>	
    <script src="./js/SessionBookmarksList.js" type="text/javascript"></script>
    <script src="./js/MessagePanel.js" type="text/javascript"></script>	
    <script src="./js/SessionSlider.js" type="text/javascript"></script>
    <script src="./js/EditMediaPanel.js" type="text/javascript"></script>
    <script src="./js/SessionController.js" type="text/javascript"></script>
    <script src="./js/SessionCtrlButtonPanel.js" type="text/javascript"></script>
    <script src="./js/SessionCtrlBookmarkPanel.js" type="text/javascript"></script>
    <script src="./js/SessionCtrlZoomPanel.js" type="text/javascript"></script>
    <script src="./js/WindowCtrlButtonPanel.js" type="text/javascript"></script>
    <script src="./js/StreamPlayer.js" type="text/javascript"></script>
    <script src="./js/StreamPlayerChromelessHLS.js" type="text/javascript"></script>
    <script src="./js/StreamPlayerHTML5videoHLS.js" type="text/javascript"></script>
    <script src="./js/SessionPlayer.js" type="text/javascript"></script>
    <script src="./js/Skinner.js" type="text/javascript"></script>
    <script src="./js/PlayerSelection.js" type="text/javascript"></script>
    <script src="./js/BarcoPlayer.js" type="text/javascript"></script>

  </head>
  <body>
    <!-- essential requirement; 'id' is optional -->
    <div id="ifp" class="barcoplayer" style="width:100%; height:100%;"></div>

    <script>
      $(document).ready(function() {
        // get conf query params from URL
        var currentURL = document.URL;
        var urlObj = BarcoUtil.parseURL(currentURL);
        var confObj = {};
        for (var key in urlObj) {
          if (key.indexOf('confOpt_') >= 0) {
            var option = key.replace('confOpt_', '');
            if (urlObj[key] != 'null') {
              confObj[option] = urlObj[key];
            }
          }
        }
        // load the player class
        var barcoplayerDiv = $('div.barcoplayer');
        //barcoplayerDiv.height(window.innerHeight);
        barcoplayerDiv.player(confObj);

    // set (and play) the media if mediaURL is available
        if (urlObj.requestName) {
          var mediaURL = currentURL.replace("/gui/player/src/player.php", "/player");
          barcoplayerDiv.player('play', mediaURL);
        }

    // receive messages from parent
        function _receiveMessage(event) {
          // make sure message is coming from parent
          var data = event.data ? JSON.parse(event.data)[0] : null;
          var arg1 = event.data ? JSON.parse(event.data)[1] : null;
          if (data == 'conf') {
            confObj = arg1;
          }
          if (event.origin.indexOf(confObj.playerParentHost) < 0) {
            BarcoUtil.log('BarcoPlayerInterface_receiveMessage skip ' + event.origin);
            return;
          }
          if (data == 'conf') {
            barcoplayerDiv.player('conf', confObj);
          }
          if (data == 'authenticate') {
            barcoplayerDiv.player('authenticate', arg1);
          }
          else if (data == 'play') {
            barcoplayerDiv.player('play');
          }
          else if (data == 'stop') {
            barcoplayerDiv.player('stop');
          }
          else if (data == 'pause') {
            barcoplayerDiv.player('pause');
          }
          else if (data == 'resume') {
            barcoplayerDiv.player('resume');
          }
          else if (data == 'skipforward') {
            barcoplayerDiv.player('skipforward', arg1);
          }
          else if (data == 'skipbackward') {
            barcoplayerDiv.player('skipbackward', arg1);
          }
          else if (data == 'catchup') {
            barcoplayerDiv.player('catchup');
          }
          else if (data == 'seekTo') {
            barcoplayerDiv.player('seekTo', arg1);
          }
          else if (data == 'mute') {
            barcoplayerDiv.player('mute');
          }
          else if (data == 'unmute') {
            barcoplayerDiv.player('unmute');
          }
          else if (data == 'zoom') {
            barcoplayerDiv.player('zoom', arg1);
          }
          else if (data == 'setLocalKBMState') {
            barcoplayerDiv.player('setLocalKBMState', arg1);
          }
          else if (data == 'toggleScreen') {
            barcoplayerDiv.player('toggleScreen');
          }
          else if (data == 'snapShot') {
            barcoplayerDiv.player('snapShot');
          }
        }

        // send messages to parent
        function _sendMessage() {
          if (window.parent) {
            window.parent.postMessage(JSON.stringify(arguments), '*');
          }
        }

        function _stashVersion() {
          var version = barcoplayerDiv.player('getVersion');
          _sendMessage({'version': version});
        }

        jQuery.subscribe("onStart", _sendMessage);
        jQuery.subscribe("onStop", _sendMessage);
        jQuery.subscribe("onMute", _sendMessage);
        jQuery.subscribe("onUnmute", _sendMessage);
        jQuery.subscribe("onPause", _sendMessage);
        jQuery.subscribe("onResume", _sendMessage);
        jQuery.subscribe("onCatchup", _sendMessage);
        jQuery.subscribe("onSeek", _sendMessage);
        jQuery.subscribe("onFullscreen", _sendMessage);
        jQuery.subscribe("onError", _sendMessage);
        jQuery.subscribe("onAuthorizing", _sendMessage);
        jQuery.subscribe("onStreamPlayerReady", _stashVersion);
        if (window.addEventListener) {
          addEventListener("message", _receiveMessage, false)
        } else {
          attachEvent("onmessage", _receiveMessage)
        }
        _sendMessage({'ready': true, id: urlObj.id});
      });

    </script>
  </body>
</html>
