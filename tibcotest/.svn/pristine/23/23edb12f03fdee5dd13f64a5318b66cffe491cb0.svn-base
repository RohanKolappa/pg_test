var PNameSpace = "IPVSClientUI";
PNameSpace = eval(PNameSpace);

// TBD - need to get this from defaultAsset record
var DefaultDestNID = "";

var g_TreeRootNodeImage = "images/icons/RootNode.gif";
var g_TreeNodeImage = "images/icons/Node.gif";

var CSSRule_tabOn = "tabOn";
var CSSRule_tabOff = "tabOff";
var CSSbtnRemoteModeUp = "btnRemoteModeUp";
var CSSbtnRemoteModeDown = "btnRemoteModeDown";
var CSSbtnLocalModeUp = "btnLocalModeUp";
var CSSbtnLocalModeDown = "btnLocalModeDown";
var CSSbtnPIPModeUp = "btnPIPModeUp";
var CSSbtnPIPModeDown = "btnPIPModeDown";

var CSSbtnPIPSize2x8Up = "btnPIPSize2x8Up";
var CSSbtnPIPSize2x8Down = "btnPIPSize2x8Down";
var CSSbtnPIPSize4x8Up = "btnPIPSize4x8Up";
var CSSbtnPIPSize4x8Down = "btnPIPSize4x8Down";
var CSSbtnPIPSize6x8Up = "btnPIPSize6x8Up";
var CSSbtnPIPSize6x8Down = "btnPIPSize6x8Down";
var CSSbtnPIPSize8x8Up = "btnPIPSize8x8Up";
var CSSbtnPIPSize8x8Down = "btnPIPSize8x8Down";
var CSSbtnPIPLocationUp = "btnPIPLocationUp";
var CSSbtnPIPLocationOver = "btnPIPLocationOver";
var CSSbtnPIPLocationDown = "btnPIPLocationDown";
//player windowing header
var CSSpvrWindow = "pvrWindow";
var CSSpvrWindowSelected = "pvrWindowSelected";
var CSSpvrWindowOver = "pvrWindowOver";

var jsxSleepMonitor = null;

function app_InitOnLoad() {

  /**
  * bug #40267 fix -- move common_InitLog to Applet.onAfterAttach
  * otherwise cause applet loading fail without any error for version jre 1.7.0_01 above
  **/
  //  common_InitLog();

  //crate application topic and event handler
  common_InitApplicationTopic();

  //initial and get client type(web/local),browser type(IE/FF)
  common_InitClientEnvironment();

  common_InitDefaultsLoad();

  // get the version from the version.xml
  var versionData = "0.0.r0.b0";
  var nodeVersionData = loadXMLFile("JSXAPPS/IPVS_ClientUI/xml/version.xml");
  var nodeVersion = nodeVersionData.selectSingleNode(".//version");
  if (nodeVersion != null) {
    versionData = nodeVersion.getValue();
  }
  topic_Set(tApp, "BuildVersion", versionData);
  
  //init the permission class
  clsUserPermission = new com.ipvs.util.UserPermission();
  
  if (clientType != "WEB") {
    //initialize commonPlayer Defaults
    playerInitDefaults();
    // turn of http mode to work around antivirus issues
    topic_Set(tApp, "IPVSPlayerDefaultUseHTTP", 0);

    topic_Set(tApp, "DestNID", DefaultDestNID);

    var PCIPAddress = null;
    // LoginInterface mac address
    var loginInterfaceMacAddress = getLocalPreferences(PNameSpace, ".//Login/InterfaceMacAddress", DefaultLoginInterfaceMacAddress);
    if (loginInterfaceMacAddress == "DefaultInterface" || jsx3.util.strEmpty(loginInterfaceMacAddress)) {
      PCIPAddress = getHWInfo_IPAddress();
    }
    else {
      PCIPAddress = getHWInfo_IPAddressOfMacAddress(loginInterfaceMacAddress);
    }
    topic_Set(tApp, "ClientIPAddress", PCIPAddress);

    //TBD:SJ This should really be done after login (and for each port)
    // setup Dest Stream Agent
    commonDSA_Init();

    // setup Display Agent
    agtDisplay = new com.ipvs.agent.DisplayAgent(eventApp);

    // setup Destination Agent
    agtMediaStreamDst = new com.ipvs.agent.MediaStreamDstAgent(eventApp);
  }
  //init the skin class
  clsCustomizeSkin = new com.ipvs.util.CustomizeSkin();
    
  // get the windowTitle
  var appTitle = clsCustomizeSkin.getAttrValue("IPVSClient", "appTitle");
  if (!jsx3.util.strEmpty(appTitle)) {
    var blkAppTitleBlkJSX = PNameSpace.getJSXByName("appTitle");
    blkAppTitleBlkJSX.setText(appTitle, true);
  }
 

  // setup Controller Agent
  agtCollaborationCtrl = new com.ipvs.agent.SessionCtrlAgent(eventApp);

  common_InitStartApp();

}

function app_SubscribeLogoutState() {
   // handle logout success
  IpvsLogin.subscribe(com.ipvs.api.Login.ON_LOGINSTATE_CHANGE, this,
                      function(objEvent) {
                        if( objEvent.stateVar == com.ipvs.api.Login.LOGINSTATE  && objEvent.stateVal == com.ipvs.api.Login.LOGIN_LOGOUT_SUCCESS ) {
						              //Handle unsubscribe the browser close
                          unSubscribeBrowserClose();
                          try{
                            app_FinalOnUnLoad(); 
                          }
                          catch (e) {
                            var errMsg = e.description;
                            log.error("Error occurs during appFinalOnUnload: "+errMsg);
                          }
                          finally {
                            //ensure to clean web controller from DOM after logout
                            var blkWebControllerJSX = PNameSpace.getJSXByName("blkMediaController");
                            if (blkWebControllerJSX && blkWebControllerJSX.getFirstChild()) {
                              blkWebControllerJSX.removeChildren();
                              log.info("appFinalOnUnload::remove blkMediaController children block in the final unload.");
                            }
                          }
                          autoLogin = "LOGOUT";
                          app_InitOnLoad();
                        }
                      } );

  // if logout fails - fatal error
  IpvsLogin.subscribe(com.ipvs.api.Login.ON_LOGOUT_ERROR, this,
                      function(objEvent) {
                        if( objEvent.stateVar == com.ipvs.api.Login.LOGINSTATE  && objEvent.stateVal == com.ipvs.api.Login.LOGIN_LOGOUT_ERROR ) {
                          alert( "Internal Error: Logout Failed\n" +
                                  "Please exit the application and retry" );
                        }
                      } );                      
}

function app_FinalOnUnLoad() {
//  jsxSleepMonitor.stop();
  // destroy agents
  // TBD:SJ - why is there no destroy for agtMediaStreamDst?
  agtCollaborationCtrl.destroy();
  commonDSA_Destroy();
  commonAgentCache_Final();
  common_FinalOnUnLoad();
}