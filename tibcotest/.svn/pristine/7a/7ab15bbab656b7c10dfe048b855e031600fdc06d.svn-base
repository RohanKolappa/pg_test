
function dlgMsg_Create(dlgMode) {
  var Message = "";
  var dlgPrompt = null; 
  var cmpDlgHtmlSrc = null;
  if( dlgMode == "FatalError" ) {
    Message = "<b>Fatal Internal Error:</b><br> Please shutdown and restart the application." + "<br>";
    Message += "Please click 'GenerateLog' button to generate log file or 'OK' to close dialog." + "<br>";
    dlgPrompt = PNameSpace.confirm("Fatal Error Dialog",
                      Message, dlg_Destroy, cmpDlgGenerateLog, 
                      "OK", "GenerateLog", { width: 430, height: 220 });
  }
  else if( dlgMode == "Prompt" ) {
    Message = "<b>you have requested a log dump. </b> <br> This could take a while." + "<br>";
    Message += "If you want to continue click 'GenerateLog' button or 'Cancel' to close dialog." + "<br>";
    dlgPrompt = PNameSpace.confirm("Prompt",
                      Message, dlg_Destroy, cmpDlgGenerateLog, 
                       "Cancel", "GenerateLog", { width: 430, height: 220 });
  }
  else if (dlgMode == "RenderHtml") {
    Message = "Please input a GI component name otherwise it will return current page HTML source." + "<br><br>";
    dlgPrompt = PNameSpace.prompt("Rendered HTML Source",
                        Message, dlg_Destroy, cmpDlgSetMessage,
                        "Close", "GetSource", { width: 430, height: 220 });

  }
  
  function cmpDlgGenerateLog() {
    handleMemoryHandler();
    dlg_Destroy();
  }
  function dlg_Destroy() {
    dlgPrompt.doClose();
    dlgPrompt = null;
  }

  // fnc to get dlg data
  function cmpDlgSetMessage() {
    //get input value from jsx3.gui.Alerts.prompt
    var inputCmpName = dlgPrompt.getDescendantOfName('value').getValue();
    var cmpName = jsx3.util.strEmpty(inputCmpName) ? "JSXBODY" : inputCmpName;
    var cmpSrcBlkJSX = PNameSpace.getJSXByName(cmpName);
    if (cmpSrcBlkJSX) {
      var cmpSrc = null;
      if (cmpSrcBlkJSX instanceof jsx3.gui.Window) {
        cmpSrc = cmpSrcBlkJSX.getRootBlock().getRendered();
      }
      else {
        cmpSrc = cmpSrcBlkJSX.getRendered();
      }  
      var renderedHtml = cmpSrc ? cmpSrc.innerHTML : "EMPTY";
      Message = jsx3.util.strEscapeHTML(renderedHtml);
      dlg_Destroy();
      dlgPrompt = PNameSpace.alert("Select block:'" + cmpName + "' rendered HTML source:",
                      Message, dlg_Destroy, "Close", { width: 530, height: 450 });
    }
    else {
      dlg_Destroy();
      Message = "A wrong block name or the block not being rendered yet. Try again with a correct block name.";
      dlgPrompt = PNameSpace.alert("Error block name: '" + cmpName + "'",
                      Message, dlg_Destroy, "Close", { width: 230, height: 180 });
    }
  }
}