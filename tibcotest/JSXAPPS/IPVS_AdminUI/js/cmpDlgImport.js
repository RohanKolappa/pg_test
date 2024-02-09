function cmpBlkImportMedia_Create(selDirNID) {
  closeDialogIfExists("dlgImport");
  var DlgFile = "components/media/mediaImport.xml";
  
  var BlkJSX = PNameSpace.getBodyBlock().load(DlgFile);
  
  // setup a local topic for dlg specific messaging
  var ImportDirNID = selDirNID;

  var tDlg = "Dialog_" + jsx3.xml.CDF.getKey();
  topic_Create(tDlg);

  // setup a listner for it
  var eventDlg = new EventListner(tDlg);
  var folderDlgHandle = null;
  
  function cmpBlkImportMedia_Destroy() {
    // destroy any local eventlistners
    eventDlg.destroy();
    
    // destroy all local components
    cmpDlgImport.destroy();
    
    // destroy the local topic
    topic_Destroy(tDlg);

    folderDlgHandle = null;
    
    // close dialog
    BlkJSX.doClose();
  }

  var cmpImportList = new Component(["dlgImport", "mtxImportList"]);
  cmpImportList.BlkJSX.getDescendantOfName("chkjsxChkColumn").setEvent('eval("handleCheckboxToggle(this);")', jsx3.gui.Interactive.TOGGLE);

  handleCheckboxToggle = function (obj) {    
    var objRecordId = obj.emGetSession().recordId;
    var ObjRecord = obj.emGetSession().matrix.getRecordNode(objRecordId);
    if (ObjRecord.getAttribute("jsxStreamType") == "GROUP") {
      var checkVal = ObjRecord.getAttribute("jsxChkColumn");
      var clipRecordNodes = ObjRecord.selectNodes("./record");
      for (var it = clipRecordNodes.iterator() ; it.hasNext() ;) {
        var clipRecordNode = it.next();
        clipRecordNode.setAttribute("jsxChkColumn", checkVal);
      }
      cmpImportList.BlkJSX.repaint();
    }
  }   // end DeleteSession


  var cmpChkSelectUnselect = new CmpCheckBox(["dlgImport", "chkSelectUnselect"], null,
                                             { action: "FNCCALL",
                                               context: this,
                                               fnc: function () {
                                                 var checkVal = "0";
                                                 if (cmpChkSelectUnselect.BlkJSX.getChecked() == jsx3.gui.CheckBox.CHECKED) {
                                                   checkVal = "1";
                                                 }
                                                 var strXMLCacheId = cmpImportList.BlkJSX.getXMLId();
                                                 var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
                                                 if (objXML) {
                                                   var objRecordNodes = objXML.selectNodes("//record");
                                                   for (var it = objRecordNodes.iterator(); it.hasNext(); ) {
                                                     var objRecordNode = it.next();
                                                     objRecordNode.setAttribute("jsxChkColumn", checkVal);
                                                   }
                                                 }
                                                 cmpImportList.BlkJSX.repaint();
                                               }
                                             }
                                            );

  var cmpBtnImportCancel = new CmpButton(["dlgImport","btnMediaImportCancel"],
                                               null, 
                                               {action:"FNCCALL", 
                                                context:null, 
                                                fnc:function() {
                                                  handleMask(false);
                                                  cmpBlkImportMedia_Destroy();
                                                }
                                              });

  var cmpBtnImport = new CmpButton(["dlgImport", "btnMediaImportOk"],
                                   null,
                                   { action: "FNCCALL",
                                     context: null,
                                     fnc: function () {
                                       var sourcePath = getJsxByName("txtFolderPath").getValue();
                                       if (!jsx3.util.strEmpty(sourcePath)) {
                                         var fileSelected = null;
                                         // recreate the mtx cache data with only the checked files
                                         var strXMLCacheId = cmpImportList.BlkJSX.getXMLId();
                                         var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
                                         if (objXML) {
                                           if (!objXML.selectNodeIterator("./record").hasNext()) {
                                             alert("No media files to be imported.");
                                             return;
                                           }

                                           fileSelected = objXML.selectNodes("./record[@jsxChkColumn='1']");
                                           if (fileSelected.getLength() == 0) {
                                             alert("Please Select a file/files to import.");
                                             return;
                                           }
                                         }
                                         else {
                                           return;
                                         }

                                         //check if it is 2dot4 media then create temp folder to hold .zip files
                                         if (getJsxByName("txtIsMedia2dot4").getValue() == "true") {
                                           var tmpFolderName = "tmp" + (newServerDate()).getTime();
                                           var tmpFolderPath = folderDlgHandle.getFolderPath() + "\\" + tmpFolderName;
                                           var FSO = new jsx3.io.FSOFileSystem();
                                           var FSOFile = FSO.getFile(tmpFolderPath);
                                           FSOFile.mkdir();
                                           folderDlgHandle.FolderPath = tmpFolderPath;
                                           getJsxByName("txtFolderPath").setValue(tmpFolderPath);
                                           delete FSO;
                                         }

                                         var reqImportMedia = new ReqExportImportMedia("AddImportFile", null,
                                                                                      { action: "FNCCALL", context: this,
                                                                                        fnc: function (respCode, respVal) {
                                                                                          if (respCode == "SUCCESS" && respVal != null) {
                                                                                            cmpBlkImportMedia_Destroy();
                                                                                            topic_Publish(tApp, "ImportState", "Started");
                                                                                          }
                                                                                          else {
                                                                                            var codeErrorMsg = CommonErrorCodeArr[respCode].errorDesc;
                                                                                            var ErrorMsg = "Error: Importing files request failed \n";
                                                                                            ErrorMsg += "Error description: " + codeErrorMsg + "\n";
                                                                                            ErrorMsg += "Error code: " + respCode;
                                                                                            alert(ErrorMsg);
                                                                                          }
                                                                                        }
                                                                                      }
                                                                                     );
                                         reqImportMedia.sendRequest();
                                       }
                                       else {
                                         alert("Select the Source Path");
                                         return;
                                       }
                                     }
                                   });
  var cmpBtnImportClose = new CmpButton(["dlgImport","btnDialogCloseX"],
                                               null, 
                                               {action:"FNCCALL", 
                                                context:null, 
                                                fnc:function() {
                                                  handleMask(false);
                                                  cmpBlkImportMedia_Destroy();
                                                }
                                               });

  var cmpBtnBrowse = new CmpButton(["dlgImport","btnMediaImportBrowse"],
                                   null, 
                                   {action:"FNCCALL", 
                                    context:null, 
                                    fnc:function() {
                                      folderDlgHandle.doBrowse();
                                    }
                                   });

  var cmpDlgImport = new CmpContainer("dlgImport",
                              [{ cmp: cmpImportList },
                                { cmp: cmpBtnBrowse },
                                { cmp: cmpBtnImportCancel },
                                { cmp: cmpBtnImport },
                                { cmp: cmpBtnImportClose },
                                { cmp: cmpChkSelectUnselect }
                              ],
                              null);
                                                            
  cmpDlgImport.BlkJSX = BlkJSX;
  cmpDlgImport.getBlkData =  cmpDlgImport_Repaint;
  //function to execute after dir selection
  cmpDlgImport.onFolderChoose = cmpDlgImport_onFolderChoose
      
  // init and repaint happens after the dialog is loaded
  jsx3.sleep( (function(){
                 this.init();
                 this.getBlkData();
                 }
               ),
               null, cmpDlgImport );

  function cmpDlgImport_Repaint() {   
    getJsxByName("txtSelDirNID").setValue(ImportDirNID);
    folderDlgHandle = new DirSelection(cmpDlgImport.onFolderChoose);
  }

  function cmpDlgImport_onFolderChoose() {
    cmpImportList.BlkJSX.getDescendantOfName("jsxtextmediaCreated", false, false).setFormatHandler("@datetime,MM/dd/yyyy HH:mm");
    cmpImportList.BlkJSX.getDescendantOfName("jsxtextmediaSize", false, false).setFormatHandler(convertbytesToSize);
    cmpImportList.BlkJSX.getDescendantOfName("jsxtextmediaLength", false, false).setFormatHandler(MediaFileTime);

    var importDirPath = folderDlgHandle.getFolderPath();
    getJsxByName("txtFolderPath").setValue(importDirPath);
    handleMask(true);
    processSelectedFolder();
    handleMask(false);
  }
  
  function handleMask(flagMask){
    if(flagMask){
      cmpBtnImport.BlkJSX.setEnabled(jsx3.gui.Form.STATEDISABLED, true);
      cmpImportList.BlkJSX.getParent().showMask("Loading...");
    }
    else{
      cmpBtnImport.BlkJSX.setEnabled(jsx3.gui.Form.STATEENABLED, true);
      cmpImportList.BlkJSX.getParent().hideMask();
    }
  }

  function processSelectedFolder() {
    var folderItems = folderDlgHandle.getFilesList();
    //check for db/blob folders (2.4 Media)
    var dbDir = null;
    var blobDir = null;
    for( var i = 0; i < folderItems.length; i++ ) {
      var folderItem = folderItems[i];
      if( folderItem.isHidden() || !folderItem.exists() ) continue;

      var bDirectory = folderItem.isDirectory();
      if( bDirectory && folderItem.getName() == "db" ) {
        dbDir = folderItem;
      }
      else if( bDirectory && folderItem.getName() == "blob" ) {
        blobDir = folderItem;
      }
    }
    var mkdColObj = cmpImportList.BlkJSX.getDescendantOfName("jsxtextMKDName", false, false)
    if (dbDir != null && blobDir != null) {
      mkdColObj.setText("Blob Folder", true);
      var blobList = dbDir.listFiles();
      cmpImportList.BlkJSX.clearXmlData();
      for( var i = 0; i < blobList.length; i++ ) {
        var blobItem = blobList[i];
        if( blobItem.isHidden() || !blobItem.exists() ) continue;
          var fileContent = blobItem.read();
          getTwoDotXMediaList(fileContent, blobItem.getName());
      }
      cmpImportList.BlkJSX.repaintData();
      getJsxByName("txtBlobFolder").setValue(folderDlgHandle.getFolderPath());
      getJsxByName("txtIsMedia2dot4").setValue("true");
      
    }
    else {
      mkdColObj.setText("MKD File Name", true);
      getJsxByName("txtBlobFolder").setValue("");
      getJsxByName("txtIsMedia2dot4").setValue("false");
      getImportList();
    }
  }
  
  function getImportList() {
    var mediaConvertorPath = giStartPath + "\\JSXAPPS\\IPVS-Common\\utils\\mediaconverter.exe";


    // helper function
    function processMKDFile(objFile, infoArg) {
      //For each MKD file get the info using mediaconverter.exe.
      //This will creates tmp file with info.
      //Read the content and delete before process next file.
      if (jsx3.util.strEmpty(infoArg)) infoArg = "metadata";
      var activeXWSH = new ActiveXObject('WScript.Shell');
      var tempFile = activeXWSH.ExpandEnvironmentStrings("%TEMP%\\mkd.txt");
      var clsFSOFileSystem = new jsx3.io.FSOFileSystem();

      var tempFileContent = null;
      var infoCMD = "cmd /c \"\"" + mediaConvertorPath + "\" -i " + infoArg + " -f \"" + objFile.getAbsolutePath() + "\" >> \"" + tempFile;
      var cmdResult = activeXWSH.Run(infoCMD, 0, true);
      if (cmdResult == 0) { //0 is success
        var tempFileData = clsFSOFileSystem.getFile(tempFile);
        tempFileContent = tempFileData.read();
        tempFileData.deleteFile();
      }
      else {
        //log.warn("MediaConverter: Error while getting Group/File info");
      }
      activeXWSH.Quit;
      delete clsFSOFileSystem;
      return tempFileContent;

    } // end processMKDFile

    // helper function
    function processGRPFile(objFile) {
      //.grp file is a text file with xml content with .grp extension
      var clsFSOFileSystem = new jsx3.io.FSOFileSystem();
      var grpFileContent = null;
      var tempFileData = clsFSOFileSystem.getFile(objFile.getAbsolutePath());
      grpFileContent = tempFileData.read();
      delete clsFSOFileSystem;
      return grpFileContent;
    } // end processGRPFile

    function isLatestServiceVersion(currSerVer) {
      if (jsx3.util.strEmpty(currSerVer)) return false;
      var baseSerVer = "2.31.9";
      var arrBaseSerVer = baseSerVer.split(".");
      var arrCurrSerVer = currSerVer.split(".");
      for (var i = 0; i <= arrBaseSerVer.length; i++) {
        if (parseInt(arrCurrSerVer[i]) > parseInt(arrBaseSerVer[i])) {
          return true;
        }
      }
      return false;
    }

    //helper function
    function insertMKDMediaList(strRecord, orgFileName, fileSizeOnDisk) {
      //From converter info get the required information and add records to list
      var mediaDoc = (new jsx3.xml.Document()).loadXML(strRecord);
      var grpMediaNID = "NONE";
      var clipMediaRecordNode = null;
      var streamType = null;
      var mediaStoreFileNode = mediaDoc.selectSingleNode("//MediaStoreFile");
      grpMediaNID = mediaStoreFileNode.selectSingleNode(".//MediaGroupID").getValue();
      if (isLatestServiceVersion(mediaStoreFileNode.getAttribute("serviceVer"))) {
        if (mediaStoreFileNode.selectSingleNode(".//Type").getValue() == "MediaClip") {
          var jsxMediaFileImg = "../../addins/ipvsCommon/images/icons/icon_reel.png";
        }
        else if (mediaStoreFileNode.selectSingleNode(".//Type").getValue() == "MediaGroup") {
          var jsxMediaFileImg = "../../addins/ipvsCommon/images/icons/icon_reel.png";
          streamType = "GROUP"
        }
        else {
          var jsxMediaFileImg = "../../addins/ipvsCommon/images/icons/icon_camera.png";
          streamType = "IMAGE";
        }
        if (jsx3.util.strEmpty(streamType)) streamType = mediaStoreFileNode.selectSingleNode(".//StreamType").getValue();

        if (jsx3.util.strEmpty(grpMediaNID)) grpMediaNID = "NONE";
        var objClipRec = new Object;
        objClipRec.jsxid = (streamType == "GROUP") ? grpMediaNID : orgFileName;
        objClipRec.jsxFileNID = orgFileName;
        objClipRec.jsxtextMKDName = orgFileName.substr(0,40) + "...";
        objClipRec.jsxtextmediaName = mediaStoreFileNode.selectSingleNode(".//Title").getValue();
        objClipRec.jsxtextmediaCreated = mediaStoreFileNode.selectSingleNode(".//DateCreated").getValue();
        objClipRec.jsxtextmediaLength = jsx3.util.strEmpty(mediaStoreFileNode.selectSingleNode(".//PlayLength")) ? "--" : mediaStoreFileNode.selectSingleNode(".//PlayLength").getValue();
        objClipRec.jsxtextmediaSize = fileSizeOnDisk; //Bug#47448
        objClipRec.jsxStreamType = streamType;
        objClipRec.jsxMediaFileImg = jsxMediaFileImg;
        objClipRec.jsxMediaName = orgFileName;
        objClipRec.jsxBlobSize = fileSizeOnDisk;
        objClipRec.jsxBlobName = "";
        objClipRec.jsxGrpNID = grpMediaNID;
        objClipRec.jsxChkColumn = "1";

        cmpImportList.BlkJSX.insertRecord(objClipRec, grpMediaNID, false);
        cmpChkSelectUnselect.BlkJSX.setChecked(jsx3.gui.CheckBox.CHECKED);
      }
      else {
        if (!jsx3.util.strEmpty(mediaDoc.selectSingleNode(".//record[.//Type='MediaClip']"))) {
          clipMediaRecordNode = mediaDoc.selectSingleNode(".//record[.//Type='MediaClip']");
          jsxMediaFileImg = "../../addins/ipvsCommon/images/icons/icon_reel.png";
        }
        else if(!jsx3.util.strEmpty(mediaDoc.selectSingleNode(".//record[.//Type='MediaImage']"))){
          clipMediaRecordNode = mediaDoc.selectSingleNode(".//record[.//Type='MediaImage']");
          jsxMediaFileImg = "../../addins/ipvsCommon/images/icons/icon_camera.png";
          streamType = "IMAGE";
        }

        if (jsx3.util.strEmpty(clipMediaRecordNode)) return;

        var clipMediaNID = clipMediaRecordNode.getAttribute("nid");
        var clipMSFResourceNode = mediaDoc.selectSingleNode("//MediaStreamFileResource[@NID='" + clipMediaNID + "']");
        if (jsx3.util.strEmpty(streamType)) streamType = clipMSFResourceNode.selectSingleNode(".//StreamTypeList/StreamType").getValue();
 
        var objClipRec = new Object;
        objClipRec.jsxid = orgFileName;
        objClipRec.jsxFileNID = clipMediaNID;
        var nidArr = clipMediaNID.split(".");
        var lastDot = nidArr[2];
        objClipRec.jsxtextMKDName = "..." + lastDot;
        objClipRec.jsxtextmediaName = clipMSFResourceNode.selectSingleNode(".//Info/Title").getValue();
        objClipRec.jsxtextmediaCreated = clipMSFResourceNode.selectSingleNode(".//Properties/DateCreated").getValue();
        objClipRec.jsxtextmediaLength = jsx3.util.strEmpty(clipMSFResourceNode.selectSingleNode(".//MediaClip/PlayLength")) ? "--" : clipMSFResourceNode.selectSingleNode(".//MediaClip/PlayLength").getValue();
        objClipRec.jsxtextmediaSize = fileSizeOnDisk; //Bug#47448
        objClipRec.jsxStreamType = streamType;
        objClipRec.jsxMediaFileImg = jsxMediaFileImg;
        objClipRec.jsxMediaName = orgFileName;
        objClipRec.jsxBlobSize = clipMSFResourceNode.selectSingleNode(".//Properties/Size").getValue();
        objClipRec.jsxBlobName = "";
        objClipRec.jsxGrpNID = grpMediaNID;
        objClipRec.jsxChkColumn = "1";

        cmpImportList.BlkJSX.insertRecord(objClipRec, grpMediaNID, false);
        cmpChkSelectUnselect.BlkJSX.setChecked(jsx3.gui.CheckBox.CHECKED);
      }
    } // end helper insertMKDMediaList

    //main code

    var files = folderDlgHandle.getFilesList();
    cmpImportList.BlkJSX.clearXmlData();

    //This loop will process .grp files. iTrack:VNG0001-1325
     
    for (var i = 0; i < files.length; i++) {
      var file = files[i];
      if (file.isHidden() || !file.exists()) continue;

      var bFile = file.isFile();
      //if( bFile && (!jsx3.util.strEmpty(file.getExtension()) && file.getExtension().toLowerCase() == "mkd") ) {
      if (bFile && (!jsx3.util.strEmpty(file.getExtension()))) {
        var strFileData = null;
        if (file.getExtension().toLowerCase() == "grp") {
          strFileData = processGRPFile(file, "metadata");
        }
        if (!jsx3.util.strEmpty(strFileData)) {
          insertMKDMediaList(strFileData, file.getName(), file.getStat().size); 
          bFilesAdded = true;
        }
      }
    }
     
  //This loop will process .mkd files.
      for (var i = 0; i < files.length; i++) {
      var file = files[i];
      if (file.isHidden() || !file.exists()) continue;

      var bFile = file.isFile();
      //if( bFile && (!jsx3.util.strEmpty(file.getExtension()) && file.getExtension().toLowerCase() == "mkd") ) {
      if (bFile && (!jsx3.util.strEmpty(file.getExtension()))) {
        var strFileData = null;
        if (file.getExtension().toLowerCase() == "mkd") {
          strFileData = processMKDFile(file, "metadata");
        }
        if (!jsx3.util.strEmpty(strFileData)) {
          insertMKDMediaList(strFileData, file.getName(), file.getStat().size); 
          bFilesAdded = true;
        }
      }
    }
    cmpImportList.BlkJSX.repaintData();
  } // end getImportList



  function getTwoDotXMediaList(strFileContent, fileName) {
    // helper function
    function insertTwoDotXMediaList(fileMetaDataNode, recJSXID, fileGroupID) {
      var metaDataArr = unSerializeTwoDotXMedia(fileMetaDataNode.getValue());
      metaDataArr = metaDataArr[0];
      if (!jsx3.util.strEmpty(metaDataArr)) {
        if (jsx3.util.strEmpty(fileGroupID)) {
          var streamType = "";
        }
        else {
          if (metaDataArr[11] == 1) {
            var streamType = "V2D";
          }
          else if (metaDataArr[11] == 2) {
            var streamType = "MPEGTS";
          }
          else if (metaDataArr[11] == 3) {
            var streamType = "RTP";
          }
          else if (metaDataArr[11] == 4) {
            var streamType = "UDP";
          }
          else {
            var streamType = "";
          }
        }

        var objRec = new Object;
        objRec.jsxid = recJSXID;
        objRec.jsxFileNID = recJSXID;
        objRec.jsxtextmediaName = metaDataArr[2];
        objRec.jsxtextmediaCreated = parseInt(metaDataArr[8] ? metaDataArr[8] : "0") * 1000;
        objRec.jsxtextmediaLength = parseInt(metaDataArr[4] ? metaDataArr[4] : "0") * 1000;
        objRec.jsxtextmediaSize = parseInt(metaDataArr[12] ? metaDataArr[12] : "0") * 1000;
        objRec.jsxStreamType = streamType;
        objRec.jsxIsTwodotXMedia = true;
        objRec.jsxChkColumn = "1";
        objRec.jsxMediaName = recJSXID + ".zip";
        objRec.jsxBlobSize = parseInt(metaDataArr[12] ? metaDataArr[12] : "0") * 1000;
        objRec.jsxBlobName = recJSXID;
        objRec.jsxMediaFileImg = "../../addins/ipvsCommon/images/icons/icon_reel.png";
        objRec.jsxtextMKDName = recJSXID;
        cmpImportList.BlkJSX.insertRecord(objRec, null, false);
        cmpChkSelectUnselect.BlkJSX.setChecked(jsx3.gui.CheckBox.CHECKED);
      }
    } // end helper insertTwoDotXMediaList

    var clsFSOFileSystem = new jsx3.io.FSOFileSystem();
    //Old 2.x exported media has node like <MEDIA ID=>. So replace ID to handle in xml
    //While exporting 2.x media IP was appended to filenames.Need to handle while adding to list
    var fileNameProfix = fileName.split("_")[0];
    strFileContent = strFileContent.replace(/ID=>/, "ID='0'>");
    //If there is & in string then new jsx3.xml.Document().loadXML is returning error.
    //So replacing with &amp;.
    strFileContent = strFileContent.replace(/&/g, "&amp;");
    var dbDoc = (new jsx3.xml.Document()).loadXML(strFileContent);
    if (!jsx3.util.strEmpty(dbDoc.getError()) && dbDoc.getError().code != "0") {
      return;
    }
    //There is no media group information in clips
    //look for Clips and if it is not empty then it is a group file
    var clipNode = dbDoc.selectSingleNode("//CLIPS");
    if (!jsx3.util.strEmpty(clipNode) && !jsx3.util.strEmpty(clipNode.getValue())) {
      var grpID = fileName;
      var grpFileMetadataNode = dbDoc.selectSingleNode("//METADATA");
      //insertTwoDotXMediaList(grpFileMetadataNode, fileName, "");

      var clipNodeIDArr = clipNode.getValue().split(",");
      for (var i = 0; i < clipNodeIDArr.length; i++) {
        var clipFileName = fileNameProfix + "_" + clipNodeIDArr[i];
        var clipFile = clsFSOFileSystem.getFile(folderDlgHandle.getFolderPath() + "\\db\\" + clipFileName);
        if (!clipFile.isFile() || clipFile.isHidden() || !clipFile.exists()) continue;
        var clipFileContent = clipFile.read();
        clipFileContent = clipFileContent.replace(/ID=>/, "ID='0'>");
        var clipDoc = (new jsx3.xml.Document()).loadXML(clipFileContent);
        var clipFileMetadataNode = clipDoc.selectSingleNode("//METADATA");
        insertTwoDotXMediaList(clipFileMetadataNode, clipFileName, grpID);
      }
    }

    delete clsFSOFileSystem;
  } // end getTwoDotXMediaList


} // end cmpBlkImportMedia_Create

