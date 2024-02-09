// Function to Create Backup Block .Seperated the Code in the DlgSystemSettings.js.Added code from DlgScheduleBackup.js
function cmpBlkBackup_Create() {
    topic_Set(tApp, 'BackupStatus','NONE');
    topic_Set(tApp, 'ScheduleBackupStatus','NONE');
    topic_Set(tApp, 'RestoreDBStatus','NONE');
    topic_Set(tApp, "SaveRestoreBtnState", "ENABLE");
    
    var cmpRadioBackupToServer = new CmpRadioButton(["blkBackup","radioBackupServer"], null,
                                                     { action: "FNCCALL",
                                                       context: null,
                                                       fnc: function() {
                                                         topic_Set(tApp, "BackupTo", "Server");
                                                         var txtFileName = getJsxByName(["blkBackup","txtExportFileName"]);
                                                         txtFileName.setEnabled(jsx3.gui.Form.STATEDISABLED, true);
                                                       }
                                                     });
                                                     
    var cmpRadioBackupToPC = new CmpRadioButton(["blkBackup","radioBackupPC"], null,
                                                 { action: "FNCCALL",
                                                   context: null,
                                                   fnc: function() {
                                                     topic_Set(tApp, "BackupTo", "PC");
                                                     var txtFileName = getJsxByName(["blkBackup","txtExportFileName"]);
                                                     txtFileName.setEnabled(jsx3.gui.Form.STATEENABLED, true);
                                                   }
                                               });
                                                 
    var cmpBtnSystemSetupBackup = new CmpButton(["blkBackup","btnBackupDB"],
                                                 [{action:"APPEVENT", eventvar:"SaveRestoreBtnState", eventval:null}],
                                                 {action:"FNCCALL", 
                                                  context:null, 
                                                  fnc:function() {
                                                    this.DBopType = "SaveDB";
                                                    var SaveSBTo = topic_Get(tApp, "BackupTo");
                                                    if( SaveSBTo == "PC" ) {
                                                      var txtFileName = getJsxByName(["blkBackup","txtExportFileName"]);
                                                      var localFileName = jsx3.util.strTrim(txtFileName.getValue());
                                                      if( jsx3.util.strEmpty(localFileName)) {
                                                        alert("Please enter filename");
                                                        txtFileName.focus();
                                                        return;
                                                      }
                                                      else{
                                                        var regex = /^[-abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_\s]*$/;
                                                        if (!regex.test(localFileName)){
                                                          alert("File name should not contain special characters");
                                                          txtFileName.setValue("");
                                                          txtFileName.focus();
                                                          return;
                                                        }
                                                      }
                                                      topic_Publish(tApp, 'BackupStatus','PROGRESS');
                                                      topic_Publish(tApp, "SaveRestoreBtnState", "NONE");
                                                      var appStartPath = giStartPath.replace(/%20/g, " ");
                                                      //var appStartPath = "C:\\IP Video Systems\\IPVS Collaboration Client\\GI\\JSXAPPS\\IPVS_AdminUI"; //TBR
                                                      var fso = new ActiveXObject("Scripting.FileSystemObject");
                                                      var folderObj = fso.GetFolder(appStartPath);
                                                      var driveName = folderObj.Drive;
                                                      var exportFolder = driveName + "\\IPVSBackup";
                                                      var localFilePath = exportFolder + "\\" + localFileName;
                                                      if( !fso.FolderExists(exportFolder) ) {
                                                        var newFolderName = fso.CreateFolder(exportFolder); 
                                                      }
                                                      if(fso.FileExists(localFilePath)){
                                                       var agree = confirm('Filename already exists. Do you want to overwrite?');
                                                       if( agree ) {
                                                         topic_Set(tApp, "ExportLocalFileName", localFilePath);
                                                         //cmpHandleExportImportDB("Export");
                                                         cmpSaveRestoreDB("BackupDatabase");
                                                       }
                                                       else{
                                                         var backupStatus = topic_Get(tApp, "BackupStatus");
                                                         if(backupStatus == "PROGRESS"){
                                                          topic_Publish(tApp, "BackupStatus", "NONE");
                                                         }
                                                         topic_Publish(tApp, "SaveRestoreBtnState", "ENABLE");
                                                         return;
                                                       }
                                                      }
                                                      else{
                                                        topic_Set(tApp, "ExportLocalFileName", localFilePath);
                                                        //cmpHandleExportImportDB("Export");
                                                        cmpSaveRestoreDB("BackupDatabase");
                                                      }
                                                    }
                                                    else {
                                                      topic_Publish(tApp, 'BackupStatus','PROGRESS');
                                                      topic_Publish(tApp, "SaveRestoreBtnState", "NONE");
                                                      cmpSaveRestoreDB(this.DBopType);
                                                    }
                                                  }
                                                 });

  var cmpBlkStatus = new Component(["blkBackup","blkStatus"]);
  
  var cmpBlkManualBackup = new CmpContainer(["blkBackup","blkManualBackup"],
                                        [ {cmp:cmpRadioBackupToServer},
                                          {cmp:cmpRadioBackupToPC},
                                          {cmp:cmpBtnSystemSetupBackup},
                                          {cmp:cmpBlkStatus,
                                              sel:"topic_Get('" + tApp + "', 'BackupStatus') == 'PROGRESS'"}
                                        ],
                                        [{action:"APPEVENT", topic: tApp, eventvar:"BackupStatus", eventval:null}]);
                                        

  var cmpCDFScheduleBackup = new Component(["blkBackup","cdfDlgScheduleBackup"]);
  cmpCDFScheduleBackup.BlkJSX.setXMLTransformers("xsl/EditScheduleBackup.xsl");

  var cmpChkEnableBackup = new CmpCheckBox(["blkBackup","chkEnableBkp"], null,
                                            {action:"FNCCALL", context:null, 
                                             fnc:function() {
                                               handleEnableDisableBackup();
                                             }
                                           });

  var cmptxtDbBackupTime = new CmpTxtBox(["blkBackup","txtDbBackupTime"], null, null, null, null);
  var cmpChkEnableFtp = new Component(["blkBackup", "chkEnableFtp"]);
  var cmptxtExportFtpURL = new CmpTxtBox(["blkBackup","txtExportFtpURL"], null, null, null, null);
  var cmpChkAutoRecovey = new Component(["blkBackup", "chkAutoRecoveryFrom"]);
  
  var cmpBtnScheduleBackupSetDbConfig = new CmpButton(["blkBackup","btnSetBackupSchedule"], null, 
                                                       {action:"FNCCALL", context:null, 
                                                        fnc:function() {
                                                          cmpSetScheduleBackup();
                                                        }
                                                       });

  var cmpBlkSchedule = new CmpContainer(["blkBackup","blkSchedule"],
                                        [ {cmp:cmpChkEnableBackup},
                                          {cmp:cmptxtDbBackupTime},
                                          {cmp:cmpChkEnableFtp},
                                          {cmp:cmptxtExportFtpURL},
                                          {cmp:cmpChkAutoRecovey},
                                          {cmp:cmpBtnScheduleBackupSetDbConfig}
                                        ],
                                        [{action:"TOPICEVENT", topic:tApp, eventvar:"EditScheduleBackupSave", eventval:null}]);

  var cmpBlkStatus = new Component(["blkScheduleBackup","blkStatus"]);
                                                                                          
  var cmpBlkScheduleBackup = new CmpContainer(["blkBackup","blkScheduleBackup"],
                              [ {cmp:cmpBlkSchedule},
                                {cmp:cmpCDFScheduleBackup},
                                {cmp:cmpBlkStatus,
                                    sel:"topic_Get('" + tApp + "', 'ScheduleBackupStatus') == 'PROGRESS'"}
                              ],
                              [{action:"APPEVENT", topic: tApp, eventvar:"ScheduleBackupStatus", eventval:null}]);
  

  var cmpBlkBackup = new CmpContainer(["blkBackup"],
                                      [ {cmp:cmpBlkManualBackup},
                                        {cmp:cmpBlkScheduleBackup}
                                      ],
                                      null);
                                        
  eventApp.subscribe("SystemAdminMenuSelection", "BACKUP", this,
                      function() {
                       var DBState = topic_Get(tApp, "DatabaseState");
//                       var SettingDlgState = topic_Get(tApp, "SettingsDlgState");
                       if( DBState == "VALID" ) {
                         cmpRadioBackupToServer.BlkJSX.setSelected(jsx3.gui.RadioButton.SELECTED);
                         topic_Set(tApp, "BackupTo", "Server");
                         var txtFileName = getJsxByName(["blkBackup","txtExportFileName"]);
                         txtFileName.setEnabled(jsx3.gui.Form.STATEDISABLED, true);
                         cmpScheduleBackupSetDirtyBitChangeHandler();
                         cmpGetBackupConfig();
                       }
                     } 
                    );
                    
  //************************************* Functions Used by Schedule Backup part ********************************************                  
                    
 // function to get Schedule BackUp config                  
 function cmpGetBackupConfig(){
   var reqGetSystemConfig = new ReqSessionGetRequest("GetSystemConfig",
                                                      { action: "FNCCALL", context: null,
                                                        fnc: function(respCode, respVal) {
                                                          if (respCode == "SUCCESS" && respVal != null) { 
                                                            var configNode = respVal;
                                                            cmpGetScheduleBackup(configNode);
                                                          }
                                                          else {
                                                            log.error("GetSystemConfig request failed");
                                                          }
                                                        }
                                                      }
                                                     );
    reqGetSystemConfig.ServiceName = "SessionService";
    var reqParams = new Object;
    reqParams["ObjectList_WhereConfig"] = "ScheduleBackup";                      
    reqGetSystemConfig.reqParams = reqParams;
    // send the request
    reqGetSystemConfig.sendRequest(this);
  }// end cmpGetBackupConfig
  
    // function to set dirtybit flag
  function cmpScheduleBackupSetDirtyBitChangeHandler() { 
    var objParent = cmpBlkSchedule.BlkJSX.getParent();
    
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
  }  // end cmpScheduleBackupSetDirtyBitChangeHandler

  // function to Set the Backup parameters in the Form
  function cmpGetScheduleBackup(configNode) {
      // set the Device Config data
      var cdfID = configNode.selectSingleNode(".//Device").getAttribute("jid");
      var strXMLCacheId = cmpCDFScheduleBackup.BlkJSX.getXMLId();
      var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
      objXML.loadXML(configNode.toString());
      cmpCDFScheduleBackup.BlkJSX.setSourceXML(objXML);   
      cmpCDFScheduleBackup.BlkJSX.setCDFId(cdfID);
      handleEnableDisableBackup();
      var strXMLCacheId = cmpCDFScheduleBackup.BlkJSX.getXMLId();
      var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
      // finish get data
      topic_Publish(tApp, "EditScheduleBackupSave", (newServerDate()).getTime());
  }// end cmpGetScheduleBackup
  
  // function to Validate the Export
  function cmpScheduleBackupValidate() {
    if(cmpChkEnableBackup.BlkJSX.getChecked()== jsx3.gui.CheckBox.CHECKED) {
      var backupAt = cmptxtDbBackupTime.BlkJSX.getValue();
      if(backupAt == "") {
        alert("Enter Backup Time");
        return false;
      }
      else{
        var backupValues = backupAt.split(","); 
        for(var i = 0;i < backupValues.length;i++){
          var backupValue = backupValues[i];
          if( isNaN(backupValue) || backupValue < 0 || backupValue >= 24){
            alert("Backup Time values should be from 0 to 23");
            PNameSpace.getJSXByName("txtDbBackupTime").focus();
            return false;
          }
        }
      }
      if(cmpChkEnableFtp.BlkJSX.getChecked()== jsx3.gui.CheckBox.CHECKED) {
        var ftpUrl = cmptxtExportFtpURL.BlkJSX.getValue();
        if(ftpUrl == "") {
          alert("Enter Valid FTP URL");
          return false;
        }
      }
    }
    return true;
  }// end cmpScheduleBackupValidate

  // function to Send DeviceConfig Request to Save the Backup Schedule
  function cmpSetScheduleBackup() {
    if( cmpScheduleBackupValidate() != true ) return;     
    var ScheduleBackupNode = null;
    topic_Publish(tApp, 'ScheduleBackupStatus','PROGRESS');
    // write the data back to the CDF
    cmpCDFScheduleBackup.BlkJSX.write();
    //get the IPVS DB Service data
    var strXMLCacheId = cmpCDFScheduleBackup.BlkJSX.getXMLId();
    var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);    
    var SelectedAssetAgentJID = objXML.selectSingleNode(".//record").getAttribute("jsxnid");
    var xslfile = PNameSpace.loadInclude("JSXAPPS/IPVS_AdminUI/xsl/EditScheduleBackupRequest.xsl","EditScheduleBackupRequest","xsl");
    // genrate the request xml
    var xslDoc = jsx3.xml.XslDocument.wrap(xslfile);
    var ScheduleBackupXML = xslDoc.transform(objXML);
    ScheduleBackupNode = new jsx3.xml.Document();
    ScheduleBackupNode.loadXML(ScheduleBackupXML);

    if( ScheduleBackupNode == null ) return;
    
    // check if anything got chnaged
    var modifyNode = ScheduleBackupNode.selectSingleNode('.//*[@Dirty="MODIFY"]');
    if(modifyNode == null) {
      topic_Publish(tApp, 'ScheduleBackupStatus','NONE');
      topic_Publish(tApp, "ScheduleBackup", "DONE");
      return;    
    }
    var reqSetDeviceConfig = new ReqSessionGetRequest("SetDeviceConfig", 
                                                     { action:"FNCCALL", context:this,
                                                       fnc:function(respCode, respVal) {
                                                             if(respCode == "SUCCESS" && respVal != null) {
                                                               alert("Changes to your Schedule Backups were successful.");
                                                               topic_Publish(tApp, "ScheduleBackup", "DONE");
                                                             } else {
                                                               var codeErrorMsg = CommonErrorCodeArr[respCode].errorDesc;
                                                               var ErrorMsg = "Error: Device->Schedule Backup data request failed \n";
                                                               ErrorMsg += "Error description: " + codeErrorMsg + "\n";
                                                               ErrorMsg += "Error code: " + respCode;
                                                               alert(ErrorMsg);
                                                             }
                                                             topic_Publish(tApp, 'ScheduleBackupStatus','NONE');
                                                           }
                                                     }
                                                   );
  
    reqSetDeviceConfig.ServiceName = "SessionService";
    var requestDataObjectArr = new Object;
    requestDataObjectArr["DeviceConfig"] = ScheduleBackupNode;
    reqSetDeviceConfig.requestDataObjectArr = requestDataObjectArr;
    var reqParams = new Object;
    reqParams["ObjectList_ByNID"] = SelectedAssetAgentJID;
    reqParams["ObjectList_ByTitle"] = "";
    reqParams["ObjectList_Path"] = "";
    reqSetDeviceConfig.reqParams = reqParams;
    reqSetDeviceConfig.sendRequest(); 
  }// end cmpSetScheduleBackup

  function handleEnableDisableBackup() {
    if( cmpChkEnableBackup.BlkJSX.getChecked()== jsx3.gui.CheckBox.CHECKED ) {
      cmptxtDbBackupTime.BlkJSX.setEnabled(jsx3.gui.Form.STATEENABLED, true);
      cmpChkEnableFtp.BlkJSX.setEnabled(jsx3.gui.Form.STATEENABLED, true);
      cmptxtExportFtpURL.BlkJSX.setEnabled(jsx3.gui.Form.STATEENABLED, true);
    }
    else {
      cmptxtDbBackupTime.BlkJSX.setEnabled(jsx3.gui.Form.STATEDISABLED, true);
      cmpChkEnableFtp.BlkJSX.setEnabled(jsx3.gui.Form.STATEDISABLED, true);
      cmptxtExportFtpURL.BlkJSX.setEnabled(jsx3.gui.Form.STATEDISABLED, true);
    }
  }
  
  return cmpBlkBackup;
}// end cmpBlkBackup_Create

// Function to Create Restore Block .Seperated the Code in the DlgSystemSettings.js
function cmpBlkRestore_Create() { 
    var cmpBtnSystemSetupBrowseForRestore = new CmpButton(["blkRestore","btnBrowse"],
                                                           null, 
                                                           {action:"FNCCALL", 
                                                            context:null, 
                                                            fnc:function() {
                                                                doBrowseForFile("txtImportFilePath");
                                                            }
                                                           });
                                                 
    var cmpBtnSystemSetupRestore = new CmpButton(["blkRestore","btnRestoreDB"],
                                                 [{action:"APPEVENT", eventvar:"SaveRestoreBtnState", eventval:null}],
                                                 {action:"FNCCALL", 
                                                  context:null, 
                                                  fnc:function() {
                                                    this.DBopType = "RestoreDB";
                                                    var restoreFrom = getRestoreFrom();
                                                    if( restoreFrom == "LastImported" ) {
                                                      var txtFilePath = getJsxByName(["blkRestore","txtImportFilePath"]);
                                                      var localFilePath = txtFilePath.getValue();
                                                      if( localFilePath == "" ) {
                                                        alert("Select backup file to restore");
                                                        return;
                                                      }
                                                      topic_Publish(tApp, 'RestoreDBStatus','PROGRESS');
                                                      topic_Publish(tApp, "SaveRestoreBtnState", "NONE");
                                                      topic_Set(tApp, "ImportLocalFileName", localFilePath);
                                                      cmpSaveRestoreDB("ImportServiceDB");
                                                    }
                                                    else {
                                                      topic_Publish(tApp, 'RestoreDBStatus','PROGRESS');
                                                      topic_Publish(tApp, "SaveRestoreBtnState", "NONE");
                                                      cmpSaveRestoreDB(this.DBopType);
                                                    }
                                                  }
                                                 });

  var cmpBlkStatus = new Component(["blkRestore","blkStatus"]);

  var cmpBlkRestore = new CmpContainer("blkRestore",
                                        [ {cmp:cmpBtnSystemSetupBrowseForRestore},
                                          {cmp:cmpBtnSystemSetupRestore},
                                          {cmp:cmpBlkStatus,
                                              sel:"topic_Get('" + tApp + "', 'RestoreDBStatus') == 'PROGRESS'"}
                                        ],
                                        [{action:"APPEVENT", topic: tApp, eventvar:"RestoreDBStatus", eventval:null}]);

  return cmpBlkRestore;
}// end cmpBlkRestore_Create

//******************** Functions Commanly used by Backup and Restore Blocks ***********************

function cmpSaveRestoreDB(DBopType) {
  var ArchiverName = null;
  var objectType = DBopType;
  var reqSaveRestoreDB = new ReqSaveRestoreDB(ArchiverName, objectType, 
                                              {action:"FNCCALL", context:null, 
                                                 fnc:function(respCode, respVal) {
                                                       topic_Publish(tApp, "SaveRestoreBtnState", "ENABLE");
                                                       var backupStatus = topic_Get(tApp, "BackupStatus");
                                                       if(backupStatus == "PROGRESS"){
                                                        topic_Publish(tApp, "BackupStatus", "NONE");
                                                       }
                                                       var restoreDBStatus = topic_Get(tApp, "RestoreDBStatus");
                                                       if(restoreDBStatus == "PROGRESS"){
                                                        topic_Publish(tApp, "RestoreDBStatus", "NONE");
                                                       }
                                                       if(respCode == "SUCCESS" && respVal != null) {
                                                         if (objectType == "RestoreDB" || objectType == "ImportServiceDB") {
                                                           alert("The System Database restore was successful");
                                                         }
                                                         else {
                                                           alert("The System Database backup was successful.");
                                                         }
                                                       } else {
                                                         var objError = CommonErrorCodeArr[respCode];
                                                         if( !jsx3.util.strEmpty(objError) ) {
                                                           var codeErrorMsg = objError.errorDesc;
                                                         }
                                                         else {
                                                           var codeErrorMsg = "Unkown Error";
                                                         }
                                                         var ErrorMsg = "Error: " + objectType +" request failed \n";
                                                         ErrorMsg += "Error description: " + codeErrorMsg + "\n";
                                                         ErrorMsg += "Error code: " + respCode;
                                                         alert(ErrorMsg);
                                                       }
                                                     }
                                                } 
                                             );
  if( objectType == "RestoreDB" ) {
    var restoreFrom = getRestoreFrom();
    var reqParams = new Object;
    reqParams["RestoreFrom"] = restoreFrom;  
    reqSaveRestoreDB.reqParams = reqParams;
  }
  else if (objectType == "ImportServiceDB" || objectType == "BackupDatabase") {
    var ObjType = (objectType == "ImportServiceDB") ? "Import" : "Export";
    var reqParams = new Object;
    reqParams["LocalFileName"] = topic_Get(tApp, ObjType + "LocalFileName");
    reqSaveRestoreDB.reqParams = reqParams;
  }
  reqSaveRestoreDB.sendRequest(); 
}
  
  
function getRestoreFrom() {
  if( PNameSpace.getJSXByName("radioRestoreFactory").getSelected() ) {
    return "Factory";
  }
  else if( PNameSpace.getJSXByName("radioRestoreSystem").getSelected() ) {
    return "LastBackup";
  }
  else {
    return "LastImported";
  }
}

function getRestoreChkBoxValue(){
  if( PNameSpace.getJSXByName("chkAutoRecoveryFrom").getChecked() ){
    return "LastBackup";
  } else {
    return "Factory";
  }
}



