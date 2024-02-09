var PNameSpace = "IPVSAuthMediaClient";
PNameSpace = eval(PNameSpace);
var clsDefaultSettings = null;

function app_InitOnLoad() {
  // setup Display Agent
  agtDisplay = new com.ipvs.agent.DisplayAgent(null);
  var avc = PNameSpace.getServer().getRootBlock().getDescendantsOfType("com.ipvs.gui.authmediaclient.MediaList")[0];
  avc.init();
}

function app_OnUnLoad() {
  clsDefaultSettings = null;
  agtDisplay = null;
}