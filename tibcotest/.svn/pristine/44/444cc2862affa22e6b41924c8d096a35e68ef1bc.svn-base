/* place JavaScript code here */
jsx3.require("jsx3.gui.Block");

jsx3.Class.defineClass("com.ipvs.gui.authmediaclient.Controller", jsx3.gui.Block, [com.ipvs.util.IPVSComponentI], function (Controller, Controller_prototype) {
  Controller_prototype._LOG = jsx3.util.Logger.getLogger(Controller.jsxclass.getName());

  Controller.CONTROL_SELECT = "CONTROLSELECTED";
  Controller.CONTROL_REWIND = "REWIND";
  Controller.CONTROL_PLAY = "PLAY";
  Controller.CONTROL_PAUSE = "PAUSE";
  Controller.CONTROL_STOP = "STOP";
  Controller.CONTROL_FASTFORWARD = "FASTFORWARD";
  Controller.CONTROL_WINFULLSCREEN = "WINFULLSCREEN";
  Controller.CONTROL_WIN1TO1 = "WIN1TO1";
  Controller.CONTROL_WINEXPAND = "WINEXPAND";
  Controller.CONTROL_WINMUTE = "WINMUTE";
  Controller.CONTROL_WINDELETE = "WINDELETE";

  Controller_prototype.init = function () {
    this.ControlSelected = null;

    this.btnRewind = this.getDescendantOfName("btnRewind");
    this.btnPlay = this.getDescendantOfName("btnPlay");
    this.btnPause = this.getDescendantOfName("btnPause");
    this.btnStop = this.getDescendantOfName("btnStop");
    this.btnFastforward = this.getDescendantOfName("btnFastforward");
    this.FPSSlider = this.getDescendantOfName("FPSSlider");
    this.lblTxtFPS = this.getDescendantOfName("lblTxtFPS");
    this.window_fullscreen = this.getDescendantOfName("window_fullscreen");
    this.window_1_to_1 = this.getDescendantOfName("window_1_to_1");
    this.window_expand = this.getDescendantOfName("window_expand");
    this.window_mute = this.getDescendantOfName("window_mute");
    this.window_delete = this.getDescendantOfName("window_delete");

    this.btnRewind.subscribe(jsx3.gui.Interactive.EXECUTE, this, this._onClick_Rewind);
    this.btnPlay.subscribe(jsx3.gui.Interactive.EXECUTE, this, this._onClick_Play);
    this.btnPause.subscribe(jsx3.gui.Interactive.EXECUTE, this, this._onClick_Pause);
    this.btnStop.subscribe(jsx3.gui.Interactive.EXECUTE, this, this._onClick_Stop);
    this.btnFastforward.subscribe(jsx3.gui.Interactive.EXECUTE, this, this._onClick_FF);
    this.FPSSlider.subscribe(jsx3.gui.Interactive.CHANGE, this, this._onClick_Slider);

    this.window_fullscreen.subscribe(jsx3.gui.Interactive.EXECUTE, this, this._onClick_WinFullSscreen);
    this.window_1_to_1.subscribe(jsx3.gui.Interactive.EXECUTE, this, this._onClick_Win1to1);
    this.window_expand.subscribe(jsx3.gui.Interactive.EXECUTE, this, this._onClick_WinExpand);
    this.window_mute.subscribe(jsx3.gui.Interactive.EXECUTE, this, this._onClick_WinMute);
    this.window_delete.subscribe(jsx3.gui.Interactive.EXECUTE, this, this._onClick_WinDelete);


    this.blkApp = this.getAncestorOfType("com.ipvs.authmediaclient.App");

  }

  Controller_prototype.getControllerSelected = function () {
    return this.ControlSelected;
  }

  Controller_prototype._setControllerSelection = function (objEvent, controlSelected, bPublish) {
    this.ControlSelected = controlSelected;
    if (bPublish != false) this.doEvent(Controller.CONTROL_SELECT, { objEVENT: objEvent });
  }

  Controller_prototype._onClick_Rewind = function (objEvent) {
    this._setControllerSelection(objEvent, Controller.CONTROL_REWIND);
  }

  Controller_prototype._onClick_Play = function (objEvent) {
    this._setControllerSelection(objEvent, Controller.CONTROL_PLAY);
  }


  Controller_prototype._onClick_Pause = function (objEvent) {
    this._setControllerSelection(objEvent, Controller.CONTROL_PAUSE);
  }


  Controller_prototype._onClick_Stop = function (objEvent) {
    this._setControllerSelection(objEvent, Controller.CONTROL_STOP);
  }


  Controller_prototype._onClick_FF = function (objEvent) {
    this._setControllerSelection(objEvent, Controller.CONTROL_FASTFORWARD);
  }


  Controller_prototype._onClick_Slider = function (objEvent) {

  }


  Controller_prototype._onClick_WinFullSscreen = function (objEvent) {

  }


  Controller_prototype._onClick_Win1to1 = function (objEvent) {

  }


  Controller_prototype._onClick_WinExpand = function (objEvent) {

  }


  Controller_prototype._onClick_WinMute = function (objEvent) {

  }


  Controller_prototype._onClick_WinDelete = function (objEvent) {

  }


  Controller_prototype.destroy = function () {

  }

});