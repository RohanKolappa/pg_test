//collect main menu create to a separate file
function cmpCanvasMenu_Create(cmpBlkCanvasMenu) {

  //--- Logoff
  var cmpBtnLogOff = new CmpBlockButton(["blkUnmannedMenu", "blkLogOff"], null,
                                        { action: "FNCCALL", context: this,
                                          fnc: function() {
                                            IpvsLogin.doLogout();
                                          } 
                                        }, null, null);

  //--- Canvas
 /* var cmpBtnLaunchDisplay = new CmpBlockButton(["blkUnmannedMenu", "blkLaunchDisplay"], null,
                                               { action: "FNCCALL", context: null,
                                                 fnc: function() {
                                                   //launch full sreen browser window for 'UnManned'
                                                   agtDisplay.open();
                                                 }
                                               }, null, null);*/
                                               
  var cmpBtnPreferences = new CmpBlockButton(["blkUnmannedMenu", "blkPreferences"], null,
                                             { action: "FNCCALL",
                                               context: null,
                                               fnc: function() {
                                                  cmpWinPreferences_Create(this);
                                               }
                                              }, null, null);

  cmpBlkCanvasMenu.SubCmpArr = [ { cmp: cmpBtnLogOff },
                                 //{ cmp: cmpBtnLaunchDisplay },
                                 {cmp:cmpBtnPreferences}
                               ]
  cmpBlkCanvasMenu.init();
}