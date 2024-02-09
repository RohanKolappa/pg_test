var CollaborationList = [];

function cmpWinCollaboration_Create(SessionConfigNID, collabName) {

  // create a dynamic topic based on NID
  var tCollaboration = "Session_" + jsx3.xml.CDF.getKey();
  // create only once
  if( CollaborationList[tCollaboration] != null ) {
    CollaborationList[tCollaboration].show();
    return;
  }

  // create a topic / listner for messaging in it
  var tCollaborationDoc = topic_Create(tCollaboration);
  var eventCollaboration = new EventListner(tCollaboration);

  //init "SaveAndClose"
  topic_Set(tCollaboration, "SaveAndClose", "NONE");
  //init "CollaborationState"
  topic_Set(tCollaboration, "CollaborationState", "NONE");

  // set input NID in topic
  if (SessionConfigNID == null) SessionConfigNID = "NONE";
  topic_Set(tCollaboration, "SessionConfigNID", SessionConfigNID);
  
  // set the window title name as the collaboration name
  var windowName = "New Session Config";
  if (SessionConfigNID != null) {
    windowName = "Session Config";
    if( !jsx3.util.strEmpty(collabName) ) {
      windowName += " : " + collabName;
    }
  }
  
  topic_Set(tCollaboration, "MediaSDRGetDataState", "NONE" );
  
  function cmpWinCollaboration_Destroy()
  {
    if(tMediaList != null) {
      var winMediaListHandler = PNameSpace.getAppWindow(tMediaList);
      if (/*(topic_Get(tCollaboration, "SaveAndClose") != "NONE") &&*/
          (winMediaListHandler != null) && (winMediaListHandler.isOpen()))
      {
        winMediaListHandler.close();
      }
    }
    // destroy any eventlistners
    eventCollaboration.destroy();
    var pubSubLength = eventCollaboration.PubSubList.length;
    for (--pubSubLength; pubSubLength >= 0; pubSubLength--) {
      eventCollaboration.unsubscribe(eventCollaboration.PubSubList[pubSubLength].Var, eventCollaboration.PubSubList[pubSubLength].Id);
    }
    eventCollaboration = null;
    // destroy the local topic
    tCollaborationDoc.removeChildren();
    tCollaborationDoc = null;
    topic_Destroy(tCollaboration);

    if (winCollaboration.SubCmpArr == null) return;
    for (var i = 0; i < winCollaboration.SubCmpArr.length; i++) {
      if (winCollaboration.SubCmpArr[i].cmp == null) continue;
      winCollaboration.SubCmpArr[i].cmp.destroy();
      winCollaboration.SubCmpArr[i].cmp = null;
      delete winCollaboration.SubCmpArr[i];
    }
    delete winCollaboration.SubCmpArr;

    // dereference the top block
    CollaborationList[tCollaboration] = null;
  }

    var winCollaboration = new CmpAppWindow(tCollaboration, eventCollaboration, "components/collaboration.xml",
                                             { WinTitle: windowName,
                                               WinHeight: 600,
                                               WinWidth: 1150,
                                               WinResizable: true,
                                              WinScrollable:true},
                                              null,
                                              null,
                                              { action: "FNCCALL",
                                                context: null,
                                               fnc: function(){
                                                  var tt = "tt";
                                                  if( topic_Get(tCollaboration, "CollaborationState") != "NONE" ) {  
                                                    // shut down the controller
                                                    topic_Publish(tCollaboration, "ControllerEventRequest", "STOP");
                                                    eventCollaboration.subscribe("CollaborationState", "NONE", this,
                                                                                   function() {
                                                                                     cmpWinCollaboration_Destroy();
                                                                                   });
                                                  }
                                                  else {
                                                    cmpWinCollaboration_Destroy();
                                                  }
                                                }
                                              }
                                            );
    winCollaboration.SessionConfigNID = SessionConfigNID;
    // adding to the list
    CollaborationList[tCollaboration] = winCollaboration;

    // -- METHOD to create / subscribe/ init/ show - all the sub comonents of the top level window
    // all the window cmp logic is here
    winCollaboration.createSubCmp = function () {

      // get vars from object instance
      var winCollaboration = this;
      var winCollaborationBlkJSX = this.BlkJSX;
      var tCollaboration = this.tWin;
      var eventCollaboration = this.eventWin;
      var ccNID = this.SessionConfigNID;

      // close / cancel button
      this.cmpBtnCollaborationCancel = new CmpButton([winCollaborationBlkJSX, "btnNewSessionCancel"],
                                                   null,
                                                   { action: "FNCCALL",
                                                     context: this,
                                                     fnc: function () {
                                                       // close window 
                                                       this.close();
                                                     }
                                                   });

      // save button 
      this.cmpBtnCollaborationSave = new CmpButton([winCollaborationBlkJSX, "btnNewSessionSave"],
                                                   null,
                                                   { action: "FNCCALL",
                                                     context: this,
                                                     fnc: function () {
                                                       // make sure it at least has a name
                                                       var collaborationName = this.sessionConfig.getName();
                                                       if (jsx3.util.strEmpty(collaborationName.trim())) {
                                                         windowAlert(winCollaborationBlkJSX, "Please enter Collaboration name");
                                                         return false;
                                                       }
                                                       // validate the pvr trim length.
                                                       winCollaboration.validatePVRTrimLength();

                                                       // get the logged in user
                                                       var userGroupsNode = topic_GetNode(tApp, "//ServiceClientData//User/Groups").cloneNode(true);
                                                       if (jsx3.util.strEmpty(userGroupsNode)) {
                                                         var agree = windowConfirm(winCollaborationBlkJSX, 'Session will be saved without tags.');
                                                         if (!agree) {
                                                           return false;
                                                         }
                                                       }
                                                       else {
                                                         // add the logged in user groups
                                                         this.sessionConfig.setGroupsNode(userGroupsNode);
                                                       }

                                                       // decide on set mode
                                                       var SetMode = "Add";
                                                       // get the current cc node from topic
                                                       var ccNode = this.sessionConfig.getRootNode().cloneNode(true);
                                                       var ccNID = ccNode.getAttribute("NID");
                                                       if (!jsx3.util.strEmpty(ccNID)) {
                                                         if (ccNID != "DEFAULT_SESSIONCONFIG_NID") {
                                                           SetMode = "Replace";
                                                         }
                                                         else {
                                                           ccNode.setAttribute("NID", "");
                                                         }
                                                       }
                                                       // send request - will close window on success
                                                       cmpReq_SetCollaborationConfig.call(this, tCollaboration, ccNode, SetMode);
                                                       topic_Publish(tCollaboration, "CollaborationEventMsg", " \nSaving Session...");

                                                     }
                                                   });

      // Cancel button of the groupTag Block                                                       
      this.cmpBtnCollaborationGroupTagCancel = new CmpButton([winCollaborationBlkJSX, "btnCollabGroupTagCancel"],
                                                           null,
                                                           { action: "FNCCALL",
                                                             context: null,
                                                             fnc: function () {
                                                               topic_Publish(tCollaboration, "CollaborationGroupTagSelected", "NONE");
                                                             }
                                                           }
                                                        );

      // Done button of the groupTag Block                                                       
      this.cmpBtnCollaborationGroupTagDone = new CmpButton([winCollaborationBlkJSX, "btnCollabGroupTagDone"],
                                                           null,
                                                           { action: "FNCCALL",
                                                             context: null,
                                                             fnc: function () {
                                                               var groupTagBlkJSX = getJsxByName([winCollaborationBlkJSX, "mtxCollabGroupTag"]);
                                                               winCollaboration.updateGroupTag(groupTagBlkJSX);
                                                               topic_Publish(tCollaboration, "CollaborationGroupTagSelected", "NONE");
                                                             }
                                                           }
                                                        );

      this.cmpBtnCollaborationGroupTagAdd = new CmpButton([winCollaborationBlkJSX, "btnCollabGroupTagAdd"],
                                                          null,
                                                          { action: "FNCCALL",
                                                            context: null,
                                                            fnc: function () {

                                                              var nameRecord = this.GroupTagNameBlkJSX.getValue();
                                                              var selNameRecord = this.GroupTagNameBlkJSX.getRecord(nameRecord);
                                                              var groupTagName = "";
                                                              var groupTagValue = "";
                                                              if (selNameRecord == null) {
                                                                groupTagName = nameRecord
                                                              } else {
                                                                groupTagName = selNameRecord.jsxtext;
                                                              }
                                                              if (groupTagName == "" || groupTagName == null) {
                                                                windowAlert(winCollaborationBlkJSX, "Please enter/select tag name");
                                                                return false;
                                                              }

                                                              var valueRecord = this.GroupTagValueBlkJSX.getValue();
                                                              var selValueRecord = this.GroupTagValueBlkJSX.getRecord(valueRecord);
                                                              if (selValueRecord == null) {
                                                                groupTagValue = valueRecord;
                                                              } else {
                                                                groupTagValue = selValueRecord.jsxtext;
                                                              }
                                                              if (groupTagValue == "" || groupTagValue == null) {
                                                                windowAlert(winCollaborationBlkJSX, "Please enter/select tag value");
                                                                return false;
                                                              }

                                                              var o = new Object;
                                                              o.jsxid = jsx3.xml.CDF.getKey();
                                                              o.jsxGroupTagName = groupTagName;
                                                              o.jsxGroupTagValue = groupTagValue;
                                                              this.GroupTagListBlkJSX.insertRecord(o, null, false);
                                                              this.GroupTagNameBlkJSX.setValue(null);
                                                              this.GroupTagValueBlkJSX.setValue(null);
                                                              this.GroupTagListBlkJSX.repaintData();
                                                            }
                                                          });
      this.cmpBtnCollaborationGroupTagAdd.GroupTagNameBlkJSX = getJsxByName([winCollaborationBlkJSX, "cmbCollabGroupTagName"]);
      this.cmpBtnCollaborationGroupTagAdd.GroupTagValueBlkJSX = getJsxByName([winCollaborationBlkJSX, "cmbCollabGroupTagValue"]);
      this.cmpBtnCollaborationGroupTagAdd.GroupTagListBlkJSX = getJsxByName([winCollaborationBlkJSX, "mtxCollabGroupTag"]);

      // groupTag matrix block on loading data it refreshes itself
      this.cmpMtxCollaborationGroupTag = new Component([winCollaborationBlkJSX, "mtxCollabGroupTag"],
                                                     [{ action: "TOPICEVENT", topic: tCollaboration, eventvar: "CollaborationDataUpdate", eventval: null}]
                                                );

      this.cmpMtxCollaborationGroupTag.repaint = function () {
        // get the groupTags
        var groupTagIter = winCollaboration.sessionConfig.getGroupTagIter();
        this.BlkJSX.resetCacheData();
        while (groupTagIter.hasNext()) {
          var tagNode = groupTagIter.next();
          var tagName = tagNode.getAttribute("Name");
          var tagValue = tagNode.getAttribute("Value");
          if ((tagName != "") && (tagValue != "")) {
            var o = new Object;
            o.jsxid = jsx3.xml.CDF.getKey();
            o.jsxGroupTagName = tagName;
            o.jsxGroupTagValue = tagValue;
            this.BlkJSX.insertRecord(o, null, false);
          }
        }
        this.postpaint();
      }


      this.cmpCmbSelCollaborationTagDefValue = new CmpNIDElementList([winCollaborationBlkJSX, "cmbCollabGroupTagValue"],
                                                                    "TagDefNameSelected",
                                                                    ".//TagValueList/TagValue",
                                                                    {
                                                                      jsxtext: { objxpath: "\".\"" }
                                                                    },
                                                                    [{ action: "REQEVENT", eventvar: "GroupTagDataState", eventval: null },
                                                                     { action: "APPEVENT", eventvar: "TagDefNameSelected", eventval: null }
                                                                    ],
                                                                    { persist: { col: "jsxtext", initval: false },
                                                                      action: "TOPICEVENT", topic: tCollaboration, eventvar: "TagDefValueSelected", eventval: "jsxtext"
                                                                    }
                                                                   );

      this.cmpCmbSelCollaborationTagDefName = new CmpNIDList([winCollaborationBlkJSX, "cmbCollabGroupTagName"],
                                                             "GroupTagNIDList",
                                                             null,
                                                             null,
                                                             { jsxid: { objxpath: "\"./@NID\"" },
                                                               jsxtext: { objxpath: "\".//TagName\"" }
                                                             },
                                                             [{ action: "REQEVENT", eventvar: "GroupTagDataState", eventval: null}],
                                                             { persist: { col: "jsxtext", initval: false },
                                                               action: "APPEVENT", eventvar: "TagDefNameSelected", eventval: "jsxid"
                                                             }
                                                          );


      // collab group tags dialog                               
      this.cmpDlgGroupTagSettings = new CmpContainer([winCollaborationBlkJSX, "blkCollabGroupTagSettings"],
                                                   [{ cmp: this.cmpBtnCollaborationGroupTagCancel },
                                                    { cmp: this.cmpBtnCollaborationGroupTagDone },
                                                    { cmp: this.cmpBtnCollaborationGroupTagAdd },
                                                    { cmp: this.cmpMtxCollaborationGroupTag },
                                                    { cmp: this.cmpCmbSelCollaborationTagDefValue },
                                                    { cmp: this.cmpCmbSelCollaborationTagDefName}],
                                                   null,
                                                   null
                                                );
      // GroupTag button
      this.cmpBtnCollaborationGroupTag = new CmpBlockButton([winCollaborationBlkJSX, "blkTagsConfig"],
                                                           null,
                                                           { action: "TOPICEVENT", topic: tCollaboration, eventvar: "CollaborationGroupTagSelected", eventval: "OPEN" });


      this.cmpChkSessionEnableRecord = new CmpCheckBox([winCollaborationBlkJSX, "chkNewSessionEnableRecord"],
                                                    null,
                                                    { action: "FNCCALL",
                                                      context: null,
                                                      fnc: function () {
                                                        if (this.BlkJSX.getChecked()) {
                                                          //winCollaboration.sessionConfig.setRecAutoStart(true);
                                                        }
                                                        else {
                                                          //winCollaboration.sessionConfig.setRecAutoStart(false);
                                                        }
                                                      }
                                                    });

      this.cmpTxtCollaborationName = new CmpTxtBoxWithBlur([winCollaborationBlkJSX, "txtNewSessionCollaborationName"],
                                                             null, null,
                                                             { action: "FNCCALL", context: null,
                                                               fnc: function () {
                                                                 var collaborationName = this.BlkJSX.getValue();
                                                                 winCollaboration.sessionConfig.setName(collaborationName);
                                                                 topic_Publish(tCollaboration, "CollaborationName", collaborationName);
                                                                 //set the resourcetitle in the groupfile with the new collab name
                                                                 var objGrpFile = winCollaboration.sessionConfig.getDefaultRecGrpFile();
                                                                 if (!jsx3.util.strEmpty(objGrpFile)) {
                                                                   objGrpFile.grpFileTitle = collaborationName;
                                                                   winCollaboration.sessionConfig.updateDefaultRecGrpFile(objGrpFile);
                                                                 }
                                                               }
                                                             });

      this.cmpPVRTrimLengthSetting = new CmpPVRTrimLengthContainer([winCollaborationBlkJSX, "blkSessionSetup"],
                                                                  [[winCollaborationBlkJSX, "chkNewSessionEnablePVR"],
                                                                   [winCollaborationBlkJSX, "txtHrPVRTrimLength"],
                                                                   [winCollaborationBlkJSX, "txtPVRTrimLength"]
                                                                  ],
                                                                  null,
                                                                  null,
                                                                  winCollaborationBlkJSX,
                                                                  winCollaboration
                                                                  );

      this.cmpSelSessionPlaybackOption = new CmpStaticList([winCollaborationBlkJSX, "selPlaybackOptions"],
                                                           "xml/selections/PlaybackOption.xml",
                                                           null,
                                                           { action: "FNCCALL", context: null,
                                                             fnc: function () {
                                                               var objBlkJSX = this.BlkJSX;
                                                               var objBlkJSXId = objBlkJSX.getValue();
                                                               if (!jsx3.util.strEmpty(objBlkJSXId)) {
                                                                 winCollaboration.sessionConfig.setRecVersion(objBlkJSXId);
                                                               }
                                                             }
                                                           });

      // collab setup 
      this.cmpCtrSetupCtrl = new CmpContainer([winCollaborationBlkJSX, "blkSetupCtrl"],
                                             [{ cmp: this.cmpSelSessionPlaybackOption },
                                              { cmp: this.cmpPVRTrimLengthSetting },
      //{ cmp: this.cmpChkSessionEnableRecord },
                                              {cmp: this.cmpTxtCollaborationName },
                                              { cmp: this.cmpBtnCollaborationGroupTag },
                                              { cmp: this.cmpDlgGroupTagSettings,
                                                sel: "topic_Get('" + tCollaboration + "', 'CollaborationGroupTagSelected') != 'NONE'"
                                              },
                                              { cmp: this.cmpBtnCollaborationCancel },
                                              { cmp: this.cmpBtnCollaborationSave}],
                                             [{ action: "TOPICEVENT", topic: tCollaboration, eventvar: "CollaborationDataUpdate", eventval: null },
                                              { action: "TOPICEVENT", topic: tCollaboration, eventvar: "CollaborationGroupTagSelected", eventval: null}],
                                            null
                                          );

      // init dialog sel vars
      topic_Set(tCollaboration, "CollaborationGroupTagSelected", "NONE");
      topic_Set(tCollaboration, "CollaborationMode", "RECORD");

      // session media list and connection lists + biz logic

      eventCollaboration.subscribe("OpenMediaList", null, this,
                                  function (EventVar, EventVal) {
                                    if (EventVal == "OPEN") {
                                      var ThisClass = this;
                                      jsx3.sleep((function () {
                                        cmpWinMediaList_Create(tCollaboration, winCollaborationBlkJSX);
                                      }), null, ThisClass);
                                    }
                                  }
                                );

      //this.cmpCtrSessionInfo = cmpBlkSessionInfo_Create.call(this);

      eventCollaboration.subscribe("PlyFileSelected", null, this,
                                  function (EventVar, EventVal) {
                                    var isEnabled = this.cmpSelSessionPlaybackOption.BlkJSX.getEnabled();
                                    var selVal = winCollaboration.sessionConfig.getRecVersion();
                                    if ((!isEnabled) && (jsx3.util.strEmpty(selVal))) {
                                      var defaultVal = "DEFAULT";
                                      this.cmpSelSessionPlaybackOption.BlkJSX.setEnabled(jsx3.gui.Form.STATEENABLED, true);
                                      this.cmpSelSessionPlaybackOption.BlkJSX.setValue(defaultVal);
                                      winCollaboration.sessionConfig.setRecVersion(defaultVal);
                                    }
                                  }
                                );

      eventCollaboration.subscribe("NoPlyFileSelected", null, this,
                                  function (EventVar, EventVal) {
                                    var defaultVal = "DEFAULT";
                                    this.cmpSelSessionPlaybackOption.BlkJSX.setEnabled(jsx3.gui.Form.STATEDISABLED, true);
                                    this.cmpSelSessionPlaybackOption.BlkJSX.setValue(defaultVal);
                                  }
                                );

      eventCollaboration.subscribe("CollaborationDataUpdate", null, this,
                                  function (EventVar, EventVal) {
                                    var collaborationMode = topic_Get(tCollaboration, "CollaborationMode");
                                    var collabName = winCollaboration.sessionConfig.getName();
                                    this.cmpTxtCollaborationName.BlkJSX.setValue(collabName);

                                    topic_Publish(tCollaboration, "CollaborationName", topic_Get(tCollaboration, "CollaborationName"));
                                    // get the dir name
                                    var objDir = winCollaboration.sessionConfig.getDefaultRecDir();

                                    this.cmpPVRTrimLengthSetting.onResponse("SUCCESS", winCollaboration.sessionConfig);

                                    // get the startonRun value
                                    var startOnRun = winCollaboration.sessionConfig.getRecAutoStart();
                                    if (startOnRun == "true") {
                                      //this.cmpChkSessionEnableRecord.BlkJSX.setChecked("1");
                                    } else {
                                      //this.cmpChkSessionEnableRecord.BlkJSX.setChecked("0");
                                    }

                                    this.cmpBtnCollaborationSave.BlkJSX.setEnabled(jsx3.gui.Form.STATEENABLED, true);

                                    var playBackOption = winCollaboration.sessionConfig.getRecVersion();
                                    this.cmpSelSessionPlaybackOption.BlkJSX.setValue(playBackOption);

                                    // find if any src entity is the media file if yes then enable the drop down else disable it.
                                    var plyFileIter = winCollaboration.sessionConfig.getEntityIterByType("PlyFile");
                                    if (plyFileIter.hasNext()) {
                                      this.cmpSelSessionPlaybackOption.BlkJSX.setEnabled(jsx3.gui.Form.STATEENABLED, true);
                                    }
                                    else {
                                      this.cmpSelSessionPlaybackOption.BlkJSX.setEnabled(jsx3.gui.Form.STATEDISABLED, true);
                                    }
                                  }
                                );

      // -- METHOD to add GroupTags to the collab config XML
      this.updateGroupTag = function (mtxGroupTagBlkJSX) {
        // get vars from object instance
        var winCollaboration = this;
        var winCollaborationBlkJSX = this.BlkJSX;
        var tCollaboration = this.tWin;
        var eventCollaboration = this.eventWin;

        var strXMLCacheId = mtxGroupTagBlkJSX.getXMLId();
        var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
        if (objXML) {
          var objRecordNodes = objXML.selectNodes("//record");
          for (var it = objRecordNodes.iterator(); it.hasNext(); ) {
            var objRecordNode = it.next();
            var tagName = objRecordNode.getAttribute("jsxGroupTagName");
            var tagValue = objRecordNode.getAttribute("jsxGroupTagValue");

            winCollaboration.sessionConfig.addGroupTag(tagName, tagValue);
          }
        }
      } // end winCollaboration.updateGroupTag  

      //-- METHOD to validate the pvr trim length
      this.validatePVRTrimLength = function () {
        if (this.cmpPVRTrimLengthSetting.chkEnablePVR.BlkJSX.getChecked()) {
          var trimLength = this.cmpPVRTrimLengthSetting.validatePVRTrimLength();
          if (trimLength != false) {
            winCollaboration.sessionConfig.setPVRTrimLength(trimLength);
          }
          else {
            return false;
          }
        }
        return true;
      } // end validatePVRTrimLength

      this.cmpLblStatus = new CmpLabelTxtStream([winCollaborationBlkJSX, "lblStatus"], null,
                                                 [{ action: "TOPICEVENT", topic: tCollaboration, eventvar: "CollaborationEventMsg", eventval: null}],
                                                 null,
                                                 "\nCollaboration Event Messages ...",
                                                 "JSXAPPS/IPVS_ClientUI/components/dialogEventLog.xml",
                                                 "blkEventLog");

      this.cmpLblWindowName = new CmpLabel(([winCollaborationBlkJSX, "lblWindowName"]),
                                             null,
                                             [{ action: "TOPICEVENT", topic: tCollaboration, eventvar: "CollaborationName", eventval: null}]);

      // set the window customize image if any
      this.cmpClientWindowHeaderLogo = new Component([winCollaborationBlkJSX, "clientWindowHeaderLogo"], null, null, null);
      this.cmpClientWindowHeaderLogo.customizeSkin = function () {
        var clientWindowHeaderLogo = clsCustomizeSkin.getAttrValue("IPVSClient", "clientWindowHeaderLogo");
        if (!jsx3.util.strEmpty(clientWindowHeaderLogo)) {
          this.BlkJSX.setSrc(clientWindowHeaderLogo);
          this.BlkJSX.repaint();
        }
      }

      // sub component array
      this.SubCmpArr = [{ cmp: this.cmpClientWindowHeaderLogo },
      //{ cmp: this.cmpCtrSessionInfo },
                        {cmp: this.cmpCtrSetupCtrl },
                        { cmp: this.cmpLblWindowName },
                        { cmp: this.cmpLblStatus }
                     ];



      // init and repaint happens after the window is loaded
      var ThisClass = this;
      jsx3.sleep((function () {
        if (winCollaborationBlkJSX._jsxopener) {
          if (topic_Get(tCollaboration, "SessionConfigNID") != "NONE") {
            winCollaboration.BlkJSX._jsxopener.parent.blur();
            winCollaboration.BlkJSX._jsxopener.parent.hideFocus = false;
            if (winBrowseSessionsBlkJSX != null) {
              winBrowseSessionsBlkJSX._jsxwindow.blur();
              winBrowseSessionsBlkJSX._jsxwindow.hideFocus = false;
            }
          }
        } else if (winCollaborationBlkJSX._jsxpB) {
          if (topic_Get(tCollaboration, "SessionConfigNID") != "NONE") {
            winCollaboration.BlkJSX._jsxpB.parent.blur();
            winCollaboration.BlkJSX._jsxpB.parent.hideFocus = false;
            if (winBrowseSessionsBlkJSX != null) {
              winBrowseSessionsBlkJSX._jsxwindow.blur();
              winBrowseSessionsBlkJSX._jsxwindow.hideFocus = false;
            }
          }
        }
        ThisClass.init();
        ThisClass.show();
        cmpBlkSessionInfo_Create.call(ThisClass, winCollaboration);

        winCollaboration.BlkJSX._jsxwindow.focus();
      }), null, this);
    }

    // open the window - which will call createSub - which will init and show
    winCollaboration.open();
    //winCollaboration.BlkJSX._jsxwindow.focus();
}
