function cmpWinMediaPlayer_Create(mediaInfo,server) {
  var WinName = mediaInfo.fileName;
  var WinXML = "components/mediaPlayer.xml";
  var myWin = server.getAppWindow(WinName);
  if (myWin == null) {
    myWin = server.createAppWindow(WinName);
  }
  // load the XML in it
  var rootBlock = myWin.getRootBlock();
  rootBlock.setBackgroundColor("#1D4AD9",true);
  var childBlkJSX = myWin.load(WinXML);
  var fpsSliderMaxValue = 60;
  var skipBackTimeOutId = null;
  var skipForwardTimeOutId = null;

  //var childBlkJSX = rootBlock.load(WinXML);
  rootBlock.setChild(childBlkJSX);

  myWin.setTitle("Media Player: " + WinName);
  myWin.setWidth(800);  
  myWin.setHeight(600);
  myWin.setResizable(true);
  myWin.setScrollable(true);
  myWin.setDependent(true);

  function handleLogMessage() {
    var logger = jsx3.util.Logger.getLogger("com.ipvs");
    return logger;
  }
  
  function onClick_Rewind() {
    if (jsx3.util.strEmpty(IPVideoPlayer)) return;
    IPVideoPlayer.SkipBack(10000);
    lblStatusDisplay.setText("Skipped Back", true);
    if (skipBackTimeOutId != null) {
      clearTimeout(skipBackTimeOutId);
      skipBackTimeOutId = null;  
    }
    skipBackTimeOutId = setTimeout(function () { lblStatusDisplay.setText("Connected", true); skipBackTimeOutId = null; }, 2000);
    btnPlay.setDisplay(jsx3.gui.Block.DISPLAYNONE, true);
    btnPause.setDisplay(jsx3.gui.Block.DISPLAYBLOCK, true);
  }

  function onClick_Play() {
    if (jsx3.util.strEmpty(IPVideoPlayer)) return;
    if (mediaInfo.streamtype == "V2D") {
      IPVideoPlayer.Resume();
    } else {
        IPVideoPlayer.Play();
    }
      lblStatusDisplay.setText("Connected", true);
      btnPlay.setDisplay(jsx3.gui.Block.DISPLAYNONE, true);
      btnPause.setDisplay(jsx3.gui.Block.DISPLAYBLOCK, true);
  }

  function onClick_Stop() {
    var log = handleLogMessage();
    log.info("\n\onClick_Stop IPVideoPlayer::" + IPVideoPlayer + "\n");
    if (jsx3.util.strEmpty(IPVideoPlayer)) return;
//    IPVideoPlayer.Stop();
//    lblStatusDisplay.setText("Stopped", true);
    myWin.winClose();
    log.info("\n\onClick_Stop close\n");
    myWin.close();
  }

  function onClick_Pause() {
    if (jsx3.util.strEmpty(IPVideoPlayer)) return;
    IPVideoPlayer.Pause();
    lblStatusDisplay.setText("Paused", true);
    btnPlay.setDisplay(jsx3.gui.Block.DISPLAYBLOCK, true);
    btnPause.setDisplay(jsx3.gui.Block.DISPLAYNONE, true);
  }

   function onClick_FF() {
    if (jsx3.util.strEmpty(IPVideoPlayer)) return;
    IPVideoPlayer.SkipFwd(30000);
    lblStatusDisplay.setText("Skipped Forward", true);
    if (skipForwardTimeOutId != null) {
      clearTimeout(skipForwardTimeOutId);
      skipForwardTimeOutId = null;
    }
    skipForwardTimeOutId = setTimeout(function () { lblStatusDisplay.setText("Connected", true); skipForwardTimeOutId = null; }, 2000);
    btnPlay.setDisplay(jsx3.gui.Block.DISPLAYNONE, true);
    btnPause.setDisplay(jsx3.gui.Block.DISPLAYBLOCK, true);
  }

  function onSelect_Slider(fpVALUE, fpPREVIOUS) {
     if (jsx3.util.strEmpty(IPVideoPlayer)) return;
     var fpsVal = fpVALUE;
     var TgtOffset = Math.round(fpsVal * (fpsSliderMaxValue / 100));
     if (TgtOffset == 0) {
       TgtOffset = '1';
       fpsVal = 1;
     }
     FPSSlider.setValue(fpsVal);
     IPVideoPlayer.Fps = fpsVal;
     lblTxtFPS.setText(TgtOffset);
     lblTxtFPS.repaint();
  }

  function onDrag_Slider(fpVALUE) {
    if (jsx3.util.strEmpty(IPVideoPlayer)) return;
    var fpsVal = fpVALUE;
    var TgtOffset = Math.round(fpsVal * (fpsSliderMaxValue / 100));
    if (TgtOffset == 0) {
      TgtOffset = '1';
    }
    FPSSlider.setTip(TgtOffset.toString());
  }

  function onClick_WinFullSscreen() {
    if (jsx3.util.strEmpty(IPVideoPlayer)) return;
    IPVideoPlayer.ToggleDisplay();
  }

  function onClick_Win1to1() {
    if (jsx3.util.strEmpty(IPVideoPlayer)) return;
    var btnState = window_1_to_1.getState();
    if (btnState == "1") {
      var nativeWidth = IPVideoPlayer.GetNativeWidth();
      var nativeHeight = IPVideoPlayer.GetNativeHeight();
      IPVideoPlayer.style.width = nativeWidth;
      IPVideoPlayer.style.height = nativeHeight
      IPVideoPlayer.ShowNativeVideoResolution();
    } else {
      IPVideoPlayer.style.width = "100%";
      IPVideoPlayer.style.height = "100%";
      IPVideoPlayer.ScaleVideoToWindowSize();
    }
  }

  function onClick_WinExpand() {
    if (jsx3.util.strEmpty(IPVideoPlayer)) return;
    IPVideoPlayer.style.width = "100%";
    IPVideoPlayer.style.height = "100%";
    IPVideoPlayer.ScaleVideoToWindowSize();
  }

  function onClick_WinMute() {
    if (jsx3.util.strEmpty(IPVideoPlayer)) return;
    IPVideoPlayer.ToggleMute();
  }

  function onClick_WinDelete() {
    var log = handleLogMessage();
    log.info("\n\onClick_WinDelete IPVideoPlayer::" + IPVideoPlayer + "\n");
    if (jsx3.util.strEmpty(IPVideoPlayer)) return;
//    IPVideoPlayer.Stop();
    myWin.winClose();
    log.info("\n\onClick_WinDelete close\n");
    myWin.close();
  }

  myWin.toggleController = function (EventVal) {
    var lytPlayer = myWin.getDescendantOfName("lytPlayer");
    var lytPlayerArr = lytPlayer.getRows().split(',');
    var eventVal = EventVal.context.objEVENT;
    var instanceId = eventVal.substring(0, eventVal.indexOf(':'));
    var callBackMsg = eventVal.substring(eventVal.indexOf(':') + 1);
    if (callBackMsg == "true") {
      controller.setDisplay(jsx3.gui.Block.DISPLAYNONE, true);
      lytPlayerArr[1] = "0";
    } else {
      controller.setDisplay(jsx3.gui.Block.DISPLAYBLOCK, true);
      lytPlayerArr[1] = "50";
    }
    lytPlayer.setRows(lytPlayerArr.join(), true);
//    var renderHTML = myWin.getRootBlock().getRendered().innerHTML;
//    var Message = jsx3.util.strEscapeHTML(renderHTML);
//    var tt = "";
//    var log = handleLogMessage();
//    log.info("\n\nMessage" + Message + "\n");
//    log.info("\n\nrenderHTML" + renderHTML + "\n");
//    var rootblk = myWin.getRootBlock();
//    var width = rootblk.getWidth();
//    var height = rootblk.getHeight();
    var btnState = window_fullscreen.getState();
    if (btnState == "1") {
      window_fullscreen.setState(jsx3.gui.ImageButton.STATE_OFF, true);
    }

    jsx3.sleep(function () { myWin.onResize(); }, null, this);
  }

  myWin.winClose = function () {
    if (jsx3.util.strEmpty(IPVideoPlayer)) return;
    var log = handleLogMessage();
    log.info("\n\winClose\n");
    myWin.unsubscribe(jsx3.gui.Window.WILL_CLOSE, myWin.winClose);

    myWin.getRootBlock().clearHotKeys();
    playerInstance.unloadPlayer();
    var rootBlock = myWin.getRootBlock();
    rootBlock.removeChildren();
    //IPVideoPlayer.Stop();
    var ab = myWin.getParent();
  }

  myWin.subscribe(jsx3.gui.Window.WILL_CLOSE, myWin.winClose);

  myWin.open();

  var controller = myWin.getDescendantOfName("blkControllerRoot");
  var btnRewind = controller.getDescendantOfName("btnRewind");
  var btnPlay = controller.getDescendantOfName("btnPlay");
  var btnPause = controller.getDescendantOfName("btnPause");
  var btnStop = controller.getDescendantOfName("btnStop");
  var btnFastforward = controller.getDescendantOfName("btnFastforward");
  var FPSSlider = controller.getDescendantOfName("FPSSlider");
  var lblTxtFPS = controller.getDescendantOfName("lblTxtFPS");
  var lblStatusDisplay = controller.getDescendantOfName("lblStatusDisplay");
  var window_fullscreen = controller.getDescendantOfName("window_fullscreen");
  var window_1_to_1 = controller.getDescendantOfName("window_1_to_1");
  //var window_expand = controller.getDescendantOfName("window_expand");
  var window_mute = controller.getDescendantOfName("window_mute");
  var window_delete = controller.getDescendantOfName("window_delete");

  btnRewind.subscribe(jsx3.gui.Interactive.EXECUTE, this, onClick_Rewind);
  btnPlay.subscribe(jsx3.gui.Interactive.EXECUTE, this, onClick_Play);
  btnPause.subscribe(jsx3.gui.Interactive.EXECUTE, this, onClick_Pause);
  btnStop.subscribe(jsx3.gui.Interactive.EXECUTE, this, onClick_Stop);
  btnFastforward.subscribe(jsx3.gui.Interactive.EXECUTE, this, onClick_FF);

  //set the slider functions
  FPSSlider.removeEvent(jsx3.gui.Interactive.CHANGE);
  FPSSlider.setEvent("FPSSlider.onSelect_Slider(fpVALUE,fpPREVIOUS);", jsx3.gui.Interactive.CHANGE);
  FPSSlider.onSelect_Slider = onSelect_Slider;
  FPSSlider.removeEvent(jsx3.gui.Interactive.INCR_CHANGE);
  FPSSlider.setEvent("FPSSlider.onDrag(fpVALUE);", jsx3.gui.Interactive.INCR_CHANGE);
  FPSSlider.onDrag = onDrag_Slider;

  window_fullscreen.subscribe(jsx3.gui.Interactive.EXECUTE, this, onClick_WinFullSscreen);
  window_1_to_1.subscribe(jsx3.gui.Interactive.EXECUTE, this, onClick_Win1to1);
  //window_expand.subscribe(jsx3.gui.Interactive.EXECUTE, this, onClick_WinExpand);
  window_mute.subscribe(jsx3.gui.Interactive.EXECUTE, this, onClick_WinMute);
  window_delete.subscribe(jsx3.gui.Interactive.EXECUTE, this, onClick_WinDelete);

//  var ctlBlk = controller.loadAndCache("components/controller.xml");
//  controller.setChild(ctlBlk);
  var IPVideoPlayer = null;
  var playerInstance = myWin.getRootBlock().getDescendantsOfType("com.ipvs.gui.MediaPlayerWindow")[0];

  playerInstance.subscribe(com.ipvs.gui.MediaPlayerWindow.MSG_ONFULLSCREEN, myWin.toggleController);

  playerInstance.asyncLoadPlayer().when(
                                      function () {
                                        // prjAbsolutePath defined in the index.hta or htm
                                        var strServer = prjAbsolutePath.substring(parseInt(prjAbsolutePath.indexOf("/") + 3), parseInt(prjAbsolutePath.indexOf("IPVSAuthMediaClient"))).replace(/\%20/g, " ") + "Media/blob/" + mediaInfo.fileNID;
                                        //FOR DEBUGGING
                                        //var strServer = "D://authtool//Player Test//Media//blob//" + mediaInfo.fileNID;
                                        var bitrate = 10240;
                                        var serverPort = 6060;
                                        var avMode = 3;
                                        var fps = 10;
                                        var multicast = false;
                                        var http = false;
                                        IPVideoPlayer = playerInstance.getPlayer();
                                        var fileType = "v2dfile://"
                                        if (mediaInfo.streamtype == "RTP") {
                                          fileType = "rtpfile://";
                                        } else if (mediaInfo.streamtype == "MPEGTS") {
                                          fileType = "mpegtsfile://";
                                        }

                                        if (mediaInfo.streamtype == "RTP") {
                                          var sdpInfoStr = playerInstance.getDecodeUnescapeStr(mediaInfo.sdpinfo);
                                          IPVideoPlayer.SDPData = sdpInfoStr;
                                        }
                                        IPVideoPlayer.Filename = fileType + strServer;
                                        IPVideoPlayer.ServerPort = serverPort;
                                        IPVideoPlayer.MaxBandwidth = parseInt(bitrate);
                                        IPVideoPlayer.AVMode = avMode;
                                        IPVideoPlayer.Fps = fps;
                                        IPVideoPlayer.Multicast = multicast;
                                        IPVideoPlayer.useHTTP = http;
                                        IPVideoPlayer.SliceReduction = 1;
                                        IPVideoPlayer.Play();

                                        lblStatusDisplay.setText("Connected", true);
                                        lblStatusDisplay.setColor("#16FB01", true);
                                        var renderHTML = myWin.getRootBlock().getRendered().innerHTML;
                                        var Message = jsx3.util.strEscapeHTML(renderHTML);
                                        var tt = "";
                                      });






} // end cmpWinMediaPlayer_Create