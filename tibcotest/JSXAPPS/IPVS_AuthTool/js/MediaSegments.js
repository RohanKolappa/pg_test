/*
* Copyright (c) IP Video Systems Inc.
* Use, modification, and distribution subject to terms of license.
*/

/**
* MediaSegments class provides UI to configure cut-points for a media before adding to a project.
* This is during second step(AddFiles) of NewProject.
* <p/>
* This class uses jsx3.gui.Block, com.ipvs.util.IPVSComponentI.
*/
jsx3.require("jsx3.gui.Block");

jsx3.Class.defineClass("com.ipvs.gui.authtool.MediaSegments", jsx3.gui.Block, [com.ipvs.util.IPVSComponentI], function(MediaSegments, MediaSegments_prototype) {

  /** @private @jsxobf-clobber */
  MediaSegments_prototype._LOG = jsx3.util.Logger.getLogger(MediaSegments.jsxclass.getName());

  MediaSegments_prototype.onAfterAttach = function() {
    this.jsxsuper();
    this.init();
  }

  /**
  * The instance initializer.
  */
  MediaSegments_prototype.init = function() {
    this.mediaInfoNode = null;
    this.activeInPointTC = null;
    this.activeOutPointTC = null;
    this.openMode = "ADD";
    //Get handle of child components and subscribe as required
    this.txtMediaTitle = this.getDescendantOfName("txtMediaTitle");
    this.lblCurrentTime = this.getDescendantOfName("lblCurrentTime");
    this.sliderTcHandle = this.getDescendantOfName("sliderTcHandle");
    this.blkSliderBar = this.getDescendantOfName("blkSliderBar");
    this.blkSegmentFill = this.getDescendantOfName("blkSegmentFill");
    this.blkInPointMarker = this.getDescendantOfName("blkInPointMarker");
    this.blkOutPointMarker = this.getDescendantOfName("blkOutPointMarker");
    this.lblTotalTime = this.getDescendantOfName("lblTotalTime");

    this.btnSetInPoint = this.getDescendantOfName("btnSetInPoint");
    this.txtInPointTC = this.getDescendantOfName("txtInPointTC");
    this.btnInPointUp = this.getDescendantOfName("btnInPointUp");
    this.btnInPointDown = this.getDescendantOfName("btnInPointDown");

    this.btnSetOutPoint = this.getDescendantOfName("btnSetOutPoint");
    this.txtOutPointTC = this.getDescendantOfName("txtOutPointTC");
    this.btnOutPointUp = this.getDescendantOfName("btnOutPointUp");
    this.btnOutPointDown = this.getDescendantOfName("btnOutPointDown");

    this.txtDuration = this.getDescendantOfName("txtDuration");
    this.lblClearEntry = this.getDescendantOfName("lblClearEntry");
    this.btnAddSegment = this.getDescendantOfName("btnAddSegment");

    this.mtxMediaSegmentsList = this.getDescendantOfName("mtxMediaSegmentsList");
    this.imgDeleteSegment = this.mtxMediaSegmentsList.getDescendantOfName("imgDeleteSegment");
    this.mtxMediaSegmentsList.getDescendantOfName("inpoint", false, false).setFormatHandler(jsx3.$F(this.formatMediaFileTime).bind(this));
    this.mtxMediaSegmentsList.getDescendantOfName("outpoint", false, false).setFormatHandler(jsx3.$F(this.formatMediaFileTime).bind(this));

    this.blkClipsInfo = this.getDescendantOfName("blkClipsInfo");
    this.mtxMediaClipsList = this.getDescendantOfName("mtxMediaClipsList");

    this.imgCancel = this.getDescendantOfName("imgCancel");
    this.imgDone = this.getDescendantOfName("imgDone");

    this.sliderTcHandle.removeEvent(jsx3.gui.Interactive.CHANGE);
    this.sliderTcHandle.setEvent("this.onSelect(fpVALUE);", jsx3.gui.Interactive.CHANGE);
    // the above event will fire in the context of the blk - so we setup its onSelect to point to ours
    this.sliderTcHandle.onSelect = jsx3.$F(this._onChange_TCHandle).bind(this);
    // setup the select event handler
    this.sliderTcHandle.removeEvent(jsx3.gui.Interactive.INCR_CHANGE);
    this.sliderTcHandle.setEvent("this.onDrag(fpVALUE);", jsx3.gui.Interactive.INCR_CHANGE);
    // the above event will fire in the context of the blk - so we setup its onSelect to point to ours
    this.sliderTcHandle.onDrag = jsx3.$F(this._onChange_TCHandle).bind(this);
    this.blkBookmarksList = this.getDescendantOfName("blkBookmarksList");
    this.blkBookmarksList.subscribe("BOOKMARKSELECTED", this, this._onBookmarkSelect);

    this.btnSetInPoint.subscribe(jsx3.gui.Interactive.EXECUTE, this, this._onClick_InPoint);
    this.btnInPointUp.subscribe(jsx3.gui.Interactive.EXECUTE, this, this._onClick_InPointUp);
    this.btnInPointDown.subscribe(jsx3.gui.Interactive.EXECUTE, this, this._onClick_InPointDown);

    this.btnSetOutPoint.subscribe(jsx3.gui.Interactive.EXECUTE, this, this._onClick_OutPoint);
    this.btnOutPointUp.subscribe(jsx3.gui.Interactive.EXECUTE, this, this._onClick_OutPointUp);
    this.btnOutPointDown.subscribe(jsx3.gui.Interactive.EXECUTE, this, this._onClick_OutPointDown);

    this.lblClearEntry.removeEvent(jsx3.gui.Interactive.JSXCLICK);
    this.lblClearEntry.setEvent("this.onClick();", jsx3.gui.Interactive.JSXCLICK);
    this.lblClearEntry.onClick = jsx3.$F(this._onClick_ClearEntry).bind(this);

    this.btnAddSegment.subscribe(jsx3.gui.Interactive.EXECUTE, this, this._onClick_AddSegment);
    this.imgDeleteSegment.subscribe(jsx3.gui.Interactive.EXECUTE, this, this._deleteSegment);
    this.imgCancel.subscribe(jsx3.gui.Interactive.EXECUTE, this, this._onClick_Cancel);
    this.imgDone.subscribe(jsx3.gui.Interactive.EXECUTE, this, this._onClick_Done);

    //get the handle of Ancestor(AddFiles) class
    this.blkAddFiles = this.getAncestorOfType("com.ipvs.gui.authtool.AddFiles");
    this._reset();
    this._LOG.info("ProjectAddFiles: Media edit segment dialog initiated");
  }

  /**
  * Shows media cut-point dialog and set media info.
  * @param {Entity} Media group/clip node
  * @param {String} dailog open mode ADD/EDIT
  */
  MediaSegments_prototype.open = function(mediaInfoNode, mode) {
    if(jsx3.util.strEmpty(mediaInfoNode)) return;
    this.openMode = mode;
    this.mediaInfoNode = mediaInfoNode.cloneNode(true);
    this.mediaPlayLength = this.mediaInfoNode.getAttribute("jsxLength");
    this.mediaNodeType = jsx3.util.strEmpty(this.mediaInfoNode.getAttribute("jsxGrpNID")) ? "GROUP" : "CLIP";
    var newMediaTitle = this._getNewTitleForMedia(this.mediaInfoNode)
    this.mediaInfoNode.setAttribute("jsxName", newMediaTitle);
    this.txtMediaTitle.setValue(newMediaTitle);
    this.lblTotalTime.setText(this.milliSec2HMS(this.mediaPlayLength), true);
    this._setGroupInfo();
    if(this.mediaNodeType == "GROUP") {
      this.blkClipsInfo.setDisplay(jsx3.gui.Block.DISPLAYBLOCK, true);
    }
    else {
      this.blkClipsInfo.setDisplay(jsx3.gui.Block.DISPLAYNONE, true);
    }
    if(this.openMode != "ADD") {
      this._setSegmentList();
    }
    this._setBookmarks();
    this.cmpShow();
  }

  /** 
  *@private @jsxobf-clobber
  *If selected item media is a group then it displays clips list to change titles.
  */
  MediaSegments_prototype._setGroupInfo = function() {
    this.mtxMediaClipsList.clearXmlData();
    this.mtxMediaClipsList.repaintData();
    var recIter = this.mediaInfoNode.selectNodeIterator(".//record");
    while(recIter.hasNext()) {
      var clipNode = recIter.next();
      var newClipTitle = this._getNewTitleForMedia(clipNode);
      clipNode.setAttribute("jsxName", newClipTitle);
      var objRec = {
        jsxid: clipNode.getAttribute("jsxid"),
        jsxName: newClipTitle,
        jsxChkColumn: (this.openMode == "ADD" || clipNode.getAttribute("jsxActive") != "false") ? jsx3.gui.CheckBox.CHECKED : jsx3.gui.CheckBox.UNCHECKED
      }
      this.mtxMediaClipsList.insertRecord(objRec, null, true);
    }
  }

  /** 
  *@private @jsxobf-clobber
  *If dialog opened for edit then it will display segment list and information
  */
  MediaSegments_prototype._setSegmentList = function() {
    var segmentIter = this.mediaInfoNode.selectNodeIterator("./SEGMENTLIST/SEGMENT");
    while(segmentIter.hasNext()) {
      var segment = segmentIter.next();
      //skipping milliseconds while displaying 
      var duration = Math.floor(segment.getAttribute("outPoint") / 1000) - Math.floor(segment.getAttribute("inPoint") / 1000);
      var objSegmentRec = {
        jsxid: segment.getAttribute("id"),
        jsxInPointTC: segment.getAttribute("inPoint"),
        jsxOutPointTC: segment.getAttribute("outPoint"),
        jsxDuration: this.milliSec2HMS(duration * 1000)
      };
      this.mtxMediaSegmentsList.insertRecord(objSegmentRec, null, true);
      this._addSegmentFill(objSegmentRec.jsxid, objSegmentRec.jsxInPointTC, objSegmentRec.jsxOutPointTC);
    }
  }

  /** @private @jsxobf-clobber */
  MediaSegments_prototype._getNewTitleForMedia = function(mediaNode) {
    var newTitle = mediaNode.getAttribute("jsxName");
    if(this.openMode == "ADD") {
      var newTitle = this.blkAddFiles.getNewName(mediaNode.getAttribute("jsxName"), mediaNode.getAttribute("jsxCreatedDate"), this.mediaInfoNode);
    }
    return newTitle;
  }

  /** @private @jsxobf-clobber */
  //This will get the info from mkd file and extract the bookmarks node from info
  MediaSegments_prototype._setBookmarks = function() {
    if(this.mediaInfoNode.getAttribute("jsxIsTwodotXMedia") == "true") {
      this._LOG.info("_setBookmarks::SetBookmarks is not supported for 2.X Media");
      return;
    }
    var clipNode = this.mediaInfoNode;
    if(this.mediaNodeType == "GROUP") {
      //get first clip to read bookmarks
      clipNode = this.mediaInfoNode.selectSingleNode(".//record");
    }
    var mkdFilePath = clipNode.getAttribute("jsxBasePath") + "\\" + clipNode.getAttribute("jsxFileNID") + ".mkd";
    var bookmarksNode = this.blkAddFiles.getInfoFromMKD(mkdFilePath, "bookmarks", null, false);
    this.blkBookmarksList.displayBookmarks(bookmarksNode, this.mediaPlayLength);
  }

  /** @private @jsxobf-clobber */
  //On bookmark click it sets the slider pointer position
  MediaSegments_prototype._onBookmarkSelect = function(objEvent) {
    var objBookMark = objEvent.context;
    var sliderPosition = (objBookMark.MarkOffset / this.mediaPlayLength) * 100;
    this.sliderTcHandle.setValue(sliderPosition);
  }

  /** Resets and hides cut-point dialog*/
  MediaSegments_prototype.close = function() {
    this._reset();
    this.cmpHide();
  }

  /**@private @jsxobf-clobber */
  MediaSegments_prototype._reset = function() {
    this.mediaInfoNode = null;
    this.mediaPlayLength = 0;
    this.blkSegmentFill.setDisplay(jsx3.gui.Block.DISPLAYNONE, true);
    this.blkSliderBar.removeChildren();
    this.lblCurrentTime.setText("00:00:00", true);
    this.lblTotalTime.setText("00:00:00", true);
    this.sliderTcHandle.setValue(0);
    this._clearEntry();
    this.mtxMediaSegmentsList.clearXmlData();
    this.mtxMediaSegmentsList.repaintData();
    this.blkBookmarksList.clearList();
  }

  /**
  * @private @jsxobf-clobber
  * This will be called incase of drag slider pointer.
  * This will set current media offset label 
  */
  MediaSegments_prototype._onChange_TCHandle = function(fpVALUE) {
    var offSet = Math.round(fpVALUE * (this.mediaPlayLength / 100));
    this.lblCurrentTime.setText(this.milliSec2HMS(offSet), true);
  }

  /**
  * @private @jsxobf-clobber
  * This will be called onclick of Inpoint btn.
  * This will set InPointTC and set the position of InPointMarker 
  */
  MediaSegments_prototype._onClick_InPoint = function() {
    var sliderPosition = this.sliderTcHandle.getValue();
    var offSet = Math.round(sliderPosition * (this.mediaPlayLength / 100));
    if(jsx3.util.strEmpty(this.activeOutPointTC) || this._setDuration(offSet, this.activeOutPointTC)) {
      this.activeInPointTC = offSet;
      this.txtInPointTC.setValue(this.milliSec2HMS(offSet));
      this.blkInPointMarker.setDisplay(jsx3.gui.Block.DISPLAYBLOCK, true);
      var markerLeft = parseInt((this.blkSliderBar.getLeft() - this.blkInPointMarker.getWidth()) + (sliderPosition * this.blkSliderBar.getWidth() / 100));
      this.blkInPointMarker.setLeft(markerLeft, true);
      this.btnInPointUp.setEnabled(jsx3.gui.Form.STATEENABLED, true);
      this.btnInPointDown.setEnabled(jsx3.gui.Form.STATEENABLED, true);
    }
  }

  /**
  * @private @jsxobf-clobber
  * This will be called onclick of InpointUp btn.
  * This will increase the InPointTC by 1 sec and set the position of InPointMarker 
  */
  MediaSegments_prototype._onClick_InPointUp = function() {
    if(jsx3.util.strEmpty(this.activeInPointTC)) this.activeInPointTC = 0;
    var offSet = this.activeInPointTC + 1000;
    if(offSet > this.mediaPlayLength) offSet = this.mediaPlayLength;
    if(jsx3.util.strEmpty(this.activeOutPointTC) || this._setDuration(offSet, this.activeOutPointTC)) {
      this.activeInPointTC = offSet;
      this.txtInPointTC.setValue(this.milliSec2HMS(this.activeInPointTC));
      this.blkInPointMarker.setDisplay(jsx3.gui.Block.DISPLAYBLOCK, true);
      var offSetPosition = (this.activeInPointTC / this.mediaPlayLength) * 100;
      var markerLeft = parseInt((this.blkSliderBar.getLeft() - this.blkInPointMarker.getWidth()) + (offSetPosition * this.blkSliderBar.getWidth() / 100));
      this.blkInPointMarker.setLeft(markerLeft, true);
    }
  }

  /**
  * @private @jsxobf-clobber
  * This will be called onclick of InpointDown btn.
  * This will decrease the InPointTC by 1 sec and set the position of InPointMarker 
  */
  MediaSegments_prototype._onClick_InPointDown = function() {
    if(jsx3.util.strEmpty(this.activeInPointTC)) this.activeInPointTC = 0;
    this.activeInPointTC = this.activeInPointTC - 1000;
    if(this.activeInPointTC < 0) this.activeInPointTC = 0;
    this.txtInPointTC.setValue(this.milliSec2HMS(this.activeInPointTC));
    //this._setDuration();
    this.blkInPointMarker.setDisplay(jsx3.gui.Block.DISPLAYBLOCK, true);
    var offSetPosition = (this.activeInPointTC / this.mediaPlayLength) * 100;
    var markerLeft = parseInt((this.blkSliderBar.getLeft() - this.blkInPointMarker.getWidth()) + (offSetPosition * this.blkSliderBar.getWidth() / 100));
    this.blkInPointMarker.setLeft(markerLeft, true);
  }

  /**
  * @private @jsxobf-clobber
  * This will be called onclick of OutPoint btn.
  * This will set OutPointTC and set the position of OutPointMarker 
  */
  MediaSegments_prototype._onClick_OutPoint = function() {
    if(jsx3.util.strEmpty(this.activeInPointTC)) {
      alert("Select InPoint before selecting OutPoint");
      return;
    }
    var sliderPosition = this.sliderTcHandle.getValue();
    var offSet = Math.round(sliderPosition * (this.mediaPlayLength / 100));
    if(this._setDuration(this.activeInPointTC, offSet)) {
      this.activeOutPointTC = offSet;
      this.txtOutPointTC.setValue(this.milliSec2HMS(offSet));
      this.blkOutPointMarker.setDisplay(jsx3.gui.Block.DISPLAYBLOCK, true);
      var markerLeft = parseInt(this.blkSliderBar.getLeft() + (sliderPosition * this.blkSliderBar.getWidth() / 100));
      this.blkOutPointMarker.setLeft(markerLeft, true);
      this.btnOutPointUp.setEnabled(jsx3.gui.Form.STATEENABLED, true);
      this.btnOutPointDown.setEnabled(jsx3.gui.Form.STATEENABLED, true);
      this.btnAddSegment.setEnabled(jsx3.gui.Form.STATEENABLED, true);
    }
  }

  /**
  * @private @jsxobf-clobber
  * This will be called onclick of OutPointUp btn.
  * This will increase the OutPointTC by 1 sec and set the position of OutPointMarker 
  */
  MediaSegments_prototype._onClick_OutPointUp = function() {
    if(jsx3.util.strEmpty(this.activeOutPointTC)) this.activeOutPointTC = 0;
    this.activeOutPointTC = this.activeOutPointTC + 1000;
    if(this.activeOutPointTC > this.mediaPlayLength) this.activeOutPointTC = this.mediaPlayLength;
    this.txtOutPointTC.setValue(this.milliSec2HMS(this.activeOutPointTC));
    //this._setDuration();
    this.blkOutPointMarker.setDisplay(jsx3.gui.Block.DISPLAYBLOCK, true);
    var offSetPosition = (this.activeOutPointTC / this.mediaPlayLength) * 100;
    var markerLeft = parseInt(this.blkSliderBar.getLeft() + (offSetPosition * this.blkSliderBar.getWidth() / 100));
    this.blkOutPointMarker.setLeft(markerLeft, true);
  }

  /**
  * @private @jsxobf-clobber
  * This will be called onclick of OutPointDown btn.
  * This will decrease the OutPointTC by 1 sec and set the position of OutPointMarker 
  */
  MediaSegments_prototype._onClick_OutPointDown = function() {
    var offSet = this.activeOutPointTC - 1000;
    if(offSet < 0) offSet = 0;
    if(this._setDuration(this.activeInPointTC, offSet)) {
      this.activeOutPointTC = offSet;
      this.txtOutPointTC.setValue(this.milliSec2HMS(this.activeOutPointTC));
      this.blkOutPointMarker.setDisplay(jsx3.gui.Block.DISPLAYBLOCK, true);
      var offSetPosition = (this.activeOutPointTC / this.mediaPlayLength) * 100;
      var markerLeft = parseInt(this.blkSliderBar.getLeft() + (offSetPosition * this.blkSliderBar.getWidth() / 100));
      this.blkOutPointMarker.setLeft(markerLeft, true);
    }
  }

  /**
  * @private @jsxobf-clobber
  * This will clear/resets the current segment selection
  */
  MediaSegments_prototype._onClick_ClearEntry = function() {
    this._clearEntry();
  }

  /**
  * @private @jsxobf-clobber
  * This will add segment filler on slider bar and also adds a record into segment list
  */
  MediaSegments_prototype._onClick_AddSegment = function() {
    if(!this._setDuration(this.activeInPointTC, this.activeOutPointTC)) return;
    var objRec = {
      jsxid: "Seg_" + getUUID().toLowerCase(),
      jsxInPointTC: this.activeInPointTC,
      jsxOutPointTC: this.activeOutPointTC,
      jsxDuration: this.txtDuration.getValue()
    };
    this.mtxMediaSegmentsList.insertRecord(objRec, null, true);
    this._addSegmentFill(objRec.jsxid, objRec.jsxInPointTC, objRec.jsxOutPointTC);
    this._clearEntry();
  }

  /**
  * @private @jsxobf-clobber
  * This will delete segment filler from slider bar and also deletes record from segment list
  */
  MediaSegments_prototype._deleteSegment = function() {
    var recoedId = this.mtxMediaSegmentsList.getValue();
    var objRecord = this.mtxMediaSegmentsList.deleteRecord(recoedId);
    //delete the segment filling 
    var arrSegments = this.blkSliderBar.getChildren();
    var iterSegments = (new jsx3.util.List(arrSegments)).iterator();
    while(iterSegments.hasNext()) {
      var blkSegment = iterSegments.next();
      if(blkSegment.ID == recoedId) {
        this.blkSliderBar.removeChild(blkSegment);
      }
    }
  }

  /** @private @jsxobf-clobber */
  MediaSegments_prototype._addSegmentFill = function(segmentId, inPoint, outPoint) {
    var newSegmentFill = this.blkSegmentFill.doClone(jsx3.app.Model.PERSISTEMBED);
    this.blkSliderBar.adoptChild(newSegmentFill, true, true);
    newSegmentFill.ID = segmentId;
    var left = parseInt((inPoint / this.mediaPlayLength) * this.blkSliderBar.getWidth());
    var width = parseInt(((outPoint - inPoint) / this.mediaPlayLength) * this.blkSliderBar.getWidth());
    //set its position
    newSegmentFill.setDimensions(left, 0, width, '100%', true);
    newSegmentFill.setDisplay(jsx3.gui.Block.DISPLAYBLOCK, true);
  }

  /** @private @jsxobf-clobber */
  MediaSegments_prototype._setDuration = function(inPoint, outPoint) {
    //skipping milliseconds while displaying 
    var segDuration = Math.floor(outPoint / 1000) - Math.floor(inPoint / 1000);
    if(segDuration <= 0) {
      alert("Select Valid InPoint/OutPoints \nOutPoint should be greater than InPoint");
      return false;
    }
    this.txtDuration.setValue(this.milliSec2HMS(segDuration * 1000));
    return true;
  }

  /** @private @jsxobf-clobber */
  MediaSegments_prototype._clearEntry = function() {
    this.blkInPointMarker.setDisplay(jsx3.gui.Block.DISPLAYNONE, true);
    this.blkOutPointMarker.setDisplay(jsx3.gui.Block.DISPLAYNONE, true);
    this.txtInPointTC.setValue("00:00:00");
    this.txtOutPointTC.setValue("00:00:00");
    this.activeInPointTC = null;
    this.activeOutPointTC = null;
    this.txtDuration.setValue("00:00:00");
    this.blkBookmarksList.clearSelection();
    this.btnInPointUp.setEnabled(jsx3.gui.Form.STATEDISABLED, true);
    this.btnInPointDown.setEnabled(jsx3.gui.Form.STATEDISABLED, true);
    this.btnOutPointUp.setEnabled(jsx3.gui.Form.STATEDISABLED, true);
    this.btnOutPointDown.setEnabled(jsx3.gui.Form.STATEDISABLED, true);
    this.btnAddSegment.setEnabled(jsx3.gui.Form.STATEDISABLED, true);
  }

  /** @private @jsxobf-clobber */
  MediaSegments_prototype._onClick_Cancel = function() {
    this.close();
  }

  /** @private @jsxobf-clobber */
  MediaSegments_prototype._onClick_Done = function() {
    //Check Title duplication
    if(this._validateMediaName()) {
      //get segment list from matrix
      var objXML = this.mtxMediaSegmentsList.getXML();
      var xslfile = this.getServer().loadInclude("jsxuser:///JSXAPPS/IPVS_AuthTool/xsl/MediaSegmentListCDFtoXML.xsl", "MediaSegmentList", "xsl");
      // genrate the media segmentlist xml
      var xslDoc = jsx3.xml.XslDocument.wrap(xslfile);
      var mediaSegmentListNode = xslDoc.transformToObject(objXML);
      var strCutPoints = "";
      var trimLength = 0;
      var segmentIter = mediaSegmentListNode.selectNodeIterator("//SEGMENTLIST/SEGMENT");
      while(segmentIter.hasNext()) {
        segment = segmentIter.next();
        strCutPoints = strCutPoints + "," + segment.getAttribute("inPoint") + ":" + segment.getAttribute("outPoint");
        trimLength = trimLength + parseInt(segment.getAttribute("outPoint") - segment.getAttribute("inPoint"));
      }

      var mediaNode = this.mediaInfoNode.cloneNode();
      if(!jsx3.util.strEmpty(strCutPoints)) {
        strCutPoints = strCutPoints.replace(/^,*(.*)/, '$1');
        mediaNode.setAttribute("jsxPlayLength", trimLength);
      }
      //if it is add new media to project then generate new UUID
      if(this.openMode == "ADD") {
        var newJSXID = getUUID().toLowerCase();
        mediaNode.setAttribute("jsxid", newJSXID);
      }
      mediaNode.setAttribute("jsxName", this.txtMediaTitle.getValue());
      mediaNode.setAttribute("jsxselected", "0");
      //add segmentslist node to media
      mediaNode.appendChild(mediaSegmentListNode.cloneNode(true));
      var mediaLength = 0;
      var mediaSize = 0;
      //check if the node is type GROUP
      //Then add iter all clips in the group
      if(this.mediaNodeType == "GROUP") {
        var mediaClipsListXML = this.mtxMediaClipsList.getXML();
        var recIter = mediaClipsListXML.selectNodeIterator(".//record");
        while(recIter.hasNext()) {
          var clipNode = recIter.next().cloneNode();
          var mediaClipNode = this.mediaInfoNode.selectSingleNode("//record[@jsxid='" + clipNode.getAttribute("jsxid") + "']").cloneNode();
          var is2dotXMedia = mediaClipNode.getAttribute("jsxIsTwodotXMedia");
          if(this.openMode == "ADD") {
            mediaClipNode.setAttribute("jsxid", getUUID().toLowerCase());
            mediaClipNode.setAttribute("jsxGrpNID", newJSXID);
          }
          mediaClipNode.setAttribute("jsxName", clipNode.getAttribute("jsxName"));
          mediaClipNode.setAttribute("jsxselected", "0");
          //Check check box for a clip is selected to include in group after edit
          if(clipNode.getAttribute("jsxChkColumn") == jsx3.gui.CheckBox.CHECKED) {
            mediaClipNode.setAttribute("jsxActive", true);
            mediaClipNode.setAttribute("jsxstyle", "display:block");
            //Update playLength if clip is segmented
            if(!jsx3.util.strEmpty(strCutPoints)) {
              mediaClipNode.setAttribute("jsxPlayLength", trimLength);
              mediaLength = mediaLength + trimLength;
            }
            else {
              mediaLength = mediaLength + parseInt(mediaClipNode.getAttribute("jsxPlayLength"));
            }
            //get the size for selected segment portion of clip
            if(is2dotXMedia == "true") {
              var mkdFilePath = mediaClipNode.getAttribute("jsxBasePath") + "\\blob\\" + mediaClipNode.getAttribute("jsxFileNID");
            }
            else {
              var mkdFilePath = mediaClipNode.getAttribute("jsxBasePath") + "\\" + mediaClipNode.getAttribute("jsxFileNID") + ".mkd";
            }
            var trimmedSize = this.blkAddFiles.getInfoFromMKD(mkdFilePath, "size", strCutPoints, is2dotXMedia);
            mediaClipNode.setAttribute("jsxSize", trimmedSize);
            mediaSize = mediaSize + parseInt(trimmedSize);
          }
          else {
            mediaClipNode.setAttribute("jsxActive", false);
            mediaClipNode.setAttribute("jsxstyle", "display:none");
          }
          mediaClipNode.setAttribute("isRoot", false);
          //Append segment list to the clip
          mediaClipNode.appendChild(mediaSegmentListNode.cloneNode(true));
          //Append the clip to mediaNode
          mediaNode.appendChild(mediaClipNode);
        }
        //update the size in group
        mediaNode.setAttribute("jsxSize", mediaSize);
      }
      //Otherwise add segment list for clip node
      else {
        mediaNode.setAttribute("jsxActive", true);
        mediaNode.setAttribute("isRoot", "");

        //Update playLength if clip is segmented
        if(!jsx3.util.strEmpty(strCutPoints)) {
          mediaLength = mediaLength + trimLength;
        }
        else {
          mediaLength = mediaLength + parseInt(mediaNode.getAttribute("jsxPlayLength"));
        }

        var is2dotXMedia = mediaNode.getAttribute("jsxIsTwodotXMedia");
        //get the size for selected segment portion of clip
        if(is2dotXMedia == "true") {
          var mkdFilePath = mediaNode.getAttribute("jsxBasePath") + "\\blob\\" + mediaNode.getAttribute("jsxFileNID");
        }
        else {
          var mkdFilePath = mediaNode.getAttribute("jsxBasePath") + "\\" + mediaNode.getAttribute("jsxFileNID") + ".mkd";
        }
        var trimmedSize = this.blkAddFiles.getInfoFromMKD(mkdFilePath, "size", strCutPoints, is2dotXMedia);
        mediaNode.setAttribute("jsxSize", trimmedSize);
        mediaSize = mediaSize + parseInt(trimmedSize);
      }
      //Add media gropu/clip node to project list
      this.blkAddFiles.processEditedFile(mediaNode, mediaLength, mediaSize, this.openMode);
      this.close();
    }
  }

  /** @private @jsxobf-clobber */
  MediaSegments_prototype._validateMediaName = function() {
    var mediaTitle = this.txtMediaTitle.getValue();
    if(this.mediaNodeType == "GROUP") {
      //check group name duplication with clip names
      var mediaClipsListXML = this.mtxMediaClipsList.getXML();
      if(!jsx3.util.strEmpty(mediaClipsListXML.selectSingleNode("//record[@jsxName='" + mediaTitle + "']"))) {
        alert("Same title is not allowed for group and clip(s)");
        return false;
      }
      //check clips with in the group has same name.
      var recIter = mediaClipsListXML.selectNodeIterator(".//record");
      while(recIter.hasNext()) {
        var clipNode = recIter.next();
        var clipjsxid = clipNode.getAttribute("jsxid");
        var jsxName = clipNode.getAttribute("jsxName");
        if(!jsx3.util.strEmpty(mediaClipsListXML.selectSingleNode("//record[@jsxName='" + jsxName + "' and @jsxid!='" + clipjsxid + "']"))) {
          alert("Duplicate names not allowed for clips");
          return false;
        }
        //Check if same title exists in project
        else if(!this.blkAddFiles.validateMediaName(jsxName, (this.openMode == "ADD" ? null : clipjsxid))) {
          alert("Media already exists with title '" + jsxName + "'.");
          return false;
        }
      }
    }

    var jsxid = this.mediaInfoNode.getAttribute("jsxid");
    //Check if same title exists in project
    if(!this.blkAddFiles.validateMediaName(mediaTitle, (this.openMode == "ADD" ? null : jsxid))) {
      alert("Media already exists with title '" + mediaTitle + "'.");
      return false;
    }
    return true;
  }
});