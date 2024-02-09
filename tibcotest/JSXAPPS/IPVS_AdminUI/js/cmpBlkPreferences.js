function cmpBlkPreferences_Create() {

  var cmpTxtMaxClientLogins = new CmpTxtBox(["blkPreferences","txtMaxClientLogins"], null, null, null, null);

  var cmpBtnSetPreferences = new CmpButton(["blkPreferences","btnSetPreferences"], null,
                                           {action:"FNCCALL", context:this, 
                                            fnc:function() {
                                              cmpSetSystemPreferences();
                                            }
                                           });
                                           
  var cmpBlkPreferences = new CmpContainer("blkPreferences",
                                           [{cmp:cmpTxtMaxClientLogins},
                                            {cmp:cmpBtnSetPreferences}
                                           ],
                                           null);

  eventApp.subscribe("SystemAdminMenuSelection", "PREFERENCES", this,
                      function() {
                        cmpGetSystemConfiguartion();
                       } 
                    );

 // function to get System Configuartion for Preferences                 
 function cmpGetSystemConfiguartion(){
   var reqGetSystemConfig = new ReqSessionGetRequest("GetSystemConfig",
                                                      { action: "FNCCALL", context: null,
                                                        fnc: function(respCode, respVal) {
                                                          if (respCode == "SUCCESS" && respVal != null) { 
                                                            var configNode = respVal;
                                                            cmpGetSystemPreferences(configNode);
                                                          }
                                                          else {
                                                            log.error("GetSystemConfig request failed");
                                                          }
                                                        }
                                                      }
                                                     );
    reqGetSystemConfig.ServiceName = "SessionService";
    var reqParams = new Object;
    reqParams["ObjectList_WhereConfig"] = "Preferences";                      
    reqGetSystemConfig.reqParams = reqParams;
    // send the request
    reqGetSystemConfig.sendRequest(this);
  }// end cmpGetSystemConfiguartion
  
  // function to populate the MaxClient information in the Prefrences Block
  function cmpGetSystemPreferences(configNode){
    cmpBlkPreferences.SelectedAssetAgentJID = configNode.selectSingleNode(".//Device").getAttribute("NID");
    cmpBlkPreferences.MaxMediaClients = configNode.selectSingleNode(".//Device/Preferences/Login").getAttribute("maxMediaClients");
    if(!jsx3.util.strEmpty(cmpBlkPreferences.MaxMediaClients)){
      cmpTxtMaxClientLogins.BlkJSX.setValue(cmpBlkPreferences.MaxMediaClients);
    }
    else{
      //The default value should be 999
      var defaultMaxMediaClients = "999";
      cmpTxtMaxClientLogins.BlkJSX.setValue(defaultMaxMediaClients);
    }
  }// end cmpGetSystemPreferences
  
  //function to Set Preferences and send Request
  function cmpSetSystemPreferences(){
   if( cmpMaxClientLoginValidate() != true ) return; 
   var maxMediaClient = cmpTxtMaxClientLogins.BlkJSX.getValue();    
   var MaxClientLoginString = '<DeviceConfig><ServiceElementsConfig><XMPPServiceConfig><ServicePreferences><Login><MaxMediaClients></MaxMediaClients></Login></ServicePreferences></XMPPServiceConfig></ServiceElementsConfig></DeviceConfig>';
   var MaxClientLoginNode = new jsx3.xml.Document().loadXML(MaxClientLoginString);
   if(maxMediaClient == cmpBlkPreferences.MaxMediaClients){
   // no Change Occured to the Max client Value
    return;
   }
   var servicePreferencesNode = MaxClientLoginNode.selectSingleNode(".//ServiceElementsConfig/XMPPServiceConfig/ServicePreferences");
   servicePreferencesNode.setAttribute("Dirty","MODIFY");
   var maxMediaClientsNode = servicePreferencesNode.selectSingleNode(".//Login/MaxMediaClients");
   maxMediaClientsNode.setValue(maxMediaClient);
   var reqSetDeviceConfig = new ReqSessionGetRequest("SetDeviceConfig",
                                                     { action:"FNCCALL", context:this,
                                                       fnc:function(respCode, respVal) {
                                                             if(respCode == "SUCCESS" && respVal != null) {
                                                              alert("Limit on maximum number of concurrent Media Client logins was set successfully");
                                                             } else {
                                                               var codeErrorMsg = CommonErrorCodeArr[respCode].errorDesc;
                                                               var ErrorMsg = "Error: Device->MaxMediaClients data request failed \n";
                                                               ErrorMsg += "Error description: " + codeErrorMsg + "\n";
                                                               ErrorMsg += "Error code: " + respCode;
                                                               alert(ErrorMsg);
                                                             }
                                                           }
                                                     }
                                                   );
  
    reqSetDeviceConfig.ServiceName = "SessionService";
    var requestDataObjectArr = new Object;
    requestDataObjectArr["DeviceConfig"] = MaxClientLoginNode;
    reqSetDeviceConfig.requestDataObjectArr = requestDataObjectArr;
    var reqParams = new Object;
    reqParams["ObjectList_ByNID"] = cmpBlkPreferences.SelectedAssetAgentJID;
    reqParams["ObjectList_ByTitle"] = "";
    reqParams["ObjectList_Path"] = "";
    reqSetDeviceConfig.reqParams = reqParams;
    reqSetDeviceConfig.sendRequest();
  }// end cmpSetSystemPreferences
 
  // Function to Validate the Max Clients
  function cmpMaxClientLoginValidate(){
    var maxClientLogin = cmpTxtMaxClientLogins.BlkJSX.getValue();
    if(jsx3.util.strEmpty(maxClientLogin)){
      alert("Please enter the maximum number of concurrent Media Client logins you want to allow");
      cmpTxtMaxClientLogins.BlkJSX.setValue(cmpBlkPreferences.MaxMediaClients);
      return false;
    }
    if(isNaN(maxClientLogin) || maxClientLogin <= 0 || maxClientLogin > 999){
      alert("Please enter the maximum number of concurrent Media Client logins between 1 and 999");
      cmpTxtMaxClientLogins.BlkJSX.setValue(cmpBlkPreferences.MaxMediaClients);
      return false;
    }
    return true;
  }// end cmpMaxClientLoginValidate

  return cmpBlkPreferences;
}// end cmpBlkPrefrences_Create