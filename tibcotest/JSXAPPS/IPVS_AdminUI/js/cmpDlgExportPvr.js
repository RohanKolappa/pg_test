function cmpBlkExportPvrMedia_Create(objListDoc) {
  closeDialogIfExists("dlgPvrExport");
  var DlgFile = "components/media/mediaExportPvr.xml";
  
  var BlkJSX = PNameSpace.getBodyBlock().load(DlgFile);
  
  // setup a local topic for dlg specific messaging
  var ExportList = objListDoc;

  var tDlg = "Dialog_" + jsx3.xml.CDF.getKey();
  topic_Create(tDlg);

  // setup a listner for it
  var eventDlg = new EventListner(tDlg);
  var folderDlgHandle = null;

  function cmpBlkExportMedia_Destroy() {
    if( !jsx3.util.strEmpty(dlgSLider) ){
      dlgSLider.incrementStop();
    }

    // destroy any local eventlistners
    eventDlg.destroy();
    
    // destroy all local components
    cmpDlgExport.destroy();
    
    // destroy the local topic
    topic_Destroy(tDlg);

    folderDlgHandle = null;
    
    // close dialog
    BlkJSX.doClose();
  }

  var dlgSLider = null;

  var cmpBlkHiddenCDF = new Component(["dlgPvrExport", "HiddenBlockForCDF"]);

  var cmpBtnExportCancel = new CmpButton(["dlgPvrExport","btnMediaExportCancel"],
                                               null, 
                                               {action:"FNCCALL", 
                                                context:null, 
                                                fnc:function() {
                                                  cmpBlkExportMedia_Destroy();
                                                }
                                              });

  var cmpBtnExport = new CmpButton(["dlgPvrExport", "btnMediaExportOk"],
                                   null,
                                   { action: "FNCCALL",
                                     context: null,
                                     fnc: function () {
                                       var destinationPath = getJsxByName("txtFolderPath").getValue();
                                       if (!jsx3.util.strEmpty(destinationPath)) {
                                         var startOffset = dlgSLider.getPvrOffset("START");
                                         var endOffset = dlgSLider.getPvrOffset("END");
                                         var diffOffset = parseInt(endOffset - startOffset, 10);
                                         if (diffOffset == 0 && diffOffset < 108000) {
                                           alert("Please select atleast 1 min of PVR file to export.");
                                           return;
                                         }

                                         var strXMLCacheId = cmpBlkHiddenCDF.BlkJSX.getXMLId();
                                         var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
                                         var selRecord = objXML.selectSingleNode(".//record");
                                         selRecord.setAttribute("jsxstartOffset", dlgSLider.getPvrOffset("START"));
                                         selRecord.setAttribute("jsxendOffset", dlgSLider.getPvrOffset("END"));

                                         var reqExportMedia = new ReqExportImportMedia("AddExportFile", null,
                                                                                      { action: "FNCCALL", context: this,
                                                                                        fnc: function (respCode, respVal) {
                                                                                          if (respCode == "SUCCESS" && respVal != null) {
                                                                                            cmpBlkExportMedia_Destroy();
                                                                                            topic_Publish(tApp, "ExportState", "Started");
                                                                                          }
                                                                                          else {
                                                                                            var codeErrorMsg = CommonErrorCodeArr[respCode].errorDesc;
                                                                                            var ErrorMsg = "Error: Exporting files request failed \n";
                                                                                            ErrorMsg += "Error description: " + codeErrorMsg + "\n";
                                                                                            ErrorMsg += "Error code: " + respCode;
                                                                                            alert(ErrorMsg);
                                                                                          }
                                                                                        }
                                                                                      }
                                                                                     );
                                         var reqParams = new Object;
                                         reqParams["ExportPriority"] = "0";
                                         reqParams["MediaFormat"] = "Proprietary";
                                         reqExportMedia.reqParams = reqParams;
                                         reqExportMedia.sendRequest();
                                       }
                                       else {
                                         alert("Select the Destination Path");
                                         return;
                                       }
                                     }
                                   });
  var cmpBtnExportClose = new CmpButton(["dlgPvrExport","btnDialogCloseX"],
                                               null, 
                                               {action:"FNCCALL", 
                                                context:null, 
                                                fnc:function() {
                                                  cmpBlkExportMedia_Destroy();
                                                }
                                               });

  var cmpBtnBrowse = new CmpButton(["dlgPvrExport","btnMediaExportBrowse"],
                                   null, 
                                   {action:"FNCCALL", 
                                    context:null, 
                                    fnc:function() {
                                      folderDlgHandle.doBrowse();
                                    }
                                   });

  var cmpDlgExport = new CmpContainer("dlgPvrExport",
                                      [{ cmp: cmpBtnBrowse },
                                        { cmp: cmpBtnExportCancel },
                                        { cmp: cmpBtnExport },
                                        { cmp: cmpBtnExportClose },
                                        { cmp: cmpBlkHiddenCDF }
                                      ],
                                      null);
                                                            
  cmpDlgExport.BlkJSX = BlkJSX;
  cmpDlgExport.getBlkData =  cmpDlgExport_Repaint;

  //function to execute after dir selection
  cmpDlgExport.onFolderChoose = cmpDlgExport_onFolderChoose;
    
  // init and repaint happens after the dialog is loaded
  jsx3.sleep((function () {
      this.init();
      this.getBlkData();
    }
  ),
  null, cmpDlgExport);

  function cmpDlgExport_Repaint() {

    var strXMLCacheId = cmpBlkHiddenCDF.BlkJSX.getXMLId();
    var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
    objXML.loadXML(ExportList.toString());
    var selRecord = objXML.selectSingleNode(".//record");
    var pvrFileNID = selRecord.getAttribute("jsxid");
    // get the pvr file available pvr might have changed
    topic_Set(tDlg, "PVRFileNID", pvrFileNID);

    var reqGetFile = new ReqSessionGetRequest("GetPVRFile",
                                                { action: "FNCCALL", context: null,
                                                  fnc: function (respCode, respVal) {
                                                    if (respCode == "SUCCESS" && respVal != null) {
                                                      processResponse(respVal);
                                                    }
                                                    else {
                                                      try {
                                                        var codeErrorMsg = CommonErrorCodeArr[respCode].errorDesc;
                                                      } catch (e) {
                                                      }
                                                      var ErrorMsg = "Error: GetFile PVR request failed \n";
                                                      ErrorMsg += "Error description: " + codeErrorMsg + "\n";
                                                      ErrorMsg += "Error code: " + respCode;
                                                      alert(ErrorMsg);
                                                    }
                                                  }
                                                }
                                               );

    reqGetFile.ServiceName = "SessionService";
    var reqParams = new Object;
    reqParams["ObjectList_WhereNID"] = pvrFileNID;
    reqParams["ObjectList_SelectFormat"] = "EXTENDED";
    reqParams["ObjectList_WherePermissions"] = "MODIFY,DELETE,EXPORT";

    reqGetFile.reqParams = reqParams;
    // send the request
    reqGetFile.sendRequest(this); 

    folderDlgHandle = new DirSelection(cmpDlgExport.onFolderChoose);
  }

  function cmpDlgExport_onFolderChoose() {
    var exportDirPath = folderDlgHandle.getFolderPath();
    getJsxByName("txtFolderPath").setValue(exportDirPath);
  }

  function processResponse(respVal) {
    var fileNode = respVal.selectSingleNode(".//File");
    if (jsx3.util.strEmpty(fileNode)) return;

    var availPlaylength = fileNode.getAttribute("playlength");
    var availStartTC = fileNode.getAttribute("startTC");
    var availTrimLength = fileNode.getAttribute("trimLength");

    if (parseInt(availPlaylength, 10) < 0) {
      var ErrorMsg = "Error: GetPVRFile response gives \n";
      ErrorMsg += "playlength: " + availPlaylength + ".\n";
      ErrorMsg += "This happens only when your DMS and MS are not time synced.";
      alert(ErrorMsg);
      return;
    }

    dlgSLider = cmpDlgExport.BlkJSX.getDescendantsOfType("com.ipvs.gui.PvrRecordSlider")[0];
    dlgSLider.init();

    var pvrInfoObj = new Object;
    pvrInfoObj.action = "Start";
    pvrInfoObj.enablePVR = true;
    pvrInfoObj.isTimeShifted = false;
    pvrInfoObj.playlength = availPlaylength;
    pvrInfoObj.pvrInfo = new Object;
    pvrInfoObj.pvrInfo.canPVR = true;
    pvrInfoObj.pvrInfo.pvrState = "STREAMING";
    pvrInfoObj.pvrInfo.startTC = availStartTC;
    pvrInfoObj.pvrInfo.timeLength = (jsx3.util.strEmpty(availStartTC)) ? availPlaylength : availTrimLength;
    pvrInfoObj.timeMode = "PERSISTENT_PVR";
    pvrInfoObj.timeInfo = new Object;
    pvrInfoObj.timeInfo.startWC = "";
    pvrInfoObj.timeInfo.lastWC = "";
    pvrInfoObj.timeInfo.lastOffset = "";
    pvrInfoObj.timeInfo.timeScale = "";

    dlgSLider.applyCollaborationInfo(pvrInfoObj);
     dlgSLider.show();
  }


  function convertMMtoHrMinTemp(trimLength) {
    var convertedVal = "00:00";
    var trimLengthMin = parseInt(trimLength, 10) / (60 * 1000);
    if (trimLengthMin < 60) {
      convertedVal = "00:" + parseInt(trimLengthMin,10);
    }
    else {
      var trimLengthHr = parseInt(trimLength, 10) / (60 * 60 * 1000);
      trimLengthHr = Math.floor(trimLengthHr)
      if (trimLengthHr >= 1 && trimLengthHr <= 24) {
        trimLengthMin = parseInt(trimLength, 10) - (parseInt(trimLengthHr, 10) * (60 * 60 * 1000));
        trimLengthMin = parseInt(trimLengthMin, 10) / (60 * 1000);

        convertedVal = parseInt(trimLengthHr,10) + ":" + parseInt(trimLengthMin,10) ;
      }
    }
    return convertedVal;
  }

}