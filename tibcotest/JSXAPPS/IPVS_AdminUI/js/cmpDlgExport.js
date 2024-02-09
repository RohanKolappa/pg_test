function cmpBlkExportMedia_Create(objListDoc) {
  closeDialogIfExists("dlgExport");
  var DlgFile = "components/media/mediaExport.xml";
  
  var BlkJSX = PNameSpace.getBodyBlock().load(DlgFile);
  
  // setup a local topic for dlg specific messaging
  var ExportList = objListDoc;

  var tDlg = "Dialog_" + jsx3.xml.CDF.getKey();
  topic_Create(tDlg);

  // setup a listner for it
  var eventDlg = new EventListner(tDlg);
  var folderDlgHandle = null;
  
  function cmpBlkExportMedia_Destroy() {
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

  var cmpExportList =  new Component(["dlgExport","mtxExportList"]);
  
  var cmpSelMediaExportFormat = new CmpStaticList(["dlgExport","selMediaExportFormat"],
                                                  "xml/selections/MediaExportFormatType.xml",  
                                                  null,
                                                  {action:"FNCCALL", 
                                                   context:null, 
                                                   fnc:function() {
                                                      var selMediaExportFormatVal = cmpSelMediaExportFormat.BlkJSX.getValue();
                                                      if (jsx3.util.strEmpty(selMediaExportFormatVal)) return;
                                                   }
                                                  });

  var cmpMediaFormatHelp = new CmpBlock(["dlgExport", "blKMediaFormatHelp"]);
  cmpMediaFormatHelp.BlkJSX.setCanSpy(jsx3.Boolean.TRUE);
  cmpMediaFormatHelp.BlkJSX.setEvent("showMediaExportFormatHelp();", jsx3.gui.Interactive.SPYGLASS);

  var cmpBtnExportCancel = new CmpButton(["dlgExport","btnMediaExportCancel"],
                                               null, 
                                               {action:"FNCCALL", 
                                                context:null, 
                                                fnc:function() {
                                                  cmpBlkExportMedia_Destroy();
                                                }
                                               });
  var cmpBtnExport = new CmpButton(["dlgExport","btnMediaExportOk"],
                                   null, 
                                   {action:"FNCCALL", 
                                    context:null, 
                                    fnc:function() {
                                      var destinationPath = getJsxByName("txtFolderPath").getValue();
                                      if(!jsx3.util.strEmpty(destinationPath)){
                                        var reqExportMedia = new ReqExportImportMedia("AddExportFile", null,  
                                                                                      {action:"FNCCALL", context:this, 
                                                                                         fnc:function(respCode, respVal) {
                                                                                               if(respCode == "SUCCESS" && respVal != null) {
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
                                        reqParams["ExportPriority"] = "2";
                                        reqParams["MediaFormat"] = cmpSelMediaExportFormat.BlkJSX.getValue();
                                        reqExportMedia.reqParams = reqParams;   
                                        reqExportMedia.sendRequest();
                                      }
                                      else{
                                        alert("Select the Destination Path");
                                        return;
                                      }
                                    }
                                   });
  var cmpBtnExportClose = new CmpButton(["dlgExport","btnDialogCloseX"],
                                               null, 
                                               {action:"FNCCALL", 
                                                context:null, 
                                                fnc:function() {
                                                  cmpBlkExportMedia_Destroy();
                                                }
                                               });

  var cmpBtnBrowse = new CmpButton(["dlgExport","btnMediaExportBrowse"],
                                   null, 
                                   {action:"FNCCALL", 
                                    context:null, 
                                    fnc:function() {
                                      folderDlgHandle.doBrowse();
                                    }
                                   });
                                                                                         
  var cmpDlgExport = new CmpContainer("dlgExport",
                              [ {cmp:cmpExportList},
                                {cmp:cmpSelMediaExportFormat},
                                {cmp:cmpMediaFormatHelp},
                                {cmp:cmpBtnBrowse},
                                {cmp:cmpBtnExportCancel},
                                {cmp:cmpBtnExport},
                                {cmp:cmpBtnExportClose}                                
                              ],
                              null);
                                                            
  cmpDlgExport.BlkJSX = BlkJSX;
  cmpDlgExport.getBlkData =  cmpDlgExport_Repaint;

  //function to execute after dir selection
  cmpDlgExport.onFolderChoose = cmpDlgExport_onFolderChoose
    
  // init and repaint happens after the dialog is loaded
  jsx3.sleep( (function(){
                 this.init();
                 this.getBlkData();
                 }
               ),
               null, cmpDlgExport ); 
               
  function cmpDlgExport_Repaint() {
    var strXMLCacheId = cmpExportList.BlkJSX.getXMLId();
    var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
    objXML.loadXML(ExportList.toString());
    //Passing Call back function to DirSelection class
    folderDlgHandle = new DirSelection(cmpDlgExport.onFolderChoose);
    //set default value
    cmpSelMediaExportFormat.BlkJSX.setValue("Proprietary");
  }

  function cmpDlgExport_onFolderChoose() {
    var exportDirPath = folderDlgHandle.getFolderPath();
    getJsxByName("txtFolderPath").setValue(exportDirPath);
  }
}