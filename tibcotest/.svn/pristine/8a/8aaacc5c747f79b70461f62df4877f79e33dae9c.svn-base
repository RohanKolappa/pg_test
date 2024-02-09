/* initialize Project Namespace */
var PNameSpace="IPVSAdminUI";
PNameSpace=eval(PNameSpace);

var g_TreeRootNodeImage = "images/icons/RootNode.gif";
var g_TreeNodeImage = "images/icons/Node.gif";

var getTagsXSLfile = PNameSpace.loadInclude("JSXAPPS/IPVS_AdminUI/xsl/GetTagColumnsList.xsl", "GetTagColumnsList", "xsl");
var getTagsXSLDoc = jsx3.xml.XslDocument.wrap(getTagsXSLfile);

var geFilterXSLfile = PNameSpace.loadInclude("JSXAPPS/IPVS_AdminUI/xsl/GetTagFilterList.xsl", "GetTagFilterList", "xsl");
var getFilterXSLDoc = jsx3.xml.XslDocument.wrap(geFilterXSLfile);


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
  var nodeVersionData = loadXMLFile("JSXAPPS/IPVS_AdminUI/xml/version.xml");
  var nodeVersion = nodeVersionData.selectSingleNode(".//version");
  if (nodeVersion != null) {
    versionData = nodeVersion.getValue();
  }
  topic_Set(tApp, "BuildVersion", versionData);
  
  //init the permission class
  clsUserPermission = new com.ipvs.util.UserPermission();
  
  //init the device discovery agent
  discoverDeviceAgent = new com.ipvs.agent.DeviceDiscoveryAgent(eventApp);

  //init the skin class
  clsCustomizeSkin = new com.ipvs.util.CustomizeSkin();

  common_InitStartApp();
    
  jsxSleepMonitor = new jsxSleepQueueMonitor(1000);
  jsxSleepMonitor.start();
}

function app_SubscribeLogoutState() {
  // handle logout success
  IpvsLogin.subscribe(com.ipvs.api.Login.ON_LOGINSTATE_CHANGE, this,
                      function(objEvent) {
                        if( objEvent.stateVar == com.ipvs.api.Login.LOGINSTATE  && objEvent.stateVal == com.ipvs.api.Login.LOGIN_LOGOUT_SUCCESS ) {
						              app_FinalOnUnLoad();
						              app_InitOnLoad();
                        }
                      } );
}

function app_FinalOnUnLoad() {
  jsxSleepMonitor.stop();
  commonAgentCache_Final();
  common_FinalOnUnLoad();
  clsUserPermission = null;
  discoverDeviceAgent = null;
}

