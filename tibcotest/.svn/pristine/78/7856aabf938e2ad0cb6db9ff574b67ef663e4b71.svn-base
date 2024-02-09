//collect main menu create to a separate file
function cmpMainMenu_Create(cmpBlkMainMenu) {

  //--- Logoff
  var cmpBtnLogOff = new CmpBlockButton(["blkMainMenu", "blkLogOff"], null,
                                        { action: "FNCCALL", context: this,
                                          fnc: function() {
                                            IpvsLogin.doLogout();
                                          } 
                                        }, null, null);

  //--- Medialist - select and play in new player
  var cmpBtnMediaList = new CmpBlockButton(["blkMainMenu", "blkMediaList"], null,
                                            { action: "FNCCALL",
                                              context: null,
                                              fnc: function() {
                                                //'false' to display connect button only
                                                //cmpWinMediaList_Create(tApp,false);
                                              cmpWinMediaController_Create(tApp,false);
                                              }
                                            }, null, null);

  var cmpBlkMediaList = new CmpContainer(["blkMainMenu", "blkMediaList"],
                                          [{ cmp: cmpBtnMediaList}],
                                          null,
                                          null);
                                                                                      
  // New Session
  var cmpBtnNewSession = new CmpBlockButton(["blkMainMenu", "blkNewSession"], null,
                                            { action: "FNCCALL",
                                              context: null,
                                              fnc: function() {
                                                cmpWinCollaboration_Create(null);
                                              }
                                            }, null, null);

  var cmpBlkNewSession = new CmpContainer(["blkMainMenu", "blkNewSession"],
                                          [{ cmp: cmpBtnNewSession}],
                                          null,
                                          null);
                                      
  var cmpBtnPreferences = new CmpBlockButton(["blkMainMenu", "blkPreferences"], null, { action: "FNCCALL",
                                              context: null,
                                              fnc: function() {
                                                cmpWinPreferences_Create(this);
                                              }
                                            }, null, null);

  var cmpBtnChatNow = new CmpBlockButton(["blkMainMenu", "blkChatNow"], null, { action: "FNCCALL",
                                              context: null,
                                              fnc: function() {
                                                alert("chatNow");
                                              }
                                            }, null, null);
                                                                                       
  var cmpBtnBrowseSessions = new CmpBlockButton(["blkMainMenu", "blkBrowseSessions"], null, { action: "FNCCALL",
                                              context: null,
                                              fnc: function() {
                                                cmpWinBrowseSessions_Create(tApp);
                                              }
                                            }, null, null);

  var cmpBlkBrowseSessions = new CmpContainer(["blkMainMenu", "blkBrowseSessions"],
                                              [{ cmp: cmpBtnBrowseSessions}],
                                              null,
                                              null);
                                        
  var cmpBtnMyStuff = new CmpBlockButton(["blkMainMenu", "blkMyStuff"], null, { action: "FNCCALL",
                                              context: null,
                                              fnc: function() {
                                                cmpWinLiveSessions_Create(this);
                                              }
                                            }, null, null);

  var cmpBlkMyStuff = new CmpContainer(["blkMainMenu", "blkMyStuff"],
                                        [{ cmp: cmpBtnMyStuff}],
                                        null,
                                        null);

  cmpBlkMainMenu.SubCmpArr = [ { cmp: cmpBtnLogOff },
                               { cmp: cmpBtnPreferences },
                               { cmp: cmpBtnChatNow },
                               { cmp: cmpBlkMediaList,
                                 sel: "(clsUserPermission.getPermission('Port', 'read') == 'true') || (clsUserPermission.getPermission('Dir', 'read') == 'true' && clsUserPermission.getPermission('File', 'read') == 'true')"},
                               { cmp: cmpBlkNewSession,
                                 sel: "clsUserPermission.getPermission('SessionConfig', 'add') == 'true'"},
                               { cmp: cmpBlkBrowseSessions,
                                 sel: "clsUserPermission.getPermission('SessionConfig', 'read') == 'true'"},
                               { cmp: cmpBlkMyStuff,
                                 sel: "clsUserPermission.getPermission('SessionConfig', 'read') == 'true'"}
                              ]

  cmpBlkMainMenu.init();
}



