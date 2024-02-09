// this code has some potential issues 
// - w.r.t object reference counts - when objects created in here go out of scope etc
// - loading sync issues - dlg may not be loaded before the object init methods are called

function dlgAssetMenu_Create(AssetNID) {

  if( AssetNID == null ) return;
  
  // load view - dialog component file
  var DlgFile = "components/asset/AssetMonitor.xml";
  var BlkJSX = PNameSpace.getBodyBlock().load(DlgFile);
  
  var tDlg = "Dialog_" + jsx3.xml.CDF.getKey()
  topic_Create(tDlg);
  var eventDlg = new EventListner(tDlg);
  
  function dlgEditUser_Destroy() {
    // destroy any local eventlistners
    eventDlg.destroy();
    
    // destroy all local components
    cmpDlgEditUser.destroy();

    // destroy the local topic                  
    topic_Destroy(tDlg);
    
    // close dialog    
    BlkJSX.doClose();
  }

  var cmpBtnEditUserAddGroup = new CmpButton("btnUserValueTagAdd",
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
                                          o.jsxid = jsx3.xml.CDF.getKey();
                                          o.jsxUserTagName = groupTagName;
                                          o.jsxUserTagValue = groupTagValue;
                                          this.GroupTagListBlkJSX.insertRecord(o, null, false);
                                          this.GroupTagNameBlkJSX.setValue(null);
                                          this.GroupTagValueBlkJSX.setValue(null);
                                          this.GroupTagListBlkJSX.repaintData();
                                        }
                                      });
    cmpBtnEditUserAddGroup.GroupTagNameBlkJSX = PNameSpace.getJSXByName("cmbUserAddTagDefName");
    cmpBtnEditUserAddGroup.GroupTagValueBlkJSX = PNameSpace.getJSXByName("cmbUserAddTagDefValue");
    cmpBtnEditUserAddGroup.GroupTagListBlkJSX = PNameSpace.getJSXByName("mtxUserTagSelected");
    
    var cmpBtnEditUserClose = new CmpButton("btnUserClose",
                                             null, 
                                             {action:"FNCCALL", 
                                              context:null, 
                                              fnc:function() {
                                                dlgEditUser_Destroy();
                                              }
                                             });
            
    var cmpBtnEditUserCancel = new CmpButton("btnUserAccountCancel",
                                             null, 
                                             {action:"FNCCALL", 
                                              context:null, 
                                              fnc:function() {
                                                dlgEditUser_Destroy();
                                              }
                                             });
            
    var cmpBtnEditUserOK = new CmpButton("btnUserAccountDone",
                                             null, 
                                             {action:"FNCCALL", 
                                              context:null, 
                                              fnc:function() {
                                                cmpDlgEditUserSetUser.call(this, UserNID);
                                              }
                                             });

  var cmpCmbSelUserTagDefValue = new CmpNIDElementList("cmbUserAddTagDefValue",
                                        "UserTagDefNameSelected",
                                        ".//TagValueList/TagValue",
                                        {
                                         jsxtext:{objxpath:"\".\""}
                                        },
                                        [{action:"REQEVENT", eventvar:"GroupTagDataState", eventval:null},
                                         {action:"APPEVENT", eventvar:"UserTagDefNameSelected", eventval:null}],
                                        {persist:{col:"jsxtext",initval:false},
                                         action:"APPEVENT", topic:tDlg, eventvar:"UserTagDefValueSelected", eventval:"jsxtext"
                                        }
                                       );
  var cmpCmbSelUserTagDefName = new CmpNIDList( "cmbUserAddTagDefName",
                                           "GroupTagNIDList",
                                           null, 
                                           null,
                                           {jsxid:{objxpath:"\"./@NID\""},
                                            jsxtext:{objxpath:"\".//TagName\""}},
                                           [{action:"REQEVENT", eventvar:"GroupTagDataState", eventval:null}],
                                           {persist:{col:"jsxtext",initval:false},
                                            action:"APPEVENT", eventvar:"UserTagDefNameSelected", eventval:"jsxid"}
                                           );                                     
                                                                                      
    var cmpDlgEditUser = new CmpContainer("DialogUser",
                              [ {cmp:cmpCmbSelUserTagDefName},
                                {cmp:cmpCmbSelUserTagDefValue},
                                {cmp:cmpBtnEditUserClose},
                                {cmp:cmpBtnEditUserCancel},
                                {cmp:cmpBtnEditUserOK},
                                {cmp:cmpBtnEditUserAddGroup}
                              ],
                              [{action:"TOPICEVENT", topic:tDlg, eventvar:"EditUserDataState", eventval:UserNID}]);                            
    
    cmpDlgEditUser.BlkJSX = BlkJSX;   
    cmpDlgEditUser.ObjectNID = UserNID;
    cmpDlgEditUser.getBlkData =  cmpDlgEditUserGetUser;
  
    // init and repaint happens after the dialog is loaded
    jsx3.sleep( (function(){
                 this.init();
                 
                // disable login for edit - shoudl make this a component
                if( this.ObjectNID != "DEFAULT_USER_NID" ) {
                var cmpTxtUserLoginNameBlkJSX = PNameSpace.getJSXByName("txtUserLoginName");
                cmpTxtUserLoginNameBlkJSX.setEnabled(jsx3.gui.Form.STATEDISABLED, true); 
                }
                 
                 this.getBlkData(this.ObjectNID);
                 }
               ),
               null, cmpDlgEditUser );
 
      
    // fnc to get dlg data
    function cmpDlgEditUserGetUser(UserNID) { 
      var userNode = topic_GetNode(tCache, "//*[@NID='" + UserNID + "']");
      if( userNode == null ) {
        var reqUserData = new ReqGetObjectNID(UserNID, "User",
                                                {action:"FNCCALL", context:this, 
                                                  fnc:function(respCode, respVal) {
                                                        if(respCode == "SUCCESS" && respVal != null) {
                                                          cmpDlgEditUserGetUser(UserNID);
                                                        } else {
                                                           var codeErrorMsg = CommonErrorCodeArr[respCode].errorDesc;
                                                           var ErrorMsg = "Error: Get user data request failed \n";
                                                           ErrorMsg += "Error description: " + codeErrorMsg + "\n";
                                                           ErrorMsg += "Error code: " + respCode;
                                                           alert(ErrorMsg);
                                                         }
                                                      }
                                                } 
                                             );
        reqUserData.sendRequest();
      }
      else {
        cmpDlgEditUser.setMapRuleData("User", "IN", "NID", UserNID);
        topic_Publish(tDlg, "EditUserDataState", UserNID);
      }
    }
       
    // fnc to set dlg data - runs in the context of the OK button
    function cmpDlgEditUserSetUser(UserNID) {
      // validate input 
      if( cmpDlgEditUserValidate() != true ) return;
      // decide add or update
      var SetMsg = "Edit user";
      var SetMode = "Replace";
      if( UserNID == "DEFAULT_USER_NID" ) {
        SetMsg = "Add user";
        SetMode = "Add";
      }
      // setup user map rule info
      var userMapRule = new Object;
      userMapRule.state = false;
      userMapRule.objtype = "User";
      userMapRule.rule = "User";
          
      // get User node info from UI
      var userNode = utilRuleGetOutboundDoc.call(this, userMapRule.rule);
      if( userNode == null ) return;
      // set NID in user node if replace mode
      if( SetMode == "Replace" ) {
        userNode.setAttribute("NID", UserNID);
      }
      // do set request for User
      var reqSetUserObject = new ReqSetObject(SetMode, userMapRule.objtype, userNode, 
                                                {action:"FNCCALL", context:this, 
                                                   fnc:function(respCode, respVal) {
                                                         if(respCode == "SUCCESS" && respVal != null) {
                                                           dlgEditUser_Destroy();
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
      reqSetUserObject.sendRequest();
    } 
    
    function  chkForUserLoginNameExistence(a_loginName, a_fieldName, a_Msg) {
      if( topic_Get(tApp, "UserDataState") != null) {
        var userNIDListNode = topic_GetNode(tApp, "//UserNIDList");
        if( userNIDListNode != null ) {
          var userNIDListIter = userNIDListNode.selectNodeIterator(".//ObjectNID");
          while( userNIDListIter.hasNext() ) {
            var userNID = userNIDListIter.next().getValue();
            var userNode = topic_GetNode(tCache, ".//*[@NID='" + userNID + "']");
            var userLoginName = userNode.selectSingleNode(".//Username").getValue();
            if( userLoginName == a_loginName) {
              alert(a_Msg);
              PNameSpace.getJSXByName(a_fieldName).focus();
              return false;
            }
          }
        }
      }
      return true;
    }
    
    function cmpDlgEditUserValidate() {
      var valFullName = PNameSpace.getJSXByName("txtUserFullName").getValue();
      var valLoginName = PNameSpace.getJSXByName("txtUserLoginName").getValue();
      var valPassword = PNameSpace.getJSXByName("txtUserPassword").getValue();
      var valEmail = PNameSpace.getJSXByName("txtUserEmail").getValue();
      //var valSelRecordID = PNameSpace.getJSXByName("mtxUserTagSelected").getValue();

      var fullName = checkValue(valFullName , "txtUserFullName" , "Please enter full name");
      if(fullName) {
        var loginName = checkValue(valLoginName , "txtUserLoginName" , "Please enter login name");
        var chkDuplicateLoginName = "true";
        if( UserNID == "DEFAULT_USER_NID" ) {
          chkDuplicateLoginName = chkForUserLoginNameExistence(valLoginName, "txtUserLoginName", "Duplicate Login name. Please enter some other Login name" );
        }
      }
      if(loginName && chkDuplicateLoginName) {
        var pass = validatePassword(valPassword,"txtUserPassword","");   
      }
      if(pass)
        var email = checkValue(valEmail , "txtUserEmail" , "Please enter email address");
/*
      if(email)
        var tagSelected = checkValue(valSelRecordID , "mtxUserTagSelected" , "Please select tag ");
      if(tagSelected)
        cmpUser_Add_Request();
*/
      if(email)
        return true;
        
      return false;
    }

       
}

