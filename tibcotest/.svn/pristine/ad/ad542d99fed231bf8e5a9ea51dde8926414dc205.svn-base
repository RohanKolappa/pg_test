function cmpBlkSessionBookmark_Create(){
  
  // get vars from object instance
  var winBookmakSession = this;
  var winBookmakSessionBlkJSX = winBookmakSession.BlkJSX;
  var tBookmakSession = winBookmakSession.tWin;
  var eventBookmakSession = winBookmakSession.eventWin;

  topic_Set(tBookmakSession, "BookmarkGetDataState", "NONE"); 
  topic_Set(tBookmakSession, "BookmarkSessionFilter","NONE");
  topic_Set(tBookmakSession, "AddBookmarkSession","NONE");
  topic_Set(tBookmakSession, "NoBookmarkSessionMsg","NONE");
  
  topic_Set(tBookmakSession, "whereNID",""); 
  topic_Set(tBookmakSession, "whereTextSubStr","");
  topic_Set(tBookmakSession, "whereDateRange","From:,To:,Interval:");
  topic_Set(tBookmakSession, "whereTCRange","From:,To:,Interval:");
  topic_Set(tBookmakSession, "whereFileNID","");
  topic_Set(tBookmakSession, "BookmarkEditState","NONE");
  topic_Set(tBookmakSession, "AddEditBookmarkMode","ADD");
  topic_Set(tBookmakSession, "LabelTSOffset", "Timestamp:");

  var blkSessionBookmarksContainer = getJsxByName([winBookmakSessionBlkJSX, "blkSessionBookmarksContainer"]);
  blkSessionBookmarksContainer.load("components/bookmarks.xml");

  var blkAlert = winBookmakSessionBlkJSX.getDescendantsOfType("com.ipvs.gui.AlertBlock")[0];
  var BookmarkCountToFetch = 50;

  var sessionId = winBookmakSession.sessionId;
  
  topic_Set(tBookmakSession, "whereSessionInfo","sessionId:"+sessionId +",connGroupId:,connId:,entityId:");
  
  var cmpMtxBookmarkList = new CmpCustomPaginationList([winBookmakSessionBlkJSX,"mtxSessionBookmark"],
                                                       [{action:"TOPICEVENT", topic:tBookmakSession, eventvar:"BookmarkGetDataState", eventval:null}],
                                                       {persist:{col:"jsxid",initval:false},
                                                        action:"TOPICEVENT", topic:tBookmakSession, eventvar:"BookmarkSelected", eventval:"jsxid"},
                                                       [{ColumnName: "bookmarkTS", FncName:"@datetime,HH:mm:ss" }],
                                                       null, "xsl/Bookmark.xsl"
                                                       );   
  
  cmpMtxBookmarkList.SortPathArr =  {jsxtext:"title",
                                     bookmarkTS:"TC"
                                    };  
                                    
  cmpMtxBookmarkList.cmpReqParamArr = { StartFrom: "ObjectList_StartFrom",
                                        CountToFetch: "ObjectList_CountToFetch",
                                        SortOrder: "ObjectList_SortOrder",
                                        ObjectCount: "ObjectCount",
                                        SortByXpath: "ObjectList_SortBy"
                                      };
                                          
  cmpMtxBookmarkList.DataUpdatedEvent = {action:"TOPICEVENT", topic:tBookmakSession, eventvar:"BookmarkDataUpdated", eventval:"LOADED"};                       

  cmpMtxBookmarkList.cmpReq = new cmpReq_GetSessionBookmarkPaginationList(tBookmakSession, 
                                                                          {action: "FNCCALL", context: cmpMtxBookmarkList,
                                                                           fnc: function(respCode, respVal) {
                                                                                  if(respCode == "SUCCESS" && respVal != null) {
                                                                                    this.onResponse(respCode, respVal);
                                                                                  }
                                                                                  else {
                                                                                    log.info("GetSessionBookmarkRequest failed. Error Code :" + respCode);
                                                                                    this.onResponseErrorHandle("GetSessionBookmark",respCode, respVal);
                                                                                  }
                                                                                }
                                                                          } 
                                                                         );                                                                     
//  cmpMtxBookmarkList.BlkJSX.setXMLTransformers("xsl/Bookmark.xsl");
   
  cmpMtxBookmarkList.columnDataState = null;
  cmpMtxBookmarkList.columnMtxDoc = null;
  
  cmpMtxBookmarkList.filterTagDataState = null;
  cmpMtxBookmarkList.filterTagDoc = null; //new jsx3.xml.CDF.Document.newDocument();

  this.DeleteBookmark = function (obj) {
    var objRecordId = obj.emGetSession().recordId;
    var ObjRecord = obj.emGetSession().matrix.getRecord(objRecordId);
    var bookmarkNID = ObjRecord.jsxid;
    var bookmarkName = ObjRecord.jsxtext;
    var objAlert = blkAlert.confirm("Warning",
                                      'Are you sure you want to delete bookmark: "' + bookmarkName + '" ? ',
                                       deleteBookmarkItem, null, "OK", "Cancel");
    objAlert.bookmarkNID = bookmarkNID;
  }

  function deleteBookmarkItem(ObjConfirm) {
    if( ObjConfirm == null ) { return;}
    ObjConfirm.doClose();
    topic_Set(tBookmakSession, "whereNID",  ObjConfirm.bookmarkNID);
    sessionHandleBookmark(tBookmakSession, "Delete");
  }

  // setting the function on onlcik of the delete button in the matrix
  var imgDeleteBookmarkBlkJSX = cmpMtxBookmarkList.BlkJSX.getDescendantOfName("jsxImgSessionBookmarkDelete");
  imgDeleteBookmarkBlkJSX.setEvent('this.DeleteBookmark(this)', jsx3.gui.Interactive.EXECUTE);
  imgDeleteBookmarkBlkJSX.DeleteBookmark = jsx3.$F(this.DeleteBookmark).bind(this);

  this.EditBookmark = function (obj) {
    topic_Publish(tBookmakSession, "AddEditBookmarkMode", "EDIT");
    var objRecordId = obj.emGetSession().recordId;
    var ObjRecord = obj.emGetSession().matrix.getRecord(objRecordId);
    var bookmarkNID = ObjRecord.jsxid;
    var bookmarkName = ObjRecord.jsxtext;
    var bookmarkDesc = ObjRecord.jsxDescription;
    var bookmarkTS = ObjRecord.bookmarkTS;
    var bookmarkOffset = ObjRecord.jsxOffset;

    cmpTxtAreaAddSessionBookmarkNID.BlkJSX.setValue(bookmarkNID);
    cmpTxtAddSessionBookmarkName.BlkJSX.setValue(bookmarkName);
    cmpTxtAreaAddSessionBookmarkComment.BlkJSX.setValue(bookmarkDesc);
    cmpTxtAddSessionBookmarkTC.BlkJSX.setValue(bookmarkTS);
    cmpTxtAddSessionBookmarkOffset.BlkJSX.setValue(bookmarkOffset);

    if (!jsx3.util.strEmpty(bookmarkTS)) {
      var dDate = new Date();
      var TimeCode = bookmarkTS.match(/\d{1,}/);
      dDate.setTime(TimeCode);
      var bookmarkTCTimeformatted = formatDate(dDate, "HH:mm:ss");
      var bookmarkTCDateformatted = formatDate(dDate, "MMM dd, yyyy");
      cmpTxtAddSessionBookmarkTCTimeShow.BlkJSX.setValue(bookmarkTCTimeformatted);
      cmpTxtAddSessionBookmarkTCDate.BlkJSX.setValue(bookmarkTCDateformatted);
    }
    topic_Publish(tBookmakSession, "AddBookmarkSession", "OPEN");
    cmpTxtAddSessionBookmarkName.BlkJSX.focus();
  }

  // setting the function on onlcik of the edit button in the matrix
  var imgEditBookmarkBlkJSX = cmpMtxBookmarkList.BlkJSX.getDescendantOfName("jsxImgSessionBookmarkEdit");
  imgEditBookmarkBlkJSX.setEvent('this.EditBookmark(this)', jsx3.gui.Interactive.EXECUTE);
  imgEditBookmarkBlkJSX.EditBookmark = jsx3.$F(this.EditBookmark).bind(this);


  this.PlayFromBookmark = function (obj) {
    var objRecordId = obj.emGetSession().recordId;
    var ObjRecord = obj.emGetSession().matrix.getRecord(objRecordId);
    var bookmarkNID = ObjRecord.jsxid;
    var bookmarkName = ObjRecord.jsxtext;
    var bookmarkDesc = ObjRecord.jsxDescription;
    var bookmarkTS = ObjRecord.bookmarkTS;
    var bookmarkOffset = ObjRecord.jsxOffset;

    // if the PVR is enabled then send the command
    if (topic_Get(tBookmakSession, "CollaborationPVRState") == "CONNECTED") {
      // call the ctrl agent function/command
      topic_Set(tBookmakSession, "ActionBookmarkNID", bookmarkNID);
      topic_Publish(tBookmakSession, "ControllerEvent", "RESUMEFROMBOOKMARK");
    }
    else {
      windowAlert(winBookmakSessionBlkJSX, "Cannot play from bookmark: PVR not enabled for the session.");
      return false;
    }
  }

  // setting the function on onlcik of the playfrom button in the matrix
  var imgPlayFromBookmarkBlkJSX = cmpMtxBookmarkList.BlkJSX.getDescendantOfName("jsxImgSessionBookmarkPlayFrom");
  imgPlayFromBookmarkBlkJSX.setEvent('this.PlayFromBookmark(this)', jsx3.gui.Interactive.EXECUTE);
  imgPlayFromBookmarkBlkJSX.PlayFromBookmark = jsx3.$F(this.PlayFromBookmark).bind(this);

  this.PlayToBookmark = function (obj) {
    var objRecordId = obj.emGetSession().recordId;
    var ObjRecord = obj.emGetSession().matrix.getRecord(objRecordId);
    var bookmarkNID = ObjRecord.jsxid;
    var bookmarkName = ObjRecord.jsxtext;
    var bookmarkDesc = ObjRecord.jsxDescription;
    var bookmarkTS = ObjRecord.bookmarkTS;
    var bookmarkOffset = ObjRecord.jsxOffset;
    // call the ctrl agent function/command  

  }

  // setting the function on onlcik of the playto button in the matrix
  var imgPlayToBookmarkBlkJSX = cmpMtxBookmarkList.BlkJSX.getDescendantOfName("jsxImgSessionBookmarkPlayTo");
  imgPlayToBookmarkBlkJSX.setEvent('this.PlayToBookmark(this)', jsx3.gui.Interactive.EXECUTE);
  imgPlayToBookmarkBlkJSX.PlayToBookmark = jsx3.$F(this.PlayToBookmark).bind(this);


  var cmpNavControls = new CmpPageNavButtonsContainer([winBookmakSessionBlkJSX, "blkRecordingsListPaging"],
                                                      [[winBookmakSessionBlkJSX, "btnPageFirst"],
                                                       [winBookmakSessionBlkJSX, "btnPagePrev"],
                                                       [winBookmakSessionBlkJSX, "btnPageNext"],
                                                       [winBookmakSessionBlkJSX, "btnPageLast"],
                                                       null,
                                                       null,
                                                       [winBookmakSessionBlkJSX, "btnPageRefresh"],
                                                       [winBookmakSessionBlkJSX, "btnPageRefreshSpinner"]
                                                      ],
                                                      cmpMtxBookmarkList,
                                                      [{ action: "TOPICEVENT", topic: tBookmakSession, eventvar: "BookmarkDataUpdated", eventval: null}],
                                                      null
                                                     );
  
  var cmpSessionBookmarksLayoutGrid = new Component([winBookmakSessionBlkJSX, "blkSessionBookmarksLayoutGrid"]);
  var cmpFilterSessionBookmark = new Component([winBookmakSessionBlkJSX, "blkFilterSessionBookmark"]);
  
  var cmpBtnApplyFilterSessionBookmark = new CmpButton([winBookmakSessionBlkJSX,"blkFilterSessionBookmark","btnApplyFilterSessionBookmark"],
                                                         null,
                                                         { action: "FNCCALL", context: null,
                                                           fnc: function() {
                                                                  var bookmarkFilterDisplay = cmpFilterSessionBookmark.BlkJSX.getDisplay();
                                                                  var getRows = cmpSessionBookmarksLayoutGrid.BlkJSX.getRows();
                                                                  var setRows = "24,0,*,22";
                                                                  cmpSessionBookmarksLayoutGrid.BlkJSX.setRows(setRows,true);
                                                                  cmpFilterSessionBookmark.BlkJSX.setDisplay(jsx3.gui.Block.DISPLAYNONE, true);
                                                                  // get the xapths
                                                                  //check for sessionbookmarksearch
                                                                    // check for name
                                                                    // check for description
                                                                   // format the xpath
                                                                  
                                                                  
                                                                   
                                                           }
                                                         }
                                                       );
  var cmpBtnCancelFilterSessionBookmark = new CmpButton([winBookmakSessionBlkJSX,"blkFilterSessionBookmark","btnCancelFilterSessionBookmark"],
                                                         null,
                                                         { action: "FNCCALL", context: null,
                                                           fnc: function() {
                                                                  var bookmarkFilterDisplay = cmpFilterSessionBookmark.BlkJSX.getDisplay();
                                                                  var getRows = cmpSessionBookmarksLayoutGrid.BlkJSX.getRows();
                                                                  var setRows = "24,0,*,22";
                                                                  cmpSessionBookmarksLayoutGrid.BlkJSX.setRows(setRows, true);
                                                                  cmpFilterSessionBookmark.BlkJSX.setDisplay(jsx3.gui.Block.DISPLAYNONE, true);
                                                           }
                                                         }
                                                       );  

  var cmpFilterSessionBookmark = new Component([winBookmakSessionBlkJSX, "blkSessionNoBookmarkMsg"]);
  var cmpBlkNoBookmarkMsg = new CmpContainer([winBookmakSessionBlkJSX, "blkSessionBookmarksContainer"], 
                                                  [{cmp:cmpFilterSessionBookmark, sel: "topic_Get('"+tBookmakSession+"', 'NoBookmarkSessionMsg') != 'NONE'"}
                                                  ],
                                                  [{action:"TOPICEVENT", topic:tBookmakSession, eventvar: "NoBookmarkSessionMsg", eventval: null }],
                                                  null ); 
                                                                                             
  var cmpBlkFilterSessionBookmark = new CmpContainer([winBookmakSessionBlkJSX, "blkSessionBookmarksContainer"], 
                                                  [{cmp:cmpFilterSessionBookmark, sel: "topic_Get('"+tBookmakSession+"', 'BookmarkSessionFilter') != 'NONE'"},
                                                   {cmp:cmpBtnCancelFilterSessionBookmark},
                                                   {cmp:cmpBtnApplyFilterSessionBookmark}
                                                  ],
                                                  [{action:"TOPICEVENT", topic:tBookmakSession, eventvar: "BookmarkSessionFilter", eventval: null }],
                                                  null ); 
                                                                                          
  var cmpBtnFilterSessionBookmark = new CmpButton([winBookmakSessionBlkJSX,"btnFilterSessionBookmark"],
                                                   null,
                                                   { action: "FNCCALL", context: null,
                                                     fnc: function() {
                                                            var bookmarkFilterDisplay = cmpFilterSessionBookmark.BlkJSX.getDisplay();
                                                            var getRows = cmpSessionBookmarksLayoutGrid.BlkJSX.getRows();
                                                            var setRows = "24,0,*,22";
                                                            if( bookmarkFilterDisplay == "none") {
                                                              setRows = "24,200,*,22";
                                                              cmpSessionBookmarksLayoutGrid.BlkJSX.setRows(setRows, true);
                                                              cmpFilterSessionBookmark.BlkJSX.setDisplay(jsx3.gui.Block.DISPLAYBLOCK, true);  
                                                            }
                                                            else{
                                                              cmpSessionBookmarksLayoutGrid.BlkJSX.setRows(setRows, true);
                                                              cmpFilterSessionBookmark.BlkJSX.setDisplay(jsx3.gui.Block.DISPLAYNONE, true);
                                                            }
                                                     }
                                                   }
                                                 );  

  var cmpAddSessionBookmark = new Component([winBookmakSessionBlkJSX, "blkSessionAddBookmark"]);
  
  var cmpTxtAddSessionBookmarkName = new Component([winBookmakSessionBlkJSX, "blkSessionAddBookmark", "txtSessionBookmarkName"]);
  cmpTxtAddSessionBookmarkName.BlkJSX.setEvent("this.txtOnChange(strPREVIOUS, strVALUE);", jsx3.gui.Interactive.CHANGE);
  cmpTxtAddSessionBookmarkName.BlkJSX.txtOnChange = function(strPREVIOUS, strVALUE) {
                                                      var oldValue = strPREVIOUS;
                                                      var newValue = strVALUE;
                                                      if(oldValue != newValue) {
                                                        topic_Set(tBookmakSession, "BookmarkEditState","MODIFY");
                                                      }
                                                    }
                                                    
  var cmpTxtAddSessionBookmarkOffset = new Component([winBookmakSessionBlkJSX, "blkSessionAddBookmark", "txtSessionBookmarkOffset"]);                                                    
  cmpTxtAddSessionBookmarkOffset.BlkJSX.setEvent("this.txtOnChange(strPREVIOUS, strVALUE);", jsx3.gui.Interactive.CHANGE);
  cmpTxtAddSessionBookmarkOffset.BlkJSX.txtOnChange = function(strPREVIOUS, strVALUE) {
                                                        var oldValue = strPREVIOUS;
                                                        var newValue = strVALUE;
                                                        if(oldValue != newValue) {
                                                          topic_Set(tBookmakSession, "BookmarkEditState","MODIFY");
                                                        }
                                                      }  
  
  var cmpTxtAddSessionBookmarkTC = new Component([winBookmakSessionBlkJSX, "blkSessionAddBookmark", "txtSessionBookmarkTime"]);
  var cmpTxtAddSessionBookmarkTCDate = new Component([winBookmakSessionBlkJSX, "blkSessionAddBookmark", "txtSessionBookmarkDate"]);
  var cmpTxtSessionBookmarkMSFNID = new Component([winBookmakSessionBlkJSX, "blkSessionAddBookmark", "txtSessionBookmarkMSFNID"]);
  var cmpTxtSessionBookmarkOwner = new Component([winBookmakSessionBlkJSX, "blkSessionAddBookmark", "txtSessionBookmarkOwner"]);
  var cmpTxtAddSessionBookmarkTCTimeShow = new Component([winBookmakSessionBlkJSX, "blkSessionAddBookmark", "txtSessionBookmarkTimeShow"]);
  cmpTxtAddSessionBookmarkTCTimeShow.BlkJSX.setEvent("this.txtOnChange(strPREVIOUS, strVALUE);", jsx3.gui.Interactive.CHANGE);
  cmpTxtAddSessionBookmarkTCTimeShow.BlkJSX.txtOnChange = function(strPREVIOUS, strVALUE) {
                                                            var oldValue = strPREVIOUS;
                                                            var newValue = strVALUE;
                                                            if(oldValue != newValue) {
                                                              topic_Set(tBookmakSession, "BookmarkEditState","MODIFY");
                                                            }
                                                          }  
                                                        
  var cmpTxtAreaAddSessionBookmarkComment = new Component([winBookmakSessionBlkJSX, "blkSessionAddBookmark", "txtaSessionBookmarkComment"]);
  cmpTxtAreaAddSessionBookmarkComment.BlkJSX.setEvent("this.txtOnChange(strPREVIOUS, strVALUE);", jsx3.gui.Interactive.CHANGE);
  cmpTxtAreaAddSessionBookmarkComment.BlkJSX.txtOnChange = function(strPREVIOUS, strVALUE) {
                                                            var oldValue = strPREVIOUS;
                                                            var newValue = strVALUE;
                                                            if(oldValue != newValue) {
                                                              topic_Set(tBookmakSession, "BookmarkEditState","MODIFY");
                                                            }
                                                          }  
  var cmpLblTSOffset = new CmpLabel([winBookmakSessionBlkJSX, "blkSessionAddBookmark", "lblTSOffset"],
                                     null,
                                     [{action:"TOPICEVENT", topic:tBookmakSession, eventvar:"LabelTSOffset", eventval:null}]);  

  var cmpTxtAreaAddSessionBookmarkNID = new Component([winBookmakSessionBlkJSX, "blkSessionAddBookmark", "txtSessionBookmarkNID"]);
  
  var cmpBtnCancelAddSessionBookmark = new CmpButton([winBookmakSessionBlkJSX,"blkSessionAddBookmark","btnCancelAddNewSessionBookmark"],
                                                         null,
                                                         { action: "FNCCALL", context: null,
                                                           fnc: function() {
                                                                  var bookmarkNID = cmpTxtAreaAddSessionBookmarkNID.BlkJSX.getValue();
                                                                  if( !jsx3.util.strEmpty(bookmarkNID) ){
                                                                    topic_Set(tBookmakSession, "whereNID",bookmarkNID);
                                                                    sessionHandleBookmark(tBookmakSession, "Delete");
                                                                  }
                                                                  //topic_Publish(tBookmakSession, "AddBookmarkSession","NONE");
                                                           }
                                                         }
                                                       );
                                                       
  var cmpBtnSaveAddSessionBookmark = new CmpButton([winBookmakSessionBlkJSX,"blkSessionAddBookmark","btnAddNewSessionBookmark"],
                                                         null,
                                                         { action: "FNCCALL", context: null,
                                                           fnc: function() {
                                                                  var editstate = topic_Get(tBookmakSession, "BookmarkEditState");
                                                                  if(editstate == "MODIFY") {
                                                                    var addBookmark = sessionHandleBookmark(tBookmakSession, "EDIT");
                                                                    if(addBookmark != false){
                                                                      topic_Set(tBookmakSession, "BookmarkEditState","NONE");
                                                                    }
                                                                    
                                                                  }
                                                                  else{
                                                                    topic_Publish(tBookmakSession, "BookmarkActionDataUpdate","ADD");
                                                                    topic_Publish(tBookmakSession, "AddBookmarkSession","NONE");
                                                                    resetAddEditBlkValues();
                                                                  }
                                                           }
                                                         }
                                                       );
                                                        
  var cmpBtnCancelEditSessionBookmark = new CmpButton([winBookmakSessionBlkJSX,"blkSessionAddBookmark","btnCancelEditSessionBookmark"],
                                                         null,
                                                         { action: "FNCCALL", context: null,
                                                           fnc: function() {
                                                                  topic_Publish(tBookmakSession, "AddBookmarkSession","NONE");
                                                                  resetAddEditBlkValues();
                                                           }
                                                         }
                                                       );

  var cmpBtnSaveEditSessionBookmark = new CmpButton([winBookmakSessionBlkJSX,"blkSessionAddBookmark","btnSaveEditSessionBookmark"],
                                                         null,
                                                         { action: "FNCCALL", context: null,
                                                           fnc: function() {
                                                                  var editstate = topic_Get(tBookmakSession, "BookmarkEditState");
                                                                  if(editstate == "MODIFY") {
                                                                    var editBookmark = sessionHandleBookmark(tBookmakSession, "EDIT");
                                                                    if(editBookmark != false){
                                                                      topic_Set(tBookmakSession, "BookmarkEditState","NONE");
                                                                    }
                                                                    
                                                                  }
                                                                  else{
                                                                    // nothing changed so no need to send request
                                                                    //topic_Publish(tBookmakSession, "BookmarkActionDataUpdate","EDIT");
                                                                    
                                                                    topic_Publish(tBookmakSession, "AddBookmarkSession","NONE");
                                                                    resetAddEditBlkValues();
                                                                  }
                                                           }
                                                         }
                                                       );    
                                                                                                                                                                                                                      
  var cmpBlkAddSessionBookmark = new CmpContainer([winBookmakSessionBlkJSX, "blkSessionBookmarksContainer"], 
                                                  [{cmp:cmpAddSessionBookmark, sel: "topic_Get('"+tBookmakSession+"', 'AddBookmarkSession') != 'NONE'"},
                                                   {cmp:cmpBtnCancelAddSessionBookmark, sel: "topic_Get('"+tBookmakSession+"', 'AddEditBookmarkMode') == 'ADD'"},
                                                   {cmp:cmpBtnSaveAddSessionBookmark, sel: "topic_Get('"+tBookmakSession+"', 'AddEditBookmarkMode') == 'ADD'"},
                                                   
                                                   {cmp:cmpBtnCancelEditSessionBookmark, sel: "topic_Get('"+tBookmakSession+"', 'AddEditBookmarkMode') == 'EDIT'"},
                                                   {cmp:cmpBtnSaveEditSessionBookmark, sel: "topic_Get('"+tBookmakSession+"', 'AddEditBookmarkMode') == 'EDIT'"},                                                   
                                                   {cmp:cmpLblTSOffset},
                                                   {cmp:cmpTxtAddSessionBookmarkName},
                                                   {cmp:cmpTxtAddSessionBookmarkTCTimeShow, sel: "topic_Get('"+tBookmakSession+"', 'LabelTSOffset') == 'Timestamp:'"},
                                                   {cmp:cmpTxtAddSessionBookmarkTCDate},
                                                   {cmp:cmpTxtAreaAddSessionBookmarkComment},
                                                   {cmp:cmpTxtAreaAddSessionBookmarkNID},
                                                   {cmp:cmpTxtAddSessionBookmarkTC},
                                                   {cmp:cmpTxtAddSessionBookmarkOffset, sel: "topic_Get('"+tBookmakSession+"', 'LabelTSOffset') == 'Offset:'"}
                                                  ],//LabelTSOffset
                                                  [{action:"TOPICEVENT", topic:tBookmakSession, eventvar: "AddBookmarkSession", eventval: null }],
                                                  null );
                                                  
  var cmpBtnAddSessionBookmark = new CmpButton([winBookmakSessionBlkJSX,"btnOpenAddSessionBookmark"],
                                                   null,
                                                   { action: "FNCCALL", context: null,
                                                     fnc: function() {
                                                            topic_Publish(tBookmakSession, "AddEditBookmarkMode","ADD");
                                                            sessionHandleBookmark(tBookmakSession, "ADD");
                                                            topic_Set(tBookmakSession, "BookmarkEditState","NONE");
                                                     }
                                                   }
                                                 );
  function sessionHandleBookmark(tInstance, Mode) {
    if(Mode == "ADD") {
      var addBookmarkNode = loadXMLFile("JSXAPPS/IPVS_ClientUI/xml/defaults/AddBookmarkRequest.xml");
      var whereSessionInfo = topic_Get(tInstance, "whereSessionInfo")
      addBookmarkNode.setAttribute("whereSessionInfo", whereSessionInfo);
      var addBookmaekReq = new ReqSessionHandleBookmark("AddBookmark", addBookmarkNode,
                                                         {action:"FNCCALL", context:this,
                                                           fnc:function(respCode, respVal){
                                                                if( respCode == "SUCCESS" && respVal != null) {
                                                                  //var responseDoc =  new jsx3.xml.Document();
                                                                  //responseDoc.loadXML(respVal);
                                                                  //utilRuleSetInboundDoc.call(this, "SessionAddBookmarkResponseData", responseDoc);
                                                                  var newBookmarkNode = respVal.selectSingleNode("//Bookmark");
                                                                  var bookmarkTC = newBookmarkNode.getAttribute("TC");
                                                                  var NID = newBookmarkNode.getAttribute("NID");
                                                                  var title = newBookmarkNode.getAttribute("title");
                                                                  var dateCreated = newBookmarkNode.getAttribute("dateCreated");
                                                                  var offset = newBookmarkNode.getAttribute("offset");
                                                                  var mediaSFID = newBookmarkNode.getAttribute("mediaStreamFileNID");
                                                                  var owner = newBookmarkNode.getAttribute("owner");
                                                                  var description = newBookmarkNode.selectSingleNode("//Description").getValue();
                                                                  cmpTxtAddSessionBookmarkTCTimeShow.BlkJSX.setValue(bookmarkTC); 
                                                                  cmpTxtAddSessionBookmarkTC.BlkJSX.setValue(bookmarkTC);
                                                                  cmpTxtAreaAddSessionBookmarkNID.BlkJSX.setValue(NID);
                                                                  cmpTxtAddSessionBookmarkName.BlkJSX.setValue(title);
                                                                  cmpTxtAddSessionBookmarkTCDate.BlkJSX.setValue(dateCreated);
                                                                  cmpTxtAddSessionBookmarkOffset.BlkJSX.setValue(offset);
                                                                  cmpTxtSessionBookmarkMSFNID.BlkJSX.setValue(mediaSFID);
                                                                  cmpTxtSessionBookmarkOwner.BlkJSX.setValue(owner);
                                                                  cmpTxtAreaAddSessionBookmarkComment.BlkJSX.setValue(description);
                                                                  if( !jsx3.util.strEmpty(bookmarkTC) ) {
                                                                    topic_Publish(tInstance, "LabelTSOffset","Timestamp:");
                                                                    var dDate = new Date();
                                                                    var TimeCode = bookmarkTC.match(/\d{1,}/);
                                                                    dDate.setTime(TimeCode);
                                                                    var bookmarkTCTimeformatted = formatDate(dDate, "HH:mm:ss");
                                                                    var bookmarkTCDateformatted = formatDate(dDate, "MMM dd, yyyy");
                                                                    cmpTxtAddSessionBookmarkTCTimeShow.BlkJSX.setValue(bookmarkTCTimeformatted);
                                                                    cmpTxtAddSessionBookmarkTCDate.BlkJSX.setValue(bookmarkTCDateformatted);
                                                                  }
                                                                  else{
                                                                    topic_Publish(tInstance, "LabelTSOffset","Offset:");
                                                                  }
                                                                  topic_Publish(tInstance, "AddBookmarkSession","OPEN");
                                                                  cmpTxtAddSessionBookmarkName.BlkJSX.focus();
                                                                }
                                                                else {
                                                                  //publishing Error code to the connection state msg tooltip
                                                                  cmpReq_HandleErrorCode(tInstance, respCode);
                                                                }
                                                                return true;
                                                              }
                                                          }      
                                                       );
      addBookmaekReq.sendRequest(this);
    }
    else if( Mode == "Delete" ) {
      var deleteBookmarkNode = loadXMLFile("JSXAPPS/IPVS_ClientUI/xml/defaults/DeleteBookmarkRequest.xml");
      var bookMarkNID = topic_Get(tBookmakSession, "whereNID");
      deleteBookmarkNode.setAttribute("whereNID", bookMarkNID);
      var deleteBookmaekReq = new ReqSessionHandleBookmark("DeleteBookmark", deleteBookmarkNode,
                                                         {action:"FNCCALL", context:this,
                                                           fnc:function(respCode, respVal){
                                                                if( respCode == "SUCCESS" && respVal != null) {
                                                                  topic_Set(tBookmakSession, "whereNID","");
                                                                  topic_Publish(tBookmakSession, "BookmarkActionDataUpdate","DELETE");
                                                                  topic_Publish(tBookmakSession, "AddBookmarkSession","NONE"); 
                                                                }
                                                                else {
                                                                  //publishing Error code to the connection state msg tooltip
                                                                  cmpReq_HandleErrorCode(tInstance, respCode);
                                                                }
                                                                return true;
                                                              }
                                                          }      
                                                       );
      deleteBookmaekReq.sendRequest(this);      
    }
    else if( Mode == "EDIT") {
      var bookmarkName = jsx3.util.strTrim(cmpTxtAddSessionBookmarkName.BlkJSX.getValue());
      if(jsx3.util.strEmpty(bookmarkName)) {
        windowAlert(winBookmakSessionBlkJSX, "Please Enter Bookmark Name");
        cmpTxtAddSessionBookmarkName.BlkJSX.focus();
        return false;
      }
      var bTime = cmpTxtAddSessionBookmarkTCTimeShow.BlkJSX.getValue();
      if( !jsx3.util.strEmpty(bTime) ) {
        var bDate = cmpTxtAddSessionBookmarkTCDate.BlkJSX.getValue();
        var bTC = cmpTxtAddSessionBookmarkTC.BlkJSX.getValue();
        var dateString = bDate+" " + bTime ;
        var myDate = new Date(dateString);
        var myTC  = myDate.getTime();
        var bMS = parseInt(bTC) - parseInt(myTC);
        var newBookmarkTS = parseInt(bTC) - parseInt(bMS);
      }
      var bookmarkDesc = cmpTxtAreaAddSessionBookmarkComment.BlkJSX.getValue();
      var bookmarkNID = cmpTxtAreaAddSessionBookmarkNID.BlkJSX.getValue();
      var bookmarkOffset = cmpTxtAddSessionBookmarkOffset.BlkJSX.getValue();
      var editBookmarkNode = loadXMLFile("JSXAPPS/IPVS_ClientUI/xml/defaults/UpdateBookmarkRequest.xml");    
      
      editBookmarkNode.setAttribute("whereNID", bookmarkNID);
      var bookmarkNode = editBookmarkNode.selectSingleNode(".//Bookmark");
      bookmarkNode.setAttribute("title",bookmarkName);
      var LabelTSOffset =  topic_Get(tBookmakSession,"LabelTSOffset");
      // If TC is shown and changed
      if( LabelTSOffset == "Timestamp:"){
        bookmarkNode.setAttribute("TC",newBookmarkTS);
        bookmarkNode.setAttribute("offset", "");      
      }
      // If Offset is shown and changed
      else if(LabelTSOffset == "Offset:"){
        bookmarkNode.setAttribute("TC","");
        bookmarkNode.setAttribute("offset", bookmarkOffset);      
      }

      editBookmarkNode.selectSingleNode(".//Bookmark/Description").setValue(bookmarkDesc);

      var editBookmarkReq = new ReqSessionHandleBookmark("UpdateBookmark", editBookmarkNode,
                                                         {action:"FNCCALL", context:this,
                                                           fnc:function(respCode, respVal){
                                                                if( respCode == "SUCCESS" && respVal != null) {
                                                                  topic_Publish(tBookmakSession, "BookmarkActionDataUpdate","EDIT");
                                                                  topic_Publish(tBookmakSession, "AddBookmarkSession","NONE");                                                                
                                                                  resetAddEditBlkValues();
                                                                }
                                                                else {
                                                                  //publishing Error code to the connection state msg tooltip
                                                                  cmpReq_HandleErrorCode(tInstance, respCode);
                                                                }
                                                                return true;
                                                              }
                                                          }      
                                                       );
      editBookmarkReq.sendRequest(this);      
      
    }
    
  }                                                 

  //helper function to reset the values in the add/edit block text boxes etc.
  function resetAddEditBlkValues() {
    cmpTxtAddSessionBookmarkName.BlkJSX.setValue("");
    cmpTxtAddSessionBookmarkTCTimeShow.BlkJSX.setValue("");
    cmpTxtAddSessionBookmarkTCDate.BlkJSX.setValue("");
    cmpTxtAreaAddSessionBookmarkComment.BlkJSX.setValue("");
    cmpTxtAreaAddSessionBookmarkNID.BlkJSX.setValue("");
    cmpTxtAddSessionBookmarkTC.BlkJSX.setValue("");
    cmpTxtAddSessionBookmarkOffset.BlkJSX.setValue("");
  }
  
                                                                                         
  var cmpBlkSessionBookmark = new CmpContainer([winBookmakSessionBlkJSX, "blkSessionBookmarksContainer"], 
                                                [{cmp:cmpMtxBookmarkList},
                                                 {cmp:cmpBlkAddSessionBookmark},
                                                 {cmp:cmpBtnAddSessionBookmark},
                                                 {cmp:cmpBtnFilterSessionBookmark},
                                                 {cmp:cmpBlkFilterSessionBookmark},
                                                 {cmp:cmpBlkNoBookmarkMsg},
                                                 {cmp:cmpSessionBookmarksLayoutGrid},
                                                 {cmp:cmpNavControls}                                      
                                                ],
                                                null,
                                                null );
                                                                          
  eventBookmakSession.subscribe("BookmarkActionDataUpdate", null, this,
                                function(EventVar, EventVal) {
                                  if( EventVal == "ADD" || EventVal == "DELETE" ||  EventVal == "EDIT") {
                                    cmpMtxBookmarkList.doRepaint();
                                  }
                                }
                              );

  eventBookmakSession.subscribe("ShowBookmarkPanel", null, this,
                                function(EventVar, EventVal) {
                                  if(topic_Get(tBookmakSession, "ViewBookmarkEnabled") == 1 ){
                                    if(topic_Get(tBookmakSession, "BookmarkGetDataState") == "NONE" && topic_Get(tBookmakSession, "BookmarkLoadingStatus") == "LOADED"){
                                      topic_Publish(tBookmakSession, "BookmarkGetDataState", (newServerDate()).getTime());
                                    }
                                  }
                                  else {
                                    topic_Publish(tBookmakSession, "NoBookmarkSessionMsg", "OPEN");
                                  } 
                                }
                              );

  //separate cmpBlkSessionBookmark from cmpBlkMediaPlayer 
  jsx3.sleep((function(){
                          this.init();
                          this.show();
                          //push this component to parent window's SubCmpArr
                          winBookmakSession.SubCmpArr.push({ cmp: cmpBlkSessionBookmark });
                          topic_Publish(tBookmakSession, "BookmarkLoadingStatus", "LOADED");
                          log.info("cmpBlkSessionBookmark:bookmark panel is loaded.");
                        }
               ), null, cmpBlkSessionBookmark);
}
