function dlgDownloadReport_Create(ReportNID, ReportFormat, ReportName) {   

  closeDialogIfExists("dlgDownloadReport");
  // load view - dialog component file
  var DlgFile = "components/admin/dlgDownloadReport.xml";
  //var BlkJSX = PNameSpace.getBodyBlock().load(DlgFile);
  var BlkJSX = PNameSpace.getBodyBlock().loadAndCache(DlgFile);
  
  var tDlg = "Dialog_" + jsx3.xml.CDF.getKey()
  topic_Create(tDlg);
  
  // setup a listner for it    
  var eventDlg = new EventListner(tDlg);
  
  function dlgDownloadReport_Destroy() {
    // destroy any local eventlistners
    eventDlg.destroy();
    
    // destroy all local components
    cmpDlgDownloadReport.destroy();

    // destroy the local topic                  
    topic_Destroy(tDlg);
    
    // close dialog    
    BlkJSX.doClose();
  }
  topic_Set(tDlg, 'DownloadReport','AVAILABLE');  
  var cmpBtnDownloadReportClose = new CmpButton([BlkJSX,"btnDialogCloseX"], null, 
                                                {action:"FNCCALL", context:null, 
                                                 fnc:function() {
                                                   dlgDownloadReport_Destroy();
                                                 }
                                                });

  var cmpBlkDirPicker = new Component([BlkJSX,"blkDirPicker"],null,null,null);
                                              
  var cmpBtnDownloadReport = new CmpButton([BlkJSX,"btnDownloadReport"], null, 
                                           {action:"FNCCALL", context:null, 
                                            fnc:function() {
                                              var txtDirPath =  getJsxByName([BlkJSX,"txtDirPath"]);
                                              var localDirPath = txtDirPath.getValue();
                                              if(!jsx3.util.strEmpty(localDirPath)){
                                                var txtFileName = getJsxByName([BlkJSX,"txtFileName"]);
                                                var localFileName = txtFileName.getValue();
                                                if(!jsx3.util.strEmpty(localFileName)){
                                                  var fso = new ActiveXObject("Scripting.FileSystemObject");
                                                  var localFilePath = localDirPath + "\\" + localFileName;
                                                  if(!fso.FileExists(localFilePath)){
                                                      cmpDlgDownloadReportBeforeSendRequest(localDirPath, localFileName);
                                                  }
                                                  else{
                                                    var agree = confirm('Filename already exists. Do you want to overwrite?');
                                                    if(!agree) return false;
                                                    cmpDlgDownloadReportBeforeSendRequest(localDirPath, localFileName);
                                                  }
                                                }
                                                else{
                                                 alert("Please enter a File Name to Download Report");  
                                                }
                                              }
                                              else{
                                                alert("Please select a Directory to Download Report");
                                              }
                                            }
                                           });
                                           
  var cmpBlkStatus = new Component([BlkJSX,"blkStatus"]);                                        
   
  var cmpBlkDownload = new CmpContainer([BlkJSX,"blkDownload"],
                                       [{cmp:cmpBtnDownloadReport,
                                         sel:"topic_Get('" + tDlg + "', 'DownloadReport') == 'AVAILABLE'"},
                                        {cmp:cmpBlkStatus,
                                         sel:"topic_Get('" + tDlg + "', 'DownloadReport') == 'PROGRESS'"}
                                       ],
                                       [{action:"TOPICEVENT", topic: tDlg, eventvar:"DownloadReport", eventval:null}] );
                                           
  var cmpBtnCancel = new CmpButton([BlkJSX,"btnCancel"], null, 
                                   {action:"FNCCALL", context:null, 
                                    fnc:function() {
                                      dlgDownloadReport_Destroy();
                                    }
                                   });

  
  var cmpDlgDownloadReport = new CmpContainer([BlkJSX,"dlgDownloadReport"],
                                              [ {cmp:cmpBlkDirPicker},
                                                {cmp:cmpBtnDownloadReportClose},
                                                {cmp:cmpBlkDownload},
                                                {cmp:cmpBtnCancel}
                                              ],
                                              null);
  
  cmpDlgDownloadReport.BlkJSX = BlkJSX;
  cmpDlgDownloadReport.ObjectNID = ReportNID;
  cmpDlgDownloadReport.ObjectName = ReportName;
  cmpDlgDownloadReport.ObjectFormat = ReportFormat;
  cmpDlgDownloadReport.getBlkData = cmpDlgDownloadReportSetDefaultFileName;
    // init and repaint happens after the dialog is loaded
  jsx3.sleep( (function(){
                 this.init();
                 this.getBlkData(this.ObjectName,this.ObjectFormat);
                 }
               ),
               null, cmpDlgDownloadReport );
  
  //function to set the Default Name ReportName-timestamp.csv/xml
  function cmpDlgDownloadReportSetDefaultFileName(ReportName, ReportFormat){
    var date = formatDate(newServerDate(),"yyyy-MMM-dd-HH.mm.ss");
    if(ReportFormat == "CSV"){
      var extension = '.csv';
    }
    else{
      var extension = '.xml';
    }
    var fileName = ReportName+'-'+ date + extension;
    var txtFileName =  getJsxByName([BlkJSX,"txtFileName"]);
    txtFileName.setValue(fileName);
  }// end cmpDlgDownloadReportSetDefaultFileName           
  
  function cmpDlgDownloadReportBeforeSendRequest(localDirPath, localFileName){
    var fileExtension = localFileName.substring(localFileName.lastIndexOf(".")+1,localFileName.length);
    if(!jsx3.util.strEmpty(fileExtension)&& ((ReportFormat == "CSV" && fileExtension =="csv")||(ReportFormat == "XML" && fileExtension =="xml"))){
      topic_Publish(tDlg, 'DownloadReport','PROGRESS');
      cmpDlgDownloadReportSendDownloadReq(cmpDlgDownloadReport.ObjectNID,localDirPath + "\\"+localFileName);
    }
    else{
      alert("Please enter a ." + ReportFormat.toLowerCase()+" extension that match the report format"); 
    }
  }
  //function to send Download report Request
  function cmpDlgDownloadReportSendDownloadReq(ReportNID, FileName) {   
    var reqDownloadReport = new ReqSessionGetRequest("DownloadReport",
                                                    { action: "FNCCALL", context: null,
                                                      fnc: function(respCode, respVal) {
                                                        if (respCode == "SUCCESS" && respVal != null) { 
                                                          alert("Report was downloaded successfully to location"+ FileName);
                                                        }
                                                        else {
                                                          log.error("Get Report request failed for Report NID: " + ReportNID);
                                                          alert("Report download failed");
                                                        }
                                                        topic_Publish(tDlg, 'DownloadReport','AVAILABLE');
                                                        dlgDownloadReport_Destroy();
                                                      }
                                                    }
                                                   );
    reqDownloadReport.ServiceName = "SessionService";
    var reqParams = new Object;
    reqParams["ObjectList_WhereNID"] = ReportNID;
    reqParams["ObjectList_toFileName"] = FileName;
    reqDownloadReport.reqParams = reqParams;
    // send the request
    reqDownloadReport.sendRequest(this);  
  }// end cmpDlgDownloadReportSendDownloadReq           
               
}// end dlgDownloadReport_Create