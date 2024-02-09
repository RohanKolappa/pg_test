// this code has some potential issues 
// - w.r.t object reference counts - when objects created in here go out of scope etc
// - loading sync issues - dlg may not be loaded before the object init methods are called

function dlgEditMediaSFRTitle_Create(MediaFileNID) {

  // deal with defaults for add
  if( MediaFileNID == null ) return;
  
  closeDialogIfExists("DialogMediaFileTitle");
  // load view - dialog component file
  var DlgFile = "components/media/mediaFileEditTitleDialog.xml";
  var BlkJSX = PNameSpace.getBodyBlock().load(DlgFile);
  
  // setup a local topic for dlg specific messaging
  var tDlg = "Dialog_" + jsx3.xml.CDF.getKey()
  topic_Create(tDlg);

  // setup a listner for it
  var eventDlg = new EventListner(tDlg);
  
  function dlgEditMediaSFRTitle_Destroy() {
    // destroy any local eventlistners
    eventDlg.destroy();
    
    // destroy all local components
    cmpDlgEditMediaSFRTitle.destroy();
    
    // destroy the local topic
    topic_Destroy(tDlg);
    
    // close dialog
    BlkJSX.doClose();
  }
 
  var cmpBtnEditMediaFileTitleOK = new CmpButton(["DialogMediaFileTitle","btnDoneMediaSFR"],
                                           null, 
                                           {action:"FNCCALL", 
                                            context:null, 
                                            fnc:function() {
                                              eventDlg.subscribe("EditMediaFileSetMediaFileState", "DONE", this,
                                                                function() {
                                                                  dlgEditMediaSFRTitle_Destroy();
                                                                });
                                              cmpDlgEditMediaSFRSetMediaSFRTitle.call(this, MediaFileNID);
                                            }
                                           });
            
  var cmpBtnEditMediaFileTitleCancel = new CmpButton(["DialogMediaFileTitle","btnCancelMediaSFR"],
                                               null, 
                                               {action:"FNCCALL", 
                                                context:null, 
                                                fnc:function() {
                                                  dlgEditMediaSFRTitle_Destroy();
                                                }
                                               });

  var cmpBtnEditMediaFileTitleClose = new CmpButton(["DialogMediaFileTitle","btnDialogCloseX"],
                                               null, 
                                               {action:"FNCCALL", 
                                                context:null, 
                                                fnc:function() {
                                                  dlgEditMediaSFRTitle_Destroy();
                                                }
                                               });                                               
                                                                                         
  var cmpDlgEditMediaSFRTitle = new CmpContainer("DialogMediaFileTitle",
                                [ {cmp:cmpBtnEditMediaFileTitleClose},
                                  {cmp:cmpBtnEditMediaFileTitleCancel},
                                  {cmp:cmpBtnEditMediaFileTitleOK}
                                ],
                                [{action:"TOPICEVENT", topic:tDlg, eventvar:"EditMediaFileDataState", eventval:MediaFileNID}]);
                                                            
  cmpDlgEditMediaSFRTitle.BlkJSX = BlkJSX;
  cmpDlgEditMediaSFRTitle.ObjectNID = MediaFileNID;
  cmpDlgEditMediaSFRTitle.getBlkData =  cmpDlgEditMediaSFRGetMediaSFRTitle;
  
  // init and repaint happens after the dialog is loaded
  jsx3.sleep( (function(){
                 this.init();
                 this.getBlkData(this.ObjectNID);
                 }
               ),
               null, cmpDlgEditMediaSFRTitle ); 

  // fnc to get dlg data
  function cmpDlgEditMediaSFRGetMediaSFRTitle(MediaFileNID) {

    // helper function
    function processFileData(respData) {
      var metadataNode = respData.selectSingleNode("./MetaData");
      var fileName = respData.getAttribute("title");
      var description = respData.selectSingleNode("./description").getValue();

      PNameSpace.getJSXByName("txtMediaFileName").setValue(fileName, true);
      PNameSpace.getJSXByName("txtMediaFileDescription").setValue(description, true);
      topic_Publish(tDlg, "EditMediaFileDataState", MediaFileNID);
    }

    //main code get the file
    var reqGetFile = new ReqSessionGetRequest("GetFile",
                                                { action: "FNCCALL", context: null,
                                                  fnc: function (respCode, respVal) {
                                                    if (respCode == "SUCCESS" && respVal != null) {
                                                      processFileData(respVal.selectSingleNode(".//File[@NID='"+MediaFileNID+"']"));
                                                    }
                                                    else {
                                                      try {
                                                        var codeErrorMsg = CommonErrorCodeArr[respCode].errorDesc;
                                                      } catch (e) {
                                                      }
                                                      var ErrorMsg = "Error: GetFile request failed \n";
                                                      ErrorMsg += "Error description: " + codeErrorMsg + "\n";
                                                      ErrorMsg += "Error code: " + respCode;
                                                      alert(ErrorMsg);
                                                    }
                                                  }
                                                }
                                               );

    reqGetFile.ServiceName = "SessionService";
    var reqParams = new Object;
    reqParams["ObjectList_WhereNID"] = MediaFileNID;
    reqParams["ObjectList_SelectFormat"] = "EXTENDED";
    reqParams["ObjectList_WherePermissions"] = "MODIFY,DELETE,EXPORT";

    reqGetFile.reqParams = reqParams;
    // send the request
    reqGetFile.sendRequest();

  } // end 

  function cmpDlgEditMediaSFRSetMediaSFRTitle(MediaFileNID) {
    if (cmpDlgEditMediaSFRValidate() != true) return;
    
    var valFullName = PNameSpace.getJSXByName("txtMediaFileName").getValue();
    var valDescription = PNameSpace.getJSXByName("txtMediaFileDescription").getValue();
    var reqSendMediaSFRStoreInfo = new ReqSessionGetRequest("SetFileInfo",
                                                            { action: "FNCCALL", context: null,
                                                                fnc: function (respCode, respVal) {
                                                                    if(respCode == "SUCCESS" && respVal != null) {
                                                                      dlgEditMediaSFRTitle_Destroy();
                                                                    }
                                                                    else {
                                                                      try {
                                                                         var codeErrorMsg = CommonErrorCodeArr[respCode].errorDesc;
                                                                      } catch (e) {
                                                                      }
                                                                      var ErrorMsg = "Error: SetFileInfo request failed \n";
                                                                      ErrorMsg += "Error description: " + codeErrorMsg + "\n";
                                                                      ErrorMsg += "Error code: " + respCode;
                                                                      alert(ErrorMsg);
                                                                    }
                                                                }
                                                            }
                                                           );
    reqSendMediaSFRStoreInfo.ServiceName = "SessionService";
    var reqParams = new Object;
    reqParams["ObjectList_ByNID"] = MediaFileNID;
    reqParams["ObjectList_Title"] = valFullName;
    reqParams["ObjectList_description"] = valDescription;
    reqSendMediaSFRStoreInfo.reqParams = reqParams;
    // send the request
    reqSendMediaSFRStoreInfo.sendRequest();
  }

  function cmpDlgEditMediaSFRValidate() {
    var valFullName = PNameSpace.getJSXByName("txtMediaFileName").getValue();

    var fullName = checkValue(valFullName , "txtMediaFileName" , "Please enter file name");
    if(fullName)
      return true;
      
    return false;
  } // end cmpDlgEditMediaSFRValidate
}