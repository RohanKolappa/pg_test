function dlgSystemSettings_Create() {
  //check that these dialog is call from initail setup or to change settings
  var DBState = topic_Get(tApp, "DatabaseState");
  var SettingDlgState = topic_Get(tApp, "SettingsDlgState");
  // load view - dialog component file
  if( (DBState == "VALID") && (SettingDlgState == "SAVERESTOREDB") ) {
    closeDialogIfExists("dlgAdminPassword");
    var DlgFile = "components/systemSetup.xml";
  }
  else {
    var DlgFile = "components/initialSettings.xml";
  }
  var BlkJSX = PNameSpace.getBodyBlock().load(DlgFile);
  
  // setup a local topic for dlg specific messaging
  var tDlg = "Dialog_" + jsx3.xml.CDF.getKey()
  topic_Create(tDlg);

  // setup a listner for it
  var eventDlg = new EventListner(tDlg);
  
  function dlgSystemSettings_Destroy() {
    // destroy any local eventlistners
    eventDlg.destroy();
    
    // destroy all local components
    cmpDlgSystemSetup.destroy();
    
    // destroy the local topic
    topic_Destroy(tDlg);
    
    unsubscribeEnterKeyAction();
    // close dialog
    BlkJSX.doClose();
  }

  var cmpBtnSystemSetupClose = new CmpButton(["dlgAdminPassword","btnDialogCloseX"],
                                               null, 
                                               {action:"FNCCALL", 
                                                context:null, 
                                                fnc:function() {
                                                  dlgSystemSettings_Destroy();
                                                }
                                               });
  if( (DBState == "VALID") && (SettingDlgState == "SAVERESTOREDB") ) {
    var cmpBtnSystemSetupCancel = new CmpButton(["dlgAdminPassword","btnCancel"],
                                               null, 
                                               {action:"FNCCALL", 
                                                context:null, 
                                                fnc:function() {
                                                  dlgSystemSettings_Destroy();
                                                }
                                               });

    var cmpRadioBackupToServer = new CmpRadioButton(["dlgAdminPassword", "radioBackupServer"], null,
                                                     { action: "FNCCALL",
                                                       context: null,
                                                       fnc: function() {
                                                         topic_Set(tDlg, "BackupTo", "Server");
                                                         var txtFileName = getJsxByName([BlkJSX,"txtExportFileName"]);
                                                         txtFileName.setEnabled(jsx3.gui.Form.STATEDISABLED, true);
                                                       }
                                                     });
    var cmpRadioBackupToPC = new CmpRadioButton(["dlgAdminPassword", "radioBackupPC"], null,
                                                 { action: "FNCCALL",
                                                   context: null,
                                                   fnc: function() {
                                                     topic_Set(tDlg, "BackupTo", "PC");
                                                     var txtFileName = getJsxByName([BlkJSX,"txtExportFileName"]);
                                                     txtFileName.setEnabled(jsx3.gui.Form.STATEENABLED, true);
                                                   }
                                               });
                                                 
    var cmpBtnSystemSetupBackup = new CmpButton(["dlgAdminPassword","btnBackupDB"],
                                                 [{action:"TOPICEVENT", topic:tDlg, eventvar:"SaveRestoreBtnState", eventval:null}],
                                                 {action:"FNCCALL", 
                                                  context:null, 
                                                  fnc:function() {
                                                    this.DBopType = "SaveDB";
                                                    var SaveSBTo = topic_Get(tDlg, "BackupTo");
                                                    if( SaveSBTo == "PC" ) {
                                                      var txtFileName = getJsxByName([BlkJSX,"txtExportFileName"]);
                                                      var localFileName = txtFileName.getValue();
                                                      if( localFileName == "" ) {
                                                        alert("Please enter filename to export DB");
                                                        return;
                                                      }
                                                      topic_Publish(tDlg, "SaveRestoreBtnState", "NONE");
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
                                                      topic_Set(tDlg, "ExportLocalFileName", localFilePath);
                                                      cmpDlgHandleExportImportDB("Export");
                                                    }
                                                    else {
                                                      topic_Publish(tDlg, "SaveRestoreBtnState", "NONE");
                                                      cmpDlgSystemSetup_SaveRestoreDB.call(this);
                                                    }
                                                  }
                                                 });

    var cmpBtnSystemSetupBrowseForRestore = new CmpButton(["dlgAdminPassword","btnBrowse"],
                                                           null, 
                                                           {action:"FNCCALL", 
                                                            context:null, 
                                                            fnc:function() {
                                                                doBrowseForFile("txtImportFilePath");
                                                            }
                                                           });
                                                 
    var cmpBtnSystemSetupRestore = new CmpButton(["dlgAdminPassword","btnRestoreDB"],
                                                 [{action:"TOPICEVENT", topic:tDlg, eventvar:"SaveRestoreBtnState", eventval:null}],
                                                 {action:"FNCCALL", 
                                                  context:null, 
                                                  fnc:function() {
                                                    this.DBopType = "RestoreDB";
                                                    var restoreFrom = getRestoreFrom();
                                                    if( restoreFrom == "LastImported" ) {
                                                      var txtFilePath = getJsxByName([BlkJSX,"txtImportFilePath"]);
                                                      var localFilePath = txtFilePath.getValue();
                                                      if( localFilePath == "" ) {
                                                        alert("Select backup file to restore");
                                                        return;
                                                      }
                                                      topic_Publish(tDlg, "SaveRestoreBtnState", "NONE");
                                                      topic_Set(tDlg, "ImportLocalFileName", localFilePath);
                                                      cmpDlgHandleExportImportDB("Import");
                                                    }
                                                    else {
                                                      topic_Publish(tDlg, "SaveRestoreBtnState", "NONE");
                                                      cmpDlgSystemSetup_SaveRestoreDB.call(this);
                                                    }
                                                  }
                                                 });
                                                                                           
    var cmpDlgSystemSetup = new CmpContainer("dlgAdminPassword",
                                [ {cmp:cmpRadioBackupToServer},
                                  {cmp:cmpRadioBackupToPC},
                                  {cmp:cmpBtnSystemSetupBackup},
                                  {cmp:cmpBtnSystemSetupBrowseForRestore},
                                  {cmp:cmpBtnSystemSetupRestore},
                                  {cmp:cmpBtnSystemSetupCancel},
                                  {cmp:cmpBtnSystemSetupClose}
                                ],
                                null);
  }
  else {
    var cmpBtnSystemSetupSetPwd = new CmpButton(["dlgAdminPassword","btnSetPwd"],
                                           null, 
                                           {action:"FNCCALL", 
                                            context:null, 
                                            fnc:function() {
                                              eventDlg.subscribe("SystemSettings", "DONE", this,
                                                                function() {
                                                                  dlgSystemSettings_Destroy();
                                                                });
                                              cmpDlgCreateAdminSetUp.call();
                                            }
                                           });
    var cmpDlgSystemSetup = new CmpContainer("dlgAdminPassword",
                                [ {cmp:cmpBtnSystemSetupSetPwd},
                                  {cmp:cmpBtnSystemSetupClose}
                                ],
                                null);
  }
                                                            
  cmpDlgSystemSetup.BlkJSX = BlkJSX;
  cmpDlgSystemSetup.getBlkData =  cmpDlgSystemSetup_SetInit;
      
  // init and repaint happens after the dialog is loaded
  jsx3.sleep( (function(){
                 this.init();
                 this.getBlkData();
                 // Trigger Save Action for EnterKey
                 subscribeEnterKeyAction();
                 }
               ),
               null, cmpDlgSystemSetup );

  function cmpDlgSystemSetup_SetInit() {
   if( (DBState == "VALID") && (SettingDlgState == "SAVERESTOREDB") ) {
     cmpRadioBackupToServer.BlkJSX.setSelected(jsx3.gui.RadioButton.SELECTED);
     topic_Set(tDlg, "BackupTo", "Server");
     var txtFileName = getJsxByName([BlkJSX,"txtExportFileName"]);
     txtFileName.setEnabled(jsx3.gui.Form.STATEDISABLED, true);
   }
   else{
    PNameSpace.getJSXByName("txtAdminPwd").focus();
   }
  }
  
  function subscribeEnterKeyAction() {
    jsx3.gui.Event.subscribe(jsx3.gui.Event.KEYPRESS, eval(handleEnterKeyAction));
  }

  function unsubscribeEnterKeyAction() {
    jsx3.gui.Event.unsubscribe(jsx3.gui.Event.KEYPRESS, eval(handleEnterKeyAction));
  }

  function handleEnterKeyAction(objEvent) {
    if (objEvent.event.enterKey()) {
      cmpDlgCreateAdminSetUp();
    }
  }

  function cmpDlgCreateAdminSetUp() {
    var adminPwd = PNameSpace.getJSXByName("txtAdminPwd").getValue();
    var verifyPwd = PNameSpace.getJSXByName("txtAdminVerifyPwd").getValue();
    var validPwd = false;
    var pass = validatePassword(adminPwd,"txtAdminPwd",null);
    if(pass) {   
      var validateVerifyPass = validateVerifyPassword(adminPwd, verifyPwd, "txtAdminPwd", null);
      if( validateVerifyPass ) {
        validPwd = true;
      }
    }
    if(!validPwd ) return;

    var UserNID = "DEFAULT_USER_NID";
    //var SetMsg = "Add admin user";
    var SetMode = "Add";

    var DBState = topic_Get(tApp, "DatabaseState");
    if( DBState == "VALID" ) {
     // SetMsg = "Edit admin user";
      SetMode = "Replace";
    }

    // get User node info from UI
    var userStr = '<User Ver="" Rev=""><Groups><Tag Name="" Value=""/></Groups><Username>username</Username><Password>password</Password><Email>email</Email><Name>name</Name><CreationDate>'+ new Date()+'</CreationDate><ModifiedDate>'+new Date()+'</ModifiedDate><Roster><Item jid="" askstatus="" recvstatus="" substatus="" name=""/></Roster><ExternalAuthentication>No</ExternalAuthentication><UserPrivateData>UserPrivateData</UserPrivateData></User>';
    var userNode = new jsx3.xml.Document().loadXML(userStr); 

    if( SetMode == "Replace" ) {
      var adminUserNode = topic_GetNode(tCache, "//UserList/*[Username='superuser-default']");
      if( adminUserNode != null ) {
        UserNID = adminUserNode.getAttribute("NID");
        userNode.setAttribute("NID", UserNID);
      }
    }

    userNode.selectSingleNode(".//Name").setValue("superuser-default");
    userNode.selectSingleNode(".//Username").setValue("superuser-default");
    userNode.selectSingleNode(".//Password").setValue(adminPwd);
    userNode.selectSingleNode(".//Email").setValue("superuser-default");
    userNode.selectSingleNode(".//UserPrivateData").setValue("");
    
    //delete empty group tag node
    deleteEmptyGroupTag(userNode);

    // do set request for User
    var reqSetUserObject = new ReqSetObject(SetMode, "User", userNode, 
                                              {action:"FNCCALL", context:this, 
                                                 fnc:function(respCode, respVal) {
                                                       if(respCode == "SUCCESS" && respVal != null) {
                                                         alert("Changes to your password were successful.");
                                                         dlgSystemSettings_Destroy();
                                                         IpvsLogin.doLogout();
                                                       } else {
                                                         var codeErrorMsg = CommonErrorCodeArr[respCode].errorDesc;
                                                         var ErrorMsg = "Error: Change password request failed \n";
                                                         ErrorMsg += "Error description: " + codeErrorMsg + "\n";
                                                         ErrorMsg += "Error code: " + respCode;
                                                         alert(ErrorMsg);
                                                       }
                                                     }
                                              } 
                                           );
    reqSetUserObject.sendRequest();
  }

  function cmpDlgSystemSetup_SaveRestoreDB() {
    var ArchiverName = null;
    var objectType = this.DBopType;
    var reqSaveRestoreDB = new ReqSaveRestoreDB(ArchiverName, objectType, 
                                                {action:"FNCCALL", context:this, 
                                                   fnc:function(respCode, respVal) {
                                                         topic_Publish(tDlg, "SaveRestoreBtnState", "ENABLE");
                                                         if(respCode == "SUCCESS" && respVal != null) {
                                                           if( objectType == "RestoreDB" ) {
                                                             alert("The System Database restore was successful");
                                                             dlgSystemSettings_Destroy();
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
    reqSaveRestoreDB.sendRequest(); 
  }

  function cmpDlgHandleExportImportDB(ObjType) {
    var localFileName = topic_Get(tDlg, ObjType + "LocalFileName");
    var ObjectType = ObjType + "ServiceDB";
    var reqFileTransfer = new ReqFileTransfer(ObjectType, 
                                              {action:"FNCCALL", context:this, 
                                               fnc:function(respCode, respVal) {
                                                     if(respCode == "SUCCESS" && respVal != null) {
                                                       var ObjFileXferNode = respVal.selectSingleNode(".//FileXfer");
                                                       initFileXfer(ObjFileXferNode, ObjType);
                                                     }
                                                     else {
                                                       alert("Error in " + ObjType + " DB");
                                                     }
                                                   }
                                                } 
                                             );
    var reqParams = new Object;
    reqParams["LocalFileName"] = localFileName;  
    reqFileTransfer.reqParams = reqParams;
    reqFileTransfer.sendRequest(); 
  }

  function initFileXfer(responseNode, XferMode) {
    var tXferDlg = "Xfer_" + jsx3.xml.CDF.getKey()
    topic_Create(tXferDlg);
    var XferWinTitle = "Export Database";
    if( XferMode == "Import" ) {
      XferWinTitle = "Import Database";
    }
    topic_Set(tXferDlg, "XferWindowTitle", XferWinTitle);

    // setup a listner for it
    var eventXferDlg = new EventListner(tXferDlg);
    eventXferDlg.subscribe("FileXferDone", null, this,
                            function(EventVar, EventVal) {
                              var XferMode = EventVal;
                              //check any xfers state is not completed
                              var XferErrorNode = topic_GetNode(tXferDlg, ".//*[@XferState!='Completed']");
                              if( XferErrorNode != null ) {
                                topic_Publish(tDlg, "SaveRestoreBtnState", "ENABLE");
                                alert( XferMode + " process failed.Please verify that the firewall setup in your network is not blocking port 7777.");
                              }
                              else {
                                if( XferMode == "Export" ) {
                                  topic_Publish(tDlg, "SaveRestoreBtnState", "ENABLE");
                                  alert("The System Database backup was successful.");
                                }
                                else if( XferMode == "Import" ) {
                                  this.DBopType = "RestoreDB";
                                  cmpDlgSystemSetup_SaveRestoreDB(this);
                                }
                              }
                              //destroy eventXfer topic
                              eventXferDlg.destroy();
                              topic_Destroy(tXferDlg);
                              tXferDlg = null;
                              eventXferDlg = null;
                            });
    var FileXferListNode = topic_Set(tXferDlg, "FileXferList", null);
    FileXferListNode.appendChild(responseNode);
    dlgFileXfers_Create(tXferDlg, XferMode);
  }
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