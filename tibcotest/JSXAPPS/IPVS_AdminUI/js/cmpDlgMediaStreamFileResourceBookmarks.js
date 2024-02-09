// this code has some potential issues 
// - w.r.t object reference counts - when objects created in here go out of scope etc
// - loading sync issues - dlg may not be loaded before the object init methods are called

function dlgEditMediaSFRBookmarks_Create(MediaFileNID) {

  // deal with defaults for add
  if( MediaFileNID == null ) return;
  
  closeDialogIfExists("DialogMediaFileBookmarks");
  // load view - dialog component file
  var DlgFile = "components/media/mediaFileEditBookmarksDialog.xml";
  var BlkJSX = PNameSpace.getBodyBlock().load(DlgFile);
  
  // setup a local topic for dlg specific messaging
  var tDlg = "Dialog_" + jsx3.xml.CDF.getKey()
  topic_Create(tDlg);

  // setup a listner for it
  var eventDlg = new EventListner(tDlg);
  
  function dlgEditMediaSFRBookmarks_Destroy() {
    // destroy any local eventlistners
    eventDlg.destroy();
    
    // destroy all local components
    cmpDlgEditMediaSFRBookmarks.destroy();
    
    // destroy the local topic
    topic_Destroy(tDlg);
    
    // close dialog
    BlkJSX.doClose();
  }

  var cmpBtnEditMediaFileBookmarksOK = new CmpButton(["DialogMediaFileBookmarks","btnDoneMediaSFR"],
                                           null, 
                                           {action:"FNCCALL", 
                                            context:null, 
                                            fnc:function() {
                                              eventDlg.subscribe("EditMediaFileSetMediaFileState", "DONE", this,
                                                                function() {
                                                                  dlgEditMediaSFRBookmarks_Destroy();
                                                                });
                                              cmpDlgEditMediaSFRSetMediaSFRBookmarks.call(this, MediaFileNID);
                                            }
                                           });
            
  var cmpBtnEditMediaFileBookmarksCancel = new CmpButton(["DialogMediaFileBookmarks","btnCancelMediaSFR"],
                                               null, 
                                               {action:"FNCCALL", 
                                                context:null, 
                                                fnc:function() {
                                                  dlgEditMediaSFRBookmarks_Destroy();
                                                }
                                               });

  var cmpBtnEditMediaFileBookmarksClose = new CmpButton(["DialogMediaFileBookmarks","btnDialogCloseX"],
                                               null, 
                                               {action:"FNCCALL", 
                                                context:null, 
                                                fnc:function() {
                                                  dlgEditMediaSFRBookmarks_Destroy();
                                                }
                                               });                                               
                                                                                         
  var cmpDlgEditMediaSFRBookmarks = new CmpContainer("DialogMediaFileBookmarks",
                                [ {cmp:cmpBtnEditMediaFileBookmarksClose},
                                  {cmp:cmpBtnEditMediaFileBookmarksCancel},
                                  {cmp:cmpBtnEditMediaFileBookmarksOK}
                                ],
                                [{action:"TOPICEVENT", topic:tDlg, eventvar:"EditMediaFileDataState", eventval:MediaFileNID}]);
                                                            
  cmpDlgEditMediaSFRBookmarks.BlkJSX = BlkJSX;
  cmpDlgEditMediaSFRBookmarks.ObjectNID = MediaFileNID;
  cmpDlgEditMediaSFRBookmarks.getBlkData =  cmpDlgEditMediaSFRGetMediaSFRBookmarks;
  
  // init and repaint happens after the dialog is loaded
  jsx3.sleep( (function(){
                 this.init();
                 this.getBlkData(this.ObjectNID);
                 }
               ),
               null, cmpDlgEditMediaSFRBookmarks );
               
  function cmpDlgEditMediaSFRGetMediaSFRBookmarks(MediaFileNID) {
    var reqGetBookmarkList = new ReqGetObjectList("Bookmark", 
                                  {action:"FNCCALL", context:this, 
                                   fnc:function(respCode, respVal){
                                         if( respCode == "SUCCESS" && respVal != null) {
                                           cmpDlgEditMediaSFRBookMarks_Repaint(MediaFileNID);
                                         }
                                         else {
                                           alert("Get Bookmark list request failed");
                                         }
                                         return true;
                                       }
                                });
    reqGetBookmarkList.ServiceName = "mediaroomservice";
    var reqParams = new Object;
    reqParams["MediaStreamFileResourceNID"] = MediaFileNID;
    reqParams["WhereXPath"] = "";
    reqParams["StartFrom"] = 0;
    reqParams["CountToFetch"] = "-1";
    reqGetBookmarkList.reqParams = reqParams;
    reqGetBookmarkList.sendRequest(this);
  } 

  function cmpDlgEditMediaSFRBookMarks_Repaint(MediaFileNID) {
    var objMtx = PNameSpace.getJSXByName("mtxMediaFileBookmarks");
    objMtx.resetCacheData();
    var mediaBookmarkListNode = topic_GetNode(tCache, "//BookmarkList");
    if( mediaBookmarkListNode == null ) return;
    // for each bookmark
    var nodeBookmarkIter = mediaBookmarkListNode.selectNodeIterator("./*[.//MediaStreamFileResourceNID='" + MediaFileNID + "']");
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
      objMtx.insertRecord(o, null, false);
    }
    objMtx.repaint();
  } // end cmpDlgEditMediaSFRBookmarks_Repaint

  function cmpDlgEditMediaSFRSetMediaSFRBookmarks(MediaFileNID) {
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
    topic_Publish(tDlg, "EditMediaFileSetMediaFileState", "DONE");
  }

  function cmpDlgDeleteBookmark(bookMarkNID) {
    var reqDeleteBookmark = new ReqDeleteObjectNID(bookMarkNID, "Bookmark",
                                      {action:"FNCCALL", context:this,
                                       fnc:function(respCode, respVal){
                                            if( respCode == "SUCCESS" && respVal != null) {
                                              //TBD
                                            }
                                            else {
                                              alert("Delete Bookmark request failed");
                                            }
                                            return true;
                                          }
                                      });
    reqDeleteBookmark.ServiceName = "assetadminservice";
    reqDeleteBookmark.sendRequest();
  }
}