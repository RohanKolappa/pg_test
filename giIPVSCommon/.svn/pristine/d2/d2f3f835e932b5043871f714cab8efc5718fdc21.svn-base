/*
* Copyright (c) IP Video Systems Inc.
* Use, modification, and distribution subject to terms of license.
*/
jsx3.Class.defineClass('com.ipvs.util.HandleMemory', null, null, function(HandleMemory, HandleMemory_prototype) {

  /** @private @jsxobf-clobber */
  HandleMemory._LOG = jsx3.util.Logger.getLogger("com.ipvs.util.HandleMemory");

  /** * The instance initializer.  */
  HandleMemory_prototype.init = function() {
    this.clsApp = com.ipvs.app.App.clsApp;
    if(! this.clsApp) return;
    if (this.clsApp.INITED) {
      this._onAppInit();
    }
    else {
      this.clsApp.subscribe(com.ipvs.app.App.APP_INIT, this, this._onAppInit);
    } 
  }
  HandleMemory_prototype._onAppInit = function() {
    this.Message = "";
    this.dlgPrompt = null;
    this.jsxBody = null;
    this.browserType = this.clsApp.getBrowserType();
    this.jsxBody = this.clsApp.jsxBody.getParent();
    var intKey = jsx3.gui.HotKey.keyDownCharToCode("d");
    var ThisClass = this;
    this.jsxBody.registerHotKey(function() { HandleMemory_prototype._promptUserToGenLog(ThisClass); }, intKey, false, true, true);
    var intXKey = jsx3.gui.HotKey.keyDownCharToCode("x");
    this.jsxBody.registerHotKey(function() { HandleMemory_prototype._disableMemoryHandler(ThisClass); }, intXKey, false, true, true);
  }

  HandleMemory_prototype._promptUserToGenLog = function(ThisClass) {
    ThisClass._createMessageLogDialog();
  }


  HandleMemory_prototype._disableMemoryHandler = function(ThisClass) {
    var logManager = jsx3.util.Logger.Manager.getManager();
    var memoryHandler = logManager.getHandler("memory");
    memoryHandler.setLevel(jsx3.util.Logger.OFF);
  }

  HandleMemory_prototype._createMessageLogDialog = function() {
    this.Message = "<b>you have requested a log dump. </b> <br> This could take a while." + "<br>";
    this.Message += "If you want to continue click 'GenerateLog' button or 'Cancel' to close dialog." + "<br>";
    var ThisClass = this;
    this.dlgPrompt = this.jsxBody.getServer().confirm("Prompt",
                      this.Message, function() { HandleMemory_prototype._destroyMemoryLogDialog(ThisClass); }, function() { HandleMemory_prototype._generateLog(ThisClass); },
                        "Cancel", "GenerateLog", { width: 430, height: 220 });
  }

  HandleMemory_prototype._generateLog = function(ThisClass) {
    ThisClass._handleMemoryHandler();
    ThisClass._destroyMemoryLogDialog(ThisClass);
  }

  HandleMemory_prototype._destroyMemoryLogDialog = function(ThisClass) {
    ThisClass.dlgPrompt.doClose();
    ThisClass.dlgPrompt = null;
  }

  HandleMemory_prototype._handleMemoryHandler = function() {
    var logManager = jsx3.util.Logger.Manager.getManager();
    var memoryHandler = logManager.getHandler("memory");
    var memoryHandlerRecords = memoryHandler.getRecords();
    var memoryRecordCount = memoryHandlerRecords.length;
    var recString = "---------- MEMORY LOG -----------\n\n";
    for(var recCount = 0;recCount < memoryRecordCount;recCount++) {
      recString += memoryHandlerRecords[recCount].getDate();
      recString += " - ";
      recString += memoryHandlerRecords[recCount].getLoggerName();
      recString += " - ";
      recString += jsx3.util.Logger.levelAsString(memoryHandlerRecords[recCount].getLevel());
      recString += " - ";
      recString += memoryHandlerRecords[recCount].getMessage().replace(/\&lt;/g, '<').replace(/\&gt;/g, '>');
      recString += "\n";
    }
    recString += "\n";

    var dateTimeFormat = new jsx3.util.DateFormat("MM-dd-yyyy-HH-mm-ss");
    var dateTime = dateTimeFormat.format(new Date());
    //call different browser based function  
    if(this.browserType == "MSIE") {
      memoryLogWriteHandler_Msie(dateTime, recString);
    }
    else {
      this._LOG.info("Current browser is not supported for log dump.");
    }
  }
});