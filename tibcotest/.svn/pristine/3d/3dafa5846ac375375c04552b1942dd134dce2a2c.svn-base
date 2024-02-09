// Function to Create MS Upgrade Block .Moved the Code from the cmpDlgUpgradeManager.js
function cmpBlkUpgrade_Create() {

  topic_Set(tApp, 'MSUpgradeSelection','AVAILABLE');
  
  // ---- MS upgrade section ----
  var cmpLblBuildVersion = new CmpLabel(["blkUpgrades","lblBuildVersion"],null,null,null,null);
  
  var cmpTxtBuildImageFileName = new Component(["blkUpgrades","txtBuildImageFileName"],null,null,null);
  cmpTxtBuildImageFileName.BlkJSX.setReadonly(jsx3.Boolean.TRUE);

  var cmpBtnBroswerImageUpload = new CmpButton(["blkUpgrades","btnBrowse"],
                                                null, 
                                                { action:"FNCCALL", 
                                                  context:null, 
                                                  fnc:function() {
                                                    doBrowseForFile("txtBuildImageFileName");
                                                  }
                                                }
                                              );

  var cmpBtnInstallMSUpdate = new CmpButton(["blkUpgrades","btnInstallMSUpdate"],
                                            null, 
                                            { action:"FNCCALL", 
                                              context:null, 
                                              fnc:function() {
                                                  var imageFilePath = cmpTxtBuildImageFileName.BlkJSX.getValue();
                                                  if(!jsx3.util.strEmpty(imageFilePath)){
                                                    var fileExtension = imageFilePath.substring(imageFilePath.lastIndexOf(".")+1,imageFilePath.length);
                                                    if(!jsx3.util.strEmpty(fileExtension)&& fileExtension =="tar"){
                                                      sendUpgradeMSRequest(imageFilePath,'');
                                                    }
                                                    else{
                                                      alert("File Format not supported.Please Select a .tar Image file to upgrade");
                                                    }
                                                  }
                                                  else{
                                                    alert("Please Select an Image to upgrade");
                                                  }
                                              }
                                            }
                                           );
                                           
  var cmpBlkStatus = new Component(["blkUpgrades","blkStatus"]);

  // ---- upload device specific software ----      
  var cmpLblDeviceSoftwareVersion = new CmpLabel(["blkUpgrades", "lblDeviceSoftwareVersion"], null, null, null, null);                                  
  var cmpTxtUploadImageFileName = new Component(["blkUpgrades", "txtUploadImageFileName"], null, null, null);
  cmpTxtUploadImageFileName.BlkJSX.setReadonly(jsx3.Boolean.TRUE);

  var cmpBtnDeviceBrowserUpload = new CmpButton(["blkUpgrades", "btnUploadBrowse"],
                        null,
                        { action: "FNCCALL",
                            context: null,
                            fnc: function () {
                                doBrowseForFile("txtUploadImageFileName");
                            }
                        }
                       );

  var cmpBtnUpload = new CmpButton(["blkUpgrades", "btnUpload"],
                        null,
                        { action: "FNCCALL",
                            context: null,
                            fnc: function () {
                                var imageFilePath = cmpTxtUploadImageFileName.BlkJSX.getValue();
                                if (!jsx3.util.strEmpty(imageFilePath)) {
                                    var fileExtension = imageFilePath.substring(imageFilePath.lastIndexOf(".") + 1, imageFilePath.length);
                                    if (!jsx3.util.strEmpty(fileExtension) && fileExtension == "tar") {
                                        sendUpgradeMSRequest(imageFilePath, 'V2D-XPi-Device');//NGS-D200
                                    }
                                    else {
                                        alert("File Format not supported.Please Select a .tar Image file to upgrade");
                                    }
                                }
                                else {
                                    alert("Please Select an Image to upgrade");
                                }
                            }
                        }
                        );

    var cmpBlkUploadStatus = new Component(["blkUpgrades", "blkUploadStatus"]);

    var cmpRadioMSUpgrade = new CmpRadioButton(["blkUpgrades", "radioMsUpgrade"], null,
                                { action: "FNCCALL",
                                    context: null,
                                    fnc: function () {
                                        toogleBtnField('Upgrade');
                                    }
                                });

    var cmpRadioUploadDevice = new CmpRadioButton(["blkUpgrades", "radioDevUpload"], null,
                            { action: "FNCCALL",
                                context: null,
                                fnc: function () {
                                    toogleBtnField('Upload');
                                    topic_Publish(tApp, 'DevUploadSelection', 'AVAILABLE');
                                }
                            });

    var cmpUploadComplete = new CmpRadioButton(["blkUpgrades", "blkUploadCompleted"]);
    var cmpBlkInstallMSUpdate = new CmpContainer(["blkUpgrades", "blkInstallMSUpdate"],
                        [
                            { cmp: cmpBtnInstallMSUpdate,
                                sel: "topic_Get(tApp, 'MSUpgradeSelection') == 'AVAILABLE'"
                            },
                            { cmp: cmpBlkStatus,
                                sel: "topic_Get(tApp, 'MSUpgradeSelection') == 'PROGRESS'"
                            },
                            { cmp: cmpBtnUpload,
                                sel: "topic_Get(tApp, 'DevUploadSelection') == 'AVAILABLE'"
                            },
                            { cmp: cmpBlkUploadStatus,
                                sel: "topic_Get(tApp, 'DevUploadSelection') == 'PROGRESS'"
                            },
                            { cmp: cmpBtnDeviceBrowserUpload,
                                sel: "topic_Get(tApp, 'DevUploadSelection') == 'AVAILABLE'"
                            },
                            { cmp: cmpUploadComplete,
                                sel: "topic_Get(tApp, 'DevUploadCompleteStatus') == 'COMPLETE'"
                            }
                        ],
                        [{ action: "APPEVENT", eventvar: "MSUpgradeSelection", eventval: null },
                         { action: "APPEVENT", eventvar: "DevUploadSelection", eventval: null },
                         { action: "APPEVENT", eventvar: "DevUploadCompleteStatus", eventval: null}]);

  var cmpBlkUpgrades = new CmpContainer("blkUpgrades",
                                        [
                                          //upgrade
                                          {cmp: cmpLblBuildVersion},
                                          {cmp: cmpBtnBroswerImageUpload},
                                          {cmp: cmpTxtBuildImageFileName },
                                          //upload
                                          {cmp: cmpLblDeviceSoftwareVersion },
                                          {cmp: cmpTxtUploadImageFileName },
                                          //general
                                          {cmp: cmpRadioMSUpgrade },
                                          {cmp: cmpRadioUploadDevice },
                                          {cmp: cmpBlkInstallMSUpdate }
                                        ],
                                        null);

  //set MsDeviceSoftwareVersion to initial
  eventApp.subscribe("SystemAdminMenuSelection", "UPGRADE", this,
                      function () {
                        //set build version
                        var buildVersion = topic_Get(tApp, "MSSoftwareVersion");
                        cmpLblBuildVersion.BlkJSX.setText(buildVersion, true);
                        //set device software version
                        var deviceSoftwareVersion = topic_Get(tApp, "MsDeviceSoftwareVersion");
                        if (!jsx3.util.strEmpty(deviceSoftwareVersion)) {
                          cmpLblDeviceSoftwareVersion.BlkJSX.setText(deviceSoftwareVersion, true);
                        }
                      }
                    );

  //change install button, browser button, input field status for both upgrade and upload section
  function toogleBtnField(upgradeOrUpload) {
      var formVar1 = (upgradeOrUpload == 'Upgrade') ? jsx3.gui.Form.STATEENABLED : jsx3.gui.Form.STATEDISABLED;
      var formVar2 = (upgradeOrUpload == 'Upgrade') ? jsx3.gui.Form.STATEDISABLED : jsx3.gui.Form.STATEENABLED;
      //upgrade  
      cmpBtnBroswerImageUpload.BlkJSX.setEnabled(formVar1, true);
      cmpBtnInstallMSUpdate.BlkJSX.setEnabled(formVar1, true);
      cmpTxtBuildImageFileName.BlkJSX.setEnabled(formVar1, true);
      //upload
      cmpBtnDeviceBrowserUpload.BlkJSX.setEnabled(formVar2, true);
      cmpBtnUpload.BlkJSX.setEnabled(formVar2, true);
      cmpTxtUploadImageFileName.BlkJSX.setEnabled(formVar2, true);    
  } 



  //Send Request to upgrade the MS with selected Image from Local dir
  function sendUpgradeMSRequest(imageFilePath, deviceType) {
    if (deviceType == 'V2D-XPi-Device') {
        topic_Publish(tApp, 'DevUploadSelection', 'PROGRESS');
        topic_Publish(tApp, 'DevUploadCompleteStatus', 'RESET');
    }
    else {
        topic_Publish(tApp, 'MSUpgradeSelection', 'PROGRESS');  
    }
  
    var ObjectType = "Upgrade";
    var reqUpgradeMS = new ReqFileTransfer(ObjectType,
                                            { action: "FNCCALL", context: this,
                                              fnc: function (respCode, respVal) {
                                                if (respCode == "SUCCESS" && respVal != null) {
                                                  //if response success
                                                  if (deviceType == 'V2D-XPi-Device') {
                                                    var versionNode = respVal.selectSingleNode("//DeviceSoftwareVersion");
                                                    var uploadDeviceSoftwareVersion = versionNode.getValue();
                                                    topic_Publish(tApp, 'DevUploadCompleteStatus', 'COMPLETE');
                                                    topic_Set(tApp, "MsDeviceSoftwareVersion", uploadDeviceSoftwareVersion);
                                                    //set device software version
                                                    if (!jsx3.util.strEmpty(uploadDeviceSoftwareVersion)) {
                                                      cmpLblDeviceSoftwareVersion.BlkJSX.setText(uploadDeviceSoftwareVersion, true);
                                                    }
                                                    topic_Publish(tApp, 'AssetDataUpdate', 'EDIT');
                                                  }
                                                }
                                                else {
                                                  log.error("Upgrade MS Request failed.Error :" + respVal);
                                                  var codeErrorMsg = "Unkown Error";
                                                  var objError = '';
                                                  var description = '';
                                                  if (respVal.match(/ResponseXML/)) {
                                                    var ResponseXML = (respVal.replace(/\n/g, ' ')).replace(/.*ResponseXML:(.*)/, "$1");
                                                    var responseDoc = new jsx3.xml.Document;
                                                    responseDoc.loadXML(ResponseXML);
                                                    try {
                                                      description = responseDoc.selectSingleNode(".//Description").getValue();
                                                    } catch (e) {
                                                      log.error("Upgrade MS Request failed.Error : no error description" + e);
                                                    }
                                                  }
                                                  if (description != "Unkown Error") {
                                                    codeErrorMsg = description;
                                                  }
                                                  else {
                                                    objError = CommonErrorCodeArr[respCode];
                                                    if (!jsx3.util.strEmpty(objError)) {
                                                      codeErrorMsg = objError.errorDesc;
                                                    }
                                                  }
                                                  var ErrorMsg = "Error:Upgrade request failed \n";
                                                  ErrorMsg += "Error description: " + codeErrorMsg + "\n";
                                                  ErrorMsg += "Error code: " + respCode;
                                                  alert(ErrorMsg);
                                                }
                                                if (deviceType == 'V2D-XPi-Device') {
                                                  topic_Publish(tApp, 'DevUploadSelection', 'AVAILABLE');
                                                }
                                                else {
                                                  topic_Publish(tApp, 'MSUpgradeSelection', 'AVAILABLE');
                                                }
                                              }
                                            }
                                           );
    var reqParams = new Object;
    reqParams["DeviceType"] = deviceType; 
    reqParams["LocalFileName"] = imageFilePath;  
    reqUpgradeMS.reqParams = reqParams;
    reqUpgradeMS.sendRequest(); 
  }

  return cmpBlkUpgrades;
}