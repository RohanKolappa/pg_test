/*
* Copyright (c) IP Video Systems Inc.
* Use, modification, and distribution subject to terms of license.
*/

/**
* This class uses jsx3.net.Form.
*/
jsx3.require("jsx3.gui.Block","jsx3.net.Form");
jsx3.Class.defineClass("com.ipvs.gui.FilePicker", jsx3.gui.Block, null, function(FilePicker, FilePicker_prototype) {

  /** @private @jsxobf-clobber */
  FilePicker_prototype._LOG = jsx3.util.Logger.getLogger(FilePicker.jsxclass.getName());

  FilePicker.ON_FILE_SELECT = "ONFILESELECTED";
  /**
  * The instance initializer.
  */
  FilePicker_prototype.init = function() {
    this.txtFilePath = this.getDescendantOfName("txtFilePath");
    this.btnBrowse = this.getDescendantOfName("btnBrowseForFile");
    this.btnBrowse.subscribe(jsx3.gui.Interactive.EXECUTE, this, this._doBrowse);
    this._filePathFieldName = "txtFilePath";
    this._objFile = new jsx3.net.Form( );
    this._objFile.addFileUploadField(this._filePathFieldName);
    this._objFile.subscribe(jsx3.net.Form.EVENT_FILE_SELECTED, this, this.onFileSelected);
  }

  /**
   * Returns the selected file path.
   * @return {String}.
   */
  FilePicker_prototype._doBrowse = function() {
    this._objFile.promptForFile(this._filePathFieldName);
  }

  /**
   * Gets the FilePath
   * @return {String}
   */
  FilePicker_prototype.getFilePath = function() {
    return this.FilePath;
  }
  
  /**
   * Set the File path
   * @param fPath {String}
   */
  FilePicker_prototype.setFilePath = function(fPath) {
    this.txtFilePath.setValue(fPath);
    this.FilePath = fPath;
  }
  
  /**
   * Set the File location
   * @param objEvent {String}
   */
  FilePicker_prototype.onFileSelected = function(objEvent) { 
    this.setFilePath(objEvent.value);
    this.doEvent(FilePicker.ON_FILE_SELECT, {objEVENT:objEvent});
  }

  /**
   * Called during deserialization of this object. This method provides a hook for initializing
   * an object during deserialization since init() is not called. Called after this object has been instantiated and
   * after its fields and children have been assembled.This method is called before this object is attached to the
   * DOM, therefore &lt;code&gt;getParent()&lt;/code&gt;, &lt;code&gt;getServer()&lt;/code&gt;, &lt;code&gt;getXML()&lt;/code&gt;, etc. return &lt;code&gt;null&lt;/code&gt;.
   * @param objParent {jsx3.app.Model} the parent of this object once it is attached to the DOM.
   * @param objServer {jsx3.app.Server} the server that this DOM object will attach to.
   * @protected
   */
  FilePicker_prototype.onAfterAssemble = function(objParent, objServer) {
    this.init();
  };

});