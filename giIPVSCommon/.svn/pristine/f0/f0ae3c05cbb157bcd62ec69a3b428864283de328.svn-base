/*
* Copyright (c) IP Video Systems Inc.
* Use, modification, and distribution subject to terms of license.
*/

/**
* List class.
* <p/>
*/
jsx3.require("jsx3.gui.Matrix");
jsx3.Class.defineClass("com.ipvs.gui.List", jsx3.gui.Matrix, null, function (List, List_prototype) {

  /** @private @jsxobf-clobber */
  List_prototype._LOG = jsx3.util.Logger.getLogger(List.jsxclass.getName());

  List.SELECTEDRECORDREFRESH = "SelectedRecordRefresh";

  List_prototype.onAfterAttach = function () {
    this.jsxsuper();
    this.clsApp = com.ipvs.app.App.clsApp;
    if (!this.clsApp) return;
    if (this.clsApp.INITED) {
      this._onAppInit();
    }
    else {
      this.clsApp.subscribe(com.ipvs.app.App.APP_INIT, this, this._onAppInit);
    }
  }
  List_prototype._onAppInit = function () {
  }

  //Workaround:To HighLight selected record which is set while the list is hidden
  List_prototype.onAfterRestoreView = function () {
    this.jsxsuper();
    var pBlk = this;
    //check any of the parent display is none.
    while (pBlk = pBlk.getParent()) {
      if (pBlk.getDisplay() == jsx3.gui.Block.DISPLAYNONE) {
        return;
      }
    }
    if (!jsx3.util.strEmpty(this.getValue())) {
      this.redrawRecord(this.getValue(), jsx3.xml.CDF.UPDATE);
    }
  }


  /**
  * Overriding Matrix _toggleScrollBoxDisplay.
  * Hides the corner graphic and removes bottom padding to provide max height for list
  * @private
  * @jsxobf-clobber
  */
  if (List_prototype._toggleScrollBoxDisplay) {
    List_prototype._toggleScrollBoxDisplay = function () {
      var objGUI = this.getRendered();
      if (objGUI) {
        objGUI.childNodes[4].style.display = "none";
        objGUI.childNodes[2].style.height = parseInt(objGUI.childNodes[1].style.height) + jsx3.gui.Painted.Box.getScrollSize();
      }
    }
  }
  else if (List_prototype.xx) {
    List_prototype.xx = function () {
      var objGUI = this.getRendered();
      if (objGUI) {
        objGUI.childNodes[4].style.display = "none";
        objGUI.childNodes[2].style.height = parseInt(objGUI.childNodes[1].style.height) + jsx3.gui.Painted.Box.getScrollSize();
      }
    }
  }

  List_prototype.initList = function (xslPath, bPagination, objSortPath) {
    this.clearList();
    this.setXmlAsync(true);
    this.setXMLTransformers(xslPath);
    this.bPagination = bPagination;
    this.SortPathArr = objSortPath;
    this.resetToStart(1, 5000);
    this.autoSelectFirstRow = false;
  }

  List_prototype.resetToStart = function (startFrom, countToFetch, sortOrder, sortBy) {
    this.StartFrom = startFrom;
    this.CountToFetch = countToFetch;
    this.ObjectCount = 0;
    this.SortOrder = sortOrder;
    this.SortAttr = "_SORTBY_";
    this.SortAttrVal = sortBy;
  }

  List_prototype.setURL = function (strURL) {
    this.xmlURL = strURL;
  }

  List_prototype.setObjURLTemplate = function (objURLInfo) {
    this._objURLTemplate = objURLInfo;
  }
  List_prototype.getObjURLTemplate = function () {
    //clone the url template object so that this class will have original template.
    //Otherwise changes for this object from external/parent class will change the template
    return jsx3.$O(this._objURLTemplate).clone();
  }

  List_prototype.setList = function () {
    //check if any request is in process
    if (this.requestInProgress) {
      //Mark request queue flag as true
      //wait till the inprogress request completes
      this.requestInQueue = true;
    }
    else {
      this.clearList();
      this.requestInProgress = true;
      var strURL = this.xmlURL;
      strURL = this._replaceVarinURL(strURL);
      this.resetXmlCacheData();
      this.setXMLURL(strURL);
      this.repaintData();
    }
  }

  List_prototype._replaceVarinURL = function (urlStr, whereNID) {
    urlStr = urlStr.replace(/_STARTFROM_/g, this.StartFrom);
    urlStr = urlStr.replace(/_COUNTTOFETCH_/g, this.CountToFetch);
    urlStr = urlStr.replace(/_SORTORDER_/g, this.SortOrder);
    //Set the sortby attribute and value depending on sort.
    //Set "" for remaing sort by attributes.
    for (var sortPathArrItem in this.SortPathArr) {
      var SortAttr = this.SortPathArr[sortPathArrItem].SortAttr;
      if (SortAttr == this.SortAttr) {
        urlStr = eval('urlStr.replace(/' + SortAttr + '/g, this.SortAttrVal)');
      }
      else {
        urlStr = eval('urlStr.replace(/' + SortAttr + '/g, "")');
      }
    }
    return urlStr;
  }

  List_prototype.doSort = function () {
    this.jsxsuper();
    if (this.bPagination) {
      var sortPath = this.getSortPath();
      this.SortAttr = this.SortPathArr[sortPath].SortAttr
      this.SortAttrVal = this.SortPathArr[sortPath].SortAttrVal;
      this.SortOrder = this.getSortDirection();
      this.setList();
    }
  }

  List_prototype.onXmlBinding = function (objEvent) {
    this.jsxsuper(objEvent);
    this.requestInProgress = false;
    this._updatePaginationBlock();
    if (this.autoSelectFirstRow) {
      this._setAutoSelectFirstRow();
    }
    else {
      this._setSelections();
    }
    //check if any request is in queue
    if (this.requestInQueue) {
      //send latest request in queue
      this.setList();
    }
  };

  List_prototype._setAutoSelectFirstRow = function () {
    var xmlData = this.getXML();
    if (!jsx3.util.strEmpty(xmlData)) {
      firstRecNode = xmlData.getFirstChild();
      if (!jsx3.util.strEmpty(firstRecNode)) {
        firstRecNode.setAttribute("jsxselected", "1");
      }
    }
    this.publish({ subject: jsx3.gui.Interactive.SELECT, context: { objEVENT: this} });
  }

  List_prototype._setSelections = function () {
    if (jsx3.util.strEmpty(this._persistSelection)) return;
    var record = this.getRecord(this._persistSelection);
    if (!jsx3.util.strEmpty(record)) {
      this.selectRecord(this._persistSelection);
    }
  }

  List_prototype._updatePaginationBlock = function () {
    if (this.bPagination) {
      var xmlData = this.getXML();
      this.StartFrom = xmlData.getAttribute("startFrom");
      this.ObjectCount = xmlData.getAttribute("objectCount");
      this.publish({ subject: "PaginationListUpdated", context: { objEVENT: this} });
    }
  }

  List_prototype.clearList = function () {
    if (!jsx3.util.strEmpty(this._notificationTimerID)) {
      clearTimeout(this._notificationTimerID);
    }
    this._clearList();
  }

  List_prototype.setPersistSelection = function (id) {
    this._persistSelection = id;
    this._persistRecordNode = this.getRecordNode(id);
  }

  List_prototype.getPersistRecordNode = function () {
    return this._persistRecordNode;
  }

  List_prototype._clearList = function () {
    this._notificationNIDArr = new Array();
    this._whereNIDArr = new Array();
    this._notificationTimerID = null;
    this.clearXmlData();
    this.repaintData();
    this.requestInQueue = false;
    this.requestInProgress = false;
  }

  List_prototype.initNotificationHandling = function (objType) {
    this._notificationURL = null;
    this._objectTypeArr = objType.split(",");
    this.clsApp.clsNotification.subscribe("DBUpdate", this, "_handleNotification");
  }

  List_prototype.setNotificationURL = function (strURL) {
    this._notificationURL = strURL
  }

  List_prototype._handleNotification = function (objEvent) {
    if (objEvent == null) return;
    var objPresent = false;
    for (var i = 0; i < this._objectTypeArr.length; i++) {
      if (objEvent.objType == this._objectTypeArr[i]) {
        objPresent = true;
        break;
      }
    }

    if (!objPresent) return;

    var eventType = objEvent.type;
    if (eventType == "add") {
      //TBC in case of pagination
    }
    else if (eventType == "replace") {
      var ItemNode = this.getRecord(objEvent.nodeID);
      //process the notification even it is not in current view but it is persist record.
      if (jsx3.util.strEmpty(ItemNode) && objEvent.nodeID != this._persistSelection) {
        return;
      }
    }
    else if (eventType == "delete") {
      var ItemNode = this.getRecord(objEvent.nodeID);
      var isSelected = false;
      if (!jsx3.util.strEmpty(ItemNode)) {
        //Check deleted item is currently selected
        if (ItemNode.getAttribute("jsxselected")) isSelected = true;
        this.deleteRecord(objEvent.nodeID);
      }
      //if deleted record is persist record then mark it as currently selected record.
      if (objEvent.nodeID == this._persistSelection) {
        this.setPersistSelection(null);
        isSelected = true;
      }

      //if the record is current active/selected record then notify
      if (isSelected) {
        this.publish({ subject: jsx3.gui.Interactive.SELECT, context: { objEVENT: this} });
      }
      return;
    }
    if (!jsx3.util.strEmpty(this._notificationNIDArr)) {
      this._notificationNIDArr.push(objEvent.nodeID);
      if (jsx3.util.strEmpty(this._notificationTimerID)) {
        var ThisClass = this;
        this._notificationTimerID = setTimeout(function () { List_prototype._sendReqForNotificationItems(ThisClass); }, 2000);
      }
    }
  }

  List_prototype._sendReqForNotificationItems = function (ThisClass) {
    ThisClass._notificationTimerID = null;
    if (jsx3.util.strEmpty(ThisClass._notificationURL)) return;
    if (ThisClass._notificationNIDArr.length > 0) {
      ThisClass._whereNIDArr = ThisClass._notificationNIDArr;
      ThisClass._notificationNIDArr = new Array();
      var reqURL = ThisClass._notificationURL;
      reqURL = ThisClass._replaceVarinURL(reqURL);
      reqURL = reqURL.replace(/_WHERENID_/g, ThisClass._whereNIDArr.join());
      this._sendAsyncReq(reqURL).when(jsx3.$F(function (objResponse) {
        var respStatus = objResponse.Status;
        var respVal = objResponse.Val;
        if (respStatus == jsx3.net.Request.STATUS_OK) {
          var xslFilePath = this.getXMLTransformers()[0];
          var xslURI = this.getUriResolver().resolveURI(xslFilePath);
          var XSLFile = this.getServer().loadInclude(xslURI, this.getXMLId() + "_Update", "xsl");
          if (XSLFile) {
            var XSLDoc = jsx3.xml.XslDocument.wrap(XSLFile);
            var updatedDoc = XSLDoc.transformToObject(respVal);
            this._onResponseUpdateRecord(updatedDoc);
          }
        }
      }).bind(ThisClass));
    }
  }

  List_prototype._onResponseUpdateRecord = function (recNode) {
    var recordNodeIterartor = recNode.selectNodeIterator(".//record");
    while (recordNodeIterartor.hasNext()) {
      var recordNode = recordNodeIterartor.next();
      var nodeId = recordNode.getAttribute("jsxid");
      var orgRecordNode = this.getRecordNode(nodeId);
      //check if the record is there or not.
      var isRecordSelected = 0;
      var isPersistRecord = false;
      //check result for persist record if any
      if (this._persistSelection == nodeId) {
        isPersistRecord = true;
        this._persistRecordNode = recordNode;
      }
      //If the record is in current view then change the flag
      if (!jsx3.util.strEmpty(orgRecordNode)) {
        isRecordSelected = orgRecordNode.getAttribute("jsxselected");
        recordNode.setAttribute("jsxselected", isRecordSelected);
        isPersistRecord = false;
      }
      if (!isPersistRecord) {
        this.insertRecordNode(recordNode);
      }

      if (isRecordSelected || isPersistRecord) {
        this.publish({ subject: List.SELECTEDRECORDREFRESH, context: { objEVENT: this} });
      }
      if (!jsx3.util.strEmpty(this.onAfterRecordUpdate)) this.onAfterRecordUpdate(recordNode);
    }
  }

  List_prototype._sendAsyncReq = jsx3.$Y(function (cb) {
    var notificationRequest = new com.ipvs.api.Request();
    notificationRequest.async_send(cb.args()[0]).when(function (objResponse) {
      notificationRequest.endRequest();
      cb.done(objResponse);
    });
  });
});