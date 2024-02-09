// a topic / listner for messaging in it
var tMediaController = "MediaController_" + jsx3.xml.CDF.getKey(); ;
var tMediaControllerDoc = null;
var eventMediaController = null;
// the top level block
var winMediaController = null;

function cmpWinMediaController_Create(tCaller, parentWindow)
{
  // save the callers topic/name in a gloabl static var
  // this will be used to target events later
//  MediaListCallerID = tCaller;
  
  // create only once
  if( winMediaController != null ) {
    if (parentWindow) {
      //call from 'SessionInfo' -- Collaboration window
      //display Add button only
      topic_Publish(tMediaController, "ConnectOrAdd", "ADD");
    }
    else {
      //call from BlkUI -- Media List window
      //display connect button only 
      topic_Publish(tMediaController, "ConnectOrAdd", "CONNECT");
    }
    // show/focus the window  
    winMediaController.show();
    return;
  }
  
  // create a topic / listner for messaging in it
  tMediaControllerDoc = topic_Create(tMediaController);
  eventMediaController = new EventListner(tMediaController);

  function cmpWinMediaController_Destroy() {
    //destroy any DBUpdate listners
    var DBUpdate_ID = topic_Get(tMediaController, "DBUpdate_ID");
    if (!jsx3.util.strEmpty(DBUpdate_ID)) {
      eventApp.unsubscribe("DBUpdate", DBUpdate_ID);
    }

    var DBUpdate_2_ID = topic_Get(tMediaController, "DBUpdate_2_ID");
    if (!jsx3.util.strEmpty(DBUpdate_2_ID)) {
      eventApp.unsubscribe("DBUpdate", DBUpdate_2_ID);
    }

    var PORTCONNECTIONUPDATE_ID = topic_Get(tMediaController, "PORTCONNECTIONUPDATE_ID");
    if (!jsx3.util.strEmpty(PORTCONNECTIONUPDATE_ID)) {
      eventApp.unsubscribe("PORTCONNECTIONUPDATE", PORTCONNECTIONUPDATE_ID);
    }

    var PORTCONNECTIONSTATEUPDATE_ID = topic_Get(tMediaController, "PORTCONNECTIONSTATEUPDATE_ID");
    if (!jsx3.util.strEmpty(PORTCONNECTIONSTATEUPDATE_ID)) {
      eventApp.unsubscribe("PORTCONNECTIONSTATEUPDATE", PORTCONNECTIONSTATEUPDATE_ID);
    }

    var PORTDISPLAYCONFIGUPDATE_ID = topic_Get(tMediaController, "PORTDISPLAYCONFIGUPDATE_ID");
    if (!jsx3.util.strEmpty(PORTDISPLAYCONFIGUPDATE_ID)) {
      eventApp.unsubscribe("PORTDISPLAYCONFIGUPDATE", PORTDISPLAYCONFIGUPDATE_ID);
    }

    var PORTDISPLAYSTATUSUPDATE_ID = topic_Get(tMediaController, "PORTDISPLAYSTATUSUPDATE_ID");
    if (!jsx3.util.strEmpty(PORTDISPLAYSTATUSUPDATE_ID)) {
      eventApp.unsubscribe("PORTDISPLAYSTATUSUPDATE", PORTDISPLAYSTATUSUPDATE_ID);
    }

    var PVRTrimLengthDataUpdated_ID = topic_Get(tMediaController, "PVRTrimLengthDataUpdated_ID");
    if (!jsx3.util.strEmpty(PVRTrimLengthDataUpdated_ID)) {
      eventApp.unsubscribe("PVRTrimLengthDataUpdated", PVRTrimLengthDataUpdated_ID);
    }

    var PORTCONNECTIONUPDATE_2_ID = topic_Get(tMediaController, "PORTCONNECTIONUPDATE_2_ID");
    if (!jsx3.util.strEmpty(PORTCONNECTIONUPDATE_2_ID)) {
      eventApp.unsubscribe("PORTCONNECTIONUPDATE", PORTCONNECTIONUPDATE_2_ID);
    }

    // destroy any eventlistners
    eventMediaController.destroy();
    eventMediaController = null;
    // destroy the local topic
    tMediaControllerDoc = null;
    topic_Destroy(tMediaController);
    if (parentWindow && parentWindow.isOpen())
    {
      parentWindow.focus();
    }
    // dereference the top block
    winMediaController = null;
  }
  
  // Init/Open a window
  winMediaController = new CmpAppWindow(tMediaController, eventMediaController,"components/mediaController.xml",
                                   {WinTitle:"Media List", 
                                    WinHeight:600, 
                                    WinWidth:1000, 
                                    WinResizable:true, 
                                    WinScrollable:true},
                                    null,
                                    null,
                                    {action: "FNCCALL",
                                     context: null,
                                     fnc: function()
                                          {
                                           cmpWinMediaController_Destroy();
                                          }
                                    }
                                  );

  winMediaController.eventMediaController = eventMediaController;
  winMediaController.tMediaController = tMediaController;

  // open the window - which will call createSub - which will init and show
  winMediaController.asyncOpen().when(jsx3.$F(function () {
    winMediaController.window = winMediaController.BlkJSX._jsxwindow;
    winMediaController.createSubCmp = cmpMediaControllerSubArr_Create;
  }).bind(winMediaController));
  
}



