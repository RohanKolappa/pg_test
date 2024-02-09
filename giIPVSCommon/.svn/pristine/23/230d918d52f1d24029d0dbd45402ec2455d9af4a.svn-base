/*
 * Copyright (c) IP Video Systems Inc.
 * Use, modification, and distribution subject to terms of license.
 */


/**
 * This interface provides browser based behavior 
 */
jsx3.require("jsx3.gui.Block");
jsx3.Class.defineClass("com.ipvs.gui.MediaPlayerIE", jsx3.gui.Block, null, function (MediaPlayerIE, MediaPlayerIE_prototype) {

  // create Logger for this class
  MediaPlayerIE_prototype._LOG = jsx3.util.Logger.getLogger("com.ipvs.gui.MediaPlayerIE");

  /** 
  * Get playerTextHTML, the playerTextHTML has difference by browser type
  * @return playerTextHTML
  */
  MediaPlayerIE_prototype.getPlayerTextHTML = function(playerId, playerType, blkId) {
    // set the ActiveX Object HTML as it's text
    var playerTextHTML = "<table border=\"0\" cellspacing=\"0\" cellpadding=\"0\" width=\"100%\" height=\"100%\"><tr><td align=\"center\" valign=\"middle\">" +
                             "<OBJECT id=" + playerId + " codeBase=\"//player//IPVPlayerCtrl.cab#Version=3,8,2,2\"  autosize=\"true\" border=\"8px\" bordercolor=\"#222\" classid=CLSID:A2A6189E-44F4-4D69-9CE4-8329312D2E3A VIEWASTEXT>" +
                             "<PARAM NAME=\"BackColor\" VALUE=\"7874560\"><PARAM NAME=\"BorderColor\" VALUE=\"0\">" +
                             "<PARAM NAME=\"AutoSize\" VALUE=\"0\"><PARAM NAME=\"AutoStart\" VALUE=\"0\">" +
                             "<PARAM NAME=\"SessionId\" VALUE=\"0\"><PARAM NAME=\"Windowless\"  VALUE=\"" + playerType + "\">" +
                             "</OBJECT><SPAN style=\"FILTER: alpha(opacity=0)\"></SPAN></td></tr></table>" +
                             "<SCRIPT for=" + playerId + " event=\"OnNativeSizeChange(a_lWidth,a_lHeight)\" LANGUAGE=\"JavaScript\"> " +
                                 "var jsx3BridgeModel = jsx3.GO('" + blkId + "'); " +
                                 "jsx3BridgeModel.doOnNativeSizeChange(a_lWidth,a_lHeight); " +
                             "</SCRIPT>" +
                             "<SCRIPT for=" + playerId + " event=\"OnFullScreen(bFullScreen)\" LANGUAGE=\"JavaScript\"> " +
                                 "var jsx3BridgeModel = jsx3.GO('" + blkId + "'); " +
                                 "jsx3BridgeModel.doFullScreen(bFullScreen); " +
                            "</SCRIPT>" +
                            "<SCRIPT for=" + playerId + " event=\"OnKBMSEnable(bKBMSEnabled)\" LANGUAGE=\"JavaScript\"> " +
                                 "var jsx3BridgeModel = jsx3.GO('" + blkId + "'); " +
                                 "jsx3BridgeModel.doKBMSEnable(bKBMSEnabled); " +
                            "</SCRIPT>" +
                            "<SCRIPT for=" + playerId + " event=\"OnRFBEnable(bRFBEnabled)\" LANGUAGE=\"JavaScript\"> " +
                                 "var jsx3BridgeModel = jsx3.GO('" + blkId + "'); " +
                                 "jsx3BridgeModel.doRFBEnabled(bRFBEnabled); " +
                            "</SCRIPT>";
    return playerTextHTML;
  };
});
