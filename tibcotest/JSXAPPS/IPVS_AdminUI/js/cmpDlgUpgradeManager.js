function dlgUpgradeManager_Create() {
//  closeDialogIfExists("dlgUpgradeManager");
//  // load view - dialog component file
//  var DlgFile = "components/upgradeManagerDialog.xml";
//  var BlkJSX = PNameSpace.getBodyBlock().load(DlgFile);
//  
//  // setup a local topic for dlg specific messaging
//  var tDlg = "Dialog_" + jsx3.xml.CDF.getKey()
//  topic_Create(tDlg);

//  // setup a listner for it
//  var eventDlg = new EventListner(tDlg);
//  
//  function dlgUpgradeManager_Destroy() {
//    // destroy any local eventlistners
//    eventDlg.destroy();
//    
//    // destroy all local components
//    cmpDlgUpgradeManager.destroy();
//    
//    // destroy the local topic
//    topic_Destroy(tDlg);
//    
//    // close dialog
//    BlkJSX.doClose();
//  }
//  
//  topic_Set(tDlg, 'MSUpgradeSelection','AVAILABLE');
//  
//  var cmpBtnUpgradeManagerClose = new CmpButton(["dlgUpgradeManager","btnDialogCloseX"],
//                                                null, 
//                                                { action:"FNCCALL", 
//                                                  context:null, 
//                                                  fnc:function() {
//                                                    dlgUpgradeManager_Destroy();
//                                                  }
//                                                }
//                                               );
//                                               
//  var buildVersion = topic_Get(tApp, "MSSoftwareVersion");
//  
//  var cmpLblBuildVersion = new CmpLabel(["dlgUpgradeManager","lblBuildVersion"],null,null,null,buildVersion);
//  
//  var cmpTxtBuildImageFileName = new Component(["dlgUpgradeManager","txtBuildImageFileName"],null,null,null);

//  var cmpBtnBroswerImageUpload = new CmpButton(["dlgUpgradeManager","btnBrowse"],
//                                                null, 
//                                                { action:"FNCCALL", 
//                                                  context:null, 
//                                                  fnc:function() {
//                                                    doBrowseForFile("txtBuildImageFileName");
//                                                  }
//                                                }
//                                              );

//  var cmpBtnInstallMSUpdate = new CmpButton(["dlgUpgradeManager","btnInstallMSUpdate"],
//                                            null, 
//                                            { action:"FNCCALL", 
//                                              context:null, 
//                                              fnc:function() {
//                                                  var imageFilePath = cmpTxtBuildImageFileName.BlkJSX.getValue();
//                                                  if(!jsx3.util.strEmpty(imageFilePath)){
//                                                    var fileExtension = imageFilePath.substring(imageFilePath.lastIndexOf(".")+1,imageFilePath.length);
//                                                    if(!jsx3.util.strEmpty(fileExtension)&& fileExtension =="tar"){
//                                                      sendUpgradeMSRequest(imageFilePath);
//                                                    }
//                                                    else{
//                                                      alert("File Format not supported.Please Select a .tar Image file to upgrade");
//                                                    }
//                                                  }
//                                                  else{
//                                                    alert("Please Select an Image to upgrade");
//                                                  }
//                                              }
//                                            }
//                                           );
//                                           
//  var cmpBlkStatus = new Component(["dlgUpgradeManager","blkStatus"]);
//  
//  var cmpBlkInstallMSUpdate = new CmpContainer(["dlgUpgradeManager","blkInstallMSUpdate"],
//                                               [{cmp:cmpBtnInstallMSUpdate,
//                                                 sel:"topic_Get('" + tDlg + "', 'MSUpgradeSelection') == 'AVAILABLE'"},
//                                                {cmp:cmpBlkStatus,
//                                                 sel:"topic_Get('" + tDlg + "', 'MSUpgradeSelection') == 'PROGRESS'"}
//                                               ],
//                                               [{action:"TOPICEVENT", topic: tDlg, eventvar:"MSUpgradeSelection", eventval:null}] );
//  
//  var cmpBtnUpgradeManagerCancel = new CmpButton(["dlgUpgradeManager","btnCancel"],
//                                                  null, 
//                                                  { action:"FNCCALL", 
//                                                    context:null, 
//                                                    fnc:function() {
//                                                      dlgUpgradeManager_Destroy();
//                                                    }
//                                                  }
//                                                 );
//  
//  var cmpDlgUpgradeManager = new CmpContainer("dlgUpgradeManager",
//                                              [ {cmp:cmpLblBuildVersion},
//                                                {cmp:cmpBtnBroswerImageUpload},
//                                                {cmp:cmpBlkInstallMSUpdate},
//                                                {cmp:cmpTxtBuildImageFileName},
//                                                {cmp:cmpBtnUpgradeManagerCancel}
//                                              ],
//                                              null);
//                                              
//  cmpDlgUpgradeManager.BlkJSX = BlkJSX;
//  cmpDlgUpgradeManager.getBlkData =  cmpDlgUpgradeManager_SetInit;
//  // init and repaint happens after the dialog is loaded
//  jsx3.sleep( (function(){
//                 this.init();
//                 this.getBlkData();
//                 }
//               ),
//               null, cmpDlgUpgradeManager );

//  function cmpDlgUpgradeManager_SetInit() {
//  //Disable input field to prevent user from over writing the file path.
//   cmpTxtBuildImageFileName.BlkJSX.setReadonly(jsx3.Boolean.TRUE);
//  }
//  
//  //Send Request to upgrade the MS with selected Image from Local dir
//  function sendUpgradeMSRequest(imageFilePath){
//    topic_Publish(tDlg, 'MSUpgradeSelection','PROGRESS');
//    var ObjectType = "Upgrade";
//    var reqUpgradeMS = new ReqFileTransfer( ObjectType, 
//                                            {action:"FNCCALL", context:this, 
//                                             fnc:function(respCode, respVal) {
//                                                   if(respCode == "SUCCESS" && respVal != null) {
//                                                     topic_Publish(tDlg, 'MSUpgradeSelection','AVAILABLE');
//                                                     dlgUpgradeManager_Destroy();
//                                                   }
//                                                   else {
//                                                     log.error("Upgrade MS Request failed.Error :" + respVal);
//                                                     var objError = CommonErrorCodeArr[respCode];
//                                                     if( !jsx3.util.strEmpty(objError) ) {
//                                                       var codeErrorMsg = objError.errorDesc;
//                                                     }
//                                                     else {
//                                                       var codeErrorMsg = "Unkown Error";
//                                                     }
//                                                     var ErrorMsg = "Error:Upgrade request failed \n";
//                                                     ErrorMsg += "Error description: " + codeErrorMsg + "\n";
//                                                     ErrorMsg += "Error code: " + respCode;
//                                                     alert(ErrorMsg);
//                                                     topic_Publish(tDlg, 'MSUpgradeSelection','AVAILABLE');
//                                                   }
//                                                 }
//                                              } 
//                                           );
//    var reqParams = new Object;
//    reqParams["LocalFileName"] = imageFilePath;  
//    reqUpgradeMS.reqParams = reqParams;
//    reqUpgradeMS.sendRequest(); 
//  }
}
