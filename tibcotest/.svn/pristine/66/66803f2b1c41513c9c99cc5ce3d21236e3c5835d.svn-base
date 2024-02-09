function cmpApp_Create() {

  topic_Set(tApp, "CreateAppmenu", "NONE");
  topic_Set(tApp, "SHOWTABDATA", "NONE");
  topic_Set(tApp, "TagNameResponseSelectFormat", "NameValueTree");

  var cmpBlkLogin = cmpBlkLogin_Create("blkLogin", 
                                       "txtLogin", "txtPassword", "selDomain", 
                                       "blkAutoLogin", "chkAutoLogin",
                                       "blkLoginResource", "txtLoginResource",
                                       "blkLoginResourcePassword","radioAutoPassword","radioManualPassword","txtLoginResourcePassword",
                                       "blkInterface", "selLoginInterface", "txtLoginInterfaceIP", "btnAdvancedSettings", "blkAdvancedSettings",
                                       "blkLoginServer", "cmbLoginServer",
                                       "lblMessage",
                                       "btnLogin", "btnCancelRetryLogin", null, "blkCancel");

  var cmpLoginLogo = new Component(["blkLogin", "adminLoginLogo"], null, null, null);
  cmpLoginLogo.customizeSkin = function() {
                                var adminLoginLogo = clsCustomizeSkin.getAttrValue("IPVSAdmin", "adminLoginLogo");
                                if (!jsx3.util.strEmpty(adminLoginLogo)) {
                                  cmpLoginLogo.BlkJSX.setSrc(adminLoginLogo);
                                  cmpLoginLogo.BlkJSX.repaint();
                                }
                              }

  var windowTitle = clsCustomizeSkin.getAttrValue("IPVSAdmin", "windowTitle");
  if (jsx3.util.strEmpty(windowTitle)) {
    windowTitle = "Management Server";
  }
  document.title = windowTitle;

  var cmpBlkLoginUI = new CmpContainer( "blkUI",
                                       [{cmp:cmpLoginLogo},
                                        {cmp:cmpBlkLogin,
                                         sel:"IpvsLogin.getStateParam(com.ipvs.api.Login.LOGINSTATE) != com.ipvs.api.Login.LOGIN_SUCCESS"} 
                                       ],
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
                                        {action:"APPEVENT", eventvar:"UserPermissions", eventval:null}] );
  
  eventApp.subscribe("AppletLoaded", null, this,
                      function(EventVar, EventVal) {
                        var appletStatus = EventVal;
                        if (appletStatus == "DONE") {
                          if(topic_Get(tApp, "CreateAppmenu") == "NONE"){
                            topic_Set(tApp, "CreateAppmenu", "DONE");                
                            cmpAppMenu_Create();
                          }
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
                          if( jsx3.gui.Dialog ) {
                            try {
                              var DialogBoxArr = PNameSpace.getBodyBlock().getDescendantsOfType(jsx3.gui.Dialog, false);
                              for (var i = 0; i < DialogBoxArr.length; i++) {
                                var BlkJSX = DialogBoxArr[i];
                                BlkJSX.doToggleState(jsx3.gui.Dialog.MINIMIZED);
                              }
                            }catch(e) {
                              log.warn("cmpBlkUI: minimize Dialog:" + e.description);
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
                          cmpBlkMsOk.BlkJSX.setDisplay(jsx3.gui.Block.DISPLAYBLOCK, true);
                          cmpBlkMsConnectionMask.BlkJSX.setDisplay(jsx3.gui.Block.DISPLAYBLOCK, true);
                        }
                        else{
                          if(cmpBlkMsOk.BlkJSX.getDisplay() == jsx3.gui.Block.DISPLAYBLOCK){
                            if( jsx3.gui.Dialog ) {
                              try {
                                var DialogBoxArr = PNameSpace.getBodyBlock().getDescendantsOfType(jsx3.gui.Dialog, false);
                                for (var i = 0; i < DialogBoxArr.length; i++) {
                                  var BlkJSX = DialogBoxArr[i];
                                  BlkJSX.doToggleState(jsx3.gui.Dialog.MAXIMIZED);
                                  BlkJSX.focus();
                                }
                              }catch(e) {
                                log.warn("cmpBlkUI: maximize Dialog:" + e.description);
                              }
                            }
                            jsx3.gui.Event.unsubscribe(jsx3.gui.Event.CLICK, cmpBlkMsConnectionMask, eval(onMaskClick));
                            cmpBlkMsOk.BlkJSX.setDisplay(jsx3.gui.Block.DISPLAYNONE, true);
                            cmpBlkMsConnectionMask.BlkJSX.setDisplay(jsx3.gui.Block.DISPLAYNONE, true);
                          }
                        }
                      }
                    );

  return cmpBlkLoginUI;
}

function cmpAppMenu_Create() {

  var blkMainBlkJSX = PNameSpace.getJSXByName("blkMain");
  blkMainBlkJSX.load("components/main.xml");
  var blkUIBlkJSX = PNameSpace.getJSXByName("blkUI");
  var buildVersion = "Version " + topic_Get(tApp, "BuildVersion");
  var cmpLblBuildVersion = new CmpLabel("lblVersion",null,null,null,buildVersion);
  var cmpBlkDevice = new Component();
  var cmpBlkProfile = new Component();
  var cmpBlkPolicy = new Component();
  var cmpBlkSessionMonitor = new Component();
  var cmpBlkUser = new Component();
  var cmpBlkMedia = new Component(); 
  var cmpBlkSystemAdmin = new Component();

  eventApp.subscribe("MenuSelection", null, this,
                      function () {
                        var MenuSelection = topic_Get(tApp, 'MenuSelection');
                        if (MenuSelection == "SYSTEMSETTINGS") return;

                        for (var i = 0; i < cmpMenuUI.SubCmpArr.length; i++) {
                          if (cmpMenuUI.SubCmpArr[i].cmp == null) continue;
                          if (cmpMenuUI.SubCmpArr[i].selType == MenuSelection && cmpMenuUI.SubCmpArr[i].onDemandFnc != null) {
                            cmpMenuUI.BlkJSX.getParent().showMask("<span style='font-size:24px;'>Loading...</span>");
                          }
                        }
                        cmpBtnSessionMonitor.BlkJSX.setState(jsx3.gui.ImageButton.STATE_OFF);
                        cmpBtnDevice.BlkJSX.setState(jsx3.gui.ImageButton.STATE_OFF);
                        cmpBtnProfile.BlkJSX.setState(jsx3.gui.ImageButton.STATE_OFF);
                        cmpBtnUser.BlkJSX.setState(jsx3.gui.ImageButton.STATE_OFF);
                        cmpBtnMedia.BlkJSX.setState(jsx3.gui.ImageButton.STATE_OFF);
                        cmpBtnPolicy.BlkJSX.setState(jsx3.gui.ImageButton.STATE_OFF);
                        cmpBtnSystemAdmin.BlkJSX.setState(jsx3.gui.ImageButton.STATE_OFF);

                        if (MenuSelection == "DEVICE") {
                          cmpBtnDevice.BlkJSX.setState(jsx3.gui.ImageButton.STATE_ON);
                          jsx3.sleep(function () {
                            topic_Publish(tApp, "SHOWTABDATA", (newServerDate()).getTime());
                          }, null, this);
                        }
                        else if (MenuSelection == "POLICY") {
                          cmpBtnPolicy.BlkJSX.setState(jsx3.gui.ImageButton.STATE_ON);

                          var PMselect = topic_Get(tApp, "PolicyMenuSelection");
                          if (PMselect == null) {
                            topic_Publish(tApp, "PolicyMenuSelection", "GROUPTAG");
                          }
                          jsx3.sleep(function () {
                            topic_Publish(tApp, "SHOWTABDATA", (newServerDate()).getTime());
                          }, null, this);
                        }
                        else if (MenuSelection == "SESSIONMONITOR") {
                          cmpBtnSessionMonitor.BlkJSX.setState(jsx3.gui.ImageButton.STATE_ON);
                          jsx3.sleep(function () {
                            topic_Publish(tApp, "SHOWTABDATA", (newServerDate()).getTime());
                          }, null, this);
                        }
                        else if (MenuSelection == "USER") {
                          cmpBtnUser.BlkJSX.setState(jsx3.gui.ImageButton.STATE_ON);
                          jsx3.sleep(function () {
                            topic_Publish(tApp, "SHOWTABDATA", (newServerDate()).getTime());
                          }, null, this);
                        }
                        else if (MenuSelection == "MEDIA") {
                          cmpBtnMedia.BlkJSX.setState(jsx3.gui.ImageButton.STATE_ON);
                          //send getRequest for MediaStreamDirResource to subscribe.//To fix bug#25925
                          // now sending subscription id in the getdir call
//                          var cmpReqMediaSDRObj = cmpReqObj_Get("MediaSDRDataState");
//                          if ((cmpReqMediaSDRObj != null) &&
//                               (cmpReqMediaSDRObj.listreqstate == null)) {
//                            // if yes fire it off
//                            jsx3.sleep(function () {
//                              cmpReqMediaSDRObj.req.sendRequest();
//                            }, null, this);
//                          }
                          jsx3.sleep(function () {
                            topic_Publish(tApp, "SHOWTABDATA", (newServerDate()).getTime());
                          }, null, this);
                        }
                        else if (MenuSelection == "PROFILE") {
                          cmpBtnProfile.BlkJSX.setState(jsx3.gui.ImageButton.STATE_ON);
                          var ProfileSelect = topic_Get(tApp, "ProfileMenuSelection");
                          if (ProfileSelect == null) {
                            topic_Publish(tApp, "ProfileMenuSelection", "CONNECTION");
                          }
                          jsx3.sleep(function () {
                            topic_Publish(tApp, "SHOWTABDATA", (newServerDate()).getTime());
                          }, null, this);
                        }
                        else if (MenuSelection == "SYSTEMADMIN") {
                          cmpBtnSystemAdmin.BlkJSX.setState(jsx3.gui.ImageButton.STATE_ON);
                          var SystemAdminSelect = topic_Get(tApp, "SystemAdminMenuSelection");
                          if (SystemAdminSelect == null) {
                            if (clsUserPermission.getPermission('Report', 'read') == 'true') {
                              topic_Publish(tApp, "SystemAdminMenuSelection", "REPORTS");
                            }
                            else if (clsUserPermission.getPermission('System', 'backup') == 'true') {
                              topic_Publish(tApp, "SystemAdminMenuSelection", "BACKUP");
                            }
                            else if (clsUserPermission.getPermission('System', 'restore') == 'true') {
                              topic_Publish(tApp, "SystemAdminMenuSelection", "RESTORE");
                            }
                            else if (clsUserPermission.getPermission('System', 'upgrade') == 'true') {
                              topic_Publish(tApp, "SystemAdminMenuSelection", "UPGRADE");
                            }
                            else if (clsUserPermission.getPermission('System', 'read') == 'true') {
                              topic_Publish(tApp, "SystemAdminMenuSelection", "EXTERNALAUTH");
                            }
                          }
                          jsx3.sleep(function () {
                            topic_Publish(tApp, "SHOWTABDATA", (newServerDate()).getTime());
                          }, null, this);
                        }
                      });

  IpvsLogin.subscribe(com.ipvs.api.Login.ON_LOGINSTATE_CHANGE, this,
                      function(objEvent) {
                        if (objEvent.stateVar == com.ipvs.api.Login.LOGINSTATE && objEvent.stateVal == com.ipvs.api.Login.LOGIN_SUCCESS) {
                          //get superuser record
                          var loginUser = IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_USER);
                          if (loginUser == "superuser-default") {
                            cmpReq_GetSuperUser();
                          }
                          else {
                            topic_Publish(tApp, "DatabaseState", "VALID");
                          }
                        }
                      }
                     );

   eventApp.subscribe("DatabaseState", null, this,
                      function(EventVar, EventVal) {
                        if (EventVal == "VALID") {
                          //find out the read permission for the user and publish
                          if (clsUserPermission.hasAdminPermission('Asset') == 'true') {
                            topic_Publish(tApp, "UserPermissions", "VALID");
                            topic_Publish(tApp, "MenuSelection", "DEVICE");                        
                          }
                          else if (clsUserPermission.hasAdminPermission('Profile') == 'true') {
                            topic_Publish(tApp, "UserPermissions", "VALID");
                            topic_Publish(tApp, "MenuSelection", "PROFILE");                        
                          }
                          else if (clsUserPermission.hasAdminPermission('Media') == 'true') {
                            topic_Publish(tApp, "UserPermissions", "VALID");
                            topic_Publish(tApp, "MenuSelection", "MEDIA");                                                 
                          }
                          else if (clsUserPermission.hasAdminPermission('User') == 'true') {
                            topic_Publish(tApp, "UserPermissions", "VALID");
                            topic_Publish(tApp, "MenuSelection", "USER");                          
                          }
                          else if (clsUserPermission.hasAdminPermission('Policy') == 'true') {
                            topic_Publish(tApp, "UserPermissions", "VALID");
                            topic_Publish(tApp, "MenuSelection", "POLICY");                                                                                
                          }
                          else if (clsUserPermission.hasAdminPermission('Session') == 'true') {
                            topic_Publish(tApp, "UserPermissions", "VALID");
                            topic_Publish(tApp, "MenuSelection", "SESSIONMONITOR");                                                                               
                          }
                          else if (clsUserPermission.hasAdminPermission('System') == 'true'){
                            topic_Publish(tApp, "UserPermissions", "VALID"); 
                            topic_Publish(tApp, "MenuSelection", "SYSTEMADMIN");                                                                              
                          }
                          else{
                            jsx3.sleep(function() {
                                    alert("You have no permissions. Please contact your Network Administrator");
                                    IpvsLogin.doLogout();
                                 }, null, this);                          
                          }
                        }
                        else {
                          topic_Publish(tApp, "MenuSelection", "SYSTEMSETTINGS");
                        }
                      }
                    );
 
  eventApp.subscribe("MenuSelection", "SYSTEMSETTINGS", this,
                      function() {
                        dlgSystemSettings_Create();
                      } 
                    );

  var cmpTxtWelcomeText = new CmpLabel([blkUIBlkJSX,"lblWelcomeText"],
                                       null,
                                       [{action:"APPEVENT", eventvar:"LoginUser", eventval:null}]);

  var cmpMenuUI = new CmpContainer([blkUIBlkJSX, "blkMenuUI"],
                                    [{ cmp: cmpBlkDevice,
                                      onDemandFnc: cmpBlkDevice_Create,
                                      onDemandPub: "topic_Publish(tApp, 'MENUUIDATA','DEVICE')",
                                      sel: "topic_Get(tApp, 'MenuSelection') == 'DEVICE'",
                                      selType: "DEVICE"
                                    },
	                                    { cmp: cmpBlkProfile,
	                                      onDemandFnc: cmpBlkProfile_Create,
	                                      onDemandPub: "topic_Publish(tApp, 'MENUUIDATA','PROFILE')",
	                                      sel: "topic_Get(tApp, 'MenuSelection') == 'PROFILE'",
	                                      selType: "PROFILE"
	                                    },
                                      { cmp: cmpBlkPolicy,
                                        onDemandFnc: cmpBlkPolicy_Create,
                                        onDemandPub: "topic_Publish(tApp, 'MENUUIDATA','POLICY')",
                                        sel: "topic_Get(tApp, 'MenuSelection') == 'POLICY'",
                                        selType: "POLICY"
                                      },
                                      { cmp: cmpBlkSessionMonitor,
                                        onDemandFnc: cmpBlkSessionMonitor_Create,
                                        onDemandPub: "topic_Publish(tApp, 'MENUUIDATA','SESSIONMONITOR')",
                                        sel: "topic_Get(tApp, 'MenuSelection') == 'SESSIONMONITOR'",
                                        selType: "SESSIONMONITOR"
                                      },
                                      { cmp: cmpBlkUser,
                                        onDemandFnc: cmpBlkUser_Create,
                                        onDemandPub: "topic_Publish(tApp, 'MENUUIDATA','USER')",
                                        sel: "topic_Get(tApp, 'MenuSelection') == 'USER'",
                                        selType: "USER"
                                      },
                                      { cmp: cmpBlkMedia,
                                        onDemandFnc: cmpBlkMedia_Create,
                                        onDemandPub: "topic_Publish(tApp, 'MENUUIDATA','MEDIA')",
                                        sel: "topic_Get(tApp, 'MenuSelection') == 'MEDIA'",
                                        selType: "MEDIA"
                                      },
                                      { cmp: cmpBlkSystemAdmin,
                                        onDemandFnc: cmpBlkSystemAdmin_Create,
                                        onDemandPub: "topic_Publish(tApp, 'MENUUIDATA','SYSTEMADMIN')",
                                        sel: "topic_Get(tApp, 'MenuSelection') == 'SYSTEMADMIN'",
                                        selType: "SYSTEMADMIN"
                                      }
                                    ],
                                    [{ action: "APPEVENT", eventvar: "SHOWTABDATA", eventval: null}]);
                                    //null);


  var cmpBtnDevice = new CmpImageButtonButton([blkUIBlkJSX,"btnAssets"], null, { action: "APPEVENT", eventvar: "MenuSelection", eventval: "DEVICE" });
  var cmpBtnProfile = new CmpImageButtonButton([blkUIBlkJSX,"btnProfiles"], null, { action: "APPEVENT", eventvar: "MenuSelection", eventval: "PROFILE" });
  var cmpBtnPolicy = new CmpImageButtonButton([blkUIBlkJSX,"btnPolicy"], null, { action: "APPEVENT", eventvar: "MenuSelection", eventval: "POLICY" });
  var cmpBtnSessionMonitor = new CmpImageButtonButton([blkUIBlkJSX,"btnSessionMonitor"], null, { action: "APPEVENT", eventvar: "MenuSelection", eventval: "SESSIONMONITOR" });
  var cmpBtnUser = new CmpImageButtonButton([blkUIBlkJSX,"btnUsers"], null, { action: "APPEVENT", eventvar: "MenuSelection", eventval: "USER" });
  var cmpBtnMedia = new CmpImageButtonButton([blkUIBlkJSX,"btnMedia"], null, { action: "APPEVENT", eventvar: "MenuSelection", eventval: "MEDIA" });
  var cmpBtnSystemAdmin = new CmpImageButtonButton([blkUIBlkJSX,"btnSystemAdmin"], null, {action:"APPEVENT", eventvar:"MenuSelection", eventval:"SYSTEMADMIN"});

  var cmpBtnLogout = new CmpButton([blkUIBlkJSX, "btnLogOut"], null,
                                    { action: "FNCCALL", context: this,
                                      fnc: function () {
                                        getExportStatus();
                                        //IpvsLogin.doLogout();
                                      }
                                    });


  var cmpAppTitle = new Component([blkUIBlkJSX,"appTitle"], null, null, null);
  cmpAppTitle.customizeSkin = function() {
                                var appTitle = clsCustomizeSkin.getAttrValue("IPVSAdmin", "appTitle");
                                if (!jsx3.util.strEmpty(appTitle)) {
                                  cmpAppTitle.BlkJSX.setText(appTitle, true);
                                }
                              }

  var cmpAdminHeaderLogo = new Component([blkUIBlkJSX,"adminHeaderLogo"], null, null, null);
  cmpAdminHeaderLogo.customizeSkin = function() {
                                      var adminHeaderLogo = clsCustomizeSkin.getAttrValue("IPVSAdmin", "adminHeaderLogo");
                                      if (!jsx3.util.strEmpty(adminHeaderLogo)) {
                                        cmpAdminHeaderLogo.BlkJSX.setSrc(adminHeaderLogo);
                                        cmpAdminHeaderLogo.BlkJSX.repaint();
                                      }
                                    }

  var cmpBlkTabMenu = new CmpContainer([blkUIBlkJSX, "blkMain"],
                                      [{ cmp: cmpBtnDevice,
                                          sel: "topic_Get(tApp, 'MenuSelection') != 'SYSTEMSETTINGS' && clsUserPermission.hasAdminPermission('Asset') == 'true'"
                                        },
	                                      { cmp: cmpBtnProfile,
	                                        sel: "topic_Get(tApp, 'MenuSelection') != 'SYSTEMSETTINGS' && clsUserPermission.hasAdminPermission('Profile') == 'true'"
	                                      },
                                        { cmp: cmpBtnPolicy,
                                          sel: "topic_Get(tApp, 'MenuSelection') != 'SYSTEMSETTINGS' && clsUserPermission.hasAdminPermission('Policy') == 'true'"
                                        },
                                        { cmp: cmpBtnSessionMonitor,
                                          sel: "topic_Get(tApp, 'MenuSelection') != 'SYSTEMSETTINGS' && clsUserPermission.hasAdminPermission('Session') == 'true'"
                                        },
                                        { cmp: cmpBtnUser,
                                          sel: "topic_Get(tApp, 'MenuSelection') != 'SYSTEMSETTINGS' && clsUserPermission.hasAdminPermission('User') == 'true'"
                                        },
                                        { cmp: cmpBtnMedia,
                                          sel: "topic_Get(tApp, 'MenuSelection') != 'SYSTEMSETTINGS' && clsUserPermission.hasAdminPermission('Media') == 'true'"
                                        },
									                      { cmp: cmpBtnSystemAdmin,
									                        sel: "topic_Get(tApp, 'MenuSelection') != 'SYSTEMSETTINGS' && clsUserPermission.hasAdminPermission('System') == 'true'"
									                      }
                                      ],
                                      [{ action: "APPEVENT", eventvar: "MenuSelection", eventval: null}]);                                    

  var cmpBlkMenu = new CmpContainer( [blkUIBlkJSX,"blkMain"],                             
                                      [ {cmp:cmpAppTitle},
                                        {cmp:cmpAdminHeaderLogo},
                                        {cmp:cmpBlkTabMenu},
                                        {cmp:cmpBtnLogout },
                                        {cmp:cmpTxtWelcomeText },
                                        {cmp:cmpLblBuildVersion },
                                        {cmp:cmpMenuUI} 
                                      ],
                                      null );

  var cmpBlkMain = new CmpContainer( "blkMain",
                                       [{cmp:cmpBlkMenu,
                                         sel:"IpvsLogin.getStateParam(com.ipvs.api.Login.LOGINSTATE) == com.ipvs.api.Login.LOGIN_SUCCESS && " + 
                                             "topic_Get(tApp, 'UserPermissions') == 'VALID'"}
                                       ],
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
                                        {action:"APPEVENT", eventvar:"UserPermissions", eventval:null}] );


  function getExportStatus() {
    var reqHandleExportImportMedia = new ReqExportImportMedia("QueueStatusExportImportFile", null,
                        { action: "FNCCALL", context: this,
                          fnc: function (respCode, respVal) {
                            if (respCode == "SUCCESS" && respVal != null) {
                              var exportStatus = false;
                              var queueState = respVal.selectSingleNode(".//QueueState").getValue();
                              if ( queueState == "running") {
                                exportStatus = true;
                              }
                              processLogout(exportStatus);
                            }
                          }
                        }
                        );

    reqHandleExportImportMedia.sendRequest();
  }

  function processLogout(exportStatus) {
      if (exportStatus) {
          var msg = 'Are you sure you want to log out?\n System is exporting media file. You will lose\n the file if you continue.';
          var agree = confirm(msg);
          if (agree) {
              IpvsLogin.doLogout();
          }
      }
      else {
          IpvsLogin.doLogout();      
      }
  }
                                            
  cmpBlkMain.init();                                                                              
  cmpApp.SubCmpArr.push({ cmp: cmpBlkMain });

} // end cmpAppMenu_Create

function cmpBlkMain_Destroy() {
  var blkMainBlkJSX = PNameSpace.getJSXByName("blkMain");
  blkMainBlkJSX.removeChildren();
}

function cmpApp_Destroy() {
  cmpBlkDevice_Destroy();
  cmpBlkSystemAdmin_Destroy();
  cmpBlkSessionMonitor_Destroy();
  cmpBlkProfile_Destroy();
  cmpBlkPolicy_Destroy();
  cmpBlkMedia_Destroy();
  cmpBlkUser_Destroy();
  cmpBlkMain_Destroy();
}
