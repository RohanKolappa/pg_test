/*
* Copyright (c) IP Video Systems Inc.
* Use, modification, and distribution subject to terms of license.
*/

/**
* Pagination class.
* <p/>
*/
jsx3.require("jsx3.gui.Block");
jsx3.Class.defineClass("com.ipvs.gui.Pagination", jsx3.gui.Block, null, function(Pagination, Pagination_prototype) {

  /** @private @jsxobf-clobber */
  Pagination_prototype._LOG = jsx3.util.Logger.getLogger(Pagination.jsxclass.getName());

  Pagination_prototype.onAfterAttach = function() {
    this.jsxsuper();
    this.clsApp = com.ipvs.app.App.clsApp;
    if(!this.clsApp) return;
    if(this.clsApp.INITED) {
      this._onAppInit();
    }
    else {
      this.clsApp.subscribe(com.ipvs.app.App.APP_INIT, this, this._onAppInit);
    }
  }
  Pagination_prototype._onAppInit = function() {
  }
  Pagination_prototype.initPagination = function(objMtx) {
    if(jsx3.util.strEmpty(objMtx)) return;
    this.btnFirst = this.getDescendantOfName("btnPageFirst");
    this.btnPrev = this.getDescendantOfName("btnPagePrev");
    this.btnRefresh = this.getDescendantOfName("btnPageRefresh");
    this.btnNext = this.getDescendantOfName("btnPageNext");
    this.btnLast = this.getDescendantOfName("btnPageLast");
    this.Mtx = objMtx;
    this.Mtx.subscribe("PaginationListUpdated", this, this._handleControls);
    this.btnFirst.subscribe(jsx3.gui.Interactive.EXECUTE, this, this._onClick_First);
    this.btnPrev.subscribe(jsx3.gui.Interactive.EXECUTE, this, this._onClick_Prev);
    this.btnRefresh.subscribe(jsx3.gui.Interactive.EXECUTE, this, this._onClick_Refresh);
    this.btnNext.subscribe(jsx3.gui.Interactive.EXECUTE, this, this._onClick_Next);
    this.btnLast.subscribe(jsx3.gui.Interactive.EXECUTE, this, this._onClick_Last);
    this._handleControls("INIT");
  }

  Pagination_prototype._handleControls = function(objEvent) {
    if(this.Mtx.StartFrom > 1) {
      this.btnFirst.setEnabled(jsx3.gui.Form.STATEENABLED, true);
      this.btnPrev.setEnabled(jsx3.gui.Form.STATEENABLED, true);
    }
    else {
      this.btnFirst.setEnabled(jsx3.gui.Form.STATEDISABLED, true);
      this.btnPrev.setEnabled(jsx3.gui.Form.STATEDISABLED, true);
    }

    if((parseInt(this.Mtx.StartFrom) + parseInt(this.Mtx.CountToFetch)) <= parseInt(this.Mtx.ObjectCount)) {
      this.btnNext.setEnabled(jsx3.gui.Form.STATEENABLED, true);
      this.btnLast.setEnabled(jsx3.gui.Form.STATEENABLED, true);
    }
    else {
      this.btnNext.setEnabled(jsx3.gui.Form.STATEDISABLED, true);
      this.btnLast.setEnabled(jsx3.gui.Form.STATEDISABLED, true);
    }
    if(objEvent == "INIT") {
      this.btnRefresh.setEnabled(jsx3.gui.Form.STATEDISABLED, true);
    }
    else {
      this.btnRefresh.setEnabled(jsx3.gui.Form.STATEENABLED, true);
    }
  }

  Pagination_prototype._onClick_First = function() {
    if(!jsx3.util.strEmpty(this.onBeforeFirst)) this.onBeforeFirst();
    this.Mtx.StartFrom = 1;
    this.Mtx.setList();
  }

  Pagination_prototype._onClick_Prev = function() {
    if(!jsx3.util.strEmpty(this.onBeforePrev)) this.onBeforePrev();
    var prevStartFrom = parseInt(this.Mtx.StartFrom);
    var countToFetch = parseInt(this.Mtx.CountToFetch);
    var objectCount = parseInt(this.Mtx.ObjectCount);

    var nextStartFrom = prevStartFrom - countToFetch;
    if(nextStartFrom <= 0) {
      nextStartFrom = 1;
    }
    this.Mtx.StartFrom = nextStartFrom;
    this.Mtx.setList();
  }

  Pagination_prototype._onClick_Refresh = function() {
    if(!jsx3.util.strEmpty(this.onBeforeRefresh)) this.onBeforeRefresh();
    this.Mtx.setList();
  }

  Pagination_prototype._onClick_Next = function() {
    if(!jsx3.util.strEmpty(this.onBeforeNext)) this.onBeforeNext();
    var prevStartFrom = parseInt(this.Mtx.StartFrom);
    var countToFetch = parseInt(this.Mtx.CountToFetch);
    var objectCount = parseInt(this.Mtx.ObjectCount);

    var nextStartFrom = prevStartFrom + countToFetch;
    if(nextStartFrom > objectCount) {
      nextStartFrom = objectCount;
    }
    this.Mtx.StartFrom = nextStartFrom;
    this.Mtx.setList();
  }

  Pagination_prototype._onClick_Last = function() {
    if(!jsx3.util.strEmpty(this.onBeforeLast)) this.onBeforeLast();
    var prevStartFrom = parseInt(this.Mtx.StartFrom);
    var countToFetch = parseInt(this.Mtx.CountToFetch);
    var objectCount = parseInt(this.Mtx.ObjectCount);

    // Modulus (%) operator returns only the remainder. means last number of records to fetch
    var lastCountToFetch = objectCount % countToFetch;
    if(lastCountToFetch == 0) {
      lastCountToFetch = countToFetch;
    }
    var nextStartFrom = objectCount - lastCountToFetch + 1;
    this.Mtx.StartFrom = nextStartFrom;
    this.Mtx.setList();
  }

});