// a topic / listner for messaging in it
var tUserList = "UserList";
var tUserListDoc = null;
var eventUserList = null;
// the top level block
var winUserList = null;

var UserListCallerID = null;

function cmpWinUserList_Create(tCaller,parentWindow)
{
  // save the callers topic/name in a gloabl static var
  // this will be used to target events later
  UserListCallerID = tCaller;

  // create only once
  if( winUserList != null ) {
    winUserList.show();
    return;
  }
  
  // create a topic / listner for messaging in it
  tUserListDoc = topic_Create(tUserList);
  eventUserList = new EventListner(tUserList);  
  
  function cmpWinUserList_Destroy() {
    // destroy any eventlistners
    eventUserList.destroy();
    eventUserList = null;
    // destroy the local topic
    tUserListDoc = null;
    topic_Destroy(tUserList);
    if (parentWindow!=null && parentWindow.isOpen())
    {
      parentWindow.focus();
    }
    // dereference the top block
    winUserList = null;
  }
  
  // Init/Open a window
  winUserList = new CmpAppWindow(tUserList, eventUserList, "components/userList.xml",
                                   {WinTitle:"Participants List", 
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
                                           cmpWinUserList_Destroy();
                                          }
                                    }
                                  );
  //init tabs 
  var userTab = PNameSpace.getJSXByName("blkUsersButton");
  userTab.setClassName(CSSRule_tabOn);

  winUserList.createSubCmp = function()
  {
    // get vars from object instance
    var winUserListBlkJSX = this.BlkJSX;
    var tUserList = this.tWin;
    var eventUserList = this.eventWin;

    //Destination Port group tree
    var cmpTreeUserGroupTree = new CmpNIDGroupTagValueList([winUserListBlkJSX, "treeUsers"],
                                                            "DisplayUserGroupTree",
                                                            null,
                                                            [{ action: "TOPICEVENT", topic: tUserList, eventvar: "DisplayUserListUpdated", eventval: null }],
                                                            { persist: { col: "jsxtext", initval: true },
                                                              action: "TOPICEVENT", topic: tUserList, eventvar: "UserGroupSelected", eventval: "jsxpath"
                                                            },
                                                            "TREE",
                                                            "images/icons/icon_folder.png",
                                                            "images/icons/icon_tag.png"
                                                          );

    //User matrix data  
    var cmpMtxUserList = new CmpNIDList([winUserListBlkJSX, "mtxUserList"],
                                        "DisplayUserGroupTree",
                                        null,
                                        "" + tUserList + ":UserGroupSelected",
                                       {"jsxid":"\"./@NID\"",
                                        "jsxtextuserName":"\".//Name\"",
                                        "jsxtext":"\".//Username\"",
                                        "jsxtextuserEmail":"\".//Email\"",
                                        "jsxtextuserCreated":"\".//CreationDate\""},
                                      [{ action: "TOPICEVENT", topic: tUserList, eventvar: "DisplayUserListUpdated", eventval: null },
                                       { action: "TOPICEVENT", topic: tUserList, eventvar: "UserGroupSelected", eventval: null}],
                                      { persist: { col: "jsxid", initval: false },
                                        action: "TOPICEVENT", topic: tUserList, eventvar: "UserSelected", eventval: "jsxid"
                                      }
                                      );
                                      
                             
    // Destination Port Tab Panel
    var cmpBlkUserPanel = new CmpContainer([winUserListBlkJSX, "blkUser"],
                                            [{ cmp: cmpTreeUserGroupTree },
                                             { cmp: cmpMtxUserList}],
                                            null);
    // Tab Panels                                            
    var cmpBlkTabPanels = new CmpContainer([winUserListBlkJSX, "blkTabPanels"],
                                           [{ cmp: cmpBlkUserPanel}],
                                           [{ action: "TOPICEVENT", topic: tUserList, eventvar: "ButtonSelection", eventval: null}]
                                          );

    var cmpBtnUser = new CmpBlockButton([winUserListBlkJSX, "blkUsersButton"], null,
                                         { action: "FNCCALL",
                                           context: null,
                                           fnc: function()
                                           {
                                             topic_Publish(tUserList, "ButtonSelection", "User");
                                           }
                                         }, CSSRule_tabOn, CSSRule_tabOff);

    var cmpBlkTabButtons = new CmpContainer([winUserListBlkJSX, "blkTabButtonContainer"],
                                             [{ cmp: cmpBtnUser}],
                                             null
                                           );


    // Action function
    function getUserInfo()
    {
      // get the currently selected media info
      var mediaInfo = new Object;
      mediaInfo.Group = topic_Get(tUserList, "UserGroupSelected");
      mediaInfo.NID = topic_Get(tUserList, "UserSelected");
      mediaInfo.EntityObjType = "User";
      return mediaInfo;
    }

    var cmpBtnAdd = new CmpBlockButton( [winUserListBlkJSX, "blkAddButton"], null,
                                        { action: "FNCCALL",
                                          context: null,
                                          fnc: function() {
                                            // get the currently selected media info
                                            var mediaInfo = getUserInfo();
                                            // publish it to the callers topic
                                            if (mediaInfo.NID != null) {
                                              // using same event listener as src and destination
                                              topic_Publish(tCaller, "MediaListCollabSelection", mediaInfo);
                                            }

                                          }
                                        }, null);

    // Action buttons
    var cmpBlkActionButtons = new CmpContainer([winUserListBlkJSX, "blkUserListSubHeader"],
                                               [{ cmp: cmpBtnAdd}],
                                               null
                                              );

    // Top level sub cmp array
    this.SubCmpArr = [{ cmp: cmpBlkTabPanels },
                      { cmp: cmpBlkTabButtons },
                      { cmp: cmpBlkActionButtons }
                     ];

    // init selection vars
    topic_Set(tUserList, "UserGroupSelected", "NONE");
    topic_Set(tUserList, "UserSelected", "NONE");
                   
    // init and repaint happens after the window is loaded
    jsx3.sleep((function()
    {
      this.init();
      this.show();
      topic_Publish(tUserList, "DisplayUserListUpdated", (newServerDate()));
    }
               ),
               null, this);
  }
 
  // open the window - which will call createSub - which will init and show
  winUserList.open();
}


