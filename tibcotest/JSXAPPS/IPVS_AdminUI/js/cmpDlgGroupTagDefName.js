// this code has some potential issues 
// - w.r.t object reference counts - when objects created in here go out of scope etc
// - loading sync issues - dlg may not be loaded before the object init methods are called

function dlgEditGroupTag_Create(GroupTagNID) {

  // deal with defaults for add
  if( GroupTagNID == null ) GroupTagNID = "DEFAULT_GROUPTAG_NID";
  
  closeDialogIfExists("DialogGroupTag");
  // load view - dialog component file
  var DlgFile = "components/policy/groupTagNewDialog.xml";
  var BlkJSX = PNameSpace.getBodyBlock().load(DlgFile);
  
  var tDlg = "Dialog_" + jsx3.xml.CDF.getKey()
  topic_Create(tDlg);
  var eventDlg = new EventListner(tDlg);
  
  function dlgEditGroupTag_Destroy() {
    // destroy any local eventlistners
    eventDlg.destroy();  
    
    // destroy all local components
    cmpDlgEditGroupTag.destroy();
    
    // destroy the local topic
    topic_Destroy(tDlg);
    
    // close dialog
    BlkJSX.doClose(); 
  }
     
  var cmpBtnEditGroupTagClose = new CmpButton([BlkJSX,"btnDialogCloseX"],
                                               null, 
                                               {action:"FNCCALL", 
                                                context:null, 
                                                fnc:function() {
                                                  dlgEditGroupTag_Destroy();
                                                }
                                               });
            
  var cmpBtnEditGroupTagCancel = new CmpButton([BlkJSX,"btnGroupTagCancel"],
                                               null, 
                                               {action:"FNCCALL", 
                                                context:null, 
                                                fnc:function() {
                                                  dlgEditGroupTag_Destroy();
                                                }
                                               });
            
  var cmpBtnEditGroupTagDone = new CmpButton([BlkJSX,"btnGroupTagDone"],
                                               null, 
                                               {action:"FNCCALL", 
                                                context:null, 
                                                fnc:function() {
                                                  cmpDlgEditGroupTagSetGroupTag.call(this, GroupTagNID);
                                                }
                                               });
                                                                                         
  var cmpDlgEditGroupTag = new CmpContainer([BlkJSX,"DialogGroupTag"],
                                            [ {cmp:cmpBtnEditGroupTagClose},
                                              {cmp:cmpBtnEditGroupTagCancel},
                                              {cmp:cmpBtnEditGroupTagDone}
                                            ],
                                            [{action:"TOPICEVENT", topic:tDlg, eventvar:"GroupTagEditDataState", eventval:GroupTagNID}]);                            
              
  cmpDlgEditGroupTag.BlkJSX = BlkJSX;
  cmpDlgEditGroupTag.ObjectNID = GroupTagNID;
  cmpDlgEditGroupTag.getBlkData =  cmpDlgEditGroupTagGetGroupTag;
  
  // init and repaint happens after the dialog is loaded
  jsx3.sleep( (function(){
                 this.init();
                 this.getBlkData(this.ObjectNID);
                 }
               ),
               null, cmpDlgEditGroupTag );        
      
  // fnc to get dlg data
  function cmpDlgEditGroupTagGetGroupTag(GroupTagNID) { 
    var GroupTagNode = topic_GetNode(tCache, "//*[@NID='" + GroupTagNID + "']");
    if( GroupTagNode == null ) {
      var reqData = new ReqGetObjectNID(GroupTagNID, "GroupTagDef",
                                         {action:"FNCCALL", context:this, 
                                           fnc:function(respCode, respVal) {
                                                 if(respCode == "SUCCESS" && respVal != null) {
                                                   cmpDlgEditGroupTagGetGroupTag(GroupTagNID);
                                                 } else {
                                                   var codeErrorMsg = CommonErrorCodeArr[respCode].errorDesc;
                                                   var ErrorMsg = "Error: TagName get data request failed \n";
                                                   ErrorMsg += "Error description: " + codeErrorMsg + "\n";
                                                   ErrorMsg += "Error code: " + respCode;
                                                   alert(ErrorMsg);
                                                 }                                                   
                                               }
                                         } 
                                       );
      reqData.sendRequest();
    }
    else {
      //cmpDlgEditGroupTag.setMapRuleData("GroupTag", "IN", "NID", GroupTagNID);
      //topic_Publish(tDlg, "GroupTagEditDataState", GroupTagNID);

      // using the old repaint of the dialog
      DisplayEditValuesForGroupTag(GroupTagNID);
      topic_Publish(tDlg, "GroupTagEditDataState", GroupTagNID);
    }
  }
       
  // fnc to set dlg data - runs in the context of the OK button
  function cmpDlgEditGroupTagSetGroupTag(GroupTagNID) {
    // validate input 
    if( cmpDlgEditGroupTagValidate() != true ) return;
    // decide add or update
    var SetMsg = "Edit TagName";
    var SetMode = "Replace";
    if( GroupTagNID == "DEFAULT_GROUPTAG_NID" ) {
      SetMsg = "Add TagName";
      SetMode = "Add";
    }
    
    // setup GroupTag map rule info
    var GroupTagNode = "";

    if( SetMode == "Replace" ) {
      GroupTagNode = cmpGroupTagName_Edit_RequestRuleXML(GroupTagNID);
    } else if (SetMode == "Add") {
      var txtTagNameBlkJSX = getJsxByName([BlkJSX, "txtTagName"]);

      var groupTagDefStr = "<GroupTagDef serviceVer='" + getServiceVersion() + "' Ver='1.3' Rev='1' NID='' parentNID=''><TagName>" + txtTagNameBlkJSX.getValue() + "</TagName> "+creatGroupTagTypeStrList()+" <TagValueList><TagValue /></TagValueList></GroupTagDef>";

      GroupTagNode = new jsx3.xml.Document().loadXML(groupTagDefStr);
    }
          
    if( GroupTagNode == null ) return;
    // set NID in GroupTag node if replace mode
    if( SetMode == "Replace" ) {
      GroupTagNode.setAttribute("NID", GroupTagNID);
    }

    // do set request for GroupTag              
    var reqSetGroupTagObject = new ReqSetObject(SetMode, "GroupTagDef", GroupTagNode, 
                                                  {action:"FNCCALL", context:this, 
                                                     fnc:function(respCode, respVal) {
                                                           if(respCode == "SUCCESS" && respVal != null) {
                                                             dlgEditGroupTag_Destroy();
                                                             topic_Publish(tApp, "GroupTagDefDataUpdate", "ADD");
                                                           } else {
                                                             var codeErrorMsg = CommonErrorCodeArr[respCode].errorDesc;
                                                             var ErrorMsg = "Error: "+ SetMsg +" request failed \n";
                                                             ErrorMsg += "Error description: " + codeErrorMsg + "\n";
                                                             ErrorMsg += "Error code: " + respCode;
                                                             alert(ErrorMsg);
                                                           }
                                                         }
                                                  }                                                 
                                               );
                                               
    if( reqSetGroupTagObject != null) {
      reqSetGroupTagObject.sendRequest(this);
    }
  } 
      
  function cmpDlgEditGroupTagValidate() {

    var valTagNameName = getJsxByName([BlkJSX,"txtTagName"]).getValue();
    var duplicateCheck = "";
    var tagName = checkValue(valTagNameName , [BlkJSX,"txtTagName"] , "Please enter Tag name");
        
    if( GroupTagNID == "DEFAULT_GROUPTAG_NID" ) {
      if(tagName) {
        duplicateCheck = chkForNameExistence(valTagNameName,[BlkJSX,"txtTagName"],"mtxGroupTagName", "Duplicate Tag name. Please enter some other Tag name");
        if(duplicateCheck) {
          return true;
        }    
      }     
    } else {
      if(tagName) {
        return true;
      }    
    }
     
    return false;
  }

  function DisplayEditValuesForGroupTag(GroupTagNID) {
    var groupTagNameNode = topic_GetNode(tCache, ".//*[@NID='" + GroupTagNID + "']");
    var version = groupTagNameNode.getAttribute("Ver");
    var versionBlkJSX = getJsxByName([BlkJSX,"txtGrpTagNameXMLVersion"]);
    versionBlkJSX.setValue(version);
    
    var revision = groupTagNameNode.getAttribute("Rev");
    var revisionBlkJSX = getJsxByName([BlkJSX,"txtGrpTagNameXMLRevision"]);
    revisionBlkJSX.setValue(revision);

    var serviceVer = groupTagNameNode.getAttribute("serviceVer");
    var serviceVerBlkJSX = getJsxByName([BlkJSX,"txtGrpTagNameXMLServiceVersion"]);
    serviceVerBlkJSX.setValue(serviceVer);
    
    var groupTagName = groupTagNameNode.selectSingleNode(".//TagName").getValue();
    var txtTagNameBlkJSX = getJsxByName([BlkJSX,"txtTagName"]);
    txtTagNameBlkJSX.setValue(groupTagName);
    if( GroupTagNID != "DEFAULT_GROUPTAG_NID" ) {
      txtTagNameBlkJSX.setEnabled(jsx3.gui.Form.STATEDISABLED, true);
    }

    var groupTagPropertyListIter = groupTagNameNode.selectNodeIterator(".//TagObjectTypeList/TagObjectType");
    while( groupTagPropertyListIter.hasNext() ) {
      var groupTagPropertyNode = groupTagPropertyListIter.next();
      var groupTagPropertyType = groupTagPropertyNode.getValue();
      switch(groupTagPropertyType) {
        case "User":
          changeCheckBoxState([BlkJSX,"chkTagTypeUser"],"1");
          break;
        case "AssetResource":
          changeCheckBoxState([BlkJSX,"chkTagTypeAsset"],"1");
          break;
        case "MediaStreamSrcPortResource":
        case "MediaStreamDstPortResource":
        case "MediaStreamRelayPortResource":
        case "MediaStorePortResource":
          changeCheckBoxState([BlkJSX,"chkTagTypeResource"],"1");
          break;
        case "MediaStreamProfile":
        case "DisplayProfile":
          changeCheckBoxState([BlkJSX,"chkTagTypeProfile"],"1");
          break;
        case "MediaRoom":
          changeCheckBoxState([BlkJSX,"chkTagTypeMediaRoom"],"1");
        case "MediaStreamFileResource":
        case "MediaStreamDirResource":
          changeCheckBoxState([BlkJSX,"chkTagTypeMedia"],"1");          
          break;
      }
    }
    txtTagNameBlkJSX.focus();
  }
  
  function cmpGroupTagName_Add_RequestRuleXML(){
    var oService = PNameSpace.loadResource("GroupTagDef_xml");
    oService.setOperation("");
    var RequestXML = oService.getServiceMessage();     
    var finalDoc = new jsx3.xml.Document();
    finalDoc.loadXML(RequestXML);
    var elementNode = finalDoc.getRootNode().cloneNode(true);
    
    return elementNode;
  }

  function cmpGroupTagName_Edit_RequestRuleXML(GroupTagNID){
    var CacheGroupTagNode = (getObjectCacheDefaultNode("GroupTagDef", GroupTagNID)).cloneNode(true);
    GroupTagNewNode = cmpGroupTagName_Add_RequestRuleXML();

    var groupTagValueRuleList = GroupTagNewNode.selectSingleNode(".//TagValueList");
    var groupTagValueCacheList = CacheGroupTagNode.selectSingleNode(".//TagValueList");
    var groupTagValueListParent = GroupTagNewNode.selectSingleNode(".//TagValueList").getParent();
        groupTagValueListParent.replaceNode(groupTagValueCacheList,groupTagValueRuleList);
    
    return  GroupTagNewNode;
  }

  function creatGroupTagTypeStrList() {
    var typeList = "<TagObjectTypeList>";
  
    var typeMedia = getJsxByName([BlkJSX,"chkTagTypeMedia"]).getChecked();
    var typeSrcPortResource = getJsxByName([BlkJSX,"chkTagTypeResource"]).getChecked();
    var typeProfile = getJsxByName([BlkJSX,"chkTagTypeProfile"]).getChecked();
    var typeMediaRoom = getJsxByName([BlkJSX,"chkTagTypeMediaRoom"]).getChecked();
    var typeUser = getJsxByName([BlkJSX,"chkTagTypeUser"]).getChecked();
    var typeAsset = getJsxByName([BlkJSX,"chkTagTypeAsset"]).getChecked();
    
    if(typeMedia) {
      typeList += "<TagObjectType>MediaStreamFileResource</TagObjectType>";
      typeList += "<TagObjectType>MediaStreamDirResource</TagObjectType>";
    }
    if(typeSrcPortResource) {
      typeList += "<TagObjectType>MediaStreamSrcPortResource</TagObjectType>";
      typeList += "<TagObjectType>MediaStreamDstPortResource</TagObjectType>"; 
      typeList += "<TagObjectType>MediaStreamRelayPortResource</TagObjectType>";
      typeList += "<TagObjectType>MediaStorePortResource</TagObjectType>";        
    }    
    if(typeProfile) {
      typeList += "<TagObjectType>MediaStreamProfile</TagObjectType>";
      typeList += "<TagObjectType>DisplayProfile</TagObjectType>";    
    }
    if (typeMediaRoom) {
      typeList += "<TagObjectType>MediaRoom</TagObjectType>";
   
    }
    if(typeUser) {
      typeList += "<TagObjectType>User</TagObjectType>";
    }
    if(typeAsset) {
      typeList += "<TagObjectType>AssetResource</TagObjectType>";
    }

    typeList += "</TagObjectTypeList>";
    return typeList;
  }//function end

}