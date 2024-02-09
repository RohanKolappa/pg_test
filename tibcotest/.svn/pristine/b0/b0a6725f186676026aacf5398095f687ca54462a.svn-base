// this code has some potential issues 
// - w.r.t object reference counts - when objects created in here go out of scope etc
// - loading sync issues - dlg may not be loaded before the object init methods are called

function dlgEditMediaSFRMetaData_Create(MediaFileNID) {

  // deal with defaults for add
  if (MediaFileNID == null) return;

  closeDialogIfExists("DialogEditMetaData");
  // load view - dialog component file
  var DlgFile = "components/media/mediaEditMetadataDialog.xml";
  var BlkJSX = PNameSpace.getBodyBlock().load(DlgFile);
  
  // setup a local topic for dlg specific messaging
  var tDlg = "Dialog_" + jsx3.xml.CDF.getKey()
  topic_Create(tDlg);

  // setup a listner for it
  var eventDlg = new EventListner(tDlg);
  
  function dlgEditMediaSFRMetaData_Destroy() {
    // destroy any local eventlistners
    eventDlg.destroy();
    
    // destroy all local components
    cmpDlgEditMediaSFRMetaData.destroy();
    
    // destroy the local topic
    topic_Destroy(tDlg);
    
    // close dialog
    BlkJSX.doClose();
  }

  topic_Set(tDlg, "EditMediaFileDataState", "NONE");
  topic_Set(tDlg, "ShowError", "NONE");
  topic_Set(tDlg, "EditMediaFileSetMediaFileState", "DONE");

  var errorMsg = "Metadata cannot have the following characters: & < '";
  
  var cmpBtnEditMediaFileMetaDataOK = new CmpButton([BlkJSX, "btnEditMetadataDone"],
                                           null, 
                                           {action:"FNCCALL", 
                                            context:null, 
                                            fnc:function() {
                                              cmpDlgEditMediaSFRSetMediaSFRMetaData.call(this);
                                            }
                                           });

  var cmpBtnEditMediaFileMetaDataCancel = new CmpButton([BlkJSX, "btnEditMetadataCancel"],
                                               null, 
                                               {action:"FNCCALL", 
                                                context:null, 
                                                fnc:function() {
                                                  dlgEditMediaSFRMetaData_Destroy();
                                                }
                                               });

  var cmpBtnEditMediaFileMetaDataClose = new CmpButton([BlkJSX, "btnDialogCloseX"],
                                               null, 
                                               {action:"FNCCALL", 
                                                context:null, 
                                                fnc:function() {
                                                  dlgEditMediaSFRMetaData_Destroy();
                                                }
                                               });

  var cmpMetadataBlkJSX = getJsxByName([BlkJSX, "blkMetadataItemOrg"]);
  var cmpLblErrorMsgBlkJSX = getJsxByName([BlkJSX, "lblMetadataErrorMsg"]);
  cmpLblErrorMsgBlkJSX.setText(errorMsg, true);

  var cmpMetadataList = new Component([BlkJSX, "blkMetadataList"]);
  var cmpLblFileName = new Component([BlkJSX, "lblMetadataSource"]);
  var cmpBlkErrorMsg = new Component([BlkJSX, "blkErrorMsg"]);

  var cmpBlkError = new CmpContainer(BlkJSX,
                                    [{ cmp: cmpBlkErrorMsg, sel: "topic_Get('" + tDlg + "', 'ShowError') != 'NONE'"}],
                                    [{ action: "TOPICEVENT", topic: tDlg, eventvar: "ShowError", eventval: null}],
                                    null);

  var cmpDlgEditMediaSFRMetaData = new CmpContainer([BlkJSX, "DialogEditMetaData"],
                                                    [{ cmp: cmpBtnEditMediaFileMetaDataClose },
                                                      { cmp: cmpBtnEditMediaFileMetaDataCancel },
                                                      { cmp: cmpBtnEditMediaFileMetaDataOK },
                                                      { cmp: cmpMetadataList },
                                                      { cmp: cmpLblFileName },
                                                      { cmp: cmpBlkError }
                                                    ],
                                                    [{ action: "TOPICEVENT", topic: tDlg, eventvar: "EditMediaFileDataState", eventval: MediaFileNID }]);
                                                            
  cmpDlgEditMediaSFRMetaData.BlkJSX = BlkJSX;
  cmpDlgEditMediaSFRMetaData.MediaFileNID = MediaFileNID;
  cmpDlgEditMediaSFRMetaData.getBlkData =  cmpDlgEditMediaSFRGetMediaSFRMetaData;
  
  // init and repaint happens after the dialog is loaded
  jsx3.sleep( (function(){
                 this.init();
                 this.getBlkData(this.MediaFileNID);
                 }
               ),
               null, cmpDlgEditMediaSFRMetaData);

  eventDlg.subscribe("EditMediaFileSetMediaFileState", "DONE", this,
                      function () {
                        dlgEditMediaSFRMetaData_Destroy();
                      });


  function cmpDlgEditMediaSFRGetMediaSFRMetaData() {

    function processFileData(respData) {
      var metadataNode = respData.selectSingleNode("./MetaData");
      var fileName = respData.getAttribute("title");

      cmpLblFileName.BlkJSX.setText(fileName, true);
      var tagNodes = metadataNode.selectNodes("./Tag");
      for (var it = tagNodes.iterator(); it.hasNext(); ) {
        var metadataItemNode = it.next();
        var metadataName = metadataItemNode.getAttribute("Name");
        metadataName = jsx3.util.strTrim(metadataName);
        var metadataValue = metadataItemNode.getValue();

        var newMetadataBlkJSX = cmpMetadataBlkJSX.doClone(jsx3.app.Model.PERSISTEMBED);
        newMetadataBlkJSX.setName("blkMetadataOrg_" + metadataName);
        cmpMetadataList.BlkJSX.adoptChild(newMetadataBlkJSX, false);
        // show the block
        newMetadataBlkJSX.setDisplay(jsx3.gui.Block.DISPLAYBLOCK, true);

        var lblMetadataNameBlkJSX = getJsxByName([newMetadataBlkJSX, "lblMetadataName"]);
        lblMetadataNameBlkJSX.setText(metadataName, true);

        var txtMetadataValueBlkJSX = getJsxByName([newMetadataBlkJSX, "txtMetadataValue"]);
        //txtMetadataValueBlkJSX.setValue(unescape(metadataValue), true);
        txtMetadataValueBlkJSX.setValue(metadataValue, true);
      }
      cmpMetadataList.BlkJSX.repaint();
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
 
  } // end of MetaData Repaint

  function cmpDlgEditMediaSFRSetMediaSFRMetaDataValidate() {
    var objParent = cmpMetadataList.BlkJSX.getParent();
    // find all descendant of type text boxes
    var testStringExp = /^[^&<']{1,}$/;
    var retVal = true;
    var txtBoxArr = objParent.getDescendantsOfType(jsx3.gui.TextBox, false);
    var focusBlkJSX = null;
    for (var i = 0; i < txtBoxArr.length; i++) {
      var txtBoxBlkJSX = txtBoxArr[i];
      var txtBoxName = txtBoxBlkJSX.getName();
      if (txtBoxName != "txtMetadataValue") continue;
      var txtValue = txtBoxBlkJSX.getValue();
      if (jsx3.util.strEmpty(txtValue)) continue;
      var testresult = testStringExp.test(txtValue);
      if (!testresult) {
        //Before setting border, we need to set dynamic property for jsxborder since setBorder does not work without it.
        txtBoxBlkJSX.setBorder("solid 1px #ff0000;solid 1px #ff0000;solid 1px #ff0000;solid 1px #ff0000", true);
        //txtBoxBlkJSX.repaint();
        retVal = false;
        if (jsx3.util.strEmpty(focusBlkJSX)) {
          focusBlkJSX = txtBoxBlkJSX;
        }
      }
      else {
        //Before setting border, we need to set dynamic property for jsxborder since setBorder does not work without it.
        txtBoxBlkJSX.setBorder("", true);
      }
    }

    //show the error block
    if (!retVal) {
      topic_Publish(tDlg, "ShowError", "SHOW");
    }
    else {
      topic_Publish(tDlg, "ShowError", "NONE");
    }

    if (!jsx3.util.strEmpty(focusBlkJSX)) {
      focusBlkJSX.setSelection();
    }

    return retVal;                                             

  }

  //getting the commo sperated list for tag names whose value have some problem.
  // for each name go thru the list of blocks and show red border for the text box.
  function processError(errorMetadataTagNames) {
    var cmpBlkMetadataListChildren = cmpMetadataList.BlkJSX.getChildren().clone();
    var metadataListLength = cmpBlkMetadataListChildren.length;
    var focusBlkJSX = null;

    var tagNamesArr = errorMetadataTagNames.split(",");
    for (var errorNameNum = 0; errorNameNum < tagNamesArr.length; errorNameNum++) {
      var tagName = tagNamesArr[errorNameNum];
      tagName = jsx3.util.strTrim(tagName);
      for (var blkNum = 0; blkNum < metadataListLength; blkNum++) {
        var cmpBlkMetadataChild = cmpBlkMetadataListChildren[blkNum];
        var cmpBlkMetadataChildName = cmpBlkMetadataChild.getName();
        if (cmpBlkMetadataChildName.match(/blkMetadataOrg_/)) {
          var metadataName = cmpBlkMetadataChildName.split("blkMetadataOrg_")[1];
          if (tagName == metadataName) {
            var txtBoxArr = cmpBlkMetadataChild.getDescendantsOfType(jsx3.gui.TextBox, false);
            for (var txtNum = 0; txtNum < txtBoxArr.length; txtNum++) {
              var txtBoxBlkJSX = txtBoxArr[txtNum];
              var txtBoxName = txtBoxBlkJSX.getName();
              if (txtBoxName != "txtMetadataValue") continue;
              txtBoxBlkJSX.setBorder("solid 1px #ff0000;solid 1px #ff0000;solid 1px #ff0000;solid 1px #ff0000", true);
              if (jsx3.util.strEmpty(focusBlkJSX)) {
                focusBlkJSX = txtBoxBlkJSX;
              }
            }
            break;
          }
        }
      }
      topic_Publish(tDlg, "ShowError", "SHOW");
      if (!jsx3.util.strEmpty(focusBlkJSX)) {
        focusBlkJSX.setSelection();
      }
    }
  } // end processError

  function cmpDlgEditMediaSFRSetMediaSFRMetaData() {
    if (cmpDlgEditMediaSFRSetMediaSFRMetaDataValidate() != true) return;

    var metadataName = "";
    var metadataValue = "";

    var metadataNode = topic_Set(tDlg, "MetaData", null);
    metadataNode.removeChildren();

    var cmpBlkMetadataListChildren = cmpMetadataList.BlkJSX.getChildren().clone();
    var arrLength = cmpBlkMetadataListChildren.length;
    for (var blkNum = 0; blkNum < arrLength; blkNum++) {
      var metadataName = "";
      var metadataValue = "";
      var cmpBlkMetadataChild = cmpBlkMetadataListChildren[blkNum];
      var cmpBlkMetadataChildName = cmpBlkMetadataChild.getName();
      if (cmpBlkMetadataChildName.match(/blkMetadataOrg_/)) {
        metadataName = cmpBlkMetadataChildName.split("blkMetadataOrg_")[1];
        var txtBoxArr = cmpBlkMetadataChild.getDescendantsOfType(jsx3.gui.TextBox, false);
        for (var txtNum = 0; txtNum < txtBoxArr.length; txtNum++) {
          var txtBoxBlkJSX = txtBoxArr[txtNum];
          var txtBoxName = txtBoxBlkJSX.getName();
          if (txtBoxName != "txtMetadataValue") continue;
          metadataValue = txtBoxBlkJSX.getValue();
        }
        var tagNode = metadataNode.createNode(jsx3.xml.Entity.TYPEELEMENT, "Tag");
        tagNode.setAttribute("Name", metadataName);
        tagNode.setValue(metadataValue);
        metadataNode.appendChild(tagNode.cloneNode(true));
      }
    }

    var reqSetResourceObject = new ReqSessionGetRequest("SetFileMetaData",
                                                    { action: "FNCCALL", context: this,
                                                      fnc: function (respCode, respVal) {
                                                       // respCode = '5214';
                                                        //respVal = '<Notification><notificationheader><notificationtype>XMPPEvent</notificationtype></notificationheader><notificationdata><iq id="Request_jsx_r" to="srcuser1@localhost/media1" from="srcrelay1@localhost/srcrelay1" type="error"><query xmlns="com.ipvs.session" serviceVer="2.29.19"><SetFileMetaData whereNID="default.mediastreamfileresourcelist.msfrl_f7d606c8-ac9f-4b8f-9635-075839985e4d" whereTextSubStr="" deprecated="true"><MetaData><Tag Name="ID">1349223930</Tag><Tag Name="TestCase">AddMediaStreamFile</Tag><Tag Name="Hospital">12&amp;342</Tag><Tag Name="Lab"/><Tag Name="Physician"/><Tag Name="Patient"/><Tag Name="PatientID">&amp;1</Tag><Tag Name="PatientDOB"/><Tag Name="PatientSex"/><Tag Name="ProcedureType"/><Tag Name="Notes"/></MetaData></SetFileMetaData></query><error code="400" type="MODIFY"><bad-request xmlns="urn:ietf:params:xml:ns:xmpp-stanzas"></bad-request><x xmlns="com.ipvs.session"><ipvserror code="5214"><description>ID,TestCase,Hospital,Lab,Physician,Patient,PatientID,PatientDOB,PatientSex,ProcedureType,Notes</description></ipvserror></x></error></iq></notificationdata></Notification>';
                                                          
                                                        if (respCode == "SUCCESS" && respVal != null) {
                                                          var resNID = respVal.selectSingleNode(".//File/@NID").getValue();
                                                          if (resNID != null) {
                                                            topic_Publish(tDlg, "EditMediaFileSetMediaFileState", "DONE");
                                                          } else {
                                                            alert("Internal error: Response not as expected, no NID returned");
                                                            return false;
                                                          }
                                                        }
                                                        else if (respCode == "5214") {
                                                          if (respVal.match(/com\.ipvs\.session/)) {
                                                            var errorMetadataTagNames = (respVal.replace(/\n/g, ' ')).replace(/.*<description>(.*)<\/description>.*/, "$1");
                                                            processError(errorMetadataTagNames);
                                                          }
                                                          return false;
                                                        }
                                                        else {
                                                          try {
                                                            var codeErrorMsg = CommonErrorCodeArr[respCode].errorDesc;
                                                          } catch (e) {
                                                          }
                                                          var ErrorMsg = "Error: SetMetadata request failed \n";
                                                          ErrorMsg += "Error description: " + codeErrorMsg + "\n";
                                                          ErrorMsg += "Error code: " + respCode;
                                                          alert(ErrorMsg);
                                                        }
                                                      }
                                                    });

    reqSetResourceObject.ServiceName = "SessionService";
    var reqParams = new Object;
    reqParams["ObjectList_ByNID"] = MediaFileNID;

    var requestDataObjectArr = new Object;
    requestDataObjectArr["MetaData"] = metadataNode;
    reqSetResourceObject.requestDataObjectArr = requestDataObjectArr;

    reqSetResourceObject.reqParams = reqParams;
    reqSetResourceObject.sendRequest(); 

  } // end cmpDlgEditMediaSFRSetMediaSFRMetaData


}