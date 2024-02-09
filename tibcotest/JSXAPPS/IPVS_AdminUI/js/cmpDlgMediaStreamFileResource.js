// this code has some potential issues 
// - w.r.t object reference counts - when objects created in here go out of scope etc
// - loading sync issues - dlg may not be loaded before the object init methods are called

function dlgEditMediaSFR_Create(MediaFileNID) {

  // deal with defaults for add
  if( MediaFileNID == null ) MediaFileNID = "DEFAULT_MEDIASTREAMFILE_NID";
  
  // load view - dialog component file
  var DlgFile = "components/media/mediaAddFileDialog.xml";
  var BlkJSX = PNameSpace.getBodyBlock().load(DlgFile);
  
  // setup a local topic for dlg specific messaging
  var tDlg = "Dialog_" + jsx3.xml.CDF.getKey()
  topic_Create(tDlg);

  // setup a listner for it
  var eventDlg = new EventListner(tDlg);
  
  function dlgEditMediaSFR_Destroy() {
    //Cleanout bookmarks from Cache
    trimBookmarkCacheNode(MediaFileNID);

    // destroy any local eventlistners
    eventDlg.destroy();
    
    // destroy all local components
    cmpDlgEditMediaSFR.destroy();
    
    // destroy the local topic
    topic_Destroy(tDlg);
    
    // close dialog
    BlkJSX.doClose();
  }

  var cmpBtnEditMediaFileAddMetaDataTag = new CmpButton("btnMediaFileAddMetaDataTag",
                                      null,
                                      { action:"FNCCALL", 
                                        context:null,
                                        fnc:function() {
                                          var tagName = checkValue(this.GroupTagNameBlkJSX.getValue() , "txtMediaFileMetaDataTagName" , "Please enter tag name");
                                          if(tagName)
                                            var tagValue = checkValue(this.GroupTagValueBlkJSX.getValue() , "txtMediaFileMetaDataTagName" , "Please enter tag value");
                                          if(tagValue) {
                                            var o = new Object;
                                            o.jsxid = jsx3.xml.CDF.getKey();
                                            o.jsxGroupTagName = this.GroupTagNameBlkJSX.getValue();
                                            o.jsxGroupTagValue = this.GroupTagValueBlkJSX.getValue();
                                            this.GroupTagListBlkJSX.insertRecord(o, null, false);
                                            this.GroupTagNameBlkJSX.setValue(null);
                                            this.GroupTagValueBlkJSX.setValue(null);
                                            this.GroupTagListBlkJSX.repaintData();
                                          }
                                        }
                                      });
  cmpBtnEditMediaFileAddMetaDataTag.GroupTagNameBlkJSX = PNameSpace.getJSXByName("txtMediaFileMetaDataTagName");
  cmpBtnEditMediaFileAddMetaDataTag.GroupTagValueBlkJSX = PNameSpace.getJSXByName("txtMediaFileMetaDataTagValue");
  cmpBtnEditMediaFileAddMetaDataTag.GroupTagListBlkJSX = PNameSpace.getJSXByName("mtxMediaFileMetaDataTags");
  
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
                                          o.jsxid = jsx3.xml.CDF.getKey();
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

  var cmpCmbSelMediaFileTagDefValue = new CmpNIDElementList("cmbMediaFileTagValue",
                                        "MediaFileTagDefNameSelected",
                                        ".//TagValueList/TagValue",
                                        {
                                         jsxtext:{objxpath:"\".\""}
                                        },
                                        [{action:"REQEVENT", eventvar:"GroupTagDataState", eventval:null},
                                         {action:"APPEVENT", eventvar:"MediaFileTagDefNameSelected", eventval:null}],
                                        {persist:{col:"jsxtext",initval:false},
                                         action:"APPEVENT", topic:tDlg, eventvar:"MediaFileTagDefValueSelected", eventval:"jsxtext"
                                        }
                                       );

  var cmpCmbSelMediaFileTagDefName = new CmpNIDList( "cmbMediaFileTagName",
                                           "GroupTagNIDList",
                                           null, 
                                           null,
                                           {jsxid:{objxpath:"\"./@NID\""},
                                            jsxtext:{objxpath:"\".//TagName\""}},
                                           [{action:"REQEVENT", eventvar:"GroupTagDataState", eventval:null}],
                                           {persist:{col:"jsxtext",initval:false},
                                            action:"APPEVENT", eventvar:"MediaFileTagDefNameSelected", eventval:"jsxid"}
                                           );
  
  var cmpBtnEditMediaFileOK = new CmpButton(["DialogMediaFile","btnDoneMediaSFR"],
                                           null, 
                                           {action:"FNCCALL", 
                                            context:null, 
                                            fnc:function() {
                                              eventDlg.subscribe("EditMediaFileSetMediaFileState", "DONE", this,
                                                                function() {
                                                                  dlgEditMediaSFR_Destroy();
                                                                });
                                              cmpDlgEditMediaSFRSetMediaSFR.call(this, MediaFileNID);
                                            }
                                           });
            
  var cmpBtnEditMediaFileCancel = new CmpButton(["DialogMediaFile","btnCancelMediaSFR"],
                                               null, 
                                               {action:"FNCCALL", 
                                                context:null, 
                                                fnc:function() {
                                                  dlgEditMediaSFR_Destroy();
                                                }
                                               });

  var cmpBtnEditMediaFileClose = new CmpButton(["DialogMediaFile","btnCloseMediaSFR"],
                                               null, 
                                               {action:"FNCCALL", 
                                                context:null, 
                                                fnc:function() {
                                                  dlgEditMediaSFR_Destroy();
                                                }
                                               });                                               
                                                                                         
  var cmpDlgEditMediaSFR = new CmpContainer("DialogMediaFile",
                                [ {cmp:cmpBtnEditMediaFileClose},
                                  {cmp:cmpBtnEditMediaFileCancel},
                                  {cmp:cmpBtnEditMediaFileOK},
                                  {cmp:cmpBtnEditMediaFileAddGroupTag},
                                  {cmp:cmpCmbSelMediaFileTagDefName},
                                  {cmp:cmpCmbSelMediaFileTagDefValue},
                                  {cmp:cmpBtnEditMediaFileAddMetaDataTag}
                                ],
                                [{action:"TOPICEVENT", topic:tDlg, eventvar:"EditMediaFileDataState", eventval:MediaFileNID}]);
                                                            
  cmpDlgEditMediaSFR.BlkJSX = BlkJSX;
  cmpDlgEditMediaSFR.ObjectNID = MediaFileNID;
  cmpDlgEditMediaSFR.getBlkData =  cmpDlgEditMediaSFRGetMediaSFR;
  
  // init and repaint happens after the dialog is loaded
  jsx3.sleep( (function(){
                 this.init();
                 this.getBlkData(this.ObjectNID);
                 }
               ),
               null, cmpDlgEditMediaSFR ); 

  // fnc to get dlg data
  function cmpDlgEditMediaSFRGetMediaSFR(MediaFileNID) { 
    
    // fnc to process dlg data
    function cmpDlgEditMediaSFRProcessMediaSFR(MediaFileNID) { 
      var selectedMediaDirName = "";
      
      var mediafileNode = topic_GetNode(tCache, "//*[@NID='" + MediaFileNID + "']");      
      if( mediafileNode == null ) {
        alert("Internal error: Not able to find media file");
        dlgEditMediaSFR_Destroy();
      } else {
        var mediaStorePortResourceNID = "";
        // get the MediastoreportResource NID of the selected dir
        var dirNID = topic_Get(tApp, "MediaSDRSelected");
        var matchResult = dirNID.search(/jsx/);
        if(matchResult != -1) {
          mediaStorePortResourceNID = "DEFAULT";
        } else {
          var selectedMediaDirNode = topic_GetNode(tCache, "//*[@NID='" + dirNID + "']");
          selectedMediaDirName = selectedMediaDirNode.selectSingleNode(".//Info/Title").getValue();                                                 
          mediaStorePortResourceNID = selectedMediaDirNode.selectSingleNode(".//MediaStorePortResourceNID").getValue();
        }
              
        // function to get the devicelist and extract MediastoreportResource NID and 
        // display Device name along with the NID. NID will be mapped
        //defined in cmpDlgCommonUtils.js
        cmpMediaSelMediaStoreNID_Repaint("selMediaFileMediaStoreNID", mediaStorePortResourceNID);
        
        cmpDlgEditMediaSFR.setMapRuleData("MediaStreamFileResource", "IN", "NID", MediaFileNID);
        cmpDlgEditMediaSFRMetaData_Repaint(MediaFileNID);
        cmpDlgEditMediaSFRBookMarks_Repaint(MediaFileNID);
       
        topic_Publish(tDlg, "EditMediaFileDataState", MediaFileNID); 
        cmpSelect_Select1Value("selMediaFileMediaStoreNID");
        
        PNameSpace.getJSXByName("lblMediaFileParentDirName").setText(selectedMediaDirName, true);  
             
      }
    } // end cmpDlgEditMediaSFRProcessMediaSFR

    //to get bookmarks
    function GetBookmarkList(MediaStreamFileResourceNID) {
      var reqGetBookmarkList = new ReqGetObjectList("Bookmark", 
                                  {action:"FNCCALL", context:this, 
                                   fnc:function(respCode, respVal){
                                         if( respCode == "SUCCESS" && respVal != null) {
                                           cmpDlgEditMediaSFRProcessMediaSFR(MediaStreamFileResourceNID);
                                         }
                                         else {
                                           alert("Get Bookmark list request failed");
                                         }
                                         return true;
                                       }
                                });
      reqGetBookmarkList.ServiceName = "mediaroomservice";
      var reqParams = new Object;
      reqParams["MediaStreamFileResourceNID"] = MediaStreamFileResourceNID;
      reqParams["WhereXPath"] = "";
      reqParams["StartFrom"] = 0;
      reqParams["CountToFetch"] = "-1";
      reqGetBookmarkList.reqParams = reqParams;
      reqGetBookmarkList.sendRequest(this);
    }
  
    if( MediaFileNID != "DEFAULT_MEDIASTREAMFILE_NID") {
      var reqMediaFileData = new ReqGetObjectNID(MediaFileNID, "MediaStreamFileResource",
                                                  {action:"FNCCALL", context:this, 
                                                    fnc:function(respCode, respVal) {
                                                          if(respCode == "SUCCESS" && respVal != null) {
                                                            GetBookmarkList(MediaFileNID);
                                                          } else {
                                                             var codeErrorMsg = CommonErrorCodeArr[respCode].errorDesc;
                                                             var ErrorMsg = "Error: Media file get data request failed \n";
                                                             ErrorMsg += "Error description: " + codeErrorMsg + "\n";
                                                             ErrorMsg += "Error code: " + respCode;
                                                             alert(ErrorMsg);
                                                          }                                                           
                                                        }
                                                  } 
                                                );
      reqMediaFileData.sendRequest();
    } else {
      cmpDlgEditMediaSFRProcessMediaSFR(MediaFileNID);
    }
    
    // disable device name select box (MediaStorePortResourceNID)
    var cmpMediaSelMediaStoreNIDBlkJSX = PNameSpace.getJSXByName("selMediaFileMediaStoreNID");
    cmpMediaSelMediaStoreNIDBlkJSX.setEnabled(jsx3.gui.Form.STATEDISABLED, true);    
    
  } // end cmpDlgEditMediaSFRGetMediaSFR

  function cmpDlgEditMediaSFRBookMarks_Repaint(MediaStreamFileResourceNID) {
    PNameSpace.getJSXByName("mtxMediaFileBookmarks").resetCacheData();
    var mediaBookmarkListNode = topic_GetNode(tCache, "//BookmarkList");
    if( mediaBookmarkListNode == null ) return;
    // for each bookmark
    var nodeBookmarkIter = mediaBookmarkListNode.selectNodeIterator("./*[.//MediaStreamFileResourceNID='" + MediaStreamFileResourceNID + "']");
    while( nodeBookmarkIter.hasNext() ) {
      var nodeBookmark = nodeBookmarkIter.next();
      var o = new Object;
      o.jsxid = nodeBookmark.getAttribute("NID");
      o.jsxMediaFileBmkID = nodeBookmark.getAttribute("NID");
      o.jsxMediaFileBmkTitle = nodeBookmark.selectSingleNode(".//Title").getValue();
      o.jsxMediaFileBmkComment = nodeBookmark.selectSingleNode(".//Comment").getValue();
      o.jsxMediaFileBmkDateCreated = getDateTimeFromTimeCode(nodeBookmark.selectSingleNode(".//DateCreated").getValue());
      o.jsxMediaFileBmkDateModified = nodeBookmark.selectSingleNode(".//DateModified").getValue();
      o.jsxMediaFileBmkTime = nodeBookmark.selectSingleNode(".//AtTC").getValue();
      o.jsxMediaFileBmkAuthUser = nodeBookmark.selectSingleNode(".//AuthorUserID").getValue();
      PNameSpace.getJSXByName("mtxMediaFileBookmarks").insertRecord(o, null, false);
    }
  }
  
  function cmpDlgEditMediaSFRMetaData_Repaint(MediaFileNID) {
    PNameSpace.getJSXByName("mtxMediaFileMetaDataTags").resetCacheData();
    var mediaNode = topic_GetNode(tCache, "//*[@NID='" + MediaFileNID + "']");
    if(mediaNode == null) return;
    var mediaMetaDataNode = mediaNode.selectSingleNode(".//MediaStoreFileInfo/MetaData");
    if( mediaMetaDataNode == null ) return;
    var metaDataIter = mediaMetaDataNode.selectNodeIterator(".//Tag");
    while( metaDataIter.hasNext() ) {
      var tagNode = metaDataIter.next();
      var TagName = tagNode.getAttribute("Name");
      var TagValue = tagNode.getValue();
      var o = new Object;
      o.jsxid = jsx3.xml.CDF.getKey();
      o.jsxGroupTagName = TagName;
      o.jsxGroupTagValue = TagValue;
      PNameSpace.getJSXByName("mtxMediaFileMetaDataTags").insertRecord(o, null, false);
    }  
  } // end cmpDlgEditMediaSFRMetaData_Repaint

  // fnc to set dlg data - runs in the context of the OK button
  function cmpDlgEditMediaSFRSetMediaSFR(MediaFileNID) {
    // validate input 
    if( cmpDlgEditMediaSFRValidate() != true ) return;
    // decide add or update
    var SetMsg = "Edit Media file";
    var SetMode = "Replace";
    if( MediaFileNID == "DEFAULT_MEDIASTREAMFILE_NID" ) {
      SetMode = "Add";
    }
    // setup mediafile map rule info
    var mediaFileMapRule = new Object;
    mediaFileMapRule.state = false;
    mediaFileMapRule.objtype = "MediaStreamFileResource";
    mediaFileMapRule.rule = "MediaStreamFileResource";
    var AddEditNode = null;    
    // get mediafile node info from UI
    var mediaFileNode = utilRuleGetOutboundDoc.call(this, mediaFileMapRule.rule);
    var orgMediaNode = topic_GetNode(tCache, ".//*[@NID='" + MediaFileNID + "']");
    if( mediaFileNode == null ) return;
    //get MetadataTag values from the matrix
    var mediaMetaDataNode = mediaFileNode.selectSingleNode(".//MediaStoreFileInfo/MetaData");
    if( mediaMetaDataNode == null ) return;
    var mediaMetaDataNodeEmpty = mediaMetaDataNode.selectSingleNode(".//Tag[@Name='']");
    if( mediaMetaDataNodeEmpty == null ) return;
    mediaMetaDataNode.removeChild(mediaMetaDataNodeEmpty);
    var objMatrix = PNameSpace.getJSXByName("mtxMediaFileMetaDataTags");
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
            var tagNode = mediaMetaDataNode.createNode(jsx3.xml.Entity.TYPEELEMENT, "Tag");
            tagNode.setAttribute("Name", TagName);
            tagNode.setValue(TagValue);
            mediaMetaDataNode.appendChild(tagNode);
          }
        }
      }
    }
    if( SetMode == "Replace" ) {
      var title = mediaFileNode.selectSingleNode(".//Info/Title").getValue();
      var description = mediaFileNode.selectSingleNode(".//Info/Description").getValue();
      var mediaStorePortResourceNID = mediaFileNode.selectSingleNode(".//MediaStorePortResourceNID").getValue();
      var dateModified = mediaFileNode.selectSingleNode(".//Info/Properties/DateModified").getValue();
      var groupNode = mediaFileNode.selectSingleNode(".//Info/Groups");

      orgMediaNode.selectSingleNode(".//Info/Title").setValue(title);
      orgMediaNode.selectSingleNode(".//Info/Description").setValue(description);
      orgMediaNode.selectSingleNode(".//MediaStorePortResourceNID").setValue(mediaStorePortResourceNID);
      orgMediaNode.selectSingleNode(".//Info/Properties/DateModified").setValue(dateModified);
      
      var orgGroupNode = orgMediaNode.selectSingleNode(".//Info/Groups");
      var orgMetaData = orgMediaNode.selectSingleNode(".//MediaStoreFileInfo/MetaData");
     
      orgGroupNode.getParent().replaceNode(groupNode, orgGroupNode);
      orgMetaData.getParent().replaceNode(mediaMetaDataNode, orgMetaData);
      //update bookmarks in db
      var mediaBookmarkListNode = topic_GetNode(tCache, "//BookmarkList");
      if( mediaBookmarkListNode != null ) {
        var objBookMarksMatrix = PNameSpace.getJSXByName("mtxMediaFileBookmarks");
        if( objBookMarksMatrix ) {
          var strXMLCacheId = objBookMarksMatrix.getXMLId();
          var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
          if( objXML ) {
            var nodeBookmarkIter = mediaBookmarkListNode.selectNodeIterator("./*[.//MediaStreamFileResourceNID='" + MediaFileNID + "']");
            while( nodeBookmarkIter.hasNext() ) {
              var nodeBookmark = nodeBookmarkIter.next();
              var bookMarkNID = nodeBookmark.getAttribute("NID");
              var objRecordNode = objXML.selectSingleNode("//record[@jsxMediaFileBmkID= '" + bookMarkNID + "']");
              if( objRecordNode == null ) {
                cmpDlgDeleteBookmark(bookMarkNID);
              }
            }
          }
        }
      }
      AddEditNode = orgMediaNode;     
     
    } else {
      SetMsg = "Add Media file";
      //set ParentDir if view tab selected is " view by dir"
      if( activeIndex == 0) {
        var dirNID = topic_Get(tApp, "MediaSDRSelected");
        var matchResult = dirNID.search(/jsx/);

        if(matchResult != -1) {
        } else {        
          mediaFileNode.selectSingleNode(".//Info/Properties/ParentMediaDirNID").setValue(dirNID);
        }
      }
      var oldMetaDataNode = mediaFileNode.selectSingleNode(".//MediaStoreFileInfo/MetaData");
      mediaFileNode.replaceNode(mediaMetaDataNode,oldMetaDataNode); 
      AddEditNode = mediaFileNode;
    }

    // do set request for mediafile
    var reqSetMediaFileObject = new ReqSetObject(SetMode, mediaFileMapRule.objtype, AddEditNode, 
                                                  {action:"FNCCALL", context:this, 
                                                     fnc:function(respCode, respVal) {
                                                           if(respCode == "SUCCESS" && respVal != null) {
                                                             // extract NID
                                                             var responseNIDIter = respVal.selectNodeIterator(".//ObjectNID");
                                                             var objNID = null;
                                                             while( responseNIDIter.hasNext() ) {
                                                               var objNIDNode = responseNIDIter.next().cloneNode(true);
                                                               objNID = objNIDNode.getValue();
                                                             }
                                                             // publishing event to that mediafile list natrix will be updated
                                                             // events subscribed in the cmpBlkMedia.js
                                                             if (SetMode == "Add") {
                                                                topic_Publish(tApp, "MediaSFRDataAddNode", objNID);
                                                             } else if (SetMode == "Replace") {
                                                                topic_Publish(tApp, "MediaSFRDataUpdateNode", objNID);
                                                             }
                                                             //trim the mediafile function defined in commoncachefnc.js
                                                             //trimMediaFileCacheNode(objNID);
                                                             // close dialog
                                                             dlgEditMediaSFR_Destroy();
                                                           } else {
                                                             var codeErrorMsg = CommonErrorCodeArr[respCode].errorDesc;
                                                             var ErrorMsg = "Error: "+ SetMsg +" request failed \n";
                                                             ErrorMsg += "Error description: " + codeErrorMsg + "\n";
                                                             ErrorMsg += "Error code: " + respCode;
                                                             alert(ErrorMsg);
                                                           }
                                                         }
                                                  }                                                    
                                                );
    reqSetMediaFileObject.sendRequest();
  } // end cmpDlgEditMediaSFRSetMediaSFR

  
  function cmpDlgDeleteBookmark(bookMarkNID) {
    var reqDeleteBookmark = new ReqDeleteObjectNID(bookMarkNID, "Bookmark",
                                      {action:"FNCCALL", context:this,
                                       fnc:function(respCode, respVal){
                                            if( respCode == "SUCCESS" && respVal != null) {
                                              //TBD
                                            }
                                            else {
                                              alert("Delete Bookmark request failed");  //TBR
                                              //need to get Error codes and description. waiting for list from middleware person
                                            }
                                            return true;
                                          }
                                      });
    reqDeleteBookmark.ServiceName = "assetadminservice";
    reqDeleteBookmark.sendRequest();
  }
  
  function cmpDlgEditMediaSFRValidate() {
    var valFullName = PNameSpace.getJSXByName("txtMediaFileName").getValue();

    var fullName = checkValue(valFullName , "txtMediaFileName" , "Please enter file name");
    if(fullName)
      return true;
      
    return false;
  } // end cmpDlgEditMediaSFRValidate
}