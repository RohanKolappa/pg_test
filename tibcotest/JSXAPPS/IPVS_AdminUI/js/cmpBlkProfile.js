function cmpBlkProfile_Create() {
  var blkProfileBlkJSX = PNameSpace.getJSXByName("blkProfiles");
  blkProfileBlkJSX.load("components/assets/profiles.xml");

  topic_Set(tApp, "ProfileGetDataState", "NONE");
  topic_Set(tApp, "ProfileCustomize", "NONE");
  topic_Set(tApp, "ProfileWhereXPath", ""); 
  topic_Set(tApp, "ProfileFilter", "NONE"); 
  
  topic_Set(tApp, "DisplayProfileGetDataState", "NONE");
  topic_Set(tApp, "DisplayProfileWhereXPath", "");
  topic_Set(tApp, "ProfileTagNameObjectType", "Profiles");    
  var ProfileCountToFetch = 50;
  
  var cmpMtxProfileList = new CmpCustomPaginationList(["blkProfileList","mtxProfileList"],
                                                    [{action:"APPEVENT", eventvar:"ProfileGetData", eventval:null}],
                                                    {persist:{col:"jsxid",initval:false},
                                                     action:"APPEVENT", eventvar:"ProfileSelected", eventval:"jsxid"},
                                                    null,
                                                    null
                                                   );   
  cmpMtxProfileList.allowMultiSelection = true;
  cmpMtxProfileList.deleteRequestCounter = 0;

  //translate($b/Info/Title ,'ABCDEFGHIJKLMNOPQRSTUVWXYZ','abcdefghijklmnopqrstuvwxyz')
  cmpMtxProfileList.SortPathArr = {jsxtext:"upper-case($b/Title)",
                                   jsxProfileStreamType:"upper-case($b/StreamType)",
                                   jsxindex:"$b/Priority/number(.)",
                                   jsxProfileDescription:"upper-case($b/Description)",
                                   jsxTagValue:"string-join ( $b/Groups/Tag[@Name='_TAGNAME_']/@Value, ',')"

                                 };
  cmpMtxProfileList.cmpReqParamArr = { StartFrom: "Op_Select_ListSpec_StartFrom",
                                       CountToFetch: "Op_Select_ListSpec_CountToFetch",
                                       SortOrder: "Op_Select_ListSpec_SortOrder",
                                       ObjectCount: "ObjectCount",
                                       SortByXpath: "Op_Select_ListSpec_SortByXPathList_SortByXpath"
                                     };                                  
                                  
  cmpMtxProfileList.DataUpdatedEvent = {action:"TOPICEVENT", topic:tApp, eventvar:"ProfileDataUpdated", eventval:"LOADED"};                       
  
  cmpMtxProfileList.cmpReq = new cmpReq_GetListReadQuery(tApp, "JSXAPPS/IPVS_AdminUI/query/MediaStreamProfile.xml",
                                                         [{VarName:"_WHEREXPATH_", VarRef:"ProfileWhereXPath"}
                                                         ],
                                                         {action: "FNCCALL", context: cmpMtxProfileList,
                                                          fnc: function(respCode, respVal) {
                                                              if (respCode == "SUCCESS" && respVal != null) {
                                                                this.onResponse(respCode, respVal);
                                                              }
                                                              else{
                                                                log.info("GetMediaStreamProfileRequest failed. Error Code :" + respCode);
                                                                this.onResponseErrorHandle("GetMediaStreamProfile",respCode, respVal);
                                                              }
                                                            }
                                                         } 
                                                        );

  cmpMtxProfileList.cmpItemByNIDReqFnc = function () {
                                           topic_Set(tApp, "WhereProfileNID", "[//@NID=(\"" + cmpMtxProfileList.WhereNIDArr.join("\",\"") + "\")]");
                                           return new cmpReq_GetListReadQuery(tApp, "JSXAPPS/IPVS_AdminUI/query/MediaStreamProfile.xml",
                                                                              [{VarName:"_WHEREXPATH_", VarRef:"WhereProfileNID"}],
                                                                              {action: "FNCCALL", context: cmpMtxProfileList,
                                                                               fnc: function(respCode, respVal) {
                                                                                      if (respCode == "SUCCESS" && respVal != null) {
                                                                                        this.onResponseUpdateRecord(respCode, respVal);
                                                                                      }
                                                                                      else{
                                                                                        log.info("GetMediaStreamProfileRequest failed. Error Code :" + respCode);
                                                                                        this.onResponseErrorHandle("GetMediaStreamProfile",respCode, respVal);
                                                                                      }
                                                                                    }
                                                                              }, "ItemByNID"
                                                                             );
                                         }   
  
  cmpMtxProfileList.columnDataState = null;
  cmpMtxProfileList.columnMtxDoc = new jsx3.xml.CDF.Document.newDocument();
  cmpMtxProfileList.screenName = "ConnectionProfile";
  cmpMtxProfileList.ColumnDataUpdatedEvent = {action:"TOPICEVENT", topic:tApp, eventvar:"ProfileColumnDataUpdated", eventval:"DATE"};
  cmpMtxProfileList.InsertBeforeColumnName = "jsxProfileDescription";
  cmpMtxProfileList.columnCmpReq = new cmpReq_GetSessionAPIPaginationList(tApp, "GetTags",
                                                                  [{ VarName: "ObjectList_SelectFormat", VarRef: "TagNameResponseSelectFormat" },
                                                                   { VarName: "ObjectList_WhereObjectType", VarRef: "ProfileTagNameObjectType"}],
                                                                  { action: "FNCCALL", context: cmpMtxProfileList,
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
                                               
  cmpMtxProfileList.filterTagDataState = null;
  cmpMtxProfileList.filterTagDoc = new jsx3.xml.CDF.Document.newDocument();
  cmpMtxProfileList.filterTagDataUpdatedEvent = { action: "TOPICEVENT", topic: tApp, eventvar: "ProfileFilterTagDataUpdated", eventval: "LOADED" };
  cmpMtxProfileList.filterTagCmpReq = new cmpReq_GetSessionAPIPaginationList(tApp, "GetTags",
                                                                  [{ VarName: "ObjectList_SelectFormat", VarRef: "TagNameResponseSelectFormat" },
                                                                   { VarName: "ObjectList_WhereObjectType", VarRef: "ProfileTagNameObjectType"}],
                                                                  { action: "FNCCALL", context: cmpMtxProfileList,
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
                                                                 
  cmpMtxProfileList.RefreshData = null;
  cmpMtxProfileList.RefreshColArr = null;
  
  cmpMtxProfileList.MenuEvent = {action:"APPEVENT", eventvar:"ProfileMenuSelected", eventval:"jsxid"};
                                          
  var cmpProfileCustomize = new cmpCustomizeContainer(["blkConnectionProfile","blkProfileCustomize"],
                                                    [["blkProfileCustomize","mtxCustomize"],
                                                     ["blkProfileCustomize","btnCustomizeApply"],
                                                     ["blkProfileCustomize","btnCustomizeCancel"],
                                                     ["blkProfileCustomize","btnRefreshColumnList"],
                                                     ["blkProfileCustomize", "btnCustomizeClose"],
                                                     ["blkProfileCustomize", "mtxMetaDataTable"],
                                                     ["blkProfileCustomize", "imgbtnAddMetaData"],
                                                     ["blkProfileCustomize", "imgbtnDeleteMetaData"],
                                                     ["blkProfileCustomize", "CustomizeLayout"],
                                                     ["blkProfileCustomize", "blkMetaData"],
                                                     ["HideMetaData"]
                                                    ],
                                                    cmpMtxProfileList,
                                                    tApp,
                                                    {action:"TOPICEVENT", topic:tApp, eventvar:"ProfileCustomize", eventval:"NONE"},
                                                    {action:"FNCCALL", context:cmpProfileCustomize,
                                                     fnc:function() {
                                                           var strXMLCacheId = cmpProfileCustomize.cmpMtxCustomize.BlkJSX.getXMLId();
                                                           var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
                                                           cmpProfileCustomize.MtxObjectList.columnMtxDoc = objXML;
                                                           //save the columns doc in the Perferences
                                                           setLocalPreferences("GLOBAL", ".//ClientApp[@name='IPVSAdmin']/CustomizeList/ConnectionProfile", objXML);
                                                           //get the noAutoDisplayTags val if not there then add the attribute
                                                           var noAutoDisplayTags = getLocalPreferences("GLOBAL", ".//ClientApp[@name='IPVSAdmin']/CustomizeList/ConnectionProfile/@noAutoDisplayTags", null);
                                                           if (jsx3.util.strEmpty(noAutoDisplayTags)) {
                                                             var addSuccess = addAttributeLocalPreferences("GLOBAL", ".//ClientApp[@name='IPVSAdmin']/CustomizeList/ConnectionProfile", "noAutoDisplayTags", "false");
                                                             log.info("Added noAutoDisplayTags for ConnectionProfile in Preferences.xml :" + addSuccess);
                                                           }
                                                           cmpProfileCustomize.MtxObjectList.setMtxColumn();
                                                           //cmpProfileCustomize.MtxObjectList.doRepaint();
                                                           topic_Publish(tApp, "ProfileCustomize", "NONE");
                                                         }
                                                    },
                                                    [{action:"APPEVENT", eventvar:"ProfileColumnDataUpdated", eventval:null}],
                                                    null,
                                                    null
                                                   );  
  
  var cmpBlkProfileCustomize = new CmpContainer("blkConnectionProfile", 
                                              [{cmp:cmpProfileCustomize, sel: "topic_Get(tApp, 'ProfileCustomize') != 'NONE'"}],
                                              [{ action:"APPEVENT", eventvar: "ProfileCustomize", eventval: null }],
                                              null ); 
                                               
  var cmpBtnProfileCustomize = new CmpButton(["blkConnectionProfile","btnCustomizeProfile"],
                                             null,
                                             { action: "FNCCALL", context: null,
                                               fnc: function() {
                                                   topic_Publish(tApp, "ProfileColumnDataUpdated", (newServerDate()).getTime());
                                                   jsx3.sleep(function () {
                                                       topic_Publish(tApp, "ProfileCustomize", "OPEN");
                                                   }, null, this); 
                                               }
                                             }
                                           );  
  
  var cmpProfileFilter = new cmpFilterContainer(["blkConnectionProfile","blkProfileFilter"],
                                              [["blkConnectionProfile","blkTagOrg"],
                                               ["blkConnectionProfile","blkTagList"],
                                               ["blkConnectionProfile","txtNameSearch"],
                                               ["blkConnectionProfile","chkNameSearch"],
                                               ["blkConnectionProfile","chkDescriptionSearch"],
                                               ["blkConnectionProfile","btnFilterCancel"],
                                               ["blkConnectionProfile","btnFilterApply"],
                                               ["blkConnectionProfile","btnFilterClearAll"],
                                               ["blkConnectionProfile", "btnFilterClose"],
                                               ["blkConnectionProfile", "btnFilterRefresh"],
                                               ["blkConnectionProfile", "btnFilterRefreshSpinner"],
                                               ["blkConnectionProfile", "btnFilterRefreshNotify"],
                                               ["blkConnectionProfile", "chkBookmarksSearch"],
                                               ["blkConnectionProfile", "chkMetaDataSearch"]
                                              ],
                                              cmpMtxProfileList,
                                              tApp,
                                              {action:"TOPICEVENT", topic:tApp, eventvar:"ProfileFilter", eventval:"NONE"},
                                              {action:"FNCCALL", context:cmpProfileFilter,
                                                 fnc:function() {
                                                       var profileWhereXPath = cmpProfileFilter.cmpTagList.getWhereXPath();
                                                       topic_Set(tApp, "ProfileWhereXPath", profileWhereXPath);
                                                       cmpMtxProfileList.ResetToStart(1,ProfileCountToFetch,0);
                                                       cmpProfileFilter.MtxObjectList.doRepaint();
                                                       topic_Publish(tApp, "ProfileFilter", "NONE");
                                                     }
                                              },
                                              [{action:"APPEVENT", eventvar:"ProfileFilterTagDataUpdated", eventval:null}],
                                              null,
                                              null
                                             );
                                                   
  var cmpBlkProfileFilter = new CmpContainer(["blkConnectionProfile","blkProfiles"], 
                                            [{cmp:cmpProfileFilter, sel: "topic_Get(tApp, 'ProfileFilter') != 'NONE'"}],
                                            [{ action:"APPEVENT", eventvar: "ProfileFilter", eventval: null }],
                                            null ); 
                                              
  var cmpBtnProfileFilter = new CmpButton(["blkConnectionProfile","btnFilterProfile"],
                                           null,
                                           { action: "FNCCALL", context: null,
                                             fnc: function() {
                                                 //cmpProfileFilter.cmpBtnClearAll.clearAllSelection();
                                                 jsx3.sleep(function () {
                                                     topic_Publish(tApp, "ProfileFilter", "OPEN");
                                                 }, null, this); 
                                             }
                                           }
                                         );
  
  var cmpProfileNavControls = new CmpPageNavButtonsContainer(["blkConnectionProfile","blkProfileFooter"],
                                                              [["blkConnectionProfile", "btnPageFirst"],
                                                               ["blkConnectionProfile", "btnPagePrev"],
                                                               ["blkConnectionProfile", "btnPageNext"],
                                                               ["blkConnectionProfile", "btnPageLast"],
                                                               ["blkConnectionProfile", "lblRecordInfo"],
                                                               ["blkConnectionProfile", "txtCurrentRecordNumber"],
                                                               ["blkConnectionProfile", "btnPageRefresh"],
                                                               ["blkConnectionProfile", "btnPageRefreshSpinner"],
                                                               ["blkConnectionProfile", "btnPageRefreshNotify"]
                                                              ],
                                                              cmpMtxProfileList,
                                                              [{action:"TOPICEVENT", topic:tApp, eventvar:"ProfileDataUpdated", eventval:null}],
                                                              null
                                                             ); 
  
  eventApp.subscribe("ProfileMenuSelected", null, this,
                      function() {
                        if (clsUserPermission.getPermission("Profile", "modify") == "false") {
                          return;
                        }                      
                        var ProfileMenuSelected = topic_Get(tApp, 'ProfileMenuSelected');
                        var cmpMtxProfileListRecord = cmpMtxProfileList.BlkJSX.getRecordNode(ProfileMenuSelected);
                        var ProfileType = cmpMtxProfileListRecord.getAttribute("jsxProfileStreamType");
                        topic_Publish(tApp, "ProfileMenuTypeSelected", ProfileType);
                      }
                    );
                    
  eventApp.subscribe("ProfileMenuActionSelected", null, this,
                      function() {
                        var ProfileMenuActionSelected = topic_Get(tApp, 'ProfileMenuActionSelected');
                        this.SelectedProfileNID = topic_Get(tApp, "ProfileSelected");                       

                        var selRecord = cmpMtxProfileList.BlkJSX.getRecord(this.SelectedProfileNID);
                        if(selRecord == null) {
                          alert("Please select profile");
                          return false;                        
                        }
                        else if( ProfileMenuActionSelected == "Edit..." ) {
                          dlgEditProfile_Create(this.SelectedProfileNID);
                        }
                      }
                    );
                                        
  var cmpMenuProfileList = new CmpMenu(["blkConnectionProfile","menuProfileList"],
                                        [{ObjectType:"MPEGTS",
                                          MenuListArr:["Edit..."] },
                                         {ObjectType:"RTP",
                                          MenuListArr:["Edit..."] },
                                         {ObjectType:"V2D",
                                          MenuListArr:["Edit..."] },
                                         {ObjectType:"UDP",
                                          MenuListArr:["Edit..."] },
                                         {ObjectType:"RTP-RAW",
                                          MenuListArr:["Edit..."] },
                                         {ObjectType:"RTP-PCM",
                                          MenuListArr:["Edit..."] },
                                         {ObjectType:"RFB",
                                          MenuListArr:["Edit..."] }
                                        ],
                                        null, null, null,
                                        [{action:"APPEVENT", eventvar:"ProfileMenuTypeSelected", eventval:null}],
                                        {action:"APPEVENT", eventvar:"ProfileMenuActionSelected", eventval:"jsxtext"}
                                      );

  var cmpBtnDeleteProfile = new CmpButton(["blkConnectionProfile", "btnDeleteProfile"],
                                           [{ action: "APPEVENT", eventvar: "ProfileSelected", eventval: null}],
                                           { action: "FNCCALL",
                                             context: this,
                                             fnc: function () {
                                               var selRecordNodes = cmpMtxProfileList.BlkJSX.getSelectedNodes();
                                               var agree = confirm('Are you sure you want to delete selected profiles?');
                                               cmpMtxProfileList.deleteRequestCounter = 0;
                                               if (agree) {
                                                 for (var it = selRecordNodes.iterator(); it.hasNext(); ) {
                                                   ++cmpMtxProfileList.deleteRequestCounter;
                                                   var selRecordNode = it.next();
                                                   this.SelectedProfileNID = selRecordNode.getAttribute("jsxid");
                                                   var objectType = selRecordNode.getAttribute("jsxProfileStreamType");
                                                   if (objectType != null) {
                                                     objectType = objectType + "MediaStreamProfile";
                                                   }
                                                   cmpBlkProfileDelete(this.SelectedProfileNID, objectType);
                                                 }
                                               }
                                             }
                                           });
  
  var cmpBtnAddProfile = new CmpButton(["blkConnectionProfile","btnAddProfile"],
                                         null, 
                                         {action:"FNCCALL", 
                                          context:this,
                                              fnc:function() {
                                                 dlgEditProfile_Create("DEFAULT_V2D-STREAMPROFILE_NID");
                                              }
                                            });

  var cmpBlkAddProfile = new CmpContainer("blkAddProfile",
                                          [{ cmp: cmpBtnAddProfile}],
                                          null,
                                          null);

  var cmpBlkDeleteProfile = new CmpContainer("blkDeleteProfile",
                                            [{ cmp: cmpBtnDeleteProfile}],
                                            null,
                                            null);
                                                                                                                                                   
  var cmpBlkConnectionProfile = new CmpContainer("blkConnectionProfile", 
                                                  [{cmp:cmpMtxProfileList},
                                                   { cmp: cmpBlkAddProfile,
                                                     sel: "clsUserPermission.getPermission('Profile', 'add') == 'true'"},
                                                   { cmp: cmpBlkDeleteProfile,
                                                     sel: "clsUserPermission.getPermission('Profile', 'delete') == 'true'"},
                                                   {cmp: cmpMenuProfileList},
                                                   {cmp:cmpProfileNavControls},
                                                   {cmp:cmpBtnProfileCustomize},
                                                   {cmp:cmpBlkProfileCustomize},
                                                   {cmp:cmpBtnProfileFilter},
                                                   {cmp:cmpBlkProfileFilter}  
                                                  ],
                                                  null,
                                                  null ); 
                                                  
  /***************************** END CONNECTION PROFILE *******************/
  
  /***************************** START DISPLAY PROFILE ********************/
  var cmpBtnDeleteCanvas = new CmpImageButton(["blkDisplayProfile","btnDeleteCanvas"], null,
                                                     {action:"FNCCALL", context:this,
                                                          fnc:function() {
                                                            var selRecordNodes = cmpDisplayProfilePanelHidden.BlkJSX.getSelectedNodes();
                                                            var agree = confirm('Are you sure you want to delete selected display profiles?');
                                                            if(agree) {
                                                              for (var it = selRecordNodes.iterator(); it.hasNext(); ) {
                                                                var selRecordNode = it.next();
                                                                this.SelectedDisplayProfileNID = selRecordNode.getAttribute("jsxid");
                                                                cmpBlkDisplayProfileDelete(this.SelectedDisplayProfileNID);
                                                              }
                                                            }
                                                          }
                                                     });

  var cmpBtnAddCanvas = new CmpImageButton(["blkDisplayProfile", "btnAddCanvas"],
                                             null, 
                                             {action:"FNCCALL", 
                                              context:this,
                                                  fnc:function() {
                                                    dlgEditDisplayProfile_Create("DEFAULT_DISPLAYPROFILE_NID");   
                                                  }
                                             });      
                                       
  var cmpBtnEditCanvas = new CmpImageButton(["blkDisplayProfile","btnEditCanvas"], null,
                                                   {action:"FNCCALL", context:this,
                                                        fnc:function() {
                                                          this.SelectedDisplayProfileNID = topic_Get(tApp, "DisplayProfileSelected");
                                                          dlgEditDisplayProfile_Create(this.SelectedDisplayProfileNID);
                                                           
                                                        }
                                                      });  
                                                                                        
  var cmpDisplayProfilePanel = new CmpCanvas(["blkDisplayProfile","BlockDisplayProfilePanel"],
                                                         null,
                                                         { persist: { col: "jsxid", initval: false },
                                                           action: "TOPICEVENT", topic: tApp, eventvar: "DisplayProfileSelected", eventval: "jsxid" }
                                                        );
  cmpDisplayProfilePanel.allowMultiSelection = true;
  
  var cmpDisplayProfilePanelHidden = new CmpCustomPaginationList(["blkDisplayProfile","mtxDisplayProfileHidden"],
                                                                [{action:"APPEVENT", eventvar:"DisplayProfileGetData", eventval:null}],
                                                                {persist:{col:"jsxid",initval:false},
                                                                 action:"APPEVENT", eventvar:"DisplayProfileSelected", eventval:"jsxid"},
                                                                null,
                                                                null
                                                               );   
  
  //translate($b/Info/Title ,'ABCDEFGHIJKLMNOPQRSTUVWXYZ','abcdefghijklmnopqrstuvwxyz')
  cmpDisplayProfilePanelHidden.SortPathArr = {jsxtext:"upper-case($b/Title)",
                                               jsxPriority:"$b/Priority/number(.)",
                                               jsxDescription:"upper-case($b/Description)"
                                              };
                                     
  cmpDisplayProfilePanelHidden.cmpReqParamArr = { StartFrom: "Op_Select_ListSpec_StartFrom",
                                                 CountToFetch: "Op_Select_ListSpec_CountToFetch",
                                                 SortOrder: "Op_Select_ListSpec_SortOrder",
                                                 ObjectCount: "ObjectCount",
                                                 SortByXpath: "Op_Select_ListSpec_SortByXPathList_SortByXpath"
                                               };                                  
                                  
  cmpDisplayProfilePanelHidden.DataUpdatedEvent = {action:"TOPICEVENT", topic:tApp, eventvar:"DisplayProfileDataUpdated", eventval:"LOADED"};

  cmpDisplayProfilePanelHidden.cmpReq = new cmpReq_GetListReadQuery(tApp, "JSXAPPS/IPVS_AdminUI/query/DisplayProfile.xml",
                                                               [{ VarName: "_WHEREXPATH_", VarRef: "DisplayProfileWhereXPath" }
                                                               ],
                                                               { action: "FNCCALL", context: cmpDisplayProfilePanelHidden,
                                                                 fnc: function (respCode, respVal) {
                                                                   if (respCode == "SUCCESS" && respVal != null) {
                                                                     var docXformResp = this.onResponse(respCode, respVal); topic_Get(tApp, "DisplayProfileSelected");
                                                                     cmpDisplayProfilePanel.onResponse(docXformResp);
                                                                   }
                                                                   else {
                                                                     log.info("GetDisplayProfileRequest failed. Error Code :" + respCode);
                                                                     this.onResponseErrorHandle("GetDisplayProfile", respCode, respVal);
                                                                   }
                                                                 }
                                                               }
                                                             );

  //TBR after MW implemets the request/Response
  //cmpDisplayProfilePanelHidden.cmpReq.SimulateResponse = true;
  //cmpDisplayProfilePanelHidden.cmpReq.SimulateResponseXML = '<Response serviceVer="2.19"><header><servicename>assetadminservice</servicename><requestname>AssetManagementRequest</requestname><userJID>superuser-default@localhost/anujpc12</userJID><requestNID>superuser-default@localhost/anujpc12/36354</requestNID><state>8</state><clientdata>Request_jsx_5s</clientdata></header><data><AssetManagementResponseData><ObjectType>null</ObjectType><ReadQueryResponse><ResponseID>1</ResponseID><ResponseXML><result>    <GetListObjectResponseData>        <ObjectCount>3</ObjectCount>        <ObjectLastWhereEndCount>1</ObjectLastWhereEndCount>        <CountToFetch>50</CountToFetch>    </GetListObjectResponseData><data jsxid="jsxroot">        <record jsxDescription="Canvas_desc_1"                jsxMediaStreamIOPortTypeTitle="IOPortTypeTitle_1"                jsxid="default.1"                 jsxPriority="1" jsxtext="Canvas_title_1">         <Groups>            <Tag Name="" Value=""></Tag>         </Groups>         <Canvas>            <Background enable="" zIndex="" content="">               <ImageContent imageURL="" color=""></ImageContent>               <SessionContent enableConnPanel="" enableBookmarkPanel="" enableCtrlPanel="" enableHeaderPanel="" enablePlayerPanel="" sessionId=""></SessionContent>            </Background>             <Window id="w11" enable="" content="">               <ConnectionContent MediaStreamIOPortTypeTitle="" MediaStreamIOPortElementID="" sessionId="" connId=""></ConnectionContent>               <ThumbnailContent MediaStreamIOPortTypeTitle="" MediaStreamIOPortElementID="" imageURL="" refreshInterval=""></ThumbnailContent>               <Geometry zIndex="" border="" vpos="" hpos="" vres="" hres="" aspect="" transparency=""></Geometry>               <SubCanvas></SubCanvas>            </Window>            <Window id="w12" enable="" content="">               <ConnectionContent MediaStreamIOPortTypeTitle="" MediaStreamIOPortElementID="" sessionId="" connId=""></ConnectionContent>               <ThumbnailContent MediaStreamIOPortTypeTitle="" MediaStreamIOPortElementID="" imageURL="" refreshInterval=""></ThumbnailContent>               <Geometry zIndex="" border="" vpos="" hpos="" vres="" hres="" aspect="" transparency=""></Geometry>               <SubCanvas></SubCanvas>            </Window>         </Canvas>      </record>         <record jsxDescription="Canvas_desc_2"                jsxMediaStreamIOPortTypeTitle="IOPortTypeTitle_2"                jsxid="default.2"                 jsxPriority="1" jsxtext="Canvas_title_2">         <Groups>            <Tag Name="" Value=""></Tag>         </Groups>         <Canvas>            <Background enable="" zIndex="" content="">               <ImageContent imageURL="" color=""></ImageContent>               <SessionContent enableConnPanel="" enableBookmarkPanel="" enableCtrlPanel="" enableHeaderPanel="" enablePlayerPanel="" sessionId=""></SessionContent>            </Background>             <Window id="w21" enable="" content="">               <ConnectionContent MediaStreamIOPortTypeTitle="" MediaStreamIOPortElementID="" sessionId="" connId=""></ConnectionContent>               <ThumbnailContent MediaStreamIOPortTypeTitle="" MediaStreamIOPortElementID="" imageURL="" refreshInterval=""></ThumbnailContent>               <Geometry zIndex="" border="" vpos="" hpos="" vres="" hres="" aspect="" transparency=""></Geometry>               <SubCanvas></SubCanvas>            </Window>         </Canvas>      </record>         <record jsxDescription="Canvas_desc_3"                jsxMediaStreamIOPortTypeTitle="IOPortTypeTitle_3"                jsxid="default.3"                 jsxPriority="3" jsxtext="Canvas_title_3">         <Groups>            <Tag Name="" Value=""></Tag>         </Groups>         <Canvas>            <Background enable="" zIndex="" content="">               <ImageContent imageURL="" color=""></ImageContent>               <SessionContent enableConnPanel="" enableBookmarkPanel="" enableCtrlPanel="" enableHeaderPanel="" enablePlayerPanel="" sessionId=""></SessionContent>            </Background>             <Window id="31" enable="" content="">               <ConnectionContent MediaStreamIOPortTypeTitle="" MediaStreamIOPortElementID="" sessionId="" connId=""></ConnectionContent>               <ThumbnailContent MediaStreamIOPortTypeTitle="" MediaStreamIOPortElementID="" imageURL="" refreshInterval=""></ThumbnailContent>               <Geometry zIndex="" border="" vpos="" hpos="" vres="" hres="" aspect="" transparency=""></Geometry>               <SubCanvas></SubCanvas>            </Window>            <Window id="32" enable="" content="">               <ConnectionContent MediaStreamIOPortTypeTitle="" MediaStreamIOPortElementID="" sessionId="" connId=""></ConnectionContent>               <ThumbnailContent MediaStreamIOPortTypeTitle="" MediaStreamIOPortElementID="" imageURL="" refreshInterval=""></ThumbnailContent>               <Geometry zIndex="" border="" vpos="" hpos="" vres="" hres="" aspect="" transparency=""></Geometry>               <SubCanvas></SubCanvas>            </Window>            <Window id="33" enable="" content="">               <ConnectionContent MediaStreamIOPortTypeTitle="" MediaStreamIOPortElementID="" sessionId="" connId=""></ConnectionContent>               <ThumbnailContent MediaStreamIOPortTypeTitle="" MediaStreamIOPortElementID="" imageURL="" refreshInterval=""></ThumbnailContent>               <Geometry zIndex="" border="" vpos="" hpos="" vres="" hres="" aspect="" transparency=""></Geometry>               <SubCanvas></SubCanvas>            </Window>            <Window id="34" enable="" content="">               <ConnectionContent MediaStreamIOPortTypeTitle="" MediaStreamIOPortElementID="" sessionId="" connId=""></ConnectionContent>               <ThumbnailContent MediaStreamIOPortTypeTitle="" MediaStreamIOPortElementID="" imageURL="" refreshInterval=""></ThumbnailContent>               <Geometry zIndex="" border="" vpos="" hpos="" vres="" hres="" aspect="" transparency=""></Geometry>               <SubCanvas></SubCanvas></Window><Window id="35" enable="" content=""><ConnectionContent MediaStreamIOPortTypeTitle="" MediaStreamIOPortElementID="" sessionId="" connId=""></ConnectionContent><ThumbnailContent MediaStreamIOPortTypeTitle="" MediaStreamIOPortElementID="" imageURL="" refreshInterval=""></ThumbnailContent>               <Geometry zIndex="" border="" vpos="" hpos="" vres="" hres="" aspect="" transparency=""></Geometry><SubCanvas></SubCanvas></Window></Canvas></record></data></result></ResponseXML></ReadQueryResponse></AssetManagementResponseData></data><log/></Response>';
                                                            
  cmpDisplayProfilePanelHidden.cmpItemByNIDReqFnc = function () {
                                                     topic_Set(tApp, "WhereDisplayProfileNID", "[//@NID=(\"" + cmpDisplayProfilePanelHidden.WhereNIDArr.join("\",\"") + "\")]");
                                                     return new cmpReq_GetListReadQuery(tApp, "JSXAPPS/IPVS_AdminUI/query/DisplayProfile.xml",
                                                                                        [{VarName:"_WHEREXPATH_", VarRef:"WhereDisplayProfileNID"}],
                                                                                        {action: "FNCCALL", context: cmpDisplayProfilePanelHidden,
                                                                                         fnc: function(respCode, respVal) {
                                                                                                if (respCode == "SUCCESS" && respVal != null) {
                                                                                                  this.onResponseUpdateRecord(respCode, respVal);
                                                                                                }
                                                                                                else{
                                                                                                  log.info("GetDisplayProfileRequest failed. Error Code :" + respCode);
                                                                                                  this.onResponseErrorHandle("GetDisplayProfile",respCode, respVal);
                                                                                                }
                                                                                              }
                                                                                        }, "ItemByNID"
                                                                                       );
                                                   }    

  var cmpDisplayProfileNavControls = new CmpPageNavButtonsContainer(["blkDisplayProfile","blkDisplayProfileFooter"],
                                                                    [["blkDisplayProfile", "btnPageFirst"],
                                                                     ["blkDisplayProfile", "btnPagePrev"],
                                                                     ["blkDisplayProfile", "btnPageNext"],
                                                                     ["blkDisplayProfile", "btnPageLast"],
                                                                     ["blkDisplayProfile", "lblRecordInfo"],
                                                                     ["blkDisplayProfile", "txtCurrentRecordNumber"],
                                                                     ["blkDisplayProfile", "btnPageRefresh"],
                                                                     ["blkDisplayProfile", "btnPageRefreshSpinner"],
                                                                     ["blkDisplayProfile", "btnPageRefreshNotify"]
                                                                    ],
                                                                    cmpDisplayProfilePanelHidden,
                                                                    [{action:"TOPICEVENT", topic:tApp, eventvar:"DisplayProfileDataUpdated", eventval:null}],
                                                                    null
                                                                   );

  var cmpBlkAddDisplayCanvas = new CmpContainer(["blkDisplayProfile","blkAddCanvas"],
                                                [{ cmp: cmpBtnAddCanvas}],
                                                null,
                                                null);

  var cmpBlkDeleteDisplayCanvas = new CmpContainer(["blkDisplayProfile","blkDeleteCanvas"],
                                                    [{cmp: cmpBtnDeleteCanvas,
                                                      sel: "(clsUserPermission.getPermission('Profile', 'delete') == 'true') && " +
                                                          "(topic_Get(tApp, 'DeleteDisplayProfileSelection') != null && " +
                                                          "topic_Get(tApp, 'DeleteDisplayProfileSelection') != 'NONE' && " +
                                                          "topic_Get(tApp, 'DeleteDisplayProfileSelection') == 'DELETE')"}],
                                                    null,
                                                    null);

  var cmpBlkEditDisplayCanvas = new CmpContainer(["blkDisplayProfile","blkEditCanvas"],
                                                  [{cmp: cmpBtnEditCanvas,
                                                    sel: "(clsUserPermission.getPermission('Profile', 'modify') == 'true') && " +
                                                        "(topic_Get(tApp, 'EditDisplayProfileSelection') != null && " +
                                                        "topic_Get(tApp, 'EditDisplayProfileSelection') != 'NONE' && " +
                                                        "topic_Get(tApp, 'EditDisplayProfileSelection') == 'EDIT')"}],
                                                  null,
                                                  null);
                                                                                                                                                                                                                         
  var cmpBlkDisplayProfile = new CmpContainer("blkDisplayProfile",
                                              [{ cmp:cmpBlkDeleteDisplayCanvas,
                                                  sel: "clsUserPermission.getPermission('Profile', 'delete') == 'true'"},
                                               { cmp:cmpBlkAddDisplayCanvas,
                                                 sel: "clsUserPermission.getPermission('Profile', 'add') == 'true'"},
                                               { cmp:cmpBlkEditDisplayCanvas,
                                                 sel: "clsUserPermission.getPermission('Profile', 'modify') == 'true'"},
                                               {cmp:cmpDisplayProfilePanelHidden},
                                               {cmp:cmpDisplayProfilePanel},
                                               {cmp:cmpDisplayProfileNavControls}
                                              ],
                                              [{ action: "APPEVENT", eventvar: "DeleteDisplayProfileSelection", eventval: null },
                                              { action: "APPEVENT", eventvar: "EditDisplayProfileSelection", eventval: null }
                                              ],
                                              null );  
                                                  
  /***************************** END DISPLAY PROFILE **********************/                                                  
                                      
  var cmpBlkProfilesUI = new CmpContainer("blkProfileUI",
                                          [ {cmp:cmpBlkConnectionProfile,
                                             sel:"topic_Get(tApp, 'ProfileMenuSelection') == null || topic_Get(tApp, 'ProfileMenuSelection') == 'CONNECTION'"},
                                            {cmp:cmpBlkDisplayProfile,
                                             sel:"topic_Get(tApp, 'ProfileMenuSelection') == 'DISPLAY'"}
                                          ],
                                          [{action:"APPEVENT", eventvar:"ProfileMenuSelection", eventval:null}] 
                                         );

                                          
  var cmpLblConnectionProfile = new CmpLabelHighlight(["blkProfiles","leftNav","lblConnectionProfile"], null, 
                                                       [{action:"APPEVENT", eventvar:"ProfileMenuSelection", eventval:null}], 
                                                       {action:"APPEVENT", eventvar:"ProfileMenuSelection", eventval:"CONNECTION"});

  var cmpLblDisplayProfile = new CmpLabelHighlight(["blkProfiles","leftNav","lblDisplayProfile"], null, 
                                                   [{action:"APPEVENT", eventvar:"ProfileMenuSelection", eventval:null}], 
                                                   {action:"APPEVENT", eventvar:"ProfileMenuSelection", eventval:"DISPLAY"});

  var cmpBlkProfile = new CmpContainer( "blkProfiles",  
                                        [ {cmp:cmpLblConnectionProfile},
                                          {cmp:cmpLblDisplayProfile},
                                          {cmp:cmpBlkProfilesUI}		  
                                        ],
                                        null
                                      );                                                 
 
  eventApp.subscribe("MENUUIDATA", "PROFILE", this,
                      function(EventVar, EventVal) {
                        if( EventVal == "PROFILE" ) {
                          var getDataState = topic_Get(tApp, "ProfileGetDataState");
                          if(getDataState == "NONE") {
                            setObjectCacheDefaultXML("MediaStreamProfile", "JSXAPPS/IPVS-Common/model/objectXml/V2DMediaStreamProfile.xml", "DEFAULT_V2D-STREAMPROFILE_NID"); 
                            setObjectCacheDefaultXML("MediaStreamProfile", "JSXAPPS/IPVS-Common/model/objectXml/MPEGTSMediaStreamProfile.xml", "DEFAULT_MPEGTS-STREAMPROFILE_NID"); 
                            setObjectCacheDefaultXML("MediaStreamProfile", "JSXAPPS/IPVS-Common/model/objectXml/RTPMediaStreamProfile.xml", "DEFAULT_RTP-STREAMPROFILE_NID"); 
                            setObjectCacheDefaultXML("MediaStreamProfile", "JSXAPPS/IPVS-Common/model/objectXml/UDPMediaStreamProfile.xml", "DEFAULT_UDP-STREAMPROFILE_NID"); 
                            setObjectCacheDefaultXML("MediaStreamProfile", "JSXAPPS/IPVS-Common/model/objectXml/RTP-RAWMediaStreamProfile.xml", "DEFAULT_RTP-RAW-STREAMPROFILE_NID");
                            setObjectCacheDefaultXML("MediaStreamProfile", "JSXAPPS/IPVS-Common/model/objectXml/RTP-PCMMediaStreamProfile.xml", "DEFAULT_RTP-PCM-STREAMPROFILE_NID"); 
                            setObjectCacheDefaultXML("MediaStreamProfile", "JSXAPPS/IPVS-Common/model/objectXml/RFBMediaStreamProfile.xml", "DEFAULT_RFB-STREAMPROFILE_NID"); 
                            setObjectCacheDefaultXML("DisplayProfile", "JSXAPPS/IPVS-Common/model/objectXml/DisplayProfile.xml", "DEFAULT_DISPLAYPROFILE_NID"); 
                            topic_Set(tApp, "EditProfileDataState", "DEFAULT_V2D-STREAMPROFILE_NID");
                                                    
                            topic_Publish(tApp, "ProfileMenuSelection", "CONNECTION");
                          }
                        }
                      }
                    );   
  
  eventApp.subscribe("ProfileMenuSelection", null, this,
                      function(EventVar, EventVal) {
                        if( EventVal == "CONNECTION" ) {
                          var getDataState = topic_Get(tApp, "ProfileGetDataState");
                          if(getDataState == "NONE") {
                            topic_Set(tApp, "ProfileGetDataState", (newServerDate()).getTime() );
                            cmpMtxProfileList.ResetToStart(1,ProfileCountToFetch,0);
                            topic_Publish(tApp, "ProfileGetData", (newServerDate()).getTime() ); 
                          }
                        }
                        else if(EventVal == "DISPLAY"){
                          var getDataState = topic_Get(tApp, "DisplayProfileGetDataState");
                          if(getDataState == "NONE") {
                            topic_Set(tApp, "DisplayProfileGetDataState", (newServerDate()).getTime() );
                            cmpDisplayProfilePanelHidden.ResetToStart(1,ProfileCountToFetch,0);
                            topic_Publish(tApp, "DisplayProfileGetData", (newServerDate()).getTime() ); 
                          }                        
                        }
                      }
                    ); 
  
  eventApp.subscribe("DisplayProfileSelected", null, this,
                    function() {
                       var selRecordNodes = cmpDisplayProfilePanelHidden.BlkJSX.getSelectedNodes();
                       if(selRecordNodes.size() > 1){
                        topic_Publish(tApp, "EditDisplayProfileSelection", "NONE");
                        topic_Publish(tApp, "DeleteDisplayProfileSelection", "DELETE");
                       }
                       else if(selRecordNodes.size() == 1){
                        //get the selected. Behaviour is different if selection is made through Ctrl key
                        for (var it = selRecordNodes.iterator(); it.hasNext(); ) {
                          var selRecordNode = it.next();
                          topic_Set(tApp, "DisplayProfileSelected",selRecordNode.getAttribute("jsxid"));
                        }
                                              
                        topic_Publish(tApp, "EditDisplayProfileSelection", "EDIT");
                        topic_Publish(tApp, "DeleteDisplayProfileSelection", "DELETE");
                       }
                       else{
                        topic_Publish(tApp, "EditDisplayProfileSelection", "NONE");
                        topic_Publish(tApp, "DeleteDisplayProfileSelection", "NONE");
                       }
                    }
                  );

  eventApp.subscribe("DBUpdate", null, this,
                     function(EventVar, EventVal) {
                       if( jsx3.util.strEmpty(EventVal) ) return;
                       if( EventVal.objType == "MediaStreamProfile" ) {
                         cmpMtxProfileList.handleNotification(EventVal);
                       }
                       else if(EventVal.objType == "DisplayProfile" ){
                         cmpDisplayProfilePanelHidden.handleNotification(EventVal);
                       }
                       if (EventVal.objType == "GroupTagDef") {
                         cmpProfileFilter.handleNotification(EventVal);
                       }                                                        
                     });

  eventApp.subscribe("ProfileDataUpdate", null, this,
                      function(EventVar, EventVal) {
                        if( EventVal == "ADD" || EventVal == "DELETE" ||  EventVal == "EDIT") {
                          //cmpMtxProfileList.getColumnsFromDB();
                          cmpMtxProfileList.doRepaint();
                        }
                      }
                    ); 
                      
  eventApp.subscribe("DisplayProfileDataUpdate", null, this,
                      function(EventVar, EventVal) {
                        if( EventVal == "ADD" || EventVal == "DELETE" ||  EventVal == "EDIT") {
                          cmpDisplayProfilePanelHidden.doRepaint();
                        }
                      }
                    );                         
  return cmpBlkProfile; 
  
  function cmpBlkProfileDelete(profileNID, objectType) {
    var reqDelProfile = new ReqDeleteObjectNID(profileNID, objectType, 
                                                {action:"FNCCALL", context:this,
                                                   fnc:function(respCode, respVal) {
                                                     if (respCode == "SUCCESS" && respVal != null) {
                                                       --cmpMtxProfileList.deleteRequestCounter < 0 ? 0 : cmpMtxProfileList.deleteRequestCounter;
                                                       if (cmpMtxProfileList.deleteRequestCounter == 0) {
                                                        topic_Publish(tApp, "ProfileDataUpdate", "DELETE");
                                                       }
                                                    } else {
                                                      --cmpMtxProfileList.deleteRequestCounter < 0 ? 0 : cmpMtxProfileList.deleteRequestCounter;
                                                      var codeErrorMsg = CommonErrorCodeArr[respCode].errorDesc;
                                                      var ErrorMsg = "Error: Profile delete request failed \n";
                                                      ErrorMsg += "Error description: " + codeErrorMsg + "\n";
                                                      ErrorMsg += "Error code: " + respCode;
                                                      alert(ErrorMsg);
                                                    }
                                                  }
                                                }
                                              );
    reqDelProfile.sendRequest();
  } 
  
  
  function cmpBlkDisplayProfileDelete(displayProfileNID) {
    var reqDelProfile = new ReqDeleteObjectNID(displayProfileNID, "DisplayProfile", 
                                                {action:"FNCCALL", context:this,
                                                   fnc:function(respCode, respVal) {
                                                         if(respCode == "SUCCESS" && respVal != null) {
                                                           topic_Publish(tApp, "DeleteDisplayProfileSelection", "NONE");
                                                           topic_Publish(tApp, "DisplayProfileDataUpdate", "DELETE");
                                                         } else {
                                                           var codeErrorMsg = CommonErrorCodeArr[respCode].errorDesc;
                                                           var ErrorMsg = "Error: Profile delete request failed \n";
                                                           ErrorMsg += "Error description: " + codeErrorMsg + "\n";
                                                           ErrorMsg += "Error code: " + respCode;
                                                           alert(ErrorMsg);
                                                         }
                                                       }
                                                }
                                              );
    reqDelProfile.sendRequest();
  }  
  
                                                                 
}

function cmpBlkProfile_Destroy() {
  var blkProfileBlkJSX = PNameSpace.getJSXByName("blkProfiles");
  blkProfileBlkJSX.removeChildren();
}