
function cmpApp_Create() {
  //keep all requested roomNID
  this.roomEventArr = new Object;

  //[1]Launch MediaPlayer from LiveSession with a SessionId
  //[2]Launch MediaPlayer from BrowseSession with a SessionConfigNID move to cmpBlkBrowseSessions.StartSessionConfig()
  eventApp.subscribe("LiveSessionSelection", null, this,
                        function(EventVar, EventVal) {
                          var mediaInfo = EventVal;
                          // get session id from mediaroom (in cache)
                          if( ! mediaInfo.sessionId ) {
                            mediaInfo.sessionId = getSessionIdforRoomNID(mediaInfo.RoomNID);
                          }
                          // if found - check and launch player if needed
                          if (mediaInfo.sessionId) {
                            log.info("cmpBlkUI.LiveSessionSelection: agtDisplay.launchPlayerBySessionId: Session Id: " + mediaInfo.sessionId+":tMediaPlayer:null");
                            agtDisplay.launchPlayerBySessionId(mediaInfo, true);
                          }
                          else {
                            log.error("cmpBlkUI: LiveSession: Session Id not found for Room::" + mediaInfo.RoomNID);
                          }
                       });

  //Launch Collaboration window -- while clicking 'Edit' button in BrowseSession  
  eventApp.subscribe("CollabSessionEdit", null, this,
                        function(EventVar, EventVal) {
                          var mediaInfo = EventVal;
                          if (mediaInfo != null)
                            cmpWinCollaboration_Create(mediaInfo.SessionConfigNID);
                          else
                            log.error("Edit collaboration session error: \n" + EventVal);
                        });

  
                    
  // when ServiceState is 'VALID' send the requests of MediaStreamSrcPortResource and MediaStreamDstPortResource
  topic_Set(tApp, "LoginUserNID","");
  IpvsLogin.subscribe(com.ipvs.api.Login.ON_LOGINSTATE_CHANGE, this,
                      function (objEvent) {
                        if (objEvent.stateVar == com.ipvs.api.Login.LOGINSTATE && objEvent.stateVal == com.ipvs.api.Login.LOGIN_SUCCESS) {
                          // get the details for the logged in user
                          var loginUser = IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_USER);

                          //get client data from tApp
                          var serviceClientData = topic_GetNode(tApp, "ServiceClientData");
                          var nodeDefaultPort = serviceClientData.selectSingleNode(".//GetServiceClient/PortList/Port");
                          if (!jsx3.util.strEmpty(nodeDefaultPort)) {
                            var defaultPortTitle = nodeDefaultPort.getAttribute("title");
                            topic_Set(tApp, "DefaultPortTitle", defaultPortTitle);
                          }
                          var nodeUser = serviceClientData.selectSingleNode(".//User");
                          if (!jsx3.util.strEmpty(nodeUser)) {
                            var userNID = nodeUser.getAttribute("NID");
                            topic_Set(tApp, "LoginUserNID", userNID);
                            var nodeEndPointControllerData = nodeUser.selectSingleNode(".//EndPointControllerData");
                            if (nodeEndPointControllerData) {
                              var endPointConfigurable = nodeUser.selectSingleNode(".//EndPointTableConfig").getAttribute("Configurable");
                              topic_Set(tApp, "EndPointConfigurable", endPointConfigurable);
                            }
                            else {
                              topic_Set(tApp, "EndPointConfigurable", "true");
                            }
                          }
                          else {
                            log.error("InComplete Service Client Data: " + serviceClientData);
                          }
                          //launching MediaList directly after WEB client login
                          if (clientType == "WEB") {
                            if (jsx3.util.strEmpty(userNID)) {
                              alert("Please contact your Administrator regarding user managenet policy");
                              return false;
                            }
                            else {
                              try {
                                PNameSpace.getJSXByName("blkLogin").setDisplay(jsx3.gui.Block.DISPLAYNONE, true);
                                var blkMediaController = cmpWebController_Create();
                                var cmpBlkMediaControllerObj = { cmp: blkMediaController,
                                  sel: "IpvsLogin.getStateParam(com.ipvs.api.Login.LOGINSTATE) == com.ipvs.api.Login.LOGIN_SUCCESS && " +
                                                                      "topic_Get(tApp, 'ClientLaunchMode') == 'ModuleMediaList'"
                                };
                                cmpBlkUI.SubCmpArr.push(cmpBlkMediaControllerObj);

                                var checkVal = getLocalPreferences(PNameSpace, ".//RemoveLoginInfo") == "true" ? jsx3.gui.CheckBox.CHECKED : jsx3.gui.CheckBox.UNCHECKED;
                                cmpRemoveInfo.BlkJSX.setChecked(checkVal);
                                topic_Publish(tApp, "ClientLaunchMode", "ModuleMediaList");
                              }
                              catch (e) {
                                log.error("Fail to create Web Controller.");
                              }
                            }
                          }
                          else {
                            var clientLaunchModeNode = serviceClientData.selectSingleNode(".//Asset/FeatureList/Feature[@name='Mode']");
                            if (!jsx3.util.strEmpty(clientLaunchModeNode) && clientLaunchModeNode.getValue() == "UnManned") {
                              if (!agtDisplay.checkSettings()) {
                                //DPI setting not equal default value '96'
                                IpvsLogin.doLogout();
                                return;
                              }
                              cmpCanvasMenu_Create(cmpBlkCanvasMenu);
                              var serviceClientData = topic_GetNode(tApp, "ServiceClientData");
                              var deviceNode = serviceClientData.selectSingleNode(".//GetServiceClient/Device");
                              var deviceNID = deviceNode.getAttribute('NID');
                              var nodeDisplayConfig = deviceNode.selectSingleNode(".//AVCodec/Port/DisplayConfig");
                              agtDisplay.setDeviceNID(deviceNID);
                              //no DB update while launching new window
                              var nodeDisplayConfigObj = new Object;
                              nodeDisplayConfigObj.nodeDisplayConfig = nodeDisplayConfig;
                              nodeDisplayConfigObj.clientInfoObj = null;

                              agtDisplay.setPortDisplayConfig(nodeDisplayConfigObj, true);
                              topic_Publish(tApp, "ClientLaunchMode", "UnManned");
                              agtDisplay.open();
                            }
                            else {
                              cmpMainMenu_Create(cmpBlkMainMenu);
                              topic_Publish(tApp, "ClientLaunchMode", "MainMenu");
                            }

                          }
                        }
                        else if (objEvent.stateVar == com.ipvs.api.Login.LOGINSTATE && objEvent.stateVal == com.ipvs.api.Login.LOGIN_LOGOUT_SUCCESS) {
                          if (clientType == "WEB") {
                            PNameSpace.getJSXByName("blkLogin").setDisplay(jsx3.gui.Block.DISPLAYBLOCK, true);
                          }
                        }
                      }
                    );
                    

  eventApp.subscribe("MediaStreamIOPortResourceDataState", null, this,
                      function() {
                        //send request to update Src/Dst lists
                        arrServiceObjectInfo["MediaStreamSrcPortResource"].req.sendRequest(this);
                        arrServiceObjectInfo["MediaStreamDstPortResource"].req.sendRequest(this);
                      }
                    );
                    
  eventApp.subscribe("UpgradeClientSoftware", null, this,
                      function(EventVar, EventVal) {
                          var nodeUpgradeDevice = EventVal;
                          var deviceAssetNID = nodeUpgradeDevice.getAttribute("whereAssetNID");
                          var serviceClientData = topic_GetNode(tApp, "ServiceClientData");
                          var clientAssetNID = serviceClientData.selectSingleNode(".//GetServiceClient/Asset").getAttribute("NID");
                          if(deviceAssetNID == clientAssetNID){
                            alert("Client needs to be Upgraded. Click Ok to proceed");
                            IpvsLogin.doLogout();
                            unloadApplet();
                            jsx3.sleep( function() {
                                            window.close();
                                          //For UI if UpgradeDevice happens for Client then point the user to new available software download 
                                            var loginServer = getLocalPreferences(PNameSpace, ".//Login/LastLoginServer", DefaultLoginServer);
                                            window.open("http://"+ loginServer +"/","_self","fullscreen=no,titlebar=yes,toolbar=no,statusbar=no,menubar=no",true);
                                        }, null, this);
                          }
                      }
                    );                  

  var cmpBlkMsConnectionMask = new CmpBlock("blkMsConnectionMask");
  var cmpBlkMsOk = new CmpBlock("blkMsOk");
  jsx3.gui.Event.unsubscribe(jsx3.gui.Event.CLICK, cmpBlkMsConnectionMask, eval(onMaskClick));

  function onMaskClick(objEvent) {
    if (objEvent.event.getType() == "click") {
      if (jsx3.util.strEmpty(cmpBlkMsConnectionMask.BlkJSX)) return;
      topic_Publish(tApp, "SCStarted", false);
    }
  }
  
  eventApp.subscribe("SCDown", null, this,
                      function(EventVar, EventVal) {
                        var scStatus = EventVal;
                        if (scStatus) {
                          if (jsx3.gui.Window) {
                            try {
                              var windowArr = PNameSpace.WINDOWS.getChildren();
                              jsx3.$A(windowArr).each(function(x) {
                                for (var i = 0; i < windowArr.length; i++) {
                                  var windowBlock = windowArr[i];
                                  if(windowBlock.jsxname == "Panel1"){
                                    windowBlock.getRootBlock().hideMask();
                                    windowBlock.getRootBlock().showMask("Unable to access the Server. Retrying connection...");
                                  }
                                  var connectionOKMask = windowBlock.getDescendantOfName("blkMsOk");
                                  connectionOKMask.setDisplay(jsx3.gui.Block.DISPLAYNONE, true);
                                  var connectionMask = windowBlock.getDescendantOfName("blkMsConnectionMask");
                                  connectionMask.setDisplay(jsx3.gui.Block.DISPLAYBLOCK, true);
                                }
                              });
                            } catch (e) {
                              log.warn("cmpBlkUI: Window:" + e.description);
                            }
                          }
                          jsx3.gui.Event.subscribe(jsx3.gui.Event.CLICK, cmpBlkMsConnectionMask, eval(onMaskClick));
                          cmpBlkMsOk.BlkJSX.setDisplay(jsx3.gui.Block.DISPLAYNONE, true);
                          cmpBlkMsConnectionMask.BlkJSX.setDisplay(jsx3.gui.Block.DISPLAYBLOCK, true);
                        }
                        else{
                          cmpBlkMsConnectionMask.BlkJSX.setDisplay(jsx3.gui.Block.DISPLAYNONE, true);
                        }
                      }
                    );

  eventApp.subscribe("SCStarted", null, this,
                      function(EventVar, EventVal) {
                        var scStatus = EventVal;
                        if (scStatus) {
                          if (jsx3.gui.Window) {
                            try {
                              var windowArr = PNameSpace.WINDOWS.getChildren();
                              jsx3.$A(windowArr).each(function(x) {
                                for (var i = 0; i < windowArr.length; i++) {
                                  var windowBlock = windowArr[i];
                                  if(windowBlock.jsxname == "Panel1"){
                                    windowBlock.getRootBlock().showMask("Successfully connected to the Management Server.");
                                  }
                                  var connectionMask = windowBlock.getDescendantOfName("blkMsConnectionMask");
                                  connectionMask.setDisplay(jsx3.gui.Block.DISPLAYBLOCK, true);
                                  var blkJSXMsOK = windowBlock.getDescendantOfName("blkMsOk");
                                  blkJSXMsOK.setDisplay(jsx3.gui.Block.DISPLAYBLOCK, true);
                                }
                              });
                            } catch (e) {
                              log.warn("cmpBlkUI: Window:" + e.description);
                            }
                          }
                          cmpBlkMsOk.BlkJSX.setDisplay(jsx3.gui.Block.DISPLAYBLOCK, true);
                          cmpBlkMsConnectionMask.BlkJSX.setDisplay(jsx3.gui.Block.DISPLAYBLOCK, true);
                        }
                        else{
                          if(cmpBlkMsOk.BlkJSX.getDisplay() == jsx3.gui.Block.DISPLAYBLOCK){
                            if (jsx3.gui.Window) {
                              try {
                                var windowArr = PNameSpace.WINDOWS.getChildren();
                                jsx3.$A(windowArr).each(function(x) {
                                  for (var i = 0; i < windowArr.length; i++) {
                                    var windowBlock = windowArr[i];
                                    if(windowBlock.jsxname == "Panel1"){
                                      windowBlock.getRootBlock().showMask("");
                                      windowBlock.getRootBlock().hideMask();
                                    }
                                    var connectionMask = windowBlock.getDescendantOfName("blkMsConnectionMask");
                                    connectionMask.setDisplay(jsx3.gui.Block.DISPLAYNONE, true);
                                  }
                                });
                              } catch (e) {
                                log.warn("cmpBlkUI: Window:" + e.description);
                              }
                            }
                            jsx3.gui.Event.unsubscribe(jsx3.gui.Event.CLICK, cmpBlkMsConnectionMask, eval(onMaskClick));
                            cmpBlkMsOk.BlkJSX.setDisplay(jsx3.gui.Block.DISPLAYNONE, true);
                            cmpBlkMsConnectionMask.BlkJSX.setDisplay(jsx3.gui.Block.DISPLAYNONE, true);
                          }
                        }
                      }
                    );
  var cmpBlkLoginUI = null;
  var cmpBlkMainMenu = null;
  var cmpBlkCanvasMenu = null;
  if (clientType != "WEB") {
    cmpBlkLoginUI = new CmpContainer("blkLogin", [], null, null);
    cmpBlkMainMenu = new CmpContainer("blkMainMenu", [], null, null);
    cmpBlkCanvasMenu = new CmpContainer("blkUnmannedMenu", [], null, null);
  }

  //--- Login
  var cmpBlkLogin = cmpBlkLogin_Create( "blkDlgLogin",
                                        "txtLogin", "txtPassword", "selDomain",
                                        "blkAutoLogin", "chkAutoLogin",
                                        "blkLoginResource", "txtLoginResource",
                                        "blkLoginResourcePassword","radioAutoPassword","radioManualPassword","txtLoginResourcePassword",
                                        "blkInterface", "selLoginInterface", "txtLoginInterfaceIP", "btnAdvancedSettings", "blkAdvancedSettings",
                                        "blkLoginServer", "cmbLoginServer",
                                        "lblMessage", 
                                        "btnLogin", "btnCancelRetryLogin", null, "blkCancel");

  //Only for WEB client: remove cookies
  var cmpRemoveInfo = new CmpCheckBox("chkRemoveInfo",
                                  null, { action: "FNCCALL",
                                    context: this,
                                    fnc: function() {
                                      var state = cmpRemoveInfo.BlkJSX.getChecked();
                                      if (state == 0) {
                                        setLocalPreferences(PNameSpace, ".//RemoveLoginInfo", false);
                                      }
                                      else {
                                        setLocalPreferences(PNameSpace, ".//RemoveLoginInfo", true);
                                      }
                                    }
                                  });

  //Only for WEB client: Logout and back to Login page
  var cmpWebLogout = new CmpButton("btnWebLogout",
                                  null, { action: "FNCCALL",
                                    context: this,
                                    fnc: function() {
                                      if(getLocalPreferences(PNameSpace, ".//RemoveLoginInfo") == "true"){
                                        eraseCookie(prefCookieName);
                                      }
                                      IpvsLogin.doLogout();
                                    }
                                  });

  var buildVersion = "Version " + topic_Get(tApp, "BuildVersion");

  
  var cmpLblBuildVersion = new CmpLabel("lblBuildVersion",
         null,
         null,
         null,
         buildVersion
         );

  var cmpLblWelcomeText = new CmpLabel("lblWelcome",
                                       null,
                                       [{ action: "APPEVENT", eventvar: "LoginUser", eventval: null}]);
  var cmpLblWelcomeTextWebClient = new CmpLabel("lblWelcomeText",
                                       null,
                                       [{ action: "APPEVENT", eventvar: "LoginUser", eventval: null}]);

  var cmpAppTitle = new Component("appTitle", null, null, null);
  cmpAppTitle.customizeSkin = function() {
                                var appTitle = clsCustomizeSkin.getAttrValue("IPVSClient", "appTitle");
                                  if (!jsx3.util.strEmpty(appTitle)) {
                                    cmpAppTitle.BlkJSX.setText(appTitle, true);
                                  }
                                }

  var cmpClientHeaderLogo = new Component("clientHeaderLogo", null, null, null);
  cmpClientHeaderLogo.customizeSkin = function() {
                                      var clientHeaderLogo = clsCustomizeSkin.getAttrValue("IPVSClient", "clientHeaderLogo");
                                      if (!jsx3.util.strEmpty(clientHeaderLogo)) {
                                        cmpClientHeaderLogo.BlkJSX.setSrc(clientHeaderLogo);
                                          cmpClientHeaderLogo.BlkJSX.repaint();
                                        }
                                      }

  windowTitle = clsCustomizeSkin.getAttrValue("IPVSClient", "windowTitle");
  if (jsx3.util.strEmpty(windowTitle)) {
    windowTitle = "Barco ClientUI";
  }
  document.title = windowTitle;
                                       
  //--- Top Level block
  var cmpBlkUI = new CmpContainer("blkUI",
                                   [{ cmp: cmpBlkLogin,
                                      sel: "IpvsLogin.getStateParam(com.ipvs.api.Login.LOGINSTATE) != com.ipvs.api.Login.LOGIN_SUCCESS"},
                                    { cmp: cmpBlkLoginUI,
                                      sel: "IpvsLogin.getStateParam(com.ipvs.api.Login.LOGINSTATE) != com.ipvs.api.Login.LOGIN_SUCCESS"},
                                    { cmp: cmpBlkMainMenu,
                                      sel: "IpvsLogin.getStateParam(com.ipvs.api.Login.LOGINSTATE) == com.ipvs.api.Login.LOGIN_SUCCESS && " +
                                           "topic_Get(tApp, 'ClientLaunchMode') == 'MainMenu'"
                                    },
                                    { cmp: cmpBlkCanvasMenu,
                                      sel: "IpvsLogin.getStateParam(com.ipvs.api.Login.LOGINSTATE) == com.ipvs.api.Login.LOGIN_SUCCESS && " +
                                           "topic_Get(tApp, 'ClientLaunchMode') == 'UnManned'"
                                    },
                                    { cmp: cmpWebLogout, sel: "IpvsLogin.getStateParam(com.ipvs.api.Login.LOGINSTATE) == com.ipvs.api.Login.LOGIN_SUCCESS && " +
                                                              "topic_Get(tApp, 'ClientLaunchMode') == 'ModuleMediaList'"
                                    },
                                    { cmp: cmpRemoveInfo, sel: "IpvsLogin.getStateParam(com.ipvs.api.Login.LOGINSTATE) == com.ipvs.api.Login.LOGIN_SUCCESS && " +
                                                               "topic_Get(tApp, 'ClientLaunchMode') == 'ModuleMediaList'"
                                    },
                                    { cmp: cmpLblBuildVersion },
                                    { cmp: cmpAppTitle },
                                    { cmp: cmpClientHeaderLogo },
                                    { cmp: cmpLblWelcomeText, sel: "topic_Get(tApp,'clientType')!='WEB' && IpvsLogin.getStateParam(com.ipvs.api.Login.LOGINSTATE) == com.ipvs.api.Login.LOGIN_SUCCESS "
                                    },
                                    { cmp: cmpLblWelcomeTextWebClient, sel: "topic_Get(tApp,'clientType')=='WEB' && IpvsLogin.getStateParam(com.ipvs.api.Login.LOGINSTATE) == com.ipvs.api.Login.LOGIN_SUCCESS "
                                    } ],
                                    [{ action: "CLSEVENT", 
                                       eventcls:IpvsLogin, 
                                       eventsub:com.ipvs.api.Login.ON_LOGINSTATE_CHANGE, 
                                       eventvar: com.ipvs.api.Login.LOGINSTATE, 
                                       eventval: null },
                                     { action: "CLSEVENT", 
                                       eventcls:IpvsLogin, 
                                       eventsub:com.ipvs.api.Login.ON_LOGIN_ERROR, 
                                       eventvar: com.ipvs.api.Login.LOGINSTATE, 
                                       eventval: null },
                                     { action: "APPEVENT", eventvar: "ClientLaunchMode", eventval: null}
                                    ]
                                 );

  return cmpBlkUI;
}

//get a sesesionId for a specified roomNID
function getSessionIdforRoomNID(roomNID) {
  var nodeMediaRoom = topic_GetNIDNode(tCache, roomNID);
  var sessionId = nodeMediaRoom ? nodeMediaRoom.getAttribute("id") : null;
  return sessionId;
}

//keep as further destroy action
function cmpApp_Destroy() {
  if(clientType == "WEB") {
    var blkMediaController = getJsxByName("blkMediaController");
    if (blkMediaController) {
      // clean all children of 'blkMediaController' from browser DOM
      blkMediaController.removeChildren();
    }
  }
}


//move SessionStart request from cmpBlkMediaPlayer to here for performance reason
function startSessionRequest(mediaInfo) {
  var sessionConfig = null;
  if (!jsx3.util.strEmpty(mediaInfo.NID)) {
    //CASE 1. launch media player from media list
    sessionConfig = com.ipvs.api.SessionConfig.Document.newDocument();
    // dest is ourself - add that as entity
    var objDstEntity = new Object();
    objDstEntity.type = "DstPort";
    objDstEntity.portNID = topic_Get(tApp, "DestNID");
    objDstEntity.portTitle = topic_Get(tApp, "DefaultPortTitle");//  IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_RESOURCE);
    objDstEntity.maxInstanceCount = 16;
    objDstEntity.streamType = "MULTI";
    var dstEntityId = sessionConfig.addEntity(objDstEntity);
    // get src entity info from mediaInfo
    if (mediaInfo.type == "SrcPort") {
      var objSrcPortEntity = new Object();
      objSrcPortEntity.name = mediaInfo.Name;
      objSrcPortEntity.type = mediaInfo.type;
      objSrcPortEntity.portNID = mediaInfo.NID;
      objSrcPortEntity.portTitle = mediaInfo.Name;
      objSrcPortEntity.maxInstanceCount = mediaInfo.maxInstanceCount;
      objSrcPortEntity.streamType = mediaInfo.streamType;
      objSrcPortEntity.streamTypeList = mediaInfo.streamTypeList;
      // add entity to session
      var srcPortEntityId = sessionConfig.addEntity(objSrcPortEntity);
      // add a new connection group with this src entity/instance
      var connGrpId = sessionConfig.addConnGroup(srcPortEntityId, dstEntityId);
      // enable PVR
      sessionConfig.setPVREnable(true);
    }
    else if (mediaInfo.type == "PlyFile") {
      var objFileEntity = new Object();
      objFileEntity.name = mediaInfo.Name;
      objFileEntity.type = mediaInfo.type;
      objFileEntity.fileNID = mediaInfo.NID;
      objFileEntity.fileTitle = mediaInfo.Name;
      objFileEntity.streamType = mediaInfo.streamType;
      objFileEntity.clipId = mediaInfo.clipID;
      objFileEntity.recSessionConfigNID = mediaInfo.sessionConfigNID;
      // add entity to session
      var srcFileEntityId = sessionConfig.addEntity(objFileEntity);
      // add a new connection group with this src entity/instance
      var connGrpId = sessionConfig.addConnGroup(srcFileEntityId, dstEntityId);
    }
    else if (mediaInfo.type == "GrpFile") {
      var objGrpFile = new Object();
      objGrpFile.grpFileType = "SyncPly";
      objGrpFile.grpFileNID = mediaInfo.NID;
      objGrpFile.grpFileTitle = mediaInfo.Name;
      // add the grpFile to session
      var grpFileId = sessionConfig.addGrpFile(objGrpFile);
      for (var i = 0; i < mediaInfo.arrClips.length; i++) {
        var clipInfo = mediaInfo.arrClips[i];
        var objClipEntity = new Object();
        objClipEntity.name = clipInfo.Name;
        objClipEntity.type = "PlyFile";
        objClipEntity.fileNID = clipInfo.NID;
        objClipEntity.fileTitle = clipInfo.Name;
        objClipEntity.streamType = clipInfo.streamType;
        objClipEntity.grpFileId = grpFileId;
        // add entity to session
        var srcClipEntityId = sessionConfig.addEntity(objClipEntity);
        // add a new connection group with this src entity/instance
        var connGrpId = sessionConfig.addConnGroup(srcClipEntityId, dstEntityId);
      }
    }
  }
  else if (!jsx3.util.strEmpty(mediaInfo.SessionConfigNID)) {
    //CASE 3. launch media player from browse session with a collaboration configNID
    //        a sub-case is launch MediaPlayer from Exodus(SessionStart)
    sessionConfig = topic_GetNIDNode(tCache, mediaInfo.SessionConfigNID);
    // add SessionConfigNID to all the recSessionConfigNID attribute(GrpFile, Entity)
    var recSessionConfigIter = sessionConfig.selectNodeIterator(".//*[@recSessionConfigNID]");
    while (recSessionConfigIter.hasNext()) {
      var recSessionConfigNode = recSessionConfigIter.next();
      var recSessionConfigNID = recSessionConfigNode.getAttribute("recSessionConfigNID");
      if (jsx3.util.strEmpty(recSessionConfigNID)) {
        recSessionConfigNode.setAttribute("recSessionConfigNID", mediaInfo.SessionConfigNID);
      }
    }
  }

  else if (mediaInfo.type == "Connect") {
    sessionConfig = com.ipvs.api.SessionConfig.Document.newDocument();
    // Add destination entity
    var objDstEntity = new Object();
    objDstEntity.name = mediaInfo.DstName;
    objDstEntity.type = mediaInfo.Dsttype;
    objDstEntity.portNID = mediaInfo.DstNID;
    objDstEntity.portTitle = mediaInfo.DstName;
    objDstEntity.maxInstanceCount = mediaInfo.DstmaxInstanceCount;
    objDstEntity.streamType = mediaInfo.DststreamType;
    objDstEntity.streamTypeList = mediaInfo.DststreamTypeList;
    // add entity to session
    var dstEntityId = sessionConfig.addEntity(objDstEntity);


    if (mediaInfo.Srctype == "GrpFile") {
      var objGrpFile = new Object();
      objGrpFile.grpFileType = "SyncPly";
      objGrpFile.grpFileNID = mediaInfo.SrcNID;
      objGrpFile.grpFileTitle = mediaInfo.SrcName;
      // add the grpFile to session
      var grpFileId = sessionConfig.addGrpFile(objGrpFile);
      for (var i = 0; i < mediaInfo.arrClips.length; i++) {
        var clipInfo = mediaInfo.arrClips[i];
        var objClipEntity = new Object();
        objClipEntity.name = clipInfo.Name;
        objClipEntity.type = "PlyFile";
        objClipEntity.fileNID = clipInfo.NID;
        objClipEntity.fileTitle = clipInfo.Name;
        objClipEntity.streamType = clipInfo.streamType;
        objClipEntity.grpFileId = grpFileId;
        // add entity to session
        var srcClipEntityId = sessionConfig.addEntity(objClipEntity);
        // add a new connection group with this src entity/instance
        var connGrpId = sessionConfig.addConnGroup(srcClipEntityId, dstEntityId);
      }
    }
    else if (mediaInfo.Srctype == "PlyFile") {
      // Add Source Entity
      var objSrcPortEntity = new Object();
      objSrcPortEntity.name = mediaInfo.SrcName;
      objSrcPortEntity.type = mediaInfo.Srctype;
      objSrcPortEntity.fileNID = mediaInfo.SrcNID;
      objSrcPortEntity.fileTitle = mediaInfo.SrcName;
      objSrcPortEntity.streamType = mediaInfo.SrcstreamType;
      objSrcPortEntity.streamTypeList = mediaInfo.SrcstreamTypeList;
      objSrcPortEntity.clipId = mediaInfo.clipID;
      objSrcPortEntity.recSessionConfigNID = mediaInfo.sessionConfigNID;
      // add entity to session
      var srcPortEntityId = sessionConfig.addEntity(objSrcPortEntity);

      // add a new connection group with this src entity/instance
      var connGrpId = sessionConfig.addConnGroup(srcPortEntityId, dstEntityId);    
    }
    else {
      // Add Source Entity
      var objSrcPortEntity = new Object();
      objSrcPortEntity.name = mediaInfo.SrcName;
      objSrcPortEntity.type = mediaInfo.Srctype;
      objSrcPortEntity.portNID = mediaInfo.SrcNID;
      objSrcPortEntity.portTitle = mediaInfo.SrcName;
      objSrcPortEntity.maxInstanceCount = mediaInfo.SrcmaxInstanceCount;
      objSrcPortEntity.streamType = mediaInfo.SrcstreamType;
      objSrcPortEntity.streamTypeList = mediaInfo.SrcstreamTypeList;
      // add entity to session
      var srcPortEntityId = sessionConfig.addEntity(objSrcPortEntity);

      // add a new connection group with this src entity/instance
      var connGrpId = sessionConfig.addConnGroup(srcPortEntityId, dstEntityId);
    }
  }
  return setSessionXML(sessionConfig, mediaInfo.sessionId, mediaInfo.Name, mediaInfo.SessionConfigNID);
}

// setup a Session node using the sessioninfo from the sessionconfig
function setSessionXML(nodeSessionConfig, sessionId, sessionName, sessionConfigNID) {
  if (jsx3.util.strEmpty(sessionConfigNID)) {
    sessionConfigNID = "";
  }

  // create the sessionConfigXML
  var sessionConfig = new com.ipvs.api.SessionConfig.Document(nodeSessionConfig);
  var nodeGrpFileListIter = sessionConfig.getGrpFileIter();
  var nodeEntityListIter = sessionConfig.getEntityIter();
  var nodeConnListIter = sessionConfig.getConnIter();
  var nodeDirListIter = sessionConfig.getDirIter();

  // create the new SessionXML node
  var nodeSession = com.ipvs.api.Session.Document.newDocument();
  nodeSession.setID(sessionId);
  nodeSession.setsessionConfigNID(sessionConfigNID);
  nodeSession.setGrpFileList(nodeGrpFileListIter);
  nodeSession.setEntityList(nodeEntityListIter);
  nodeSession.setDirList(nodeDirListIter);
  nodeSession.setConnList(nodeConnListIter);
  if (jsx3.util.strEmpty(sessionName)) sessionName = sessionId;
  nodeSession.setName(sessionName);
  //initial "SessionStartSuccess"
  topic_Set(sessionId, "SessionStartSuccess", null);
  return nodeSession;
}
