function cmpBlkMulticastIPAddress_Create() {

  var cmpTxtMulticastStartIP = new CmpTxtBox(["blkMulticastIPAddress","txtMulticastStartIP"], null, null, null, null);

  var cmpTxtMulticastEndIP = new CmpTxtBox(["blkMulticastIPAddress","txtMulticastEndIP"], null, null, null, null);

  var cmpTxtMulticastMonitor = new CmpTxtBox(["blkMulticastIPAddress","txtMulticastMonitor"], null, null, null, null);

  var cmpChkMulticast = new CmpCheckBox(["blkMulticastIPAddress","chkEnableMulticast"], null,
                                        {action:"FNCCALL", context:null, 
                                         fnc:function() {
                                           handleEnableDisableMulticast();
                                         }
                                       });

  var cmpBtnRefresh = new CmpButton(["blkMulticastIPAddress","btnRefresh"], null,
                                    {action:"FNCCALL", context:this, 
                                      fnc:function() {
                                        cmpGetSystemConfiguartion();
                                      }
                                    });
                                           
  var cmpBtnSetMulticastIPAddress = new CmpButton(["blkMulticastIPAddress","btnSetMulticastIPAddress"], null,
                                                  {action:"FNCCALL", context:this, 
                                                    fnc:function() {
                                                      cmpSetSystemMulticastIPAddress();
                                                    }
                                                  });

  var cmpBlkMulticastIPAddress = new CmpContainer("blkMulticastIPAddress",
                                                 [{cmp:cmpTxtMulticastStartIP},
                                                  {cmp:cmpTxtMulticastEndIP},
                                                  {cmp:cmpTxtMulticastMonitor},
                                                  {cmp:cmpChkMulticast},
                                                  {cmp:cmpBtnRefresh},
                                                  {cmp:cmpBtnSetMulticastIPAddress}
                                                 ],
                                                 null);

  eventApp.subscribe("SystemAdminMenuSelection", "MULTICASTIPADDRESS", this,
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
                                                            cmpGetSystemMulticastIPAddress(configNode);
                                                          }
                                                          else {
                                                            log.error("GetSystemConfig request failed");
                                                          }
                                                        }
                                                      }
                                                     );
    reqGetSystemConfig.ServiceName = "SessionService";
    var reqParams = new Object;
    reqParams["ObjectList_WhereConfig"] = "ALL";                      
    reqGetSystemConfig.reqParams = reqParams;
    // send the request
    reqGetSystemConfig.sendRequest(this);
  }// end cmpGetSystemConfiguartion
  
  // function to populate the Multicast IPAddress information in the Multicast IPAddress Block
  function cmpGetSystemMulticastIPAddress(configNode){
    cmpBlkMulticastIPAddress.SelectedAssetAgentJID = configNode.selectSingleNode(".//Device").getAttribute("NID");
    var multicastIPAddressNode = configNode.selectSingleNode(".//Device/MulticastIPAddressPool");
    if(!jsx3.util.strEmpty(multicastIPAddressNode)){
      cmpBlkMulticastIPAddress.EnableMulticast = multicastIPAddressNode.getAttribute("enable");
      cmpBlkMulticastIPAddress.StartIPAddress = multicastIPAddressNode.getAttribute("start");
      cmpBlkMulticastIPAddress.EndIPAddress = multicastIPAddressNode.getAttribute("end");
      cmpTxtMulticastStartIP.BlkJSX.setValue(cmpBlkMulticastIPAddress.StartIPAddress);
      cmpTxtMulticastEndIP.BlkJSX.setValue(cmpBlkMulticastIPAddress.EndIPAddress);
      if(cmpBlkMulticastIPAddress.EnableMulticast == "true"){
        cmpChkMulticast.BlkJSX.setChecked("1");
        cmpTxtMulticastStartIP.BlkJSX.setEnabled(jsx3.gui.Form.STATEENABLED, true);
        cmpTxtMulticastEndIP.BlkJSX.setEnabled(jsx3.gui.Form.STATEENABLED, true);
      }
      else{
        cmpChkMulticast.BlkJSX.setChecked("0");
        cmpTxtMulticastStartIP.BlkJSX.setEnabled(jsx3.gui.Form.STATEDISABLED, true);
        cmpTxtMulticastEndIP.BlkJSX.setEnabled(jsx3.gui.Form.STATEDISABLED, true);
      }
      var usedPoolNode = multicastIPAddressNode.selectSingleNode(".//UsedIPAddressList");
      var usedIter = usedPoolNode.selectNodeIterator(".//UsedIPAddress");
      var usedIPAddresses = "";
      while (usedIter.hasNext()) {
        var usedNode = usedIter.next();
        var ipAddress = usedNode.getAttribute("ipAddress");
        if(!jsx3.util.strEmpty(ipAddress)){
          usedIPAddresses = usedIPAddresses + ipAddress + ",";
        }
      }
      usedIPAddresses = usedIPAddresses.slice(0, -1);
      cmpTxtMulticastMonitor.BlkJSX.setValue(usedIPAddresses);
    }
  }// end cmpGetSystemMulticastIPAddress
  
  //function to Set MulticastIPAddress and send Request
  function cmpSetSystemMulticastIPAddress(){
   if( cmpMulticastIPAddressValidate() != true ) return; 
   var reqSetDeviceConfig = new ReqSessionGetRequest("SetMulticastIPAddressPool",
                                                     { action:"FNCCALL", context:this,
                                                       fnc:function(respCode, respVal) {
                                                             if(respCode == "SUCCESS" && respVal != null) {
                                                               var multicastMode = cmpChkMulticast.BlkJSX.getChecked() == jsx3.gui.CheckBox.CHECKED ? "Enabled" : "Disabled";
                                                               if(multicastMode == "Enabled"){
                                                                 alert("Multicast IP Address range was set successfully");
                                                               }
                                                               else{
                                                                 alert("Multicast IP Address range was disabled successfully");
                                                               }
                                                             } else {
                                                               var codeErrorMsg = CommonErrorCodeArr[respCode].errorDesc;
                                                               var ErrorMsg = "Error: Device->Multicast IPAddress data request failed \n";
                                                               ErrorMsg += "Error description: " + codeErrorMsg + "\n";
                                                               ErrorMsg += "Error code: " + respCode;
                                                               alert(ErrorMsg);
                                                             }
                                                           }
                                                     }
                                                   );
  
    reqSetDeviceConfig.ServiceName = "SessionService";
    var reqParams = new Object;
    reqParams["ObjectList_Enable"] = cmpChkMulticast.BlkJSX.getChecked() == jsx3.gui.CheckBox.CHECKED ? true : false;
    reqParams["ObjectList_StartIPAddress"] = cmpTxtMulticastStartIP.BlkJSX.getValue();
    reqParams["ObjectList_EndIPAddress"] = cmpTxtMulticastEndIP.BlkJSX.getValue();
    reqSetDeviceConfig.reqParams = reqParams;
    reqSetDeviceConfig.sendRequest();
  }// end cmpSetSystemMulticastIPAddress
 
  // Function to Validate the MulticastIPAddress
  function cmpMulticastIPAddressValidate(){
    var startIPAddress = cmpTxtMulticastStartIP.BlkJSX.getValue();
    if(jsx3.util.strEmpty(startIPAddress)){
      alert("Please enter the Starting IP");
      cmpTxtMulticastStartIP.BlkJSX.setValue(cmpBlkMulticastIPAddress.StartIPAddress);
      return false;
    }
    else{
      var startIPAddressValidation = verifyMulticastIPAddress(startIPAddress);
      if(!startIPAddressValidation){
        cmpTxtMulticastStartIP.BlkJSX.focus();
        return false;
      }
    }
    var endIPAddress = cmpTxtMulticastEndIP.BlkJSX.getValue();
    if(jsx3.util.strEmpty(endIPAddress)){
      alert("Please enter the Ending IP");
      cmpTxtMulticastEndIP.BlkJSX.setValue(cmpBlkMulticastIPAddress.EndIPAddress);
      return false;
    }
    else{
      var endIPAddressValidation = verifyMulticastIPAddress(endIPAddress);
      if(!endIPAddressValidation){
        cmpTxtMulticastEndIP.BlkJSX.focus();
        return false;
      }
    }
    if(cmpChkMulticast.BlkJSX.getChecked() == jsx3.gui.CheckBox.CHECKED){
      var startIPValue = ipAddress2num(startIPAddress);
      var endIPValue = ipAddress2num(endIPAddress);
      if(endIPValue <= startIPValue){
        alert("Ending IP should be greater than Starting IP");
        return false;
      }
    }
    return true;
  }// end cmpMulticastIPAddressValidate
  
  function handleEnableDisableMulticast() {
    if( cmpChkMulticast.BlkJSX.getChecked()== jsx3.gui.CheckBox.CHECKED ) {
      cmpChkMulticast.BlkJSX.setChecked("1");
      cmpTxtMulticastStartIP.BlkJSX.setEnabled(jsx3.gui.Form.STATEENABLED, true);
      cmpTxtMulticastEndIP.BlkJSX.setEnabled(jsx3.gui.Form.STATEENABLED, true);
    }
    else{
      cmpChkMulticast.BlkJSX.setChecked("0");
      cmpTxtMulticastStartIP.BlkJSX.setEnabled(jsx3.gui.Form.STATEDISABLED, true);
      cmpTxtMulticastEndIP.BlkJSX.setEnabled(jsx3.gui.Form.STATEDISABLED, true);
    }
  }

  return cmpBlkMulticastIPAddress;
}// end cmpBlkMulticastIPAddress_Create