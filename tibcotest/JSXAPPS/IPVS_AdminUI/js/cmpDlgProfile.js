// this code has some potential issues 
// - w.r.t object reference counts - when objects created in here go out of scope etc
// - loading sync issues - dlg may not be loaded before the object init methods are called

function dlgEditProfile_Create(ProfileNID) {

  // deal with defaults for add
  if( ProfileNID == null ) ProfileNID = "DEFAULT_V2D-STREAMPROFILE_NID";

  // load view - dialog component file
  var DlgFile = "components/assets/addProfileDialog.xml";
  var BlkJSX = PNameSpace.getBodyBlock().load(DlgFile);
  
  var tDlg = "Dialog_" + jsx3.xml.CDF.getKey()
  topic_Create(tDlg);
  
  // setup a listner for it    
  var eventDlg = new EventListner(tDlg);
  var orgProfilePriority = "";
  var firstRequest = true;
  
  var dlgText = "Add Profile";
  if( ProfileNID != "DEFAULT_V2D-STREAMPROFILE_NID" ){
    dlgText = "Edit Profile";
  }
  var windowBarBlkJSX = getJsxByName([BlkJSX, "windowBar"]);
  windowBarBlkJSX.setText(dlgText, true);
  closeDialogIfExists(dlgText);

  function dlgEditProfile_Destroy() {
    // destroy any local eventlistners
    eventDlg.destroy();
    
    // destroy all local components
    cmpDlgEditProfile.destroy();
    
    // destroy the local topic      
    topic_Destroy(tDlg);
    
    // close dialog     
    BlkJSX.doClose();
  }
                    
  // load control logic class instances   
  var cmpBlkEditProfileConfig = cmpBlkProfileStreamType_Create(BlkJSX, tDlg);

  var cmpBlkEditProfileGroupMatrix = new CmpBlock([BlkJSX,"mtxProfileTags"]);
  cmpBlkEditProfileGroupMatrix.BlkJSX.setXMLTransformers("xsl/TagToCDFTag.xsl");
  cmpBlkEditProfileGroupMatrix.BlkJSX.setSelectionModel(jsx3.gui.Matrix.SELECTION_ROW);
  
  var cmpCDFEditProfile = new Component([BlkJSX,"cdfDlgEditProfile"]);
  cmpCDFEditProfile.BlkJSX.setXMLTransformers("xsl/EditV2DProfile.xsl");

  var cmpCmbSelEditProfileTagDefName = new CmpCustomPaginationList([BlkJSX, "cmbProfileAddTagName"],
                                                           [{action:"TOPICEVENT", topic: tDlg, eventvar:"ProfileTagDefNameState", eventval:null}],
                                                             { persist: { col: "jsxtext", initval: false },
                                                                 action: "TOPICEVENT", topic: tDlg, eventvar: "ProfileTagDefNameSelected", eventval: "jsxid"
                                                             },
                                                          null,
                                                          null,
                                                          "xsl/TagNameList.xsl"
                                                          );

  cmpCmbSelEditProfileTagDefName.cmpReqParamArr = { StartFrom: "ObjectList_StartFrom",
      CountToFetch: "ObjectList_CountToFetch",
      SortOrder: "ObjectList_SortOrder",
      ObjectCount: "ObjectCount",
      SortByXpath: "ObjectList_SortBy",
      WhereObjectType: "ObjectList_WhereObjectType"
  };

  topic_Set(tDlg, "ProfileTagNameResponseSelectFormat", "NameValueTree");
  topic_Set(tDlg, "ProfileTagNameResponseObjectType", "Profiles");

  cmpCmbSelEditProfileTagDefName.cmpReq = new cmpReq_GetSessionAPIPaginationList(tDlg, "GetTags",
                                                                        [{ VarName: "ObjectList_SelectFormat", VarRef: "ProfileTagNameResponseSelectFormat"},
                                                                         { VarName: "ObjectList_WhereObjectType", VarRef: "ProfileTagNameResponseObjectType"}],
                                                                        { action: "FNCCALL", context: cmpCmbSelEditProfileTagDefName,
                                                                            fnc: function(respCode, respVal) {
                                                                                if (respCode == "SUCCESS" && respVal != null) {
                                                                                    this.onResponse(respCode, respVal);
                                                                                }
                                                                                else {
                                                                                    log.info("GetTagsRequest failed. Error Code :" + respCode);
                                                                                    this.onResponseErrorHandle("GetTags",respCode, respVal);
                                                                                }
                                                                            }
                                                                        }
                                                                      );

  var cmpCmbSelEditProfileTagDefValue = new Component([BlkJSX, "cmbProfileAddTagValue"]);
  cmpCmbSelEditProfileTagDefValue.BlkJSX.setXMLTransformers("xsl/TagValueList.xsl");

  eventDlg.subscribe("ProfileTagDefNameSelected", null, this,
                      function(eventVar, eventVal) {
                        if (eventVal != "NONE") {
                          cmpCmbSelEditProfileTagDefValue.BlkJSX.setValue("");
                          var nameRecord = cmpCmbSelEditProfileTagDefName.BlkJSX.getValue();
                          var selNameRecord = cmpCmbSelEditProfileTagDefName.BlkJSX.getRecordNode(nameRecord);
                          var tagNode = selNameRecord.selectSingleNode(".//Tag");
                          var strXMLValueCacheId = cmpCmbSelEditProfileTagDefValue.BlkJSX.getXMLId();
                          var objXML = PNameSpace.getCache().getDocument(strXMLValueCacheId);
                          objXML.loadXML(tagNode.toString());
                          cmpCmbSelEditProfileTagDefValue.BlkJSX.setSourceXML(objXML); 
                        }
                      }
                    );
                                     
  var cmpBtnEditProfileAddGroup = new CmpButton([BlkJSX,"btnProfileAddTag"],
                                                  null,
                                                  { action:"FNCCALL", 
                                                    context:null,
                                                    fnc:function() {
                                                      var nameRecord = this.GroupTagNameBlkJSX.getValue();
                                                      var selNameRecord = this.GroupTagNameBlkJSX.getRecord(nameRecord);
                                                      var groupTagName = "";
                                                      var groupTagValue = "";
                                                      if( selNameRecord == null) {
                                                        groupTagName = nameRecord
                                                      } else {
                                                        groupTagName = selNameRecord.jsxtext;
                                                      }
                                                      if( groupTagName == "" || groupTagName == null ) {
                                                        alert("Please enter/select tag name");
                                                        return false;                                          
                                                      }
                                                                                                    
                                                      var valueRecord = this.GroupTagValueBlkJSX.getValue();
                                                      var selValueRecord = this.GroupTagValueBlkJSX.getRecord(valueRecord);
                                                      if( selValueRecord == null) {
                                                        groupTagValue = valueRecord;
                                                      } else {
                                                        groupTagValue = selValueRecord.jsxtext;
                                                      } 
                                                      if( groupTagValue == "" || groupTagValue == null) {
                                                        alert("Please enter/select tag value");
                                                        return false;
                                                      }                                        
                                                    
                                                      var o = new Object;
                                                      o.jsxid = groupTagName + groupTagValue;
                                                      o.jsxTagName = groupTagName;
                                                      o.jsxTagValue = groupTagValue;
                                                      this.GroupTagListBlkJSX.insertRecord(o, null, false);
                                                      this.GroupTagNameBlkJSX.setValue(null);
                                                      this.GroupTagValueBlkJSX.setValue(null);
                                                      this.GroupTagListBlkJSX.repaintData();
                                                    }
                                                  });
                                                  
  cmpBtnEditProfileAddGroup.GroupTagNameBlkJSX = cmpCmbSelEditProfileTagDefName.BlkJSX;
  cmpBtnEditProfileAddGroup.GroupTagValueBlkJSX = cmpCmbSelEditProfileTagDefValue.BlkJSX;
  cmpBtnEditProfileAddGroup.GroupTagListBlkJSX = cmpBlkEditProfileGroupMatrix.BlkJSX;
       
  var cmpSelEditProfileProfileTypeList = new CmpStaticList([BlkJSX,"selProfileType"],
                                                            "xml/selections/ProfileType.xml",  
                                                            null,
                                                            {action:"FNCCALL", 
                                                             context:null, 
                                                             fnc:function() {
                                                                var SelProfileType = this.selRecord.jsxid;
                                                                
                                                                // check what was the previous selection If same then dont get the data or dont repaint
                                                                var ProfileType = topic_Get(tDlg, "EditProfileProfileTypeSelection");
                                                                if(ProfileType == SelProfileType) {
                                                                  firstRequest = true;
                                                                } else {
                                                                  firstRequest = false;
                                                                }
                                                                // basically checking for add or edit.
                                                                // If add then checking what type is selected and set the default nid and repaint
                                                                var profileNIDsearch = ProfileNID.search(/DEFAULT/);
                                                                if( profileNIDsearch != -1 ) {                    
                                                                  switch(SelProfileType) {
                                                                    case "V2D":
                                                                      ProfileNID = "DEFAULT_V2D-STREAMPROFILE_NID";
                                                                      break;
                                                                    case "MPEGTS":
                                                                      ProfileNID = "DEFAULT_MPEGTS-STREAMPROFILE_NID";
                                                                      break;
                                                                    case "RTP":
                                                                      ProfileNID = "DEFAULT_RTP-STREAMPROFILE_NID";
                                                                      break;
                                                                    case "UDP":
                                                                      ProfileNID = "DEFAULT_UDP-STREAMPROFILE_NID";
                                                                      break;
                                                                    case "RTP-RAW":
                                                                      ProfileNID = "DEFAULT_RTP-RAW-STREAMPROFILE_NID";
                                                                      break;
                                                                    case "RTP-PCM":
                                                                      ProfileNID = "DEFAULT_RTP-PCM-STREAMPROFILE_NID";
                                                                      break;
                                                                    case "RFB":
                                                                      ProfileNID = "DEFAULT_RFB-STREAMPROFILE_NID";
                                                                      break;
                                                                    default:
                                                                      alert( "InternalError: Get Profile: Unknown Profile Type: " + ProfileType + "\n" );
                                                                      return;
                                                                  }
                                                                  if(!firstRequest) {
                                                                    cmpDlgEditProfileGetProfile(ProfileNID);
                                                                  }                                                                     
                                                                } 
                                                             }
                                                            });
  
  var cmpTxtProfileName = new Component([BlkJSX,"txtProfileName"]);

  var cmpTxtProfilePriority = new Component([BlkJSX,"txtProfilePriority"]);

  var cmpBtnEditProfileClose = new CmpButton([BlkJSX,"btnDialogCloseX"],
                                               null, 
                                               {action:"FNCCALL", 
                                                context:null, 
                                                fnc:function() {
                                                  dlgEditProfile_Destroy();
                                                }
                                               });
        
  var cmpBtnEditProfileCancel = new CmpButton([BlkJSX,"btnCancelAddProfile"],
                                               null, 
                                               {action:"FNCCALL", 
                                                context:null, 
                                                fnc:function() {
                                                  dlgEditProfile_Destroy();
                                                }
                                               });
        
  var cmpBtnEditProfileOK = new CmpButton([BlkJSX,"btnAddProfile"],
                                           null, 
                                           {action:"FNCCALL", 
                                            context:null, 
                                            fnc:function() {
                                              cmpDlgEditProfileSetProfile.call(this, ProfileNID);
                                            }
                                           });
  var cmpTabProfileInfo = new CmpTab([BlkJSX,"tabProfileInfo"],
                                         null,
                                         {action:"TOPICEVENT", topic:tDlg, eventvar:"ProfileTabSelected", eventval:"INFO"} 
                                        );
  var cmpTabProfileConfig = new CmpTab([BlkJSX,"tabProfileConfig"],
                                         null,
                                         {action:"TOPICEVENT", topic:tDlg, eventvar:"ProfileTabSelected", eventval:"CONFIG"} 
                                        );  

  var cmpBlkProfileUIInfo = new CmpContainer([BlkJSX,"blkProfileInfo"],
                                             [{cmp:cmpSelEditProfileProfileTypeList},
                                             {cmp:cmpTxtProfileName},
                                             {cmp:cmpTxtProfilePriority},
                                             {cmp:cmpCmbSelEditProfileTagDefName},
                                             {cmp:cmpCmbSelEditProfileTagDefValue},
                                             {cmp:cmpBtnEditProfileAddGroup},
                                             {cmp:cmpBlkEditProfileGroupMatrix}],
                                             null);
                                             
  var cmpBlkProfileUIConfig = new CmpContainer([BlkJSX,"blkProfileConfig"],
                                               [{cmp:cmpBlkEditProfileConfig}],
                                               null);
  
  var cmpDlgProfileUI = new CmpContainer( [BlkJSX,"blkInfoConfigProfile"],
                                          [{cmp:cmpBlkProfileUIInfo,
                                            sel:"topic_Get(\"" + tDlg + "\", 'ProfileTabSelected') == 'INFO' || topic_Get(\"" + tDlg + "\", 'ProfileTabSelected') == null "},
                                           {cmp:cmpBlkProfileUIConfig,
                                            sel:"topic_Get(\"" + tDlg + "\", 'ProfileTabSelected') == 'CONFIG'"}
                                           ],
                                           [{action:"TOPICEVENT", topic:tDlg, eventvar:"ProfileTabSelected", eventval:null}] 
                                        );                                                                                 
                                        
  var cmpDlgTab = new CmpContainer([BlkJSX, "DialogProfile"], 
                                    [ {cmp:cmpTabProfileConfig,
                                       sel:"topic_Get(\"" + tDlg + "\", 'EditStreamTypeConfigSelection') == 'V2DStreamConfig' || topic_Get(\"" 
                                       + tDlg + "\", 'EditStreamTypeConfigSelection') == 'V2DEncoderAudio' || topic_Get(\"" 
                                       + tDlg + "\", 'EditStreamTypeConfigSelection') == 'VideoCompression'|| topic_Get(\""
                                       + tDlg + "\", 'EditStreamTypeConfigSelection') == 'RTPProfile' || topic_Get(\""
                                       + tDlg + "\", 'EditStreamTypeConfigSelection') == 'KeyboardMouse'"}, 
                                      {cmp:cmpTabProfileInfo}
                                    ],
                                    [{action:"TOPICEVENT", topic:tDlg, eventvar:"EditStreamTypeConfigSelection", eventval:null}] 
                                   );
                                   
  var cmpDlgEditProfile = new CmpContainer([BlkJSX,"DialogProfile"],
                                            [ {cmp:cmpDlgProfileUI},
                                              {cmp:cmpDlgTab},
                                              {cmp:cmpBtnEditProfileClose},
                                              {cmp:cmpBtnEditProfileCancel},
                                              {cmp:cmpBtnEditProfileOK},
                                              {cmp:cmpCDFEditProfile}],
                                            null);                            
  
  cmpDlgEditProfile.BlkJSX = BlkJSX;
  cmpDlgEditProfile.ObjectNID = ProfileNID; 
  cmpDlgEditProfile.getBlkData = cmpReq_GetCompleteProfileWithReadQuery;
  cmpDlgEditProfile.tDlg = tDlg;

  // init and repaint happens after the dialog is loaded
  jsx3.sleep( (function(){
                 this.init();
                 this.getBlkData(this.tDlg, this.ObjectNID);
                 topic_Publish(tDlg, "ProfileTagDefNameState", (newServerDate()).getTime());
                 }
               ),
               null, cmpDlgEditProfile );        

  // event published from func ( cmpReq_GetCompleteProfileWithReadQuery ) defined in cmpReq.js                  
  eventDlg.subscribe("GetCompleteProfileDone", ProfileNID, this,
                      function() {
                        cmpDlgEditProfileGetProfile(ProfileNID);
                      }
                    ); // subscribe end 

    // fnc to get dlg data
    function cmpDlgEditProfileGetProfile(ProfileNID) { 
      var reqStatus = topic_Get(tDlg, "GetCompleteProfileStatus");
      if(reqStatus != "SUCCESS") {
        dlgEditProfile_Destroy();  
      }
      var profileNode = null;
      if( ProfileNID == "DEFAULT_V2D-STREAMPROFILE_NID" || ProfileNID == "DEFAULT_MPEGTS-STREAMPROFILE_NID" || ProfileNID == "DEFAULT_RTP-STREAMPROFILE_NID" || ProfileNID == "DEFAULT_UDP-STREAMPROFILE_NID" || 
          ProfileNID == "DEFAULT_RTP-RAW-STREAMPROFILE_NID" || ProfileNID == "DEFAULT_RTP-PCM-STREAMPROFILE_NID" || ProfileNID == "DEFAULT_RFB-STREAMPROFILE_NID"){
        profileNode = topic_GetNode(tCache, "//*[@NID='" + ProfileNID + "']");
      }
      else{
        profileNode = getDialogStreamProfileDataNode(tDlg, ProfileNID);
      }
      var ProfileType = profileNode.selectSingleNode("./StreamType").getValue();
      // this publish is for the config tab so that label can be high lighted
      topic_Publish(tDlg, "EditProfileProfileTypeSelection1", ProfileType);
      // this for the other purposes
      topic_Publish(tDlg, "EditProfileProfileTypeSelection", ProfileType);
      switch(ProfileType) {
        case "V2D":
          cmpCDFEditProfile.BlkJSX.setXMLTransformers("xsl/EditV2DProfile.xsl");
          topic_Publish(tDlg ,"EditStreamTypeConfigSelection","V2DStreamConfig");
          break;
        case "MPEGTS":
          cmpCDFEditProfile.BlkJSX.setXMLTransformers("xsl/EditMPEGProfile.xsl");
          topic_Publish(tDlg ,"EditStreamTypeConfigSelection","MPEGTSProfile");
          break;
        case "RTP":
          cmpCDFEditProfile.BlkJSX.setXMLTransformers("xsl/EditRTPProfile.xsl");
          topic_Publish(tDlg ,"EditStreamTypeConfigSelection","RTPProfile");
          break;
        case "UDP":
          cmpCDFEditProfile.BlkJSX.setXMLTransformers("xsl/EditUDPProfile.xsl");
          topic_Publish(tDlg ,"EditStreamTypeConfigSelection","UDPProfile");
          break; 
        case "RTP-RAW":
          cmpCDFEditProfile.BlkJSX.setXMLTransformers("xsl/EditRTPRAWProfile.xsl");
          topic_Publish(tDlg ,"EditStreamTypeConfigSelection","RTPRAWProfile");
          break;
        case "RTP-PCM":
          cmpCDFEditProfile.BlkJSX.setXMLTransformers("xsl/EditRTPPCMProfile.xsl");
          topic_Publish(tDlg ,"EditStreamTypeConfigSelection","RTPPCMProfile");
          break;
        case "RFB":
          cmpCDFEditProfile.BlkJSX.setXMLTransformers("xsl/EditRFBProfile.xsl");
          topic_Publish(tDlg ,"EditStreamTypeConfigSelection","RFBProfile");
          break; 
        default:
          alert( "InternalError: Get Profile: Unknown Profile Type: " + ProfileType + "\n" );
          return;
      }
      
      // deleting empty group tag name and value from the default xml
      deleteEmptyGroupTag(profileNode);        

      var groupNode = profileNode.selectSingleNode(".//Groups").cloneNode(true); 
      // set the Tags data in the matrix
      var strXMLCacheId = cmpBlkEditProfileGroupMatrix.BlkJSX.getXMLId();
      var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
      objXML.loadXML(groupNode.toString());
      cmpBlkEditProfileGroupMatrix.BlkJSX.setSourceXML(objXML);        
      cmpBlkEditProfileGroupMatrix.BlkJSX.repaint();
      
      // set the profile data
      var strXMLCacheId = cmpCDFEditProfile.BlkJSX.getXMLId();
      var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
      objXML.loadXML(profileNode.toString());
      cmpCDFEditProfile.BlkJSX.setSourceXML(objXML);         
      cmpCDFEditProfile.BlkJSX.setCDFId(ProfileNID);   
      
      topic_Publish(tDlg, "EditProfileDataState", ProfileNID);
      firstRequest = false;
    
      var profileNIDsearch = ProfileNID.search(/DEFAULT/);
      if( profileNIDsearch != -1 ) {
      } else {
        var cmpSelProfileTypeBlkJSX = cmpSelEditProfileProfileTypeList.BlkJSX;
        var cmpTxtProfileNameBlkJSX = cmpTxtProfileName.BlkJSX;
        cmpSelProfileTypeBlkJSX.setEnabled(jsx3.gui.Form.STATEDISABLED, true);
        cmpTxtProfileNameBlkJSX.setEnabled(jsx3.gui.Form.STATEDISABLED, true);
                    
      }
      orgProfilePriority = cmpTxtProfilePriority.BlkJSX.getValue();
    }
    
    //basic validation for name, priority and duplicate
    function cmpDlgEditProfileSetProfileValidate() {
      var profileName = cmpTxtProfileName.BlkJSX.getValue();
      var profilePriority = cmpTxtProfilePriority.BlkJSX.getValue();
      var chkProfileName = checkValue(profileName , [BlkJSX,"txtProfileName"] , "Please enter profile name");
      var chkDuplicateName = "";
      var chkProfilePriority = "";
      var chkDuplicatePriority = "";
      
      var profileNIDsearch =  ProfileNID.search(/DEFAULT/);
      if( profileNIDsearch != -1 ) {
        if(chkProfileName) {
          chkProfilePriority = checkIntergerValue(profilePriority , [BlkJSX,"txtProfilePriority"] , "Please enter integer profile priority"); 
        }
        if(chkProfilePriority) {
          chkDuplicatePriority = chkForOrderExistence(profilePriority, [BlkJSX,"txtProfilePriority"], "mtxProfileList", "Duplicate Profile priority. Please enter some other Profile priority");  
        }
        if(chkDuplicatePriority) {
          return true;
        }        
      } else {
        chkProfilePriority = checkIntergerValue(profilePriority , [BlkJSX,"txtProfilePriority"] , "Please enter integer profile priority"); 
        if(chkProfilePriority) {
          if( orgProfilePriority != profilePriority) {
            chkDuplicatePriority = chkForOrderExistence(profilePriority, [BlkJSX,"txtProfilePriority"], "mtxProfileList", "Duplicate Profile priority. Please enter some other Profile priority");    
          } else {
            return true;
          }
        }
        if(chkDuplicatePriority) {
          return true;
        }
      }
      return false;
    }
   
    // fnc to set dlg data - runs in the context of the OK button
    function cmpDlgEditProfileSetProfile(ProfileNID) {
    
      if( cmpDlgEditProfileSetProfileValidate() != true ) return;
      
      // decide add or update
      var SetMsg = "Edit Profile";
      var SetMode = "Replace";
      var profileNIDSearch = ProfileNID.search(/DEFAULT/);
      if( profileNIDSearch != -1 ) {
        SetMsg = "Add Profile";
        SetMode = "Add";
      }

      // write the data back to the CDF
      cmpCDFEditProfile.BlkJSX.write();

      //get the profile data
      var strXMLCacheId = cmpCDFEditProfile.BlkJSX.getXMLId();
      var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);      

      var xslfile = "";
      var ProfileType = topic_Get(tDlg, "EditProfileProfileTypeSelection");
      // setup resource map rule info
      switch(ProfileType) {
        case "V2D":
          xslfile = PNameSpace.loadInclude("jsxuser:///JSXAPPS/IPVS_AdminUI/xsl/EditV2DProfileRequest.xsl","V2DProfileRequest","xsl");
          break;
        case "MPEGTS":
          xslfile = PNameSpace.loadInclude("jsxuser:///JSXAPPS/IPVS_AdminUI/xsl/EditMPEGProfileRequest.xsl","MPEGProfileRequest","xsl");
          break;        
        case "RTP":
          xslfile = PNameSpace.loadInclude("jsxuser:///JSXAPPS/IPVS_AdminUI/xsl/EditRTPProfileRequest.xsl","RTPProfileRequest","xsl");
          break;        
        case "UDP":
          xslfile = PNameSpace.loadInclude("jsxuser:///JSXAPPS/IPVS_AdminUI/xsl/EditUDPProfileRequest.xsl","UDPProfileRequest","xsl");
          break; 
        case "RTP-RAW":
          xslfile = PNameSpace.loadInclude("jsxuser:///JSXAPPS/IPVS_AdminUI/xsl/EditRTPRAWProfileRequest.xsl","RTPRAWProfileRequest","xsl");
          break;        
        case "RTP-PCM":
          xslfile = PNameSpace.loadInclude("jsxuser:///JSXAPPS/IPVS_AdminUI/xsl/EditRTPPCMProfileRequest.xsl","RTPPCMProfileRequest","xsl");
          break;        
        case "RFB":
          xslfile = PNameSpace.loadInclude("jsxuser:///JSXAPPS/IPVS_AdminUI/xsl/EditRFBProfileRequest.xsl","RFBProfileRequest","xsl");
          break;        
        default:
          alert( "InternalError: Set Profile: Unknown Profile Type: " + ProfileType + "\n" );
          return;
      }

      // genrate the request xml
      var xslDoc = jsx3.xml.XslDocument.wrap(xslfile);
      var profileXML = xslDoc.transform(objXML);
      var profileNode = new jsx3.xml.Document();
      profileNode.loadXML(profileXML);
      
      //get the groups
      var strXMLCacheId = cmpBlkEditProfileGroupMatrix.BlkJSX.getXMLId();
      var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
      //convert the matrix data to Tag nodes
      var xslfile = PNameSpace.loadInclude("jsxuser:///JSXAPPS/IPVS_AdminUI/xsl/CDFTagToTag.xsl","TagNode","xsl");
      var xslDoc = jsx3.xml.XslDocument.wrap(xslfile);
      var newGroupsXML = xslDoc.transform(objXML);        
      var newGroupsNode = new jsx3.xml.Document();
      newGroupsNode.loadXML(newGroupsXML);
      
      // replace the groups in the profileNode
      var oldGroupsNode = profileNode.selectSingleNode(".//Groups");              
      var groupParentNode = profileNode.selectSingleNode(".//Groups").getParent();
      groupParentNode.replaceNode(newGroupsNode,oldGroupsNode);

      // set NID in profile node if replace mode
      if( SetMode == "Replace" ) {
        profileNode.setAttribute("NID", ProfileNID);
      }
      
      //deletes empty group tag node
      deleteEmptyGroupTag(profileNode);
      // do set request for Profile
      var reqSetProfileObject = new ReqSetObject(SetMode, "MediaStreamProfile", profileNode, 
                                                  {action:"FNCCALL", context:this, 
                                                     fnc:function(respCode, respVal) {
                                                           if(respCode == "SUCCESS" && respVal != null) {
                                                             topic_Publish(tApp, "ProfileDataUpdate", "ADD");
                                                             dlgEditProfile_Destroy();
                                                           } 
                                                           else if(respCode == "5206" ) {
                                                             alert("Duplicate Profile name. Please enter some other Profile name");
                                                             return false;
                                                           }
                                                           else {
                                                             try{
                                                               var codeErrorMsg = CommonErrorCodeArr[respCode].errorDesc;
                                                             } catch(e){
                                                             }
                                                             var ErrorMsg = "Error: " + SetMsg +" request failed \n";
                                                             ErrorMsg += "Error description: " + codeErrorMsg + "\n";
                                                             ErrorMsg += "Error code: " + respCode;
                                                             alert(ErrorMsg);                                                           
                                                           }
                                                         }
                                                  }                                                 
                                                );
      reqSetProfileObject.sendRequest();
    } 

/*
  // init the logic
  cmpDlgEditProfile.init();
  
  // load the data - no need for show - should fire repaint on data load complete event
  cmpDlgEditProfileGetProfile(ProfileNID);
  */ 

}

function cmpBlkProfileStreamType_Create(ProfileBlkName, tDlg) {

  var cmpBlkV2DStreamConfig = new Component( [ProfileBlkName,"blkAssetV2DTxStreamConfig"] );
  var cmpBlkV2DEncoderAudio = new Component( [ProfileBlkName,"blkAssetV2DTxEncoderAudio"] );
  var cmpBlkVideoCompression = new Component( [ProfileBlkName,"blkAssetV2DTxVideoCompression"] );
  //var cmpBlkKeyboardMouse = new Component( [ProfileBlkName,"blkAssetV2DTxKeyboardMouse"] );
  var cmpBlkMPEGTSConfig = new Component( [ProfileBlkName,"blkAssetMPEGTSProfile"] );
  var cmpBlkRTPConfig = new Component( [ProfileBlkName,"blkAssetRTPProfile"] );
  var cmpBlkUDPConfig = new Component( [ProfileBlkName,"blkAssetUDPProfile"] );
  var cmpBlkRTPRAWConfig = new Component( [ProfileBlkName,"blkAssetRTPRAWProfile"] );
  var cmpBlkRTPPCMConfig = new Component( [ProfileBlkName,"blkAssetRTPPCMProfile"] );
  var cmpBlkRFBConfig = new Component( [ProfileBlkName,"blkAssetRFBProfile"] );

  var cmpBlkProfileStreamTypeUI = new CmpContainer( [ProfileBlkName,"paneProfileBasicUI"],
                                                     [ {cmp:cmpBlkV2DStreamConfig,
                                                        sel:"topic_Get(\"" + tDlg + "\", 'EditStreamTypeConfigSelection') == 'V2DStreamConfig' || topic_Get(\"" + tDlg + "\", 'EditStreamTypeConfigSelection') == null "},
                                                       {cmp:cmpBlkV2DEncoderAudio,
                                                        sel:"topic_Get(\"" + tDlg + "\", 'EditStreamTypeConfigSelection') == 'V2DEncoderAudio'"},
                                                       {cmp:cmpBlkVideoCompression,
                                                        sel:"topic_Get(\"" + tDlg + "\", 'EditStreamTypeConfigSelection') == 'VideoCompression'"},
                                                       //{cmp:cmpBlkKeyboardMouse,
                                                       // sel:"topic_Get(\"" + tDlg + "\", 'EditStreamTypeConfigSelection') == 'KeyboardMouse'"},
                                                       {cmp:cmpBlkMPEGTSConfig,
                                                        sel:"topic_Get(\"" + tDlg + "\", 'EditStreamTypeConfigSelection') == 'MPEGTSProfile'"},
                                                       {cmp:cmpBlkRTPConfig,
                                                        sel:"topic_Get(\"" + tDlg + "\", 'EditStreamTypeConfigSelection') == 'RTPProfile'"},
                                                       {cmp:cmpBlkUDPConfig,
                                                        sel:"topic_Get(\"" + tDlg + "\", 'EditStreamTypeConfigSelection') == 'UDPProfile'"},
                                                       {cmp:cmpBlkRTPRAWConfig,
                                                        sel:"topic_Get(\"" + tDlg + "\", 'EditStreamTypeConfigSelection') == 'RTPRAWProfile'"},
                                                       {cmp:cmpBlkRTPPCMConfig,
                                                        sel:"topic_Get(\"" + tDlg + "\", 'EditStreamTypeConfigSelection') == 'RTPPCMProfile'"},
                                                       {cmp:cmpBlkRFBConfig,
                                                        sel:"topic_Get(\"" + tDlg + "\", 'EditStreamTypeConfigSelection') == 'RFBProfile'"}],
                                                     [{action:"TOPICEVENT", topic:tDlg, eventvar:"EditStreamTypeConfigSelection", eventval:null}]
                                                   );

  var cmpBlkLblV2DStreamConfig = new Component( [ProfileBlkName,"blklblV2DStreamConfig"] );
  var cmpBlkLblV2DEncoderAudio = new Component( [ProfileBlkName,"blklblV2DEncoderAudio"] );
  var cmpBlkLblVideoCompression = new Component( [ProfileBlkName,"blklblV2DVideoCompression"] );
  //var cmpBlkLblKeyboardMouse = new Component( [ProfileBlkName,"blklblV2DKeyboardMouse"] );
  var cmpBlkLblMPEGTS = new Component( [ProfileBlkName,"blklblMPEGTSProfile"] );
  var cmpBlkLblRTP = new Component( [ProfileBlkName,"blklblRTPProfile"] );
  var cmpBlkLblUDP = new Component( [ProfileBlkName,"blklblUDPProfile"] );
  var cmpBlkLblRTPRAW = new Component( [ProfileBlkName,"blklblRTPRAWProfile"] );
  var cmpBlkLblRTPPCM = new Component( [ProfileBlkName,"blklblRTPPCMProfile"] );
  var cmpBlkLblRFB = new Component( [ProfileBlkName,"blklblRFBProfile"] );

                                                      
  var cmpBlkProfileLabelBlk = new CmpContainer( [ProfileBlkName,"blkLabelStreamConfigMenu"],
                                                 [ {cmp:cmpBlkLblV2DStreamConfig,
                                                    sel:"topic_Get(\"" + tDlg + "\", 'EditProfileProfileTypeSelection1') == 'V2D' || topic_Get(\"" + tDlg + "\", 'EditProfileProfileTypeSelection1') == null "},
                                                   /*{cmp:cmpBlkLblV2DEncoderAudio,//To Fix Task#38572
                                                    sel:"topic_Get(\"" + tDlg + "\", 'EditProfileProfileTypeSelection1') == 'V2D'"},*/
                                                   {cmp:cmpBlkLblVideoCompression,//To Fix Task#50857
                                                    sel:"topic_Get(\"" + tDlg + "\", 'EditProfileProfileTypeSelection1') == 'V2D'"},
                                                   /*{cmp:cmpBlkLblKeyboardMouse,// moved to first screen
                                                    sel:"topic_Get(\"" + tDlg + "\", 'EditProfileProfileTypeSelection1') == 'V2D'"},*/
                                                   {cmp:cmpBlkLblMPEGTS,
                                                    sel:"topic_Get(\"" + tDlg + "\", 'EditProfileProfileTypeSelection1') == 'MPEGTS'"},
                                                   {cmp:cmpBlkLblRTP,
                                                    sel:"topic_Get(\"" + tDlg + "\", 'EditProfileProfileTypeSelection1') == 'RTP'"},
                                                   {cmp:cmpBlkLblUDP,
                                                    sel:"topic_Get(\"" + tDlg + "\", 'EditProfileProfileTypeSelection1') == 'UDP'"},
                                                   {cmp:cmpBlkLblRTPRAW,
                                                    sel:"topic_Get(\"" + tDlg + "\", 'EditProfileProfileTypeSelection1') == 'RTP-RAW'"},
                                                   {cmp:cmpBlkLblRTPPCM,
                                                    sel:"topic_Get(\"" + tDlg + "\", 'EditProfileProfileTypeSelection1') == 'RTP-PCM'"},
                                                   {cmp:cmpBlkLblRFB,
                                                    sel:"topic_Get(\"" + tDlg + "\", 'EditProfileProfileTypeSelection1') == 'RFB'"}
                                                 ],
                                                 [{action:"TOPICEVENT", topic:tDlg, eventvar:"EditProfileProfileTypeSelection1", eventval:null}]
                                               );                                                   
                                                       
  var cmpLblV2DStreamConfig = new CmpLabelHighlight( [ProfileBlkName,"lblV2DStreamConfig"], null, 
                                                     [{action:"TOPICEVENT", topic:tDlg, eventvar:"EditStreamTypeConfigSelection", eventval:null}],
                                                     {action:"TOPICEVENT", topic:tDlg, eventvar:"EditStreamTypeConfigSelection", eventval:"V2DStreamConfig"} );
  var cmpLblV2DEncoderAudio = new CmpLabelHighlight( [ProfileBlkName,"lblV2DEncoderAudio"], null, 
                                                     [{action:"TOPICEVENT", topic:tDlg, eventvar:"EditStreamTypeConfigSelection", eventval:null}],
                                                     {action:"TOPICEVENT", topic:tDlg, eventvar:"EditStreamTypeConfigSelection", eventval:"V2DEncoderAudio"} );
  var cmpLblVideoCompression = new CmpLabelHighlight( [ProfileBlkName,"lblV2DVideoCompression"], null, 
                                                      [{action:"TOPICEVENT", topic:tDlg, eventvar:"EditStreamTypeConfigSelection", eventval:null}],
                                                      {action:"TOPICEVENT", topic:tDlg, eventvar:"EditStreamTypeConfigSelection", eventval:"VideoCompression"} );
//  var cmpLblKeyboardMouse = new CmpLabelHighlight( [ProfileBlkName,"lblV2DKeyboardMouse"], null, 
//                                                   [{action:"TOPICEVENT", topic:tDlg, eventvar:"EditStreamTypeConfigSelection", eventval:null}],
//                                                   {action:"TOPICEVENT", topic:tDlg, eventvar:"EditStreamTypeConfigSelection", eventval:"KeyboardMouse"} );
  var cmpLblMPEGTS = new CmpLabelHighlight( [ProfileBlkName,"lblMPEGTSProfile"], null, 
                                            [{action:"TOPICEVENT", topic:tDlg, eventvar:"EditStreamTypeConfigSelection", eventval:null}],
                                            {action:"TOPICEVENT", topic:tDlg, eventvar:"EditStreamTypeConfigSelection", eventval:"MPEGTSProfile"} );
  var cmpLblRTP = new CmpLabelHighlight( [ProfileBlkName,"lblRTPProfile"], null, 
                                         [{action:"TOPICEVENT", topic:tDlg, eventvar:"EditStreamTypeConfigSelection", eventval:null}],
                                         {action:"TOPICEVENT", topic:tDlg, eventvar:"EditStreamTypeConfigSelection", eventval:"RTPProfile"} );
  var cmpLblUDP = new CmpLabelHighlight( [ProfileBlkName,"lblUDPProfile"], null, 
                                         [{action:"TOPICEVENT", topic:tDlg, eventvar:"EditStreamTypeConfigSelection", eventval:null}],
                                         {action:"TOPICEVENT", topic:tDlg, eventvar:"EditStreamTypeConfigSelection", eventval:"UDPProfile"} );
  var cmpLblRTPRAW = new CmpLabelHighlight( [ProfileBlkName,"lblRTPRAWProfile"], null, 
                                            [{action:"TOPICEVENT", topic:tDlg, eventvar:"EditStreamTypeConfigSelection", eventval:null}],
                                            {action:"TOPICEVENT", topic:tDlg, eventvar:"EditStreamTypeConfigSelection", eventval:"RTPRAWProfile"} );
  var cmpLblRTPPCM = new CmpLabelHighlight( [ProfileBlkName,"lblRTPPCMProfile"], null, 
                                         [{action:"TOPICEVENT", topic:tDlg, eventvar:"EditStreamTypeConfigSelection", eventval:null}],
                                         {action:"TOPICEVENT", topic:tDlg, eventvar:"EditStreamTypeConfigSelection", eventval:"RTPPCMProfile"} );
  var cmpLblRFB = new CmpLabelHighlight( [ProfileBlkName,"lblRFBProfile"], null, 
                                         [{action:"TOPICEVENT", topic:tDlg, eventvar:"EditStreamTypeConfigSelection", eventval:null}],
                                         {action:"TOPICEVENT", topic:tDlg, eventvar:"EditStreamTypeConfigSelection", eventval:"RFBProfile"} );
                                          
  var cmpBlkProfileStreamType = new CmpContainer([ProfileBlkName,"blkLabelStreamConfigMenu"],
                                                  [{cmp:cmpLblV2DStreamConfig},
                                                   {cmp:cmpLblV2DEncoderAudio},
                                                   {cmp:cmpLblVideoCompression},
                                                   //{cmp:cmpLblKeyboardMouse},
                                                   {cmp:cmpLblMPEGTS},
                                                   {cmp:cmpLblRTP},
                                                   {cmp:cmpLblUDP},
                                                   {cmp:cmpLblRTPRAW},
                                                   {cmp:cmpLblRTPPCM},
                                                   {cmp:cmpLblRFB},
                                                   {cmp:cmpBlkProfileLabelBlk},
                                                   {cmp:cmpBlkProfileStreamTypeUI}
                                                   ],
                                                   [{action:"TOPICEVENT", topic:tDlg, eventvar:"ProfileTabSelected", eventval:null}]
                                                );
                                    
  return cmpBlkProfileStreamType;
 
}                


