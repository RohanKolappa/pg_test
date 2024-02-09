// this code has some potential issues 
// - w.r.t object reference counts - when objects created in here go out of scope etc
// - loading sync issues - dlg may not be loaded before the object init methods are called

function dlgEditPolicyAdminPolicy_Create(PolicyAdminPolicyNID) {
 // deal with defaults for add
  if( PolicyAdminPolicyNID == null ) PolicyAdminPolicyNID = "DEFAULT_POLICYADMINPOLICY_NID";
  
  //closeDialogIfExists("DialogPolicyAdminPolicy");
  // load view - dialog component file
  var DlgFile = "components/policy/newPolicyAdminPolicy.xml";
  var BlkJSX = PNameSpace.getBodyBlock().load(DlgFile);

  // setup a local topic for dlg specific messaging     
  var tDlg = "Dialog_" + jsx3.xml.CDF.getKey()
  topic_Create(tDlg);

  // setup a listner for it      
  var eventDlg = new EventListner(tDlg);
  var orgOrderNumber = ""; 

  // setup a private/dlg specific app event handler
  dlgEditPolicyAdminPolicy_eventAppTagObject = new EventListner( tApp );
  
  var dlgText = "Add Manage System Policy";
  if( PolicyAdminPolicyNID != "DEFAULT_POLICYADMINPOLICY_NID" ){
    dlgText = "Edit Manage System Policy";
  }
  var windowBarBlkJSX = getJsxByName([BlkJSX, "windowBar"]);
  windowBarBlkJSX.setText(dlgText, true);
  closeDialogIfExists(dlgText);
    
  var cmpBlkPolicyAdminPolicyFilterMatrix = new CmpBlock([BlkJSX,"mtxPolicyAdminPolicyUserFilter"]);
  cmpBlkPolicyAdminPolicyFilterMatrix.BlkJSX.setXMLTransformers("xsl/PolicyUserFilterToCDF.xsl");
  cmpBlkPolicyAdminPolicyFilterMatrix.BlkJSX.setSelectionModel(jsx3.gui.Matrix.SELECTION_ROW);

  var cmpCDFPolicyAdminPolicy = new Component([BlkJSX,"cdfDlgPolicyAdminPolicy"]);
  cmpCDFPolicyAdminPolicy.BlkJSX.setXMLTransformers("xsl/PolicyAdminPolicy.xsl");
  
  function dlgEditPolicyAdminPolicy_Destroy() {
    // destroy any local eventlistners
    eventDlg.destroy();
    dlgEditPolicyAdminPolicy_eventAppTagObject.destroy();
    
    // destroy all local components
    cmpDlgEditPolicyAdminPolicy.destroy();
    
    // destroy the local topic
    topic_Destroy(tDlg);
    
    // close dialog
    BlkJSX.doClose();
  }
  
  // event subscribtion: if new tagobjectfilter is added from select box and update all the
  // select boxes in the dialog after add is finished
  dlgEditPolicyAdminPolicy_eventAppTagObject.subscribe("TagObjectFilterDataState", null, this,
                                                  function() {
                                                    updateSelectTagObjectFilters();
                                                  } );
  
  var cmpSelUserFilter = new CmpSelect([BlkJSX,"selPolicyAdminPolicyUserFilter"],
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

  var cmpSelResourceFilter = new CmpSelect([BlkJSX,"selPolicyAdminPolicyResourceFilter"],
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
  
  var cmpBtnEditPolicyAdminPolicyAddXAuthUserFilter = new CmpButton([BlkJSX,"btnPolicyAdminPolicy_AddXAuthUserFilter"],
                                                                     null, 
                                                                     {action:"FNCCALL", 
                                                                      context:null, 
                                                                      fnc:function() {
                                                                        AddUserFilterToMatrix(this.SelUserBlkJSX,this.SelresourceBlkJSX,this.UserFilterListBlkJSX);
                                                                      }
                                                                     });
  cmpBtnEditPolicyAdminPolicyAddXAuthUserFilter.SelUserBlkJSX = getJsxByName([BlkJSX,"selPolicyAdminPolicy_XAuthUserFilter"]);
  cmpBtnEditPolicyAdminPolicyAddXAuthUserFilter.SelresourceBlkJSX = getJsxByName([BlkJSX,"selPolicyAdminPolicy_XAuthResourceFilter"]);
  cmpBtnEditPolicyAdminPolicyAddXAuthUserFilter.UserFilterListBlkJSX = getJsxByName([BlkJSX,"mtxPolicyAdminPolicy_XAuthUserList"]);
  cmpBtnEditPolicyAdminPolicyAddXAuthUserFilter.btnAddUserFilterJSX = getJsxByName([BlkJSX,"btnPolicyAdminPolicy_AddXAuthUserFilter"]);                                                       
  
  var cmpBtnEditPolicyAdminPolicyAddUserFilter = new CmpButton([BlkJSX,"btnAddPolicyAdminPolicyUserFilter"],
                                                                 null, 
                                                                 {action:"FNCCALL", 
                                                                  context:null, 
                                                                  fnc:function() {
                                                                    addDatatoMatrix(this.userFilterBlkJSX, this.resourceFilterBlkJSX, this.policyAdminPolicyFilterMatrix);
                                                                  }
                                                                 });
  cmpBtnEditPolicyAdminPolicyAddUserFilter.userFilterBlkJSX =  cmpSelUserFilter.BlkJSX;
  cmpBtnEditPolicyAdminPolicyAddUserFilter.resourceFilterBlkJSX = cmpSelResourceFilter.BlkJSX;
  cmpBtnEditPolicyAdminPolicyAddUserFilter.policyAdminPolicyFilterMatrix = cmpBlkPolicyAdminPolicyFilterMatrix.BlkJSX;
        
  var cmpBtnEditPolicyAdminPolicyClose = new CmpButton([BlkJSX,"btnDialogCloseX"],
                                                       null, 
                                                       {action:"FNCCALL", 
                                                        context:null, 
                                                        fnc:function() {
                                                          dlgEditPolicyAdminPolicy_Destroy();
                                                        }
                                                       });
            
  var cmpBtnEditPolicyAdminPolicyCancel = new CmpButton([BlkJSX,"btnPolicyAdminPolicyCancel"],
                                                         null, 
                                                         {action:"FNCCALL", 
                                                          context:null, 
                                                          fnc:function() {
                                                            dlgEditPolicyAdminPolicy_Destroy();
                                                          }
                                                         });
            
  var cmpBtnEditPolicyAdminPolicyDone = new CmpButton([BlkJSX,"btnPolicyAdminPolicyDone"],
                                                       null, 
                                                       {action:"FNCCALL", 
                                                        context:null, 
                                                        fnc:function() {
                                                          cmpDlgEditPolicyAdminPolicySetPolicyAdmin.call(this, PolicyAdminPolicyNID);
                                                        }
                                                       });
                                                                                         
  var cmpDlgEditPolicyAdminPolicy = new CmpContainer([BlkJSX,"DialogPolicyAdminPolicy"],
                                                      [ {cmp:cmpBtnEditPolicyAdminPolicyClose},
                                                        {cmp:cmpBtnEditPolicyAdminPolicyCancel},
                                                        {cmp:cmpBtnEditPolicyAdminPolicyDone},
                                                        {cmp:cmpSelUserFilter},
                                                        {cmp:cmpSelResourceFilter},
                                                        {cmp:cmpBlkPolicyAdminPolicyFilterMatrix},
                                                        {cmp:cmpBtnEditPolicyAdminPolicyAddXAuthUserFilter},
                                                        {cmp:cmpBtnEditPolicyAdminPolicyAddUserFilter},
                                                        {cmp:cmpCDFPolicyAdminPolicy}
                                                      ],
                                                      [{action:"TOPICEVENT", topic:tDlg, eventvar:"EditPolicyAdminPolicyDataState", eventval:PolicyAdminPolicyNID}]);
                                                      
  cmpDlgEditPolicyAdminPolicy.BlkJSX = BlkJSX;      
  cmpDlgEditPolicyAdminPolicy.ObjectNID = PolicyAdminPolicyNID;   
  cmpDlgEditPolicyAdminPolicy.getBlkData =  cmpDlgEditPolicyAdminPolicyGetPolicyAdmin;
  
  // init and repaint happens after the dialog is loaded
  jsx3.sleep( (function(){
                 this.init();
                 this.getBlkData(this.ObjectNID);
                 }
               ),
               null, cmpDlgEditPolicyAdminPolicy );  
                   
  // fnc to get dlg data
  function cmpDlgEditPolicyAdminPolicyGetPolicyAdmin(PolicyAdminPolicyNID) { 
    var PolicyAdminPolicyNode = topic_GetNode(tCache, "//*[@NID='" + PolicyAdminPolicyNID + "']");
    if( PolicyAdminPolicyNode == null ) {
      var reqData = new ReqGetObjectNID(PolicyAdminPolicyNID, "PolicyAdminPolicyRule",
                                         {action:"FNCCALL", context:this, 
                                           fnc:function(respCode, respVal) {
                                                 if(respCode == "SUCCESS" && respVal != null) {
                                                   cmpDlgEditPolicyAdminPolicyGetPolicyAdmin(PolicyAdminPolicyNID);
                                                 } else {
                                                     var codeErrorMsg = CommonErrorCodeArr[respCode].errorDesc;
                                                     var ErrorMsg = "Error: Policy Admin Policy get data request failed \n";
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
      //cmpDlgEditPolicyAdminPolicy.setMapRuleData("PolicyAdminPolicy", "IN", "NID", PolicyAdminPolicyNID);
      //topic_Publish(tDlg, "EditPolicyAdminPolicyDataState", PolicyAdminPolicyNID);

      // using the old repaint of the dialog
      DisplayEditValuesForPolicyAdminPolicy(PolicyAdminPolicyNID);
    }
  } // end cmpDlgEditPolicyAdminPolicyGetPolicyAdmin
       
  // fnc to set dlg data - runs in the context of the OK button
  function cmpDlgEditPolicyAdminPolicySetPolicyAdmin(PolicyAdminPolicyNID) {
    // validate input 
    if( cmpDlgEditPolicyAdminPolicyValidate() != true ) return;
    // decide add or update
    var SetMsg = "Edit Policy Admin policy";
    var SetMode = "Replace";
    if( PolicyAdminPolicyNID == "DEFAULT_POLICYADMINPOLICY_NID" ) {
      SetMsg = "Add Policy Admin policy";
      SetMode = "Add";
    }

    // create roleset cache doc, used in the rule
    createPolicyAdminRoleSetDoc();
    createPolicyAdminObjectFilterSetDoc();
    
    // get PolicyAdminPolicy node info from UI
    var PolicyAdminPolicyNode = null;
    
    // write the data back to the CDF
    cmpCDFPolicyAdminPolicy.BlkJSX.write();

    var strXMLCacheId = cmpCDFPolicyAdminPolicy.BlkJSX.getXMLId();
    var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);      
    var xslfile = PNameSpace.loadInclude("JSXAPPS/IPVS_AdminUI/xsl/PolicyAdminPolicyRequest.xsl","PolicyAdminPolicyRequest","xsl");
    var xslDoc = jsx3.xml.XslDocument.wrap(xslfile);
    var policyAdminPolicyConfigXML = xslDoc.transform(objXML);
    PolicyAdminPolicyNode = new jsx3.xml.Document();
    PolicyAdminPolicyNode.loadXML(policyAdminPolicyConfigXML);

    var strXMLCacheId = cmpBlkPolicyAdminPolicyFilterMatrix.BlkJSX.getXMLId();
    var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
    var xslfile = PNameSpace.loadInclude("jsxuser:///JSXAPPS/IPVS_AdminUI/xsl/CDFToPolicyUserFilter.xsl","ServiceOperatorFilterSetNode","xsl");
    var xslDoc = jsx3.xml.XslDocument.wrap(xslfile);
    var newServiceOperatorFilterSetXML = xslDoc.transform(objXML);        
    var newServiceOperatorFilterSetNode = new jsx3.xml.Document();
    newServiceOperatorFilterSetNode.loadXML(newServiceOperatorFilterSetXML);
    
    var oldServiceOperatorFilterSetNode = PolicyAdminPolicyNode.selectSingleNode(".//ServiceOperatorFilterSet");              
    var serviceOperatorFilterSetParentNode = PolicyAdminPolicyNode.selectSingleNode(".//ServiceOperatorFilterSet").getParent();
    serviceOperatorFilterSetParentNode.replaceNode(newServiceOperatorFilterSetNode,oldServiceOperatorFilterSetNode);

    var policyAdminPolicyRoleSetNode = PNameSpace.getCache().getDocument("PolicyAdminRoleSetDoc");
    var xslfile = PNameSpace.loadInclude("jsxuser:///JSXAPPS/IPVS_AdminUI/xsl/PolicyAdminRoleSet.xsl","PolicyAdminRoleSetNode","xsl");
    var xslDoc = jsx3.xml.XslDocument.wrap(xslfile);
    var newPolicyAdminRoleSetXML = xslDoc.transform(policyAdminPolicyRoleSetNode);        
    var newPolicyAdminRoleSetNode = new jsx3.xml.Document();
    newPolicyAdminRoleSetNode.loadXML(newPolicyAdminRoleSetXML);

    var oldPolicyAdminRoleSetNode = PolicyAdminPolicyNode.selectSingleNode(".//PolicyAdminRoleSet");              
    var policyAdminRoleSetParentNode = PolicyAdminPolicyNode.selectSingleNode(".//PolicyAdminRoleSet").getParent();
    policyAdminRoleSetParentNode.replaceNode(newPolicyAdminRoleSetNode,oldPolicyAdminRoleSetNode);

    if( PolicyAdminPolicyNode == null ) return;
    // set NID in PolicyAdminPolicy node if replace mode
    if( SetMode == "Replace" ) {
      PolicyAdminPolicyNode.setAttribute("NID", PolicyAdminPolicyNID);
    }

    // set the objectfiletre value _ALL_ nid
    var tagObjectFilterListNode = getObjectCacheListNode("TagObjectFilter");
    var allTagObjectNode = tagObjectFilterListNode.selectSingleNode(".//TagObjectFilter[.//Name='_ALL_']");
    var allTagObjectFilterNid = "";
    if(!jsx3.util.strEmpty(allTagObjectNode)) {
      allTagObjectFilterNid = allTagObjectNode.getAttribute("NID");
    }
    var policyObjectFilterNode = PolicyAdminPolicyNode.selectSingleNode(".//PolicyAdminObjectFilterNID");
    if(!jsx3.util.strEmpty(policyObjectFilterNode)) {
      policyObjectFilterNode.setValue(allTagObjectFilterNid);
    }
    else{
      var policyObjectFilterSet = PolicyAdminPolicyNode.selectSingleNode(".//PolicyAdminObjectFilterSet");
      policyObjectFilterNode = policyObjectFilterSet.createNode(jsx3.xml.Entity.TYPEELEMENT, "PolicyAdminObjectFilterNID").setValue(allTagObjectFilterNid);
      policyObjectFilterSet.appendChild(policyObjectFilterNode);
    }

    // do set request for PolicyAdminPolicy              
    var reqSetPolicyAdminPolicyObject = new ReqSetObject(SetMode, "PolicyAdminPolicyRule", PolicyAdminPolicyNode,
                                                         {action:"FNCCALL", context:this, 
                                                         fnc:function(respCode, respVal) {
                                                               if(respCode == "SUCCESS" && respVal != null) {
                                                                 dlgEditPolicyAdminPolicy_Destroy();
                                                                 topic_Publish(tApp, "PolicyAdminPolicyRuleDataUpdate", "ADD");
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
                                                 
    if( reqSetPolicyAdminPolicyObject != null) {
      reqSetPolicyAdminPolicyObject.sendRequest(this);
    }
  } // end cmpDlgEditPolicyAdminPolicySetPolicyAdmin
      
  function cmpDlgEditPolicyAdminPolicyValidate() {
    var valPolicyName = getJsxByName([BlkJSX,"txtPolicyAdminPolicyName"]).getValue();
    var valPolicyOrder = getJsxByName([BlkJSX,"txtPolicyAdminPolicyOrder"]).getValue();

    var policyName = checkValue(valPolicyName , [BlkJSX,"txtPolicyAdminPolicyName"], "Please enter policy name");
    if(!policyName){
      return false;
    }
    var checkOrder = checkValue(valPolicyOrder , [BlkJSX,"txtPolicyAdminPolicyOrder"], "Please enter policy order" );
    if(!checkOrder){
      return false;
    }
    var chkDuplicateName = "";
    var chkDuplicatePriority = "";
    
    if( PolicyAdminPolicyNID == "DEFAULT_POLICYADMINPOLICY_NID" ) {
      if(policyName) {
        chkDuplicateName = chkForNameExistence(valPolicyName, [BlkJSX,"txtPolicyAdminPolicyName"], "mtxpolicyAdminPolicyList", "Duplicate Policy name. Please enter some other Policy name" );
        if(!chkDuplicateName){
          checkOrder = false;
          return false;
        }
      }
      if(chkDuplicateName) {
        checkOrder = checkIntergerValue(valPolicyOrder , [BlkJSX,"txtPolicyAdminPolicyOrder"], "Please enter an integer policy order" );
      }
      if(checkOrder) {
        chkDuplicatePriority = chkForOrderExistence(valPolicyOrder, [BlkJSX,"txtPolicyAdminPolicyOrder"], "mtxpolicyAdminPolicyList", "Policy Order already exists");
      }
      if(chkDuplicatePriority) {
        return true;
      }
    } else {
      checkOrder = checkIntergerValue(valPolicyOrder , [BlkJSX,"txtPolicyAdminPolicyOrder"], "Please enter an integer policy order" );
      if(checkOrder) {
        if(orgOrderNumber != valPolicyOrder){
          chkDuplicatePriority = chkForOrderExistence(valPolicyOrder, [BlkJSX,"txtPolicyAdminPolicyOrder"], "mtxpolicyAdminPolicyList", "Policy Order already exists");
        } else {
          return true;
        }
      }
      if(chkDuplicatePriority) {
        return true;
      }
    }
          
    return false;
  } // end cmpDlgEditPolicyAdminPolicyValidate

  function DisplayEditValuesForPolicyAdminPolicy(PolicyAdminPolicyNID) {
    var policyAdminPolicyNode = topic_GetNode(tCache, ".//*[@NID='" + PolicyAdminPolicyNID + "']");
    
    /*var version = policyAdminPolicyNode.getAttribute("Ver");
    var versionBlkJSX = getJsxByName([BlkJSX,"txtPolicyAdminPolicyXMLVersion"]);
    versionBlkJSX.setValue(version);
    
    var revision = policyAdminPolicyNode.getAttribute("Rev");
    var revisionBlkJSX = getJsxByName([BlkJSX,"txtPolicyAdminPolicyXMLRevision"]);
    revisionBlkJSX.setValue(revision);  

    var serviceVer = policyAdminPolicyNode.getAttribute("serviceVer");
    var serviceVerBlkJSX = getJsxByName([BlkJSX,"txtPolicyAdminPolicyXMLServiceVersion"]);
    serviceVerBlkJSX.setValue(serviceVer);    

    var jsxtext = policyAdminPolicyNode.selectSingleNode(".//rulename").getValue();
    var filterNameBlkJSX = getJsxByName([BlkJSX,"txtPolicyAdminPolicyName"]);
    filterNameBlkJSX.setValue(jsxtext);
    
    var index = policyAdminPolicyNode.selectSingleNode(".//order").getValue();
    var filterOrderBlkJSX = getJsxByName([BlkJSX,"txtPolicyAdminPolicyOrder"]);
    filterOrderBlkJSX.setValue(index);    

    var policyValid = policyAdminPolicyNode.selectSingleNode(".//valid").getValue();
    var filterActiveBlkJSX = getJsxByName([BlkJSX,"chkPolicyAdminPolicyActive"]);
    if(policyValid == "true") {
      filterActiveBlkJSX.setChecked(jsx3.gui.CheckBox.CHECKED);
    } else {
      filterActiveBlkJSX.setChecked(jsx3.gui.CheckBox.UNCHECKED);
    }
 
   //
    var jsxUserPolicyType = policyAdminPolicyNode.selectSingleNode(".//allow").getValue();
    var filterTypeAllowBlkJSX = getJsxByName([BlkJSX,"radioPolicyAdminPolicyTypeAllow"]);
    filterTypeAllowBlkJSX.setSelected(jsx3.gui.RadioButton.SELECTED);
//    var filterTypeDenyBlkJSX = getJsxByName([BlkJSX,"radioPolicyAdminPolicyTypeDeny"]);    
    /*if(jsxUserPolicyType == "true") {
      filterTypeAllowBlkJSX.setSelected(jsx3.gui.RadioButton.SELECTED);
      filterTypeDenyBlkJSX.setSelected(jsx3.gui.RadioButton.UNSELECTED);
    } else {
      filterTypeAllowBlkJSX.setSelected(jsx3.gui.RadioButton.UNSELECTED);
      filterTypeDenyBlkJSX.setSelected(jsx3.gui.RadioButton.SELECTED);
    }

   //
    var xAuthRuleListIter  = policyAdminPolicyNode.selectNodeIterator(".//ExternalCondition/XAuth/RuleKey");
    while( xAuthRuleListIter.hasNext() ) {
      var xAuthRuleListIterNode = xAuthRuleListIter.next();
      var conditionAction = xAuthRuleListIterNode.getValue();

      if(conditionAction == "MEDIAROOM_RULE_CONFCREATE_USER") {
        var filterxAuthUserBlkJSX = getJsxByName([BlkJSX,"chkPolicyAdminPolicy_XAuthUseServiceOperator"]);
        filterxAuthUserBlkJSX.setChecked("1");
      }
    }

    //
    var xAuthServiceOpListIter  = policyAdminPolicyNode.selectNodeIterator(".//ExternalCondition/XAuth/ServiceXAuthOperatorFilterSet/ServiceOperatorFilter");
    var mtxPolicyAdminPolicy_XAuthUserListBlkJSX = getJsxByName([BlkJSX,"mtxPolicyAdminPolicy_XAuthUserList"]);
    mtxPolicyAdminPolicy_XAuthUserListBlkJSX.resetCacheData();
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

        mtxPolicyAdminPolicy_XAuthUserListBlkJSX.insertRecord(o, null, false);
      }
    }
    mtxPolicyAdminPolicy_XAuthUserListBlkJSX.repaintData();*/

    var policyAdminPolicyNID = policyAdminPolicyNode.getAttribute("NID");
    var strXMLCacheId = cmpCDFPolicyAdminPolicy.BlkJSX.getXMLId();
    var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
    objXML.loadXML(policyAdminPolicyNode.toString());
    cmpCDFPolicyAdminPolicy.BlkJSX.setSourceXML(objXML);         
    cmpCDFPolicyAdminPolicy.BlkJSX.setCDFId(policyAdminPolicyNID);

    var serviceOpListIter  = policyAdminPolicyNode.selectNodeIterator(".//ServiceOperatorFilterSet/ServiceOperatorFilter");
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
    var strXMLCacheId = cmpBlkPolicyAdminPolicyFilterMatrix.BlkJSX.getXMLId();
    var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
    objXML.loadXML(objDoc.toString());
    cmpBlkPolicyAdminPolicyFilterMatrix.BlkJSX.setSourceXML(objXML);
    cmpBlkPolicyAdminPolicyFilterMatrix.BlkJSX.repaintData();
          
    // create roleset cache doc, used in the rule
    createPolicyAdminRoleSetDoc();
    createPolicyAdminObjectFilterSetDoc();

    // populate the select with the default values
    updateSelectTagObjectFilters();
    
    if( PolicyAdminPolicyNID != "DEFAULT_POLICYADMINPOLICY_NID" ) {
      getJsxByName([BlkJSX,"txtPolicyAdminPolicyName"]).setEnabled(jsx3.gui.Form.STATEDISABLED, true);   
    }
    orgOrderNumber = getJsxByName([BlkJSX,"txtPolicyAdminPolicyOrder"]).getValue();
    getJsxByName([BlkJSX,"txtPolicyAdminPolicyName"]).focus();
  } // end DisplayEditValuesForPolicyAdminPolicy

  function updateSelectTagObjectFilters() {
    //getFilterToSelectElement([BlkJSX,"selPolicyAdminPolicy_XAuthUserFilter"],"User");
    //getFilterToSelectElement([BlkJSX,"selPolicyAdminPolicy_XAuthResourceFilter"],"Resource");
    getFilterToSelectElement([BlkJSX,"selPolicyAdminPolicyUserFilter"],"User");
    getFilterToSelectElement([BlkJSX,"selPolicyAdminPolicyResourceFilter"],"Resource");
  }

  function createPolicyAdminRoleSetDoc() {
    var RoleObjectArr = new Array();
    
    RoleObjectArr[0] = "GroupTagDef";
    RoleObjectArr[1] = "Filter";
    RoleObjectArr[2] = "AssetAdminPolicyRule";
    RoleObjectArr[3] = "MediaStoreAdminPolicyRule";
    RoleObjectArr[4] = "UserAdminPolicyRule";
    RoleObjectArr[5] = "PolicyAdminPolicyRule";
    RoleObjectArr[6] = "MediaStreamFilter";
    RoleObjectArr[7] = "MediaRoomPolicyRule";
    RoleObjectArr[8] = "NetworkAddressRule";
    RoleObjectArr[9] = "Report";
    
    
    var RoleArr = new Array();
    RoleArr[0] = "Add";
    RoleArr[1] = "Modify";
    RoleArr[2] = "Delete";
    RoleArr[3] = "Read";       

    var roleCacheDoc = PNameSpace.getCache().getDocument("PolicyAdminRoleSetDoc");
    if(roleCacheDoc ) {
      delete roleCacheDoc;
    }
          
    var PolicyAdminPolicyRoleSet = new jsx3.xml.CDF.Document.newDocument();
    PNameSpace.Cache.setDocument("PolicyAdminRoleSetDoc", PolicyAdminPolicyRoleSet);

    for(var roleObj=0; roleObj<RoleObjectArr.length; roleObj++) {
      for(var role=0; role<RoleArr.length; role++) {
        var o = new Object();
        o.jsxid = jsx3.xml.CDF.getKey();
        o.jsxroleObject = RoleObjectArr[roleObj];
        o.jsxroleAction = RoleArr[role];
        PolicyAdminPolicyRoleSet.insertRecord(o, null, false);
      }

    }

    var RoleArr = new Array();
    RoleArr[0] = "Upgrade";
    RoleArr[1] = "Restore";
    RoleArr[2] = "Backup";
    RoleArr[3] = "SessionManagement";
    RoleArr[4] = "Read";
    RoleArr[5] = "Modify";
    for(var role=0; role<RoleArr.length; role++) {
      var o = new Object();
      o.jsxid = jsx3.xml.CDF.getKey();
      o.jsxroleObject = "System";
      o.jsxroleAction = RoleArr[role];
      PolicyAdminPolicyRoleSet.insertRecord(o, null, false);
    }
    
  } // end createPolicyAdminRoleSetDoc
  
  function createPolicyAdminObjectFilterSetDoc() {
    var objectFilterCacheDoc = PNameSpace.getCache().getDocument("PolicyAdminObjectFilterSetDoc");
    if(objectFilterCacheDoc ) {
      delete objectFilterCacheDoc;
    }
          
    var PolicyAdminPolicyObjectFilterSet = new jsx3.xml.CDF.Document.newDocument();
    PNameSpace.Cache.setDocument("PolicyAdminObjectFilterSetDoc", PolicyAdminPolicyObjectFilterSet);
    
    var objectFilterNIDListNode = topic_GetNode(tApp, "//ObjectFilterNIDList");
    if( objectFilterNIDListNode != null ) {
      var objectFilterNIDListNodeIter = objectFilterNIDListNode.selectNodeIterator(".//ObjectNID");
      while( objectFilterNIDListNodeIter.hasNext() ) {
        var objectFilterNID = objectFilterNIDListNodeIter.next().getValue();
        if( objectFilterNID != "DEFAULT_OBJECTFILTER_NID") {
          var o = new Object();
          o.jsxid = jsx3.xml.CDF.getKey();
          o.jsxObjectFilterNID = objectFilterNID;
          PolicyAdminPolicyObjectFilterSet.insertRecord(o, null, false);
        }
      }
    }      
  
  } // end createPolicyAdminObjectFilterSetDoc
  
  function AddUserFilterToMatrix(selUser, selresource, mtxUserFilter) {
    var RIDSelectOne = selUser.getValue();
    var mtachResult = RIDSelectOne.search(/Select /);

    if(mtachResult != -1){
      alert("Please "+RIDSelectOne);
      return false;
    } else {
      var SelectOneNode = selUser.getRecordNode(RIDSelectOne);
      var FirstFilterName= SelectOneNode.getAttribute("jsxtext");
      var FirstFilterNID = SelectOneNode.getAttribute("NID");
    }
    var RIDSelectTwo = selresource.getValue();
    var mtachResult = RIDSelectTwo.search(/Select /);

    if(mtachResult != -1){
      alert("Please "+RIDSelectTwo);
      return false;
    } else {
      var SelectTwoNode = selresource.getRecordNode(RIDSelectTwo);
      var SecondFilterName= SelectTwoNode.getAttribute("jsxtext");
      var SecondFilterNID = SelectTwoNode.getAttribute("NID");
    } 
    var o = new Object();
    o.jsxid = jsx3.xml.CDF.getKey() ;
    o.jsxtextOneNID = FirstFilterNID;
    o.jsxtextOne = FirstFilterName;
    o.jsxAt="@";
    o.jsxtextTwoNID = SecondFilterNID;
    o.jsxtextTwo = SecondFilterName;
    mtxUserFilter.insertRecord(o, null, false); 
    resetSelectBoxValueToDefault(selUser);
    resetSelectBoxValueToDefault(selresource);
    mtxUserFilter.repaintData();
  } // end AddUserFilterToMatrix

} // end dlgEditPolicyAdminPolicy_Create