// this code has some potential issues 
// - w.r.t object reference counts - when objects created in here go out of scope etc
// - loading sync issues - dlg may not be loaded before the object init methods are called

function dlgEditAssetAdmin_Create(AssetAdminNID, PolicyType) {

  // deal with defaults for add
  if( AssetAdminNID == null ) AssetAdminNID = "DEFAULT_ASSETADMIN_NID";
  
  // load view - dialog component file
  var DlgFile = "components/policy/newAssetPolicy.xml";
  var BlkJSX = PNameSpace.getBodyBlock().load(DlgFile);

  // setup a local topic for dlg specific messaging     
  var tDlg = "Dialog_" + jsx3.xml.CDF.getKey();
  topic_Create(tDlg);

  // setup a listner for it      
  var eventDlg = new EventListner(tDlg);
  var orgOrderNumber = "";  

  // setup a private/dlg specific app event handler
  dlgEditAssetAdmin_eventAppTagObject = new EventListner( tApp );
  dlgEditAssetAdmin_eventAppGroupTag = new EventListner(tApp);

  //set the right text for the dialog
  var dlgText = "Asset Policy";
  if (PolicyType == "PortAccessPolicy") {
    dlgText = "Source and Destination Ports Policy";
  }
  else if (PolicyType == "SessionAccessPolicy") {
    dlgText = "Configure Sessions Policy";
  }
  else if (PolicyType == "AssetAdminSessionPolicy") {
    dlgText = "Sessions Policy";
  }
  else if (PolicyType == "AssetAdminPolicy") {
    dlgText = "Manage Devices and Profiles Policy";
  }
  
  topic_Set(tDlg, "PolicyType", PolicyType);
  
  if( AssetAdminNID == "DEFAULT_ASSETADMIN_NID" ){
    dlgText = "Add " + dlgText;
  }
  else{
    dlgText = "Edit " + dlgText;
  }
  var windowBarBlkJSX = getJsxByName([BlkJSX, "windowBar"]);
  //set the text
  windowBarBlkJSX.setText(dlgText, true);
  closeDialogIfExists(dlgText);

  var cmpBlkAssetFilterMatrix = new CmpBlock([BlkJSX,"mtxAssetPolicy_AssetFilterSel"]);
  cmpBlkAssetFilterMatrix.BlkJSX.setXMLTransformers("xsl/PolicyUserFilterToCDF.xsl");
  cmpBlkAssetFilterMatrix.BlkJSX.setSelectionModel(jsx3.gui.Matrix.SELECTION_ROW);

  var cmpBlkAssetTargetFilterMatrix = new CmpBlock([BlkJSX,"mtxAssetPolicy_TargetFilterSel"]);
  cmpBlkAssetTargetFilterMatrix.BlkJSX.setXMLTransformers("xsl/PolicyTargetFilterToCDF.xsl");
  cmpBlkAssetTargetFilterMatrix.BlkJSX.setSelectionModel(jsx3.gui.Matrix.SELECTION_ROW);

  var cmpBlkAssetRuleKeyMatrix = new CmpBlock([BlkJSX,"mtxAssetPolicy_RuleKeySel"]);
  cmpBlkAssetRuleKeyMatrix.BlkJSX.setXMLTransformers("xsl/PolicyRuleKeyFilterToCDF.xsl");
  cmpBlkAssetRuleKeyMatrix.BlkJSX.setSelectionModel(jsx3.gui.Matrix.SELECTION_ROW);

  var cmpCDFAssetPolicy = new Component([BlkJSX,"cdfDlgAssetPolicy"]);
  cmpCDFAssetPolicy.BlkJSX.setXMLTransformers("xsl/AssetPolicy.xsl");

  //get all the actions blkjsx
  var titleBlock = getJsxByName([BlkJSX, "blkAssestPolicyActionSectionTitle"]);
  var actionBlock = getJsxByName([BlkJSX, "blkAssetPolicyActions"]);
  var srcPortAccessactionBlock = getJsxByName([BlkJSX, "blkAssetAdminSrcPortAccessActions"]);
  var dstPortAccessactionBlock = getJsxByName([BlkJSX, "blkAssetAdminDstPortAccessActions"]);
  var assetAdminSessionactionBlock = getJsxByName([BlkJSX, "blkAssetAdminSessionActions"]);
  
  //Hide the Actions block for these Access Policies
  if (PolicyType == "PortAccessPolicy") {
    actionBlock.setDisplay(jsx3.gui.Block.DISPLAYNONE, true);
    assetAdminSessionactionBlock.setDisplay(jsx3.gui.Block.DISPLAYNONE, true);
  }
  else if (PolicyType == "AssetAdminSessionPolicy") {
    actionBlock.setDisplay(jsx3.gui.Block.DISPLAYNONE, true);
    srcPortAccessactionBlock.setDisplay(jsx3.gui.Block.DISPLAYNONE, true);
    dstPortAccessactionBlock.setDisplay(jsx3.gui.Block.DISPLAYNONE, true);
  }
  else if (PolicyType == "SessionAccessPolicy" || PolicyType == "AssetAdminPolicy") {
    assetAdminSessionactionBlock.setDisplay(jsx3.gui.Block.DISPLAYNONE, true);
    srcPortAccessactionBlock.setDisplay(jsx3.gui.Block.DISPLAYNONE, true);
    dstPortAccessactionBlock.setDisplay(jsx3.gui.Block.DISPLAYNONE, true);
  }

  
  function dlgEditAssetAdmin_Destroy() {
    // destroy any local eventlistners
    eventDlg.destroy();
    dlgEditAssetAdmin_eventAppTagObject.destroy();
    dlgEditAssetAdmin_eventAppGroupTag.destroy();
    
    // destroy all local components
    cmpDlgEditAssetAdmin.destroy();
    
    // destroy the local topic
    topic_Destroy(tDlg);
    
    // close dialog
    BlkJSX.doClose();
  }
  
  // event subscribtion: if new tagobjectfilter is added from select box and update all the
  // select boxes in the dialog after add is finished
  dlgEditAssetAdmin_eventAppTagObject.subscribe("TagObjectFilterDataState", null, this,
                                                  function() {
                                                    updateSelectTagObjectFilters();
                                                  } );
  
  // event subscribtion: if new grouptag is added from select box and update all the
  // select boxes in the dialog after add is finished                      
  dlgEditAssetAdmin_eventAppGroupTag.subscribe("GroupTagDataState", null, this,
                                                function() {
                                                  updateSelectGroupTag();
                                                } );

  var cmpChkEditAssetAdminRoleStart = new CmpCheckBox([BlkJSX,"chkAssetPolicyRoleStart"],
                                                      null,
                                                      {action:"FNCCALL", 
                                                       context:null, 
                                                       fnc:function() {
                                                         createAssetRoleDoc();
                                                       }
                                                     });
                                                     
  var cmpChkEditAssetAdminRoleView = new CmpCheckBox([BlkJSX,"chkAssetPolicyRoleView"],
                                                      null,
                                                      {action:"FNCCALL", 
                                                       context:null, 
                                                       fnc:function() {
                                                         createAssetRoleDoc();
                                                       }
                                                     });
                                                   
  var cmpChkEditAssetAdminRoleDelete = new CmpCheckBox([BlkJSX,"chkAssetPolicyRoleDelete"],
                                                        null,
                                                        {action:"FNCCALL", 
                                                         context:null, 
                                                         fnc:function() {
                                                           createAssetRoleDoc();
                                                         }
                                                       });
                                                   
  var cmpChkEditAssetAdminRoleModify = new CmpCheckBox([BlkJSX,"chkAssetPolicyRoleModify"],
                                                        null,
                                                        {action:"FNCCALL", 
                                                         context:null, 
                                                         fnc:function() {
                                                           createAssetRoleDoc();
                                                         }
                                                       });
                                                   
  var cmpChkEditAssetAdminRoleAdd = new CmpCheckBox([BlkJSX,"chkAssetPolicyRoleAdd"],
                                                    null,
                                                    {action:"FNCCALL", 
                                                     context:null, 
                                                     fnc:function() {
                                                       createAssetRoleDoc();
                                                     }
                                                   });

  var cmpSelUserFilter = new CmpSelect([BlkJSX,"selAssetPolicy_UserFilter"],
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

  var cmpSelResourceFilter = new CmpSelect([BlkJSX,"selAssetPolicy_ResourceFilter"],
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

  var cmpBtnAddAssetFilter = new CmpButton([BlkJSX,"btnAssetPolicy_AddAssetFilter"],
                                                 null, 
                                                 {action:"FNCCALL", 
                                                  context:null, 
                                                  fnc:function() {
                                                      addDatatoMatrix(this.userFilterBlkJSX, this.resourceFilterBlkJSX, this.assetFilterMatrix);
                                                  }
                                                 });
  cmpBtnAddAssetFilter.userFilterBlkJSX = cmpSelUserFilter.BlkJSX;
  cmpBtnAddAssetFilter.resourceFilterBlkJSX = cmpSelResourceFilter.BlkJSX;
  cmpBtnAddAssetFilter.assetFilterMatrix = cmpBlkAssetFilterMatrix.BlkJSX;
        
  var cmpSelTargetFilter = new CmpSelect([BlkJSX,"selPolicyAssetFilter_Target"],
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
  
  var cmpBtnAddAssetTargetFilter = new CmpButton([BlkJSX,"btnAssetPolicy_AddTargetFilter"],
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
                                                        this.assetTargetFilterMatrix.insertRecord(o, null, false);
                                                      }
                                                      this.assetTargetFilterMatrix.repaintData();
                                                      this.targetFilterBlkJSX.setValue("Select ResourceProfile Filter");
                                                  }
                                                 });
  cmpBtnAddAssetTargetFilter.targetFilterBlkJSX = cmpSelTargetFilter.BlkJSX;
  cmpBtnAddAssetTargetFilter.assetTargetFilterMatrix = cmpBlkAssetTargetFilterMatrix.BlkJSX;
  
  var cmpSelRuleKeyLHTag = new CmpSelect([BlkJSX,"selAssetPolicy_RuleKeyLHTag"],
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
  var cmpSelRuleKeyRHTag = new CmpSelect([BlkJSX,"selAssetPolicy_RuleKeyRHTag"],
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

  var cmpBtnAddAssetRuleKey = new CmpButton([BlkJSX,"btnAddAssetPolicy_RuleKey"],
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
                                                          this.assetRuleKeyMatrix.insertRecord(o, null, false);
                                                        }
                                                      }
                                                      this.assetRuleKeyMatrix.repaintData();
                                                      this.LHFilterRuleKeyBlkJSX.setValue("Select LH Rule");
                                                      this.LHTagRuleKeyBlkJSX.setValue("Select Tag Name");
                                                      this.OperatorRuleKeyBlkJSX.setValue("Select Operation");
                                                      this.RHFilterRuleKeyBlkJSX.setValue("Select RH Rule");
                                                      this.RHTagRuleKeyBlkJSX.setValue("Select Tag Name");
                                                  }
                                                 });
  cmpBtnAddAssetRuleKey.LHFilterRuleKeyBlkJSX = getJsxByName([BlkJSX,"selAssetPolicy_RuleKeyLHFilter"]);
  cmpBtnAddAssetRuleKey.LHTagRuleKeyBlkJSX = cmpSelRuleKeyLHTag.BlkJSX;
  cmpBtnAddAssetRuleKey.OperatorRuleKeyBlkJSX = getJsxByName([BlkJSX,"selAssetPolicy_RuleKeyOp"]);
  cmpBtnAddAssetRuleKey.RHFilterRuleKeyBlkJSX = getJsxByName([BlkJSX,"selAssetPolicy_RuleKeyRHFilter"]);
  cmpBtnAddAssetRuleKey.RHTagRuleKeyBlkJSX = cmpSelRuleKeyRHTag.BlkJSX;
  cmpBtnAddAssetRuleKey.assetRuleKeyMatrix = cmpBlkAssetRuleKeyMatrix.BlkJSX;
  
  var cmpBtnEditAssetAdminClose = new CmpButton([BlkJSX,"btnDialogCloseX"],
                                                 null, 
                                                 {action:"FNCCALL", 
                                                  context:null, 
                                                  fnc:function() {
                                                    dlgEditAssetAdmin_Destroy();
                                                  }
                                                 });
            
  var cmpBtnEditAssetAdminCancel = new CmpButton([BlkJSX,"btnAssetAdminCancel"],
                                                   null, 
                                                   {action:"FNCCALL", 
                                                    context:null, 
                                                    fnc:function() {
                                                      dlgEditAssetAdmin_Destroy();
                                                    }
                                                   });
            
  var cmpBtnEditAssetAdminDone = new CmpButton([BlkJSX,"btnAssetAdminDone"],
                                               null, 
                                               {action:"FNCCALL", 
                                                context:null, 
                                                fnc:function() {
                                                  cmpDlgEditAssetAdminSetAssetAdmin.call(this, AssetAdminNID);
                                                }
                                               });
                                                                                         
  var cmpDlgEditAssetAdmin = new CmpContainer([BlkJSX,"DialogAssetAdmin"],
                                              [ {cmp:cmpBtnEditAssetAdminClose},
                                                {cmp:cmpBtnEditAssetAdminCancel},
                                                {cmp:cmpBtnEditAssetAdminDone},
                                                {cmp:cmpBlkAssetFilterMatrix},
                                                {cmp:cmpBlkAssetTargetFilterMatrix},
                                                {cmp:cmpBlkAssetRuleKeyMatrix},
                                                {cmp:cmpSelUserFilter},
                                                {cmp:cmpSelResourceFilter},
                                                {cmp:cmpSelTargetFilter},
                                                {cmp:cmpSelRuleKeyLHTag},
                                                {cmp:cmpSelRuleKeyRHTag},
                                                {cmp:cmpBtnAddAssetFilter},
                                                {cmp:cmpBtnAddAssetTargetFilter},
                                                {cmp:cmpBtnAddAssetRuleKey},
                                                {cmp:cmpChkEditAssetAdminRoleAdd},
                                                {cmp:cmpChkEditAssetAdminRoleModify},
                                                {cmp:cmpChkEditAssetAdminRoleDelete},
                                                //{cmp:cmpChkEditAssetAdminRoleView},
                                                {cmp:cmpChkEditAssetAdminRoleStart,
                                                 sel:"topic_Get(\"" + tDlg + "\", 'PolicyType') == 'SessionAccessPolicy'"},
                                                {cmp:cmpCDFAssetPolicy}
                                              ],
                                              [{action:"TOPICEVENT", topic:tDlg, eventvar:"EditAssetAdminDataState", eventval:AssetAdminNID}]);                            
                
  cmpDlgEditAssetAdmin.BlkJSX = BlkJSX;      
  cmpDlgEditAssetAdmin.ObjectNID = AssetAdminNID;   
  cmpDlgEditAssetAdmin.getBlkData =  cmpDlgEditAssetAdminGetAssetAdmin;
  
  // init and repaint happens after the dialog is loaded
  jsx3.sleep( (function(){
                 this.init();
                 this.getBlkData(this.ObjectNID);
                 }
               ),
               null, cmpDlgEditAssetAdmin );  
                   
  // fnc to get dlg data
  function cmpDlgEditAssetAdminGetAssetAdmin(AssetAdminNID) { 
    var AssetAdminNode = topic_GetNode(tCache, "//*[@NID='" + AssetAdminNID + "']");
    if( AssetAdminNode == null ) {
      var reqData = new ReqGetObjectNID(AssetAdminNID, "AssetAdminPolicyRule",
                                         {action:"FNCCALL", context:this, 
                                           fnc:function(respCode, respVal) {
                                                 if(respCode == "SUCCESS" && respVal != null) {
                                                   cmpDlgEditAssetAdminGetAssetAdmin(AssetAdminNID);
                                                 } else {
                                                     var codeErrorMsg = CommonErrorCodeArr[respCode].errorDesc;
                                                     var ErrorMsg = "Error: AssetAdmin policy get data request failed \n";
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
      //cmpDlgEditAssetAdmin.setMapRuleData("AssetAdmin", "IN", "NID", AssetAdminNID);
      //topic_Publish(tDlg, "EditAssetAdminDataState", AssetAdminNID);

      // using the old repaint of the dialog
      DisplayEditValuesForAssetAdmin(AssetAdminNID);
      topic_Publish(tDlg, "EditAssetAdminDataState", AssetAdminNID);
    }
  }
       
  // fnc to set dlg data - runs in the context of the OK button
  function cmpDlgEditAssetAdminSetAssetAdmin(AssetAdminNID) {
    // validate input 
    if( cmpDlgEditAssetAdminValidate() != true ) return;
    // decide add or update
    var SetMsg = "Edit AssetAdmin policy";
    var SetMode = "Replace";
    if( AssetAdminNID == "DEFAULT_ASSETADMIN_NID" ) {
      SetMsg = "Add AssetAdmin policy";
      SetMode = "Add";
    }

    //create the role doc
    createAssetRoleDoc();
            
    // get AssetAdmin node info from UI
    var AssetAdminNode = null;

    // write the data back to the CDF
    cmpCDFAssetPolicy.BlkJSX.write();

    var strXMLCacheId = cmpCDFAssetPolicy.BlkJSX.getXMLId();
    var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);      
    var xslfile = PNameSpace.loadInclude("JSXAPPS/IPVS_AdminUI/xsl/AssetPolicyRequest.xsl","AssetPolicyRequest","xsl");
    var xslDoc = jsx3.xml.XslDocument.wrap(xslfile);
    var assetPolicyConfigXML = xslDoc.transform(objXML);
    AssetAdminNode = new jsx3.xml.Document();
    AssetAdminNode.loadXML(assetPolicyConfigXML);

    var strXMLCacheId = cmpBlkAssetFilterMatrix.BlkJSX.getXMLId();
    var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
    var xslfile = PNameSpace.loadInclude("jsxuser:///JSXAPPS/IPVS_AdminUI/xsl/CDFToPolicyUserFilter.xsl","ServiceOperatorFilterSetNode","xsl");
    var xslDoc = jsx3.xml.XslDocument.wrap(xslfile);
    var newServiceOperatorFilterSetXML = xslDoc.transform(objXML);        
    var newServiceOperatorFilterSetNode = new jsx3.xml.Document();
    newServiceOperatorFilterSetNode.loadXML(newServiceOperatorFilterSetXML);
    
    var oldServiceOperatorFilterSetNode = AssetAdminNode.selectSingleNode(".//ServiceOperatorFilterSet");              
    var serviceOperatorFilterSetParentNode = AssetAdminNode.selectSingleNode(".//ServiceOperatorFilterSet").getParent();
    serviceOperatorFilterSetParentNode.replaceNode(newServiceOperatorFilterSetNode,oldServiceOperatorFilterSetNode);

    var assetAdminRoleSetNode = PNameSpace.getCache().getDocument("AssetAdminRoleSetDoc");
    var xslfile = PNameSpace.loadInclude("jsxuser:///JSXAPPS/IPVS_AdminUI/xsl/AssetAdminRoleSet.xsl","AssetAdminRoleSetNode","xsl");
    var xslDoc = jsx3.xml.XslDocument.wrap(xslfile);
    var newAssetAdminRoleSetXML = xslDoc.transform(assetAdminRoleSetNode);        
    var newAssetAdminRoleSetNode = new jsx3.xml.Document();
    newAssetAdminRoleSetNode.loadXML(newAssetAdminRoleSetXML);

    var oldAssetAdminRoleSetNode = AssetAdminNode.selectSingleNode(".//AssetAdminRoleSet");              
    var assetAdminRoleSetParentNode = AssetAdminNode.selectSingleNode(".//AssetAdminRoleSet").getParent();
    assetAdminRoleSetParentNode.replaceNode(newAssetAdminRoleSetNode,oldAssetAdminRoleSetNode);

    var strXMLCacheId = cmpBlkAssetTargetFilterMatrix.BlkJSX.getXMLId();
    var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
    var xslfile = PNameSpace.loadInclude("jsxuser:///JSXAPPS/IPVS_AdminUI/xsl/CDFToPolicyTargetFilter.xsl","AssetAdminObjectFilterSetNode","xsl");
    var xslDoc = jsx3.xml.XslDocument.wrap(xslfile);
    var newAssetAdminObjectFilterSetXML = xslDoc.transform(objXML);        
    var newAssetAdminObjectFilterSetNode = new jsx3.xml.Document();
    newAssetAdminObjectFilterSetNode.loadXML(newAssetAdminObjectFilterSetXML);
    
    var oldAssetAdminObjectFilterSetNode = AssetAdminNode.selectSingleNode(".//AssetAdminObjectFilterSet");              
    var assetAdminObjectFilterSetParentNode = AssetAdminNode.selectSingleNode(".//AssetAdminObjectFilterSet").getParent();
    assetAdminObjectFilterSetParentNode.replaceNode(newAssetAdminObjectFilterSetNode,oldAssetAdminObjectFilterSetNode);

    var strXMLCacheId = cmpBlkAssetRuleKeyMatrix.BlkJSX.getXMLId();
    var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
    var xslfile = PNameSpace.loadInclude("jsxuser:///JSXAPPS/IPVS_AdminUI/xsl/CDFToPolicyRuleKeyFilter.xsl","RuleKeyTagFilterSetNode","xsl");
    var xslDoc = jsx3.xml.XslDocument.wrap(xslfile);
    var newRuleKeyTagFilterSetXML = xslDoc.transform(objXML);        
    var newRuleKeyTagFilterSetNode = new jsx3.xml.Document();
    newRuleKeyTagFilterSetNode.loadXML(newRuleKeyTagFilterSetXML);
    
    var oldRuleKeyTagFilterSetNode = AssetAdminNode.selectSingleNode(".//RuleKeyTagFilterSet");              
    var ruleKeyTagFilterSetParentNode = AssetAdminNode.selectSingleNode(".//RuleKeyTagFilterSet").getParent();
    ruleKeyTagFilterSetParentNode.replaceNode(newRuleKeyTagFilterSetNode,oldRuleKeyTagFilterSetNode);

    if( AssetAdminNode == null ) return;
    // set NID in AssetAdmin node if replace mode
    if( SetMode == "Replace" ) {
      AssetAdminNode.setAttribute("NID", AssetAdminNID);
    }

    // do set request for AssetAdmin              
    var reqSetAssetAdminObject = new ReqSetObject(SetMode, "AssetAdminPolicyRule", AssetAdminNode, 
                                                   //{action:"TOPICEVENT", topic:tDlg, eventvar:"EditAssetAdminSetAssetAdminState", eventval:"DONE"}
                                                    {action:"FNCCALL", context:this, 
                                                       fnc:function(respCode, respVal) {
                                                             if(respCode == "SUCCESS" && respVal != null) {
                                                               topic_Publish(tApp, "AssetAdminPolicyRuleDataUpdate", "ADD");
                                                               dlgEditAssetAdmin_Destroy();
                                                             } else {
                                                               var codeErrorMsg = CommonErrorCodeArr[respCode].errorDesc;
                                                               var ErrorMsg = "Error: " + SetMsg +" request failed \n";
                                                               ErrorMsg += "Error description: " + codeErrorMsg + "\n";
                                                               ErrorMsg += "Error code: " + respCode;
                                                               alert(ErrorMsg);
                                                             }
                                                           }
                                                    }                                                   
                                                 );
                                                 
    if( reqSetAssetAdminObject != null) {
      reqSetAssetAdminObject.sendRequest(this);
    }
  } 
      
  function cmpDlgEditAssetAdminValidate() {
    var valAssetPolicyName = getJsxByName([BlkJSX,"txtAssetPolicyName"]).getValue();
    var valAssetPolicyOrder = getJsxByName([BlkJSX,"txtAssetPolicyOrder"]).getValue();

    var policyName = checkValue(valAssetPolicyName , [BlkJSX,"txtAssetPolicyName"] , "Please enter policy name");
    if(!policyName){
      return false;
    }
    var checkOrder = checkValue(valAssetPolicyOrder , [BlkJSX,"txtAssetPolicyOrder"] , "Please enter policy order" );
    if(!checkOrder){
      return false;
    }
    var chkDuplicateName = "";
    var chkDuplicatePriority = "";
    var chkPermission = "";
    var dialogText = windowBarBlkJSX.getText();
    
    //set the right matrix for validating
    var matrixToUse = "mtxpolicyAssetAdminList";
    if (dialogText == "Add Manage Devices and Profiles Policy" || dialogText == "Edit Manage Devices and Profiles Policy") {
      var ckhBoxArray = new Array();
      ckhBoxArray[0]= "chkAssetPolicyRoleAdd";
      ckhBoxArray[1]= "chkAssetPolicyRoleModify";
      ckhBoxArray[2]= "chkAssetPolicyRoleDelete";
      chkPermission = checkPermissionSelect(BlkJSX,"Please select Add, Modify or Delete Access Permissions",ckhBoxArray);
      if(!chkPermission) return chkPermission;
    }
    else if (dialogText == "Add Source and Destination Ports Policy" || dialogText == "Edit Source and Destination Ports Policy") {
      matrixToUse = "mtxpolicyPortAccessList";
      var ckhBoxArray = new Array();
      ckhBoxArray[0]= "chkAssetAdminSrcPortAccess";
      ckhBoxArray[1]= "chkAssetAdminDstPortAccess";
      chkPermission = checkPermissionSelect(BlkJSX,"Please select Source or Destination Ports for Access Permissions",ckhBoxArray);
      if(!chkPermission) return chkPermission;
    }
    else if (dialogText == "Add Configure Sessions Policy" || dialogText == "Edit Configure Sessions Policy") {
      matrixToUse = "mtxpolicySessionAccessList";
      var ckhBoxArray = new Array();
      ckhBoxArray[0]= "chkAssetPolicyRoleAdd";
      ckhBoxArray[1]= "chkAssetPolicyRoleModify";
      ckhBoxArray[2]= "chkAssetPolicyRoleDelete";
      ckhBoxArray[3]= "chkAssetPolicyRoleStart";
      chkPermission = checkPermissionSelect(BlkJSX,"Please select Add, Modify, Delete or Start Access Permissions",ckhBoxArray);
      if(!chkPermission) return chkPermission;
    }
    else if (PolicyType == "AssetAdminSessionPolicy") {
      matrixToUse = "mtxpolicyAssetAdminSessionList";
    }
    
    if( AssetAdminNID == "DEFAULT_ASSETADMIN_NID" ) {
      if(policyName) {
        chkDuplicateName = chkForNameExistence(valAssetPolicyName, [BlkJSX, "txtAssetPolicyName"], matrixToUse, "Duplicate Policy name. Please enter some other Policy name");
        if(!chkDuplicateName){
          checkOrder = false;
          return false;
        }
      }
      if(chkDuplicateName) {
        checkOrder = checkIntergerValue(valAssetPolicyOrder , [BlkJSX,"txtAssetPolicyOrder"] , "Please enter an integer policy order" );
      }
      if(checkOrder) {
        chkDuplicatePriority = chkForOrderExistence(valAssetPolicyOrder, [BlkJSX, "txtAssetPolicyOrder"], matrixToUse, "Policy Order already exists");
      }
      if(chkDuplicatePriority) {
        return true;
      }
    } else {
      checkOrder = checkIntergerValue(valAssetPolicyOrder , [BlkJSX,"txtAssetPolicyOrder"] , "Please enter an integer policy order" );
      if(checkOrder) {
        if(orgOrderNumber != valAssetPolicyOrder){
          chkDuplicatePriority = chkForOrderExistence(valAssetPolicyOrder, [BlkJSX, "txtAssetPolicyOrder"], matrixToUse, "Policy Order already exists");
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

  function DisplayEditValuesForAssetAdmin(AssetAdminNID) {
    var assetAdminNode = topic_GetNode(tCache, ".//*[@NID='" + AssetAdminNID + "']");
    
    /*var version = assetAdminNode.getAttribute("Ver");
    var versionBlkJSX = getJsxByName([BlkJSX,"txtPolicyAssetXMLVersion"]);
    versionBlkJSX.setValue(version);
    
    var revision = assetAdminNode.getAttribute("Rev");
    var revisionBlkJSX = getJsxByName([BlkJSX,"txtPolicyAssetXMLRevision"]);
    revisionBlkJSX.setValue(revision);    

    var serviceVer = assetAdminNode.getAttribute("serviceVer");
    var serviceVerBlkJSX = getJsxByName([BlkJSX,"txtPolicyAssetXMLServiceVersion"]);
    serviceVerBlkJSX.setValue(serviceVer);    

    var jsxtext = assetAdminNode.selectSingleNode(".//rulename").getValue();
    var filterNameBlkJSX = getJsxByName([BlkJSX,"txtAssetPolicyName"]);
    filterNameBlkJSX.setValue(jsxtext);     

    var index = assetAdminNode.selectSingleNode(".//order").getValue();
    var filterOrderBlkJSX = getJsxByName([BlkJSX,"txtAssetPolicyOrder"]);
    filterOrderBlkJSX.setValue(index); 
    
    //
    var policyValid = assetAdminNode.selectSingleNode(".//valid").getValue();
    var filterActiveBlkJSX = getJsxByName([BlkJSX,"chkAssetPolicyActive"]);
    if(policyValid == "true") {
      filterActiveBlkJSX.setChecked(jsx3.gui.CheckBox.CHECKED);
    } else {
      filterActiveBlkJSX.setChecked(jsx3.gui.CheckBox.UNCHECKED);
    }

    //
    var jsxUserPolicyType = assetAdminNode.selectSingleNode(".//allow").getValue();
    var filterTypeAllowBlkJSX = getJsxByName([BlkJSX,"radioAssetPolicyTypeAllow"]);
    filterTypeAllowBlkJSX.setSelected(jsx3.gui.RadioButton.SELECTED);
//    var filterTypeDenyBlkJSX = getJsxByName([BlkJSX,"radioAssetPolicyTypeDeny"]);
//    if(jsxUserPolicyType == "true") {
//      filterTypeAllowBlkJSX.setSelected(jsx3.gui.RadioButton.SELECTED);
//      filterTypeDenyBlkJSX.setSelected(jsx3.gui.RadioButton.UNSELECTED);
//    } else {
//      filterTypeAllowBlkJSX.setSelected(jsx3.gui.RadioButton.UNSELECTED);
//      filterTypeDenyBlkJSX.setSelected(jsx3.gui.RadioButton.SELECTED);
//    }

   //
    var xAuthRuleListIter  = assetAdminNode.selectNodeIterator(".//ExternalCondition/XAuth/RuleKey");
    while( xAuthRuleListIter.hasNext() ) {
      var xAuthRuleListIterNode = xAuthRuleListIter.next();
      var conditionAction = xAuthRuleListIterNode.getValue();

      if(conditionAction == "MEDIAROOM_RULE_CONFCREATE_USER") {
        var filterxAuthUserBlkJSX = getJsxByName([BlkJSX,"chkAssetPolicy_XAuthUseServiceOperator"]);
        filterxAuthUserBlkJSX.setChecked(jsx3.gui.CheckBox.CHECKED);
      }
    }

    //
    var xAuthServiceOpListIter  = assetAdminNode.selectNodeIterator(".//ExternalCondition/XAuth/ServiceXAuthOperatorFilterSet/ServiceOperatorFilter");
    var mtxAssetPolicy_XAuthUserListBlkJSX = getJsxByName([BlkJSX,"mtxAssetPolicy_XAuthUserList"]);
    mtxAssetPolicy_XAuthUserListBlkJSX.resetCacheData();
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

        mtxAssetPolicy_XAuthUserListBlkJSX.insertRecord(o, null, false);
      }
    }
    mtxAssetPolicy_XAuthUserListBlkJSX.repaintData();
    if (PolicyType == "PortAccessPolicy") {
      var roleListIter = assetAdminNode.selectNodeIterator(".//AssetAdminRoleSet/AssetAdminObjectRole");
      while (roleListIter.hasNext()) {
        var roleListIterNode = roleListIter.next();
        var roleObject = roleListIterNode.selectSingleNode(".//RoleObject").getValue();
        var roleAction = roleListIterNode.selectSingleNode(".//RoleAction").getValue();

        if (roleAction == "ViewFrom") {
          getJsxByName([BlkJSX, "chkAssetAdminSrcPortAccess"]).setChecked(jsx3.gui.CheckBox.CHECKED);
        }

        if (roleAction == "StreamTo") {
          getJsxByName([BlkJSX, "chkAssetAdminDstPortAccess"]).setChecked(jsx3.gui.CheckBox.CHECKED);
        }
      }
    }
    else if (PolicyType == "AssetAdminSessionPolicy") {
      var roleListIter = assetAdminNode.selectNodeIterator(".//AssetAdminRoleSet/AssetAdminObjectRole");
      while (roleListIter.hasNext()) {
        var roleListIterNode = roleListIter.next();
        var roleObject = roleListIterNode.selectSingleNode(".//RoleObject").getValue();
        var roleAction = roleListIterNode.selectSingleNode(".//RoleAction").getValue();

        if (roleAction == "Delete") {
          getJsxByName([BlkJSX, "chkAssetAdminSessionRoleDelete"]).setChecked(jsx3.gui.CheckBox.CHECKED);
        }

        if (roleAction == "Read") {
          getJsxByName([BlkJSX, "chkAssetAdminSessionRoleRead"]).setChecked(jsx3.gui.CheckBox.CHECKED);
        }
      }
    }
    else {
      var roleListIter = assetAdminNode.selectNodeIterator(".//AssetAdminRoleSet/AssetAdminObjectRole");
      while (roleListIter.hasNext()) {
        var roleListIterNode = roleListIter.next();
        var roleObject = roleListIterNode.selectSingleNode(".//RoleObject").getValue();
        var roleAction = roleListIterNode.selectSingleNode(".//RoleAction").getValue();

        if (roleAction == "Add") {
          getJsxByName([BlkJSX, "chkAssetPolicyRoleAdd"]).setChecked(jsx3.gui.CheckBox.CHECKED);
        }

        if (roleAction == "Modify") {
          getJsxByName([BlkJSX, "chkAssetPolicyRoleModify"]).setChecked(jsx3.gui.CheckBox.CHECKED);
        }

        if (roleAction == "Delete") {
          getJsxByName([BlkJSX, "chkAssetPolicyRoleDelete"]).setChecked(jsx3.gui.CheckBox.CHECKED);
        }
        
        if (roleAction == "Read") {
          getJsxByName([BlkJSX, "chkAssetPolicyRoleView"]).setChecked(jsx3.gui.CheckBox.CHECKED);
        }  
        
        if (roleAction == "Start") {
          getJsxByName([BlkJSX, "chkAssetPolicyRoleStart"]).setChecked(jsx3.gui.CheckBox.CHECKED);
        }              
      }
    }*/

    var assetNID = assetAdminNode.getAttribute("NID");
    var strXMLCacheId = cmpCDFAssetPolicy.BlkJSX.getXMLId();
    var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
    objXML.loadXML(assetAdminNode.toString());
    cmpCDFAssetPolicy.BlkJSX.setSourceXML(objXML);         
    cmpCDFAssetPolicy.BlkJSX.setCDFId(assetNID);

    //set AssetAdminRoleDoc
    createAssetRoleDoc();

    var serviceOpListIter  = assetAdminNode.selectNodeIterator(".//ServiceOperatorFilterSet/ServiceOperatorFilter");
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
    var strXMLCacheId = cmpBlkAssetFilterMatrix.BlkJSX.getXMLId();
    var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
    objXML.loadXML(objDoc.toString());
    cmpBlkAssetFilterMatrix.BlkJSX.setSourceXML(objXML);
    cmpBlkAssetFilterMatrix.BlkJSX.repaintData();

    var targetFilterListIter  = assetAdminNode.selectNodeIterator(".//AssetAdminObjectFilterSet/AssetAdminObjectFilterNID");
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
    var strXMLCacheId = cmpBlkAssetTargetFilterMatrix.BlkJSX.getXMLId();
    var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
    objXML.loadXML(objTargetDoc.toString());
    cmpBlkAssetTargetFilterMatrix.BlkJSX.setSourceXML(objXML);
    cmpBlkAssetTargetFilterMatrix.BlkJSX.repaintData();

    var ruleKeyListIter  = assetAdminNode.selectNodeIterator(".//RuleKeyTagFilterSet/RuleKeyTagFilter");
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
    var strXMLCacheId = cmpBlkAssetRuleKeyMatrix.BlkJSX.getXMLId();
    var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
    objXML.loadXML(objRuleKeyDoc.toString());
    cmpBlkAssetRuleKeyMatrix.BlkJSX.setSourceXML(objXML);
    cmpBlkAssetRuleKeyMatrix.BlkJSX.repaintData();
          
    // populate the select with the default values
    updateSelectTagObjectFilters();
    
    // populate the select rule key select with the default value
    updateSelectGroupTag();
    
    if( AssetAdminNID != "DEFAULT_ASSETADMIN_NID" ) {
      getJsxByName([BlkJSX,"txtAssetPolicyName"]).setEnabled(jsx3.gui.Form.STATEDISABLED, true);   
    }
    orgOrderNumber = getJsxByName([BlkJSX,"txtAssetPolicyOrder"]).getValue();
    getJsxByName([BlkJSX,"txtAssetPolicyName"]).focus();
  }

  function updateSelectTagObjectFilters() {
    //getFilterToSelectElement([BlkJSX,"selAssetPolicy_XAuthUserFilter"],"User");
    //getFilterToSelectElement([BlkJSX,"selAssetPolicy_XAuthResourceFilter"],"Resource");
    getFilterToSelectElement([BlkJSX,"selAssetPolicy_UserFilter"],"User");
    getFilterToSelectElement([BlkJSX,"selAssetPolicy_ResourceFilter"],"Resource");
    getFilterToSelectElement([BlkJSX,"selPolicyAssetFilter_Target"],"ResourceProfile");
  }

  function updateSelectGroupTag() {
    getTagDefToSelectElement([BlkJSX,"selAssetPolicy_RuleKeyLHTag"],"ALL");
    getTagDefToSelectElement([BlkJSX,"selAssetPolicy_RuleKeyRHTag"],"ALL");
  }

  function createAssetRoleDoc() {
    if (PolicyType == "AssetAdminPolicy") {
      createAssetAdminRoleDoc();
    }
    else if (PolicyType == "PortAccessPolicy") {
      createPortAccessRoleDoc();
    }
    else if (PolicyType == "SessionAccessPolicy") {
      createSessionAccessRoleDoc();
    }
    else if (PolicyType == "AssetAdminSessionPolicy") {
      createAssetAdminSessionRoleDoc();
    }
  }

  //creating the AssestAdmin roleDoc
  function createAssetAdminRoleDoc() {
    var RoleObjectArr = new Array();
    RoleObjectArr[0] = "AssetResource";
    RoleObjectArr[1] = "Device";
    RoleObjectArr[2] = "MediaStreamSrcPortResource";
    RoleObjectArr[3] = "MediaStreamDstPortResource";
    RoleObjectArr[4] = "MediaStreamRelayPortResource";
    RoleObjectArr[5] = "MediaStorePortResource";
    RoleObjectArr[6] = "MediaStreamProfile";
    RoleObjectArr[7] = "DisplayProfile";

    var roleCacheDoc = PNameSpace.getCache().getDocument("AssetAdminRoleSetDoc");
    if (roleCacheDoc) {
      delete roleCacheDoc;
    }

    var AssetAdminRoleSet = new jsx3.xml.CDF.Document.newDocument();
    PNameSpace.Cache.setDocument("AssetAdminRoleSetDoc", AssetAdminRoleSet);
    
    updateReadPolicy(BlkJSX, "chkAssetPolicyRoleAdd", "chkAssetPolicyRoleModify", "chkAssetPolicyRoleDelete", "chkAssetPolicyRoleView");
    var roleAdd = getJsxByName([BlkJSX, "chkAssetPolicyRoleAdd"]).getChecked();
    var roleModify = getJsxByName([BlkJSX, "chkAssetPolicyRoleModify"]).getChecked();
    var roleDelete = getJsxByName([BlkJSX, "chkAssetPolicyRoleDelete"]).getChecked();
    var roleView = getJsxByName([BlkJSX, "chkAssetPolicyRoleView"]).getChecked();

    if (roleAdd) {
      for (i = 0; i < RoleObjectArr.length; i++) {
        var o = new Object();
        o.jsxid = jsx3.xml.CDF.getKey();
        o.jsxroleObject = RoleObjectArr[i];
        o.jsxroleAction = "Add";
        AssetAdminRoleSet.insertRecord(o, null, false);
      }
    }

    if (roleModify) {
      for (i = 0; i < RoleObjectArr.length; i++) {
        var o = new Object();
        o.jsxid = jsx3.xml.CDF.getKey();
        o.jsxroleObject = RoleObjectArr[i];
        o.jsxroleAction = "Modify";
        AssetAdminRoleSet.insertRecord(o, null, false);
      }
    }

    if (roleDelete) {
      for (i = 0; i < RoleObjectArr.length; i++) {
        var o = new Object();
        o.jsxid = jsx3.xml.CDF.getKey();
        o.jsxroleObject = RoleObjectArr[i];
        o.jsxroleAction = "Delete";
        AssetAdminRoleSet.insertRecord(o, null, false);
      }
    }
    if (roleView) {
      for (i = 0; i < RoleObjectArr.length; i++) {
        var o = new Object();
        o.jsxid = jsx3.xml.CDF.getKey();
        o.jsxroleObject = RoleObjectArr[i];
        o.jsxroleAction = "Read";
        AssetAdminRoleSet.insertRecord(o, null, false);
      }
    }
  }

  //creating the AssestAdmin SessionroleDoc
  function createAssetAdminSessionRoleDoc() {
    var RoleObjectArr = new Array();
    RoleObjectArr[0] = "Session";

    var roleCacheDoc = PNameSpace.getCache().getDocument("AssetAdminRoleSetDoc");
    if (roleCacheDoc) {
      delete roleCacheDoc;
    }

    var AssetAdminRoleSet = new jsx3.xml.CDF.Document.newDocument();
    PNameSpace.Cache.setDocument("AssetAdminRoleSetDoc", AssetAdminRoleSet);
    
    var roleRead = getJsxByName([BlkJSX, "chkAssetAdminSessionRoleRead"]).getChecked();
    var roleDelete = getJsxByName([BlkJSX, "chkAssetAdminSessionRoleDelete"]).getChecked();

    if (roleRead) {
      for (i = 0; i < RoleObjectArr.length; i++) {
        var o = new Object();
        o.jsxid = jsx3.xml.CDF.getKey();
        o.jsxroleObject = RoleObjectArr[i];
        o.jsxroleAction = "Read";
        AssetAdminRoleSet.insertRecord(o, null, false);
      }
    }
    
    if (roleDelete) {
      for (i = 0; i < RoleObjectArr.length; i++) {
        var o = new Object();
        o.jsxid = jsx3.xml.CDF.getKey();
        o.jsxroleObject = RoleObjectArr[i];
        o.jsxroleAction = "Delete";
        AssetAdminRoleSet.insertRecord(o, null, false);
      }
    }
  }

  //creating the Port Access roleDoc
  function createPortAccessRoleDoc() {
    var RoleObjectArr = new Array();
    RoleObjectArr[0] = "MediaStreamSrcPortResource";
    RoleObjectArr[1] = "MediaStreamDstPortResource";

    var roleCacheDoc = PNameSpace.getCache().getDocument("AssetAdminRoleSetDoc");
    if(roleCacheDoc ) {
      delete roleCacheDoc;
    }
          
    var AssetAdminRoleSet = new jsx3.xml.CDF.Document.newDocument();
    PNameSpace.Cache.setDocument("AssetAdminRoleSetDoc", AssetAdminRoleSet);

    var viewFrom = getJsxByName([BlkJSX, "chkAssetAdminSrcPortAccess"]).getChecked();
    var streamTo = getJsxByName([BlkJSX, "chkAssetAdminDstPortAccess"]).getChecked();

    if (viewFrom) {
        var o = new Object();
        o.jsxid = jsx3.xml.CDF.getKey();
        o.jsxroleObject = RoleObjectArr[0];
        o.jsxroleAction = "ViewFrom";
        AssetAdminRoleSet.insertRecord(o, null, false);
        o.jsxid = jsx3.xml.CDF.getKey();
        o.jsxroleObject = RoleObjectArr[0];
        o.jsxroleAction = "Read";
        AssetAdminRoleSet.insertRecord(o, null, false);
    }
    if (streamTo) {
      var o = new Object();
      o.jsxid = jsx3.xml.CDF.getKey();
      o.jsxroleObject = RoleObjectArr[1];
      o.jsxroleAction = "StreamTo";
      AssetAdminRoleSet.insertRecord(o, null, false);
      o.jsxid = jsx3.xml.CDF.getKey();
      o.jsxroleObject = RoleObjectArr[1];
      o.jsxroleAction = "Read";
      AssetAdminRoleSet.insertRecord(o, null, false);
    } 
  }

  //creating the CreatSession Access roleDoc
  function createSessionAccessRoleDoc() {
    var RoleObjectArr = new Array();
    RoleObjectArr[0] = "SessionConfig";

    var roleCacheDoc = PNameSpace.getCache().getDocument("AssetAdminRoleSetDoc");
    if (roleCacheDoc) {
      delete roleCacheDoc;
    }

    var AssetAdminRoleSet = new jsx3.xml.CDF.Document.newDocument();
    PNameSpace.Cache.setDocument("AssetAdminRoleSetDoc", AssetAdminRoleSet);

    updateReadPolicy(BlkJSX, "chkAssetPolicyRoleAdd", "chkAssetPolicyRoleModify", "chkAssetPolicyRoleDelete", "chkAssetPolicyRoleView", "chkAssetPolicyRoleStart");
    var roleAdd = getJsxByName([BlkJSX, "chkAssetPolicyRoleAdd"]).getChecked();
    var roleModify = getJsxByName([BlkJSX, "chkAssetPolicyRoleModify"]).getChecked();
    var roleDelete = getJsxByName([BlkJSX, "chkAssetPolicyRoleDelete"]).getChecked();
    var roleView = getJsxByName([BlkJSX, "chkAssetPolicyRoleView"]).getChecked();
    var roleStart = getJsxByName([BlkJSX, "chkAssetPolicyRoleStart"]).getChecked();
    
    if (roleAdd) {
      for (i = 0; i < RoleObjectArr.length; i++) {
        var o = new Object();
        o.jsxid = jsx3.xml.CDF.getKey();
        o.jsxroleObject = RoleObjectArr[i];
        o.jsxroleAction = "Add";
        AssetAdminRoleSet.insertRecord(o, null, false);
      }
    }

    if (roleModify) {
      for (i = 0; i < RoleObjectArr.length; i++) {
        var o = new Object();
        o.jsxid = jsx3.xml.CDF.getKey();
        o.jsxroleObject = RoleObjectArr[i];
        o.jsxroleAction = "Modify";
        AssetAdminRoleSet.insertRecord(o, null, false);
      }
    }

    if (roleDelete) {
      for (i = 0; i < RoleObjectArr.length; i++) {
        var o = new Object();
        o.jsxid = jsx3.xml.CDF.getKey();
        o.jsxroleObject = RoleObjectArr[i];
        o.jsxroleAction = "Delete";
        AssetAdminRoleSet.insertRecord(o, null, false);
      }
    }
    
    if (roleView) {
      for (i = 0; i < RoleObjectArr.length; i++) {
        var o = new Object();
        o.jsxid = jsx3.xml.CDF.getKey();
        o.jsxroleObject = RoleObjectArr[i];
        o.jsxroleAction = "Read";
        AssetAdminRoleSet.insertRecord(o, null, false);
      }
    }
    
    if (roleStart) {
      for (i = 0; i < RoleObjectArr.length; i++) {
        var o = new Object();
        o.jsxid = jsx3.xml.CDF.getKey();
        o.jsxroleObject = RoleObjectArr[i];
        o.jsxroleAction = "Start";
        AssetAdminRoleSet.insertRecord(o, null, false);
      }
    }
        
  }

}