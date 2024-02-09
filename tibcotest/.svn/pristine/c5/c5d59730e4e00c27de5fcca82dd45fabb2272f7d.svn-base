// a topic / listner for messaging in it
var tPreferences = "BlkPreferences";
var tPreferencesDoc = null;
var eventPreferences = null;
// the top level block
var winPreferences = null;

var PreferencesCallerID = null;

function cmpWinPreferences_Create(tCaller)
{
  // save the callers topic/name in a gloabl static var
  // this will be used to target events later
  PreferencesCallerID = tCaller;

  // create only once
  if (winPreferences != null) {
    winPreferences.show();
    return;
  }
  
  // create a topic / listner for messaging in it
  tPreferencesDoc = topic_Create(tPreferences);
  eventPreferences = new EventListner(tPreferences);

  function cmpWinPreferences_Destroy() {
    winPreferences.BlkJSX.unsubscribeAllFromAll();
    winPreferences.unsubscribeEnterKeyAction();
    // destroy any eventlistners
    eventPreferences.destroy();
    eventPreferences = null;
    // destroy the local topic
    tPreferencesDoc = null;
    topic_Destroy(tPreferences);
    // dereference the top block
    winPreferences = null;
  }
  
  // Init/Open a window
  winPreferences = new CmpAppWindow(tPreferences, eventPreferences, "components/blkPreferences.xml",
                                   { WinTitle: "Preferences and Profiles",
                                     WinHeight: 630,
                                     WinWidth: 380,
                                     WinResizable: false,
                                     WinScrollable: true
                                   },
                                    null,
                                    null,
                                    { action: "FNCCALL",
                                      context: null,
                                      fnc: function () {
                                        cmpWinPreferences_Destroy();
                                      }
                                    }
                                  );

                                    winPreferences.createSubCmp = function () {
                                      // get vars from object instance
                                      var winPreferencesBlkJSX = this.BlkJSX;
                                      var tPreferences = this.tWin;
                                      var eventPreferences = this.eventWin;

                                      // get the windowTitle
                                      var cmpClientWindowTitle = new Component([winPreferencesBlkJSX, "appTitle"], null, null, null);
                                      cmpClientWindowTitle.customizeSkin = function () {
                                        var clientWindowTitle = clsCustomizeSkin.getAttrValue("IPVSClient", "appTitle");
                                        if (!jsx3.util.strEmpty(clientWindowTitle)) {
                                          cmpClientWindowTitle.BlkJSX.setText(clientWindowTitle, true);
                                        }
                                      }

                                      //get the adminHeaderLogo
                                      var cmpClientWindowHeaderLogo = new Component([winPreferencesBlkJSX, "clientHeaderLogo"], null, null, null);
                                      cmpClientWindowHeaderLogo.customizeSkin = function () {
                                        var clientWindowHeaderLogo = clsCustomizeSkin.getAttrValue("IPVSClient", "clientHeaderLogo");
                                        if (!jsx3.util.strEmpty(clientWindowHeaderLogo)) {
                                          cmpClientWindowHeaderLogo.BlkJSX.setSrc(clientWindowHeaderLogo);
                                          cmpClientWindowHeaderLogo.BlkJSX.repaint();
                                        }
                                      }

                                      // FPS info
                                      var defaultFps = getLocalPreferences(PNameSpace, ".//Player/Fps", "15");
                                      // Border
                                      var border = getLocalPreferences(PNameSpace, ".//Player/Border", "0");
                                      // UseHTTP
                                      var useHTTP = getLocalPreferences(PNameSpace, ".//Player/UseHTTP", "1");
                                      // AspectRatio
                                      var aspectRatio = getLocalPreferences(PNameSpace, ".//Player/AspectRatio", "0:0");
                                      // SkipFWD
                                      var skipFWD = getLocalPreferences(PNameSpace, ".//Player/SkipFWD", "30000");
                                      // SkipBack
                                      var skipBack = getLocalPreferences(PNameSpace, ".//Player/SkipBack", "10000");

                                      //add Auto Join 
                                      var cmpBlkAutoJoin = new Component("blkAutoJoin");
                                      var cmpChkAutoJoin = new CmpCheckBox("chkAutoJoin", null,
                                            { action: "FNCCALL",
                                              context: this,
                                              fnc: function () {
                                                topic_Publish(tPreferences, "SaveStatus", '');
                                              }
                                            });

                                      //add FPS
                                      var cmpBlkFPS = new Component("lblFps");
                                      var cmpTxtFPS = new CmpTxtBoxWithBlur("txtFps", defaultFps, null,
                                            { action: "FNCCALL",
                                              context: this,
                                              fnc: function () {
                                                //validateAndProcessFPSTxtVal(cmpTxtFPS, "FPS value should be numeric and in range 1-60");
                                              }
                                            });

                                      //add Border
                                      var cmpBlkBoder = new Component("blkBoder");
                                      var cmpTxtBoder = new CmpTxtBox("txtBoder", border, null, null,
                                      { action: "FNCCALL",
                                        context: this,
                                        fnc: function () {
                                          //validateAndProcessTxtVal(cmpTxtBoder, "Border value should be numeric");
                                        }
                                      });

                                      //add UseHTTP
                                      var cmpBlkUseHTTP = new Component("lblUseHTTP");
                                      var cmpTxtUseHTTP = new CmpTxtBox("txtUseHTTP", useHTTP, null, null,
                                        { action: "FNCCALL",
                                          context: this,
                                          fnc: function () {
                                            //validateAndProcessHttpVal(cmpTxtUseHTTP, "UseHTTP value should be '0'/'1'");
                                          }
                                        });

                                      //add AspectRatio
                                      var cmpBlkAspectRatio = new Component("lblAspectRatio");
                                      var cmpTxtAspectRatio = new CmpTxtBox("txtAspectRatio", aspectRatio, null, null,
                                            { action: "FNCCALL",
                                              context: this,
                                              fnc: function () {
                                                topic_Publish(tPreferences, "SaveStatus", '');
                                              }
                                            });

                                      //add SkipFWD
                                      var cmpBlkSkipFWD = new Component("lblSkipFWD");
                                      var cmpTxtSkipFWD = new CmpTxtBox("txtSkipFWD", skipFWD, null, null,
                                        { action: "FNCCALL",
                                          context: this,
                                          fnc: function () {
                                            //validateAndProcessTxtVal(cmpTxtSkipFWD, "SkipFWD value should be in milliseconds");
                                          }
                                        });

                                      //add SkipBack
                                      var cmpBlkSkipBack = new Component("lblSkipBack");
                                      var cmpTxtSkipBack = new CmpTxtBox("txtSkipBack", skipBack, null, null,
                                          { action: "FNCCALL",
                                            context: this,
                                            fnc: function () {
                                              //validateAndProcessTxtVal(cmpTxtSkipBack, "SkipBack value should be in milliseconds");
                                            }
                                          });

                                      //add Player Priority
                                      var cmpBlkPlayerPriority = new Component("lblPriority");
                                      var cmpChkPriority = new CmpCheckBox("chkPriority", null,
                                              { action: "FNCCALL",
                                                context: this,
                                                fnc: function () {
                                                  topic_Publish(tPreferences, "SaveStatus", '');
                                                }
                                              });

                                      //show bookmarks
                                      var cmpBlkShowBookmarks = new Component("lblBookmarks");
                                      var cmpChkShowBookmarks = new CmpCheckBox("chkShowBookmarks", null,
                                                  { action: "FNCCALL",
                                                    context: this,
                                                    fnc: function () {
                                                      topic_Publish(tPreferences, "SaveStatus", '');
                                                    }
                                                  });

                                      //show connections
                                      var cmpBlkShowConnections = new Component("lblConnections");
                                      var cmpChkShowConnections = new CmpCheckBox("chkShowConnections", null,
                                                    { action: "FNCCALL",
                                                      context: this,
                                                      fnc: function () {
                                                        topic_Publish(tPreferences, "SaveStatus", '');
                                                      }
                                                    });

                                      //single player mode
                                      var cmpBlkSinglePlayerMode = new Component("lblSinglePlayerMode");
                                      var cmpChkSinglePlayerMode = new CmpCheckBox("chkSinglePlayerMode", null,
                                                    { action: "FNCCALL",
                                                      context: this,
                                                      fnc: function () {
                                                        topic_Publish(tPreferences, "SaveStatus", '');
                                                      }
                                                    });

                                      //show bookmarks
                                      var cmpBlkShowController = new Component("lblController");
                                      var cmpChkShowController = new CmpCheckBox("chkShowController", null,
                                                    { action: "FNCCALL",
                                                      context: this,
                                                      fnc: function () {
                                                        topic_Publish(tPreferences, "SaveStatus", '');
                                                      }
                                                    });

                                      var cmpTxtSaveStatus = new CmpLabel("lblPerfMessage", null,
                                        [{ action: "TOPICEVENT", topic: tPreferences, eventvar: "SaveStatus", eventval: null}],
                                        null, "");

                                      // Save button
                                      var cmpBtnSave = new CmpBlockButton("btnSave", null,
                                   { action: "FNCCALL", context: this,
                                     fnc: function () {
                                       doSave();
                                     }
                                   });

                                      // Exit button
                                      var cmpBtnExit = new CmpBlockButton("btnExit", null,
                                  { action: "FNCCALL", context: this,
                                    fnc: function () {
                                      winPreferences.close();
                                    }
                                  });

                                      //--- Top Level block
                                      var cmpBlkUI = new CmpContainer("blkUI",
                                  [{ cmp: cmpBlkAutoJoin },
                                    { cmp: cmpChkAutoJoin },
                                    { cmp: cmpBlkFPS },
                                    { cmp: cmpTxtFPS },
                                    { cmp: cmpBlkBoder },
                                    { cmp: cmpTxtBoder },
                                    { cmp: cmpBlkUseHTTP },
                                    { cmp: cmpTxtUseHTTP },
                                    { cmp: cmpBlkAspectRatio },
                                    { cmp: cmpTxtAspectRatio },
                                    { cmp: cmpBlkSkipFWD },
                                    { cmp: cmpTxtSkipFWD },
                                    { cmp: cmpBlkSkipBack },
                                    { cmp: cmpTxtSkipBack },
                                    { cmp: cmpBlkPlayerPriority },
                                    { cmp: cmpChkPriority },
                                    { cmp: cmpBlkShowBookmarks },
                                    { cmp: cmpChkShowBookmarks },
                                    { cmp: cmpBlkShowConnections },
                                    { cmp: cmpChkShowConnections },
                                    { cmp: cmpBlkSinglePlayerMode },
                                    { cmp: cmpChkSinglePlayerMode },
                                    { cmp: cmpBlkShowController },
                                    { cmp: cmpChkShowController },
                                    { cmp: cmpTxtSaveStatus },
                                    { cmp: cmpBtnSave },
                                    { cmp: cmpBtnExit }
                                   ],
                                   null);

                                      // Top level sub cmp array
                                      this.SubCmpArr = [{ cmp: cmpBlkAutoJoin,
                                        sel: "topic_Get(tApp, 'ClientLaunchMode') != 'UnManned'"
                                      },
                      { cmp: cmpChkAutoJoin,
                        sel: "topic_Get(tApp, 'ClientLaunchMode') != 'UnManned'"
                      },
                      { cmp: cmpBlkFPS },
                      { cmp: cmpTxtFPS },
                      { cmp: cmpBlkBoder },
                      { cmp: cmpTxtBoder },
                      { cmp: cmpBlkUseHTTP },
                      { cmp: cmpTxtUseHTTP },
                      { cmp: cmpBlkAspectRatio },
                      { cmp: cmpTxtAspectRatio },
                      { cmp: cmpBlkSkipFWD },
                      { cmp: cmpTxtSkipFWD },
                      { cmp: cmpBlkSkipBack },
                      { cmp: cmpTxtSkipBack },
                      { cmp: cmpBlkPlayerPriority },
                      { cmp: cmpChkPriority },
                      { cmp: cmpBlkShowBookmarks },
                      { cmp: cmpChkShowBookmarks },
                      { cmp: cmpBlkShowConnections },
                      { cmp: cmpChkShowConnections },
                      { cmp: cmpBlkSinglePlayerMode },
                      { cmp: cmpChkSinglePlayerMode },
                      { cmp: cmpBlkShowController },
                      { cmp: cmpChkShowController },
                      { cmp: cmpTxtSaveStatus },
                      { cmp: cmpBtnSave },
                      { cmp: cmpBtnExit },
                      { cmp: cmpClientWindowTitle },
                      { cmp: cmpClientWindowHeaderLogo }
                     ];

                                      function getDefaultPlayerSettings() {
                                        //bookmarks
                                        var bookMarks = getLocalPreferences(PNameSpace, ".//ClientApp[@name='IPVSClient']/ShowBookmarks", "false");
                                        //connections
                                        var connections = getLocalPreferences(PNameSpace, ".//ClientApp[@name='IPVSClient']/ShowConnections", "false");
                                        //single player
                                        var singlePlayer = getLocalPreferences(PNameSpace, ".//ClientApp[@name='IPVSClient']/SinglePlayerMode", "false");
                                        //controller
                                        var controller = getLocalPreferences(PNameSpace, ".//ClientApp[@name='IPVSClient']/ShowController", "true");
                                        //Auto Join Info 
                                        var autoJoin = getLocalPreferences(PNameSpace, ".//ClientApp[@name='IPVSClient']/AutoJoin", "false");
                                        // Priority
                                        var priority = getLocalPreferences(PNameSpace, ".//Player/Priority", "false");

                                        cmpChkAutoJoin.BlkJSX.setValue(autoJoin);
                                        cmpChkPriority.BlkJSX.setValue(priority);
                                        cmpChkShowBookmarks.BlkJSX.setValue(bookMarks);
                                        cmpChkShowConnections.BlkJSX.setValue(connections);
                                        cmpChkSinglePlayerMode.BlkJSX.setValue(singlePlayer);
                                        cmpChkShowController.BlkJSX.setValue(controller);
                                      }

                                      function doSave() {
                                        //add AutoJoin 
                                        var valAutoJoin = false;
                                        if (cmpChkAutoJoin.BlkJSX.getChecked()) {
                                          valAutoJoin = true;
                                        }

                                        //add player priority
                                        var valPlayerPriority = false;
                                        if (cmpChkPriority.BlkJSX.getChecked()) {
                                          valPlayerPriority = true;
                                        }
                                        //player settings
                                        var showBookmarks = false;
                                        if (cmpChkShowBookmarks.BlkJSX.getChecked()) {
                                          showBookmarks = true;
                                        }
                                        var showConnections = false;
                                        if (cmpChkShowConnections.BlkJSX.getChecked()) {
                                          showConnections = true;
                                        }
                                        var singlePlayerMode = false;
                                        if (cmpChkSinglePlayerMode.BlkJSX.getChecked()) {
                                          singlePlayerMode = true;
                                        }
                                        var showController = false;
                                        if (cmpChkShowController.BlkJSX.getChecked()) {
                                          showController = true;
                                        }

                                        topic_Publish(tPreferences, "SaveStatus", '');
                                        if (valAspectRatio == "") {
                                          windowAlert(winPreferencesBlkJSX, "Please enter a value of AspectRatio.");
                                          cmpTxtAspectRatio.BlkJSX.focus();
                                          return false;
                                        }
                                        else if (!validateAndProcessTxtVal(cmpTxtSkipFWD, "Please enter a value of Skip Forward in milliseconds.")) {
                                          return false;
                                        }
                                        else if (!validateAndProcessTxtVal(cmpTxtSkipBack, "Please enter a value of Skip Back in milliseconds.")) {
                                          return false;
                                        }
                                        else if (!validateAndProcessFPSTxtVal(cmpTxtFPS, "FPS value should be numeric and in range 1-60.")) {
                                          return false;
                                        }
                                        else if (!validateAndProcessHttpVal(cmpTxtUseHTTP, "UseHTTP value should be '0'/'1'")) {
                                          return false;
                                        }
                                        else if (!validateAndProcessTxtVal(cmpTxtBoder, "Please enter a numeric value of player border.")) {
                                          return false;
                                        }
                                        else {
                                          try {
                                            //add FPS
                                            var valFPS = cmpTxtFPS.BlkJSX.getValue();
                                            //add Border
                                            var valBorder = cmpTxtBoder.BlkJSX.getValue();
                                            //add UseHTTP
                                            var valUseHTTP = cmpTxtUseHTTP.BlkJSX.getValue();
                                            //add AspectRatio
                                            var valAspectRatio = cmpTxtAspectRatio.BlkJSX.getValue();
                                            //add SkipFWD
                                            var valSkipFWD = cmpTxtSkipFWD.BlkJSX.getValue();
                                            //add SkipBack
                                            var valSkipBack = cmpTxtSkipBack.BlkJSX.getValue();

                                            //set AutoJoin to Preferences.xml
                                            setLocalPreferences("GLOBAL", ".//ClientApp[@name='IPVSClient']/AutoJoin", valAutoJoin);
                                            //set DefaultAutoJoin to tApp
                                            topic_Set(tApp, "DefaultAutoJoin", valAutoJoin);

                                            //set valFPS to Preferences.xml
                                            setLocalPreferences("GLOBAL", ".//Player/Fps", valFPS);
                                            //set IPVSPlayerDefaultFps to tApp
                                            topic_Set(tApp, "IPVSPlayerDefaultFps", valFPS);

                                            //set valBorder to Preferences.xml
                                            setLocalPreferences("GLOBAL", ".//Player/Border", valBorder);
                                            //set valBorder to tApp
                                            topic_Set(tApp, "IPVSPlayerDefaultBorder", valBorder);

                                            //set valUseHTTP to Preferences.xml
                                            setLocalPreferences("GLOBAL", ".//Player/UseHTTP", valUseHTTP);
                                            //set valUseHTTP to tApp
                                            topic_Set(tApp, "IPVSPlayerDefaultUseHTTP", valUseHTTP);

                                            //set valAspectRatio to Preferences.xml
                                            setLocalPreferences("GLOBAL", ".//Player/AspectRatio", valAspectRatio);
                                            //set valAspectRatio to tApp
                                            topic_Set(tApp, "IPVSPlayerDefaultAspectRatio", valAspectRatio);

                                            //set valSkipFWD to Preferences.xml
                                            setLocalPreferences("GLOBAL", ".//Player/SkipFWD", valSkipFWD);
                                            //set valSkipFWD to tApp
                                            topic_Set(tApp, "IPVSPlayerDefaultSkipFWD", valSkipFWD);

                                            //set valSkipBack to Preferences.xml
                                            setLocalPreferences("GLOBAL", ".//Player/SkipBack", valSkipBack);
                                            //set valSkipBack to tApp
                                            topic_Set(tApp, "IPVSPlayerDefaultSkipBack", valSkipBack);

                                            //set valPlayerPriority to Preferences.xml
                                            setLocalPreferences("GLOBAL", ".//Player/Priority", valPlayerPriority);
                                            //set valPlayerPriority to tApp
                                            topic_Set(tApp, "IPVSPlayerDefaultPriority", valPlayerPriority);

                                            //default playet settings
                                            setLocalPreferences("GLOBAL", ".//ClientApp[@name='IPVSClient']/ShowBookmarks", showBookmarks);
                                            setLocalPreferences("GLOBAL", ".//ClientApp[@name='IPVSClient']/ShowConnections", showConnections);
                                            setLocalPreferences("GLOBAL", ".//ClientApp[@name='IPVSClient']/SinglePlayerMode", singlePlayerMode);
                                            setLocalPreferences("GLOBAL", ".//ClientApp[@name='IPVSClient']/ShowController", showController);
                                            topic_Publish(tPreferences, "SaveStatus", 'Settings Saved');
                                          }
                                          catch (e) {
                                            topic_Publish(tPreferences, "SaveStatus", 'ERROR: ' + e.description);
                                          }
                                        }
                                        return true;
                                      }

                                      this.subscribeEnterKeyAction = function () {
                                        jsx3.gui.Event.subscribe(jsx3.gui.Event.KEYPRESS, eval(handleEnterKeyAction));
                                      }

                                      this.unsubscribeEnterKeyAction = function () {
                                        jsx3.gui.Event.unsubscribe(jsx3.gui.Event.KEYPRESS, eval(handleEnterKeyAction));
                                      }

                                      function handleEnterKeyAction(objEvent) {
                                        if (winPreferences.BlkJSX.getWindow().document.hasFocus()) {
                                          if (objEvent.event.enterKey()) {
                                            doSave();
                                          }
                                        }
                                      }

                                      function validateAndProcessTxtVal(objTxt, errMsg) {
                                        var prevVal = objTxt.BlkJSX.jsxvalue;
                                        var newVal = objTxt.BlkJSX.getValue();
                                        newVal = parseInt(newVal, 10);
                                        if (!jsx3.util.strEmpty(newVal) && !/\D/.test(newVal)) {
                                          objTxt.BlkJSX.setValue(newVal);
                                          topic_Publish(tPreferences, "SaveStatus", '');
                                          return true;
                                        }
                                        else {
                                          windowAlert(winPreferencesBlkJSX, errMsg);
                                          objTxt.BlkJSX.setValue(prevVal);
                                          objTxt.BlkJSX.focus();
                                          return false;
                                        }
                                      }

                                      function validateAndProcessHttpVal(objTxt, errMsg) {
                                        var useHTTPVal = objTxt.BlkJSX.getValue();
                                        useHTTPVal = parseInt(useHTTPVal, 10);
                                        if (jsx3.util.strEmpty(useHTTPVal) || useHTTPVal != 1 && useHTTPVal != 0) {
                                          windowAlert(winPreferencesBlkJSX, "UseHTTP value should be '0'/'1'");
                                          objTxt.BlkJSX.setValue(cmpTxtUseHTTP.BlkJSX.jsxvalue);
                                          objTxt.BlkJSX.focus();
                                          return false;
                                        }
                                        else {
                                          objTxt.BlkJSX.setValue(useHTTPVal);
                                          return true;
                                        }
                                      }

                                      function validateAndProcessFPSTxtVal(objTxt, errMsg) {
                                        var prevVal = objTxt.BlkJSX.jsxvalue;
                                        var newVal = objTxt.BlkJSX.getValue();
                                        newVal = parseInt(newVal, 10);
                                        if (!jsx3.util.strEmpty(newVal) && !/\D/.test(newVal)) {
                                          if (newVal < 1 || newVal > 60) {
                                            windowAlert(winPreferencesBlkJSX, "FPS value should be numeric and in range 1-60");
                                            objTxt.BlkJSX.setValue(objTxt.BlkJSX.jsxvalue);
                                            objTxt.BlkJSX.focus();
                                            return false;
                                          }
                                          else {
                                            objTxt.BlkJSX.setValue(newVal);
                                            topic_Publish(tPreferences, "SaveStatus", '');
                                            return true;
                                          }
                                        }
                                        else {
                                          windowAlert(winPreferencesBlkJSX, errMsg);
                                          objTxt.BlkJSX.setValue(prevVal);
                                          objTxt.BlkJSX.focus();
                                          return false;
                                        }

                                      }

                                      // init and repaint happens after the window is loaded
                                      jsx3.sleep((function () {
                                        this.init();
                                        this.show();
                                        // Trigger Save Action for EnterKey
                                        this.subscribeEnterKeyAction();
                                        getDefaultPlayerSettings();
                                      }), null, this);
                                    }
 
  // open the window - which will call createSub - which will init and show
  winPreferences.open();
}


