function cmpBlkSystemAdmin_Create() {
  var blkSystemAdminBlkJSX = PNameSpace.getJSXByName("blkSystemAdmin");
  blkSystemAdminBlkJSX.load("components/admin/systemAdmin.xml");
  
  // Create Component Blocks
  var cmpBlkReports = cmpBlkReports_Create();
  
  var cmpBlkUpgrades = cmpBlkUpgrade_Create();
  
  var cmpBlkBackup = cmpBlkBackup_Create();
  
  var cmpBlkRestore = cmpBlkRestore_Create();

  var cmpBlkExternalAuth = cmpBlkExternalAuth_Create();

  var cmpBlkHLS = cmpBlkHLS_Create();

  var cmpBlkPreferences = cmpBlkPreferences_Create();

  var cmpBlkDeviceDiscovery = cmpBlkDeviceDiscovery_Create();

  var cmpBlkMulticastIPAddress = cmpBlkMulticastIPAddress_Create();
  
  // Outer Container to Show/Hide the Selected Blocks
  var cmpBlkSystemAdminUI = new CmpContainer( "blkSystemAdminUI",
                                              [{cmp:cmpBlkReports,
                                                sel:"topic_Get(tApp, 'SystemAdminMenuSelection') == 'REPORTS' && clsUserPermission.getPermission('Report', 'read') == 'true'"},
                                               {cmp:cmpBlkUpgrades,
                                                sel:"topic_Get(tApp, 'SystemAdminMenuSelection') == 'UPGRADE' && clsUserPermission.getPermission('System', 'upgrade') == 'true'"},
                                               {cmp:cmpBlkBackup,
                                                sel:"topic_Get(tApp, 'SystemAdminMenuSelection') == 'BACKUP' && clsUserPermission.getPermission('System', 'backup') == 'true'"},
                                               {cmp:cmpBlkRestore,
                                                sel:"topic_Get(tApp, 'SystemAdminMenuSelection') == 'RESTORE' && clsUserPermission.getPermission('System', 'restore') == 'true'"},
                                               {cmp:cmpBlkExternalAuth,
                                                sel: "topic_Get(tApp, 'SystemAdminMenuSelection') == 'EXTERNALAUTH' && clsUserPermission.getPermission('System', 'read') == 'true'"},
                                               { cmp: cmpBlkHLS,
                                                   sel: "topic_Get(tApp, 'SystemAdminMenuSelection') == 'HLS' && clsUserPermission.getPermission('System', 'read') == 'true'"},
                                               { cmp: cmpBlkDeviceDiscovery,
                                                sel: "topic_Get(tApp, 'SystemAdminMenuSelection') == 'DEVICEDISCOVERY' && clsUserPermission.getPermission('System', 'read') == 'true'"},
                                               {cmp:cmpBlkPreferences,
                                                sel:"topic_Get(tApp, 'SystemAdminMenuSelection') == 'PREFERENCES' && clsUserPermission.getPermission('System', 'read') == 'true'"},
                                               {cmp:cmpBlkMulticastIPAddress,
                                                sel:"topic_Get(tApp, 'SystemAdminMenuSelection') == 'MULTICASTIPADDRESS' && clsUserPermission.getPermission('System', 'read') == 'true'"}
                                              ],
                                              [{action:"APPEVENT", eventvar:"SystemAdminMenuSelection", eventval:null}] );
                                        
  // Buttons in Left Panel
  var cmpLblReports = new CmpLabelHighlight(["blkSystemAdmin","leftNav","lblReports"], null, 
                                            [{ action: "APPEVENT", eventvar: "SystemAdminMenuSelection", eventval: null}],
                                            {action:"APPEVENT", eventvar:"SystemAdminMenuSelection", eventval:"REPORTS"});
                                            
  var cmpLblBackup = new CmpLabelHighlight(["blkSystemAdmin","leftNav","lblBackup"], null,
                                           [{ action: "APPEVENT", eventvar: "SystemAdminMenuSelection", eventval: null}],                                            
                                           {action:"APPEVENT", eventvar:"SystemAdminMenuSelection", eventval:"BACKUP"});
                                            
  var cmpLblRestore = new CmpLabelHighlight(["blkSystemAdmin","leftNav","lblRestore"], null, 
                                            [{ action: "APPEVENT", eventvar: "SystemAdminMenuSelection", eventval: null}],  
                                            {action:"APPEVENT", eventvar:"SystemAdminMenuSelection", eventval:"RESTORE"});
                                            
  var cmpLblUpgrade = new CmpLabelHighlight(["blkSystemAdmin","leftNav","lblUpgrade"], null,
                                            [{ action: "APPEVENT", eventvar: "SystemAdminMenuSelection", eventval: null}], 
                                            {action:"APPEVENT", eventvar:"SystemAdminMenuSelection", eventval:"UPGRADE"});

  var cmpLblExternalAuth = new CmpLabelHighlight(["blkSystemAdmin","leftNav","lblExAuth"], null,
                                                 [{ action: "APPEVENT", eventvar: "SystemAdminMenuSelection", eventval: null}],
                                                 { action: "APPEVENT", eventvar: "SystemAdminMenuSelection", eventval: "EXTERNALAUTH" });

  var cmpLblHLS = new CmpLabelHighlight(["blkSystemAdmin", "leftNav", "lblHLS"], null,
                                                 [{ action: "APPEVENT", eventvar: "SystemAdminMenuSelection", eventval: null}],
                                                 { action: "APPEVENT", eventvar: "SystemAdminMenuSelection", eventval: "HLS" });

  var cmpLblDeviceDiscovery = new CmpLabelHighlight(["blkSystemAdmin", "leftNav", "lblDeviceDiscovery"], null,
                                               [{ action: "APPEVENT", eventvar: "SystemAdminMenuSelection", eventval: null}],
                                               { action: "APPEVENT", eventvar: "SystemAdminMenuSelection", eventval: "DEVICEDISCOVERY" });
  
  var cmpLblPrefrences = new CmpLabelHighlight(["blkSystemAdmin","leftNav","lblPreferences"], null,
                                               [{ action: "APPEVENT", eventvar: "SystemAdminMenuSelection", eventval: null}], 
                                               {action:"APPEVENT", eventvar:"SystemAdminMenuSelection", eventval:"PREFERENCES"});

  var cmpLblMulticastIPAddress = new CmpLabelHighlight(["blkSystemAdmin","leftNav","lblMulticastIPAddress"], null,
                                                       [{ action: "APPEVENT", eventvar: "SystemAdminMenuSelection", eventval: null}], 
                                                       {action:"APPEVENT", eventvar:"SystemAdminMenuSelection", eventval:"MULTICASTIPADDRESS"});
                                          
  
  var cmpBlkLblReports = new Component(["blkSystemAdmin","leftNav","blkLblReports"]);
  var cmpBlkLblBackup = new Component(["blkSystemAdmin","leftNav","blkLblBackup"]);                                                  
  var cmpBlkLblRestore = new Component(["blkSystemAdmin","leftNav","blkLblRestore"]);
  var cmpBlkLblUpgrade = new Component(["blkSystemAdmin","leftNav","blkLblUpgrade"]);
  var cmpBlkLblExternalAuth = new Component(["blkSystemAdmin", "leftNav", "blkLblExAuth"]);
  var cmpBlkLblHLS = new Component(["blkSystemAdmin", "leftNav", "blkLblHLS"]);
  var cmpBlkLblDeviceDiscovery = new Component(["blkSystemAdmin", "leftNav", "blkLblDeviceDiscovery"]);     
  var cmpBlkLblPreferences = new Component(["blkSystemAdmin","leftNav","blkLblPreferences"]);                                            
  var cmpBlkLblMulticastIPAddress = new Component(["blkSystemAdmin","leftNav","blkLblMulticastIPAddress"]);                                            
                                            
  var cmpBlkSystemAdmin = new CmpContainer( "blkSystemAdmin",  
                                            [ {cmp:cmpLblReports},
                                              {cmp:cmpLblBackup},
                                              {cmp:cmpLblRestore},
                                              {cmp:cmpLblUpgrade},
                                              {cmp:cmpLblExternalAuth},
                                              {cmp:cmpLblHLS},
                                              {cmp:cmpLblDeviceDiscovery},
                                              {cmp:cmpLblPrefrences},
                                              {cmp:cmpLblMulticastIPAddress},
                                              {cmp:cmpBlkLblReports,
                                               sel: "clsUserPermission.getPermission('Report', 'read') == 'true'"},
                                              {cmp:cmpBlkLblBackup,
                                               sel: "clsUserPermission.getPermission('System', 'backup') == 'true'"},
                                              {cmp:cmpBlkLblRestore,
                                               sel: "clsUserPermission.getPermission('System', 'restore') == 'true'"},
                                              {cmp:cmpBlkLblUpgrade,
                                               sel: "clsUserPermission.getPermission('System', 'upgrade') == 'true'"},
                                              {cmp:cmpBlkLblExternalAuth,
                                               sel: "clsUserPermission.getPermission('System', 'read') == 'true'"},
                                              { cmp: cmpBlkLblHLS,
                                                  sel: "clsUserPermission.getPermission('System', 'read') == 'true'"},
                                              {cmp:cmpBlkLblDeviceDiscovery,
                                               sel: "clsUserPermission.getPermission('System', 'read') == 'true'"},
                                              {cmp:cmpBlkLblPreferences,
                                               sel: "clsUserPermission.getPermission('System', 'read') == 'true'"},
                                              {cmp:cmpBlkLblMulticastIPAddress,
                                               sel: "clsUserPermission.getPermission('System', 'read') == 'true'"},
                                              {cmp:cmpBlkSystemAdminUI}
                                            ],
                                            [{ action: "APPEVENT", eventvar: "SystemAdminMenuSelection", eventval: null}]);
                                            
  eventApp.subscribe("MENUUIDATA", "SYSTEMADMIN", this,
                      function(EventVar, EventVal) {
                        if( EventVal == "SYSTEMADMIN" ) {
                          var menuSelection = topic_Get(tApp, "SystemAdminMenuSelection");
                          if(menuSelection == "REPORTS") {
                            if (clsUserPermission.getPermission('Report', 'read') == 'true'){
                              topic_Publish(tApp, "SystemAdminMenuSelection","REPORTS");
                            }
                          }
                          else if(menuSelection == "BACKUP") {
                            if (clsUserPermission.getPermission('System', 'backup') == 'true'){
                              topic_Publish(tApp, "SystemAdminMenuSelection","BACKUP");
                            }
                          }
                          else if(menuSelection == "RESTORE") {
                            if (clsUserPermission.getPermission('System', 'restore') == 'true'){
                              topic_Publish(tApp, "SystemAdminMenuSelection","RESTORE");
                            }
                          }
                          else if(menuSelection == "UPGRADE") {
                            if (clsUserPermission.getPermission('System', 'upgrade') == 'true'){
                              topic_Publish(tApp, "SystemAdminMenuSelection","UPGRADE");
                            }
                          }
                          else if(menuSelection == "EXTERNALAUTH") {
                            if (clsUserPermission.getPermission('System', 'read') == 'true'){
                              topic_Publish(tApp, "SystemAdminMenuSelection","EXTERNALAUTH");
                            }
                          }
                          else if (menuSelection == "HLS") {
                           if (clsUserPermission.getPermission('System', 'read') == 'true') {
                              topic_Publish(tApp, "SystemAdminMenuSelection", "HLS");
                           }
                          }
                          else if (menuSelection == "DEVICEDISCOVERY") {
                            if (clsUserPermission.getPermission('System', 'read') == 'true') {
                              topic_Publish(tApp, "SystemAdminMenuSelection", "DEVICEDISCOVERY");
                            }
                          }
                          else if(menuSelection == "PREFERENCES") {
                            if (clsUserPermission.getPermission('System', 'read') == 'true'){
                              topic_Publish(tApp, "SystemAdminMenuSelection","PREFERENCES");
                            }
                          }
                          else if(menuSelection == "MULTICASTIPADDRESS") {
                            if (clsUserPermission.getPermission('System', 'read') == 'true'){
                              topic_Publish(tApp, "SystemAdminMenuSelection","MULTICASTIPADDRESS");
                            }
                          }
                        }
                      }
                    );
  return cmpBlkSystemAdmin ;
  
}//end cmpBlkSystemAdmin_Create

function cmpBlkSystemAdmin_Destroy() {
  var blkSystemAdminBlkJSX = PNameSpace.getJSXByName("blkSystemAdmin");
  blkSystemAdminBlkJSX.removeChildren();
}