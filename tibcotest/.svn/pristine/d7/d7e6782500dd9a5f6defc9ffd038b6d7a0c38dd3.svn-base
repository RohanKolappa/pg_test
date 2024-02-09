function cmpBlkUser_Create() {
  var blkUserBlkJSX = PNameSpace.getJSXByName("blkUser");
  blkUserBlkJSX.load("components/users/users.xml");
  
  topic_Set(tApp, "UserGetDataState", "NONE");
  topic_Set(tApp, "UserCustomize", "NONE");
  topic_Set(tApp, "UserWhereTextSubStr", "");
  topic_Set(tApp, "UserWhereTagExpr", "");
  topic_Set(tApp, "UserFilter", "NONE");   
  topic_Set(tApp, "UserSelectFormat", "EXTENDED");
  topic_Set(tApp, "UserTagNameObjectType", "Users");
  var UserCountToFetch = 50;
  
  var cmpMtxUserList = new CmpCustomPaginationList(["blkUserList","mtxUsersList"],
                                                    [{action:"APPEVENT", eventvar:"UserGetData", eventval:null}],
                                                    {persist:{col:"jsxid",initval:false},
                                                     action:"APPEVENT", eventvar:"UserSelected", eventval:"jsxid"},
                                                    [{ColumnName:"jsxtextuserCreated", FncName:"@datetime,MM/dd/yyyy HH:mm"}],
                                                    null
                                                   );

  cmpMtxUserList.allowMultiSelection = true;
  cmpMtxUserList.deleteRequestCounter = 0;


  //translate($b/Name ,'ABCDEFGHIJKLMNOPQRSTUVWXYZ','abcdefghijklmnopqrstuvwxyz')                                                 
  cmpMtxUserList.SortPathArr = { jsxUserFullName:"fullName",
                                 jsxtext:"username",
                                 jsxtextuserEmail:"email",
                                 jsxtextuserCreated:"creationDate"
                               };
                               
  cmpMtxUserList.cmpReqParamArr = { StartFrom: "ObjectList_StartFrom",
                                    CountToFetch: "ObjectList_CountToFetch",
                                    SortOrder: "ObjectList_SortOrder",
                                    ObjectCount: "ObjectCount",
                                    SortByXpath: "ObjectList_SortBy"
                                  };                                           

  cmpMtxUserList.DataUpdatedEvent = {action:"TOPICEVENT", topic:tApp, eventvar:"UserDataUpdated", eventval:"LOADED"};                       


  cmpMtxUserList.BlkJSX.setXMLTransformers("xsl/UserList.xsl");
  cmpMtxUserList.cmpReq = new cmpReq_GetSessionAPIPaginationList(tApp, "GetUser",
                                                                    [{ VarName: "ObjectList_SelectFormat", VarRef: "UserSelectFormat"},
                                                                     { VarName: "ObjectList_WhereTextSubStr", VarRef: "UserWhereTextSubStr"},
                                                                     { VarName: "ObjectList_WhereTagExpr", VarRef: "UserWhereTagExpr"}
                                                                    ],
                                                                    { action: "FNCCALL", context: cmpMtxUserList,
                                                                        fnc: function(respCode, respVal) {
                                                                            if (respCode == "SUCCESS" && respVal != null) {
                                                                                this.onResponse(respCode, respVal);
                                                                            }
                                                                            else {
                                                                                log.info("GetUserRequest failed. Error Code :" + respCode);
                                                                                this.onResponseErrorHandle("GetUser",respCode, respVal);
                                                                            }
                                                                        }
                                                                    }
                                                                  );                                                
  cmpMtxUserList.cmpItemByNIDReqFnc = function () {
                                        topic_Set(tApp, "WhereUserNID", cmpMtxUserList.WhereNIDArr.join(","));
                                        return new cmpReq_GetSessionAPIPaginationList(tApp, "GetUser",
                                                                          [{ VarName: "ObjectList_WhereNID", VarRef: "WhereUserNID"},
                                                                           { VarName: "ObjectList_SelectFormat", VarRef: "UserSelectFormat"},
                                                                           { VarName: "ObjectList_WhereTextSubStr", VarRef: "UserWhereTextSubStr"},
                                                                           { VarName: "ObjectList_WhereTagExpr", VarRef: "UserWhereTagExpr"}
                                                                          ],
                                                                          {action: "FNCCALL", context: cmpMtxUserList,
                                                                                fnc: function(respCode, respVal) {
                                                                                      if (respCode == "SUCCESS" && respVal != null) {
                                                                                        this.onResponseUpdateRecord(respCode, respVal);
                                                                                        cmpBlkUserSelection();
                                                                                      }
                                                                                      else{
                                                                                        log.info("GetUserRequest failed. Error Code :" + respCode);
                                                                                        this.onResponseErrorHandle("GetUser",respCode, respVal);
                                                                                      }
                                                                                    }
                                                                              }, "ItemByNID"
                                                                          );
                                      }
  
  cmpMtxUserList.columnDataState = null;
  cmpMtxUserList.columnMtxDoc = new jsx3.xml.CDF.Document.newDocument();
  cmpMtxUserList.screenName = "User";   
  cmpMtxUserList.ColumnDataUpdatedEvent = {action:"TOPICEVENT", topic:tApp, eventvar:"UserColumnDataUpdated", eventval:"DATE"};
  cmpMtxUserList.InsertBeforeColumnName = "jsxtextuserCreated";
  cmpMtxUserList.columnCmpReq = new cmpReq_GetSessionAPIPaginationList(tApp, "GetTags",
                                                                  [{ VarName: "ObjectList_SelectFormat", VarRef: "TagNameResponseSelectFormat" },
                                                                   { VarName: "ObjectList_WhereObjectType", VarRef: "UserTagNameObjectType"}],
                                                                  { action: "FNCCALL", context: cmpMtxUserList,
                                                                    fnc: function (respCode, respVal) {
                                                                      if (respCode == "SUCCESS" && respVal != null) {
                                                                        var filterTagData = respVal.cloneDocument();
                                                                        var respValNode = getTagsXSLDoc.transform(respVal);
                                                                        var tagsNode = new jsx3.xml.Document();
                                                                        tagsNode.loadXML(respValNode);
                                                                        this.setColumnsFromDB(respCode, tagsNode);
                                                                        this.setMtxColumn();

                                                                        // use the same data for the filter block
                                                                        var filterdata = getFilterXSLDoc.transform(filterTagData);
                                                                        var filterTagNode = new jsx3.xml.Document();
                                                                        filterTagNode.loadXML(filterdata);
                                                                        this.setFilterTagDoc(respCode, filterTagNode);

                                                                        this.doRepaint();
                                                                      }
                                                                      else {
                                                                        log.info("GetTagsRequest failed. Error Code :" + respCode);
                                                                        this.onResponseErrorHandle("GetTags", respCode, respVal);
                                                                      }
                                                                    }
                                                                  }
                                                                );


  cmpMtxUserList.filterTagDataState = null;
  cmpMtxUserList.filterTagDoc = new jsx3.xml.CDF.Document.newDocument();
  cmpMtxUserList.filterTagDataUpdatedEvent = { action: "TOPICEVENT", topic: tApp, eventvar: "UserFilterTagDataUpdated", eventval: "LOADED" };
  cmpMtxUserList.filterTagCmpReq = new cmpReq_GetSessionAPIPaginationList(tApp, "GetTags",
                                                                  [{ VarName: "ObjectList_SelectFormat", VarRef: "TagNameResponseSelectFormat" },
                                                                   { VarName: "ObjectList_WhereObjectType", VarRef: "UserTagNameObjectType"}],
                                                                  { action: "FNCCALL", context: cmpMtxUserList,
                                                                    fnc: function (respCode, respVal) {
                                                                      if (respCode == "SUCCESS" && respVal != null) {
                                                                        var filterTagData = respVal.cloneDocument();
                                                                        var filterdata = getFilterXSLDoc.transform(filterTagData);
                                                                        var filterTagNode = new jsx3.xml.Document();
                                                                        filterTagNode.loadXML(filterdata);
                                                                        this.setFilterTagDoc(respCode, filterTagNode);
                                                                      }
                                                                      else {
                                                                        log.info("GetTagsRequest failed. Error Code :" + respCode);
                                                                        this.onResponseErrorHandle("GetTags", respCode, respVal);
                                                                      }
                                                                    }
                                                                  }
                                                                );
                                                                 
  cmpMtxUserList.RefreshData = null;
  cmpMtxUserList.RefreshColArr = null;                                                   

  cmpMtxUserList.MenuEvent = {action:"APPEVENT", eventvar:"UserMenuSelected", eventval:"jsxid"};
                                          
  var cmpUserCustomize = new cmpCustomizeContainer(["blkUser","blkUsersCustomize"],
                                                    [["blkUsersCustomize","mtxCustomize"],
                                                     ["blkUsersCustomize","btnCustomizeApply"],
                                                     ["blkUsersCustomize","btnCustomizeCancel"],
                                                     ["blkUsersCustomize","btnRefreshColumnList"],
                                                     ["blkUsersCustomize", "btnCustomizeClose"],
                                                     ["blkUsersCustomize", "mtxMetaDataTable"],
                                                     ["blkUsersCustomize", "imgbtnAddMetaData"],
                                                     ["blkUsersCustomize", "imgbtnDeleteMetaData"],
                                                     ["blkUsersCustomize", "CustomizeLayout"],
                                                     ["blkUsersCustomize", "blkMetaData"],
                                                     ["HideMetaData"]
                                                    ],
                                                    cmpMtxUserList,
                                                    tApp,
                                                    {action:"TOPICEVENT", topic:tApp, eventvar:"UserCustomize", eventval:"NONE"},
                                                    {action:"FNCCALL", context:cmpUserCustomize,
                                                     fnc:function() {
                                                           var strXMLCacheId = cmpUserCustomize.cmpMtxCustomize.BlkJSX.getXMLId();
                                                           var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
                                                           cmpUserCustomize.MtxObjectList.columnMtxDoc = objXML;
                                                           //save the columns doc in the Perferences
                                                           setLocalPreferences("GLOBAL", ".//ClientApp[@name='IPVSAdmin']/CustomizeList/User", objXML);
                                                           //get the noAutoDisplayTags val if not there then add the attribute
                                                           var noAutoDisplayTags = getLocalPreferences("GLOBAL", ".//ClientApp[@name='IPVSAdmin']/CustomizeList/User/@noAutoDisplayTags", null);
                                                           if (jsx3.util.strEmpty(noAutoDisplayTags)) {
                                                             var addSuccess = addAttributeLocalPreferences("GLOBAL", ".//ClientApp[@name='IPVSAdmin']/CustomizeList/User", "noAutoDisplayTags", "false");
                                                             log.info("Added noAutoDisplayTags for User in Preferences.xml :" + addSuccess);
                                                           }
                                                           cmpUserCustomize.MtxObjectList.setMtxColumn();
                                                           //cmpUserCustomize.MtxObjectList.doRepaint();
                                                           topic_Publish(tApp, "UserCustomize", "NONE");
                                                         }
                                                    },
                                                    [{action:"APPEVENT", eventvar:"UserColumnDataUpdated", eventval:null}],
                                                    null,
                                                    null
                                                   );                                                   

  var cmpBlkUserCustomize = new CmpContainer("blkUser", 
                                              [{cmp:cmpUserCustomize, sel: "topic_Get(tApp, 'UserCustomize') != 'NONE'"}],
                                              [{ action:"APPEVENT", eventvar: "UserCustomize", eventval: null }],
                                              null ); 
                                               
  var cmpBtnUserCustomize = new CmpButton("btnCustomizeUser",
                                             null,
                                             { action: "FNCCALL", context: null,
                                                 fnc: function () {
                                                     topic_Publish(tApp, "UserColumnDataUpdated", (newServerDate()).getTime());
                                                     jsx3.sleep(function () {
                                                         topic_Publish(tApp, "UserCustomize", "OPEN");
                                                     }, null, this); 
                                               }
                                             }
                                           );

  var cmpUserFilter = new cmpFilterContainer(["blkUser","blkUsersFilter"],
                                              [["blkUsersFilter","blkTagOrg"],
                                               ["blkUsersFilter","blkTagList"],
                                               ["blkUsersFilter","txtNameSearch"],
                                               ["blkUsersFilter","chkNameSearch"],
                                               ["blkUsersFilter","chkDescriptionSearch"],
                                               ["blkUsersFilter","btnFilterCancel"],
                                               ["blkUsersFilter","btnFilterApply"],
                                               ["blkUsersFilter","btnFilterClearAll"],
                                               ["blkUsersFilter", "btnFilterClose"],
                                               ["blkUsersFilter", "btnFilterRefresh"],
                                               ["blkUsersFilter", "btnFilterRefreshSpinner"],
                                               ["blkUsersFilter", "btnFilterRefreshNotify"],
                                               ["blkUsersFilter", "chkBookmarksSearch"],
                                               ["blkUsersFilter", "chkMetaDataSearch"]
                                              ],
                                              cmpMtxUserList,
                                              tApp,
                                              {action:"TOPICEVENT", topic:tApp, eventvar:"UserFilter", eventval:"NONE"},
                                              {action:"FNCCALL", context:cmpUserFilter,
                                                 fnc:function() {
                                                       var UserWhereTextSubStr = cmpUserFilter.cmpTagList.getTextSubStr();
                                                       topic_Set(tApp, "UserWhereTextSubStr", UserWhereTextSubStr);
                                                       
                                                       var UserWhereTagExpr = cmpUserFilter.cmpTagList.getAPITagExpr();
                                                       topic_Set(tApp, "UserWhereTagExpr", UserWhereTagExpr);
                                                       
                                                       cmpMtxUserList.ResetToStart(1,UserCountToFetch,0);
                                                       cmpUserFilter.MtxObjectList.doRepaint();
                                                       topic_Publish(tApp, "UserFilter", "NONE");
                                                     }
                                              },
                                              [{action:"APPEVENT", eventvar:"UserFilterTagDataUpdated", eventval:null}],
                                              null,
                                              null
                                             );
                                                   
  var cmpBlkUserFilter = new CmpContainer("blkUser", 
                                          [{cmp:cmpUserFilter, sel: "topic_Get(tApp, 'UserFilter') != 'NONE'"}],
                                          [{ action:"APPEVENT", eventvar: "UserFilter", eventval: null }],
                                          null ); 
                                              
  var cmpBtnUserFilter = new CmpButton("btnFilterUser",
                                       null,
                                       { action: "FNCCALL", context: null,
                                         fnc: function() {
                                             //cmpUserFilter.cmpBtnClearAll.clearAllSelection();
                                             jsx3.sleep(function () {
                                                 topic_Publish(tApp, "UserFilter", "OPEN");
                                             }, null, this);
                                         }
                                       }
                                     );
                                         
  var cmpUserNavControls = new CmpPageNavButtonsContainer(["blkUser","blkUserFooter"],
                                                              [["blkUser", "btnPageFirst"],
                                                               ["blkUser", "btnPagePrev"],
                                                               ["blkUser", "btnPageNext"],
                                                               ["blkUser", "btnPageLast"],
                                                               ["blkUser", "lblRecordInfo"],
                                                               ["blkUser", "txtCurrentRecordNumber"],
                                                               ["blkUser", "btnPageRefresh"],
                                                               ["blkUser", "btnPageRefreshSpinner"],
                                                               ["blkUser", "btnPageRefreshNotify"]
                                                              ],
                                                              cmpMtxUserList,
                                                              [{action:"TOPICEVENT", topic:tApp, eventvar:"UserDataUpdated", eventval:null}],
                                                              null
                                                             );

  eventApp.subscribe("UserMenuSelected", null, this,
                      function () {
                        if (clsUserPermission.getPermission('User', 'modify') == "false") {
                          return;
                        }
                        var UserMenuSelected = topic_Get(tApp, 'UserMenuSelected');
                        var cmpMtxUserListRecord = cmpMtxUserList.BlkJSX.getRecordNode(UserMenuSelected);
                        var UserType = cmpMtxUserListRecord.getAttribute("jsxType");
                        var UserName = cmpMtxUserListRecord.getAttribute("jsxtext");
                        if (UserName.toLowerCase() == "admin") {
                          UserType = "SuperUser";
                        }
                        topic_Publish(tApp, "UserMenuTypeSelected", UserType);
                      }
                    );
  
  eventApp.subscribe("UserMenuActionSelected", null, this,
                      function() {
                        var UserMenuActionSelected = topic_Get(tApp, 'UserMenuActionSelected');
                        this.SelectedUserNID = topic_Get(tApp, "UserSelected");                       

                        var selRecord = cmpMtxUserList.BlkJSX.getRecord(this.SelectedUserNID);
                        if(selRecord == null) {
                          alert("Please select user");
                          return false;                        
                        }
                        else if( UserMenuActionSelected == "Edit..." ) {
                          var userName = selRecord.jsxtext;
                          if( userName.toLowerCase() == "admin" ){
                            alert("Edit Not Allowed");
                            return false;
                          }
                          else{
                            dlgEditUser_Create(this.SelectedUserNID);
                          }
                        }
                      }
                    );
                                                   
  var cmpMenuUserList = new CmpMenu("menuUserList",
                                    [{ObjectType:"User",
                                      MenuListArr:["Edit..."] },
                                     {ObjectType:"BotUser",
                                       MenuListArr: ["Edit Not Allowed"]},
                                     { ObjectType: "SuperUser",
                                       MenuListArr: ["Edit Not Allowed"]
                                     }
                                    ],
                                    null, null, null,
                                    [{action:"APPEVENT", eventvar:"UserMenuTypeSelected", eventval:null}],
                                    {action:"APPEVENT", eventvar:"UserMenuActionSelected", eventval:"jsxtext"}
                                  );

   var cmpBtnDeleteUser = new CmpImageButton("btnDeleteUser", null,
                                             { action: "FNCCALL", context: this,
                                               fnc: function () {
                                                 var selRecordNodes = cmpMtxUserList.BlkJSX.getSelectedNodes();
                                                 var agree = confirm('Are you sure you want to delete selected users? ');
                                                 cmpMtxUserList.deleteRequestCounter = 0;
                                                 if (agree) {
                                                   for (var it = selRecordNodes.iterator(); it.hasNext(); ) {
                                                     ++cmpMtxUserList.deleteRequestCounter;
                                                     var selRecordNode = it.next();
                                                     this.SelectedUserNID = selRecordNode.getAttribute("jsxid");
                                                     var objectType = selRecordNode.getAttribute("jsxType");
                                                     cmpBlkUserDelete(this.SelectedUserNID, objectType);
                                                   }
                                                 }
                                               }
                                             });
 
  
  var cmpBtnAddUser = new CmpButton("btnAddUser",
                                   null, 
                                   {action:"FNCCALL", 
                                    context:this,
                                        fnc:function() {
                                           dlgEditUser_Create("DEFAULT_USER_NID");
                                        }
                                      });
                                   
  var cmpBlkAddUser = new CmpContainer("blkAddUser",
                                        [{ cmp: cmpBtnAddUser}],
                                        null,
                                        null);

  var cmpBlkDeleteUser = new CmpContainer("blkDeleteUser",
                                        [{ cmp: cmpBtnDeleteUser, sel: "topic_Get(tApp, 'DeleteMultiUser') == 'VALID'"}],
                                        null,
                                        null);  
  var cmpBlkUserList = new CmpContainer( "blkUser",  
                                    [{cmp:cmpBlkAddUser,
                                       sel: "clsUserPermission.getPermission('User', 'add') == 'true'"},
                                     {cmp:cmpBlkDeleteUser,
                                       sel: "clsUserPermission.getPermission('User', 'delete') == 'true'"},
                                     {cmp:cmpMenuUserList},
                                     {cmp:cmpUserNavControls}  
                                    ],
                                    [{action:"APPEVENT", eventvar:"DeleteMultiUser", eventval:null}]
                                    );
                                                                                                                                                              
  var cmpBlkUser = new CmpContainer( "blkUser",
                                    [{ cmp: cmpMtxUserList },
                                     { cmp: cmpBlkUserList },
                                     {cmp:cmpBtnUserCustomize},
                                     {cmp:cmpBlkUserCustomize},
                                     {cmp:cmpBtnUserFilter},
                                     {cmp:cmpBlkUserFilter}  
                                    ],
                                    null//[{action:"APPEVENT", eventvar:"DeleteMultiUser", eventval:null}]
                                    );
  
  //Delete button is enabled, when user State is offline and user Type is non BotUser(DEVICE_BOT_USERS).
  eventApp.subscribe("UserSelected", null, this,
                    function() {
                       cmpBlkUserSelection();
                    }
                  );

  function cmpBlkUserSelection() {
    var deleteMultiUser = "VALID";
    var isUserSelected = false;
    var MenuSelection = topic_Get(tApp, 'MenuSelection');
    if(MenuSelection == "USER" ) {
      var selRecordNodes = cmpMtxUserList.BlkJSX.getSelectedNodes();
      for (var it = selRecordNodes.iterator(); it.hasNext(); ) {
        var selRecordNode = it.next();
        isUserSelected = true;
        var userType = selRecordNode.getAttribute("jsxType");
        var userState = selRecordNode.getAttribute("jsxStateFlag");
        var userName = selRecordNode.getAttribute("jsxtext");
        if (userType == "BotUser" || userState == "Ready" || userName.toLowerCase() == "admin") {
          deleteMultiUser = "NONE";
          break;
        }
      }
      if(isUserSelected){
       topic_Publish(tApp, "DeleteMultiUser", deleteMultiUser);
      }
      else{
       topic_Publish(tApp, "DeleteMultiUser", "NONE");
      }
    }
  }

  eventApp.subscribe("MENUUIDATA", "USER", this,
                      function(EventVar, EventVal) {
                        if( EventVal == "USER" ) {
                          var getDataState = topic_Get(tApp, "UserGetDataState");
                          if(getDataState == "NONE") {
                            setObjectCacheDefaultXML("User", "JSXAPPS/IPVS-Common/model/objectXml/User.xml", "DEFAULT_USER_NID");
                            topic_Set(tApp, "EditUserDataState", "DEFAULT_USER_NID");
                                                    
                            topic_Set(tApp, "UserGetDataState", (newServerDate()).getTime() );
                            cmpMtxUserList.ResetToStart(1,UserCountToFetch,0);
                            topic_Publish(tApp, "UserGetData", (newServerDate()).getTime() ); 
                          }
                        }
                      }
                    );   
                    
  eventApp.subscribe("DBUpdate", null, this,
                     function(EventVar, EventVal) {
                       if( jsx3.util.strEmpty(EventVal) ) return;
                       if( EventVal.objType == "User" ) {
                         cmpMtxUserList.handleNotification(EventVal);
                       }
                       if (EventVal.objType == "GroupTagDef") {
                         cmpUserFilter.handleNotification(EventVal);
                       }                                                          
                     });

  eventApp.subscribe("UserDataUpdate", null, this,
                      function(EventVar, EventVal) {
                        if( EventVal == "ADD" || EventVal == "DELETE" ||  EventVal == "EDIT") {
                          //cmpMtxUserList.getColumnsFromDB();
                          cmpMtxUserList.doRepaint();
                        }
                      }
                    ); 
                          
  return cmpBlkUser;
  
  function cmpBlkUserDelete(userNID, objectType) {
    var reqDelUser = new ReqDeleteObjectNID(userNID, objectType, 
                                             {action:"FNCCALL", 
                                                context:this,
                                                  fnc:function(respCode, respVal) {
                                                        if (respCode == "SUCCESS" && respVal != null) {
                                                          --cmpMtxUserList.deleteRequestCounter < 0 ? 0 : cmpMtxUserList.deleteRequestCounter;
                                                          topic_Publish(tApp, "DeleteMultiUser", "NONE");
                                                          if (cmpMtxUserList.deleteRequestCounter == 0) {
                                                            topic_Publish(tApp, "UserDataUpdate", "DELETE");
                                                          }

                                                        } else {
                                                           --cmpMtxUserList.deleteRequestCounter < 0 ? 0 : cmpMtxUserList.deleteRequestCounter;
                                                           var codeErrorMsg = CommonErrorCodeArr[respCode].errorDesc;
                                                           var ErrorMsg = "Error: User delete request failed \n";
                                                           ErrorMsg += "Error description: " + codeErrorMsg + "\n";
                                                           ErrorMsg += "Error code: " + respCode;
                                                           alert(ErrorMsg);
                                                        }
                                                      }
                                             }
                                           );
    reqDelUser.sendRequest();
  }
}
function cmpBlkUser_Destroy() {
  var blkUserBlkJSX = PNameSpace.getJSXByName("blkUser");
  blkUserBlkJSX.removeChildren();
}