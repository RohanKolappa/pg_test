function dlgViewFiles_Create(MediaDirNID, ViewDir) {
  // load view - dialog component file
  var DlgFile = "";
  var DlgName = "";
  if( ViewDir == "RECYCLEBIN" ) {
    DlgFile = "components/media/mediaRecycleBinDialog.xml";
    DlgName = "DialogRecycleBinFiles";
  }
  else if( ViewDir == "PVR" ) {
    DlgFile = "components/media/mediaPVRstorageDialog.xml";
    DlgName = "DialogPVRFiles";
  }
  else if( ViewDir == "ORPHAN" ) {
    DlgFile = "components/media/mediaOrphanFilesDialog.xml";
    DlgName = "DialogOrphanFiles";
  }  
  else {
    return;
  }

  var EmptyDirState = topic_Get(MediaDirNID, "EmptyDirState");
  if(EmptyDirState == null){
    closeDialogIfExists(DlgName);
  }
  else{
    if (jsx3.gui.Dialog) {
      try {
        var dialogBoxArr = PNameSpace.getBodyBlock().getDescendantsOfType(jsx3.gui.Dialog, false);
        for (var i = 0; i < dialogBoxArr.length; i++) {
          var lcDlgName = dialogBoxArr[i].getName();
          if (lcDlgName == DlgName) {
            dialogBoxArr[i].focus();
            return;
          }
        }
      } catch (e) {
        log.warn("Dialog error:" + e.description);
      }
    }
  }
  
  // setup a local topic for dlg specific messaging
  var tDlg = MediaDirNID;
  if (topic_Exists(tDlg)) {
    topic_Destroy(tDlg);
  }

  topic_Create(tDlg);
  topic_Set(tDlg, "SelectedDirTitle", ViewDir);
  // setup a listner for it
  var eventDlg = new EventListner(tDlg);

  var BlkJSX = PNameSpace.getBodyBlock().load(DlgFile);

  function dlgViewFiles_Destroy() {
    //unsubscribe events which subscribed in tApp for this dialog
    var updateEventID = topic_Get(tDlg, "MediaSFRUpdateEventID");
    var addEventID = topic_Get(tDlg, "MediaSFRAddEventID");
    var deleteEventID = topic_Get(tDlg, "MediaSFRDeleteEventID");
    var agentEventEventID = topic_Get(tDlg, "AgenteventEventID");
    var MediaSFRDBUpdateEventID = topic_Get(tDlg, "MediaSFRDBUpdateEventID");

    eventApp.unsubscribe("MediaSFRDataUpdateNode", updateEventID);
    eventApp.unsubscribe("MediaSFRDataAddNode", addEventID);
    eventApp.unsubscribe("MediaSFRDataDeleteNode", deleteEventID);
    eventApp.unsubscribe("AGENTEVENT", agentEventEventID);
    eventApp.unsubscribe("DBUpdate", MediaSFRDBUpdateEventID);

    // destroy any local eventlistners
    eventDlg.destroy();
    
    // destroy all local components
    cmpDlgViewFiles.destroy();
    
    // destroy the local topic
    topic_Destroy(tDlg);
    
    // close dialog
    BlkJSX.doClose();
  }
        
  var cmpMediaList = cmpBlkCommonMedia_Create(tDlg, eventDlg, BlkJSX, ViewDir);

  var cmpBtnViewFilesClose = new CmpButton([BlkJSX, "btnDialogCloseX"],
                                               null, 
                                               {action:"FNCCALL", 
                                                context:null, 
                                                fnc:function() {
                                                  var EmptyDirState = topic_Get(tDlg, "EmptyDirState");
                                                  if( EmptyDirState == "NONE" ) {
                                                    var agree = confirm('Are you sure you want to STOP empty Media Directory Process? ');
                                                    if(agree) {
                                                      eventDlg.subscribe("EmptyDirState", "ENABLE", this,
                                                                         function() {
                                                                           dlgViewFiles_Destroy();
                                                                         });
                                                      topic_Publish(tDlg, "EmptyDir", MediaDirNID);
                                                    }
                                                  }
                                                  else {
                                                    dlgViewFiles_Destroy();
                                                  }
                                                }
                                               });

  var cmpDlgViewFiles = new CmpContainer(BlkJSX,
                                        [ {cmp:cmpMediaList},
                                          {cmp:cmpBtnViewFilesClose}
                                        ],
                                        null);
                                                            
  cmpDlgViewFiles.BlkJSX = BlkJSX;
  cmpDlgViewFiles.getBlkData =  cmpDlgViewFiles_GetFiles;
      
  // init and repaint happens after the dialog is loaded
  jsx3.sleep( (function(){
                 this.init();
                 this.getBlkData();
                 }
               ),
               null, cmpDlgViewFiles ); 
               
  function cmpDlgViewFiles_GetFiles() {
    var parentDirOrphanState = topic_Get(tApp, "MediaSDROrphan");
    topic_Publish(tDlg, "MediaSDROrphan", parentDirOrphanState);
    topic_Publish(tDlg, "MediaSDRSelected", MediaDirNID);
    topic_Publish(tDlg, "MediaSFRGetData", MediaDirNID);
  }
}