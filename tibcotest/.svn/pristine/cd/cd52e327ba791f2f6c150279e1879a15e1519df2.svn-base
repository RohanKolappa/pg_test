// this code has some potential issues 
// - w.r.t object reference counts - when objects created in here go out of scope etc
// - loading sync issues - dlg may not be loaded before the object init methods are called

function dlgEditMediaSFRGroup_Create(MediaFileNID) {

  // deal with defaults for add
  if( MediaFileNID == null ) return;
  
  closeDialogIfExists("DialogMediaFileGroup");
  // load view - dialog component file
  var DlgFile = "components/media/mediaFileEditGroupDialog.xml";
  var BlkJSX = PNameSpace.getBodyBlock().load(DlgFile);
  
  // setup a local topic for dlg specific messaging
  var tDlg = "Dialog_" + jsx3.xml.CDF.getKey()
  topic_Create(tDlg);

  // setup a listner for it
  var eventDlg = new EventListner(tDlg);
  
  function dlgEditMediaSFRGroup_Destroy() {
    // destroy any local eventlistners
    eventDlg.destroy();
    
    // destroy all local components
    cmpDlgEditMediaSFRGroup.destroy();
    
    // destroy the local topic
    topic_Destroy(tDlg);
    
    // close dialog
    BlkJSX.doClose();
  }

 var cmpBtnEditMediaFileAddGroupTag = new CmpButton("btnMediaFileAddGroupTag",
                                      null,
                                      { action:"FNCCALL", 
                                        context:null,
                                        fnc:function() {
                                          var nameRecord = this.GroupTagNameBlkJSX.getValue();
                                          var selNameRecord = this.GroupTagNameBlkJSX.getRecord(nameRecord);
                                          var groupTagName = "";
                                          var groupTagValue = "";
                                          if( selNameRecord == null) {
                                            groupTagName = nameRecord
                                          } else {
                                            groupTagName = selNameRecord.jsxtext;
                                          }
                                          if( groupTagName == "" || groupTagName == null ) {
                                            alert("Please enter/select tag name");
                                            return false;                                          
                                          }
                                          
                                          var valueRecord = this.GroupTagValueBlkJSX.getValue();
                                          var selValueRecord = this.GroupTagValueBlkJSX.getRecord(valueRecord);
                                          if( selValueRecord == null) {
                                            groupTagValue = valueRecord;
                                          } else {
                                            groupTagValue = selValueRecord.jsxtext;
                                          }
                                          if( groupTagValue == "" || groupTagValue == null) {
                                            alert("Please enter/select tag value");
                                            return false;
                                          }
                                          
                                          var o = new Object;
                                          o.jsxid = groupTagName + groupTagValue;
                                          o.jsxGroupTagName = groupTagName;
                                          o.jsxGroupTagValue = groupTagValue;
                                          this.GroupTagListBlkJSX.insertRecord(o, null, false);
                                          this.GroupTagNameBlkJSX.setValue(null);
                                          this.GroupTagValueBlkJSX.setValue(null);
                                          this.GroupTagListBlkJSX.repaintData();

                                        }
                                      });
  cmpBtnEditMediaFileAddGroupTag.GroupTagNameBlkJSX = PNameSpace.getJSXByName("cmbMediaFileTagName");
  cmpBtnEditMediaFileAddGroupTag.GroupTagValueBlkJSX = PNameSpace.getJSXByName("cmbMediaFileTagValue");
  cmpBtnEditMediaFileAddGroupTag.GroupTagListBlkJSX = PNameSpace.getJSXByName("mtxMediaFileGroupTags");

  var cmpCmbSelMediaFileTagDefName = new CmpCustomPaginationList([BlkJSX, "cmbMediaFileTagName"],
                                                           [{action:"TOPICEVENT", topic: tDlg, eventvar:"MediaFileTagDefNameState", eventval:null}],
                                                             { persist: { col: "jsxtext", initval: false },
                                                                 action: "TOPICEVENT", topic: tDlg, eventvar: "MediaFileTagDefNameSelected", eventval: "jsxid"
                                                             },
                                                          null,
                                                          null,
                                                          "xsl/TagNameList.xsl"
                                                          );

  cmpCmbSelMediaFileTagDefName.cmpReqParamArr = { StartFrom: "ObjectList_StartFrom",
      CountToFetch: "ObjectList_CountToFetch",
      SortOrder: "ObjectList_SortOrder",
      ObjectCount: "ObjectCount",
      SortByXpath: "ObjectList_SortBy",
      WhereObjectType: "ObjectList_WhereObjectType"
  };

  topic_Set(tDlg, "MediaFileTagNameResponseSelectFormat", "NameValueTree");
  topic_Set(tDlg, "MediaFileTagNameResponseObjectType", "Media");

  cmpCmbSelMediaFileTagDefName.cmpReq = new cmpReq_GetSessionAPIPaginationList(tDlg, "GetTags",
                                                                        [{ VarName: "ObjectList_SelectFormat", VarRef: "MediaFileTagNameResponseSelectFormat"},
                                                                         { VarName: "ObjectList_WhereObjectType", VarRef: "MediaFileTagNameResponseObjectType"}],
                                                                        { action: "FNCCALL", context: cmpCmbSelMediaFileTagDefName,
                                                                            fnc: function(respCode, respVal) {
                                                                                if (respCode == "SUCCESS" && respVal != null) {
                                                                                    this.onResponse(respCode, respVal);
                                                                                }
                                                                                else {
                                                                                    log.info("GetTagsRequest failed. Error Code :" + respCode);
                                                                                    this.onResponseErrorHandle("GetTags",respCode, respVal);
                                                                                }
                                                                            }
                                                                        }
                                                                      );

  var cmpCmbSelMediaFileTagDefValue = new Component([BlkJSX, "cmbMediaFileTagValue"]);
  cmpCmbSelMediaFileTagDefValue.BlkJSX.setXMLTransformers("xsl/TagValueList.xsl");

  eventDlg.subscribe("MediaFileTagDefNameSelected", null, this,
                      function(eventVar, eventVal) {
                        if (eventVal != "NONE") {
                          cmpCmbSelMediaFileTagDefValue.BlkJSX.setValue("");
                          var nameRecord = cmpCmbSelMediaFileTagDefName.BlkJSX.getValue();
                          var selNameRecord = cmpCmbSelMediaFileTagDefName.BlkJSX.getRecordNode(nameRecord);
                          var tagNode = selNameRecord.selectSingleNode(".//Tag");
                          var strXMLValueCacheId = cmpCmbSelMediaFileTagDefValue.BlkJSX.getXMLId();
                          var objXML = PNameSpace.getCache().getDocument(strXMLValueCacheId);
                          objXML.loadXML(tagNode.toString());
                          cmpCmbSelMediaFileTagDefValue.BlkJSX.setSourceXML(objXML); 
                        }
                      }
                    );

  var cmpBtnEditMediaFileGroupOK = new CmpButton(["DialogMediaFileGroup","btnDoneMediaSFR"],
                                                  null, 
                                                  {action:"FNCCALL", 
                                                   context:null, 
                                                   fnc:function() {
                                                     eventDlg.subscribe("EditMediaFileSetMediaFileState", "DONE", this,
                                                                        function() {
                                                                          dlgEditMediaSFRGroup_Destroy();
                                                                        });
                                                     cmpDlgEditMediaSFRSetMediaSFRGroup.call(this, MediaFileNID);
                                                   }
                                                  });  
            
  var cmpBtnEditMediaFileGroupCancel = new CmpButton(["DialogMediaFileGroup","btnCancelMediaSFR"],
                                                      null, 
                                                      {action:"FNCCALL", 
                                                       context:null, 
                                                       fnc:function() {
                                                         dlgEditMediaSFRGroup_Destroy();
                                                       }
                                                      });

  var cmpBtnEditMediaFileGroupClose = new CmpButton(["DialogMediaFileGroup","btnDialogCloseX"],
                                                     null, 
                                                     {action:"FNCCALL", 
                                                      context:null, 
                                                      fnc:function() {
                                                        dlgEditMediaSFRGroup_Destroy();
                                                      }
                                                     });                                               
                                                            
  var cmpBlkEditMediaFileGroupMatrix = new CmpBlock("mtxMediaFileGroupTags");  
                                                                                         
  var cmpDlgEditMediaSFRGroup = new CmpContainer("DialogMediaFileGroup",
                                                  [ {cmp:cmpBlkEditMediaFileGroupMatrix},
                                                    {cmp:cmpBtnEditMediaFileGroupClose},
                                                    {cmp:cmpBtnEditMediaFileGroupCancel},
                                                    {cmp:cmpBtnEditMediaFileGroupOK},
                                                    {cmp:cmpBtnEditMediaFileAddGroupTag},
                                                    {cmp:cmpCmbSelMediaFileTagDefName},
                                                    {cmp:cmpCmbSelMediaFileTagDefValue}
                                                  ],
                                                  [{action:"TOPICEVENT", topic:tDlg, eventvar:"EditMediaFileDataState", eventval:MediaFileNID}]);
                                                            
  cmpDlgEditMediaSFRGroup.BlkJSX = BlkJSX;
  cmpDlgEditMediaSFRGroup.ObjectNID = MediaFileNID;
  cmpDlgEditMediaSFRGroup.getBlkData =  cmpDlgEditMediaSFRGetMediaSFRGroup;
  
  // init and repaint happens after the dialog is loaded
  jsx3.sleep( (function(){
                 this.init();
                 this.getBlkData(this.ObjectNID);
                 topic_Publish(tDlg, "MediaFileTagDefNameState", (newServerDate()).getTime());
                 }
               ),
               null, cmpDlgEditMediaSFRGroup );
               
  function cmpDlgEditMediaSFRGetMediaSFRGroup(MediaFileNID) {
    PNameSpace.getJSXByName("mtxMediaFileGroupTags").resetCacheData();
    // helper function
    function processFileData(respData) {

      var mediaGroupNode = respData.selectSingleNode("./Groups");
      if (mediaGroupNode == null) return;
      var GroupIter = mediaGroupNode.selectNodeIterator(".//Tag");
      while (GroupIter.hasNext()) {
        var tagNode = GroupIter.next();
        var TagName = tagNode.getAttribute("Name");
        var TagValue = tagNode.getAttribute("Value");
        if (!jsx3.util.strEmpty(TagName) && !jsx3.util.strEmpty(TagValue)) {
          var o = new Object;
          o.jsxid = TagName + TagValue;
          o.jsxGroupTagName = TagName;
          o.jsxGroupTagValue = TagValue;
          PNameSpace.getJSXByName("mtxMediaFileGroupTags").insertRecord(o, null, false);
        }
      }
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
  
  } // end of Groups Repaint

  function cmpDlgEditMediaSFRSetMediaSFRGroup(MediaFileNID) {
    var mediaGroupNodeStr = "<Groups>Groups</Groups>";
    var mediaGroupNode = new jsx3.xml.Document().loadXML(mediaGroupNodeStr);    
    mediaGroupNode.setValue("");
    var objMatrix = PNameSpace.getJSXByName("mtxMediaFileGroupTags");
    if (objMatrix) {
      var strXMLCacheId = objMatrix.getXMLId();
      var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
      if (objXML) {
        var objRecordNodes = objXML.selectNodes("//record");
        for (var it = objRecordNodes.iterator();it.hasNext();){
          var objRecordNode = it.next();
          var TagName = objRecordNode.getAttribute("jsxGroupTagName");
          var TagValue = objRecordNode.getAttribute("jsxGroupTagValue");
          if( TagName != "" && TagValue != "") {
            var tagNode = mediaGroupNode.createNode(jsx3.xml.Entity.TYPEELEMENT, "Tag");
            tagNode.setAttribute("Name", TagName);
            tagNode.setAttribute("Value", TagValue);
            mediaGroupNode.appendChild(tagNode);
          }
        }
      }
    }

    var reqSendMediaSFRInfo = new ReqSessionGetRequest("SetFileGroupInfo",
                                                            {
                                                                action: "FNCCALL", context: null,
                                                                fnc: function (respCode, respVal) {
                                                                    if (respCode == "SUCCESS" && respVal != null) {
                                                                      dlgEditMediaSFRGroup_Destroy();
                                                                    }
                                                                    else {
                                                                        try {
                                                                            var codeErrorMsg = CommonErrorCodeArr[respCode].errorDesc;
                                                                        } catch (e) {
                                                                        }
                                                                        var ErrorMsg = "Error: SetFileGroupInfo request failed \n";
                                                                        ErrorMsg += "Error description: " + codeErrorMsg + "\n";
                                                                        ErrorMsg += "Error code: " + respCode;
                                                                        alert(ErrorMsg);
                                                                    }
                                                                }
                                                            }
                                                           );
    reqSendMediaSFRInfo.ServiceName = "SessionService";
    var reqParams = new Object;
    reqParams["ObjectList_ByNID"] = MediaFileNID;
    reqSendMediaSFRInfo.reqParams = reqParams;
    var requestDataObjectArr = new Object;
    requestDataObjectArr["Groups"] = mediaGroupNode;
    reqSendMediaSFRInfo.requestDataObjectArr = requestDataObjectArr;
      // send the request
    reqSendMediaSFRInfo.sendRequest();
  }
}