// this code has some potential issues 
// - w.r.t object reference counts - when objects created in here go out of scope etc
// - loading sync issues - dlg may not be loaded before the object init methods are called

function dlgEditUserAdmin_Create(UserAdminNID) {

  // deal with defaults for add
  if( UserAdminNID == null ) UserAdminNID = "DEFAULT_USERADMIN_NID";
  
  //closeDialogIfExists("DialogUserAdmin");
  // load view - dialog component file
  var DlgFile = "components/policy/newUserPolicy.xml";
  var BlkJSX = PNameSpace.getBodyBlock().load(DlgFile);

  // setup a local topic for dlg specific messaging    
  var tDlg = "Dialog_" + jsx3.xml.CDF.getKey()
  topic_Create(tDlg);

  // setup a listner for it    
  var eventDlg = new EventListner(tDlg);
  var orgOrderNumber = "";

  // setup a private/dlg specific app event handler
  dlgEditUserAdmin_eventAppTagObject = new EventListner( tApp );
  dlgEditUserAdmin_eventAppGroupTag = new EventListner( tApp );

  var dlgText = "Add Manage Users Policy";
  if( UserAdminNID != "DEFAULT_USERADMIN_NID" ){
    dlgText = "Edit Manage Users Policy";
  }
  var windowBarBlkJSX = getJsxByName([BlkJSX, "windowBar"]);
  windowBarBlkJSX.setText(dlgText, true);
  closeDialogIfExists(dlgText);
  
  var cmpBlkUserAdminFilterMatrix = new CmpBlock([BlkJSX,"mtxUserPolicy_UserFilterSel"]);
  cmpBlkUserAdminFilterMatrix.BlkJSX.setXMLTransformers("xsl/PolicyUserFilterToCDF.xsl");
  cmpBlkUserAdminFilterMatrix.BlkJSX.setSelectionModel(jsx3.gui.Matrix.SELECTION_ROW);

  var cmpBlkUserAdminTargetFilterMatrix = new CmpBlock([BlkJSX,"mtxUserPolicy_TargetFilterSel"]);
  cmpBlkUserAdminTargetFilterMatrix.BlkJSX.setXMLTransformers("xsl/PolicyTargetFilterToCDF.xsl");
  cmpBlkUserAdminTargetFilterMatrix.BlkJSX.setSelectionModel(jsx3.gui.Matrix.SELECTION_ROW);

  var cmpBlkUserAdminRuleKeyMatrix = new CmpBlock([BlkJSX,"mtxUserPolicy_RuleKeySel"]);
  cmpBlkUserAdminRuleKeyMatrix.BlkJSX.setXMLTransformers("xsl/PolicyRuleKeyFilterToCDF.xsl");
  cmpBlkUserAdminRuleKeyMatrix.BlkJSX.setSelectionModel(jsx3.gui.Matrix.SELECTION_ROW);

  var cmpCDFUserAdminPolicy = new Component([BlkJSX,"cdfDlgUserAdminPolicy"]);
  cmpCDFUserAdminPolicy.BlkJSX.setXMLTransformers("xsl/UserAdminPolicy.xsl");
    
  function dlgEditUserAdmin_Destroy() {
    // destroy any local eventlistners
    eventDlg.destroy();
    dlgEditUserAdmin_eventAppTagObject.destroy();
    dlgEditUserAdmin_eventAppGroupTag.destroy();
    
    // destroy all local components
    cmpDlgEditUserAdmin.destroy();
    
    // destroy the local topic
    topic_Destroy(tDlg);
    
    // close dialog
    BlkJSX.doClose();
  }


  // event subscribtion: if new tagobjectfilter is added from select box and update all the
  // select boxes in the dialog after add is finished
  dlgEditUserAdmin_eventAppTagObject.subscribe("TagObjectFilterDataState", null, this,
                                                function() {
                                                  updateSelectTagObjectFilters();
                                                } );
  
  // event subscribtion: if new grouptag is added from select box and update all the
  // select boxes in the dialog after add is finished                      
  dlgEditUserAdmin_eventAppGroupTag.subscribe("GroupTagDataState", null, this,
                                                function() {
                                                  updateSelectGroupTag();
                                                } );

  var cmpSelUserFilter = new CmpSelect([BlkJSX,"selUserFilter_Users"],
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

  var cmpSelResourceFilter = new CmpSelect([BlkJSX,"selUserFilter_Resource"],
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
        
  var cmpBtnAddUserAdminFilter = new CmpButton([BlkJSX,"btnUserPolicy_AddUserFilter"],
                                                 null, 
                                                 {action:"FNCCALL", 
                                                  context:null, 
                                                  fnc:function() {
                                                      addDatatoMatrix(this.userFilterBlkJSX, this.resourceFilterBlkJSX, this.userAdminFilterMatrix);
                                                  }
                                                 });
  cmpBtnAddUserAdminFilter.userFilterBlkJSX = cmpSelUserFilter.BlkJSX;
  cmpBtnAddUserAdminFilter.resourceFilterBlkJSX = cmpSelResourceFilter.BlkJSX;
  cmpBtnAddUserAdminFilter.userAdminFilterMatrix = cmpBlkUserAdminFilterMatrix.BlkJSX;
        
  var cmpSelTargetFilter = new CmpSelect([BlkJSX,"selUserFilter_Target"],
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
  
  var cmpBtnAddUserAdminTargetFilter = new CmpButton([BlkJSX,"btnUserPolicy_AddTargetFilter"],
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
                                                        this.userAdminTargetFilterMatrix.insertRecord(o, null, false);
                                                      }
                                                      this.userAdminTargetFilterMatrix.repaintData();
                                                      this.targetFilterBlkJSX.setValue("Select User Filter");
                                                  }
                                                 });
  cmpBtnAddUserAdminTargetFilter.targetFilterBlkJSX = cmpSelTargetFilter.BlkJSX;
  cmpBtnAddUserAdminTargetFilter.userAdminTargetFilterMatrix = cmpBlkUserAdminTargetFilterMatrix.BlkJSX;
  
  var cmpSelRuleKeyLHTag = new CmpSelect([BlkJSX,"selUserPolicy_RuleKeyLHTag"],
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
  var cmpSelRuleKeyRHTag = new CmpSelect([BlkJSX,"selUserPolicy_RuleKeyRHTag"],
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
  
  var cmpBtnAddUserAdminRuleKey = new CmpButton([BlkJSX,"btnUserPolicy_AddRuleKey"],
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
                                                          this.userAdminRuleKeyMatrix.insertRecord(o, null, false);
                                                        }
                                                      }
                                                      this.userAdminRuleKeyMatrix.repaintData();
                                                      this.LHFilterRuleKeyBlkJSX.setValue("Select LH Rule");
                                                      this.LHTagRuleKeyBlkJSX.setValue("Select Tag Name");
                                                      this.OperatorRuleKeyBlkJSX.setValue("Select Operation");
                                                      this.RHFilterRuleKeyBlkJSX.setValue("Select RH Rule");
                                                      this.RHTagRuleKeyBlkJSX.setValue("Select Tag Name");
                                                  }
                                                 });
  cmpBtnAddUserAdminRuleKey.LHFilterRuleKeyBlkJSX = getJsxByName([BlkJSX,"selUserPolicy_RuleKeyLHFilter"]);
  cmpBtnAddUserAdminRuleKey.LHTagRuleKeyBlkJSX = cmpSelRuleKeyLHTag.BlkJSX;
  cmpBtnAddUserAdminRuleKey.OperatorRuleKeyBlkJSX = getJsxByName([BlkJSX,"selUserPolicy_RuleKeyOp"]);
  cmpBtnAddUserAdminRuleKey.RHFilterRuleKeyBlkJSX = getJsxByName([BlkJSX,"selUserPolicy_RuleKeyRHFilter"]);
  cmpBtnAddUserAdminRuleKey.RHTagRuleKeyBlkJSX = cmpSelRuleKeyRHTag.BlkJSX;
  cmpBtnAddUserAdminRuleKey.userAdminRuleKeyMatrix = cmpBlkUserAdminRuleKeyMatrix.BlkJSX;
  
  var cmpBtnEditUserAdminClose = new CmpButton([BlkJSX,"btnDialogCloseX"],
                                               null, 
                                               {action:"FNCCALL", 
                                                context:null, 
                                                fnc:function() {
                                                  dlgEditUserAdmin_Destroy();
                                                }
                                               });
            
  var cmpBtnEditUserAdminCancel = new CmpButton([BlkJSX,"btnUserAdminCancel"],
                                               null, 
                                               {action:"FNCCALL", 
                                                context:null, 
                                                fnc:function() {
                                                  dlgEditUserAdmin_Destroy();
                                                }
                                               });
            
  var cmpBtnEditUserAdminDone = new CmpButton([BlkJSX,"btnUserAdminDone"],
                                               null, 
                                               {action:"FNCCALL", 
                                                context:null, 
                                                fnc:function() {
                                                      cmpDlgEditUserAdminSetUserAdmin.call(this, UserAdminNID);
                                                    }
                                               }
                                             );

  var cmpChkEditUserAdminRoleView = new CmpCheckBox([BlkJSX, "chkUserPolicy_RoleView"],
                                                      null,
                                                      { action: "FNCCALL",
                                                        context: null,
                                                        fnc: function() {
                                                          createUserAdminRoleDoc();
                                                        }
                                                      });

  var cmpChkEditUserAdminRoleDelete = new CmpCheckBox([BlkJSX, "chkUserPolicy_RoleDelete"],
                                                        null,
                                                        { action: "FNCCALL",
                                                          context: null,
                                                          fnc: function() {
                                                            createUserAdminRoleDoc();
                                                          }
                                                        });

  var cmpChkEditUserAdminRoleModify = new CmpCheckBox([BlkJSX, "chkUserPolicy_RoleModify"],
                                                        null,
                                                        { action: "FNCCALL",
                                                          context: null,
                                                          fnc: function() {
                                                            createUserAdminRoleDoc();
                                                          }
                                                        });

  var cmpChkEditUserAdminRoleAdd = new CmpCheckBox([BlkJSX, "chkUserPolicy_RoleAdd"],
                                                    null,
                                                    { action: "FNCCALL",
                                                      context: null,
                                                      fnc: function() {
                                                        createUserAdminRoleDoc();
                                                      }
                                                    });

  var cmpDlgEditUserAdmin = new CmpContainer([BlkJSX, "DialogUserAdmin"],
                                              [{ cmp:cmpBtnEditUserAdminClose },
                                                {cmp:cmpBtnEditUserAdminCancel },
                                                {cmp:cmpBtnEditUserAdminDone },
                                                {cmp:cmpChkEditUserAdminRoleView },
                                                {cmp:cmpChkEditUserAdminRoleDelete },
                                                {cmp:cmpChkEditUserAdminRoleModify },
                                                {cmp:cmpChkEditUserAdminRoleAdd },
                                                {cmp:cmpBlkUserAdminFilterMatrix},
                                                {cmp:cmpBlkUserAdminTargetFilterMatrix},
                                                {cmp:cmpBlkUserAdminRuleKeyMatrix},
                                                {cmp:cmpSelUserFilter},
                                                {cmp:cmpSelResourceFilter},
                                                {cmp:cmpSelTargetFilter},
                                                {cmp:cmpSelRuleKeyLHTag},
                                                {cmp:cmpSelRuleKeyRHTag},
                                                {cmp:cmpBtnAddUserAdminFilter},
                                                {cmp:cmpBtnAddUserAdminTargetFilter},
                                                {cmp:cmpBtnAddUserAdminRuleKey},
                                                {cmp:cmpCDFUserAdminPolicy}
                                              ],
                                              [{ action: "TOPICEVENT", topic: tDlg, eventvar: "EditUserAdminDataState", eventval: UserAdminNID}]);                            
                
  cmpDlgEditUserAdmin.BlkJSX = BlkJSX;
  cmpDlgEditUserAdmin.ObjectNID = UserAdminNID;   
  cmpDlgEditUserAdmin.getBlkData =  cmpDlgEditUserAdminGetUserAdmin;
  
  // init and repaint happens after the dialog is loaded
  jsx3.sleep( (function(){
                 this.init();
                 this.getBlkData(this.ObjectNID);
                 }
               ),
               null, cmpDlgEditUserAdmin );        
      
  // fnc to get dlg data
  function cmpDlgEditUserAdminGetUserAdmin(UserAdminNID) { 
    var UserAdminNode = topic_GetNode(tCache, "//*[@NID='" + UserAdminNID + "']");
    if( UserAdminNode == null ) {
      var reqData = new ReqGetObjectNID(UserAdminNID, "UserAdminPolicyRule",
                                         {action:"FNCCALL", context:this, 
                                           fnc:function(respCode, respVal) {
                                                 if(respCode == "SUCCESS" && respVal != null) {
                                                   cmpDlgEditUserAdminGetUserAdmin(UserAdminNID);
                                                 } else {
                                                   var codeErrorMsg = CommonErrorCodeArr[respCode].errorDesc;
                                                   var ErrorMsg = "Error: UserAdmin policy get data request failed \n";
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
      //cmpDlgEditUserAdmin.setMapRuleData("UserAdmin", "IN", "NID", UserAdminNID);
      //topic_Publish(tDlg, "EditUserAdminDataState", UserAdminNID);

      // using the old repaint of the dialog
      DisplayEditValuesForUserAdmin(UserAdminNID);
    }
  }
       
  // fnc to set dlg data - runs in the context of the OK button
  function cmpDlgEditUserAdminSetUserAdmin(UserAdminNID) {
    // validate input 
    if( cmpDlgEditUserAdminValidate() != true ) return;
    // decide add or update
    var SetMsg = "Edit UserAdmin policy";
    var SetMode = "Replace";
    if( UserAdminNID == "DEFAULT_USERADMIN_NID" ) {
      SetMsg = "Add UserAdmin policy";
      SetMode = "Add";
    }

    // cerate roleobject doc
    createUserAdminRoleDoc();
            
    // get UserAdmin node info from UI
    var UserAdminNode = null;

    // write the data back to the CDF
    cmpCDFUserAdminPolicy.BlkJSX.write();

    var strXMLCacheId = cmpCDFUserAdminPolicy.BlkJSX.getXMLId();
    var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);      
    var xslfile = PNameSpace.loadInclude("JSXAPPS/IPVS_AdminUI/xsl/UserAdminPolicyRequest.xsl","UserAdminPolicyRequest","xsl");
    var xslDoc = jsx3.xml.XslDocument.wrap(xslfile);
    var userAdminPolicyConfigXML = xslDoc.transform(objXML);
    UserAdminNode = new jsx3.xml.Document();
    UserAdminNode.loadXML(userAdminPolicyConfigXML);

    var strXMLCacheId = cmpBlkUserAdminFilterMatrix.BlkJSX.getXMLId();
    var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
    var xslfile = PNameSpace.loadInclude("jsxuser:///JSXAPPS/IPVS_AdminUI/xsl/CDFToPolicyUserFilter.xsl","ServiceOperatorFilterSetNode","xsl");
    var xslDoc = jsx3.xml.XslDocument.wrap(xslfile);
    var newServiceOperatorFilterSetXML = xslDoc.transform(objXML);        
    var newServiceOperatorFilterSetNode = new jsx3.xml.Document();
    newServiceOperatorFilterSetNode.loadXML(newServiceOperatorFilterSetXML);
    
    var oldServiceOperatorFilterSetNode = UserAdminNode.selectSingleNode(".//ServiceOperatorFilterSet");              
    var serviceOperatorFilterSetParentNode = UserAdminNode.selectSingleNode(".//ServiceOperatorFilterSet").getParent();
    serviceOperatorFilterSetParentNode.replaceNode(newServiceOperatorFilterSetNode,oldServiceOperatorFilterSetNode);

    var userAdminRoleSetNode = PNameSpace.getCache().getDocument("UserAdminRoleSetDoc");
    var xslfile = PNameSpace.loadInclude("jsxuser:///JSXAPPS/IPVS_AdminUI/xsl/UserAdminRoleSet.xsl","UserAdminRoleSetNode","xsl");
    var xslDoc = jsx3.xml.XslDocument.wrap(xslfile);
    var newUserAdminRoleSetXML = xslDoc.transform(userAdminRoleSetNode);        
    var newUserAdminRoleSetNode = new jsx3.xml.Document();
    newUserAdminRoleSetNode.loadXML(newUserAdminRoleSetXML);

    var oldUserAdminRoleSetNode = UserAdminNode.selectSingleNode(".//UserAdminRoleSet");              
    var userAdminRoleSetParentNode = UserAdminNode.selectSingleNode(".//UserAdminRoleSet").getParent();
    userAdminRoleSetParentNode.replaceNode(newUserAdminRoleSetNode,oldUserAdminRoleSetNode);

    var strXMLCacheId = cmpBlkUserAdminTargetFilterMatrix.BlkJSX.getXMLId();
    var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
    var xslfile = PNameSpace.loadInclude("jsxuser:///JSXAPPS/IPVS_AdminUI/xsl/CDFToUserAdminPolicyTargetFilter.xsl","UserAdminObjectFilterSetNode","xsl");
    var xslDoc = jsx3.xml.XslDocument.wrap(xslfile);
    var newUserAdminObjectFilterSetXML = xslDoc.transform(objXML);        
    var newUserAdminObjectFilterSetNode = new jsx3.xml.Document();
    newUserAdminObjectFilterSetNode.loadXML(newUserAdminObjectFilterSetXML);
    
    var oldUserAdminObjectFilterSetNode = UserAdminNode.selectSingleNode(".//UserAdminObjectFilterSet");              
    var userAdminObjectFilterSetParentNode = UserAdminNode.selectSingleNode(".//UserAdminObjectFilterSet").getParent();
    userAdminObjectFilterSetParentNode.replaceNode(newUserAdminObjectFilterSetNode,oldUserAdminObjectFilterSetNode);

    var strXMLCacheId = cmpBlkUserAdminRuleKeyMatrix.BlkJSX.getXMLId();
    var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
    var xslfile = PNameSpace.loadInclude("jsxuser:///JSXAPPS/IPVS_AdminUI/xsl/CDFToPolicyRuleKeyFilter.xsl","RuleKeyTagFilterSetNode","xsl");
    var xslDoc = jsx3.xml.XslDocument.wrap(xslfile);
    var newRuleKeyTagFilterSetXML = xslDoc.transform(objXML);        
    var newRuleKeyTagFilterSetNode = new jsx3.xml.Document();
    newRuleKeyTagFilterSetNode.loadXML(newRuleKeyTagFilterSetXML);
    
    var oldRuleKeyTagFilterSetNode = UserAdminNode.selectSingleNode(".//RuleKeyTagFilterSet");              
    var ruleKeyTagFilterSetParentNode = UserAdminNode.selectSingleNode(".//RuleKeyTagFilterSet").getParent();
    ruleKeyTagFilterSetParentNode.replaceNode(newRuleKeyTagFilterSetNode,oldRuleKeyTagFilterSetNode);
    
    if( UserAdminNode == null ) return;
    // set NID in UserAdmin node if replace mode
    if( SetMode == "Replace" ) {
      UserAdminNode.setAttribute("NID", UserAdminNID);
    }

    // do set request for UserAdmin              
    var reqSetUserAdminObject = new ReqSetObject(SetMode, "UserAdminPolicyRule", UserAdminNode,   
                                                  {action:"FNCCALL", context:this, 
                                                    fnc:function(respCode, respVal) {
                                                          if(respCode == "SUCCESS" && respVal != null) {
                                                            dlgEditUserAdmin_Destroy();
                                                            topic_Publish(tApp, "UserAdminPolicyRuleDataUpdate", "ADD");
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
    if( reqSetUserAdminObject != null) {
      reqSetUserAdminObject.sendRequest(this);
    }
  } 
      
  function cmpDlgEditUserAdminValidate() {
    var valUserPolicyName = getJsxByName([BlkJSX,"txtUserPolicy_Name"]).getValue();
    var valUserPolicyOrder = getJsxByName([BlkJSX,"txtUserPolicy_Order"]).getValue();
    var policyName = checkValue(valUserPolicyName , [BlkJSX,"txtUserPolicy_Name"] , "Please enter policy name");
    if(!policyName){
      return false;
    }
    var checkOrder = checkValue(valUserPolicyOrder , [BlkJSX,"txtUserPolicy_Order"], "Please enter policy order" );
    if(!checkOrder){
      return false;
    }
    var chkDuplicateName = "";
    var chkDuplicatePriority = "";
    var chkPermission = "";
    var ckhBoxArray = new Array();
    ckhBoxArray[0]= "chkUserPolicy_RoleAdd";
    ckhBoxArray[1]= "chkUserPolicy_RoleModify";
    ckhBoxArray[2]= "chkUserPolicy_RoleDelete";
    chkPermission = checkPermissionSelect(BlkJSX, "Please select Add, Modify or Delete Access Permissions", ckhBoxArray);
    if(!chkPermission) return chkPermission;

    if( UserAdminNID == "DEFAULT_USERADMIN_NID" ) {
      if(policyName){
        chkDuplicateName = chkForNameExistence(valUserPolicyName, [BlkJSX,"txtUserPolicy_Name"], "mtxpolicyUserAdminList", "Duplicate Policy name. Please enter some other Policy name" );
        if(!chkDuplicateName){
          checkOrder = false;
          return false;
        }
      }
      if(chkDuplicateName) {
        checkOrder = checkIntergerValue(valUserPolicyOrder , [BlkJSX,"txtUserPolicy_Order"] , "Please enter an integer policy order" );
      }
      if(checkOrder) {
        chkDuplicatePriority = chkForOrderExistence(valUserPolicyOrder, [BlkJSX,"txtUserPolicy_Order"], "mtxpolicyUserAdminList", "Policy Order already exists");
      } 
      if(chkDuplicatePriority) {
          return true;
      }       
    } else {
      checkOrder = checkIntergerValue(valUserPolicyOrder , [BlkJSX,"txtUserPolicy_Order"] , "Please enter an integer policy order" );
      if(checkOrder) {
        if( orgOrderNumber != valUserPolicyOrder) {
          chkDuplicatePriority = chkForOrderExistence(valUserPolicyOrder, [BlkJSX,"txtUserPolicy_Order"], "mtxpolicyUserAdminList", "Policy Order already exists");    
        } else {
          return true;
        }
      }
      if(chkDuplicatePriority){
        return true;
      } 
    }
          
    return false;
  }

  function DisplayEditValuesForUserAdmin(UserAdminNID) {
    var userAdminNode = topic_GetNode(tCache, ".//*[@NID='" + UserAdminNID + "']");

    /*var version = userAdminNode.getAttribute("Ver");
    var versionBlkJSX = getJsxByName([BlkJSX,"txtUserPolicyXMLVersion"]);
    versionBlkJSX.setValue(version);
    
    var revision = userAdminNode.getAttribute("Rev");
    var revisionBlkJSX = getJsxByName([BlkJSX,"txtUserPolicyXMLRevision"]);
    revisionBlkJSX.setValue(revision);    

    var serviceVer = userAdminNode.getAttribute("serviceVer");
    var serviceVerBlkJSX = getJsxByName([BlkJSX,"txtUserPolicyXMLServiceVersion"]);
    serviceVerBlkJSX.setValue(serviceVer);    

    var jsxtext = userAdminNode.selectSingleNode(".//rulename").getValue();
    var filterNameBlkJSX = getJsxByName([BlkJSX,"txtUserPolicy_Name"]);
    filterNameBlkJSX.setValue(jsxtext);    

    var index = userAdminNode.selectSingleNode(".//order").getValue();
    var filterOrderBlkJSX = getJsxByName([BlkJSX,"txtUserPolicy_Order"]);
    filterOrderBlkJSX.setValue(index);     

    var policyValid = userAdminNode.selectSingleNode(".//valid").getValue();
    var filterActiveBlkJSX = getJsxByName([BlkJSX,"chkUserPolicy_Active"]);
    if(policyValid == "true") {
      filterActiveBlkJSX.setChecked(jsx3.gui.CheckBox.CHECKED);
    } else {
      filterActiveBlkJSX.setChecked(jsx3.gui.CheckBox.UNCHECKED);
    }

    var jsxUserPolicyType = userAdminNode.selectSingleNode(".//allow").getValue();
    var filterTypeAllowBlkJSX = getJsxByName([BlkJSX,"radioUserPolicy_PolicyTypeAllow"]);
    filterTypeAllowBlkJSX.setSelected(jsx3.gui.RadioButton.SELECTED);
    /*var filterTypeDenyBlkJSX = getJsxByName([BlkJSX,"radioUserPolicy_PolicyTypeDeny"]);
    if(jsxUserPolicyType == "true") {
      filterTypeAllowBlkJSX.setSelected(jsx3.gui.RadioButton.SELECTED);
      filterTypeDenyBlkJSX.setSelected(jsx3.gui.RadioButton.UNSELECTED);
    } else {
      filterTypeAllowBlkJSX.setSelected(jsx3.gui.RadioButton.UNSELECTED);
      filterTypeDenyBlkJSX.setSelected(jsx3.gui.RadioButton.SELECTED);
    }


    var xAuthRuleListIter  = userAdminNode.selectNodeIterator(".//ExternalCondition/XAuth/RuleKey");
    while( xAuthRuleListIter.hasNext() ) {
      var xAuthRuleListIterNode = xAuthRuleListIter.next();
      var conditionAction = xAuthRuleListIterNode.getValue();

      if(conditionAction == "MEDIAROOM_RULE_CONFCREATE_USER") {
        var filterxAuthUserBlkJSX = getJsxByName([BlkJSX,"chkUserPolicy_XAuthUseServiceOperator"]);
        filterxAuthUserBlkJSX.setChecked(jsx3.gui.CheckBox.CHECKED);
      }
    }

    //
    var xAuthServiceOpListIter  = userAdminNode.selectNodeIterator(".//ExternalCondition/XAuth/ServiceXAuthOperatorFilterSet/ServiceOperatorFilter");
    var mtxUserPolicyXAuthUserListBlkJSX = getJsxByName([BlkJSX,"mtxUserPolicyXAuthUserList"]);
    mtxUserPolicyXAuthUserListBlkJSX.resetCacheData();
    while( xAuthServiceOpListIter.hasNext() ) {
      var xAuthServiceOpListIterNode = xAuthServiceOpListIter.next();
      var UserFilterNID = xAuthServiceOpListIterNode.selectSingleNode(".//UserFilterNID").getValue();
      var ResourceFilterNID = xAuthServiceOpListIterNode.selectSingleNode(".//ResourceFilterNID").getValue();
      if( (UserFilterNID != "") && (ResourceFilterNID != "")) {
        var UserFilterName = getTagObjectFilterName(UserFilterNID);
        var ResourceFilterName = getTagObjectFilterName(ResourceFilterNID);
        
        var o = new Object();
        o.jsxid = jsx3.xml.CDF.getKey();
        o.jsxtextOne = UserFilterName ;
        o.jsxtextOneNID = UserFilterNID ;
        o.jsxtextTwo = ResourceFilterName ;
        o.jsxtextTwoNID = ResourceFilterNID ;
        o.jsxAt = "@";
        
        mtxUserPolicyXAuthUserListBlkJSX.insertRecord(o, null, false);
      }
    }
    mtxUserPolicyXAuthUserListBlkJSX.repaintData();

    //
    var roleListIter  = userAdminNode.selectNodeIterator(".//UserAdminRoleSet/UserAdminObjectRole");
    while( roleListIter.hasNext() ) {
      var roleListIterNode = roleListIter.next();
      var roleObject = roleListIterNode.selectSingleNode(".//RoleObject").getValue();
      var roleAction = roleListIterNode.selectSingleNode(".//RoleAction").getValue();

      if( roleAction == "Add") {
        getJsxByName([BlkJSX,"chkUserPolicy_RoleAdd"]).setChecked(jsx3.gui.CheckBox.CHECKED);
      }

      if( roleAction == "Modify") {
        getJsxByName([BlkJSX,"chkUserPolicy_RoleModify"]).setChecked(jsx3.gui.CheckBox.CHECKED);
      }

      if( roleAction == "Delete") {
        getJsxByName([BlkJSX,"chkUserPolicy_RoleDelete"]).setChecked(jsx3.gui.CheckBox.CHECKED);
      }
      // cerate roleobject doc
      createUserAdminRoleDoc();      
    }*/

    var userAdminNID = userAdminNode.getAttribute("NID");
    var strXMLCacheId = cmpCDFUserAdminPolicy.BlkJSX.getXMLId();
    var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
    objXML.loadXML(userAdminNode.toString());
    cmpCDFUserAdminPolicy.BlkJSX.setSourceXML(objXML);         
    cmpCDFUserAdminPolicy.BlkJSX.setCDFId(userAdminNID);

    var serviceOpListIter  = userAdminNode.selectNodeIterator(".//ServiceOperatorFilterSet/ServiceOperatorFilter");
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
    var strXMLCacheId = cmpBlkUserAdminFilterMatrix.BlkJSX.getXMLId();
    var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
    objXML.loadXML(objDoc.toString());
    cmpBlkUserAdminFilterMatrix.BlkJSX.setSourceXML(objXML);
    cmpBlkUserAdminFilterMatrix.BlkJSX.repaintData();

    //
    var targetFilterListIter  = userAdminNode.selectNodeIterator(".//UserAdminObjectFilterSet/UserAdminObjectFilterNID");
    var objTargetDoc = jsx3.xml.CDF.Document.newDocument();
    while( targetFilterListIter.hasNext() ) {
      var targetFilterListIterNode = targetFilterListIter.next();
      var tareFilterNID = targetFilterListIterNode.getValue();
      if(tareFilterNID != "") {
        var targetFilterName = getTagObjectFilterName(tareFilterNID);
        
        var o = new Object();
        o.jsxid = jsx3.xml.CDF.getKey();
        o.targetFilterName = targetFilterName ;
        o.tareFilterNID = tareFilterNID ;
        objTargetDoc.insertRecord(o);
      }
    }
    var strXMLCacheId = cmpBlkUserAdminTargetFilterMatrix.BlkJSX.getXMLId();
    var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
    objXML.loadXML(objTargetDoc.toString());
    cmpBlkUserAdminTargetFilterMatrix.BlkJSX.setSourceXML(objXML);
    cmpBlkUserAdminTargetFilterMatrix.BlkJSX.repaintData();

    //
    var ruleKeyListIter  = userAdminNode.selectNodeIterator(".//RuleKeyTagFilterSet/RuleKeyTagFilter");
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
    var strXMLCacheId = cmpBlkUserAdminRuleKeyMatrix.BlkJSX.getXMLId();
    var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
    objXML.loadXML(objRuleKeyDoc.toString());
    cmpBlkUserAdminRuleKeyMatrix.BlkJSX.setSourceXML(objXML);
    cmpBlkUserAdminRuleKeyMatrix.BlkJSX.repaintData();

    // cerate roleobject doc
    createUserAdminRoleDoc();      
    
    // populate the select with the default values
    updateSelectTagObjectFilters();

    // populate the select rule key select with the default value
    updateSelectGroupTag();
   
    if( UserAdminNID != "DEFAULT_USERADMIN_NID" ) {
      getJsxByName([BlkJSX,"txtUserPolicy_Name"]).setEnabled(jsx3.gui.Form.STATEDISABLED, true);
    }
    orgOrderNumber = getJsxByName([BlkJSX,"txtUserPolicy_Order"]).getValue();
    getJsxByName([BlkJSX,"txtUserPolicy_Name"]).focus();
  } // end 

  function updateSelectTagObjectFilters() {
    //getFilterToSelectElement([BlkJSX,"selUserPolicyXAuthUserFilter"],"User");
    //getFilterToSelectElement([BlkJSX,"selUserPolicyXAuthResourceFilter"],"Resource");
    getFilterToSelectElement([BlkJSX,"selUserFilter_Users"],"User");
    getFilterToSelectElement([BlkJSX,"selUserFilter_Resource"],"Resource");
    getFilterToSelectElement([BlkJSX,"selUserFilter_Target"],"User");
  }
  
  function updateSelectGroupTag() {
    getTagDefToSelectElement([BlkJSX,"selUserPolicy_RuleKeyLHTag"],"ALL");
    getTagDefToSelectElement([BlkJSX,"selUserPolicy_RuleKeyRHTag"],"ALL"); 
  }

  //creating the CreatSession Access roleDoc
  function createUserAdminRoleDoc() {
    var RoleObjectArr = new Array();
    RoleObjectArr[0] = "User";

    var roleCacheDoc = PNameSpace.getCache().getDocument("UserAdminRoleSetDoc");
    if (roleCacheDoc) {
      delete roleCacheDoc;
    }

    var UserAdminRoleSet = new jsx3.xml.CDF.Document.newDocument();
    PNameSpace.Cache.setDocument("UserAdminRoleSetDoc", UserAdminRoleSet);

    updateReadPolicy(BlkJSX, "chkUserPolicy_RoleAdd", "chkUserPolicy_RoleModify", "chkUserPolicy_RoleDelete", "chkUserPolicy_RoleView");
    var roleAdd = getJsxByName([BlkJSX, "chkUserPolicy_RoleAdd"]).getChecked();
    var roleModify = getJsxByName([BlkJSX, "chkUserPolicy_RoleModify"]).getChecked();
    var roleDelete = getJsxByName([BlkJSX, "chkUserPolicy_RoleDelete"]).getChecked();
    var roleView = getJsxByName([BlkJSX, "chkUserPolicy_RoleView"]).getChecked();

    if (roleAdd) {
      for (i = 0; i < RoleObjectArr.length; i++) {
        var o = new Object();
        o.jsxid = jsx3.xml.CDF.getKey();
        o.jsxroleObject = RoleObjectArr[i];
        o.jsxroleAction = "Add";
        UserAdminRoleSet.insertRecord(o, null, false);
      }
    }

    if (roleModify) {
      for (i = 0; i < RoleObjectArr.length; i++) {
        var o = new Object();
        o.jsxid = jsx3.xml.CDF.getKey();
        o.jsxroleObject = RoleObjectArr[i];
        o.jsxroleAction = "Modify";
        UserAdminRoleSet.insertRecord(o, null, false);
      }
    }

    if (roleDelete) {
      for (i = 0; i < RoleObjectArr.length; i++) {
        var o = new Object();
        o.jsxid = jsx3.xml.CDF.getKey();
        o.jsxroleObject = RoleObjectArr[i];
        o.jsxroleAction = "Delete";
        UserAdminRoleSet.insertRecord(o, null, false);
      }
    }
    if (roleView) {
      for (i = 0; i < RoleObjectArr.length; i++) {
        var o = new Object();
        o.jsxid = jsx3.xml.CDF.getKey();
        o.jsxroleObject = RoleObjectArr[i];
        o.jsxroleAction = "Read";
        UserAdminRoleSet.insertRecord(o, null, false);
      }
    }
  }
  
}