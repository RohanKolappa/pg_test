
//Player states
var IPVSPlayerState_STOPPED = 0
var IPVSPlayerState_PAUSED  = 1
var IPVSPlayerState_PLAYING = 2

function playerInitDefaults() {
  // setup default Preferences
  var selViewerAspectRatio = getLocalPreferences(PNameSpace, ".//AspectRatio", "4:3");
  var selViewerFps = getLocalPreferences(PNameSpace, ".//Fps", 25);
  var selViewerSkipFWD = getLocalPreferences(PNameSpace, ".//SkipFWD", 30000);
  var selViewerSkipBack = getLocalPreferences(PNameSpace, ".//SkipBack", 10000);
  var selViewerBandwidth = getLocalPreferences(PNameSpace, ".//BandwidthKbps", 10240);
  var selViewerChkUseHTTP = getLocalPreferences(PNameSpace, ".//UseHTTP", 1);
  var selViewerChkUseMMS = getLocalPreferences(PNameSpace, ".//UseMMS", 0);

 
  //new added 
  var IPVSPlayerDefaultServerPort = getLocalPreferences(PNameSpace, ".//ServerPort", "6060");
  var IPVSPlayerDefaultAVOption = getLocalPreferences(PNameSpace, ".//AVOption", 3);
  var IPVSPlayerDefaultMulticast = getLocalPreferences(PNameSpace, ".//Multicast", 0);
  var IPVSPlayerDefaultSliceReduction = getLocalPreferences(PNameSpace, ".//SliceReduction", 1);
  var IPVSPlayerDefaultEnableKbm = getLocalPreferences(PNameSpace, ".//EnableKbm", 1);
  var IPVSDefaultLocalKBMSState = getLocalPreferences(PNameSpace, ".//LocalKBMSState", 0);
  var IPVSPlayerDefaultBorder = getLocalPreferences(PNameSpace, ".//Border", 8);
  var IPVSPlayerDefaultPriority = getLocalPreferences(PNameSpace, ".//Priority", "false");


  topic_Set(tApp, "IPVSPlayerDefaultAspectRatio", selViewerAspectRatio);
  topic_Set(tApp, "IPVSPlayerDefaultFps", selViewerFps);
  topic_Set(tApp, "IPVSPlayerDefaultSkipFWD", selViewerSkipFWD);
  topic_Set(tApp, "IPVSPlayerDefaultSkipBack", selViewerSkipBack);
  topic_Set(tApp, "IPVSPlayerDefaultBandwidthKbps", selViewerBandwidth);
  topic_Set(tApp, "IPVSPlayerDefaultUseHTTP", selViewerChkUseHTTP);
  topic_Set(tApp, "IPVSPlayerDefaultUseMMS", selViewerChkUseMMS);

  topic_Set(tApp, "IPVSPlayerDefaultServerPort", IPVSPlayerDefaultServerPort);
  topic_Set(tApp, "IPVSPlayerDefaultAVOption", IPVSPlayerDefaultAVOption);
  topic_Set(tApp, "IPVSPlayerDefaultMulticast", IPVSPlayerDefaultMulticast);
  topic_Set(tApp, "IPVSPlayerDefaultSliceReduction", IPVSPlayerDefaultSliceReduction);
  topic_Set(tApp, "IPVSPlayerDefaultEnableKbm", IPVSPlayerDefaultEnableKbm);
  topic_Set(tApp, "IPVSDefaultLocalKBMSState", IPVSDefaultLocalKBMSState);
  //set player Border to tApp
  topic_Set(tApp, "IPVSPlayerDefaultBorder", IPVSPlayerDefaultBorder);
  topic_Set(tApp, "IPVSPlayerDefaultPriority", IPVSPlayerDefaultPriority);
}

function playerLoad(BlkJSX, instanceID)
{
  // ideally we should set the text/html of the player block here with the new playerID
  //var PlayerTextHTML = "xxxxxxxx" + PlayerID + "yyyyyy";
  //BlkJSX.setText(PlayerTextHTML)
  // tbd - probably need some kind of reload here
  //     - folowed by wait till Control is loaded (which might need user to allow it etc..)
  // hack for now: look for "IPVideoPlayer_Viewer" and "IPVideoPlayer_Archiver" based on input
  log.info("Player Load: instanceID: " + instanceID);
  var IPVideoPlayer = null;
  var PlayerID = instanceID;

  if (browserType == "MSIE") {
    playerTextHTML = "<table border=\"0\" cellspacing=\"0\" cellpadding=\"0\" width=\"100%\" height=\"100%\">" +
                     "<tr><td align=\"center\" valign=\"middle\"><OBJECT id=" + PlayerID +
                     " border=\"8px\" bordercolor=\"#222\" " +
                     "classid=CLSID:A2A6189E-44F4-4D69-9CE4-8329312D2E3A VIEWASTEXT>" +
                     "<PARAM NAME=\"BackColor\" VALUE=\"7874560\"><PARAM NAME=\"BorderColor\" VALUE=\"0\">" +
                     "<PARAM NAME=\"AutoSize\" VALUE=\"0\"><PARAM NAME=\"AutoStart\" VALUE=\"0\">" +
                     "<PARAM NAME=\"SessionId\" VALUE=\"0\"><PARAM NAME=\"Windowless\"  VALUE=\"1\">" + 
                     "</OBJECT></td></tr></table>";
    
  }
  else if (browserType == "FIREFOX") {
    //playerTextHTML = "<table border=\"0\" cellspacing=\"0\" cellpadding=\"0\" width=\"100%\" height=\"100%\">" +
    //                 "<tr><td align=\"center\" valign=\"middle\"><div id=" + PlayerID + ">" +
    //                 "<img src=\"images/panels/maskBG.png\" alt=\"Firefox Don\'t Support This Player\" width=\"110\" height=\"90\"/>" +
    //                 "</div></td></tr></table>";

    playerTextHTML = "<div id=" + PlayerID + "><I><H2>" + " Firefox Don't Support This Player<H2></I> </div>";
    BlkJSX.setColor("#C023A0");
  }
  BlkJSX.setText(playerTextHTML);
  BlkJSX.repaint();
  var BlkJSXDocument = PNameSpace.getDocumentOf(BlkJSX);
  if(BlkJSXDocument != null) {
    IPVideoPlayer = BlkJSXDocument.getElementById(PlayerID);
  }
  IPVideoPlayer.MSIE = (browserType == "MSIE");   
  return IPVideoPlayer;
}

//break cycle between DOM oject and Javascript
function playerDestroy(IPVideoPlayer) {
  var attributes = IPVideoPlayer.attributes, i, l, n;
  if (attributes) {
      l = attributes.length;
      for (i = 0; i < l; i += 1) {
          n = attributes[i].name;
          if (typeof IPVideoPlayer[n] === 'function') {
              IPVideoPlayer[n] = null;
          }
      }
  }
  attributes = IPVideoPlayer.childNodes;
  if (attributes) {
      l = attributes.length;
      for (i = 0; i < l; i += 1) {
          playerDestroy(IPVideoPlayer.childNodes[i]);
      }
    }
  IPVideoPlayer = null;
}

function playerStart(IPVideoPlayer, StreamURL) {
  if (IPVideoPlayer.MSIE){
    log.info("Player Start: uniqueID: " + IPVideoPlayer.uniqueID + ": StreamURL: " + StreamURL);
    // UseHTTP
    var useHTTP = topic_Get(tApp, "IPVSPlayerDefaultUseHTTP");
    var useMMS = 0;
  // Parse Options 
  // ??? - should we be getting all this as part of URL or should some of this come from Profile?
    if( StreamURL.match(/^v2d/) ) {
      // Host
      if(useHTTP == 1){
        useMMS = topic_Get(tApp, "IPVSPlayerDefaultUseMMS");
      }
      var StreamURLHostPart = StreamURL.replace(/\/[^\/]*$/, "");
      if(useMMS == 1){
        var mmsIPAddress = IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_SERVER);
        StreamURLHostPart = "v2d://"+mmsIPAddress;
      }
      // Port
      var ServerPort = topic_Get(tApp, "IPVSPlayerDefaultServerPort");
      if( StreamURL.match(/port=/) ) {
        ServerPort = parseInt(StreamURL.replace(/^.*port=(\d+).*$/, "$1"));
      }
     
      // V2D set values on player ctrl
      IPVideoPlayer.Filename = StreamURLHostPart;
      IPVideoPlayer.ServerPort = ServerPort;
      
    }
    else if( StreamURL.match(/^http/) ) {
      IPVideoPlayer.Filename = StreamURL;
    }
    else if( StreamURL.match(/^udp/) || StreamURL.match(/^igmp/) ) {
      IPVideoPlayer.Filename = StreamURL.replace("://", "://@");
    }
    //common player parameters used for all types of StreamURL
    // Bandwidth
    var BandwidthKbps = topic_Get(tApp, "IPVSPlayerDefaultBandwidthKbps");
    if (StreamURL.match(/bandwidth=/)) {
      var Bandwidthbps = parseInt(StreamURL.replace(/^.*bandwidth=(\d+).*$/, "$1"));
      //"bandwidth in URL is in bps - need to convert to Kbps for player API"
      BandwidthKbps = Bandwidthbps / 1024;
    }
    // AVOption
    var AVOption = topic_Get(tApp, "IPVSPlayerDefaultAVOption");
    if (StreamURL.match(/avoption=/)) {
      AVOption = StreamURL.replace(/^.*avoption=(\w+).*$/, "$1");
      switch (AVOption) {
        case "Video":
          AVOption = 1;
          break;
        case "Audio":
          AVOption = 2;
          break;
        case "Both":
          AVOption = 3;
          break;
        default:
          AVOption = 3;
          break;
      }
    }
    // Fps
    var Fps = topic_Get(tApp, "IPVSPlayerDefaultFps");
    if (StreamURL.match(/fps=/)) {
      Fps = StreamURL.replace(/^.*fps=(\d+).*$/, "$1");
    }
    // Multicast
    var Multicast = topic_Get(tApp, "IPVSPlayerDefaultMulticast");
    if (StreamURL.match(/multicast=/)) {
      Multicast = StreamURL.replace(/^.*multicast=(\w+).*$/, "$1");
      switch (Multicast) {
        case "false":
          Multicast = 0;
          break;
        case "true":
          Multicast = 1;
          break;
        default:
          Multicast = 0;
          break;
      }
    }
    if (Multicast == 0) {
      IPVideoPlayer.useHTTP = useHTTP;
    } else {
      IPVideoPlayer.useHTTP = 0;
    }
    // SRD
    var SliceReduction = topic_Get(tApp, "IPVSPlayerDefaultSliceReduction");
    // Enable Kbm
    var EnableKbm = topic_Get(tApp, "IPVSPlayerDefaultEnableKbm");
    if (StreamURL.match(/enablekbm=/)) {
      EnableKbm = StreamURL.replace(/^.*enablekbm=(\w+).*$/, "$1");
      switch (EnableKbm) {
        case "false":
          EnableKbm = 0;
          break;
        case "true":
          EnableKbm = 1;
          break;
        default:
          EnableKbm = 0;
          break;
      }
    }
    // Local KBMS State
    var LocalKBMSState = topic_Get(tApp, "IPVSDefaultLocalKBMSState");
    IPVideoPlayer.MaxBandwidth = BandwidthKbps;
    IPVideoPlayer.AVMode = AVOption;
    IPVideoPlayer.Fps = Fps;
    IPVideoPlayer.Multicast = Multicast;
    // Optional  
    IPVideoPlayer.SliceReduction = SliceReduction;
    // not used by activeX control
    IPVideoPlayer.EnableKbm = EnableKbm;
    IPVideoPlayer.LocalKBMSState = LocalKBMSState;

    //set toolbar to preserve in togglescreen::
    IPVideoPlayer.Settings = "<ParameterList><toolbar>Preserve</toolbar><doubleclick>Disable</doubleclick></ParameterList>";

    // palyer priority
    var priority = topic_Get(tApp, "IPVSPlayerDefaultPriority");
    log.info("Player play with priority: " + priority + " :uniqueID: " + IPVideoPlayer.uniqueID);
    priority = (priority=="true")?1:0;
    IPVideoPlayer.Priority = priority;
    // start playback
    IPVideoPlayer.Play();
  }
  else {
    log.info("Firefox playerStart:RETURN");
    return;
  }
}

function playerStop (IPVideoPlayer) {
  if (IPVideoPlayer.MSIE) {
    log.info("Player Stop: uniqueID: " + IPVideoPlayer.uniqueID );
    IPVideoPlayer.Stop();
  }
  else {
    log.info("Firefox playerStop:RETURN");
    return;
  }
}

function playerPause(IPVideoPlayer) {
  if (IPVideoPlayer.MSIE) {
    if (IPVideoPlayer.PlayState != IPVSPlayerState_PAUSED) {
      log.info("Player Pause: uniqueID: " + IPVideoPlayer.uniqueID);
      IPVideoPlayer.Pause();
    }
  }
  else {
    log.info("Firefox playerPause:RETURN");
    return;
  }
}

function playerResume(IPVideoPlayer) {
  if (IPVideoPlayer.MSIE) {
    //from bookmark playFrom
    if (IPVideoPlayer.PlayState == IPVSPlayerState_PLAYING) {
      IPVideoPlayer.Pause();
      IPVideoPlayer.Resume();
    }
    if (IPVideoPlayer.PlayState == IPVSPlayerState_PAUSED) {
      IPVideoPlayer.Resume();
    }
    log.info("Player Resume: uniqueID: " + IPVideoPlayer.uniqueID);
  }
  else {
    log.info("Firefox playerResume:RETURN");
    return;
  }
}
//add player playerSkipBackward and playerSkipForward
function playerSkipBackward(IPVideoPlayer,backwardSeconds) {
  if (IPVideoPlayer.MSIE) {
    log.info("Player SkipBackward: uniqueID: " + IPVideoPlayer.uniqueID);
    IPVideoPlayer.SkipBack(backwardSeconds);
  }
  else {
    log.info("Firefox playerSkipBackward:RETURN");
    return;
  }
}

function playerSkipForward(IPVideoPlayer,forwardSeconds) {
  if (IPVideoPlayer.MSIE) {
    log.info("Player SkipForward: uniqueID: " + IPVideoPlayer.uniqueID);
    IPVideoPlayer.SkipFwd(forwardSeconds);
  }
  else {
    log.info("Firefox playerSkipForward:RETURN");
    return;
  }
}

//add player catchup (RTP requires this for synchronizing clock with streming server,
//V2D has no timestamp, so it's simple)
function playerCatchUp(IPVideoPlayer) {
  if (IPVideoPlayer.MSIE) {
    log.info("Player Catchup: uniqueID: " + IPVideoPlayer.uniqueID);
    IPVideoPlayer.CatchUp();
  }
  else {
    log.info("Firefox playerCatchUp:RETURN");
    return;
  }
}

function playerSetFps(IPVideoPlayer, Fps) {
  if (IPVideoPlayer.MSIE) {
    if( IPVideoPlayer.PlayState == IPVSPlayerState_PLAYING ){
      log.info("Player SetFps: uniqueID: " + IPVideoPlayer.uniqueID + ": Fps: " + Fps);
      IPVideoPlayer.Fps = Fps;
    }
  }
  else {
    log.info("Firefox playerSetFps:RETURN");
    return;
  }
}

function playerSetLocalKBMSState(IPVideoPlayer, lKBMSState) {
 if (IPVideoPlayer.MSIE) {
   log.info("Player SetLocalKBMSState: uniqueID: " + IPVideoPlayer.uniqueID);
   IPVideoPlayer.LocalKBMSState = lKBMSState;
 }
 else {
   log.info("Firefox playerSetLocalKBMSState:RETURN");
   return;
 }
}

function playerSetFocus(IPVideoPlayer){
  if (IPVideoPlayer.MSIE) {
    log.info("Player Focus: uniqueID: " + IPVideoPlayer.uniqueID);
    IPVideoPlayer.focus();
  }
  else {
    log.info("Firefox playerSetFocus:RETURN");
    return;
  }
}

//to check KBMSState is success or not
function playerGetKBMSState(IPVideoPlayer) {
 if (IPVideoPlayer.MSIE) {
   return IPVideoPlayer.IsKBMSSuccess;
 }
 else {
   log.info("Firefox playerGetKBMSState:RETURN 0");
   return 0;
 }
}

//Player mute ON or OFF
function playerMute(IPVideoPlayer,muteType) {
  if (IPVideoPlayer.MSIE) {
    IPVideoPlayer.Mute = muteType;
  }
  else {
    log.info("Firefox playerMute:RETURN");
    return;
  }
}

//HACK: playerID is being passed as 0.
// - for now this is only used in tView
function remoteKBMSState(PlayerID, rKBMSState) {
  if (IPVideoPlayer.MSIE) {
    log.info("Player Callback: remoteKBMSState: " + rKBMSState);
    //topic_Publish(PlayerID, "PlayerEvent", "REMOTESTATE");
    topic_Publish(tView, "PlayerEvent", "REMOTESTATE");
  }
  else {
    log.info("Firefox remoteKBMSState:RETURN");
    return;
  }
}
//ActiveX is expecting this function by default.
//TBD handling double click
function ChangeDisplayMode() {
  if (IPVideoPlayer.MSIE) {
    return true;
  }
  else {
    log.info("Firefox ChangeDisplayMode:RETURN false");
    return false;
  }
}

function playerToggleDisplay(IPVideoPlayer) {
  if (IPVideoPlayer.MSIE) {
    try {
      log.info("Player ToggleDisplay: uniqueID: " + IPVideoPlayer.uniqueID);
      IPVideoPlayer.ToggleDisplay();
    }
    catch (er) {
      return false;
    }
    return true;
  }
  else {
    log.info("Firefox playerToggleDisplay:RETURN false");
    return false;
  }
}

function playerGetNativeSize(IPVideoPlayer) {
  if (IPVideoPlayer.MSIE) {
    var playerSize = new Object;
    playerSize.width = IPVideoPlayer.GetNativeWidth();
    playerSize.height = IPVideoPlayer.GetNativeHeight();
    if (isNaN(playerSize.width) || (playerSize.width <= 0) || (playerSize.width == '')) {
      playerSize.width = 0;
    }
    if (isNaN(playerSize.height) || (playerSize.height <= 0) || (playerSize.height == '')) {
      playerSize.height = 0;
    }
    return playerSize;
  }
  else {
    log.info("Firefox playerGetNativeSize:RETURN null");
    return null;
  }
}

function playerSetNativeSize(IPVideoPlayer) {
  if (IPVideoPlayer.MSIE) {
    IPVideoPlayer.ShowNativeVideoResolution();
  }
  else {
    log.info("Firefox playerSetNativeSize:RETURN ");
    return;
  }
}

function playerSetScaleToWindow(IPVideoPlayer) {
  if (IPVideoPlayer.MSIE) {
    IPVideoPlayer.ScaleVideoToWindowSize();
  }
  else {
    log.info("Firefox playerSetScaleToWindow:RETURN ");
    return;
  }
}

function playerResize(IPVideoPlayer, playerSize) {
  if (IPVideoPlayer.MSIE) {
    // figure out width and height based on aspectRatio
    if (isNaN(playerSize.width) || isNaN(playerSize.height) ||
      playerSize.width <= 0 || playerSize.height <= 0) {
      return;
    }

    var width = playerSize.width;
    var height = playerSize.height;
    if (!jsx3.util.strEmpty(playerSize.aspectRatio)) {
      var ar = playerSize.aspectRatio.split(":");
      var h2 = null;
      var w2 = null;
      if ((ar[0] == 0) || (ar[1] == 0)) {
        h2 = playerSize.height;
        w2 = playerSize.width;
      }
      else {
        var w1 = playerSize.width;
        var h1 = w1 * ar[1] / ar[0];
        h2 = playerSize.height;
        w2 = h2 * ar[0] / ar[1];
        if (w1 < w2) {
          width = w1;
          height = h1;
        }
        else {
          width = w2;
          height = h2;
        }
      }
    }

    //  if( playerSize.aspectRatio == "16:9" ) {
    //    var w1 = playerSize.width;
    //    var h1 = w1*9/16;
    //    var h2 = playerSize.height;
    //    var w2 = h2*16/9;
    //    if ( w1 < w2 ) {
    //     width = w1;
    //     height = h1;
    //    }
    //    else {
    //     width = w2;
    //     height = h2;
    //    }
    //  }
    //  else if( playerSize.aspectRatio == "4:3" ) { // calculation for 4:3 aspectratio
    //    var w1 = playerSize.width;
    //    var h1 = w1*3/4;
    //    var h2 = playerSize.height;
    //    var w2 = h2*4/3;
    //    if ( w1 < w2 ) {
    //     width = w1;
    //     height = h1;
    //    }
    //    else {
    //     width = w2;
    //     height = h2;
    //    }
    //  }

    // figure out border
    if (playerSize.border == 0) {
      var border = "solid 0px";
    }
    else if (playerSize.border == 8) {
      var border = "solid 8px #222";
    }
    else if (playerSize.border != null) {
      var border = playerSize.border;
    }
    // set the props on the player
    IPVideoPlayer.style.width = width;
    IPVideoPlayer.style.height = height;
    IPVideoPlayer.style.border = border;
  }
  else {
    log.info("Firefox playerResize:RETURN ");
    return;
  }
}

function playerSnapShot(IPVideoPlayer) {
  if (IPVideoPlayer.MSIE) {
    try {
      log.info("Player SnapShot: uniqueID: " + IPVideoPlayer.uniqueID);
      IPVideoPlayer.Snapshot("", "");
    }
    catch (er) {
      return false;
    }
    return true;
  }
  else {
    log.info("Firefox playerSnapShot:RETURN ");
    return;
  }
}

//set player priority [true|false]:[normal|low]
function playerSetPriority(IPVideoPlayer, priority) {
    if (IPVideoPlayer.MSIE) {
      log.info("Player Set priority: uniqueID: " + IPVideoPlayer.uniqueID + ": Priority: " + priority);
      IPVideoPlayer.Priority = priority;
    }
    else {
      log.info("Firefox playerSetPriority:RETURN ");
      return;
    }
}