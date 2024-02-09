/* ============================================================================
 * MediaPlayerPanel Class
 * Copyright (c) Barco Inc.
 * Use, modification, and distribution subject to terms of license.
 * ========================================================================= */
var SourcePanel = Class.extend({
  _defaults: {
  },
  viewState: false,

  /**
  * init function 
  */
  init: function (options, elem) {
    this.options = jQuery.extend({}, this._defaults, options);
    this.log(bINFO, "init", "call", "");
    this.elem = elem;
    this.$elem = jQuery(elem);
    this._build();
    this.activeTab = 0;
    this.$elem.find("#source-panel #tabs").tabs({ activate: jQuery.proxy(this._ontabActivate, this) });
    this.$liveSrcListCtn = this.$elem.find("#live-source-list-container");
    this.$liveSrcList = this.$liveSrcListCtn.edp_sourceList(jQuery.extend({}, this.options, { callBack: jQuery.proxy(this._onLiveSrcSelected, this) })).data("edp_sourceList");
    this.$recordingsListCtn = this.$elem.find("#recorded-source-list-container");
    this.$recordingsList = this.$recordingsListCtn.edp_recordingsList(jQuery.extend({}, this.options, { callBack: jQuery.proxy(this._onRecSrcSelected, this) })).data("edp_recordingsList");
    this.$searchPanelInst = this.$elem.find("#sourceListToolbar").edp_searchPanel(jQuery.extend({}, this.options, { callBack: jQuery.proxy(this._onSearchFilterSource, this) })).data('edp_searchPanel');
    this.$elem.find("#btnRefresh").on('click', jQuery.proxy(this._onClickRefresh, this));
  },

  /**
  * _build function 
  */
  _build: function () {
    var html = '<div id="source-panel" class="source-list">' +
					      '<div id="tabs" style="position:absolute;top:0;bottom:0;width:100%;border-radius:0;background:none;margin:0;padding:0;overflow:hidden">' +
						      '<ul style="background:#30363a;border:0;padding:0;margin:0;border-radius:0;">' +
                    '<li><a href="#live-source-list-container"><span>Live</span></a></li>' +
						        '<li><a href="#recorded-source-list-container"><span>Recordings</span></a></li>' +
						      '</ul>' +
                  '<div id="sourceListToolbar" class="source-list-toolbar">' +
                    '<div class="pull-right">' +
                      '<a id="btnRefresh" href="#"><i class="fa fa-refresh"></i></a>' +
                    '</div>' +
                  '</div>' +
						      '<div id="live-source-list-container" style="position:absolute;top:80px;bottom:0;width:100%;padding:10px 0;overflow-x:hidden;overflow-y:auto"></div>' +
						      '<div id="recorded-source-list-container" style="position:absolute;top:80px;bottom:0;width:100%;padding:10px 0;overflow-x:hidden;overflow-y:auto"></div>' +
					      '</div>' +
               '</div>';
    jQuery(html).appendTo(this.$elem);
  },

  /**
  * setPanel function 
  */
  setPanel: function () {
    this.log(bINFO, "setPanel", "call", "");
    if (!this.viewState) {
      this.viewState = true;
      this.$liveSrcList.setList();
      this.$recordingsList.setList();
      this.$searchPanelInst.setPanel();
    }
  },

  /**
  * resetPanel function 
  */
  resetPanel: function () {
    if (this.viewState) {
      this.viewState = false;
      this.$elem.find("#source-panel #tabs").tabs("option", "active", 0);
      this.$liveSrcList.resetList();
      this.$recordingsList.resetList();
      this.$searchPanelInst.resetPanel();
    }
  },

  /**
  * destroy function 
  */
  destroy: function () {
    this.resetPanel();
    this.$liveSrcList.destroy();
    this.$recordingsList.destroy();
    this.$searchPanelInst.destroy();
    this.$elem.removeData('edp_sourcePanel');
  },

  /**
  * show function 
  */
  show: function () {
    this.$elem.removeClass('hidden');
  },

  /**
  * hide function 
  */
  hide: function () {
    this.$elem.addClass('hidden');
  },


  /**
  * _ontabActivate function 
  */
  _ontabActivate: function (event, ui) {
    if (!this.$searchPanelInst.isAdvanceHidden()) this.$searchPanelInst.hide();
    this.activeTab = this.$elem.find("#source-panel #tabs").tabs("option", "active");
    var isLiveTab = this.activeTab == 0 ? true : false;
    this.$searchPanelInst.configSearchFilterPanel(isLiveTab);
    if (isLiveTab) {
      this.$liveSrcList.setList();
    }
    else {
      this.$recordingsList.setList();
    }
  },

  /**
  * _onLiveSrcSelected function 
  */
  _onLiveSrcSelected: function (objPort) {
    var portInfo = objPort;
    if (this.options.callBack)
      this.options.callBack("LIVE", objPort);
  },


  /**
  * _onRecSrcSelected function 
  */
  _onRecSrcSelected: function (objFile) {
    var fileInfo = objFile;
    if (this.options.callBack)
      this.options.callBack("RECORDED", objFile);
  },


  /**
  * deselect function 
  */
  deselect: function () {
    this.$liveSrcList.deselect();
    this.$recordingsList.deselect();
  },


  /**
  * _onClickRefresh function 
  */
  _onClickRefresh: function () {
    if (this.viewState) {
      if (this.activeTab == 0) {
        this.$liveSrcList.getSourceList();
      }
      else if (this.activeTab == 1) {
        this.$recordingsList.getFileList();
      }
      this.$searchPanelInst.resetBasicSearch();
    }
  },


  /**
  * _onSearchFilterSource function 
  * @param cbData {JSON}, {mode:"QUERY",data:whereClause}|{mode:"DISPLAY",data:true|flase}
  */
  _onSearchFilterSource: function (cbData) {
    this.log(bINFO, "_onSearchFilterSource", "call", "");
    if (cbData.mode == "QUERY") {
      if (this.activeTab == 0) {
        this.$liveSrcList.getSourceList(cbData.data);
      } else {
        this.$recordingsList.getFileList(cbData.data);
      }
    } else if (cbData.mode == "DISPLAY") {
      if (cbData.data) {
        this.$elem.find('#btnRefresh').removeClass('hidden');
      } else {
        this.$elem.find('#btnRefresh').addClass('hidden');
      }
    }
  }

});
// expose Objects as jQuery plugins
jQuery.plugin('edp_sourcePanel', SourcePanel);
