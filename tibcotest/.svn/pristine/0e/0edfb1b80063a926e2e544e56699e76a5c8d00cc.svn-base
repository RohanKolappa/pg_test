// this code has some potential issues 
// - w.r.t object reference counts - when objects created in here go out of scope etc
// - loading sync issues - dlg may not be loaded before the object init methods are called

function dlgEditGroupTagValue_Create(GroupTagValueNID) {

  // deal with defaults for add
  if( GroupTagValueNID == null ) GroupTagValueNID = "DEFAULT_GROUPTAG_NID";
  var GroupTagNID = topic_Get(tApp, "GroupTagSelected");
  
  closeDialogIfExists("DialogGroupTagValue");
  // load view - dialog component file
  var DlgFile = "components/policy/groupValueDialog.xml";
  var BlkJSX = PNameSpace.getBodyBlock().load(DlgFile);
  
  var tDlg = "Dialog_" + jsx3.xml.CDF.getKey()
  topic_Create(tDlg);
  var eventDlg = new EventListner(tDlg);
  
  function dlgEditGroupTagValue_Destroy() {
    // destroy any local eventlistners
    eventDlg.destroy();  
    
    // destroy all local components
    cmpDlgEditGroupTagValue.destroy();
    
    // destroy the local topic
    topic_Destroy(tDlg);
    
    // close dialog
    BlkJSX.doClose();    
    
  }

        
  var cmpBtnEditGroupTagValueClose = new CmpButton([BlkJSX,"btnDialogCloseX"],
                                                     null, 
                                                     {action:"FNCCALL", 
                                                      context:null, 
                                                      fnc:function() {
                                                        dlgEditGroupTagValue_Destroy();
                                                      }
                                                     });
            
  var cmpBtnEditGroupTagValueCancel = new CmpButton([BlkJSX,"btnGroupTagValueCancel"],
                                                     null, 
                                                     {action:"FNCCALL", 
                                                      context:null, 
                                                      fnc:function() {
                                                        dlgEditGroupTagValue_Destroy();
                                                      }
                                                     });
  var cmpBtnEditGroupTagValueDone = new CmpButton([BlkJSX,"btnGroupTagValueDone"],
                                                   null, 
                                                   {action:"FNCCALL", 
                                                    context:null, 
                                                    fnc:function() {
                                                      cmpDlgEditGroupTagValueSetGroupTagValue.call(this, GroupTagValueNID);
                                                    }
                                                   });
                                                                                         
  var cmpDlgEditGroupTagValue = new CmpContainer([BlkJSX,"DialogGroupTagValue"],
                                                  [ {cmp:cmpBtnEditGroupTagValueClose},
                                                    {cmp:cmpBtnEditGroupTagValueCancel},
                                                    {cmp:cmpBtnEditGroupTagValueDone}
                                                  ],
                                                  [{action:"TOPICEVENT", topic:tDlg, eventvar:"GroupTagEditDataState", eventval:GroupTagValueNID}]);                            
                    
  cmpDlgEditGroupTagValue.BlkJSX = BlkJSX;      
  cmpDlgEditGroupTagValue.ObjectNID = GroupTagValueNID;
  cmpDlgEditGroupTagValue.getBlkData =  cmpDlgEditGroupTagValueGetGroupTagValue;
  
  // init and repaint happens after the dialog is loaded
  jsx3.sleep( (function(){
                 this.init();
                 this.getBlkData(this.ObjectNID);
                 }
               ),
               null, cmpDlgEditGroupTagValue );
          
  // fnc to get dlg data
  function cmpDlgEditGroupTagValueGetGroupTagValue(GroupTagValueNID) { 
    var GroupTagValueNode = topic_GetNode(tCache, "//*[@NID='" + GroupTagValueNID + "']");
    if( GroupTagValueNode == null ) {
      var reqData = new ReqGetObjectNID(GroupTagValueNID, "GroupTagValueDef",
                                         {action:"FNCCALL", context:this, 
                                           fnc:function(respCode, respVal) {
                                                 if(respCode == "SUCCESS" && respVal != null) {
                                                   cmpDlgEditGroupTagValueGetGroupTagValue(GroupTagValueNID);
                                                 } else {
                                                   var codeErrorMsg = CommonErrorCodeArr[respCode].errorDesc;
                                                   var ErrorMsg = "Error: TagValue get data request failed \n";
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
      //cmpDlgEditGroupTagValue.setMapRuleData("GroupTagValue", "IN", "NID", GroupTagValueNID);
      //topic_Publish(tDlg, "GroupTagEditDataState", GroupTagValueNID);

      // using the old repaint of the dialog
      DisplayEditValuesForGroupTagValue(GroupTagValueNID);
      //topic_Publish(tDlg, "GroupTagEditDataState", GroupTagValueNID);
    }
  }
       
  // fnc to set dlg data - runs in the context of the OK button
  function cmpDlgEditGroupTagValueSetGroupTagValue(GroupTagValueNID) {
    // validate input 
    if( cmpDlgEditGroupTagValueValidate() != true ) return;
    // decide add or update
    var SetMode = "Replace";

    // setup GroupTagValue map rule info
    var GroupTagValueNode = "";
    var GroupTagValueMapRule = new Object;
    GroupTagValueMapRule.state = false;
    GroupTagValueMapRule.objtype = "GroupTagDef";
    GroupTagValueMapRule.rule = "";

    if( GroupTagValueNID == "DEFAULT_GROUPTAG_NID" ) {
      GroupTagValueNode = addTagValueRequestXML();
    } else {
      GroupTagValueNode = deleteTagValueRequestXML();
    }
          
    if( GroupTagValueNode == null ) return;
    // set NID in GroupTagValue node if replace mode

    GroupTagValueNode.setAttribute("NID", GroupTagNID);
    // do set request for GroupTagValue              
    var reqSetGroupTagValueObject = new ReqSetObject(SetMode, GroupTagValueMapRule.objtype, GroupTagValueNode, 
                                                      {action:"FNCCALL", context:this, 
                                                         fnc:function(respCode, respVal) {
                                                               if(respCode == "SUCCESS" && respVal != null) {
                                                                 dlgEditGroupTagValue_Destroy();
                                                               } else {
                                                                 var codeErrorMsg = CommonErrorCodeArr[respCode].errorDesc;
                                                                 var ErrorMsg = "Error: Edit TagValue request failed \n";
                                                                 ErrorMsg += "Error description: " + codeErrorMsg + "\n";
                                                                 ErrorMsg += "Error code: " + respCode;
                                                                 alert(ErrorMsg);
                                                               }
                                                             }
                                                      }                                                       
                                                    );
    if( reqSetGroupTagValueObject != null) {
      reqSetGroupTagValueObject.sendRequest(this);
    }
  } 
      
  function cmpDlgEditGroupTagValueValidate() {
    var valUserPolicyGroupValue = getJsxByName([BlkJSX,"txtTagValue"]).getValue();

    var tagValue = checkValue(valUserPolicyGroupValue , [BlkJSX,"txtTagValue"] , "Please enter Tag value");
    var duplicateCheck = "";
    if(tagValue) {
      duplicateCheck = chkForNameExistence(valUserPolicyGroupValue, [BlkJSX,"txtTagValue"], "mtxGroupTagValue", "Duplicate Tag value. Please enter some other Tag value");
      if(duplicateCheck) {
        return true;
      } 
    }    
    return false;
  }

  function DisplayEditValuesForGroupTagValue(GroupTagValueNID) {
    var groupTagNameNode = topic_GetNode(tCache, ".//*[@NID='" + GroupTagValueNID + "']");
    
    var version = groupTagNameNode.getAttribute("Ver");
    var versionBlkJSX = getJsxByName([BlkJSX,"txtGrpTagNameXMLVersion"]);
    versionBlkJSX.setValue(version);
    
    var revision = groupTagNameNode.getAttribute("Rev");
    var revisionBlkJSX = getJsxByName([BlkJSX,"txtGrpTagNameXMLRevision"]);
    revisionBlkJSX.setValue(revision);

    var serviceVer = groupTagNameNode.getAttribute("serviceVer");
    var serviceVerBlkJSX = getJsxByName([BlkJSX,"txtGrpTagNameXMLServiceVersion"]);
    serviceVerBlkJSX.setValue(serviceVer);
    
    var groupTagValue = groupTagNameNode.selectSingleNode(".//TagValueList/TagValue").getValue();
    var txtTagValueBlkJSX = getJsxByName([BlkJSX,"txtTagValue"]);
    txtTagValueBlkJSX.setValue(groupTagValue);
    txtTagValueBlkJSX.focus();

  }
  
  function addTagValueRequestXML(){
    var CacheGroupTagListNode = (getObjectCacheDefaultNode("GroupTagDef", GroupTagNID)).cloneNode(true);
    var groupTagName = CacheGroupTagListNode.selectSingleNode(".//TagName").getValue();
    groupTagValueList = CacheGroupTagListNode.selectSingleNode(".//TagValueList/TagValue").cloneNode(true);			
    var groupTagValueListParent = CacheGroupTagListNode.selectSingleNode(".//TagValue").getParent();
    var GroupTagListNewNode = groupTagValueListParent.createNode(jsx3.xml.Entity.TYPEELEMENT, "TagValue");
    GroupTagListNewNode.setValue(PNameSpace.getJSXByName("txtTagValue").getValue());
    groupTagValueListParent.appendChild(GroupTagListNewNode);

    return  CacheGroupTagListNode;
  }

  function editTagValueRequestXML(){
    var tAppNode = topic_GetRootNode(tApp);  
    var tCacheNode = topic_GetRootNode(tCache);  
    var CacheGroupTagListNode = (getObjectCacheDefaultNode("GroupTagDef", GroupTagNID)).cloneNode(true);
    var groupTagName = CacheGroupTagListNode.selectSingleNode(".//TagName").getValue();
    groupTagValueList=CacheGroupTagListNode.selectSingleNode(".//TagValueList/TagValue").cloneNode(true);			
    var groupTagValueListParent  = CacheGroupTagListNode.selectSingleNode(".//TagValue").getParent();
    var GroupTagListNewNode = groupTagValueListParent.createNode(jsx3.xml.Entity.TYPEELEMENT, "TagValue");

    var oldTagValue = topic_Get(tApp, "GroupTagValueSelected");
    var groupTagValueListIter  = CacheGroupTagListNode.selectNodeIterator(".//TagValueList/TagValue");
    while( groupTagValueListIter .hasNext() ) {
      var groupTagValueNode = groupTagValueListIter .next();
      var groupTagValueName = groupTagValueNode.getValue();
      if(groupTagValueName == oldTagValue ) {
        GroupTagListNewNode.setValue(PNameSpace.getJSXByName("txtTagValue").getValue());
        groupTagValueListParent.replaceNode(GroupTagListNewNode,groupTagValueNode);
      }
    }   

    return  CacheGroupTagListNode;
  }

  function deleteTagValueRequestXML(){
    var CacheGroupTagListNode = (getObjectCacheDefaultNode("GroupTagDef", GroupTagNID)).cloneNode(true); 
    var groupTagName = CacheGroupTagListNode.selectSingleNode(".//TagName").getValue();
    groupTagValueList=CacheGroupTagListNode.selectSingleNode(".//TagValueList/TagValue").cloneNode(true);			
    var groupTagValueListParent  = CacheGroupTagListNode.selectSingleNode(".//TagValue").getParent();

    var oldTagValue = topic_Get(tApp, "GroupTagValueSelected");
    var groupTagValueListIter  = CacheGroupTagListNode.selectNodeIterator(".//TagValueList/TagValue");
    while( groupTagValueListIter .hasNext() ) {
      var groupTagValueNode = groupTagValueListIter .next();
      var groupTagValueName = groupTagValueNode.getValue();
      if(groupTagValueName == oldTagValue ) {
        groupTagValueListParent.removeChild(groupTagValueNode);
      }
    }   

    return  CacheGroupTagListNode;
  }


}