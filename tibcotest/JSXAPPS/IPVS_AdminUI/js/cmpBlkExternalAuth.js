// Function to Create External Authentication Block .
function cmpBlkExternalAuth_Create() {
  
  var cmpCDFExternalAuth = new Component(["blkExAuth","cdfExternalAuthentication"]);
  cmpCDFExternalAuth.BlkJSX.setXMLTransformers("xsl/EditExternalAuthentication.xsl");

  var cmpChkExternalAuth = new CmpCheckBox(["blkExAuth","chkEnableExternalAuth"], null,
                                            {action:"FNCCALL", context:null, 
                                             fnc:function() {
                                               handleEnableDisableExternalAuth();
                                             }
                                           });

  var cmptxtServer = new CmpTxtBox(["blkExAuth","txtAuthServer"], null, null, null, null);
  var cmptxtAdminDN = new CmpTxtBox(["blkExAuth","txtAuthAdminDN"], null, null, null, null);
  var cmptxtPassword = new CmpTxtBox(["blkExAuth","txtAuthAdminPassword"], null, null, null, null);
  var cmptxtBaseDN = new CmpTxtBox(["blkExAuth","txtAuthBaseDN"], null, null, null, null);
  var cmptxtUserNameField = new CmpTxtBox(["blkExAuth","txtUserNameField"], null, null, null, null);

  var cmpBtnExternalAuthenticationSetDbConfig = new CmpButton(["blkExAuth","btnSaveExternalAuth"], null, 
                                                               {action:"FNCCALL", context:null, 
                                                                fnc:function() {
                                                                  cmpSetExternalAuthentication();
                                                                }
                                                               });
            
  var cmpBtnExternalAuthenticationCancel = new CmpButton(["blkExAuth","btnCancelExternalAuth"], null, 
                                                           {action:"FNCCALL", context:null, 
                                                            fnc:function() {
                                                                clearExternalAuthentication();
                                                            }
                                                           });

  var cmpExternalAuth = new CmpContainer(["blkExAuth", "blkExternalAuth"],
                                          [ {cmp:cmpChkExternalAuth},
                                            {cmp:cmptxtServer},
                                            {cmp:cmptxtAdminDN},
                                            {cmp:cmptxtPassword},
                                            {cmp:cmptxtBaseDN},
                                            {cmp:cmptxtUserNameField},
                                            {cmp:cmpBtnExternalAuthenticationSetDbConfig,
                                                 sel: "clsUserPermission.getPermission('System', 'modify') == 'true'"},
                                            {cmp:cmpBtnExternalAuthenticationCancel}
                                          ],
                                          [{action:"TOPICEVENT", topic:tApp, eventvar:"EditExternalAuthenticationSave", eventval:null}]);

  var cmpExternalAuthentication = new CmpContainer("blkExAuth",
                                        [ {cmp:cmpExternalAuth},
                                          {cmp:cmpCDFExternalAuth}
                                        ],
                                        null);
  
  eventApp.subscribe("SystemAdminMenuSelection", "EXTERNALAUTH", this,
                      function() {
                        cmpExternalAuthenticationSetDirtyBitChangeHandler();
                        cmpGetExternalAuthenticationConfig();
                      }
                    );

  // function to get External Authentication config
  function cmpGetExternalAuthenticationConfig(){
    var reqGetSystemConfig = new ReqSessionGetRequest("GetSystemConfig",
                                                      { action: "FNCCALL", context: this,
                                                        fnc: function(respCode, respVal) {
                                                                if (respCode == "SUCCESS" && respVal != null) {
                                                                  var externalAuthConfigNode = respVal;
                                                                  cmpGetExternalAuthentication(externalAuthConfigNode);
                                                                }
                                                                else{
                                                                    log.error("GetServiceConfig request failed:");
                                                                }
                                                             }
                                                      });
    reqGetSystemConfig.ServiceName = "SessionService";
    var reqParams = new Object;
    reqParams["ObjectList_WhereConfig"] = "ExternalAuthentication";                      
    reqGetSystemConfig.reqParams = reqParams;
    // send the request
    reqGetSystemConfig.sendRequest(this);
  }//end External Authentication config

  // function to set dirtybit flag
  function cmpExternalAuthenticationSetDirtyBitChangeHandler() { 
    var objParent = cmpExternalAuthentication.BlkJSX.getParent();
    
    // find all descendant of type chkbox and set its toggle event
    var ChkBoxNameArr = objParent.getDescendantsOfType(jsx3.gui.CheckBox, false);
    for(var i=0; i < ChkBoxNameArr.length; i++ ) {
      var ChkBlkJSX = ChkBoxNameArr[i];
      ChkBlkJSX.setEvent("cmpDlgEditAssetSetDirtyBitValue(this);", jsx3.gui.Interactive.TOGGLE);
    }

    // find all descendant of type txtbox and set its change event
    var TxtBoxNameArr = objParent.getDescendantsOfType(jsx3.gui.TextBox, false);
    for(var i=0; i < TxtBoxNameArr.length; i++ ) {
      var TxtBlkJSX = TxtBoxNameArr[i];
      var Name = TxtBoxNameArr[i].getName();
      var txtDirtyBitName = "txtDirtyBit" + TxtBoxNameArr[i].getParent().getName();
      if( Name != txtDirtyBitName) {
        TxtBlkJSX.setEvent("cmpDlgEditAssetSetDirtyBitValue(this);", jsx3.gui.Interactive.CHANGE);
      }
    }
  }  // end cmpExternalAuthenticationSetDirtyBitChangeHandler

  // function to Set the External Auth parameters in the Form
  function cmpGetExternalAuthentication(exAuthConfigNode) {
    var cdfID = exAuthConfigNode.selectSingleNode(".//Device").getAttribute("jid");
    var strXMLCacheId = cmpCDFExternalAuth.BlkJSX.getXMLId();
    var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
    objXML.loadXML(exAuthConfigNode.toString());
    cmpCDFExternalAuth.BlkJSX.setSourceXML(objXML);         
    cmpCDFExternalAuth.BlkJSX.setCDFId(cdfID);
    handleEnableDisableExternalAuth();
    // finish get data
    topic_Publish(tApp, "EditExternalAuthenticationSave", newServerDate().getTime());
  }//end cmpGetExternalAuthentication


  function clearExternalAuthentication(){
    if( cmpChkExternalAuth.BlkJSX.getChecked()== jsx3.gui.CheckBox.CHECKED ) {
      cmpChkExternalAuth.BlkJSX.setChecked("0");
    }
    cmptxtServer.BlkJSX.setValue("");
    cmptxtAdminDN.BlkJSX.setValue("");
    cmptxtPassword.BlkJSX.setValue("");
    cmptxtBaseDN.BlkJSX.setValue("");
    cmptxtUserNameField.BlkJSX.setValue("");
  }
  
  // function to Validate the External Authentication
  function cmpExternalAuthenticationValidate() {
    var baseDN = cmptxtBaseDN.BlkJSX.getValue();
    if( cmpChkExternalAuth.BlkJSX.getChecked()== jsx3.gui.CheckBox.CHECKED ) {
      var server = cmptxtServer.BlkJSX.getValue();
      if(server == "") {
        alert("Enter Server Name");
        return false;
      }
      var adminDN = cmptxtAdminDN.BlkJSX.getValue();
      if(adminDN == "") {
        alert("Enter Admin DN");
        return false;
      }
      var password = cmptxtPassword.BlkJSX.getValue();
      if(password == "") {
        alert("Enter Password");
        return false;
      }
      if(baseDN == "") {
        alert("Enter Base DN");
        return false;
      }
      var userNameField = cmptxtUserNameField.BlkJSX.getValue();
      if(userNameField == "") {
        alert("Enter User Name Field");
        return false;
      }
    }
    if(baseDN.length > 256){
      alert("You have entered more than 256 character for BaseDN");
      return false;
    }
    return true;
  }// end cmpExternalAuthenticationValidate

  // function to Send DeviceConfig Request to Save the External Authentication
  function cmpSetExternalAuthentication(){
    if( cmpExternalAuthenticationValidate() != true ) return;

    // write the data back to the CDF
    cmpCDFExternalAuth.BlkJSX.write();
    
    //get the IPVS DB Service data
    var strXMLCacheId = cmpCDFExternalAuth.BlkJSX.getXMLId();
    var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);      
    var SelectedAssetAgentJID = objXML.selectSingleNode(".//record").getAttribute("jsxnid");
    var xslfile = PNameSpace.loadInclude("JSXAPPS/IPVS_AdminUI/xsl/EditExternalAuthenticationRequest.xsl","EditExternalAuthenticationRequest","xsl");
    // genrate the request xml
    var xslDoc = jsx3.xml.XslDocument.wrap(xslfile);
    var ExternalAuthenticationXML = xslDoc.transform(objXML);
    ExternalAuthenticationNode = new jsx3.xml.Document();
    ExternalAuthenticationNode.loadXML(ExternalAuthenticationXML);

    if( ExternalAuthenticationNode == null ) return;
    
    // check if anything got chnaged
    var modifyNode = ExternalAuthenticationNode.selectSingleNode('.//*[@Dirty="MODIFY"]');
    if(modifyNode == null) {
      topic_Publish(tApp, "ExternalAuthentication", "DONE");
      return;    
    }

    var reqSetDeviceConfig = new ReqSessionGetRequest("SetDeviceConfig", 
                                                     { action:"FNCCALL", context:this,
                                                       fnc:function(respCode, respVal) {
                                                             if(respCode == "SUCCESS" && respVal != null) {
                                                               alert("Changes to your External Authentication were successful.");
                                                               topic_Publish(tApp, "ExternalAuthentication", "DONE");
                                                             } else {
                                                               var codeErrorMsg = CommonErrorCodeArr[respCode].errorDesc;
                                                               var ErrorMsg = "Error: Device->External Authentication data request failed \n";
                                                               ErrorMsg += "Error description: " + codeErrorMsg + "\n";
                                                               ErrorMsg += "Error code: " + respCode;
                                                               alert(ErrorMsg);
                                                             }
                                                           }
                                                     }
                                                   );
  
    reqSetDeviceConfig.ServiceName = "SessionService";
    var requestDataObjectArr = new Object;
    requestDataObjectArr["DeviceConfig"] = ExternalAuthenticationNode;
    reqSetDeviceConfig.requestDataObjectArr = requestDataObjectArr;
    var reqParams = new Object;
    reqParams["ObjectList_ByNID"] = SelectedAssetAgentJID;
    reqParams["ObjectList_ByTitle"] = "";
    reqParams["ObjectList_Path"] = "";
    reqSetDeviceConfig.reqParams = reqParams;
    reqSetDeviceConfig.sendRequest();
  }//end cmpSetExternalAuthentication

  function handleEnableDisableExternalAuth() {
    if( cmpChkExternalAuth.BlkJSX.getChecked()== jsx3.gui.CheckBox.CHECKED ) {
      cmpChkExternalAuth.BlkJSX.setChecked("1");
    }
    else{
      cmpChkExternalAuth.BlkJSX.setChecked("0");
    }
  }

  return cmpExternalAuthentication;
}