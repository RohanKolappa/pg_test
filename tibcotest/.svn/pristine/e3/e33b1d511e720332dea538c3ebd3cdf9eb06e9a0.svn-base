// a topic / listner for messaging in it
var tMediaList = "MediaList";
var tMediaListDoc = null;
var eventMediaList = null;
// the top level block
var winMediaList = null;

var MediaListCallerID = null;

function cmpWinMediaList_Create(tCaller,parentWindow)
{
  // save the callers topic/name in a gloabl static var
  // this will be used to target events later
//  MediaListCallerID = tCaller;
  
  // create only once
  if( winMediaList != null ) {
    if (parentWindow) {
      //call from 'SessionInfo' -- Collaboration window
      //display Add button only
      topic_Publish(tMediaList, "ConnectOrAdd", "ADD");
      topic_Publish(tMediaList, "ButtonSelection", topic_Get(tCaller, "ButtonSelection"));
    }
    else {
      //call from BlkUI -- Media List window
      //display connect button only 
      topic_Publish(tMediaList, "ConnectOrAdd", "CONNECT");
    }
    // show/focus the window  
    winMediaList.show();
    winMediaList.focus();
    return;
  }
  //tMediaList = tMediaList +"_"+ getUUID();

  // create a topic / listner for messaging in it
  tMediaListDoc = topic_Create(tMediaList);
  eventMediaList = new EventListner(tMediaList);

  function cmpWinMediaList_Destroy() {
    //destroy any DBUpdate listners
    var DBUpdate_2_ID = topic_Get(tMediaList, "DBUpdate_2_ID");
    if (!jsx3.util.strEmpty(DBUpdate_2_ID)) {
      eventApp.unsubscribe("DBUpdate", DBUpdate_2_ID);
    }

    var PVRTrimLengthDataUpdated_ID = topic_Get(tMediaList, "PVRTrimLengthDataUpdated_ID");
    if (!jsx3.util.strEmpty(PVRTrimLengthDataUpdated_ID)) {
      eventApp.unsubscribe("PVRTrimLengthDataUpdated", PVRTrimLengthDataUpdated_ID);
    }

    var PORTCONNECTIONUPDATE_2_ID = topic_Get(tMediaList, "PORTCONNECTIONUPDATE_2_ID");
    if (!jsx3.util.strEmpty(PORTCONNECTIONUPDATE_2_ID)) {
      eventApp.unsubscribe("PORTCONNECTIONUPDATE", PORTCONNECTIONUPDATE_2_ID);
    }

    // destroy any eventlistners
    eventMediaList.destroy();
    var pubSubLength = eventMediaList.PubSubList.length;
    for (--pubSubLength; pubSubLength >= 0; pubSubLength--) {
      eventMediaList.unsubscribe(eventMediaList.PubSubList[pubSubLength].Var, eventMediaList.PubSubList[pubSubLength].Id);
    }
    eventMediaList = null;

    // destroy the local topic
    tMediaListDoc.removeChildren();
    tMediaListDoc = null;
    topic_Destroy(tMediaList);
    tMediaList = "MediaList";

    if (parentWindow && parentWindow.isOpen()) {

      if (parentWindow._jsxopener) {
        if (winBrowseSessionsBlkJSX != null) {
          winBrowseSessionsBlkJSX._jsxwindow.blur();
          winBrowseSessionsBlkJSX._jsxwindow.hideFocus = false;
        }
      } else if (parentWindow._jsxpB) {
        if (winBrowseSessionsBlkJSX != null) {
          winBrowseSessionsBlkJSX._jsxwindow.blur();
          winBrowseSessionsBlkJSX._jsxwindow.hideFocus = false;
        }
      }
      parentWindow._jsxwindow.focus();
    }

    if (winMediaList.SubCmpArr == null) return;
    for (var i = 0; i < winMediaList.SubCmpArr.length; i++) {
      if (winMediaList.SubCmpArr[i].cmp == null) continue;
      winMediaList.SubCmpArr[i].cmp.destroy();
      winMediaList.SubCmpArr[i].cmp = null;
      delete winMediaList.SubCmpArr[i];
    }
    delete winMediaList.SubCmpArr;
    // dereference the top block
    winMediaList = null;
  }
  
  // Init/Open a window
  winMediaList = new CmpAppWindow(tMediaList, eventMediaList,"components/mediaList.xml",
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
                                           cmpWinMediaList_Destroy();
                                          }
                                    }
                                  );
                                  
  // Create window sub components
  winMediaList.createSubCmp = function () {
                                      winMediaList.eventMediaList = eventMediaList;
                                      winMediaList.tMediaList = tMediaList;
                                      winMediaList.MediaListCallerID = tCaller;
                                      winMediaList.window = winMediaList.BlkJSX._jsxwindow;
                                      // Top level sub cmp array
                                      var calledFrom = "MEDIALIST";
                                      if (parentWindow) {
                                        calledFrom = "CREATESESSION";
                                      }
                                      this.SubCmpArr = cmpMediaList_Create(winMediaList, tMediaList, eventMediaList, calledFrom);
                                      var ThisClass = this;

                                      jsx3.sleep((function () {
                                        parentWindow._jsxwindow.blur();
                                        parentWindow._jsxwindow.hideFocus = false;
                                        if (parentWindow._jsxopener) {
                                          parentWindow._jsxopener.blur();
                                          parentWindow._jsxopener.hideFocus = false;
                                        } else if (parentWindow._jsxpB) {
                                          parentWindow._jsxpB.blur();
                                          parentWindow._jsxpB.hideFocus = false;
                                        }

                                        ThisClass.init();
                                        ThisClass.show();

                                        var buttonSelected = topic_Get(tCaller, "ButtonSelection");
                                        if ((clsUserPermission.getPermission('Port', 'read') == 'true')) {
                                          if (jsx3.util.strEmpty(buttonSelected)) {
                                            buttonSelected = "Live";
                                          }
                                        } else if ((clsUserPermission.getPermission('Dir', 'read') == 'true' && clsUserPermission.getPermission('File', 'read') == 'true')) {
                                          buttonSelected = "Recorded";
                                        }
                                        topic_Publish(tMediaList, "ButtonSelection", buttonSelected);

                                        winMediaList.BlkJSX._jsxwindow.focus(); //


                                        }),
                                         null, this);
                                    
                                    }

                                   
  // open the window - which will call createSub - which will init and show
    winMediaList.open();

  //Topmost level Event Handler for selected media 
  eventMediaList.subscribe("MediaListSelectionInfo", null, this,
                            function(eventVar, eventVal) {
                              topic_Publish(tCaller, "MediaListCollabSelection", eventVal);
                            }
                          );

}