// this code has some potential issues 
// - w.r.t object reference counts - when objects created in here go out of scope etc
// - loading sync issues - dlg may not be loaded before the object init methods are called

function dlgEditUser_Create(UserNID) {

  // deal with defaults for add
  if( UserNID == null ) UserNID = "DEFAULT_USER_NID";
  
  // load view - dialog component file
  var DlgFile = "components/users/usersUserDialog.xml";
  var BlkJSX = PNameSpace.getBodyBlock().load(DlgFile);
  
  var tDlg = "Dialog_" + jsx3.xml.CDF.getKey()
  topic_Create(tDlg);
  var eventDlg = new EventListner(tDlg);
  
  var dlgText = "Add User";
  if( UserNID != "DEFAULT_USER_NID" ){
    dlgText = "Edit User";
  }
  var windowBarBlkJSX = getJsxByName([BlkJSX, "windowBar"]);
  windowBarBlkJSX.setText(dlgText, true);
  closeDialogIfExists(dlgText);

  topic_Set(tDlg, "DefTagSelected", "All");

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

  var cmpBlkEditUserGroupMatrix = new CmpBlock([BlkJSX,"mtxUserTagSelected"]);
  cmpBlkEditUserGroupMatrix.BlkJSX.setXMLTransformers("xsl/TagToCDFTag.xsl");
  cmpBlkEditUserGroupMatrix.BlkJSX.setSelectionModel(jsx3.gui.Matrix.SELECTION_ROW);
  
  var cmpCDFEditUser = new Component([BlkJSX,"cdfDlgEditUser"]);
  cmpCDFEditUser.BlkJSX.setXMLTransformers("xsl/EditUser.xsl");

  var cmpSelDefaultTag = new Component([BlkJSX, "selDefaultTag"]);
  var cmpChkEndPointTableConfigConfigurable = new Component([BlkJSX, "chkEndPointTableConfigConfigurable"]);


  var cmpMtxEPDstPort = new CmpBlock([BlkJSX, "mtxEPDstPort"]);
  cmpMtxEPDstPort.BlkJSX.setXMLTransformers("xsl/DstPortsNameList.xsl");

  var cmpCmbSelUserTagDefName = new CmpCustomPaginationList([BlkJSX, "cmbUserAddTagDefName"],
                                                           [{action:"TOPICEVENT", topic: tDlg, eventvar:"UserTagDefNameState", eventval:null}],
                                                             { persist: { col: "jsxtext", initval: false },
                                                                 action: "TOPICEVENT", topic: tDlg, eventvar: "UserTagDefNameSelected", eventval: "jsxid"
                                                             },
                                                          null,
                                                          null,
                                                          "xsl/TagNameList.xsl"
                                                          );

  cmpCmbSelUserTagDefName.cmpReqParamArr = { StartFrom: "ObjectList_StartFrom",
                                              CountToFetch: "ObjectList_CountToFetch",
                                              SortOrder: "ObjectList_SortOrder",
                                              ObjectCount: "ObjectCount",
                                              SortByXpath: "ObjectList_SortBy",
                                              WhereObjectType: "ObjectList_WhereObjectType"
                                          };

  topic_Set(tDlg, "UserTagNameResponseSelectFormat", "NameValueTree");
  topic_Set(tDlg, "UserTagNameResponseObjectType", "Users");

  cmpCmbSelUserTagDefName.cmpReq = new cmpReq_GetSessionAPIPaginationList(tDlg, "GetTags",
                                                                          [{ VarName: "ObjectList_SelectFormat", VarRef: "UserTagNameResponseSelectFormat" },
                                                                           { VarName: "ObjectList_WhereObjectType", VarRef: "UserTagNameResponseObjectType"}],
                                                                          { action: "FNCCALL", context: cmpCmbSelUserTagDefName,
                                                                            fnc: function (respCode, respVal) {
                                                                              if (respCode == "SUCCESS" && respVal != null) {
                                                                                var defTagXML = this.onResponse(respCode, respVal);
                                                                                updateSelDefaultTag(defTagXML);
                                                                              }
                                                                              else {
                                                                                log.info("GetTagsRequest failed. Error Code :" + respCode);
                                                                                this.onResponseErrorHandle("GetTags", respCode, respVal);
                                                                              }
                                                                            }
                                                                          }
                                                                        );

  var cmpCmbSelUserTagDefValue = new Component([BlkJSX, "cmbUserAddTagDefValue"]);
  cmpCmbSelUserTagDefValue.BlkJSX.setXMLTransformers("xsl/TagValueList.xsl");

  eventDlg.subscribe("UserTagDefNameSelected", null, this,
                      function(eventVar, eventVal) {
                        if (eventVal != "NONE") {
                          cmpCmbSelUserTagDefValue.BlkJSX.setValue("");
                          var nameRecord = cmpCmbSelUserTagDefName.BlkJSX.getValue();
                          var selNameRecord = cmpCmbSelUserTagDefName.BlkJSX.getRecordNode(nameRecord);
                          var tagNode = selNameRecord.selectSingleNode(".//Tag");
                          var strXMLValueCacheId = cmpCmbSelUserTagDefValue.BlkJSX.getXMLId();
                          var objXML = PNameSpace.getCache().getDocument(strXMLValueCacheId);
                          objXML.loadXML(tagNode.toString());
                          cmpCmbSelUserTagDefValue.BlkJSX.setSourceXML(objXML); 
                        }
                      }
                    );

  var cmpBtnEditUserAddGroup = new CmpButton([BlkJSX,"btnUserValueTagAdd"],
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
                                              
  cmpBtnEditUserAddGroup.GroupTagNameBlkJSX = cmpCmbSelUserTagDefName.BlkJSX;
  cmpBtnEditUserAddGroup.GroupTagValueBlkJSX = cmpCmbSelUserTagDefValue.BlkJSX;
  cmpBtnEditUserAddGroup.GroupTagListBlkJSX = cmpBlkEditUserGroupMatrix.BlkJSX;

  var cmpBtnEditUserClose = new CmpButton([BlkJSX,"btnDialogCloseX"],
                                           null, 
                                           {action:"FNCCALL", 
                                            context:null, 
                                            fnc:function() {
                                              dlgEditUser_Destroy();
                                            }
                                           });
            
  var cmpBtnEditUserCancel = new CmpButton([BlkJSX,"btnUserAccountCancel"],
                                           null, 
                                           {action:"FNCCALL", 
                                            context:null, 
                                            fnc:function() {
                                              dlgEditUser_Destroy();
                                            }
                                           });
            
  var cmpBtnEditUserOK = new CmpButton([BlkJSX,"btnUserAccountDone"],
                                           null, 
                                           {action:"FNCCALL", 
                                            context:null, 
                                            fnc:function() {
                                              cmpDlgEditUserSetUser.call(this, UserNID);
                                            }
                                           });


                                                                                   
  var cmpTxtaUserPrivateData = new Component([BlkJSX,"txtUserPrivateData"]);

  var cmpTxtEditUserPassword = new CmpTxtBoxWithBlur([BlkJSX, "txtUserPassword"], null, null, 
                                                      {action:"FNCCALL", 
                                                         context:null, 
                                                         fnc:function() {
                                                               var passwordVal = this.BlkJSX.getValue();
                                                               if(!jsx3.util.strEmpty(passwordVal)) {
                                                                 //var chkDevicePassword = validatePassword(passwordVal, [BlkJSX,"txtUserPassword"], null);
                                                               }
                                                         }
                                                      }                                             
                                                     );

  var cmpTxtEditUserVerifyPassword = new CmpTxtBoxWithBlur([BlkJSX, "txtUserVerifyPassword"], null, null, 
                                                            {action:"FNCCALL", 
                                                               context:null, 
                                                               fnc:function() {
                                                                     var userPassword = cmpTxtEditUserPassword.BlkJSX.getValue();
                                                                     if(!jsx3.util.strEmpty(userPassword) ) {
                                                                       var chkUserValidateVerifyPassword = validateVerifyPassword(userPassword, this.BlkJSX.getValue(), [BlkJSX,"txtUserPassword"], null);
                                                                     }
                                                               }
                                                            }                                                           
                                                           );

      
  var cmpTxtUserFullName = new Component([BlkJSX,"txtUserFullName"]);
  var cmpTxtUserLoginName = new Component([BlkJSX,"txtUserLoginName"]);
  var cmpTxtUserEmail = new Component([BlkJSX,"txtUserEmail"]);
  
  var cmpChkExternalAuthentication = new CmpCheckBox([BlkJSX,"chkExternalAuthentication"],
                                              null,
                                              {action:"FNCCALL", 
                                               context:null, 
                                               fnc:function() {
                                                   handleEnableDisableExternalAuth();
                                               }
                                              }
                                             );

  var cmpDlgEditUser = new CmpContainer([BlkJSX, "DialogUser"],
                                        [{ cmp: cmpTxtaUserPrivateData },
                                          { cmp: cmpCmbSelUserTagDefName },
                                          { cmp: cmpCmbSelUserTagDefValue },
                                          { cmp: cmpBtnEditUserClose },
                                          { cmp: cmpBtnEditUserCancel },
                                          { cmp: cmpBtnEditUserOK },
                                          { cmp: cmpBtnEditUserAddGroup },
                                          { cmp: cmpBlkEditUserGroupMatrix },
                                          { cmp: cmpCDFEditUser },
                                          { cmp: cmpTxtEditUserPassword },
                                          { cmp: cmpTxtEditUserVerifyPassword },
                                          { cmp: cmpTxtUserFullName },
                                          { cmp: cmpTxtUserLoginName },
                                          { cmp: cmpChkExternalAuthentication },
                                          { cmp: cmpSelDefaultTag },
                                          { cmp: cmpChkEndPointTableConfigConfigurable },
                                          { cmp: cmpMtxEPDstPort }
                                              //,{cmp:cmpTxtUserEmail}
                                        ],
                                        [{ action: "TOPICEVENT", topic: tDlg, eventvar: "EditUserDataState", eventval: UserNID}]
                                       );                            
                
  cmpDlgEditUser.BlkJSX = BlkJSX;   
  cmpDlgEditUser.ObjectNID = UserNID;
  cmpDlgEditUser.getBlkData = cmpReq_GetUser;
  cmpDlgEditUser.tDlg = tDlg;
  
  // init and repaint happens after the dialog is loaded
  jsx3.sleep( (function(){
                this.init();
                // disable login for edit - shoudl make this a component
                if( this.ObjectNID != "DEFAULT_USER_NID" ) {
                  var cmpTxtUserLoginNameBlkJSX = PNameSpace.getJSXByName("txtUserLoginName");
                  cmpTxtUserLoginNameBlkJSX.setEnabled(jsx3.gui.Form.STATEDISABLED, true);
                }
                topic_Publish(tDlg, "UserTagDefNameState", (newServerDate()).getTime());
                this.getBlkData(this.tDlg, this.ObjectNID, "EXTENDED");
               }
              ),
            null, cmpDlgEditUser );

   // event published from func ( cmpReq_GetUser ) defined in cmpReq.js
   eventDlg.subscribe("GetCompleteUserDone", UserNID, this,
                      function () {
                        cmpDlgEditUser.BlkJSX.showMask("Loading...");
                        var reqGetPort = new ReqSessionGetRequest("GetPort",
                                                { action: "FNCCALL", context: null,
                                                  fnc: function (respCode, respVal) {
                                                    if (respCode == "SUCCESS" && respVal != null) {
                                                      var strXMLValueCacheId = cmpMtxEPDstPort.BlkJSX.getXMLId();
                                                      var objXML = PNameSpace.getCache().getDocument(strXMLValueCacheId);
                                                      objXML.loadXML(respVal.toString());
                                                      cmpMtxEPDstPort.BlkJSX.setSourceXML(objXML);
                                                      cmpMtxEPDstPort.BlkJSX.repaint();

                                                      cmpDlgEditUserGetUser(UserNID);
                                                    }
                                                    else {
                                                      var codeErrorMsg = CommonErrorCodeArr[respCode].errorDesc;
                                                      var ErrorMsg = "Error: GetPort request failed \n";
                                                      ErrorMsg += "Error description: " + codeErrorMsg + "\n";
                                                      ErrorMsg += "Error code: " + respCode;
                                                      alert(ErrorMsg);
                                                      return false;
                                                    }
                                                  }
                                                }
                                               );

                        reqGetPort.ServiceName = "SessionService";
                        var reqParams = new Object;
                        reqParams["ObjectList_Type"] = "DstPort";
                        reqParams["ObjectList_SelectFormat"] = "BASIC";
                        reqParams["ObjectList_SortOrder"] = "ascending";
                        reqParams["ObjectList_SortBy"] = "title";
                        reqParams["ObjectList_WherePermissions"] = "STREAMTO";
                        reqGetPort.reqParams = reqParams;
                        // send the request
                        reqGetPort.sendRequest(this);



                      }
                    );   // subscribe end


    function updateSelDefaultTag(defTagXML) {
      var strXMLValueCacheId = cmpSelDefaultTag.BlkJSX.getXMLId();
      var objXML = PNameSpace.getCache().getDocument(strXMLValueCacheId);
      objXML.loadXML(defTagXML);
      cmpSelDefaultTag.BlkJSX.setSourceXML(objXML);
      var o = new Object;
      o.jsxid = "All";
      o.jsxtext = "All";
      cmpSelDefaultTag.BlkJSX.insertRecord(o, null, true);
      cmpSelDefaultTag.BlkJSX.setValue(topic_Get(tDlg, "DefTagSelected"));
    }
                            
    // fnc to get dlg data
    function cmpDlgEditUserGetUser(UserNID) {
      var reqStatus = topic_Get(tDlg, "GetCompleteUserStatus");
      if (reqStatus != "SUCCESS") {
        cmpDlgEditUser.BlkJSX.hideMask();
        dlgEditUser_Destroy();  
      }      
      var userNode = null;
      if( UserNID == "DEFAULT_USER_NID" ){
        userNode = topic_GetNode(tCache, "//*[@NID='" + UserNID + "']");
      }
      else{
        userNode = getDialogDataNode(tDlg, "User");
      }
      // deleting empty group tag name and value from the default xml
      deleteEmptyGroupTag(userNode);
      
      //Following change is a workaround to handle stripping of XML tags in UserPrivateData.
      //And needs to be reverted when the middleware is fixed.
      var userPrivateDataNode = userNode.selectSingleNode(".//UserPrivateData");
      var privatDataChildIter = userPrivateDataNode.getChildIterator();
      var strUserPrivateData = "";
      if( privatDataChildIter._next != null ) {
        while( privatDataChildIter.hasNext() ) {
          var childNodeStr = privatDataChildIter.next().cloneNode(true).toString();
          strUserPrivateData += childNodeStr;  
        }
        userPrivateDataNode.removeChildren();
        userPrivateDataNode.setValue(strUserPrivateData);

      }

      if (UserNID != "DEFAULT_USER_NID" && !jsx3.util.strEmpty(userPrivateDataNode.getValue())) {
        var tempUserPrivateDataNode = new jsx3.xml.Document();
        tempUserPrivateDataNode.loadXML(userPrivateDataNode.getValue().toString());
        var defTag = tempUserPrivateDataNode.selectSingleNode(".//DefaultTag").getValue();
        topic_Set(tDlg, "DefTagSelected", defTag);
        cmpSelDefaultTag.BlkJSX.setValue(defTag);

        var chkConfig = tempUserPrivateDataNode.selectSingleNode(".//EndPointTableConfig/@Configurable").getValue();
        cmpChkEndPointTableConfigConfigurable.BlkJSX.setChecked(jsx3.gui.CheckBox.UNCHECKED);
        if (chkConfig == "true") {
          cmpChkEndPointTableConfigConfigurable.BlkJSX.setChecked(jsx3.gui.CheckBox.CHECKED);
        }

        var strXMLValueCacheId = cmpMtxEPDstPort.BlkJSX.getXMLId();
        var objXML = PNameSpace.getCache().getDocument(strXMLValueCacheId);

        var endPointEntryIterator = tempUserPrivateDataNode.selectNodeIterator(".//EndPointEntry");
        while (endPointEntryIterator.hasNext()) {
          var endPointEntryNode = endPointEntryIterator.next();
          var title = endPointEntryNode.getAttribute("Title");
          var objXMlNode = objXML.selectSingleNode(".//record[@jsxColumnName='" + title + "']");
          if (!jsx3.util.strEmpty(objXMlNode)) {
            objXMlNode.setAttribute("jsxChkColumn", "1");
          }
        }
      }

      var groupNode = userNode.selectSingleNode(".//Groups").cloneNode(true); 
      // set the Tags data in the matrix
      var strXMLCacheId = cmpBlkEditUserGroupMatrix.BlkJSX.getXMLId();
      var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
      objXML.loadXML(groupNode.toString());
      cmpBlkEditUserGroupMatrix.BlkJSX.setSourceXML(objXML);        
      
      // set the User data
      var strXMLCacheId = cmpCDFEditUser.BlkJSX.getXMLId();
      var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
      objXML.loadXML("<data jsxid='jsxroot'> <record>"+userNode.toString()+"</record> </data>");
      cmpCDFEditUser.BlkJSX.setSourceXML(objXML);         
      cmpCDFEditUser.BlkJSX.setCDFId(UserNID);       
      topic_Publish(tDlg, "EditUserDataState", UserNID);
      
      var externalAuth = userNode.getAttribute("externalAuthentication");
      if( externalAuth == "Yes" ) {
        cmpChkExternalAuthentication.BlkJSX.setChecked("1");
      }
      else {
        cmpChkExternalAuthentication.BlkJSX.setChecked("0");
      }
      handleEnableDisableExternalAuth();
      if( UserNID == "DEFAULT_USER_NID" ){
        var privateData = '<EndPointControllerData ><EndPointTableConfig Configurable="true"><EndPointEntry Title="" /></EndPointTableConfig><DefaultTag>All</DefaultTag></EndPointControllerData>';
        cmpTxtaUserPrivateData.BlkJSX.setValue(privateData);

        topic_Set(tDlg, "DefTagSelected", "All");
        cmpSelDefaultTag.BlkJSX.setValue(topic_Get(tDlg, "DefTagSelected") );
        cmpChkEndPointTableConfigConfigurable.BlkJSX.setChecked(jsx3.gui.CheckBox.CHECKED);
      }
      cmpDlgEditUser.BlkJSX.hideMask();
    } //  end cmpDlgEditUserGetUser
       
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

      // write the data back to the CDF
      cmpCDFEditUser.BlkJSX.write();

      //get the user data
      var strXMLCacheId = cmpCDFEditUser.BlkJSX.getXMLId();
      var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);      

      // genratethe request xml
      var xslfile = PNameSpace.loadInclude("jsxuser:///JSXAPPS/IPVS_AdminUI/xsl/EditUserRequest.xsl","UserRequest","xsl");
      var xslDoc = jsx3.xml.XslDocument.wrap(xslfile);
      var userXML = xslDoc.transform(objXML);
      var userNode = new jsx3.xml.Document();
      userNode.loadXML(userXML);
      
      //get the groups
      var strXMLCacheId = cmpBlkEditUserGroupMatrix.BlkJSX.getXMLId();
      var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
      //convert the matrix data to Tag nodes
      var xslfile = PNameSpace.loadInclude("jsxuser:///JSXAPPS/IPVS_AdminUI/xsl/CDFTagToTag.xsl","TagNode","xsl");
      var xslDoc = jsx3.xml.XslDocument.wrap(xslfile);
      var newGroupsXML = xslDoc.transform(objXML);        
      var newGroupsNode = new jsx3.xml.Document();
      newGroupsNode.loadXML(newGroupsXML);
      
      // replace the groups in the userNode
      var oldGroupsNode = userNode.selectSingleNode(".//Groups");              
      var groupParentNode = userNode.selectSingleNode(".//Groups").getParent();
      groupParentNode.replaceNode(newGroupsNode,oldGroupsNode);
      
      var newDate = new Date();
      // set NID in user node if replace mode
      if( SetMode == "Replace" ) {
        userNode.setAttribute("NID", UserNID);
        userNode.selectSingleNode(".//ModifiedDate").setValue(newDate);
      }
      else {
        userNode.selectSingleNode(".//CreationDate").setValue(newDate );
        userNode.selectSingleNode(".//ModifiedDate").setValue(newDate );
      }
      
      //delete empty group tag node
      deleteEmptyGroupTag(userNode);

      var userPrivatedataStr = "<EndPointControllerData>";

      var epConfig = cmpChkEndPointTableConfigConfigurable.BlkJSX.getChecked();
      if (epConfig) {
        userPrivatedataStr += "<EndPointTableConfig Configurable='true'>";
      }
      else {
        userPrivatedataStr += "<EndPointTableConfig Configurable='false'>";
      }

      var strXMLCacheId = cmpMtxEPDstPort.BlkJSX.getXMLId();
      var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
      var chkIterator = objXML.selectNodeIterator(".//record[@jsxChkColumn='1']");
      while (chkIterator.hasNext()) {
        var chkNode = chkIterator.next();
        var title = chkNode.getAttribute("jsxColumnName");
        userPrivatedataStr += "<EndPointEntry Title='" + title + "' />";
      }
      userPrivatedataStr += "</EndPointTableConfig>";

      var defTag = cmpSelDefaultTag.BlkJSX.getValue();
      userPrivatedataStr += "<DefaultTag>" + defTag + "</DefaultTag>";
      userPrivatedataStr += "<SaveLogin>true</SaveLogin></EndPointControllerData>";

      userNode.selectSingleNode(".//UserPrivateData").setValue(userPrivatedataStr);

      // do set request for User
      var reqSetUserObject = new ReqSetObject(SetMode, "User", userNode, 
                                                {action:"FNCCALL", context:this, 
                                                   fnc:function(respCode, respVal) {
                                                         if(respCode == "SUCCESS" && respVal != null) {
                                                           dlgEditUser_Destroy();
                                                           var loginUser = IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_USER);
                                                           var loginPassword = IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_PASSWORD);
                                                           var userName = userNode.selectSingleNode(".//Username").getValue();
                                                           var password = userNode.selectSingleNode(".//Password").getValue();
                                                           if(loginUser == userName && loginPassword != password){
                                                             IpvsLogin.doLogout();
                                                           }
                                                           else{
                                                             //  publish to refresh the user list
                                                             topic_Publish(tApp, "UserDataUpdate", "ADD");
                                                           }
                                                         } 
                                                         else if(respCode == "5206" ) {
                                                           alert("Duplicate Login name. Please enter some other Login name");
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
      reqSetUserObject.sendRequest();
    } 
        
    function cmpDlgEditUserValidate() {    
      var valFullName = cmpTxtUserFullName.BlkJSX.getValue();
      var valLoginName = cmpTxtUserLoginName.BlkJSX.getValue();
      var valPassword = cmpTxtEditUserPassword.BlkJSX.getValue();
      var valVerifyPassword = cmpTxtEditUserVerifyPassword.BlkJSX.getValue();
      //var valEmail = cmpTxtUserEmail.BlkJSX.getValue();

      var fullName = checkValue(valFullName , [BlkJSX,"txtUserFullName"] , "Please enter full name");
      if(fullName) {
        var loginName = checkValue(valLoginName , [BlkJSX,"txtUserLoginName"] , "Please enter login name");
      }
      if( cmpChkExternalAuthentication.BlkJSX.getChecked()!= jsx3.gui.CheckBox.CHECKED ) {
        if(loginName) {
          var pass = validatePassword(valPassword,[BlkJSX,"txtUserPassword"],null);
          if(pass) {   
            var validateVerifyPass = validateVerifyPassword(valPassword, valVerifyPassword, [BlkJSX,"txtUserPassword"], null);
          }
        }
        if (validateVerifyPass)
          var email = true;// checkValue(valEmail , [BlkJSX,"txtUserEmail" ], "Please enter email address");
        if(email)
          return true;
      }
      else{
        return true;
      }
        
      return false;
    } // end cmpDlgEditUserValidate

    function handleEnableDisableExternalAuth() {
      if( cmpChkExternalAuthentication.BlkJSX.getChecked()== jsx3.gui.CheckBox.CHECKED ) {
        cmpTxtEditUserPassword.BlkJSX.setEnabled(jsx3.gui.Form.STATEDISABLED, true);
        cmpTxtEditUserVerifyPassword.BlkJSX.setEnabled(jsx3.gui.Form.STATEDISABLED, true);
        cmpChkExternalAuthentication.BlkJSX.setChecked("1");
        cmpTxtEditUserPassword.BlkJSX.setValue("");
        cmpTxtEditUserVerifyPassword.BlkJSX.setValue("");
      }
      else {
        cmpChkExternalAuthentication.BlkJSX.setChecked("0");
        cmpTxtEditUserPassword.BlkJSX.setEnabled(jsx3.gui.Form.STATEENABLED, true);
        cmpTxtEditUserVerifyPassword.BlkJSX.setEnabled(jsx3.gui.Form.STATEENABLED, true);
      }
    }   
}

