/* place JavaScript code here */
jsx3.require("jsx3.gui.Block");

jsx3.Class.defineClass("com.ipvs.gui.authmediaclient.MediaList", jsx3.gui.Block, [com.ipvs.util.IPVSComponentI], function (MediaList, MediaList_prototype) {
  MediaList_prototype._LOG = jsx3.util.Logger.getLogger(MediaList.jsxclass.getName());
  MediaList.Server = null;
  MediaList_prototype.init = function () {
    // THIS IS FOR PRODUCTION PATH TBR
    this.mtxRecordingListXMLURL = "jsxuser:///../../Media/Medialist.xml";
    this.MedialIstLogoBasePath = "jsxuser:///../../";
    // 2 paths ONLY FOR DEBUDDING TBC
    //this.mtxRecordingListXMLURL = "D:\\authtool\\Player Test\\Media\\Medialist.xml";
    //this.MedialIstLogoBasePath = "D:\\authtool\\Player Test\\";

    this.mtxRecordingListXSL = "jsxuser:///JSXAPPS/IPVS_AuthMediaClient/xsl/MediaList.xsl";
    this.mtxRecordingList = this.getDescendantOfName("mtxRecordingList");
    this.btnPlayStream = this.mtxRecordingList.getDescendantOfName("imgPlayStream");

    if (!MediaList.Server) MediaList.Server = this.getServer();
    this.rootBlk = this.getParent("mtxRecordingList").getParent();
    this.lblMediaGuideTitle = this.rootBlk.getDescendantOfName("lblMediaGuideTitle");
    this.imgMediaGuideLogo = this.rootBlk.getDescendantOfName("imgMediaGuideLogo");

    this.mtxRecordingList.subscribe(jsx3.gui.Interactive.SELECT, this, this._selectStream);
    this.btnPlayStream.subscribe(jsx3.gui.Interactive.EXECUTE, this, this._playStream);

    this.blkApp = this.getAncestorOfType("com.ipvs.authmediaclient.App");

    //get the data from the xml file
    var xmlNode = this.mtxRecordingList.getXML();
    var xmlURL = this.mtxRecordingList.getXMLURL();
    var xslT = this.mtxRecordingList.getXMLTransformers();
    var objXML = this.getServer().getCache().getDocument(this.mtxRecordingList.getXMLId());
    var dataXSL = this.getServer().loadInclude(this.mtxRecordingListXSL, "MediaList.xsl", "xsl", true);
    this._LOG.info("Medialist dataXSL: " + dataXSL);
    var dataXML = this.getServer().loadInclude(this.mtxRecordingListXMLURL, "Medialist.xml", "xml", true);
    this._LOG.info("Medialist dataXML: " + dataXML);

    //get the title from the xml
    var MediaGuideTitle = dataXML.selectSingleNode(".//LOGOTITLE").getValue();
    if (!jsx3.util.strEmpty(MediaGuideTitle)) {
      this._LOG.info("Medialist Title: " + MediaGuideTitle);
      this.lblMediaGuideTitle.setText(MediaGuideTitle + " Media Guide", true);
    }
    //get the logo file from the xml
    var MediaGuideLogo = dataXML.selectSingleNode(".//LOGOPATH").getValue();
    if (!jsx3.util.strEmpty(MediaGuideLogo)) {
      this.MediaListLogoSrc = this.MedialIstLogoBasePath + MediaGuideLogo;
      this._LOG.info("MedialIst Logo path: " + this.MediaListLogoSrc);
      this.imgMediaGuideLogo.setSrc(this.MediaListLogoSrc);
      this.imgMediaGuideLogo.repaint();
    }

    var XSLDoc = jsx3.xml.XslDocument.wrap(dataXSL);
    dataXML = XSLDoc.transformToObject(dataXML);
    this._LOG.info("Medialist before insertingXML: " + dataXML);
    objXML.loadXML(dataXML.toString());
    var xmlNode1 = this.mtxRecordingList.getXML();

    this._LOG.info("Medialist after insertingXML: " + dataXML);
    try {
      var objHeight = this.mtxRecordingList.getRendered().offsetHeight;
      var rowHeight = this.mtxRecordingList.getRowHeight(jsx3.gui.Matrix.DEFAULT_ROW_HEIGHT);
      var headerHeight = this.mtxRecordingList.getHeaderHeight(jsx3.gui.Matrix.DEFAULT_HEADER_HEIGHT);
      var availableHeight = objHeight - headerHeight;
      var noOfRows = this.mtxRecordingList.getSortedIds().length;
      var requiredHeight = noOfRows * rowHeight;
      this.mtxRecordingList.setSuppressVScroller(jsx3.Boolean.FALSE, true);
      if (availableHeight > requiredHeight) {
        this.mtxRecordingList.setSuppressVScroller(jsx3.Boolean.TRUE, true);
      }
    } catch (e) {
    }
    this.mtxRecordingList.repaint();

  }

  MediaList_prototype._initList = function () {
    var imgPlayStream = this.mtxRecordingList.getDescendantOfName("imgPlayStream");
    imgPlayStream.removeEvent(jsx3.gui.Interactive.EXECUTE);
    imgPlayStream.setEvent('eval("PlayStream(this);")', jsx3.gui.Interactive.EXECUTE);
    PlayStream = this._playStream;
    this.mtxRecordingList.setEvent('eval("SelectStream(this, objEVENT, strRECORDID);")', jsx3.gui.Interactive.SELECT);
    SelectStream = this._selectStream;
  }

  MediaList_prototype._playStream = function (objEvent) {
    var objRecordId = this.mtxRecordingList.getValue();
    var objRecord = this.mtxRecordingList.getRecord(objRecordId);
    if (objRecord.jsxfileType != "MEDIAGROUP") {
      this._LOG.info("Opening media player to play file nid:" + objRecordId);
      var mediaInfo = this.getMediaInfo(objRecord);
      this.openMediaPlayer(mediaInfo);
    }
    else {
      var grpRecord = this.mtxRecordingList.getRecordNode(objRecord.jsxid);
      var recordIter = grpRecord.selectNodeIterator(".//record");
      while (recordIter.hasNext()) {
        var record = recordIter.next();
        var objRecord = this.mtxRecordingList.getRecord(record.getAttribute("jsxid"));
        var mediaInfo = this.getMediaInfo(objRecord);
        this.openMediaPlayer(mediaInfo);
      }
    }
  }

  MediaList_prototype._selectStream = function (objEvent) {
    var objRecordId = this.mtxRecordingList.getValue();
    var objRecord = this.mtxRecordingList.getRecord(objRecordId);
    if (objRecord.jsxfileType != "MEDIAGROUP") {
      this._LOG.info("Opening media player to play file nid:" + objRecordId);
      // this.mtxRecordingList.deselectAllRecords();
      var mediaInfo = this.getMediaInfo(objRecord);
      this.openMediaPlayer(mediaInfo);
    }
  }

  MediaList_prototype.getMediaInfo = function (objRecord) {
    var mediaInfo = new Object;
    if (objRecord.jsxfileType != "MEDIAGROUP") {
      mediaInfo.type = "Clip";
      mediaInfo.fileNID = objRecord.jsxid;
      mediaInfo.fileName = objRecord.jsxtextmediaName;
      mediaInfo.sdpinfo = objRecord.jsxSdpInfo;
      mediaInfo.streamtype = objRecord.jsxStreamType;
    }
    return mediaInfo;
  }

  MediaList_prototype.openMediaPlayer = function (mediaInfo) {
    agtDisplay.launchPlayerByFileNID(mediaInfo);

  }

  MediaList_prototype.destroy = function () {

  }

});