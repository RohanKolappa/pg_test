function dlgSessionMonitor_Create(cmpMediaController, tEndPointSession, eventEndPointSession, sessionID, sessionName){
  var DlgFile = "components/dlgSessionMonitor.xml";
  var dlgHeaderName = "Session Name:" + sessionName + "   ID:" + sessionID;
  // check if the dialog is already open
  var objParent = PNameSpace.getServer().getJSXByName("blkMediaControllerBody");
  
  if (jsx3.gui.Dialog){
    try {
      var dialogBoxArr = objParent.getDescendantsOfType(jsx3.gui.Dialog, false);
      for (var i = 0; i < dialogBoxArr.length; i++) {
        var dlgName = dialogBoxArr[i].getName();
        if( dlgName == sessionID ) {
          topic_Publish(tEndPointSession, "SessionConnData", (newServerDate()).getTime() );
          dialogBoxArr[i].focus();
          return ;
        }
      }
    } catch (e) {
    }
  }  
  
  var objDlg = objParent.load(DlgFile);
  objDlg.setName(sessionID);
  objDlg.getChild("windowBar").setTip(dlgHeaderName,"true");
  objDlg.getChild("windowBar").setText(dlgHeaderName,"true");
 
 
  function dlgSessionMonitor_Destroy() {
    // destroy all local components
    cmpDlgSessionMonitor.destroy();
    
    // close dialog
    objDlg.doClose();    
  } // end dlgSessionMonitor_Destroy
  var BlkJSX = objDlg;
  var cmpDlgBtnSessionMonitorClose = new CmpButton([BlkJSX, "btnDialogCloseX"],
                                                   null, 
                                                   {action:"FNCCALL", 
                                                    context:null, 
                                                    fnc:function() {
                                                      dlgSessionMonitor_Destroy();
                                                    }
                                                   }); 
                                                    
  var cmpDlgBlkSessionMonitor = cmpSessionMonitor_Create(BlkJSX, tEndPointSession, eventEndPointSession, "DIALOG");
 
  var cmpDlgSessionMonitor = new CmpContainer([BlkJSX,"DialogSessionMonitor"],
                                              [{cmp:cmpDlgBlkSessionMonitor},
                                               {cmp:cmpDlgBtnSessionMonitorClose}],
                                              [{action:"TOPICEVENT", topic:tEndPointSession, eventvar:"SessionConnData", eventval:null}]
                                              );

  cmpDlgSessionMonitor.BlkJSX = BlkJSX;
  cmpDlgSessionMonitor.sessionID = sessionID;
  cmpDlgSessionMonitor.getBlkData = cmpDlgSessionMonitor_Repaint;


  // init and repaint happens after the dialog is loaded
  jsx3.sleep( (function(){
               this.init();
               //this.show();
               this.getBlkData();
               }
             ),
             null, cmpDlgSessionMonitor );

  function cmpDlgSessionMonitor_Repaint(){
    topic_Publish(tEndPointSession, "SessionConnData", (newServerDate()).getTime() );
  } // end cmpDlgSessionMonitor_Repaint


} // end dlgSessionMonitor_Create