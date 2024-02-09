function cmpWinSessionMonitor_Create(tSessionStatus,eventSessionStatus, parentWindow){
  var statusWinInstance = topic_Get(tSessionStatus, "SessionStatusWindow");
  if (!jsx3.util.strEmpty(statusWinInstance) && statusWinInstance != "NONE") {
    var statusWinInstanceBlkJSX = PNameSpace.getAppWindow(statusWinInstance);
    statusWinInstanceBlkJSX.focus();
    return;
  }
  
  var tSessionStatusWin = tSessionStatus + "SessionStatus" + jsx3.xml.CDF.getKey();
  topic_Set(tSessionStatus, "SessionStatusWindow", tSessionStatusWin);
  
  var xslfile = PNameSpace.loadInclude("JSXAPPS/IPVS_ClientUI/xsl/formatXML.xsl","formatXML","xsl");        
  var xslDoc = jsx3.xml.XslDocument.wrap(xslfile);  
          
  function cmpWinSessionStatus_Destroy() {
    tSessionStatusWin = null;
    WinSessionStatus = null;
    topic_Set(tSessionStatus, "SessionStatusWindow", "NONE");
    if (parentWindow!=null && parentWindow.isOpen()) {
      parentWindow.focus();
    }
  }// end cmpWinSessionStatus_Destroy
  
  var SessionName = topic_Get(tSessionStatus, "CollaborationName");
  // Init/Open a window
  var WinSessionStatus = new CmpAppWindow(tSessionStatusWin, eventSessionStatus, "components/sessionMonitor.xml",
                                       {WinTitle:SessionName+" Session Status", 
                                        WinHeight:600, 
                                        WinWidth:1000, 
                                        WinResizable:true, 
                                        WinScrollable:true},
                                        null,
                                        null,
                                        {action: "FNCCALL",
                                         context: null,
                                         fnc: function() {
                                               cmpWinSessionStatus_Destroy();
                                              }
                                        }
                                     ); 
                                      
  WinSessionStatus.get_Data = function() {
  } // end get_Data
  
  WinSessionStatus.createSubCmp = function() {  
    var cmpBlkSessionMonitorUI = cmpSessionMonitor_Create(WinSessionStatus, tSessionStatus, eventSessionStatus);
    
    // Top level sub cmp array
    this.SubCmpArr = [{ cmp: cmpBlkSessionMonitorUI }];
 
    function getSession(){
      topic_Publish(tSessionStatus, "SessionConnData", (newServerDate()).getTime() );
    }                        
    // init and repaint happens after the window is loaded
    jsx3.sleep((function() {
                  this.init();
                  this.show();
                  getSession();
                }
               ),
               null, this);
  } // endcreateSubCmp
  
  // open the window - which will call createSub - which will init and show
  WinSessionStatus.open();
     
} // end cmpWinSessionMonitor_Create

