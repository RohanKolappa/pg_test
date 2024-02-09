// this code has some potential issues 
// - w.r.t object reference counts - when objects created in here go out of scope etc
// - loading sync issues - dlg may not be loaded before the object init methods are called

function dlgEditMediaStoreAdmin_Create(MediaStoreAdminNID, PolicyType) {

  // deal with defaults for add
  if( MediaStoreAdminNID == null ) MediaStoreAdminNID = "DEFAULT_MEDIASTOREADMIN_NID";
  
  // load view - dialog component file
  var DlgFile = "components/policy/newMediaStorePolicy.xml";
  var BlkJSX = PNameSpace.getBodyBlock().load(DlgFile);

  // setup a local topic for dlg specific messaging     
  var tDlg = "Dialog_" + jsx3.xml.CDF.getKey();
  topic_Create(tDlg);

  // setup a listner for it      
  var eventDlg = new EventListner(tDlg);
  var orgOrderNumber = "";  

  // setup a private/dlg specific app event handler
  dlgEditMediaStoreAdmin_eventAppTagObject = new EventListner( tApp );
  dlgEditMediaStoreAdmin_eventAppGroupTag = new EventListner( tApp );

  //set the right text for the dialog
  var dlgText = "Manage Media Policy";
  if (PolicyType == "MediaAccessPolicy") {
    dlgText = "Playback Files Policy";
  }
  else if (PolicyType == "RecordingAccessPolicy") {
    dlgText = "Record/PVR To Directories Policy";
  }
  if( MediaStoreAdminNID == "DEFAULT_MEDIASTOREADMIN_NID" ){
    dlgText = "Add " + dlgText;
  }
  else{
    dlgText = "Edit " + dlgText;
  }
  var windowBarBlkJSX = getJsxByName([BlkJSX, "windowBar"]);
  //set the text
  windowBarBlkJSX.setText(dlgText, true);
  closeDialogIfExists(dlgText);

  var cmpBlkMediaStoreFilterMatrix = new CmpBlock([BlkJSX,"mtxMediaStorePolicy_MediaStoreFilterSel"]);
  cmpBlkMediaStoreFilterMatrix.BlkJSX.setXMLTransformers("xsl/PolicyUserFilterToCDF.xsl");
  cmpBlkMediaStoreFilterMatrix.BlkJSX.setSelectionModel(jsx3.gui.Matrix.SELECTION_ROW);

  var cmpBlkMediaStoreTargetFilterMatrix = new CmpBlock([BlkJSX,"mtxMediaStorePolicy_TargetFilterSel"]);
  cmpBlkMediaStoreTargetFilterMatrix.BlkJSX.setXMLTransformers("xsl/PolicyTargetFilterToCDF.xsl");
  cmpBlkMediaStoreTargetFilterMatrix.BlkJSX.setSelectionModel(jsx3.gui.Matrix.SELECTION_ROW);

  var cmpBlkMediaStoreRuleKeyMatrix = new CmpBlock([BlkJSX,"mtxMediaStorePolicy_RuleKeySel"]);
  cmpBlkMediaStoreRuleKeyMatrix.BlkJSX.setXMLTransformers("xsl/PolicyRuleKeyFilterToCDF.xsl");
  cmpBlkMediaStoreRuleKeyMatrix.BlkJSX.setSelectionModel(jsx3.gui.Matrix.SELECTION_ROW);

  var cmpCDFMediaStorePolicy = new Component([BlkJSX,"cdfDlgMediaStorePolicy"]);
  cmpCDFMediaStorePolicy.BlkJSX.setXMLTransformers("xsl/MediaStorePolicy.xsl");

  // hide the action block for these policies
  if (PolicyType == "MediaStoreAdminPolicy" || PolicyType == "MediaAccessPolicy" || PolicyType == "RecordingAccessPolicy") {
    var titleBlock = getJsxByName([BlkJSX, "blkMediaStoreActionSectionTitle"]);
    var actionBlock = getJsxByName([BlkJSX, "blkMediaStorePolicyActions"]);
    var actionDirBlock = getJsxByName([BlkJSX, "blkMediaStoreDirPolicyActions"]);
    var recordPVRBlock = getJsxByName([BlkJSX, "blkMediaStorePolicyRecordPVRActions"]);
    if(PolicyType != "MediaStoreAdminPolicy") {
      titleBlock.setDisplay(jsx3.gui.Block.DISPLAYNONE, true);
      actionBlock.setDisplay(jsx3.gui.Block.DISPLAYNONE, true);
      actionDirBlock.setDisplay(jsx3.gui.Block.DISPLAYNONE, true);
    }
    recordPVRBlock.setDisplay(jsx3.gui.Block.DISPLAYNONE, true);
    if(PolicyType == "RecordingAccessPolicy") {
      recordPVRBlock.setDisplay(jsx3.gui.Block.DISPLAYBLOCK, true);
    }
  }


  function dlgEditMediaStoreAdmin_Destroy() {
    // destroy any local eventlistners
    eventDlg.destroy();
    dlgEditMediaStoreAdmin_eventAppTagObject.destroy();
    dlgEditMediaStoreAdmin_eventAppGroupTag.destroy();
    
    // destroy all local components
    cmpDlgEditMediaStoreAdmin.destroy();
    
    // destroy the local topic
    topic_Destroy(tDlg);
    
    // close dialog
    BlkJSX.doClose();
  }
  
  // event subscribtion: if new tagobjectfilter is added from select box and update all the
  // select boxes in the dialog after add is finished
  dlgEditMediaStoreAdmin_eventAppTagObject.subscribe("TagObjectFilterDataState", null, this,
                                                  function() {
                                                    updateSelectTagObjectFilters();
                                                  } );
  
  // event subscribtion: if new grouptag is added from select box and update all the
  // select boxes in the dialog after add is finished                      
  dlgEditMediaStoreAdmin_eventAppGroupTag.subscribe("GroupTagDataState", null, this,
                                                function() {
                                                  updateSelectGroupTag();
                                                } );

  var cmpSelUserFilter = new CmpSelect([BlkJSX,"selMediaStorePolicy_UserFilter"],
                                               null,
                                               {action:"FNCCALL", 
                                               context:null, 
                                               fnc:function() {
                                                  var selVal = cmpSelUserFilter.BlkJSX.getValue();
                                                  if (jsx3.util.strEmpty(selVal)) return;
                                                  var strXMLCacheId = cmpSelUserFilter.BlkJSX.getXMLId();
                                                  var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
                                                  if (jsx3.util.strEmpty(objXML)) return;
                                                  var record = objXML.selectSingleNode("//record[@jsxid='" + selVal + "']");
                                                  checkAddForNewFilter(record, "User");
                                               }
                                              });

  var cmpSelResourceFilter = new CmpSelect([BlkJSX,"selMediaStorePolicy_ResourceFilter"],
                                               null,
                                               {action:"FNCCALL", 
                                               context:null, 
                                               fnc:function() {
                                                  var selVal = cmpSelResourceFilter.BlkJSX.getValue();
                                                  if (jsx3.util.strEmpty(selVal)) return;
                                                  var strXMLCacheId = cmpSelResourceFilter.BlkJSX.getXMLId();
                                                  var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
                                                  if (jsx3.util.strEmpty(objXML)) return;
                                                  var record = objXML.selectSingleNode("//record[@jsxid='" + selVal + "']");
                                                  checkAddForNewFilter(record, "Resource");
                                               }
                                              });

  var cmpBtnAddMediaStoreFilter = new CmpButton([BlkJSX,"btnMediaStorePolicy_AddUserFilter"],
                                                 null, 
                                                 {action:"FNCCALL", 
                                                  context:null, 
                                                  fnc:function() {
                                                      addDatatoMatrix(this.userFilterBlkJSX, this.resourceFilterBlkJSX, this.mediaStoreFilterMatrix);
                                                  }
                                                 });
  cmpBtnAddMediaStoreFilter.userFilterBlkJSX = cmpSelUserFilter.BlkJSX;
  cmpBtnAddMediaStoreFilter.resourceFilterBlkJSX = cmpSelResourceFilter.BlkJSX;
  cmpBtnAddMediaStoreFilter.mediaStoreFilterMatrix = cmpBlkMediaStoreFilterMatrix.BlkJSX;
        
  var cmpSelTargetFilter = new CmpSelect([BlkJSX,"selMediaStorePolicyFilter_Target"],
                                               null,
                                               {action:"FNCCALL", 
                                               context:null, 
                                               fnc:function() {
                                                  var selVal = cmpSelTargetFilter.BlkJSX.getValue();
                                                  if (jsx3.util.strEmpty(selVal)) return;
                                                  var strXMLCacheId = cmpSelTargetFilter.BlkJSX.getXMLId();
                                                  var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
                                                  if (jsx3.util.strEmpty(objXML)) return;
                                                  var record = objXML.selectSingleNode("//record[@jsxid='" + selVal + "']");
                                                  checkAddForNewFilter(record, "Resource");
                                               }
                                              });
  
  var cmpBtnAddMediaStoreTargetFilter = new CmpButton([BlkJSX,"btnMediaStorePolicy_AddTargetFilter"],
                                                 null, 
                                                 {action:"FNCCALL", 
                                                  context:null, 
                                                  fnc:function() {
                                                      var targetRecord = this.targetFilterBlkJSX.getValue();
                                                      if(targetRecord!=""){
                                                        var mtachResult = targetRecord.search(/Select /);

                                                        if(mtachResult != -1){
                                                          alert("Please "+targetRecord);
                                                          return false;
                                                        } else {
                                                          var targetNode = this.targetFilterBlkJSX.getRecord(targetRecord);
                                                          var targetName= targetNode.jsxtext;
                                                          var targetNID = targetNode.NID;
                                                        }
                                                        var o = new Object();
                                                        o.jsxid = jsx3.xml.CDF.getKey();
                                                        o.jsxtextOneNID = targetNID;
                                                        o.jsxtextOne = targetName;
                                                        this.mediaStoreTargetFilterMatrix.insertRecord(o, null, false);
                                                      }
                                                      this.mediaStoreTargetFilterMatrix.repaintData();
                                                      this.targetFilterBlkJSX.setValue("Select ResourceProfile Filter");
                                                  }
                                                 });
  cmpBtnAddMediaStoreTargetFilter.targetFilterBlkJSX = cmpSelTargetFilter.BlkJSX;
  cmpBtnAddMediaStoreTargetFilter.mediaStoreTargetFilterMatrix = cmpBlkMediaStoreTargetFilterMatrix.BlkJSX;
  
  var cmpSelRuleKeyLHTag = new CmpSelect([BlkJSX,"selMediaStorePolicy_RuleKeyLHTag"],
                                               null,
                                               {action:"FNCCALL", 
                                               context:null, 
                                               fnc:function() {
                                                  var selVal = cmpSelRuleKeyLHTag.BlkJSX.getValue();
                                                  if (jsx3.util.strEmpty(selVal)) return;
                                                  var strXMLCacheId = cmpSelRuleKeyLHTag.BlkJSX.getXMLId();
                                                  var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
                                                  if (jsx3.util.strEmpty(objXML)) return;
                                                  var record = objXML.selectSingleNode("//record[@jsxid='" + selVal + "']");
                                                  checkAddForNewTagName(record);
                                               }
                                              });
  var cmpSelRuleKeyRHTag = new CmpSelect([BlkJSX,"selMediaStorePolicy_RuleKeyRHTag"],
                                               null,
                                               {action:"FNCCALL", 
                                               context:null, 
                                               fnc:function() {
                                                  var selVal = cmpSelRuleKeyRHTag.BlkJSX.getValue();
                                                  if (jsx3.util.strEmpty(selVal)) return;
                                                  var strXMLCacheId = cmpSelRuleKeyRHTag.BlkJSX.getXMLId();
                                                  var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
                                                  if (jsx3.util.strEmpty(objXML)) return;
                                                  var record = objXML.selectSingleNode("//record[@jsxid='" + selVal + "']");
                                                  checkAddForNewTagName(record);
                                               }
                                              });
  
  var cmpBtnAddMediaStoreRuleKey = new CmpButton([BlkJSX,"btnAddMediaStorePolicy_RuleKey"],
                                                 null, 
                                                 {action:"FNCCALL", 
                                                  context:null, 
                                                  fnc:function() {
                                                      var ruleKeyLHRecord = this.LHFilterRuleKeyBlkJSX.getValue();
                                                      if(ruleKeyLHRecord!=""){
                                                        var LHSRuleKey = SelectRuleKeyCheck(this.LHFilterRuleKeyBlkJSX);
                                                        if(LHSRuleKey)
                                                          var LHSTagName = SelectRuleKeyCheck(this.LHTagRuleKeyBlkJSX);
                                                        if(LHSTagName)
                                                          var MatchOperation = SelectRuleKeyCheck(this.OperatorRuleKeyBlkJSX);
                                                        if(MatchOperation)
                                                          var RHSRuleKey = SelectRuleKeyCheck(this.RHFilterRuleKeyBlkJSX);
                                                        if(RHSRuleKey)
                                                          var RHSTagName = SelectRuleKeyCheck(this.RHTagRuleKeyBlkJSX);
                                                        if(RHSTagName) {
                                                          var o = new Object();
                                                          o.jsxid = jsx3.xml.CDF.getKey() ;
                                                          o.jsxtextLHRuleKey = LHSRuleKey;
                                                          o.jsxtextLHTagName = LHSTagName;
                                                          o.jsxtextOperation = MatchOperation;
                                                          o.jsxtextRHRuleKey = RHSRuleKey;
                                                          o.jsxtextRHTagName = RHSTagName;
                                                          this.mediaStoreRuleKeyMatrix.insertRecord(o, null, false);
                                                        }
                                                      }
                                                      this.mediaStoreRuleKeyMatrix.repaintData();
                                                      this.LHFilterRuleKeyBlkJSX.setValue("Select LH Rule");
                                                      this.LHTagRuleKeyBlkJSX.setValue("Select Tag Name");
                                                      this.OperatorRuleKeyBlkJSX.setValue("Select Operation");
                                                      this.RHFilterRuleKeyBlkJSX.setValue("Select RH Rule");
                                                      this.RHTagRuleKeyBlkJSX.setValue("Select Tag Name");
                                                  }
                                                 });
  cmpBtnAddMediaStoreRuleKey.LHFilterRuleKeyBlkJSX = getJsxByName([BlkJSX,"selMediaStorePolicy_RuleKeyLHFilter"]);
  cmpBtnAddMediaStoreRuleKey.LHTagRuleKeyBlkJSX = cmpSelRuleKeyLHTag.BlkJSX;
  cmpBtnAddMediaStoreRuleKey.OperatorRuleKeyBlkJSX = getJsxByName([BlkJSX,"selMediaStorePolicy_RuleKeyOp"]);
  cmpBtnAddMediaStoreRuleKey.RHFilterRuleKeyBlkJSX = getJsxByName([BlkJSX,"selMediaStorePolicy_RuleKeyRHFilter"]);
  cmpBtnAddMediaStoreRuleKey.RHTagRuleKeyBlkJSX = cmpSelRuleKeyRHTag.BlkJSX;
  cmpBtnAddMediaStoreRuleKey.mediaStoreRuleKeyMatrix = cmpBlkMediaStoreRuleKeyMatrix.BlkJSX;
       
  var cmpBtnEditMediaStoreAdminClose = new CmpButton([BlkJSX,"btnDialogCloseX"],
                                                       null, 
                                                       {action:"FNCCALL", 
                                                        context:null, 
                                                        fnc:function() {
                                                          dlgEditMediaStoreAdmin_Destroy();
                                                        }
                                                       });
            
  var cmpBtnEditMediaStoreAdminCancel = new CmpButton([BlkJSX,"btnMediaStoreAdminCancel"],
                                                       null, 
                                                       {action:"FNCCALL", 
                                                        context:null, 
                                                        fnc:function() {
                                                          dlgEditMediaStoreAdmin_Destroy();
                                                        }
                                                       });
            
  var cmpBtnEditMediaStoreAdminDone = new CmpButton([BlkJSX,"btnMediaStoreAdminDone"],
                                                     null, 
                                                     {action:"FNCCALL", 
                                                      context:null, 
                                                      fnc:function() {
                                                        cmpDlgEditMediaStoreAdminSetMediaStoreAdmin.call(this, MediaStoreAdminNID);
                                                      }
                                                     });
                                                                                         
  var cmpDlgEditMediaStoreAdmin = new CmpContainer([BlkJSX,"DialogMediaStoreAdmin"],
                                                    [ {cmp:cmpBtnEditMediaStoreAdminClose},
                                                      {cmp:cmpBtnEditMediaStoreAdminCancel},
                                                      {cmp:cmpBtnEditMediaStoreAdminDone},
                                                      {cmp:cmpBlkMediaStoreFilterMatrix},
                                                      {cmp:cmpBlkMediaStoreTargetFilterMatrix},
                                                      {cmp:cmpBlkMediaStoreRuleKeyMatrix},
                                                      {cmp:cmpSelUserFilter},
                                                      {cmp:cmpSelResourceFilter},
                                                      {cmp:cmpSelTargetFilter},
                                                      {cmp:cmpSelRuleKeyLHTag},
                                                      {cmp:cmpSelRuleKeyRHTag},
                                                      {cmp:cmpBtnAddMediaStoreFilter},
                                                      {cmp:cmpBtnAddMediaStoreTargetFilter},
                                                      {cmp:cmpBtnAddMediaStoreRuleKey},
                                                      {cmp:cmpCDFMediaStorePolicy}
                                                    ],
                                                    [{action:"TOPICEVENT", topic:tDlg, eventvar:"EditMediaStoreAdminDataState", eventval:MediaStoreAdminNID}]);                            
                      
  cmpDlgEditMediaStoreAdmin.BlkJSX = BlkJSX;      
  cmpDlgEditMediaStoreAdmin.ObjectNID = MediaStoreAdminNID;   
  cmpDlgEditMediaStoreAdmin.getBlkData =  cmpDlgEditMediaStoreAdminGetMediaStoreAdmin;
  
  // init and repaint happens after the dialog is loaded
  jsx3.sleep( (function(){
                 this.init();
                 this.getBlkData(this.ObjectNID);
                 }
               ),
               null, cmpDlgEditMediaStoreAdmin );  
                   
  // fnc to get dlg data
  function cmpDlgEditMediaStoreAdminGetMediaStoreAdmin(MediaStoreAdminNID) { 
    var MediaStoreAdminNode = topic_GetNode(tCache, "//*[@NID='" + MediaStoreAdminNID + "']");
    if( MediaStoreAdminNode == null ) {
      var reqData = new ReqGetObjectNID(MediaStoreAdminNID, "MediaStoreAdminPolicyRule",
                                         {action:"FNCCALL", context:this, 
                                           fnc:function(respCode, respVal) {
                                                 if(respCode == "SUCCESS" && respVal != null) {
                                                   cmpDlgEditMediaStoreAdminGetMediaStoreAdmin(MediaStoreAdminNID);
                                                 } else {
                                                   var codeErrorMsg = CommonErrorCodeArr[respCode].errorDesc;
                                                   var ErrorMsg = "Error: MediaStoreAdmin policy get data request failed \n";
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
      //cmpDlgEditMediaStoreAdmin.setMapRuleData("MediaStoreAdmin", "IN", "NID", MediaStoreAdminNID);
      //topic_Publish(tDlg, "EditMediaStoreAdminDataState", MediaStoreAdminNID);

      // using the old repaint of the dialog
      DisplayEditValuesForMediaStoreAdmin(MediaStoreAdminNID);
    }
  }
       
  // fnc to set dlg data - runs in the context of the OK button
  function cmpDlgEditMediaStoreAdminSetMediaStoreAdmin(MediaStoreAdminNID) {
    // validate input 
    if( cmpDlgEditMediaStoreAdminValidate() != true ) return;
    // decide add or update
    var SetMsg = "Edit MediaStoreAdmin policy";
    var SetMode = "Replace";
    if( MediaStoreAdminNID == "DEFAULT_MEDIASTOREADMIN_NID" ) {
      SetMsg = "Add MediaStoreAdmin policy";
      SetMode = "Add";
    }

    //function to create role action CDF document. Used in the mapping rule
    createMediaStoreRoleDoc();

    // get MediaStoreAdmin node info from UI
    var MediaStoreAdminNode = null;

    // write the data back to the CDF
    cmpCDFMediaStorePolicy.BlkJSX.write();

    var strXMLCacheId = cmpCDFMediaStorePolicy.BlkJSX.getXMLId();
    var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);      
    var xslfile = PNameSpace.loadInclude("JSXAPPS/IPVS_AdminUI/xsl/MediaStorePolicyRequest.xsl","MediaStorePolicyRequest","xsl");
    var xslDoc = jsx3.xml.XslDocument.wrap(xslfile);
    var mediaStorePolicyConfigXML = xslDoc.transform(objXML);
    MediaStoreAdminNode = new jsx3.xml.Document();
    MediaStoreAdminNode.loadXML(mediaStorePolicyConfigXML);

    var strXMLCacheId = cmpBlkMediaStoreFilterMatrix.BlkJSX.getXMLId();
    var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
    var xslfile = PNameSpace.loadInclude("jsxuser:///JSXAPPS/IPVS_AdminUI/xsl/CDFToPolicyUserFilter.xsl","ServiceOperatorFilterSetNode","xsl");
    var xslDoc = jsx3.xml.XslDocument.wrap(xslfile);
    var newServiceOperatorFilterSetXML = xslDoc.transform(objXML);        
    var newServiceOperatorFilterSetNode = new jsx3.xml.Document();
    newServiceOperatorFilterSetNode.loadXML(newServiceOperatorFilterSetXML);
    
    var oldServiceOperatorFilterSetNode = MediaStoreAdminNode.selectSingleNode(".//ServiceOperatorFilterSet");              
    var serviceOperatorFilterSetParentNode = MediaStoreAdminNode.selectSingleNode(".//ServiceOperatorFilterSet").getParent();
    serviceOperatorFilterSetParentNode.replaceNode(newServiceOperatorFilterSetNode,oldServiceOperatorFilterSetNode);

    var mediaStoreRoleSetNode = PNameSpace.getCache().getDocument("MediaStoreAdminRoleSetDoc");
    var xslfile = PNameSpace.loadInclude("jsxuser:///JSXAPPS/IPVS_AdminUI/xsl/MediaStoreRoleSet.xsl","MediaStoreRoleSetNode","xsl");
    var xslDoc = jsx3.xml.XslDocument.wrap(xslfile);
    var newMediaStoreRoleSetXML = xslDoc.transform(mediaStoreRoleSetNode);        
    var newMediaStoreRoleSetNode = new jsx3.xml.Document();
    newMediaStoreRoleSetNode.loadXML(newMediaStoreRoleSetXML);

    var oldMediaStoreRoleSetNode = MediaStoreAdminNode.selectSingleNode(".//MediaStoreAdminRoleSet");              
    var mediaStoreRoleSetParentNode = MediaStoreAdminNode.selectSingleNode(".//MediaStoreAdminRoleSet").getParent();
    mediaStoreRoleSetParentNode.replaceNode(newMediaStoreRoleSetNode,oldMediaStoreRoleSetNode);

    var strXMLCacheId = cmpBlkMediaStoreTargetFilterMatrix.BlkJSX.getXMLId();
    var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
    var xslfile = PNameSpace.loadInclude("jsxuser:///JSXAPPS/IPVS_AdminUI/xsl/CDFToMediaStorePolicyTargetFilter.xsl","MediaStoreObjectFilterSetNode","xsl");
    var xslDoc = jsx3.xml.XslDocument.wrap(xslfile);
    var newMediaStoreObjectFilterSetXML = xslDoc.transform(objXML);        
    var newMediaStoreObjectFilterSetNode = new jsx3.xml.Document();
    newMediaStoreObjectFilterSetNode.loadXML(newMediaStoreObjectFilterSetXML);
    
    var oldMediaStoreObjectFilterSetNode = MediaStoreAdminNode.selectSingleNode(".//MediaStoreAdminObjectFilterSet");              
    var mediaStoreObjectFilterSetParentNode = MediaStoreAdminNode.selectSingleNode(".//MediaStoreAdminObjectFilterSet").getParent();
    mediaStoreObjectFilterSetParentNode.replaceNode(newMediaStoreObjectFilterSetNode,oldMediaStoreObjectFilterSetNode);

    var strXMLCacheId = cmpBlkMediaStoreRuleKeyMatrix.BlkJSX.getXMLId();
    var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
    var xslfile = PNameSpace.loadInclude("jsxuser:///JSXAPPS/IPVS_AdminUI/xsl/CDFToPolicyRuleKeyFilter.xsl","RuleKeyTagFilterSetNode","xsl");
    var xslDoc = jsx3.xml.XslDocument.wrap(xslfile);
    var newRuleKeyTagFilterSetXML = xslDoc.transform(objXML);        
    var newRuleKeyTagFilterSetNode = new jsx3.xml.Document();
    newRuleKeyTagFilterSetNode.loadXML(newRuleKeyTagFilterSetXML);
    
    var oldRuleKeyTagFilterSetNode = MediaStoreAdminNode.selectSingleNode(".//RuleKeyTagFilterSet");              
    var ruleKeyTagFilterSetParentNode = MediaStoreAdminNode.selectSingleNode(".//RuleKeyTagFilterSet").getParent();
    ruleKeyTagFilterSetParentNode.replaceNode(newRuleKeyTagFilterSetNode,oldRuleKeyTagFilterSetNode);

    if( MediaStoreAdminNode == null ) return;
    // set NID in MediaStoreAdmin node if replace mode
    if( SetMode == "Replace" ) {
      MediaStoreAdminNode.setAttribute("NID", MediaStoreAdminNID);
    }

    // do set request for MediaStoreAdmin              
    var reqSetMediaStoreAdminObject = new ReqSetObject(SetMode, "MediaStoreAdminPolicyRule", MediaStoreAdminNode, 
                                                        {action:"FNCCALL", context:this, 
                                                           fnc:function(respCode, respVal) {
                                                                 if(respCode == "SUCCESS" && respVal != null) {
                                                                   topic_Publish(tApp, "MediaStoreAdminPolicyRuleDataUpdate", "ADD");
                                                                   dlgEditMediaStoreAdmin_Destroy();
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
    if( reqSetMediaStoreAdminObject != null) {
      reqSetMediaStoreAdminObject.sendRequest(this);
    }
  } 
      
  function cmpDlgEditMediaStoreAdminValidate() {
    var valMediaStorePolicyName = getJsxByName([BlkJSX,"txtMediaStorePolicyName"]).getValue();
    var valMediaStorePolicyOrder = getJsxByName([BlkJSX,"txtMediaStorePolicyOrder"]).getValue();

    var policyName = checkValue(valMediaStorePolicyName , [BlkJSX,"txtMediaStorePolicyName"] , "Please enter policy name");
    if(!policyName){
      return false;
    }
    var checkOrder = checkValue(valMediaStorePolicyOrder , [BlkJSX,"txtMediaStorePolicyOrder"], "Please enter policy order" );
    if(!checkOrder){
      return false;
    }
    var chkDuplicateName = "";
    var chkDuplicatePriority = "";
    var chkPermission = "";
    var dialogText = windowBarBlkJSX.getText();

    var matrixToUse = "mtxpolicyMediaStoreList";
    if(dialogText == "Add Manage Media Policy" || dialogText == "Edit Manage Media Policy"){
      var ckhBoxArray = new Array();
      ckhBoxArray[0]= "chkMediaStoreDirPolicyRoleModify";
      ckhBoxArray[1]= "chkMediaStoreDirPolicyRoleDelete";
      ckhBoxArray[2]= "chkMediaStoreDirPolicyRoleImport";
      ckhBoxArray[0]= "chkMediaStoreFilePolicyRoleModify";
      ckhBoxArray[1]= "chkMediaStoreFilePolicyRoleDelete";
      ckhBoxArray[2]= "chkMediaStoreFilePolicyRoleExport";
      chkPermission = checkPermissionSelect(BlkJSX, "Please select Import, Export, Modify or Delete Access Permissions",ckhBoxArray);
      if(!chkPermission) return chkPermission;
    }
    else if (dialogText == "Add Playback Files Policy" || dialogText == "Edit Playback Files Policy") {
      matrixToUse = "mtxpolicyMediaAccessList";
    }
    else if (dialogText == "Add Record/PVR To Directories Policy" || dialogText == "Edit Record/PVR To Directories Policy") {
      matrixToUse = "mtxpolicyCreateRecordingsList";
      var ckhBoxArray = new Array();
      ckhBoxArray[0]= "chkMediaStoreDirPolicyRoleRecord";
      ckhBoxArray[1]= "chkMediaStoreDirPolicyRolePVR";
      chkPermission = checkPermissionSelect(BlkJSX, "Please select Record or PVR Access Permissions",ckhBoxArray);
      if(!chkPermission) return chkPermission;
    }
    
    if( MediaStoreAdminNID == "DEFAULT_MEDIASTOREADMIN_NID" ) {
      if(policyName) {
        chkDuplicateName = chkForNameExistence(valMediaStorePolicyName, [BlkJSX, "txtMediaStorePolicyName"], matrixToUse, "Duplicate Policy name. Please enter some other Policy name");
        if(!chkDuplicateName){
          checkOrder = false;
          return false;
        }
      }
      if(chkDuplicateName) {
        checkOrder = checkIntergerValue(valMediaStorePolicyOrder , [BlkJSX,"txtMediaStorePolicyOrder"], "Please enter an integer policy order" );
      }
      if(checkOrder) {
        chkDuplicatePriority = chkForOrderExistence(valMediaStorePolicyOrder, [BlkJSX, "txtMediaStorePolicyOrder"], matrixToUse, "Policy Order already exists");
      }
      if(chkDuplicatePriority) {
        return true;
      }    
    } else {
      checkOrder = checkIntergerValue(valMediaStorePolicyOrder , [BlkJSX,"txtMediaStorePolicyOrder"], "Please enter an integer policy order" );
      if(checkOrder) {
        if( orgOrderNumber != valMediaStorePolicyOrder) {
          chkDuplicatePriority = chkForOrderExistence(valMediaStorePolicyOrder, [BlkJSX, "txtMediaStorePolicyOrder"], matrixToUse, "Policy Order already exists");
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

  function DisplayEditValuesForMediaStoreAdmin(MediaStoreAdminNID) {
    var mediaStoreAdminNode = topic_GetNode(tCache, ".//*[@NID='" + MediaStoreAdminNID + "']");

    /*var version = mediaStoreAdminNode.getAttribute("Ver");
    var versionBlkJSX = getJsxByName([BlkJSX,"txtPolicyMediaStoreXMLVersion"]);
    versionBlkJSX.setValue(version);
    
    var revision = mediaStoreAdminNode.getAttribute("Rev");
    var revisionBlkJSX = getJsxByName([BlkJSX,"txtPolicyMediaStoreXMLRevision"]);
    revisionBlkJSX.setValue(revision);    

    var serviceVer = mediaStoreAdminNode.getAttribute("serviceVer");
    var serviceVerBlkJSX = getJsxByName([BlkJSX,"txtPolicyMediaStoreXMLServiceVersion"]);
    serviceVerBlkJSX.setValue(serviceVer);    

    var jsxtext = mediaStoreAdminNode.selectSingleNode(".//rulename").getValue();
    var filterNameBlkJSX = getJsxByName([BlkJSX,"txtMediaStorePolicyName"]);
    filterNameBlkJSX.setValue(jsxtext);
    
    var index = mediaStoreAdminNode.selectSingleNode(".//order").getValue();
    var filterOrderBlkJSX = getJsxByName([BlkJSX,"txtMediaStorePolicyOrder"]);
    filterOrderBlkJSX.setValue(index);     

    //
    var policyValid = mediaStoreAdminNode.selectSingleNode(".//valid").getValue();
    var filterActiveBlkJSX = getJsxByName([BlkJSX,"chkMediaStorePolicyActive"]);
    if(policyValid == "true") {
      filterActiveBlkJSX.setChecked(jsx3.gui.CheckBox.CHECKED);
    } else {
      filterActiveBlkJSX.setChecked(jsx3.gui.CheckBox.UNCHECKED);
    }

    //
    var jsxUserPolicyType = mediaStoreAdminNode.selectSingleNode(".//allow").getValue();
    var filterTypeAllowBlkJSX = getJsxByName([BlkJSX,"radioMediaStorePolicyTypeAllow"]);
    filterTypeAllowBlkJSX.setSelected(jsx3.gui.RadioButton.SELECTED);
//    var filterTypeDenyBlkJSX = getJsxByName([BlkJSX,"radioMediaStorePolicyTypeDeny"]);
//    if(jsxUserPolicyType == "true") {
//      filterTypeAllowBlkJSX.setSelected(jsx3.gui.RadioButton.SELECTED);
//      filterTypeDenyBlkJSX.setSelected(jsx3.gui.RadioButton.UNSELECTED);
//    } else {
//      filterTypeAllowBlkJSX.setSelected(jsx3.gui.RadioButton.UNSELECTED);
//      filterTypeDenyBlkJSX.setSelected(jsx3.gui.RadioButton.SELECTED);
//    }

    //
    var xAuthRuleListIter  = mediaStoreAdminNode.selectNodeIterator(".//ExternalCondition/XAuth/RuleKey");
    while( xAuthRuleListIter.hasNext() ) {
      var xAuthRuleListIterNode = xAuthRuleListIter.next();
      var conditionAction = xAuthRuleListIterNode.getValue();

      if(conditionAction == "MEDIAROOM_RULE_CONFCREATE_USER") {
        var filterxAuthUserBlkJSX = getJsxByName([BlkJSX,"chkMediaStorePolicy_XAuthUseServiceOperator"]);
        filterxAuthUserBlkJSX.setChecked(jsx3.gui.CheckBox.CHECKED);        
      }
    }

    //
    var xAuthServiceOpListIter  = mediaStoreAdminNode.selectNodeIterator(".//ExternalCondition/XAuth/ServiceXAuthOperatorFilterSet/ServiceOperatorFilter");
    var mtxMediaStorePolicy_XAuthUserListBlkJSX = getJsxByName([BlkJSX,"mtxMediaStorePolicy_XAuthUserList"]);
    mtxMediaStorePolicy_XAuthUserListBlkJSX.resetCacheData();
    while( xAuthServiceOpListIter.hasNext() ) {
      var xAuthServiceOpListIterNode = xAuthServiceOpListIter.next();
      var UserFilterNID = xAuthServiceOpListIterNode.selectSingleNode(".//UserFilterNID").getValue();
      var ResourceFilterNID = xAuthServiceOpListIterNode.selectSingleNode(".//ResourceFilterNID").getValue();
      if( (UserFilterNID != "" ) && ( ResourceFilterNID != "") ) {
        var UserFilterName = getTagObjectFilterName(UserFilterNID);
        var ResourceFilterName = getTagObjectFilterName(ResourceFilterNID);
    
        var o = new Object();
        o.jsxid = jsx3.xml.CDF.getKey();
        o.jsxtextOne = UserFilterName ;
        o.jsxtextOneNID = UserFilterNID ;
        o.jsxtextTwo = ResourceFilterName ;
        o.jsxtextTwoNID = ResourceFilterNID ;
        o.jsxAt = "@";

        mtxMediaStorePolicy_XAuthUserListBlkJSX.insertRecord(o, null, false);
      }
    }
    mtxMediaStorePolicy_XAuthUserListBlkJSX.repaintData();

    // hide the action block for these policies
    if (PolicyType == "MediaAccessPolicy" || PolicyType == "RecordingAccessPolicy") {
      var titleBlock = getJsxByName([BlkJSX, "blkMediaStoreActionSectionTitle"]);
      var actionBlock = getJsxByName([BlkJSX, "blkMediaStorePolicyActions"]);
      var actionDirBlock = getJsxByName([BlkJSX, "blkMediaStoreDirPolicyActions"]);
      var recordPVRBlock = getJsxByName([BlkJSX, "blkMediaStorePolicyRecordPVRActions"]);
      titleBlock.setDisplay(jsx3.gui.Block.DISPLAYNONE, true);
      actionBlock.setDisplay(jsx3.gui.Block.DISPLAYNONE, true);
      actionDirBlock.setDisplay(jsx3.gui.Block.DISPLAYNONE, true);
      recordPVRBlock.setDisplay(jsx3.gui.Block.DISPLAYNONE, true);
      if(PolicyType == "RecordingAccessPolicy") {
        recordPVRBlock.setDisplay(jsx3.gui.Block.DISPLAYBLOCK, true);
        var roleListIter = mediaStoreAdminNode.selectNodeIterator(".//MediaStoreAdminRoleSet/MediaStoreAdminObjectRole[.//RoleObject='MediaStreamDirResource']");
        while (roleListIter.hasNext()) {
          var roleListIterNode = roleListIter.next();
          var roleObject = roleListIterNode.selectSingleNode(".//RoleObject").getValue();
          var roleAction = roleListIterNode.selectSingleNode(".//RoleAction").getValue();
          if (roleAction == "Record") {
            getJsxByName([BlkJSX, "chkMediaStoreDirPolicyRoleRecord"]).setChecked(jsx3.gui.CheckBox.CHECKED);
          }

          if (roleAction == "PVR") {
            getJsxByName([BlkJSX, "chkMediaStoreDirPolicyRolePVR"]).setChecked(jsx3.gui.CheckBox.CHECKED);
          }
        }
      }
    }
    else {
      // get the MediaStreamFileResource Role RoleObjects and set the check boxes
      var roleListIter = mediaStoreAdminNode.selectNodeIterator(".//MediaStoreAdminRoleSet/MediaStoreAdminObjectRole[.//RoleObject='MediaStreamFileResource']");
      while (roleListIter.hasNext()) {
        var roleListIterNode = roleListIter.next();
        var roleObject = roleListIterNode.selectSingleNode(".//RoleObject").getValue();
        var roleAction = roleListIterNode.selectSingleNode(".//RoleAction").getValue();

        if (roleAction == "Modify") {
          getJsxByName([BlkJSX, "chkMediaStoreFilePolicyRoleModify"]).setChecked(jsx3.gui.CheckBox.CHECKED);
        }

        if (roleAction == "Delete") {
          getJsxByName([BlkJSX, "chkMediaStoreFilePolicyRoleDelete"]).setChecked(jsx3.gui.CheckBox.CHECKED);
        }

        if (roleAction == "Export") {
          getJsxByName([BlkJSX, "chkMediaStoreFilePolicyRoleExport"]).setChecked(jsx3.gui.CheckBox.CHECKED);
        }
      }

      //get the MediaStreamDirResource Role RoleObjects and set the check boxes
      var roleListIter = mediaStoreAdminNode.selectNodeIterator(".//MediaStoreAdminRoleSet/MediaStoreAdminObjectRole[.//RoleObject='MediaStreamDirResource']");
      while (roleListIter.hasNext()) {
        var roleListIterNode = roleListIter.next();
        var roleObject = roleListIterNode.selectSingleNode(".//RoleObject").getValue();
        var roleAction = roleListIterNode.selectSingleNode(".//RoleAction").getValue();

        if (roleAction == "Modify") {
          getJsxByName([BlkJSX, "chkMediaStoreDirPolicyRoleModify"]).setChecked(jsx3.gui.CheckBox.CHECKED);
        }

        if (roleAction == "Delete") {
          getJsxByName([BlkJSX, "chkMediaStoreDirPolicyRoleDelete"]).setChecked(jsx3.gui.CheckBox.CHECKED);
        }

        if (roleAction == "Import") {
          getJsxByName([BlkJSX, "chkMediaStoreDirPolicyRoleImport"]).setChecked(jsx3.gui.CheckBox.CHECKED);
        }
      }
    }*/

    // hide the action block for these policies
    if (PolicyType == "MediaAccessPolicy" || PolicyType == "RecordingAccessPolicy") {
      var titleBlock = getJsxByName([BlkJSX, "blkMediaStoreActionSectionTitle"]);
      var actionBlock = getJsxByName([BlkJSX, "blkMediaStorePolicyActions"]);
      var actionDirBlock = getJsxByName([BlkJSX, "blkMediaStoreDirPolicyActions"]);
      var recordPVRBlock = getJsxByName([BlkJSX, "blkMediaStorePolicyRecordPVRActions"]);
      titleBlock.setDisplay(jsx3.gui.Block.DISPLAYNONE, true);
      actionBlock.setDisplay(jsx3.gui.Block.DISPLAYNONE, true);
      actionDirBlock.setDisplay(jsx3.gui.Block.DISPLAYNONE, true);
      recordPVRBlock.setDisplay(jsx3.gui.Block.DISPLAYNONE, true);
      if(PolicyType == "RecordingAccessPolicy") {
        recordPVRBlock.setDisplay(jsx3.gui.Block.DISPLAYBLOCK, true);
      }
    }

    var mediaStoreNID = mediaStoreAdminNode.getAttribute("NID");
    var strXMLCacheId = cmpCDFMediaStorePolicy.BlkJSX.getXMLId();
    var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
    objXML.loadXML(mediaStoreAdminNode.toString());
    cmpCDFMediaStorePolicy.BlkJSX.setSourceXML(objXML);         
    cmpCDFMediaStorePolicy.BlkJSX.setCDFId(mediaStoreNID);

    var serviceOpListIter  = mediaStoreAdminNode.selectNodeIterator(".//ServiceOperatorFilterSet/ServiceOperatorFilter");
    var objDoc = jsx3.xml.CDF.Document.newDocument();
    while( serviceOpListIter.hasNext() ) {
      var serviceOpListIterNode = serviceOpListIter.next();
      var UserFilterNID = serviceOpListIterNode.selectSingleNode(".//UserFilterNID").getValue();
      var ResourceFilterNID = serviceOpListIterNode.selectSingleNode(".//ResourceFilterNID").getValue();
      if( (UserFilterNID != "") && (ResourceFilterNID != "") ) {
        var UserFilterName = getTagObjectFilterName(UserFilterNID);
        var ResourceFilterName = getTagObjectFilterName(ResourceFilterNID);

        var o = new Object();
        o.jsxid = jsx3.xml.CDF.getKey(); 
        o.UserFilterName = UserFilterName;
        o.UserFilterNID = UserFilterNID;
        o.ResourceFilterName = ResourceFilterName;
        o.ResourceFilterNID = ResourceFilterNID;
        objDoc.insertRecord(o);
      }
    }
    var strXMLCacheId = cmpBlkMediaStoreFilterMatrix.BlkJSX.getXMLId();
    var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
    objXML.loadXML(objDoc.toString());
    cmpBlkMediaStoreFilterMatrix.BlkJSX.setSourceXML(objXML);
    cmpBlkMediaStoreFilterMatrix.BlkJSX.repaintData();

    var targetFilterListIter  = mediaStoreAdminNode.selectNodeIterator(".//MediaStoreAdminObjectFilterSet/MediaStoreAdminObjectFilterNID");
    var objTargetDoc = jsx3.xml.CDF.Document.newDocument();
    while( targetFilterListIter.hasNext() ) {
      var targetFilterListIterNode = targetFilterListIter.next();
      var tareFilterNID = targetFilterListIterNode.getValue();
      if( tareFilterNID != "" ) {
        var targetFilterName = getTagObjectFilterName(tareFilterNID);
    
        var o = new Object();
        o.jsxid = jsx3.xml.CDF.getKey();
        o.targetFilterName = targetFilterName ;
        o.tareFilterNID = tareFilterNID ;
        objTargetDoc.insertRecord(o);
      }
    }
    var strXMLCacheId = cmpBlkMediaStoreTargetFilterMatrix.BlkJSX.getXMLId();
    var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
    objXML.loadXML(objTargetDoc.toString());
    cmpBlkMediaStoreTargetFilterMatrix.BlkJSX.setSourceXML(objXML);
    cmpBlkMediaStoreTargetFilterMatrix.BlkJSX.repaintData();

    //
    var ruleKeyListIter  = mediaStoreAdminNode.selectNodeIterator(".//RuleKeyTagFilterSet/RuleKeyTagFilter");
    var objRuleKeyDoc = jsx3.xml.CDF.Document.newDocument();
    while( ruleKeyListIter.hasNext() ) {
      var ruleKeyListIterNode = ruleKeyListIter.next();
      var LHName = ruleKeyListIterNode.selectSingleNode(".//RuleKeyLHTagName/RuleKey").getValue();
      var LHTagName = ruleKeyListIterNode.selectSingleNode(".//RuleKeyLHTagName/TagName").getValue();
      var Operator = ruleKeyListIterNode.selectSingleNode(".//MatchOp").getValue();
      var RHName = ruleKeyListIterNode.selectSingleNode(".//RuleKeyRHTagName/RuleKey").getValue();
      var RHTagName = ruleKeyListIterNode.selectSingleNode(".//RuleKeyRHTagName/TagName").getValue();
      if( LHTagName != "") {
        var o = new Object();
        o.jsxid = jsx3.xml.CDF.getKey();
        o.LHName = LHName;
        o.LHTagName = splitTagName(LHTagName);
        o.Operator = Operator;
        o.RHName = RHName;
        o.RHTagName = splitTagName(RHTagName);
        objRuleKeyDoc.insertRecord(o);
      }
    }
    var strXMLCacheId = cmpBlkMediaStoreRuleKeyMatrix.BlkJSX.getXMLId();
    var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
    objXML.loadXML(objRuleKeyDoc.toString());
    cmpBlkMediaStoreRuleKeyMatrix.BlkJSX.setSourceXML(objXML);
    cmpBlkMediaStoreRuleKeyMatrix.BlkJSX.repaintData();
          
    createMediaStoreRoleDoc();

    // populate the select with the default values
    updateSelectTagObjectFilters();
    
    // populate the select rule key select with the default value
    updateSelectGroupTag();
    
    if( MediaStoreAdminNID != "DEFAULT_MEDIASTOREADMIN_NID" ) {
      getJsxByName([BlkJSX,"txtMediaStorePolicyName"]).setEnabled(jsx3.gui.Form.STATEDISABLED, true); 
    }
    
    orgOrderNumber = getJsxByName([BlkJSX,"txtMediaStorePolicyOrder"]).getValue();
    getJsxByName([BlkJSX,"txtMediaStorePolicyName"]).focus();
  }

  function updateSelectTagObjectFilters() {
    //getFilterToSelectElement([BlkJSX,"selMediaStorePolicy_XAuthUserFilter"],"User");
    //getFilterToSelectElement([BlkJSX,"selMediaStorePolicy_XAuthResourceFilter"],"Resource");
    getFilterToSelectElement([BlkJSX,"selMediaStorePolicy_UserFilter"],"User");
    getFilterToSelectElement([BlkJSX,"selMediaStorePolicy_ResourceFilter"],"Resource");
    getFilterToSelectElement([BlkJSX,"selMediaStorePolicyFilter_Target"],"ResourceProfile");
  }

  function updateSelectGroupTag() {
    getTagDefToSelectElement([BlkJSX,"selMediaStorePolicy_RuleKeyLHTag"],"ALL");
    getTagDefToSelectElement([BlkJSX,"selMediaStorePolicy_RuleKeyRHTag"],"ALL");
  }

  //create the roleset object doc
  function createMediaStoreRoleDoc() {
    if (PolicyType == "MediaAccessPolicy") {
      createMediaAccessPolicyRoleDoc();
    }
    else if (PolicyType == "RecordingAccessPolicy") {
      createRecordingAccessPolicyRoleDoc();
    }
    else {
      createMediaStorePolicyRoleDoc();
    }
  }

  function createMediaAccessPolicyRoleDoc() {
    var RoleObjectArr = new Array();
    RoleObjectArr[0] = "MediaStreamFileResource";
    RoleObjectArr[1] = "MediaStreamDirResource";

    var roleCacheDoc = PNameSpace.getCache().getDocument("MediaStoreAdminRoleSetDoc");
    if (roleCacheDoc) {
      delete roleCacheDoc;
    }

    var MediaStoreAdminRoleSet = new jsx3.xml.CDF.Document.newDocument();
    PNameSpace.Cache.setDocument("MediaStoreAdminRoleSetDoc", MediaStoreAdminRoleSet);

    for (roleObject = 0; roleObject < RoleObjectArr.length; roleObject++) {
      var o = new Object();
      o.jsxid = jsx3.xml.CDF.getKey();
      o.jsxroleObject = RoleObjectArr[roleObject];
      o.jsxroleAction = "Read";
      MediaStoreAdminRoleSet.insertRecord(o, null, false);
    }

    var o = new Object();
    o.jsxid = jsx3.xml.CDF.getKey();
    o.jsxroleObject = RoleObjectArr[0];
    o.jsxroleAction = "Playback";
    MediaStoreAdminRoleSet.insertRecord(o, null, false);

  }

  function createRecordingAccessPolicyRoleDoc() {
    var RoleObjectArr = new Array();
    RoleObjectArr[0] = "MediaStreamDirResource";

    var roleCacheDoc = PNameSpace.getCache().getDocument("MediaStoreAdminRoleSetDoc");
    if (roleCacheDoc) {
      delete roleCacheDoc;
    }

    var MediaStoreAdminRoleSet = new jsx3.xml.CDF.Document.newDocument();
    PNameSpace.Cache.setDocument("MediaStoreAdminRoleSetDoc", MediaStoreAdminRoleSet);

    for (roleObject = 0; roleObject < RoleObjectArr.length; roleObject++) {
      var roleDirRecord = getJsxByName([BlkJSX, "chkMediaStoreDirPolicyRoleRecord"]).getChecked();
      var roleDirPVR = getJsxByName([BlkJSX, "chkMediaStoreDirPolicyRolePVR"]).getChecked();

      if (roleDirRecord) {
        var o = new Object();
        o.jsxid = jsx3.xml.CDF.getKey();
        o.jsxroleObject = RoleObjectArr[0];
        o.jsxroleAction = "Record";
        MediaStoreAdminRoleSet.insertRecord(o, null, false);
      }
      if (roleDirPVR) {
        var o = new Object();
        o.jsxid = jsx3.xml.CDF.getKey();
        o.jsxroleObject = RoleObjectArr[0];
        o.jsxroleAction = "PVR";
        MediaStoreAdminRoleSet.insertRecord(o, null, false);
      }

      var o = new Object();
      o.jsxid = jsx3.xml.CDF.getKey();
      o.jsxroleObject = RoleObjectArr[0];
      o.jsxroleAction = "Read";
      MediaStoreAdminRoleSet.insertRecord(o, null, false);
    }

  }

  function createMediaStorePolicyRoleDoc() {
    var RoleObjectArr = new Array();
    RoleObjectArr[0] = "MediaStreamFileResource"; 
    RoleObjectArr[1] = "MediaStreamDirResource";

    var roleCacheDoc = PNameSpace.getCache().getDocument("MediaStoreAdminRoleSetDoc");
    if(roleCacheDoc ) {
      delete roleCacheDoc;
    }
          
    var MediaStoreAdminRoleSet = new jsx3.xml.CDF.Document.newDocument();
    PNameSpace.Cache.setDocument("MediaStoreAdminRoleSetDoc", MediaStoreAdminRoleSet);

    updateReadPolicy(BlkJSX, "chkMediaStoreFilePolicyRoleModify", "chkMediaStoreFilePolicyRoleDelete", "chkMediaStoreFilePolicyRoleExport", "chkMediaStoreFilePolicyRoleView");
    var roleFileModify = getJsxByName([BlkJSX, "chkMediaStoreFilePolicyRoleModify"]).getChecked();
    var roleFileDelete = getJsxByName([BlkJSX, "chkMediaStoreFilePolicyRoleDelete"]).getChecked();
    var roleFileView = getJsxByName([BlkJSX, "chkMediaStoreFilePolicyRoleView"]).getChecked();
    var roleFileExport = getJsxByName([BlkJSX, "chkMediaStoreFilePolicyRoleExport"]).getChecked();
    
    updateReadPolicy(BlkJSX, "chkMediaStoreDirPolicyRoleModify", "chkMediaStoreDirPolicyRoleDelete", "chkMediaStoreDirPolicyRoleImport", "chkMediaStoreDirPolicyRoleView");
    var roleDirModify = getJsxByName([BlkJSX, "chkMediaStoreDirPolicyRoleModify"]).getChecked();
    var roleDirDelete = getJsxByName([BlkJSX, "chkMediaStoreDirPolicyRoleDelete"]).getChecked();
    var roleDirView = getJsxByName([BlkJSX, "chkMediaStoreDirPolicyRoleView"]).getChecked();
    var roleDirImport = getJsxByName([BlkJSX, "chkMediaStoreDirPolicyRoleImport"]).getChecked();

    if (roleFileModify) {
      var o = new Object();
      o.jsxid = jsx3.xml.CDF.getKey();
      o.jsxroleObject = RoleObjectArr[0];
      o.jsxroleAction = "Modify";
      MediaStoreAdminRoleSet.insertRecord(o, null, false);
    }
    if (roleFileDelete) {
      var o = new Object();
      o.jsxid = jsx3.xml.CDF.getKey();
      o.jsxroleObject = RoleObjectArr[0];
      o.jsxroleAction = "Delete";
      MediaStoreAdminRoleSet.insertRecord(o, null, false);
    }
    if (roleFileView) {
      var o = new Object();
      o.jsxid = jsx3.xml.CDF.getKey();
      o.jsxroleObject = RoleObjectArr[0];
      o.jsxroleAction = "Read";
      MediaStoreAdminRoleSet.insertRecord(o, null, false);
    }
    if (roleFileExport) {
      var o = new Object();
      o.jsxid = jsx3.xml.CDF.getKey();
      o.jsxroleObject = RoleObjectArr[0];
      o.jsxroleAction = "Export";
      MediaStoreAdminRoleSet.insertRecord(o, null, false);
    }

    if (roleDirModify) {
      var o = new Object();
      o.jsxid = jsx3.xml.CDF.getKey();
      o.jsxroleObject = RoleObjectArr[1];
      o.jsxroleAction = "Modify";
      MediaStoreAdminRoleSet.insertRecord(o, null, false);
    }
    if (roleDirDelete) {
      var o = new Object();
      o.jsxid = jsx3.xml.CDF.getKey();
      o.jsxroleObject = RoleObjectArr[1];
      o.jsxroleAction = "Delete";
      MediaStoreAdminRoleSet.insertRecord(o, null, false);
    }
    if (roleDirView || roleFileView) {
      var o = new Object();
      o.jsxid = jsx3.xml.CDF.getKey();
      o.jsxroleObject = RoleObjectArr[1];
      o.jsxroleAction = "Read";
      MediaStoreAdminRoleSet.insertRecord(o, null, false);
    }
    if (roleDirImport) {
      var o = new Object();
      o.jsxid = jsx3.xml.CDF.getKey();
      o.jsxroleObject = RoleObjectArr[1];
      o.jsxroleAction = "Import";
      MediaStoreAdminRoleSet.insertRecord(o, null, false);
    }
    if (roleDirImport || roleFileExport) {
      var RoleArr = new Array();
      RoleArr[0] = "Add";
      RoleArr[1] = "Modify";
      RoleArr[2] = "Delete";
      RoleArr[3] = "Read";
      for (var role = 0; role < RoleArr.length; role++) {
        var o = new Object();
        o.jsxid = jsx3.xml.CDF.getKey();
        o.jsxroleObject = "ExportImportFileListItem";
        o.jsxroleAction = RoleArr[role]; ;
        MediaStoreAdminRoleSet.insertRecord(o, null, false);
      } 
    }

  }
  
}