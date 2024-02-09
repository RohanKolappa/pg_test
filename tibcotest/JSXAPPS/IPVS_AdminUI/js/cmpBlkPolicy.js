function cmpBlkPolicy_Create() {
  var blkPolicyBlkJSX = PNameSpace.getJSXByName("blkPolicy");
  blkPolicyBlkJSX.load("components/policy/policies.xml");
  var CountToFetch = 20;  
  
  topic_Set(tApp, "GroupTagPolicyDataState", "NONE");
  topic_Set(tApp, "TagObjectFilterPolicyDataState", "NONE");
  topic_Set(tApp, "PortAccessPolicyDataState", "NONE");
  topic_Set(tApp, "MediaAccessPolicyDataState", "NONE");
  topic_Set(tApp, "RecordingAccessPolicyDataState", "NONE");
  topic_Set(tApp, "SessionAccessPolicyDataState", "NONE");
  topic_Set(tApp, "UserAccessPolicyDataState", "NONE");
  topic_Set(tApp, "AssetAccessPolicyDataState", "NONE");
  topic_Set(tApp, "MediaStoreAccessPolicyDataState", "NONE");
  topic_Set(tApp, "PolicyAccessPolicyDataState", "NONE");
  topic_Set(tApp, "NATAccessPolicyDataState", "NONE");
  topic_Set(tApp, "MediaStreamFilterPolicyDataState", "NONE");
  topic_Set(tApp, "CollabAccessPolicyDataState", "NONE");
  /************************** GroupTagDEF Block START************/
  
  var cmpBtnDeleteGroupTagValue = new CmpButton("btnDeleteGroupTagValuePolicy", 
                                   [{action:"APPEVENT", eventvar:"GroupTagValueSelected", eventval:null}],
                                   {action:"FNCCALL", 
                                    context:this,
                                        fnc:function() {
                                           cmpBlkGroupTagValueDelete();
                                        }
                                   });

  var cmpBtnAddGroupTagValue = new CmpButton("btnAddGroupTagValuePolicy",
                                   [{action:"APPEVENT", eventvar:"GroupTagSelected", eventval:null}], 
                                   {action:"FNCCALL", 
                                    context:this,
                                        fnc:function() {
                                           dlgEditGroupTagValue_Create("DEFAULT_GROUPTAG_NID");
                                        }
                                   });

  var cmpMtxGroupTagValueList = new CmpNIDElementList("mtxGroupTagValue",
                                        "GroupTagSelected",
                                        ".//TagValueList/TagValue",
                                        {
                                         jsxtext:{objxpath:"\".\""}
                                        },
                                        [{action:"REQEVENT", eventvar:"GroupTagDataState", eventval:null},
                                         {action:"APPEVENT", eventvar:"GroupTagSelected", eventval:null}],
                                        {persist:{col:"jsxtext",initval:false},
                                         action:"APPEVENT", eventvar:"GroupTagValueSelected", eventval:"jsxtext"
                                        }
                                       );

  var cmpBtnDeleteGroupTag = new CmpButton("btnDeleteGroupTagPolicy", 
                                   [{action:"APPEVENT", eventvar:"GroupTagSelected", eventval:null}],
                                   {action:"FNCCALL", 
                                    context:this,
                                        fnc:function() {
                                           this.SelectedNID = topic_Get(tApp, "GroupTagSelected");
                                           cmpBlkGroupTagDelete.call(this);
                                        }
                                   });

  var cmpBtnAddGroupTag = new CmpButton("btnAddGroupTagPolicy",
                                   null, 
                                   {action:"FNCCALL", 
                                    context:this,
                                        fnc:function() {
                                           dlgEditGroupTag_Create("DEFAULT_GROUPTAG_NID");
                                        }
                                   });


  eventApp.subscribe("GroupTagSelected", null, this,
                      function() {
                        //topic_Publish(tApp, "GroupTagValueSelected", "UNSELECTED");
                        cmpGroupTagProperties_update();
                      } );
  eventApp.subscribe("GroupTagDataState", null, this,
                      function() {
                        cmpGroupTagProperties_update();
                      } );
                      
  topic_Set(tApp, "GroupTagWhereXPath", "");
  var cmpMtxGroupTagList = new CmpCustomPaginationList(["blkGroupTags","mtxGroupTagName"],
                                                      [{action:"APPEVENT", eventvar:"GroupTagPolicyData", eventval:null}],
                                                      {persist:{col:"jsxid",initval:false},
                                                       action:"APPEVENT", eventvar:"GroupTagSelected", eventval:"jsxid"},
                                                      null,
                                                      null
                                                     ); 

  cmpMtxGroupTagList.SortPathArr = {jsxtext:"upper-case($b//TagName)"
                                   };

  cmpMtxGroupTagList.cmpReqParamArr = {StartFrom: "Op_Select_ListSpec_StartFrom",
                                       CountToFetch: "Op_Select_ListSpec_CountToFetch",
                                       SortOrder: "Op_Select_ListSpec_SortOrder",
                                       ObjectCount: "ObjectCount",
                                       SortByXpath: "Op_Select_ListSpec_SortByXPathList_SortByXpath"
                                      };

  cmpMtxGroupTagList.DataUpdatedEvent = { action: "TOPICEVENT", topic: tApp, eventvar: "GroupTagDataUpdated", eventval: "LOADED" };

  cmpMtxGroupTagList.cmpReq = new cmpReq_GetListReadQuery(tApp, "JSXAPPS/IPVS_AdminUI/query/GroupTagRule.xml",
                                                          [{ VarName: "_WHEREXPATH_", VarRef: "GroupTagWhereXPath"}
                                                          ],
                                                          { action: "FNCCALL", context: cmpMtxGroupTagList,
                                                              fnc: function(respCode, respVal) {
                                                                  if (respCode == "SUCCESS" && respVal != null) {
                                                                      this.onResponse(respCode, respVal);
                                                                  }
                                                                  else {
                                                                      log.info("GetGroupTagRequest failed. Error Code :" + respCode);
                                                                      this.onResponseErrorHandle("GetGroupTagRequest",respCode, respVal);
                                                                  }
                                                              }
                                                          }
                                                        );    
 
    cmpMtxGroupTagList.cmpItemByNIDReqFnc = function () {
                                             var strWhereNID = cmpMtxGroupTagList.WhereNIDArr.join("\",\"");
                                             topic_Set(tApp, "WhereGroupNID", "[//@NID=(\"" + strWhereNID + "\")]");
                                             return new cmpReq_GetListReadQuery(tApp, "JSXAPPS/IPVS_AdminUI/query/GroupTagRule.xml",
                                                                                [{VarName:"_WHEREXPATH_", VarRef:"WhereGroupNID"}],
                                                                                {action: "FNCCALL", context: cmpMtxGroupTagList,
                                                                                 fnc: function(respCode, respVal) {
                                                                                        if (respCode == "SUCCESS" && respVal != null) {
                                                                                          this.onResponseUpdateRecord(respCode, respVal);
                                                                                        }
                                                                                        else{
                                                                                          log.info("GetGroupTagRequest failed. Error Code :" + respCode);
                                                                                          this.onResponseErrorHandle("GetGroupTagRequest",respCode, respVal);
                                                                                        }
                                                                                      }
                                                                                }, "ItemByNID"
                                                                               );
                                           }

  var cmpGroupTagNavControls = new CmpPageNavButtonsContainer(["blkGroupTags","blkGroupTagPagination"],
                                                              [["blkGroupTags", "btnPageFirst"],
                                                               ["blkGroupTags", "btnPagePrev"],
                                                               ["blkGroupTags", "btnPageNext"],
                                                               ["blkGroupTags", "btnPageLast"],
                                                               ["blkGroupTags", "lblRecordInfo"],
                                                               ["blkGroupTags", "txtCurrentRecordNumber"],
                                                               ["blkGroupTags", "btnPageRefresh"],
                                                               ["blkGroupTags", "btnPageRefreshSpinner"],
                                                               ["blkGroupTags", "btnPageRefreshNotify"]
                                                              ],
                                                              cmpMtxGroupTagList,
                                                              [{action:"TOPICEVENT", topic:tApp, eventvar:"GroupTagDataUpdated", eventval:null}],
                                                              null
                                                             );

  var cmpBlkGroupTagList = new CmpContainer( "blkGroupTags", 
                              [ {cmp:cmpMtxGroupTagList},
                                {cmp:cmpBtnAddGroupTag},
                                {cmp:cmpBtnDeleteGroupTag},
                                {cmp:cmpMtxGroupTagValueList},
                                {cmp:cmpBtnAddGroupTagValue},
                                {cmp:cmpBtnDeleteGroupTagValue},
                                {cmp:cmpGroupTagNavControls}
                              ],
                              null );

  /************************** GroupTagDEF Block END ^************/

  /************************** TAGOBJECT FILTER Block START  ************/
  var cmpBtnDeleteTagObjectFilter = new CmpButton("btnDeleteTagObjectFilterPolicy", 
                                   [{action:"APPEVENT", eventvar:"TagObjectFilterSelected", eventval:null}],
                                   {action:"FNCCALL", 
                                    context:this,
                                        fnc:function() {
                                            this.SelectedNID = topic_Get(tApp, "TagObjectFilterSelected");
                                            var selRecord = cmpMtxTagObjectFilterList.BlkJSX.getRecord(this.SelectedNID);
                                            var SelectedTagObjectFilterName = selRecord.jsxtext;
                                            if(SelectedTagObjectFilterName == "_ALL_") {
                                              alert("Cannot delete filter: _ALL_ ");
                                              return false;
                                            } else {
                                              cmpBlkTagObjectFilterDelete.call(this);
                                            }
                                        }
                                   });

  var cmpBtnAddTagObjectFilter = new CmpButton("btnAddTagObjectFilterPolicy",
                                   null, 
                                   {action:"FNCCALL", 
                                    context:this,
                                        fnc:function() {
                                           dlgEditTagObjectFilter_Create("DEFAULT_TAGOBJECTFILTER_NID");
                                        }
                                   });

  eventApp.subscribe("TagObjectFilterSelected", null, this,
                      function() {
                        cmpTagObjectFilterProperties_update();
                      } );
  eventApp.subscribe("TagObjectFilterDataState", null, this,
                      function() {
                        cmpTagObjectFilterProperties_update();
                      } );                      
                      
  var cmpMtxTagObjectFilterValueList = new CmpNIDElementList("mtxFilterValue",
                                                              "TagObjectFilterSelected",
                                                              ".//TagFilterSet/TagFilter",
                                                              {
                                                               jsxtext:{objxpath:"\".//LHTagName\""},
                                                               jsxOperation:{objxpath:"\".//MatchOp\""},
                                                               jsxMatchString:{objxpath:"\".//RHRegexp\""}
                                                              },
                                                              [{action:"REQEVENT", eventvar:"TagObjectFilterDataState", eventval:null},
                                                               {action:"APPEVENT", eventvar:"TagObjectFilterSelected", eventval:null}],
                                                              {persist:{col:"jsxtext",initval:false}
                                                              }
                                                             );


  topic_Set(tApp, "TagObjectFilterWhereXPath", "");
  var cmpMtxTagObjectFilterList = new CmpCustomPaginationList(["blkFilters","mtxFilterName"],
                                                              [{action:"APPEVENT", eventvar:"TagObjectFilterPolicyData", eventval:null}],
                                                              {persist:{col:"jsxid",initval:false},
                                                               action:"APPEVENT", eventvar:"TagObjectFilterSelected", eventval:"jsxid"},
                                                              null,
                                                              null
                                                             ); 

  cmpMtxTagObjectFilterList.SortPathArr = {jsxtext:"upper-case($b//Name)"
                                          };

  cmpMtxTagObjectFilterList.cmpReqParamArr = {StartFrom: "Op_Select_ListSpec_StartFrom",
                                              CountToFetch: "Op_Select_ListSpec_CountToFetch",
                                              SortOrder: "Op_Select_ListSpec_SortOrder",
                                              ObjectCount: "ObjectCount",
                                              SortByXpath: "Op_Select_ListSpec_SortByXPathList_SortByXpath"
                                             };

  cmpMtxTagObjectFilterList.DataUpdatedEvent = { action: "TOPICEVENT", topic: tApp, eventvar: "TagObjectFilterDataUpdated", eventval: "LOADED" };

  cmpMtxTagObjectFilterList.cmpReq = new cmpReq_GetListReadQuery(tApp, "JSXAPPS/IPVS_AdminUI/query/TagObjectFilterRule.xml",
                                                                [{ VarName: "_WHEREXPATH_", VarRef: "TagObjectFilterWhereXPath"}
                                                                ],
                                                                { action: "FNCCALL", context: cmpMtxTagObjectFilterList,
                                                                    fnc: function(respCode, respVal) {
                                                                        if (respCode == "SUCCESS" && respVal != null) {
                                                                            this.onResponse(respCode, respVal);
                                                                        }
                                                                        else {
                                                                            log.info("GetTagObjectFilterRequest failed. Error Code :" + respCode);
                                                                            this.onResponseErrorHandle("GetTagObjectFilterRequest",respCode, respVal);
                                                                        }
                                                                    }
                                                                }
                                                              );  
                                                              
  cmpMtxTagObjectFilterList.cmpItemByNIDReqFnc = function () {
                                                 var strWhereNID = cmpMtxTagObjectFilterList.WhereNIDArr.join("\",\"");
                                                 topic_Set(tApp, "WhereGroupNID", "[//@NID=(\"" + strWhereNID + "\")]");
                                                 return new cmpReq_GetListReadQuery(tApp, "JSXAPPS/IPVS_AdminUI/query/TagObjectFilterRule.xml",
                                                                                    [{VarName:"_WHEREXPATH_", VarRef:"WhereGroupNID"}],
                                                                                    {action: "FNCCALL", context: cmpMtxTagObjectFilterList,
                                                                                     fnc: function(respCode, respVal) {
                                                                                            if (respCode == "SUCCESS" && respVal != null) {
                                                                                              this.onResponseUpdateRecord(respCode, respVal);
                                                                                            }
                                                                                            else{
                                                                                              log.info("GetTagObjectFilterRequest failed. Error Code :" + respCode);
                                                                                              this.onResponseErrorHandle("GetTagObjectFilterRequest",respCode, respVal);
                                                                                            }
                                                                                          }
                                                                                    }, "ItemByNID"
                                                                                   );
                                               }
  cmpMtxTagObjectFilterList.MenuEvent = {action:"APPEVENT", eventvar:"PolicyMenuSelected", eventval:"jsxid"};

  var cmpFilterNavControls = new CmpPageNavButtonsContainer(["blkFilters","blkFilterPagination"],
                                                            [["blkFilters", "btnPageFirst"],
                                                             ["blkFilters", "btnPagePrev"],
                                                             ["blkFilters", "btnPageNext"],
                                                             ["blkFilters", "btnPageLast"],
                                                             ["blkFilters", "lblRecordInfo"],
                                                             ["blkFilters", "txtCurrentRecordNumber"],
                                                             ["blkFilters", "btnPageRefresh"],
                                                             ["blkFilters", "btnPageRefreshSpinner"],
                                                             ["blkFilters", "btnPageRefreshNotify"]
                                                            ],
                                                            cmpMtxTagObjectFilterList,
                                                            [{action:"TOPICEVENT", topic:tApp, eventvar:"TagObjectFilterDataUpdated", eventval:null}],
                                                            null
                                                           );
  var cmpMenuFilterPolicyList = new CmpMenu("menuFilterPolicyList",
                                            [{ObjectType:"TAGOBJECTFILTER",
                                              MenuListArr:["Edit..."] },
                                             {ObjectType:"_ALL_",
                                              MenuListArr: ["Edit Not Allowed"]}
                                            ],
                                            null, null, null,
                                            [{action:"APPEVENT", eventvar:"PolicyMenuTypeSelected", eventval:null}],
                                            {action:"APPEVENT", eventvar:"PolicyMenuActionSelected", eventval:"jsxtext"}
                                          );

  var cmpBlkTagObjectFilter = new CmpContainer( "blkFilters", 
                              [ {cmp:cmpMtxTagObjectFilterList},
                                {cmp:cmpMtxTagObjectFilterValueList},
                                {cmp:cmpBtnAddTagObjectFilter},
                                {cmp:cmpMenuFilterPolicyList},
                                {cmp:cmpBtnDeleteTagObjectFilter},
                                {cmp:cmpFilterNavControls}
                              ],
                              null );
                              
  /************************** TAGOBJECT FILTER Block END ^  *********/  
	  
  /************************** OBJECT FILTER Block START  ************/
  var cmpBtnDeleteObjectFilter = new CmpButton("btnDeleteObjectFilterPolicy", 
                                   [{action:"APPEVENT", eventvar:"ObjectFilterSelected", eventval:null}],
                                   {action:"FNCCALL", 
                                    context:this,
                                        fnc:function() {
                                           this.SelectedNID = topic_Get(tApp, "ObjectFilterSelected");
                                           cmpBlkObjectFilterDelete.call(this);
                                        }
                                   });

  var cmpBtnEditObjectFilter = new CmpButton("btnEditObjectFilterPolicy", 
                                   [{action:"APPEVENT", eventvar:"ObjectFilterSelected", eventval:null}],
                                   {action:"FNCCALL", 
                                    context:this,
                                        fnc:function() {
                                           this.SelectedNID = topic_Get(tApp, "ObjectFilterSelected");
                                           dlgEditObjectFilter_Create(this.SelectedNID);
                                        }
                                   });
  
  var cmpBtnAddObjectFilter = new CmpButton("btnAddObjectFilterPolicy",
                                   null, 
                                   {action:"FNCCALL", 
                                    context:this,
                                        fnc:function() {
                                           dlgEditObjectFilter_Create("DEFAULT_OBJECTFILTER_NID");
                                        }
                                   });

  eventApp.subscribe("ObjectFilterSelected", null, this,
                      function() {
                        cmpObjectFilterProperties_update();
                      } );
  eventApp.subscribe("ObjectFilterDataState", null, this,
                      function() {
                        cmpObjectFilterProperties_update();
                      } );                      
                      
  var cmpMtxObjectFilterValueList = new CmpNIDElementList("mtxObjectFilterValue",
                                        "ObjectFilterSelected",
                                        ".//AttrFilterSet/AttrFilter",
                                        {
                                         jsxtext:{objxpath:"\".//LHObjectRelativeXPath\""},
                                         jsxOperation:{objxpath:"\".//MatchOp\""},
                                         jsxMatchString:{objxpath:"\".//RHRegexp\""}
                                        },
                                        [{action:"REQEVENT", eventvar:"ObjectFilterDataState", eventval:null},
                                         {action:"APPEVENT", eventvar:"ObjectFilterSelected", eventval:null}],
                                        {persist:{col:"jsxtext",initval:false}
                                        }
                                       );


  var cmpMtxObjectFilterList = new CmpNIDList( "mtxObjectFilterName",
                                                "ObjectFilterNIDList",
                                                 null, 
                                                 null,
                                               {jsxid:{objxpath:"\"./@NID\""},
                                               jsxtext:{objxpath:"\".//Name\""}},
                                              [{action:"REQEVENT", eventvar:"GroupTagDataState", eventval:null},
                                               {action:"REQEVENT", eventvar:"TagObjectFilterDataState", eventval:null},
	                                           {action:"REQEVENT", eventvar:"ObjectFilterDataState", eventval:null}],
                                              {persist:{col:"jsxtext",initval:false},
                                               action:"APPEVENT", eventvar:"ObjectFilterSelected", eventval:"jsxid"}
                                              );

  var cmpBlkObjectFilter = new CmpContainer( "blkObjectFilters", 
                              [ {cmp:cmpMtxObjectFilterList},
                                {cmp:cmpMtxObjectFilterValueList},
                                {cmp:cmpBtnAddObjectFilter},
                                {cmp:cmpBtnEditObjectFilter},
                                {cmp:cmpBtnDeleteObjectFilter}
                              ],
                              null );
                              
  /************************** OBJECT FILTER Block END ^  *********/  


  /************************** STREAM FILTER Block START ******************/
  var cmpBtnDeleteMediaStreamFilter = new CmpButton("btnDeleteMediaStreamFilterPolicy", 
                                   [{action:"APPEVENT", eventvar:"MediaStreamFilterSelected", eventval:null}],
                                   {action:"FNCCALL", 
                                    context:this,
                                        fnc:function() {
                                           this.SelectedPolicy = "MediaStreamFilter";
                                           cmpBlkPolicyDelete.call(this);
                                        }
                                   });

  var cmpBtnAddMediaStreamFilter = new CmpButton("btnAddMediaStreamFilterPolicy",
                                   null, 
                                   {action:"FNCCALL", 
                                    context:this,
                                        fnc:function() {
                                           dlgEditMediaStreamFilter_Create("DEFAULT_MEDIASTREAMFILTER_NID");
                                        }
                                   });

  topic_Set(tApp, "MediaStreamFilterPolicyWhereXPath", "");
  var cmpMtxMediaStreamFilterList = new CmpCustomPaginationList(["blkStreamPolicyList","mtxpolicyMediaStreamFilterList"],
                                                                [{action:"APPEVENT", eventvar:"MediaStreamFilterPolicyData", eventval:null}],
                                                                {persist:{col:"jsxid",initval:false},
                                                                 action:"APPEVENT", eventvar:"MediaStreamFilterSelected", eventval:"jsxid"},
                                                                null,
                                                                null
                                                               );

  cmpMtxMediaStreamFilterList.allowMultiSelection = true;

  cmpMtxMediaStreamFilterList.SortPathArr = {jsxtext:"upper-case($b//Name)"
                                           };
  
  cmpMtxMediaStreamFilterList.cmpReqParamArr = {StartFrom: "Op_Select_ListSpec_StartFrom",
                                                CountToFetch: "Op_Select_ListSpec_CountToFetch",
                                                SortOrder: "Op_Select_ListSpec_SortOrder",
                                                ObjectCount: "ObjectCount",
                                                SortByXpath: "Op_Select_ListSpec_SortByXPathList_SortByXpath"
                                               };

  cmpMtxMediaStreamFilterList.DataUpdatedEvent = { action: "TOPICEVENT", topic: tApp, eventvar: "MediaStreamFilterPolicyDataUpdated", eventval: "LOADED" };

  cmpMtxMediaStreamFilterList.cmpReq = new cmpReq_GetListReadQuery(tApp, "JSXAPPS/IPVS_AdminUI/query/MediaStreamFilterPolicyRule.xml",
                                                                  [{ VarName: "_WHEREXPATH_", VarRef: "MediaStreamFilterPolicyWhereXPath"}
                                                                  ],
                                                                  { action: "FNCCALL", context: cmpMtxMediaStreamFilterList,
                                                                      fnc: function(respCode, respVal) {
                                                                          if (respCode == "SUCCESS" && respVal != null) {
                                                                              this.onResponse(respCode, respVal);
                                                                          }
                                                                          else {
                                                                              log.info("GetMediaStreamFilterPolicyRequest failed. Error Code :" + respCode);
                                                                              this.onResponseErrorHandle("GetMediaStreamFilterPolicyRequest",respCode, respVal);
                                                                          }
                                                                      }
                                                                  }
                                                                );
                                                                
  cmpMtxMediaStreamFilterList.cmpItemByNIDReqFnc = function () {
                                                   var strWhereNID = cmpMtxMediaStreamFilterList.WhereNIDArr.join("\",\"");
                                                   topic_Set(tApp, "WhereGroupNID", "[//@NID=(\"" + strWhereNID + "\")]");
                                                   return new cmpReq_GetListReadQuery(tApp, "JSXAPPS/IPVS_AdminUI/query/MediaStreamFilterPolicyRule.xml",
                                                                                      [{VarName:"_WHEREXPATH_", VarRef:"WhereGroupNID"}],
                                                                                      {action: "FNCCALL", context: cmpMtxMediaStreamFilterList,
                                                                                       fnc: function(respCode, respVal) {
                                                                                              if (respCode == "SUCCESS" && respVal != null) {
                                                                                                this.onResponseUpdateRecord(respCode, respVal);
                                                                                              }
                                                                                              else{
                                                                                                log.info("GetMediaStreamFilterRequest failed. Error Code :" + respCode);
                                                                                                this.onResponseErrorHandle("GetMediaStreamFilterRequest",respCode, respVal);
                                                                                              }
                                                                                            }
                                                                                      }, "ItemByNID"
                                                                                     );
                                                 }
                                                                
  cmpMtxMediaStreamFilterList.MenuEvent = {action:"APPEVENT", eventvar:"PolicyMenuSelected", eventval:"jsxid"};

  var cmpMediaStreamFilterNavControls = new CmpPageNavButtonsContainer(["blkStreamPolicyList","blkMediaStreamFilterFooter"],
                                                                      [["blkStreamPolicyList", "btnPageFirst"],
                                                                       ["blkStreamPolicyList", "btnPagePrev"],
                                                                       ["blkStreamPolicyList", "btnPageNext"],
                                                                       ["blkStreamPolicyList", "btnPageLast"],
                                                                       ["blkStreamPolicyList", "lblRecordInfo"],
                                                                       ["blkStreamPolicyList", "txtCurrentRecordNumber"],
                                                                       ["blkStreamPolicyList", "btnPageRefresh"],
                                                                       ["blkStreamPolicyList", "btnPageRefreshSpinner"],
                                                                       ["blkStreamPolicyList", "btnPageRefreshNotify"]
                                                                      ],
                                                                      cmpMtxMediaStreamFilterList,
                                                                      [{action:"TOPICEVENT", topic:tApp, eventvar:"MediaStreamFilterPolicyDataUpdated", eventval:null}],
                                                                      null
                                                                     );
  
  var cmpMenuStreamFilterPolicyList = new CmpMenu("menuStreamFilterPolicyList",
                                                  [{ObjectType:"STREAMPOLICY",
                                                    MenuListArr:["Edit..."] }
                                                  ],
                                                  null, null, null,
                                                  [{action:"APPEVENT", eventvar:"PolicyMenuTypeSelected", eventval:null}],
                                                  {action:"APPEVENT", eventvar:"PolicyMenuActionSelected", eventval:"jsxtext"}
                                                );

  var cmpBlkMediaStreamFilterList = new CmpContainer( "blkStreamPolicyList", 
                              [ {cmp:cmpMtxMediaStreamFilterList},
                                {cmp:cmpBtnAddMediaStreamFilter},
                                {cmp:cmpMenuStreamFilterPolicyList},
                                {cmp:cmpBtnDeleteMediaStreamFilter},
                                {cmp:cmpMediaStreamFilterNavControls}
                              ],
                              null );
  /************************** STREAM FILTER Block END ^  ******************/
  
  /************************** User Admin Block START ******************/
  var cmpBtnDeleteUserAdmin = new CmpButton("btnDeleteUserAdminPolicy", 
                                   [{action:"APPEVENT", eventvar:"UserAdminSelected", eventval:null}],
                                   {action:"FNCCALL", 
                                    context:this,
                                        fnc:function() {
                                           this.SelectedPolicy = "UserAdminPolicyRule";
                                           cmpBlkPolicyDelete.call(this);
                                        }
                                   });

  var cmpBtnAddUserAdmin = new CmpButton("btnAddUserAdminPolicy",
                                   null, 
                                   {action:"FNCCALL", 
                                    context:this,
                                        fnc:function() {
                                           dlgEditUserAdmin_Create("DEFAULT_USERADMIN_NID");
                                        }
                                   });

  topic_Set(tApp, "UserAccessPolicyWhereXPath", "");
  var cmpMtxUserAdminList = new CmpCustomPaginationList(["blkUserAdminPolicyList","mtxpolicyUserAdminList"],
                                                        [{action:"APPEVENT", eventvar:"UserAccessPolicyData", eventval:null}],
                                                        {persist:{col:"jsxid",initval:false},
                                                         action:"APPEVENT", eventvar:"UserAdminSelected", eventval:"jsxid"},
                                                        null,
                                                        null
                                                       );

   cmpMtxUserAdminList.allowMultiSelection = true;

  cmpMtxUserAdminList.SortPathArr = {index:"$b//order",
                                     jsxtext:"upper-case($b//rulename)",
                                     jsxUserPolicyType:"$b//allow"
                                   };
  
  cmpMtxUserAdminList.cmpReqParamArr = {StartFrom: "Op_Select_ListSpec_StartFrom",
                                        CountToFetch: "Op_Select_ListSpec_CountToFetch",
                                        SortOrder: "Op_Select_ListSpec_SortOrder",
                                        ObjectCount: "ObjectCount",
                                        SortByXpath: "Op_Select_ListSpec_SortByXPathList_SortByXpath"
                                       };

  cmpMtxUserAdminList.DataUpdatedEvent = { action: "TOPICEVENT", topic: tApp, eventvar: "UserAccessPolicyDataUpdated", eventval: "LOADED" };

  cmpMtxUserAdminList.cmpReq = new cmpReq_GetListReadQuery(tApp, "JSXAPPS/IPVS_AdminUI/query/UserAccessPolicyRule.xml",
                                                          [{ VarName: "_WHEREXPATH_", VarRef: "UserAccessPolicyWhereXPath"}
                                                          ],
                                                          { action: "FNCCALL", context: cmpMtxUserAdminList,
                                                              fnc: function(respCode, respVal) {
                                                                  if (respCode == "SUCCESS" && respVal != null) {
                                                                      this.onResponse(respCode, respVal);
                                                                  }
                                                                  else {
                                                                      log.info("GetUserAccessPolicyRequest failed. Error Code :" + respCode);
                                                                      this.onResponseErrorHandle("GetUserAccessPolicyRequest",respCode, respVal);
                                                                  }
                                                              }
                                                          }
                                                        );    

  cmpMtxUserAdminList.cmpItemByNIDReqFnc = function () {
                                           var strWhereNID = cmpMtxUserAdminList.WhereNIDArr.join("\",\"");
                                           topic_Set(tApp, "WhereGroupNID", "[//@NID=(\"" + strWhereNID + "\")]");
                                           return new cmpReq_GetListReadQuery(tApp, "JSXAPPS/IPVS_AdminUI/query/UserAccessPolicyRule.xml",
                                                                              [{VarName:"_WHEREXPATH_", VarRef:"WhereGroupNID"}],
                                                                              {action: "FNCCALL", context: cmpMtxUserAdminList,
                                                                               fnc: function(respCode, respVal) {
                                                                                      if (respCode == "SUCCESS" && respVal != null) {
                                                                                        this.onResponseUpdateRecord(respCode, respVal);
                                                                                      }
                                                                                      else{
                                                                                        log.info("GetUserAccessPolicyRequest failed. Error Code :" + respCode);
                                                                                        this.onResponseErrorHandle("GetUserAccessPolicyRequest",respCode, respVal);
                                                                                      }
                                                                                    }
                                                                              }, "ItemByNID"
                                                                             );
                                         }
                                                                
  cmpMtxUserAdminList.MenuEvent = {action:"APPEVENT", eventvar:"PolicyMenuSelected", eventval:"jsxid"};

  var cmpUserAccessNavControls = new CmpPageNavButtonsContainer(["blkUserAdminPolicyList","blkUserAccessFooter"],
                                                                [["blkUserAdminPolicyList", "btnPageFirst"],
                                                                 ["blkUserAdminPolicyList", "btnPagePrev"],
                                                                 ["blkUserAdminPolicyList", "btnPageNext"],
                                                                 ["blkUserAdminPolicyList", "btnPageLast"],
                                                                 ["blkUserAdminPolicyList", "lblRecordInfo"],
                                                                 ["blkUserAdminPolicyList", "txtCurrentRecordNumber"],
                                                                 ["blkUserAdminPolicyList", "btnPageRefresh"],
                                                                 ["blkUserAdminPolicyList", "btnPageRefreshSpinner"],
                                                                 ["blkUserAdminPolicyList", "btnPageRefreshNotify"]
                                                                ],
                                                                cmpMtxUserAdminList,
                                                                [{action:"TOPICEVENT", topic:tApp, eventvar:"UserAccessPolicyDataUpdated", eventval:null}],
                                                                null
                                                               );
  
  var cmpMenuUserPolicyList = new CmpMenu("menuUserPolicyList",
                                          [{ObjectType:"USERADMIN",
                                            MenuListArr:["Edit..."] }
                                          ],
                                          null, null, null,
                                          [{action:"APPEVENT", eventvar:"PolicyMenuTypeSelected", eventval:null}],
                                          {action:"APPEVENT", eventvar:"PolicyMenuActionSelected", eventval:"jsxtext"}
                                        );

  var cmpBlkUserAdminList = new CmpContainer( "blkUserAdminPolicyList", 
                              [ {cmp:cmpMtxUserAdminList},
                                {cmp:cmpBtnAddUserAdmin},
                                {cmp:cmpMenuUserPolicyList},
                                {cmp:cmpBtnDeleteUserAdmin},
                                {cmp:cmpUserAccessNavControls}
                              ],
                              null );

  /************************** User Admin Block END ^ ***************/
  
  /************************** Asset Admin Block START ******************/
  var cmpBtnDeleteAssetAdmin = new CmpButton("btnDeleteAssetAdminPolicy",
                                   [{ action: "APPEVENT", eventvar: "AssetAdminSelected", eventval: null}],
                                   { action: "FNCCALL",
                                     context: this,
                                     fnc: function () {
                                       this.SelectedPolicy = "AssetAdminPolicy";
                                       cmpBlkPolicyDelete.call(this);
                                     }
                                   });

  var cmpBtnAddAssetAdmin = new CmpButton("btnAddAssetAdminPolicy",
                                   null, 
                                   {action:"FNCCALL", 
                                    context:this,
                                        fnc:function() {
                                        dlgEditAssetAdmin_Create("DEFAULT_ASSETADMIN_NID", "AssetAdminPolicy");
                                        }
                                   });

  topic_Set(tApp, "AssetAccessPolicyWhereXPath", "");
  var cmpMtxAssetAdminList = new CmpCustomPaginationList(["blkAssetAdminPolicyList","mtxpolicyAssetAdminList"],
                                                         [{action:"APPEVENT", eventvar:"AssetAccessPolicyData", eventval:null}],
                                                         {persist:{col:"jsxid",initval:false},
                                                          action:"APPEVENT", eventvar:"AssetAdminSelected", eventval:"jsxid"},
                                                         null,
                                                         null
                                                        );

  cmpMtxAssetAdminList.allowMultiSelection = true;

  cmpMtxAssetAdminList.SortPathArr = {index:"$b//order",
                                     jsxtext:"upper-case($b//rulename)",
                                     jsxAssetPolicyType:"$b//allow"
                                   };

  cmpMtxAssetAdminList.cmpReqParamArr = {StartFrom: "Op_Select_ListSpec_StartFrom",
                                         CountToFetch: "Op_Select_ListSpec_CountToFetch",
                                         SortOrder: "Op_Select_ListSpec_SortOrder",
                                         ObjectCount: "ObjectCount",
                                         SortByXpath: "Op_Select_ListSpec_SortByXPathList_SortByXpath"
                                        };

  cmpMtxAssetAdminList.DataUpdatedEvent = { action: "TOPICEVENT", topic: tApp, eventvar: "AssetAccessPolicyDataUpdated", eventval: "LOADED" };

  cmpMtxAssetAdminList.cmpReq = new cmpReq_GetListReadQuery(tApp, "JSXAPPS/IPVS_AdminUI/query/AssetAccessPolicyRule.xml",
                                                            [{ VarName: "_WHEREXPATH_", VarRef: "AssetAccessPolicyWhereXPath"}
                                                            ],
                                                            { action: "FNCCALL", context: cmpMtxAssetAdminList,
                                                                fnc: function(respCode, respVal) {
                                                                    if (respCode == "SUCCESS" && respVal != null) {
                                                                        this.onResponse(respCode, respVal);
                                                                    }
                                                                    else {
                                                                        log.info("GetAssetAccessRequest failed. Error Code :" + respCode);
                                                                        this.onResponseErrorHandle("GetAssetAccessRequest",respCode, respVal);
                                                                    }
                                                                }
                                                            }
                                                          );
                                                          
  cmpMtxAssetAdminList.cmpItemByNIDReqFnc = function () {
                                             var strWhereNID = cmpMtxAssetAdminList.WhereNIDArr.join("\",\"");
                                             topic_Set(tApp, "WhereGroupNID", "[//@NID=(\"" + strWhereNID + "\")]");
                                             return new cmpReq_GetListReadQuery(tApp, "JSXAPPS/IPVS_AdminUI/query/AssetAccessPolicyRule.xml",
                                                                                [{VarName:"_WHEREXPATH_", VarRef:"WhereGroupNID"}],
                                                                                {action: "FNCCALL", context: cmpMtxAssetAdminList,
                                                                                 fnc: function(respCode, respVal) {
                                                                                        if (respCode == "SUCCESS" && respVal != null) {
                                                                                          this.onResponseUpdateRecord(respCode, respVal);
                                                                                        }
                                                                                        else{
                                                                                          log.info("GetAssetAccessRequest failed. Error Code :" + respCode);
                                                                                          this.onResponseErrorHandle("GetAssetAccessRequest",respCode, respVal);
                                                                                        }
                                                                                      }
                                                                                }, "ItemByNID"
                                                                               );
                                           }
                                                                
  cmpMtxAssetAdminList.MenuEvent = {action:"APPEVENT", eventvar:"PolicyMenuSelected", eventval:"jsxid"};

  var cmpAssetAccessNavControls = new CmpPageNavButtonsContainer(["blkAssetAdminPolicyList","blkAssetAccessFooter"],
                                                                [["blkAssetAdminPolicyList", "btnPageFirst"],
                                                                 ["blkAssetAdminPolicyList", "btnPagePrev"],
                                                                 ["blkAssetAdminPolicyList", "btnPageNext"],
                                                                 ["blkAssetAdminPolicyList", "btnPageLast"],
                                                                 ["blkAssetAdminPolicyList", "lblRecordInfo"],
                                                                 ["blkAssetAdminPolicyList", "txtCurrentRecordNumber"],
                                                                 ["blkAssetAdminPolicyList", "btnPageRefresh"],
                                                                 ["blkAssetAdminPolicyList", "btnPageRefreshSpinner"],
                                                                 ["blkAssetAdminPolicyList", "btnPageRefreshNotify"]
                                                                ],
                                                                cmpMtxAssetAdminList,
                                                                [{action:"TOPICEVENT", topic:tApp, eventvar:"AssetAccessPolicyDataUpdated", eventval:null}],
                                                                null
                                                               );
                                             
  var cmpMenuAssetPolicyList = new CmpMenu("menuAssetPolicyList",
                                          [{ObjectType:"ASSETADMIN",
                                            MenuListArr:["Edit..."] }
                                          ],
                                          null, null, null,
                                          [{action:"APPEVENT", eventvar:"PolicyMenuTypeSelected", eventval:null}],
                                          {action:"APPEVENT", eventvar:"PolicyMenuActionSelected", eventval:"jsxtext"}
                                        );

  var cmpBlkAssetAdminList = new CmpContainer( "blkAssetAdminPolicyList", 
                              [ {cmp:cmpMtxAssetAdminList},
                                {cmp:cmpBtnAddAssetAdmin},
                                {cmp:cmpMenuAssetPolicyList},
                                {cmp:cmpBtnDeleteAssetAdmin},
                                {cmp:cmpAssetAccessNavControls}
                              ],
                              null );
  /************************** Asset Admin Block END ^ ******************/

  /************************** Asset Admin Session Block START ******************/
   var cmpBtnDeleteAssetAdminSession = new CmpButton("btnDeleteAssetAdminSessionPolicy",
                                                     [{ action: "APPEVENT", eventvar: "AssetAdminSessionSelected", eventval: null}],
                                                     { action: "FNCCALL",
                                                       context: this,
                                                       fnc: function() {
                                                         this.SelectedPolicy = "AssetAdminSessionPolicy";
                                                         cmpBlkPolicyDelete.call(this);
                                                       }
                                                     });

  var cmpBtnEditAssetAdminSession = new CmpButton("btnEditAssetAdminSessionPolicy",
                                                   [{ action: "APPEVENT", eventvar: "AssetAdminSessionSelected", eventval: null}],
                                                   { action: "FNCCALL",
                                                     context: this,
                                                     fnc: function() {
                                                     this.SelectedNID = topic_Get(tApp, "AssetAdminSessionSelected");
                                                       dlgEditAssetAdmin_Create(this.SelectedNID, "AssetAdminSessionPolicy");
                                                     }
                                                   });

  var cmpBtnAddAssetAdminSession = new CmpButton("btnAddAssetAdminSessionPolicy",
                                                   null,
                                                   { action: "FNCCALL",
                                                     context: this,
                                                     fnc: function() {
                                                       dlgEditAssetAdmin_Create("DEFAULT_ASSETADMIN_NID", "AssetAdminSessionPolicy");
                                                     }
                                                   });

  var cmpMtxAssetAdminSessionList = new CmpNIDCustomList("mtxpolicyAssetAdminSessionList", cmpAssetAdminSession_Repaint,
                                                          [{ action: "REQEVENT", eventvar: "GroupTagDataState", eventval: null },
                                                           { action: "REQEVENT", eventvar: "TagObjectFilterDataState", eventval: null },
                                                           { action: "REQEVENT", eventvar: "AssetAdminDataState", eventval: null}],
                                                          { persist: { col: "jsxtext", initval: false },
                                                            action: "APPEVENT", eventvar: "AssetAdminSessionSelected", eventval: "jsxid"
                                                          }
                                                         );

  cmpMtxAssetAdminSessionList.allowMultiSelection = true;

  var cmpBlkAssetAdminSessionList = new CmpContainer("blkAssetAdminSessionPolicyList",
                                                      [{ cmp: cmpMtxAssetAdminSessionList },
                                                        { cmp: cmpBtnAddAssetAdminSession },
                                                        { cmp: cmpBtnEditAssetAdminSession },
                                                        { cmp: cmpBtnDeleteAssetAdminSession }
                                                      ],
                                                      null);
  /************************** Asset Admin BSeesion lock END ^ ******************/  

  /************************** MediaStore Admin Block START ******************/
  var cmpBtnDeleteMediaStoreAdmin = new CmpButton("btnDeleteMediaStoreAdminPolicy", 
                                                   [{action:"APPEVENT", eventvar:"MediaStoreAdminSelected", eventval:null}],
                                                   {action:"FNCCALL", 
                                                    context:this,
                                                        fnc:function() {
                                                           this.SelectedPolicy = "MediaStorePolicy";
                                                           cmpBlkPolicyDelete.call(this);
                                                        }
                                                   });

  var cmpBtnAddMediaStoreAdmin = new CmpButton("btnAddMediaStoreAdminPolicy",
                                               null, 
                                               {action:"FNCCALL", 
                                                context:this,
                                                    fnc:function() {
                                                    dlgEditMediaStoreAdmin_Create("DEFAULT_MEDIASTOREADMIN_NID", "MediaStoreAdminPolicy");
                                                    }
                                               });

  topic_Set(tApp, "MediaStoreAccessPolicyWhereXPath", "");
  var cmpMtxMediaStoreAdminList = new CmpCustomPaginationList(["blkMediaStorePolicyList","mtxpolicyMediaStoreList"],
                                                              [{action:"APPEVENT", eventvar:"MediaStoreAccessPolicyData", eventval:null}],
                                                              {persist:{col:"jsxid",initval:false},
                                                               action:"APPEVENT", eventvar:"MediaStoreAdminSelected", eventval:"jsxid"},
                                                              null,
                                                              null
                                                             );

  cmpMtxMediaStoreAdminList.allowMultiSelection = true;

  cmpMtxMediaStoreAdminList.SortPathArr = {index:"$b//order",
                                           jsxtext:"upper-case($b//rulename)",
                                           jsxMediaStorePolicyType:"$b//allow"
                                         };
  
  cmpMtxMediaStoreAdminList.cmpReqParamArr = {StartFrom: "Op_Select_ListSpec_StartFrom",
                                              CountToFetch: "Op_Select_ListSpec_CountToFetch",
                                              SortOrder: "Op_Select_ListSpec_SortOrder",
                                              ObjectCount: "ObjectCount",
                                              SortByXpath: "Op_Select_ListSpec_SortByXPathList_SortByXpath"
                                             };

  cmpMtxMediaStoreAdminList.DataUpdatedEvent = { action: "TOPICEVENT", topic: tApp, eventvar: "MediaStoreAccessPolicyDataUpdated", eventval: "LOADED" };

  cmpMtxMediaStoreAdminList.cmpReq = new cmpReq_GetListReadQuery(tApp, "JSXAPPS/IPVS_AdminUI/query/MediaStoreAccessPolicyRule.xml",
                                                                [{ VarName: "_WHEREXPATH_", VarRef: "MediaStoreAccessPolicyWhereXPath"}
                                                                ],
                                                                { action: "FNCCALL", context: cmpMtxMediaStoreAdminList,
                                                                    fnc: function(respCode, respVal) {
                                                                        if (respCode == "SUCCESS" && respVal != null) {
                                                                            this.onResponse(respCode, respVal);
                                                                        }
                                                                        else {
                                                                            log.info("GetMediaStoreAccessRequest failed. Error Code :" + respCode);
                                                                            this.onResponseErrorHandle("GetMediaStoreAccessRequest",respCode, respVal);
                                                                        }
                                                                    }
                                                                }
                                                              );
                                                              
  cmpMtxMediaStoreAdminList.cmpItemByNIDReqFnc = function () {
                                                 var strWhereNID = cmpMtxMediaStoreAdminList.WhereNIDArr.join("\",\"");
                                                 topic_Set(tApp, "WhereGroupNID", "[//@NID=(\"" + strWhereNID + "\")]");
                                                 return new cmpReq_GetListReadQuery(tApp, "JSXAPPS/IPVS_AdminUI/query/MediaStoreAccessPolicyRule.xml",
                                                                                    [{VarName:"_WHEREXPATH_", VarRef:"WhereGroupNID"}],
                                                                                    {action: "FNCCALL", context: cmpMtxMediaStoreAdminList,
                                                                                     fnc: function(respCode, respVal) {
                                                                                            if (respCode == "SUCCESS" && respVal != null) {
                                                                                              this.onResponseUpdateRecord(respCode, respVal);
                                                                                            }
                                                                                            else{
                                                                                              log.info("GetMediaStoreAccessRequest failed. Error Code :" + respCode);
                                                                                              this.onResponseErrorHandle("GetMediaStoreAccessRequest",respCode, respVal);
                                                                                            }
                                                                                          }
                                                                                    }, "ItemByNID"
                                                                                   );
                                               }
                                                                
  cmpMtxMediaStoreAdminList.MenuEvent = {action:"APPEVENT", eventvar:"PolicyMenuSelected", eventval:"jsxid"};

  var cmpMediaStoreAccessNavControls = new CmpPageNavButtonsContainer(["blkMediaStorePolicyList","blkMediaStoreAccessFooter"],
                                                                      [["blkMediaStorePolicyList", "btnPageFirst"],
                                                                       ["blkMediaStorePolicyList", "btnPagePrev"],
                                                                       ["blkMediaStorePolicyList", "btnPageNext"],
                                                                       ["blkMediaStorePolicyList", "btnPageLast"],
                                                                       ["blkMediaStorePolicyList", "lblRecordInfo"],
                                                                       ["blkMediaStorePolicyList", "txtCurrentRecordNumber"],
                                                                       ["blkMediaStorePolicyList", "btnPageRefresh"],
                                                                       ["blkMediaStorePolicyList", "btnPageRefreshSpinner"],
                                                                       ["blkMediaStorePolicyList", "btnPageRefreshNotify"]
                                                                      ],
                                                                      cmpMtxMediaStoreAdminList,
                                                                      [{action:"TOPICEVENT", topic:tApp, eventvar:"MediaStoreAccessPolicyDataUpdated", eventval:null}],
                                                                      null
                                                                     ); 

  var cmpMenuMediaStorePolicyList = new CmpMenu("menuMediaStorePolicyList",
                                                [{ObjectType:"MEDIASTORE",
                                                  MenuListArr:["Edit..."] }
                                                ],
                                                null, null, null,
                                                [{action:"APPEVENT", eventvar:"PolicyMenuTypeSelected", eventval:null}],
                                                {action:"APPEVENT", eventvar:"PolicyMenuActionSelected", eventval:"jsxtext"}
                                              );

  var cmpBlkMediaStoreAdminList = new CmpContainer( "blkMediaStorePolicyList", 
                                                    [ {cmp:cmpMtxMediaStoreAdminList},
                                                      {cmp:cmpBtnAddMediaStoreAdmin},
                                                      {cmp:cmpMenuMediaStorePolicyList},
                                                      {cmp:cmpBtnDeleteMediaStoreAdmin},
                                                      {cmp:cmpMediaStoreAccessNavControls}
                                                    ],
                                                    null );  
  
      
  /************************** MediaStore Admin Block END ^ **************/
  
  /************************** Policy Admin Policy Block START \/ **************/
    var cmpBtnDeletePolicyAdminPolicy = new CmpButton("btnDeletePolicyAdminPolicy", 
                                                       [{action:"APPEVENT", eventvar:"PolicyAdminPolicySelected", eventval:null}],
                                                       {action:"FNCCALL", 
                                                        context:this,
                                                            fnc:function() {
                                                              this.SelectedPolicy = "PolicyAdminPolicyRule";
                                                               cmpBlkPolicyDelete.call(this);
                                                            }
                                                       });

  var cmpBtnAddPolicyAdminPolicy = new CmpButton("btnAddPolicyAdminPolicy",
                                                 null, 
                                                 {action:"FNCCALL", 
                                                  context:this,
                                                      fnc:function() {
                                                         dlgEditPolicyAdminPolicy_Create("DEFAULT_POLICYADMINPOLICY_NID");
                                                      }
                                                 });

    topic_Set(tApp, "PolicyAccessPolicyWhereXPath", "");
    var cmpMtxPolicyAdminPolicyList = new CmpCustomPaginationList(["blkPolicyAdminPolicyList","mtxpolicyAdminPolicyList"],
                                                                  [{action:"APPEVENT", eventvar:"PolicyAccessPolicyData", eventval:null}],
                                                                  {persist:{col:"jsxid",initval:false},
                                                                   action:"APPEVENT", eventvar:"PolicyAdminPolicySelected", eventval:"jsxid"},
                                                                  null,
                                                                  null
                                                                 );

    cmpMtxPolicyAdminPolicyList.allowMultiSelection = true;
                                                                  
    cmpMtxPolicyAdminPolicyList.SortPathArr = {index:"$b//order",
                                               jsxtext:"upper-case($b//rulename)",
                                               jsxPolicyAdminPolicyType:"$b//allow"
                                             };

    cmpMtxPolicyAdminPolicyList.cmpReqParamArr = {StartFrom: "Op_Select_ListSpec_StartFrom",
                                                  CountToFetch: "Op_Select_ListSpec_CountToFetch",
                                                  SortOrder: "Op_Select_ListSpec_SortOrder",
                                                  ObjectCount: "ObjectCount",
                                                  SortByXpath: "Op_Select_ListSpec_SortByXPathList_SortByXpath"
                                                 };

                                                                                               
    cmpMtxPolicyAdminPolicyList.DataUpdatedEvent = { action: "TOPICEVENT", topic: tApp, eventvar: "PolicyAccessDataUpdated", eventval: "LOADED" };

    cmpMtxPolicyAdminPolicyList.cmpReq = new cmpReq_GetListReadQuery(tApp, "JSXAPPS/IPVS_AdminUI/query/PolicyAccessPolicyRule.xml",
                                                                    [{ VarName: "_WHEREXPATH_", VarRef: "PolicyAccessPolicyWhereXPath"}
                                                                    ],
                                                                    { action: "FNCCALL", context: cmpMtxPolicyAdminPolicyList,
                                                                        fnc: function(respCode, respVal) {
                                                                            if (respCode == "SUCCESS" && respVal != null) {
                                                                                this.onResponse(respCode, respVal);
                                                                            }
                                                                            else {
                                                                                log.info("GetPolicyAccessRequest failed. Error Code :" + respCode);
                                                                                this.onResponseErrorHandle("GetPolicyAccessRequest",respCode, respVal);
                                                                            }
                                                                        }
                                                                    }
                                                                  ); 
                                                                  
    cmpMtxPolicyAdminPolicyList.cmpItemByNIDReqFnc = function () {
                                                     var strWhereNID = cmpMtxPolicyAdminPolicyList.WhereNIDArr.join("\",\"");
                                                     topic_Set(tApp, "WhereGroupNID", "[//@NID=(\"" + strWhereNID + "\")]");
                                                     return new cmpReq_GetListReadQuery(tApp, "JSXAPPS/IPVS_AdminUI/query/PolicyAccessPolicyRule.xml",
                                                                                        [{VarName:"_WHEREXPATH_", VarRef:"WhereGroupNID"}],
                                                                                        {action: "FNCCALL", context: cmpMtxPolicyAdminPolicyList,
                                                                                         fnc: function(respCode, respVal) {
                                                                                                if (respCode == "SUCCESS" && respVal != null) {
                                                                                                  this.onResponseUpdateRecord(respCode, respVal);
                                                                                                }
                                                                                                else{
                                                                                                  log.info("GetPolicyAccessRequest failed. Error Code :" + respCode);
                                                                                                  if(respCode != "3014"){
                                                                                                    this.onResponseErrorHandle("GetPolicyAccessRequest",respCode, respVal);
                                                                                                  }
                                                                                                }
                                                                                              }
                                                                                        }, "ItemByNID"
                                                                                       );
                                                   }
                                                                  
    cmpMtxPolicyAdminPolicyList.MenuEvent = {action:"APPEVENT", eventvar:"PolicyMenuSelected", eventval:"jsxid"};

    var cmpPolicyAccessNavControls = new CmpPageNavButtonsContainer(["blkPolicyAdminPolicyList","blkPolicyAccessFooter"],
                                                                    [["blkPolicyAdminPolicyList", "btnPageFirst"],
                                                                     ["blkPolicyAdminPolicyList", "btnPagePrev"],
                                                                     ["blkPolicyAdminPolicyList", "btnPageNext"],
                                                                     ["blkPolicyAdminPolicyList", "btnPageLast"],
                                                                     ["blkPolicyAdminPolicyList", "lblRecordInfo"],
                                                                     ["blkPolicyAdminPolicyList", "txtCurrentRecordNumber"],
                                                                     ["blkPolicyAdminPolicyList", "btnPageRefresh"],
                                                                     ["blkPolicyAdminPolicyList", "btnPageRefreshSpinner"],
                                                                     ["blkPolicyAdminPolicyList", "btnPageRefreshNotify"]
                                                                    ],
                                                                    cmpMtxPolicyAdminPolicyList,
                                                                    [{action:"TOPICEVENT", topic:tApp, eventvar:"PolicyAccessDataUpdated", eventval:null}],
                                                                    null
                                                                   ); 

  var cmpMenuPolicyAdminPolicyList = new CmpMenu("menuPolicyAdminPolicyList",
                                                [{ObjectType:"POLICYADMINPOLICY",
                                                  MenuListArr:["Edit..."] }
                                                ],
                                                null, null, null,
                                                [{action:"APPEVENT", eventvar:"PolicyMenuTypeSelected", eventval:null}],
                                                {action:"APPEVENT", eventvar:"PolicyMenuActionSelected", eventval:"jsxtext"}
                                              );

  var cmpBlkPolicyAdminPolicyList = new CmpContainer( "blkPolicyAdminPolicyList", 
                                                      [ {cmp:cmpMtxPolicyAdminPolicyList},
                                                        {cmp:cmpBtnAddPolicyAdminPolicy},
                                                        {cmp:cmpMenuPolicyAdminPolicyList},
                                                        {cmp:cmpBtnDeletePolicyAdminPolicy},
                                                        {cmp:cmpPolicyAccessNavControls}
                                                      ],
                                                      null );  
  
      
  /************************** Policy Admin Policy Block END ^ **************/                                 

  /************************** NAT Block START *****************/                                 
  var cmpBtnDelete = new CmpButton("btnDeleteNATPolicy", 
                                   [{action:"APPEVENT", eventvar:"NATPolicySelected", eventval:null}],
                                   {action:"FNCCALL", 
                                    context:this,
                                        fnc:function() {
                                          this.SelectedPolicy = "NetworkAddressRule";
                                           cmpBlkPolicyDelete.call(this);
                                        }
                                   });

  var cmpBtnAdd = new CmpButton("btnAddNATPolicy",
                                 null, 
                                 {action:"FNCCALL", 
                                  context:this,
                                      fnc:function() {
                                         dlgEditNetworkAddress_Create("DEFAULT_NAT_NID");
                                      }
                                 });

    topic_Set(tApp, "NATAccessPolicyWhereXPath", "");
    var cmpMtxNATList = new CmpCustomPaginationList(["blkNATPolicyList","mtxpolicyNATList"],
                                                    [{action:"APPEVENT", eventvar:"NATAccessPolicyData", eventval:null}],
                                                    {persist:{col:"jsxid",initval:false},
                                                     action:"APPEVENT", eventvar:"NATPolicySelected", eventval:"jsxid"},
                                                    null,
                                                    null
                                                   );

     cmpMtxNATList.allowMultiSelection = true;

    cmpMtxNATList.SortPathArr = {jsxOrder:"$b//order",
                                 jsxtext:"upper-case($b//rulename)"
                               };

    cmpMtxNATList.cmpReqParamArr = {StartFrom: "Op_Select_ListSpec_StartFrom",
                                    CountToFetch: "Op_Select_ListSpec_CountToFetch",
                                    SortOrder: "Op_Select_ListSpec_SortOrder",
                                    ObjectCount: "ObjectCount",
                                    SortByXpath: "Op_Select_ListSpec_SortByXPathList_SortByXpath"
                                   };

                                                                                               
    cmpMtxNATList.DataUpdatedEvent = { action: "TOPICEVENT", topic: tApp, eventvar: "NATAccessDataUpdated", eventval: "LOADED" };

    cmpMtxNATList.cmpReq = new cmpReq_GetListReadQuery(tApp, "JSXAPPS/IPVS_AdminUI/query/NATAccessPolicyRule.xml",
                                                        [{ VarName: "_WHEREXPATH_", VarRef: "NATAccessPolicyWhereXPath"}
                                                        ],
                                                        { action: "FNCCALL", context: cmpMtxNATList,
                                                            fnc: function(respCode, respVal) {
                                                                if (respCode == "SUCCESS" && respVal != null) {
                                                                    this.onResponse(respCode, respVal);
                                                                }
                                                                else {
                                                                    log.info("GetNATAccessRequest failed. Error Code :" + respCode);
                                                                    this.onResponseErrorHandle("GetNATAccessRequest",respCode, respVal);
                                                                }
                                                            }
                                                        }
                                                      );  
                                                      
    cmpMtxNATList.cmpItemByNIDReqFnc = function () {
                                       var strWhereNID = cmpMtxNATList.WhereNIDArr.join("\",\"");
                                       topic_Set(tApp, "WhereGroupNID", "[//@NID=(\"" + strWhereNID + "\")]");
                                       return new cmpReq_GetListReadQuery(tApp, "JSXAPPS/IPVS_AdminUI/query/NATAccessPolicyRule.xml",
                                                                          [{VarName:"_WHEREXPATH_", VarRef:"WhereGroupNID"}],
                                                                          {action: "FNCCALL", context: cmpMtxNATList,
                                                                           fnc: function(respCode, respVal) {
                                                                                  if (respCode == "SUCCESS" && respVal != null) {
                                                                                    this.onResponseUpdateRecord(respCode, respVal);
                                                                                  }
                                                                                  else{
                                                                                    log.info("GetNATAccessRequest failed. Error Code :" + respCode);
                                                                                    this.onResponseErrorHandle("GetNATAccessRequest",respCode, respVal);
                                                                                  }
                                                                                }
                                                                          }, "ItemByNID"
                                                                         );
                                     }
                                                                  
    cmpMtxNATList.MenuEvent = {action:"APPEVENT", eventvar:"PolicyMenuSelected", eventval:"jsxid"};

    var cmpNATAccessNavControls = new CmpPageNavButtonsContainer(["blkNATPolicyList","blkNATAccessFooter"],
                                                                [["blkNATPolicyList", "btnPageFirst"],
                                                                 ["blkNATPolicyList", "btnPagePrev"],
                                                                 ["blkNATPolicyList", "btnPageNext"],
                                                                 ["blkNATPolicyList", "btnPageLast"],
                                                                 ["blkNATPolicyList", "lblRecordInfo"],
                                                                 ["blkNATPolicyList", "txtCurrentRecordNumber"],
                                                                 ["blkNATPolicyList", "btnPageRefresh"],
                                                                 ["blkNATPolicyList", "btnPageRefreshSpinner"],
                                                                 ["blkNATPolicyList", "btnPageRefreshNotify"]
                                                                ],
                                                                cmpMtxNATList,
                                                                [{action:"TOPICEVENT", topic:tApp, eventvar:"NATAccessDataUpdated", eventval:null}],
                                                                null
                                                               ); 

  var cmpMenuNATPolicyList = new CmpMenu("menuNATPolicyList",
                                        [{ObjectType:"NATPOLICY",
                                          MenuListArr:["Edit..."] }
                                        ],
                                        null, null, null,
                                        [{action:"APPEVENT", eventvar:"PolicyMenuTypeSelected", eventval:null}],
                                        {action:"APPEVENT", eventvar:"PolicyMenuActionSelected", eventval:"jsxtext"}
                                      );

  var cmpBlkNATList = new CmpContainer( "blkNATPolicyList", 
                                        [ {cmp:cmpMtxNATList},
                                          {cmp:cmpBtnAdd},
                                          {cmp:cmpMenuNATPolicyList},
                                          {cmp:cmpBtnDelete},
                                          {cmp:cmpNATAccessNavControls}
                                        ],
                                        null );
  /************************** NAT Block End ^******************/
    
  /************************** COLLABORATION Block START ******************/
  var cmpBtnDeleteCollab = new CmpButton("btnDeleteCollabPolicy", 
                                         [{action:"APPEVENT", eventvar:"CollabSelected", eventval:null}],
                                         {action:"FNCCALL", 
                                          context:this,
                                              fnc:function() {
                                                 this.SelectedPolicy = "MediaRoomPolicyRule";
                                                 cmpBlkPolicyDelete.call(this);
                                              }
                                         });

  var cmpBtnAddCollab = new CmpButton("btnAddCollabPolicy",
                                       null, 
                                       {action:"FNCCALL", 
                                        context:this,
                                            fnc:function() {
                                               dlgEditCollab_Create("DEFAULT_COLLAB_NID");
                                            }
                                       });

  topic_Set(tApp, "CollabAccessPolicyWhereXPath", "");
  var cmpMtxCollabList = new CmpCustomPaginationList(["blkCollabPolicyList","mtxpolicyCollabList"],
                                                    [{action:"APPEVENT", eventvar:"CollabAccessPolicyData", eventval:null}],
                                                    {persist:{col:"jsxid",initval:false},
                                                     action:"APPEVENT", eventvar:"CollabSelected", eventval:"jsxid"},
                                                    null,
                                                    null
                                                   );

  cmpMtxCollabList.allowMultiSelection = true;

  cmpMtxCollabList.SortPathArr = {index:"$b//order",
                                  jsxtext:"upper-case($b//rulename)",
                                  jsxCollabPolicyType:"$b//allow"
                                 };
  
  cmpMtxCollabList.cmpReqParamArr = {StartFrom: "Op_Select_ListSpec_StartFrom",
                                     CountToFetch: "Op_Select_ListSpec_CountToFetch",
                                     SortOrder: "Op_Select_ListSpec_SortOrder",
                                     ObjectCount: "ObjectCount",
                                     SortByXpath: "Op_Select_ListSpec_SortByXPathList_SortByXpath"
                                    };

  cmpMtxCollabList.DataUpdatedEvent = { action: "TOPICEVENT", topic: tApp, eventvar: "CollabPolicyAccessDataUpdated", eventval: "LOADED" };

  cmpMtxCollabList.cmpReq = new cmpReq_GetListReadQuery(tApp, "JSXAPPS/IPVS_AdminUI/query/CollabAccessPolicyRule.xml",
                                                        [{ VarName: "_WHEREXPATH_", VarRef: "CollabAccessPolicyWhereXPath"}
                                                        ],
                                                        { action: "FNCCALL", context: cmpMtxCollabList,
                                                            fnc: function(respCode, respVal) {
                                                                if (respCode == "SUCCESS" && respVal != null) {
                                                                    this.onResponse(respCode, respVal);
                                                                }
                                                                else {
                                                                    log.info("GetCollabAccessRequest failed. Error Code :" + respCode);
                                                                    this.onResponseErrorHandle("GetCollabAccessRequest",respCode, respVal);
                                                                }
                                                            }
                                                        }
                                                      );
                                                      
   cmpMtxCollabList.cmpItemByNIDReqFnc = function () {
                                         var strWhereNID = cmpMtxCollabList.WhereNIDArr.join("\",\"");
                                         topic_Set(tApp, "WhereGroupNID", "[//@NID=(\"" + strWhereNID + "\")]");
                                         return new cmpReq_GetListReadQuery(tApp, "JSXAPPS/IPVS_AdminUI/query/CollabAccessPolicyRule.xml",
                                                                            [{VarName:"_WHEREXPATH_", VarRef:"WhereGroupNID"}],
                                                                            {action: "FNCCALL", context: cmpMtxCollabList,
                                                                             fnc: function(respCode, respVal) {
                                                                                    if (respCode == "SUCCESS" && respVal != null) {
                                                                                      this.onResponseUpdateRecord(respCode, respVal);
                                                                                    }
                                                                                    else{
                                                                                      log.info("GetCollabAccessRequest failed. Error Code :" + respCode);
                                                                                      this.onResponseErrorHandle("GetCollabAccessRequest",respCode, respVal);
                                                                                    }
                                                                                  }
                                                                            }, "ItemByNID"
                                                                           );
                                       }
                                                                
  cmpMtxCollabList.MenuEvent = {action:"APPEVENT", eventvar:"PolicyMenuSelected", eventval:"jsxid"};

  var cmpCollabAccessNavControls = new CmpPageNavButtonsContainer(["blkCollabPolicyList", "blkCollabAccessFooter"],
                                                                  [["blkCollabPolicyList", "btnPageFirst"],
                                                                   ["blkCollabPolicyList", "btnPagePrev"],
                                                                   ["blkCollabPolicyList", "btnPageNext"],
                                                                   ["blkCollabPolicyList", "btnPageLast"],
                                                                   ["blkCollabPolicyList", "lblRecordInfo"],
                                                                   ["blkCollabPolicyList", "txtCurrentRecordNumber"],
                                                                   ["blkCollabPolicyList", "btnPageRefresh"],
                                                                   ["blkCollabPolicyList", "btnPageRefreshSpinner"],
                                                                   ["blkCollabPolicyList", "btnPageRefreshNotify"]
                                                                  ],
                                                                  cmpMtxCollabList,
                                                                  [{action:"TOPICEVENT", topic:tApp, eventvar:"CollabPolicyAccessDataUpdated", eventval:null}],
                                                                  null
                                                                 ); 

  var cmpMenuCollabPolicyList = new CmpMenu("menuCollabPolicyList",
                                            [{ObjectType:"COLLABPOLICY",
                                              MenuListArr:["Edit..."] }
                                            ],
                                            null, null, null,
                                            [{action:"APPEVENT", eventvar:"PolicyMenuTypeSelected", eventval:null}],
                                            {action:"APPEVENT", eventvar:"PolicyMenuActionSelected", eventval:"jsxtext"}
                                          );

  var cmpBlkCollabList = new CmpContainer( "blkCollabPolicyList", 
                                          [ {cmp:cmpMtxCollabList},
                                            {cmp:cmpBtnAddCollab},
                                            {cmp:cmpMenuCollabPolicyList},
                                            {cmp:cmpBtnDeleteCollab},
                                            {cmp:cmpCollabAccessNavControls}
                                          ],
                                          null );
  /************************** COLLABORATION Block END ^ ******************/

  /************************** PORTACCESS Block START ^ ******************/
  
  var cmpBtnDeletePortAccessPolicy = new CmpButton("btnDeletePortAccessPolicy",
                                                     [{ action: "APPEVENT", eventvar: "PortAccessPolicySelected", eventval: null}],
                                                     { action: "FNCCALL",
                                                       context: this,
                                                       fnc: function() {
                                                         this.SelectedPolicy = "PortAccessPolicy";
                                                         cmpBlkPolicyDelete.call(this);
                                                       }
                                                     });

    var cmpBtnAddPortAccessPolicy = new CmpButton("btnAddPortAccessPolicy",
                                                  null,
                                                  { action: "FNCCALL",
                                                  context: this,
                                                  fnc: function() {
                                                  dlgEditAssetAdmin_Create("DEFAULT_ASSETADMIN_NID", "PortAccessPolicy");
                                                  }
                                                  });

    topic_Set(tApp, "PortAccessPolicyWhereXPath", "");
    var cmpMtxPortAccessPolicyList = new CmpCustomPaginationList(["blkPortAccessPolicyList","mtxpolicyPortAccessList"],
                                                                  [{action:"APPEVENT", eventvar:"PortAccessPolicyData", eventval:null}],
                                                                  {persist:{col:"jsxid",initval:false},
                                                                   action:"APPEVENT", eventvar:"PortAccessPolicySelected", eventval:"jsxid"},
                                                                  null,
                                                                  null
                                                                 );

    cmpMtxPortAccessPolicyList.allowMultiSelection = true;

    cmpMtxPortAccessPolicyList.SortPathArr = {index:"$b//order",
                                             jsxtext:"upper-case($b//rulename)",
                                             jsxPortAccessPolicyType:"$b//allow"
                                           };

    cmpMtxPortAccessPolicyList.cmpReqParamArr = {StartFrom: "Op_Select_ListSpec_StartFrom",
                                                 CountToFetch: "Op_Select_ListSpec_CountToFetch",
                                                 SortOrder: "Op_Select_ListSpec_SortOrder",
                                                 ObjectCount: "ObjectCount",
                                                 SortByXpath: "Op_Select_ListSpec_SortByXPathList_SortByXpath"
                                                };

                                                                                               
    cmpMtxPortAccessPolicyList.DataUpdatedEvent = { action: "TOPICEVENT", topic: tApp, eventvar: "PortAccessDataUpdated", eventval: "LOADED" };

    cmpMtxPortAccessPolicyList.cmpReq = new cmpReq_GetListReadQuery(tApp, "JSXAPPS/IPVS_AdminUI/query/PortAccessPolicyRule.xml",
                                                                    [{ VarName: "_WHEREXPATH_", VarRef: "PortAccessPolicyWhereXPath"}
                                                                    ],
                                                                    { action: "FNCCALL", context: cmpMtxPortAccessPolicyList,
                                                                        fnc: function(respCode, respVal) {
                                                                            if (respCode == "SUCCESS" && respVal != null) {
                                                                                this.onResponse(respCode, respVal);
                                                                            }
                                                                            else {
                                                                                log.info("GetPortAccessRequest failed. Error Code :" + respCode);
                                                                                this.onResponseErrorHandle("GetPortAccessRequest",respCode, respVal);
                                                                            }
                                                                        }
                                                                    }
                                                                  ); 
                                                                  
    cmpMtxPortAccessPolicyList.cmpItemByNIDReqFnc = function () {
                                                     var strWhereNID = cmpMtxPortAccessPolicyList.WhereNIDArr.join("\",\"");
                                                     topic_Set(tApp, "WhereGroupNID", "[//@NID=(\"" + strWhereNID + "\")]");
                                                     return new cmpReq_GetListReadQuery(tApp, "JSXAPPS/IPVS_AdminUI/query/PortAccessPolicyRule.xml",
                                                                                        [{VarName:"_WHEREXPATH_", VarRef:"WhereGroupNID"}],
                                                                                        {action: "FNCCALL", context: cmpMtxPortAccessPolicyList,
                                                                                         fnc: function(respCode, respVal) {
                                                                                                if (respCode == "SUCCESS" && respVal != null) {
                                                                                                  this.onResponseUpdateRecord(respCode, respVal);
                                                                                                }
                                                                                                else{
                                                                                                  log.info("GetPortAccessRequest failed. Error Code :" + respCode);
                                                                                                  this.onResponseErrorHandle("GetPortAccessRequest",respCode, respVal);
                                                                                                }
                                                                                              }
                                                                                        }, "ItemByNID"
                                                                                       );
                                                   }

    cmpMtxPortAccessPolicyList.MenuEvent = {action:"APPEVENT", eventvar:"PolicyMenuSelected", eventval:"jsxid"};
                                                                  
    var cmpPortAccessNavControls = new CmpPageNavButtonsContainer(["blkPortAccessPolicyList","blkPortAccessFooter"],
                                                                  [["blkPortAccessPolicyList", "btnPageFirst"],
                                                                   ["blkPortAccessPolicyList", "btnPagePrev"],
                                                                   ["blkPortAccessPolicyList", "btnPageNext"],
                                                                   ["blkPortAccessPolicyList", "btnPageLast"],
                                                                   ["blkPortAccessPolicyList", "lblRecordInfo"],
                                                                   ["blkPortAccessPolicyList", "txtCurrentRecordNumber"],
                                                                   ["blkPortAccessPolicyList", "btnPageRefresh"],
                                                                   ["blkPortAccessPolicyList", "btnPageRefreshSpinner"],
                                                                   ["blkPortAccessPolicyList", "btnPageRefreshNotify"]
                                                                  ],
                                                                  cmpMtxPortAccessPolicyList,
                                                                  [{action:"TOPICEVENT", topic:tApp, eventvar:"PortAccessDataUpdated", eventval:null}],
                                                                  null
                                                                 ); 

    var cmpMenuPortsPolicyList = new CmpMenu("menuPortsPolicyList",
                                            [{ObjectType:"PORTACCESS",
                                              MenuListArr:["Edit..."] }
                                            ],
                                            null, null, null,
                                            [{action:"APPEVENT", eventvar:"PolicyMenuTypeSelected", eventval:null}],
                                            {action:"APPEVENT", eventvar:"PolicyMenuActionSelected", eventval:"jsxtext"}
                                          );

    var cmpBlkPortAccessPolicyList = new CmpContainer("blkPortAccessPolicyList",
                                                      [{ cmp: cmpMtxPortAccessPolicyList },
                                                       { cmp: cmpBtnAddPortAccessPolicy },
                                                       { cmp: cmpMenuPortsPolicyList },
                                                       { cmp: cmpBtnDeletePortAccessPolicy },
                                                       { cmp: cmpPortAccessNavControls}
                                                      ],
                                                      null);
  /************************** PORTACCESS Block END ^ ******************/

  /************************** MEDIACCESS Block START ^ ******************/
  var cmpBtnDeleteMediaAccessPolicy = new CmpButton("btnDeleteMediaAccessPolicy",
                                                         [{ action: "APPEVENT", eventvar: "MediaAccessPolicySelected", eventval: null}],
                                                         { action: "FNCCALL",
                                                           context: this,
                                                           fnc: function() {
                                                           this.SelectedPolicy = "MediaAccessPolicy";
                                                             cmpBlkPolicyDelete.call(this);
                                                           }
                                                         });

  var cmpBtnAddMediaAccessPolicy = new CmpButton("btnAddMediaAccessPolicy",
                                                     null,
                                                     { action: "FNCCALL",
                                                       context: this,
                                                       fnc: function() {
                                                       dlgEditMediaStoreAdmin_Create("DEFAULT_MEDIASTOREADMIN_NID", "MediaAccessPolicy");
                                                       }
                                                     });

  topic_Set(tApp, "MediaAccessPolicyWhereXPath", "");
  var cmpMtxMediaAccessPolicyList = new CmpCustomPaginationList(["blkMediaAccessPolicyList","mtxpolicyMediaAccessList"],
                                                                [{action:"APPEVENT", eventvar:"MediaAccessPolicyData", eventval:null}],
                                                                {persist:{col:"jsxid",initval:false},
                                                                 action:"APPEVENT", eventvar:"MediaAccessPolicySelected", eventval:"jsxid"},
                                                                null,
                                                                null
                                                               );

  cmpMtxMediaAccessPolicyList.allowMultiSelection = true;

  cmpMtxMediaAccessPolicyList.SortPathArr = {index:"$b//order",
                                             jsxtext:"upper-case($b//rulename)",
                                             jsxMediaAccessPolicyType:"$b//allow"
                                           };

  cmpMtxMediaAccessPolicyList.cmpReqParamArr = {StartFrom: "Op_Select_ListSpec_StartFrom",
                                                CountToFetch: "Op_Select_ListSpec_CountToFetch",
                                                SortOrder: "Op_Select_ListSpec_SortOrder",
                                                ObjectCount: "ObjectCount",
                                                SortByXpath: "Op_Select_ListSpec_SortByXPathList_SortByXpath"
                                               };

                                                                                             
  cmpMtxMediaAccessPolicyList.DataUpdatedEvent = { action: "TOPICEVENT", topic: tApp, eventvar: "MediaAccessDataUpdated", eventval: "LOADED" };

  cmpMtxMediaAccessPolicyList.cmpReq = new cmpReq_GetListReadQuery(tApp, "JSXAPPS/IPVS_AdminUI/query/MediaAccessPolicyRule.xml",
                                                                  [{ VarName: "_WHEREXPATH_", VarRef: "MediaAccessPolicyWhereXPath"}
                                                                  ],
                                                                  { action: "FNCCALL", context: cmpMtxMediaAccessPolicyList,
                                                                      fnc: function(respCode, respVal) {
                                                                          if (respCode == "SUCCESS" && respVal != null) {
                                                                              this.onResponse(respCode, respVal);
                                                                          }
                                                                          else {
                                                                              log.info("GetMediaAccessRequest failed. Error Code :" + respCode);
                                                                              this.onResponseErrorHandle("GetMediaAccessRequest",respCode, respVal);
                                                                          }
                                                                      }
                                                                  }
                                                                );  
                                                                
    cmpMtxMediaAccessPolicyList.cmpItemByNIDReqFnc = function () {
                                                     var strWhereNID = cmpMtxMediaAccessPolicyList.WhereNIDArr.join("\",\"");
                                                     topic_Set(tApp, "WhereGroupNID", "[//@NID=(\"" + strWhereNID + "\")]");
                                                     return new cmpReq_GetListReadQuery(tApp, "JSXAPPS/IPVS_AdminUI/query/MediaAccessPolicyRule.xml",
                                                                                        [{VarName:"_WHEREXPATH_", VarRef:"WhereGroupNID"}],
                                                                                        {action: "FNCCALL", context: cmpMtxMediaAccessPolicyList,
                                                                                         fnc: function(respCode, respVal) {
                                                                                                if (respCode == "SUCCESS" && respVal != null) {
                                                                                                  this.onResponseUpdateRecord(respCode, respVal);
                                                                                                }
                                                                                                else{
                                                                                                  log.info("GetMediaAccessRequest failed. Error Code :" + respCode);
                                                                                                  this.onResponseErrorHandle("GetMediaAccessRequest",respCode, respVal);
                                                                                                }
                                                                                              }
                                                                                        }, "ItemByNID"
                                                                                       );
                                                   }
  cmpMtxMediaAccessPolicyList.MenuEvent = {action:"APPEVENT", eventvar:"PolicyMenuSelected", eventval:"jsxid"};

  var cmpMediaAccessNavControls = new CmpPageNavButtonsContainer(["blkMediaAccessPolicyList","blkMediaAccessFooter"],
                                                                [["blkMediaAccessPolicyList", "btnPageFirst"],
                                                                 ["blkMediaAccessPolicyList", "btnPagePrev"],
                                                                 ["blkMediaAccessPolicyList", "btnPageNext"],
                                                                 ["blkMediaAccessPolicyList", "btnPageLast"],
                                                                 ["blkMediaAccessPolicyList", "lblRecordInfo"],
                                                                 ["blkMediaAccessPolicyList", "txtCurrentRecordNumber"],
                                                                 ["blkMediaAccessPolicyList", "btnPageRefresh"],
                                                                 ["blkMediaAccessPolicyList", "btnPageRefreshSpinner"],
                                                                 ["blkMediaAccessPolicyList", "btnPageRefreshNotify"]
                                                                ],
                                                                cmpMtxMediaAccessPolicyList,
                                                                [{action:"TOPICEVENT", topic:tApp, eventvar:"MediaAccessDataUpdated", eventval:null}],
                                                                null
                                                               ); 

  var cmpMenuPlayBackPolicyList = new CmpMenu("menuPlaybackPolicyList",
                                              [{ObjectType:"MEDIACCESS",
                                                MenuListArr:["Edit..."] }
                                              ],
                                              null, null, null,
                                              [{action:"APPEVENT", eventvar:"PolicyMenuTypeSelected", eventval:null}],
                                              {action:"APPEVENT", eventvar:"PolicyMenuActionSelected", eventval:"jsxtext"}
                                            );

  var cmpBlkMediaAccessPolicyList = new CmpContainer("blkMediaAccessPolicyList",
                                                    [{ cmp: cmpMtxMediaAccessPolicyList },
                                                     { cmp: cmpBtnAddMediaAccessPolicy },
                                                     { cmp: cmpMenuPlayBackPolicyList },
                                                     { cmp: cmpBtnDeleteMediaAccessPolicy },
                                                     { cmp: cmpMediaAccessNavControls}
                                                    ],
                                                    null); 
  /************************** MEDIACCESS Block END ^ ******************/

  /************************** RECORDINGSACCESS Block START ^ ******************/
  var cmpBtnDeleteRecordingAccessPolicy = new CmpButton("btnDeleteCreateRecordingsPolicy",
                                                         [{ action: "APPEVENT", eventvar: "RecordingAccessPolicySelected", eventval: null}],
                                                         { action: "FNCCALL",
                                                           context: this,
                                                           fnc: function() {
                                                           this.SelectedPolicy = "RecordingAccessPolicy";
                                                             cmpBlkPolicyDelete.call(this);
                                                           }
                                                         });

  var cmpBtnAddRecordingAccessPolicy = new CmpButton("btnAddCreateRecordingsPolicy",
                                                     null,
                                                     { action: "FNCCALL",
                                                       context: this,
                                                       fnc: function() {
                                                       dlgEditMediaStoreAdmin_Create("DEFAULT_MEDIASTOREADMIN_NID", "RecordingAccessPolicy");
                                                       }
                                                     });

  topic_Set(tApp, "RecordingAccessPolicyWhereXPath", "");
  var cmpMtxRecordingAccessPolicyList = new CmpCustomPaginationList(["blkRecordingAccessPolicyList","mtxpolicyCreateRecordingsList"],
                                                                [{action:"APPEVENT", eventvar:"RecordingAccessPolicyData", eventval:null}],
                                                                {persist:{col:"jsxid",initval:false},
                                                                 action:"APPEVENT", eventvar:"RecordingAccessPolicySelected", eventval:"jsxid"},
                                                                null,
                                                                null
                                                               );

   cmpMtxRecordingAccessPolicyList.allowMultiSelection = true;

  cmpMtxRecordingAccessPolicyList.SortPathArr = {index:"$b//order",
                                                 jsxtext:"upper-case($b//rulename)",
                                                 jsxCreateRecordingsPolicyType:"$b//allow"
                                               };

  cmpMtxRecordingAccessPolicyList.cmpReqParamArr = {StartFrom: "Op_Select_ListSpec_StartFrom",
                                                    CountToFetch: "Op_Select_ListSpec_CountToFetch",
                                                    SortOrder: "Op_Select_ListSpec_SortOrder",
                                                    ObjectCount: "ObjectCount",
                                                    SortByXpath: "Op_Select_ListSpec_SortByXPathList_SortByXpath"
                                                   };

                                                                                             
  cmpMtxRecordingAccessPolicyList.DataUpdatedEvent = { action: "TOPICEVENT", topic: tApp, eventvar: "RecordingAccessDataUpdated", eventval: "LOADED" };

  cmpMtxRecordingAccessPolicyList.cmpReq = new cmpReq_GetListReadQuery(tApp, "JSXAPPS/IPVS_AdminUI/query/RecordingAccessPolicyRule.xml",
                                                                  [{ VarName: "_WHEREXPATH_", VarRef: "RecordingAccessPolicyWhereXPath"}
                                                                  ],
                                                                  { action: "FNCCALL", context: cmpMtxRecordingAccessPolicyList,
                                                                      fnc: function(respCode, respVal) {
                                                                          if (respCode == "SUCCESS" && respVal != null) {
                                                                              this.onResponse(respCode, respVal);
                                                                          }
                                                                          else {
                                                                              log.info("GetRecordingAccessRequest failed. Error Code :" + respCode);
                                                                              this.onResponseErrorHandle("GetRecordingAccessRequest",respCode, respVal);
                                                                          }
                                                                      }
                                                                  }
                                                                ); 
                                                                
    cmpMtxRecordingAccessPolicyList.cmpItemByNIDReqFnc = function () {
                                                         var strWhereNID = cmpMtxRecordingAccessPolicyList.WhereNIDArr.join("\",\"");
                                                         topic_Set(tApp, "WhereGroupNID", "[//@NID=(\"" + strWhereNID + "\")]");
                                                         return new cmpReq_GetListReadQuery(tApp, "JSXAPPS/IPVS_AdminUI/query/RecordingAccessPolicyRule.xml",
                                                                                            [{VarName:"_WHEREXPATH_", VarRef:"WhereGroupNID"}],
                                                                                            {action: "FNCCALL", context: cmpMtxRecordingAccessPolicyList,
                                                                                             fnc: function(respCode, respVal) {
                                                                                                    if (respCode == "SUCCESS" && respVal != null) {
                                                                                                      this.onResponseUpdateRecord(respCode, respVal);
                                                                                                    }
                                                                                                    else{
                                                                                                      log.info("GetRecordingAccessRequest failed. Error Code :" + respCode);
                                                                                                      this.onResponseErrorHandle("GetRecordingAccessRequest",respCode, respVal);
                                                                                                    }
                                                                                                  }
                                                                                            }, "ItemByNID"
                                                                                           );
                                                       }
                                                                
  cmpMtxRecordingAccessPolicyList.MenuEvent = {action:"APPEVENT", eventvar:"PolicyMenuSelected", eventval:"jsxid"};

  var cmpRecordingAccessNavControls = new CmpPageNavButtonsContainer(["blkRecordingAccessPolicyList","blkRecordingAccessFooter"],
                                                                    [["blkRecordingAccessPolicyList", "btnPageFirst"],
                                                                     ["blkRecordingAccessPolicyList", "btnPagePrev"],
                                                                     ["blkRecordingAccessPolicyList", "btnPageNext"],
                                                                     ["blkRecordingAccessPolicyList", "btnPageLast"],
                                                                     ["blkRecordingAccessPolicyList", "lblRecordInfo"],
                                                                     ["blkRecordingAccessPolicyList", "txtCurrentRecordNumber"],
                                                                     ["blkRecordingAccessPolicyList", "btnPageRefresh"],
                                                                     ["blkRecordingAccessPolicyList", "btnPageRefreshSpinner"],
                                                                     ["blkRecordingAccessPolicyList", "btnPageRefreshNotify"]
                                                                    ],
                                                                    cmpMtxRecordingAccessPolicyList,
                                                                    [{action:"TOPICEVENT", topic:tApp, eventvar:"RecordingAccessDataUpdated", eventval:null}],
                                                                    null
                                                                   ); 

  var cmpMenuRecordingsAccessPolicyList = new CmpMenu("menuRecordingsAccessPolicyList",
                                                      [{ObjectType:"RECORDINGSACCESS",
                                                        MenuListArr:["Edit..."] }
                                                      ],
                                                      null, null, null,
                                                      [{action:"APPEVENT", eventvar:"PolicyMenuTypeSelected", eventval:null}],
                                                      {action:"APPEVENT", eventvar:"PolicyMenuActionSelected", eventval:"jsxtext"}
                                                    );

  var cmpBlkRecordingsAccessPolicyList = new CmpContainer("blkRecordingAccessPolicyList",
                                                          [{ cmp: cmpMtxRecordingAccessPolicyList },
                                                           { cmp: cmpBtnAddRecordingAccessPolicy },
                                                           { cmp: cmpMenuRecordingsAccessPolicyList },
                                                           { cmp: cmpBtnDeleteRecordingAccessPolicy },
                                                           { cmp: cmpRecordingAccessNavControls}
                                                          ],
                                                          null); 

  /************************** RECORDINGSACCESS Block END ^ ******************/

  /************************** SESSIONACCESS Block START ^ ******************/

  var cmpBtnDeleteSessionAccessPolicy = new CmpButton("btnDeleteSessionAccessPolicy",
                                                     [{ action: "APPEVENT", eventvar: "SessionAccessPolicySelected", eventval: null}],
                                                     { action: "FNCCALL",
                                                       context: this,
                                                       fnc: function() {
                                                         this.SelectedPolicy = "SessionAccessPolicy";
                                                         cmpBlkPolicyDelete.call(this);
                                                       }
                                                     });

  var cmpBtnAddSessionAccessPolicy = new CmpButton("btnAddSessionAccessPolicy",
                                                   null,
                                                   { action: "FNCCALL",
                                                     context: this,
                                                     fnc: function() {
                                                       dlgEditAssetAdmin_Create("DEFAULT_ASSETADMIN_NID", "SessionAccessPolicy");
                                                     }
                                                   });

  topic_Set(tApp, "SessionAccessPolicyWhereXPath", "");
  var cmpMtxSessionAccessPolicyList = new CmpCustomPaginationList(["blkSessionPolicyList","mtxpolicySessionAccessList"],
                                                                [{action:"APPEVENT", eventvar:"SessionAccessPolicyData", eventval:null}],
                                                                {persist:{col:"jsxid",initval:false},
                                                                 action:"APPEVENT", eventvar:"SessionAccessPolicySelected", eventval:"jsxid"},
                                                                null,
                                                                null
                                                               );

   cmpMtxSessionAccessPolicyList.allowMultiSelection = true;

  cmpMtxSessionAccessPolicyList.SortPathArr = {index:"$b//order",
                                               jsxtext:"upper-case($b//rulename)",
                                               jsxSessionAccessPolicyType:"$b//allow"
                                             };

  cmpMtxSessionAccessPolicyList.cmpReqParamArr = {StartFrom: "Op_Select_ListSpec_StartFrom",
                                                  CountToFetch: "Op_Select_ListSpec_CountToFetch",
                                                  SortOrder: "Op_Select_ListSpec_SortOrder",
                                                  ObjectCount: "ObjectCount",
                                                  SortByXpath: "Op_Select_ListSpec_SortByXPathList_SortByXpath"
                                                 };

                                                                                             
  cmpMtxSessionAccessPolicyList.DataUpdatedEvent = { action: "TOPICEVENT", topic: tApp, eventvar: "SessionAccessPolicyDataUpdated", eventval: "LOADED" };

  cmpMtxSessionAccessPolicyList.cmpReq = new cmpReq_GetListReadQuery(tApp, "JSXAPPS/IPVS_AdminUI/query/SessionAccessPolicyRule.xml",
                                                                  [{ VarName: "_WHEREXPATH_", VarRef: "SessionAccessPolicyWhereXPath"}
                                                                  ],
                                                                  { action: "FNCCALL", context: cmpMtxSessionAccessPolicyList,
                                                                      fnc: function(respCode, respVal) {
                                                                          if (respCode == "SUCCESS" && respVal != null) {
                                                                              this.onResponse(respCode, respVal);
                                                                          }
                                                                          else {
                                                                              log.info("GetSessionAccessPolicyRequest failed. Error Code :" + respCode);
                                                                              this.onResponseErrorHandle("GetSessionAccessPolicyRequest",respCode, respVal);
                                                                          }
                                                                      }
                                                                  }
                                                                ); 
                                                                
    cmpMtxSessionAccessPolicyList.cmpItemByNIDReqFnc = function () {
                                                       var strWhereNID = cmpMtxSessionAccessPolicyList.WhereNIDArr.join("\",\"");
                                                       topic_Set(tApp, "WhereGroupNID", "[//@NID=(\"" + strWhereNID + "\")]");
                                                       return new cmpReq_GetListReadQuery(tApp, "JSXAPPS/IPVS_AdminUI/query/SessionAccessPolicyRule.xml",
                                                                                          [{VarName:"_WHEREXPATH_", VarRef:"WhereGroupNID"}],
                                                                                          {action: "FNCCALL", context: cmpMtxSessionAccessPolicyList,
                                                                                           fnc: function(respCode, respVal) {
                                                                                                  if (respCode == "SUCCESS" && respVal != null) {
                                                                                                    this.onResponseUpdateRecord(respCode, respVal);
                                                                                                  }
                                                                                                  else{
                                                                                                    log.info("GetSessionAccessPolicyRequest failed. Error Code :" + respCode);
                                                                                                    this.onResponseErrorHandle("GetSessionAccessPolicyRequest",respCode, respVal);
                                                                                                  }
                                                                                                }
                                                                                          }, "ItemByNID"
                                                                                         );
                                                     }
                                                                
  cmpMtxSessionAccessPolicyList.MenuEvent = {action:"APPEVENT", eventvar:"PolicyMenuSelected", eventval:"jsxid"};

  var cmpSessionAccessNavControls = new CmpPageNavButtonsContainer(["blkSessionPolicyList","blkSessionAccessFooter"],
                                                                  [["blkSessionPolicyList", "btnPageFirst"],
                                                                   ["blkSessionPolicyList", "btnPagePrev"],
                                                                   ["blkSessionPolicyList", "btnPageNext"],
                                                                   ["blkSessionPolicyList", "btnPageLast"],
                                                                   ["blkSessionPolicyList", "lblRecordInfo"],
                                                                   ["blkSessionPolicyList", "txtCurrentRecordNumber"],
                                                                   ["blkSessionPolicyList", "btnPageRefresh"],
                                                                   ["blkSessionPolicyList", "btnPageRefreshSpinner"],
                                                                   ["blkSessionPolicyList", "btnPageRefreshNotify"]
                                                                  ],
                                                                  cmpMtxSessionAccessPolicyList,
                                                                  [{action:"TOPICEVENT", topic:tApp, eventvar:"SessionAccessPolicyDataUpdated", eventval:null}],
                                                                  null
                                                                 );
    
  var cmpMenuSessionAccessPolicyList = new CmpMenu("menuSessionAccessPolicyList",
                                                  [{ObjectType:"CLIENTSESSIONACCESS",
                                                    MenuListArr:["Edit..."] }
                                                  ],
                                                  null, null, null,
                                                  [{action:"APPEVENT", eventvar:"PolicyMenuTypeSelected", eventval:null}],
                                                  {action:"APPEVENT", eventvar:"PolicyMenuActionSelected", eventval:"jsxtext"}
                                                );

  var cmpBlkSessionAccessPolicyList = new CmpContainer("blkSessionPolicyList",
                                                        [{ cmp: cmpMtxSessionAccessPolicyList },
                                                         { cmp: cmpBtnAddSessionAccessPolicy },
                                                         { cmp: cmpMenuSessionAccessPolicyList },
                                                         { cmp: cmpBtnDeleteSessionAccessPolicy },
                                                         { cmp: cmpSessionAccessNavControls}
                                                        ],
                                                        null);
  /************************** SESSIONACCESS Block END ^ ******************/

  var cmpBlkPolicyUI = new CmpContainer( "blkPolicyUI",
                                        [ {cmp:cmpBlkGroupTagList,
                                           sel:"topic_Get(tApp, 'PolicyMenuSelection') == null || topic_Get(tApp, 'PolicyMenuSelection') == 'GROUPTAG'"},
                                          {cmp:cmpBlkTagObjectFilter,
                                           sel:"topic_Get(tApp, 'PolicyMenuSelection') == 'TAGOBJECTFILTER'"},
                                          {cmp:cmpBlkObjectFilter,
                                           sel: "topic_Get(tApp, 'PolicyMenuSelection') == 'OBJECTFILTER'"},
                                          {cmp: cmpBlkPortAccessPolicyList,
                                           sel: "topic_Get(tApp, 'PolicyMenuSelection') == 'PORTACCESS'"},
                                          {cmp: cmpBlkMediaAccessPolicyList,
                                           sel: "topic_Get(tApp, 'PolicyMenuSelection') == 'MEDIACCESS'"},
                                          {cmp: cmpBlkRecordingsAccessPolicyList,
                                           sel: "topic_Get(tApp, 'PolicyMenuSelection') == 'RECORDINGSACCESS'"},
                                          {cmp: cmpBlkSessionAccessPolicyList,
                                           sel: "topic_Get(tApp, 'PolicyMenuSelection') == 'CLIENTSESSIONACCESS'"},
                                          {cmp:cmpBlkMediaStreamFilterList,
                                           sel:"topic_Get(tApp, 'PolicyMenuSelection') == 'STREAMPOLICY'"},
                                          {cmp:cmpBlkUserAdminList,
                                           sel:"topic_Get(tApp, 'PolicyMenuSelection') == 'USERADMIN'"},
                                          {cmp:cmpBlkAssetAdminList,
                                           sel:"topic_Get(tApp, 'PolicyMenuSelection') == 'ASSETADMIN'"},
                                          {cmp:cmpBlkCollabList,
                                           sel: "topic_Get(tApp, 'PolicyMenuSelection') == 'COLLABPOLICY'"},
                                          {cmp:cmpBlkMediaStoreAdminList,
                                           sel: "topic_Get(tApp, 'PolicyMenuSelection') == 'MEDIASTORE'"},
                                          { cmp: cmpBlkAssetAdminSessionList,
                                           sel: "topic_Get(tApp, 'PolicyMenuSelection') == 'ASSETADMINSESSION'"},
                                          {cmp:cmpBlkNATList,
                                           sel:"topic_Get(tApp, 'PolicyMenuSelection') == 'NATPOLICY'"},
                                          {cmp:cmpBlkPolicyAdminPolicyList,
                                           sel:"topic_Get(tApp, 'PolicyMenuSelection') == 'POLICYADMINPOLICY'"}
                                        ],
                                        [{action:"APPEVENT", eventvar:"PolicyMenuSelection", eventval:null}] );

  //Tags and Filters
  var cmpLblGroupTag = new CmpLabelHighlight(["blkPolicy","leftNav","lblGroupTags"], null, 
                                             [{action:"APPEVENT", eventvar:"PolicyMenuSelection", eventval:null}], 
                                             {action:"APPEVENT", eventvar:"PolicyMenuSelection", eventval:"GROUPTAG"} );
  var cmpLblTagObjectFilter = new CmpLabelHighlight(["blkPolicy","leftNav","lblFilters"], null, 
                                                    [{action:"APPEVENT", eventvar:"PolicyMenuSelection", eventval:null}], 
                                                    {action:"APPEVENT", eventvar:"PolicyMenuSelection", eventval:"TAGOBJECTFILTER"});
  var cmpLblObjectFilter = new CmpLabelHighlight(["blkPolicy","leftNav","lblObjectFilters"], null, 
                                                 [{action:"APPEVENT", eventvar:"PolicyMenuSelection", eventval:null}], 
                                                 {action:"APPEVENT", eventvar:"PolicyMenuSelection", eventval:"OBJECTFILTER"});
  //User Permissions
  var cmpLblPortAccessPolicy = new CmpLabelHighlight(["blkPolicy", "leftNav", "lblPortAccessPolicy"], null,
                                                     [{ action: "APPEVENT", eventvar: "PolicyMenuSelection", eventval: null}],
                                                     { action: "APPEVENT", eventvar: "PolicyMenuSelection", eventval: "PORTACCESS" });
  var cmpLblMediaAccessPolicy = new CmpLabelHighlight(["blkPolicy", "leftNav", "lblMediaAccessPolicy"], null,
                                                      [{ action: "APPEVENT", eventvar: "PolicyMenuSelection", eventval: null}],
                                                      { action: "APPEVENT", eventvar: "PolicyMenuSelection", eventval: "MEDIACCESS" });
  var cmpLblMediaRecordingsPolicy = new CmpLabelHighlight(["blkPolicy", "leftNav", "lblMediaRecordingsPolicy"], null,
                                                         [{ action: "APPEVENT", eventvar: "PolicyMenuSelection", eventval: null}],
                                                         { action: "APPEVENT", eventvar: "PolicyMenuSelection", eventval: "RECORDINGSACCESS" });
  var cmpLblClientSessionPolicy = new CmpLabelHighlight(["blkPolicy", "leftNav", "lblClientSessionPolicy"], null,
                                                         [{ action: "APPEVENT", eventvar: "PolicyMenuSelection", eventval: null}],
                                                         { action: "APPEVENT", eventvar: "PolicyMenuSelection", eventval: "CLIENTSESSIONACCESS" }); 
  //Administartor Permissions
  var cmpLblUserManagement = new CmpLabelHighlight(["blkPolicy","leftNav","lblUserAdminPolicy"], null, 
                                                   [{action:"APPEVENT", eventvar:"PolicyMenuSelection", eventval:null}],
                                                   { action: "APPEVENT", eventvar: "PolicyMenuSelection", eventval: "USERADMIN" });                                
  var cmpLblAssetManagement = new CmpLabelHighlight(["blkPolicy","leftNav","lblAssetAdminPolicy"], null, 
                                                    [{action:"APPEVENT", eventvar:"PolicyMenuSelection", eventval:null}],
                                                    { action: "APPEVENT", eventvar: "PolicyMenuSelection", eventval: "ASSETADMIN" });                                              
  var cmpLblMediaStore = new CmpLabelHighlight(["blkPolicy","leftNav","lblMediaStorePolicy"], null, 
                                               [{action:"APPEVENT", eventvar:"PolicyMenuSelection", eventval:null}],
                                               { action: "APPEVENT", eventvar: "PolicyMenuSelection", eventval: "MEDIASTORE" });
  var cmpLblPolicyAdminPolicy = new CmpLabelHighlight(["blkPolicy", "leftNav", "lblPolicyAdminPolicy"], null,
                                                       [{ action: "APPEVENT", eventvar: "PolicyMenuSelection", eventval: null}],
                                                       { action: "APPEVENT", eventvar: "PolicyMenuSelection", eventval: "POLICYADMINPOLICY" });
  var cmpLblAssetAdminSession = new CmpLabelHighlight(["blkPolicy", "leftNav", "lblAssestAdminSessionPolicy"], null,
                                                       [{ action: "APPEVENT", eventvar: "PolicyMenuSelection", eventval: null}],
                                                       { action: "APPEVENT", eventvar: "PolicyMenuSelection", eventval: "ASSETADMINSESSION" });
  //Routing and Connections                                               
  var cmpLblNAT = new CmpLabelHighlight(["blkPolicy","leftNav","lblNATPolicy"], null, 
                                       [{action:"APPEVENT", eventvar:"PolicyMenuSelection", eventval:null}], 
                                       {action:"APPEVENT", eventvar:"PolicyMenuSelection", eventval:"NATPOLICY"});
  var cmpLblMediaStreamFilter = new CmpLabelHighlight(["blkPolicy", "leftNav", "lblStreamFilter"], null,
                                                    [{ action: "APPEVENT", eventvar: "PolicyMenuSelection", eventval: null}],
                                                    { action: "APPEVENT", eventvar: "PolicyMenuSelection", eventval: "STREAMPOLICY" });
  var cmpLblCollaboration = new CmpLabelHighlight(["blkPolicy", "leftNav", "lblMediaRoomPolicy"], null,
                                                  [{ action: "APPEVENT", eventvar: "PolicyMenuSelection", eventval: null}],
                                                  { action: "APPEVENT", eventvar: "PolicyMenuSelection", eventval: "COLLABPOLICY" });



  var cmpBlkPolicy = new CmpContainer( "blkPolicy",
                        [ //Tags and Filters
                          {cmp:cmpLblGroupTag},
                          {cmp:cmpLblTagObjectFilter},
	                     // {cmp:cmpLblObjectFilter}, // not in use
                          //User Permissions
                          {cmp:cmpLblPortAccessPolicy },
                          {cmp:cmpLblMediaAccessPolicy },
                          {cmp:cmpLblMediaRecordingsPolicy },
                          { cmp:cmpLblClientSessionPolicy },
                          //Administartor Permissions
                          {cmp:cmpLblUserManagement},
                          {cmp:cmpLblAssetManagement },
                          {cmp:cmpLblMediaStore },
                          {cmp:cmpLblPolicyAdminPolicy },
                      //  {cmp:cmpLblAssetAdminSession }, // not in use
                          //Routing and Connections
                          {cmp:cmpLblNAT },
                          {cmp:cmpLblMediaStreamFilter },
                          {cmp: cmpLblCollaboration },
                          // Right hand side block to dipalu the list
                          {cmp:cmpBlkPolicyUI}						  
                        ],
                        null );
 
  //setting the defa value for the selection. Removed from cmlblkUI.                    
  //topic_Set(tApp, "PolicyMenuSelection", "GROUPTAG");

  eventApp.subscribe("MENUUIDATA", "POLICY", this,
                      function(EventVar, EventVal) {
                        if( EventVal == null || EventVal == "POLICY" ) {
                          setObjectCacheDefaultXML("GroupTagDef", "JSXAPPS/IPVS-Common/model/objectXml/GroupTagDef.xml", "DEFAULT_GROUPTAG_NID"); 
                          topic_Set(tApp, "GroupTagEditDataState", "DEFAULT_GROUPTAG_NID");

                          setObjectCacheDefaultXML("TagObjectFilter", "JSXAPPS/IPVS-Common/model/objectXml/TagObjectFilter.xml", "DEFAULT_TAGOBJECTFILTER_NID");
                          topic_Set(tApp, "TagObjectFilterEditDataState", "DEFAULT_TAGOBJECTFILTER_NID");

			                    //setObjectCacheDefaultXML("ObjectFilter", "JSXAPPS/IPVS-Common/model/objectXml/ObjectFilter.xml", "DEFAULT_OBJECTFILTER_NID");
                          //topic_Set(tApp, "ObjectFilterEditDataState", "DEFAULT_OBJECTFILTER_NID");
                        
                          setObjectCacheDefaultXML("MediaStreamFilter", "JSXAPPS/IPVS-Common/model/objectXml/MediaStreamFilter.xml", "DEFAULT_MEDIASTREAMFILTER_NID");
                          topic_Set(tApp, "MediaStreamFilterEditDataState", "DEFAULT_MEDIASTREAMFILTER_NID");

                          setObjectCacheDefaultXML("UserAdminPolicyRule", "JSXAPPS/IPVS-Common/model/objectXml/UserAdminPolicyRule.xml", "DEFAULT_USERADMIN_NID");
                          topic_Set(tApp, "UserAdminEditDataState", "DEFAULT_USERADMIN_NID");

                          setObjectCacheDefaultXML("AssetAdminPolicyRule", "JSXAPPS/IPVS-Common/model/objectXml/AssetAdminPolicyRule.xml", "DEFAULT_ASSETADMIN_NID");
                          topic_Set(tApp, "AssetAdminEditDataState", "DEFAULT_ASSETADMIN_NID");
                  
                          setObjectCacheDefaultXML("MediaRoomPolicyRule", "JSXAPPS/IPVS-Common/model/objectXml/MediaRoomPolicyRule.xml", "DEFAULT_COLLAB_NID"); 
                          topic_Set(tApp, "CollabEditDataState", "DEFAULT_COLLAB_NID"); 
                        
                          setObjectCacheDefaultXML("MediaStoreAdminPolicyRule", "JSXAPPS/IPVS-Common/model/objectXml/MediaStoreAdminPolicyRule.xml", "DEFAULT_MEDIASTOREADMIN_NID"); 
                          topic_Set(tApp, "MediaStoreAdminEditDataState", "DEFAULT_MEDIASTOREADMIN_NID");

                          setObjectCacheDefaultXML("PolicyAdminPolicyRule", "JSXAPPS/IPVS-Common/model/objectXml/PolicyAdminPolicyRule.xml", "DEFAULT_POLICYADMINPOLICY_NID"); 
                          topic_Set(tApp, "MediaStoreAdminEditDataState", "DEFAULT_POLICYADMINPOLICY_NID");                        

                          setObjectCacheDefaultXML("NetworkAddressRule", "JSXAPPS/IPVS-Common/model/objectXml/NetworkAddressRule.xml", "DEFAULT_NAT_NID"); 
                          topic_Set(tApp, "EditNetworkAddressDataState", "DEFAULT_NAT_NID"); 
                                                  
                          var getDataState = topic_Get(tApp, "GroupTagPolicyDataState");
                          if(getDataState == "NONE") {
                            topic_Publish(tApp, "PolicyMenuSelection","GROUPTAG");
                          }
                        }
                      }
                    );
                    
    eventApp.subscribe("PolicyMenuSelection", null, this,
                     function(EventVar, EventVal) {
                       if( jsx3.util.strEmpty(EventVal) ) return;
                       if( EventVal == "GROUPTAG" ) {
                         var getDataState = topic_Get(tApp, "GroupTagPolicyDataState");
                         if(getDataState == "NONE") {
                           topic_Set(tApp, "GroupTagPolicyDataState", (newServerDate()).getTime() );
                           cmpMtxGroupTagList.ResetToStart(1,CountToFetch,0);
                           topic_Publish(tApp, "GroupTagPolicyData", (newServerDate()).getTime() );
                         }
                       }
                       else if( EventVal == "TAGOBJECTFILTER" ) {
                         var getDataState = topic_Get(tApp, "TagObjectFilterPolicyDataState");
                         if(getDataState == "NONE") {
                           topic_Set(tApp, "TagObjectFilterPolicyDataState", (newServerDate()).getTime() );
                           cmpMtxTagObjectFilterList.ResetToStart(1,CountToFetch,0);
                           topic_Publish(tApp, "TagObjectFilterPolicyData", (newServerDate()).getTime() );
                         }
                       }
                       else if( EventVal == "PORTACCESS" ) {
                         var getDataState = topic_Get(tApp, "PortAccessPolicyDataState");
                         if(getDataState == "NONE") {
                           topic_Set(tApp, "PortAccessPolicyDataState", (newServerDate()).getTime() );
                           cmpMtxPortAccessPolicyList.ResetToStart(1,CountToFetch,0);
                           topic_Publish(tApp, "PortAccessPolicyData", (newServerDate()).getTime() );
                         }
                       }
                       else if( EventVal == "MEDIACCESS"){
                         var getDataState = topic_Get(tApp, "MediaAccessPolicyDataState");
                         if(getDataState == "NONE") {
                           topic_Set(tApp, "MediaAccessPolicyDataState", (newServerDate()).getTime() );
                           cmpMtxMediaAccessPolicyList.ResetToStart(1,CountToFetch,0);
                           topic_Publish(tApp, "MediaAccessPolicyData", (newServerDate()).getTime() );
                         }
                       }
                       else if(EventVal == "RECORDINGSACCESS"){
                         var getDataState = topic_Get(tApp, "RecordingAccessPolicyDataState");
                         if(getDataState == "NONE") {
                           topic_Set(tApp, "RecordingAccessPolicyDataState", (newServerDate()).getTime() );
                           cmpMtxRecordingAccessPolicyList.ResetToStart(1,CountToFetch,0);
                           topic_Publish(tApp, "RecordingAccessPolicyData", (newServerDate()).getTime() );
                         }
                       }
                       else if(EventVal == "CLIENTSESSIONACCESS"){
                         var getDataState = topic_Get(tApp, "SessionAccessPolicyDataState");
                         if(getDataState == "NONE") {
                           topic_Set(tApp, "SessionAccessPolicyDataState", (newServerDate()).getTime() );
                           cmpMtxSessionAccessPolicyList.ResetToStart(1,CountToFetch,0);
                           topic_Publish(tApp, "SessionAccessPolicyData", (newServerDate()).getTime() );
                         }
                       }
                       else if(EventVal == "USERADMIN"){
                         var getDataState = topic_Get(tApp, "UserAccessPolicyDataState");
                         if(getDataState == "NONE") {
                           topic_Set(tApp, "UserAccessPolicyDataState", (newServerDate()).getTime() );
                           cmpMtxUserAdminList.ResetToStart(1,CountToFetch,0);
                           topic_Publish(tApp, "UserAccessPolicyData", (newServerDate()).getTime() );
                         }
                       }
                       else if(EventVal == "ASSETADMIN"){
                         var getDataState = topic_Get(tApp, "AssetAccessPolicyDataState");
                         if(getDataState == "NONE") {
                           topic_Set(tApp, "AssetAccessPolicyDataState", (newServerDate()).getTime() );
                           cmpMtxAssetAdminList.ResetToStart(1,CountToFetch,0);
                           topic_Publish(tApp, "AssetAccessPolicyData", (newServerDate()).getTime() );
                         }
                       }
                       else if(EventVal == "MEDIASTORE"){
                         var getDataState = topic_Get(tApp, "MediaStoreAccessPolicyDataState");
                         if(getDataState == "NONE") {
                           topic_Set(tApp, "MediaStoreAccessPolicyDataState", (newServerDate()).getTime() );
                           cmpMtxMediaStoreAdminList.ResetToStart(1,CountToFetch,0);
                           topic_Publish(tApp, "MediaStoreAccessPolicyData", (newServerDate()).getTime() );
                         }
                       }
                       else if(EventVal == "POLICYADMINPOLICY"){
                         var getDataState = topic_Get(tApp, "PolicyAccessPolicyDataState");
                         if(getDataState == "NONE") {
                           topic_Set(tApp, "PolicyAccessPolicyDataState", (newServerDate()).getTime() );
                           cmpMtxPolicyAdminPolicyList.ResetToStart(1,CountToFetch,0);
                           topic_Publish(tApp, "PolicyAccessPolicyData", (newServerDate()).getTime() );
                         }
                       }
                       else if(EventVal == "NATPOLICY"){
                         var getDataState = topic_Get(tApp, "NATAccessPolicyDataState");
                         if(getDataState == "NONE") {
                           topic_Set(tApp, "NATAccessPolicyDataState", (newServerDate()).getTime() );
                           cmpMtxNATList.ResetToStart(1,CountToFetch,0);
                           topic_Publish(tApp, "NATAccessPolicyData", (newServerDate()).getTime() );
                         }
                       }
                       else if(EventVal == "STREAMPOLICY"){
                         var getDataState = topic_Get(tApp, "MediaStreamFilterPolicyDataState");
                         if(getDataState == "NONE") {
                           topic_Set(tApp, "MediaStreamFilterPolicyDataState", (newServerDate()).getTime() );
                           cmpMtxMediaStreamFilterList.ResetToStart(1,CountToFetch,0);
                           topic_Publish(tApp, "MediaStreamFilterPolicyData", (newServerDate()).getTime() );
                         }
                       }
                       else if(EventVal == "COLLABPOLICY"){
                         var getDataState = topic_Get(tApp, "CollabAccessPolicyDataState");
                         if(getDataState == "NONE") {
                           topic_Set(tApp, "CollabAccessPolicyDataState", (newServerDate()).getTime() );
                           cmpMtxCollabList.ResetToStart(1,CountToFetch,0);
                           topic_Publish(tApp, "CollabAccessPolicyData", (newServerDate()).getTime() );
                         }
                       }
                     });

    eventApp.subscribe("PolicyMenuSelected", null, this,
                        function() {
                          if (clsUserPermission.getPermission('Policy', 'modify') == "false") {
                            return;
                          }                      
                          var policyMenuType = topic_Get(tApp, 'PolicyMenuSelection');
                          if(policyMenuType == "TAGOBJECTFILTER"){
                            this.SelectedPolicyNID = topic_Get(tApp, "TagObjectFilterSelected"); 
                            var selRecord = cmpMtxTagObjectFilterList.BlkJSX.getRecord(this.SelectedPolicyNID);
                            var SelectedTagObjectFilterName = selRecord.jsxtext;
                            if(SelectedTagObjectFilterName == "_ALL_") {
                              policyMenuType = "_ALL_";
                            }
                          }
                          topic_Publish(tApp, "PolicyMenuTypeSelected", policyMenuType);
                        }
                      );
    
    eventApp.subscribe("PolicyMenuActionSelected", null, this,
                        function() {
                          var policyMenuActionSelected = topic_Get(tApp, 'PolicyMenuActionSelected');
                          var policyType = topic_Get(tApp, "PolicyMenuTypeSelected");
                          if(policyType == "TAGOBJECTFILTER"){
                            this.SelectedPolicyNID = topic_Get(tApp, "TagObjectFilterSelected"); 
                            var selRecord = cmpMtxTagObjectFilterList.BlkJSX.getRecord(this.SelectedPolicyNID);
                            if(selRecord == null) {
                              alert("Please select filter policy");
                              return false;                        
                            }
                            else if( policyMenuActionSelected == "Edit..." ) {
                              var SelectedTagObjectFilterName = selRecord.jsxtext;
                              if(SelectedTagObjectFilterName == "_ALL_") {
                                alert("Cannot edit filter: _ALL_ ");
                                return false;
                              } else {                                           
                                dlgEditTagObjectFilter_Create(this.SelectedPolicyNID);
                              }
                            }
                          }
                          else if(policyType == "PORTACCESS"){
                            this.SelectedPolicyNID = topic_Get(tApp, "PortAccessPolicySelected");
                            var selRecord = cmpMtxPortAccessPolicyList.BlkJSX.getRecord(this.SelectedPolicyNID);
                            if(selRecord == null) {
                              alert("Please select source and destination ports policy");
                              return false;                        
                            }
                            else if( policyMenuActionSelected == "Edit..." ) {
                              dlgEditAssetAdmin_Create(this.SelectedPolicyNID, "PortAccessPolicy");
                            }
                          }
                          else if(policyType == "MEDIACCESS"){
                            this.SelectedPolicyNID = topic_Get(tApp, "MediaAccessPolicySelected");
                            var selRecord = cmpMtxMediaAccessPolicyList.BlkJSX.getRecord(this.SelectedPolicyNID);
                            if(selRecord == null) {
                              alert("Please select playback files policy");
                              return false;                        
                            }
                            else if( policyMenuActionSelected == "Edit..." ) {
                              dlgEditMediaStoreAdmin_Create(this.SelectedPolicyNID, "MediaAccessPolicy");
                            }
                          }
                          else if(policyType == "RECORDINGSACCESS"){
                            this.SelectedPolicyNID = topic_Get(tApp, "RecordingAccessPolicySelected");
                            var selRecord = cmpMtxRecordingAccessPolicyList.BlkJSX.getRecord(this.SelectedPolicyNID);
                            if(selRecord == null) {
                              alert("Please select Record to Directories policy");
                              return false;                        
                            }
                            else if( policyMenuActionSelected == "Edit..." ) {
                              dlgEditMediaStoreAdmin_Create(this.SelectedPolicyNID, "RecordingAccessPolicy");
                            }
                          }
                          else if(policyType == "CLIENTSESSIONACCESS"){
                            this.SelectedPolicyNID = topic_Get(tApp, "SessionAccessPolicySelected");
                            var selRecord = cmpMtxSessionAccessPolicyList.BlkJSX.getRecord(this.SelectedPolicyNID);
                            if(selRecord == null) {
                              alert("Please select Configure Sessions policy");
                              return false;                        
                            }
                            else if( policyMenuActionSelected == "Edit..." ) {
                              dlgEditAssetAdmin_Create(this.SelectedPolicyNID, "SessionAccessPolicy");
                            }
                          }
                          else if(policyType == "USERADMIN"){
                            this.SelectedPolicyNID = topic_Get(tApp, "UserAdminSelected");
                            var selRecord = cmpMtxUserAdminList.BlkJSX.getRecord(this.SelectedPolicyNID);
                            if(selRecord == null) {
                              alert("Please select Manage Users policy");
                              return false;                        
                            }
                            else if( policyMenuActionSelected == "Edit..." ) {
                              dlgEditUserAdmin_Create(this.SelectedPolicyNID);
                            }
                          }
                          else if(policyType == "ASSETADMIN"){
                            this.SelectedPolicyNID = topic_Get(tApp, "AssetAdminSelected");
                            var selRecord = cmpMtxAssetAdminList.BlkJSX.getRecord(this.SelectedPolicyNID);
                            if(selRecord == null) {
                              alert("Please select Manage Devices and Profiles policy");
                              return false;                        
                            }
                            else if( policyMenuActionSelected == "Edit..." ) {
                              dlgEditAssetAdmin_Create(this.SelectedPolicyNID, "AssetAdminPolicy");
                            }
                          }
                          else if(policyType == "MEDIASTORE"){
                            this.SelectedPolicyNID = topic_Get(tApp, "MediaStoreAdminSelected");
                            var selRecord = cmpMtxMediaStoreAdminList.BlkJSX.getRecord(this.SelectedPolicyNID);
                            if(selRecord == null) {
                              alert("Please select Manage Media policy");
                              return false;                        
                            }
                            else if( policyMenuActionSelected == "Edit..." ) {
                              dlgEditMediaStoreAdmin_Create(this.SelectedPolicyNID, "MediaStoreAdminPolicy");
                            }
                          }
                          else if(policyType == "POLICYADMINPOLICY"){
                            this.SelectedPolicyNID = topic_Get(tApp, "PolicyAdminPolicySelected");
                            var selRecord = cmpMtxPolicyAdminPolicyList.BlkJSX.getRecord(this.SelectedPolicyNID);
                            if(selRecord == null) {
                              alert("Please select Manage System policy");
                              return false;                        
                            }
                            else if( policyMenuActionSelected == "Edit..." ) {
                              dlgEditPolicyAdminPolicy_Create(this.SelectedPolicyNID);
                            }
                          }
                          else if(policyType == "NATPOLICY"){
                            this.SelectedPolicyNID = topic_Get(tApp, "NATPolicySelected");
                            var selRecord = cmpMtxNATList.BlkJSX.getRecord(this.SelectedPolicyNID);
                            if(selRecord == null) {
                              alert("Please select Network Address Translation policy");
                              return false;                        
                            }
                            else if( policyMenuActionSelected == "Edit..." ) {
                              dlgEditNetworkAddress_Create(this.SelectedPolicyNID);
                            }
                          }
                          else if(policyType == "STREAMPOLICY"){
                            this.SelectedPolicyNID = topic_Get(tApp, "MediaStreamFilterSelected");
                            var selRecord = cmpMtxMediaStreamFilterList.BlkJSX.getRecord(this.SelectedPolicyNID);
                            if(selRecord == null) {
                              alert("Please select Routing policy");
                              return false;                        
                            }
                            else if( policyMenuActionSelected == "Edit..." ) {
                              dlgEditMediaStreamFilter_Create(this.SelectedPolicyNID);
                            }
                          }
                          else if(policyType == "COLLABPOLICY"){
                            this.SelectedPolicyNID = topic_Get(tApp, "CollabSelected");
                            var selRecord = cmpMtxCollabList.BlkJSX.getRecord(this.SelectedPolicyNID);
                            if(selRecord == null) {
                              alert("Please select Connection Policy policy");
                              return false;                        
                            }
                            else if( policyMenuActionSelected == "Edit..." ) {
                              dlgEditCollab_Create(this.SelectedPolicyNID);
                            }
                          }
                        }
                      );
                                                   
    eventApp.subscribe("DBUpdate", null, this,
                     function(EventVar, EventVal) {
                       if( jsx3.util.strEmpty(EventVal) ) return;
                       var policyMenuSelection = topic_Get(tApp, "PolicyMenuSelection");
                       if( EventVal.objType == "GroupTagDef" ) {
                         cmpMtxGroupTagList.handleNotification(EventVal);
                       }
                       else if( EventVal.objType == "TagObjectFilter" ) {
                         cmpMtxTagObjectFilterList.handleNotification(EventVal);
                       }
                       else if( EventVal.objType == "AssetAdminPolicyRule" ) {
                         if(policyMenuSelection == "PORTACCESS"){
                          cmpMtxPortAccessPolicyList.handleNotification(EventVal);
                         }
                         else if(policyMenuSelection == "CLIENTSESSIONACCESS"){
                          cmpMtxSessionAccessPolicyList.handleNotification(EventVal);
                         }
                         else if(policyMenuSelection == "ASSETADMIN"){
                          cmpMtxAssetAdminList.handleNotification(EventVal);
                         }
                       }
                       else if(EventVal.objType == "MediaStoreAdminPolicyRule" ) {
                         if(policyMenuSelection == "MEDIACCESS"){
                          cmpMtxMediaAccessPolicyList.handleNotification(EventVal);
                         }
                         else if(policyMenuSelection == "RECORDINGSACCESS"){
                          cmpMtxRecordingAccessPolicyList.handleNotification(EventVal);
                         }
                         else if(policyMenuSelection == "MEDIASTORE"){
                          cmpMtxMediaStoreAdminList.handleNotification(EventVal);
                         }
                       }
                       else if(EventVal.objType == "UserAdminPolicyRule"){
                         cmpMtxUserAdminList.handleNotification(EventVal);
                       }
                       else if(EventVal.objType == "PolicyAdminPolicyRule" ) {
                         cmpMtxPolicyAdminPolicyList.handleNotification(EventVal);
                       }
                       else if(EventVal.objType == "NetworkAddressRule" ) {
                         cmpMtxNATList.handleNotification(EventVal);
                       }
                       else if(EventVal.objType == "MediaStreamFilter"){
                         cmpMtxMediaStreamFilterList.handleNotification(EventVal);
                       }
                       else if(EventVal.objType == "MediaRoomPolicyRule"){
                         cmpMtxCollabList.handleNotification(EventVal);
                       }
                     });

    eventApp.subscribe("GroupTagDefDataUpdate", null, this,
                      function(EventVar, EventVal) {
                        if( EventVal == "ADD" || EventVal == "DELETE" ||  EventVal == "EDIT") {
                          cmpMtxGroupTagList.doRepaint();
                        }
                      }
                    );
    
    eventApp.subscribe("TagObjectFilterDataUpdate", null, this,
                      function(EventVar, EventVal) {
                        if( EventVal == "ADD" || EventVal == "DELETE" ||  EventVal == "EDIT") {
                          cmpMtxTagObjectFilterList.doRepaint();
                        }
                      }
                    );

    eventApp.subscribe("AssetAdminPolicyRuleDataUpdate", null, this,
                      function(EventVar, EventVal) {
                        if( jsx3.util.strEmpty(EventVal) ) return;
                        var policyMenuSelection = topic_Get(tApp, "PolicyMenuSelection");
                        if( EventVal == "ADD" || EventVal == "DELETE" ||  EventVal == "EDIT") {
                          if(policyMenuSelection == "PORTACCESS"){
                            cmpMtxPortAccessPolicyList.doRepaint();
                          }
                          else if(policyMenuSelection == "CLIENTSESSIONACCESS"){
                            cmpMtxSessionAccessPolicyList.doRepaint();
                          }
                          else if(policyMenuSelection == "ASSETADMIN"){
                            cmpMtxAssetAdminList.doRepaint();
                          }
                        }
                      }
                    );

    eventApp.subscribe("MediaStoreAdminPolicyRuleDataUpdate", null, this,
                      function(EventVar, EventVal) {
                        if( jsx3.util.strEmpty(EventVal) ) return;
                        var policyMenuSelection = topic_Get(tApp, "PolicyMenuSelection");
                        if( EventVal == "ADD" || EventVal == "DELETE" ||  EventVal == "EDIT") {
                          if(policyMenuSelection == "MEDIACCESS"){
                            cmpMtxMediaAccessPolicyList.doRepaint();
                          }
                          else if(policyMenuSelection == "RECORDINGSACCESS"){
                            cmpMtxRecordingAccessPolicyList.doRepaint();
                          }
                          else if(policyMenuSelection == "MEDIASTORE"){
                            cmpMtxMediaStoreAdminList.doRepaint();
                          }
                        }
                      }
                    );
    eventApp.subscribe("UserAdminPolicyRuleDataUpdate", null, this,
                      function(EventVar, EventVal) {
                        if( EventVal == "ADD" || EventVal == "DELETE" ||  EventVal == "EDIT") {
                          cmpMtxUserAdminList.doRepaint();
                        }
                      }
                    );

    eventApp.subscribe("PolicyAdminPolicyRuleDataUpdate", null, this,
                      function(EventVar, EventVal) {
                        if( EventVal == "ADD" || EventVal == "DELETE" ||  EventVal == "EDIT") {
                          cmpMtxPolicyAdminPolicyList.doRepaint();
                        }
                      }
                    );

    eventApp.subscribe("NetworkAddressRuleDataUpdate", null, this,
                      function(EventVar, EventVal) {
                        if( EventVal == "ADD" || EventVal == "DELETE" ||  EventVal == "EDIT") {
                          cmpMtxNATList.doRepaint();
                        }
                      }
                    );

    eventApp.subscribe("MediaStreamFilterDataUpdate", null, this,
                      function(EventVar, EventVal) {
                        if( EventVal == "ADD" || EventVal == "DELETE" ||  EventVal == "EDIT") {
                          cmpMtxMediaStreamFilterList.doRepaint();
                        }
                      }
                    );

    eventApp.subscribe("MediaRoomPolicyRuleDataUpdate", null, this,
                      function(EventVar, EventVal) {
                        if( EventVal == "ADD" || EventVal == "DELETE" ||  EventVal == "EDIT") {
                          cmpMtxCollabList.doRepaint();
                        }
                      }
                    );

  return cmpBlkPolicy;

  /********************* Asset Admin Session Functions END *********************/
  function cmpAssetAdminSession_Repaint() {
    // get currently valid data
    if (topic_Get(tApp, "AssetAdminDataState") != null) {
      var assetAdminNIDListNode = topic_GetNode(tApp, "//AssetAdminNIDList");
      if (assetAdminNIDListNode != null) {
        var assetAdminNIDListIter = assetAdminNIDListNode.selectNodeIterator(".//ObjectNID");
        while (assetAdminNIDListIter.hasNext()) {
          var assetAdminNID = assetAdminNIDListIter.next().getValue();
          var assetAdminNode = topic_GetNode(tCache, ".//*[(@NID='" + assetAdminNID + "') and ((.//AssetAdminRoleSet/AssetAdminObjectRole[RoleObject='Session'] ) and (.//AssetAdminRoleSet/AssetAdminObjectRole[RoleAction='Delete'] or .//AssetAdminRoleSet/AssetAdminObjectRole[RoleAction='Read']))]");
          if (jsx3.util.strEmpty(assetAdminNode)) continue;
          var jsxtext = assetAdminNode.selectSingleNode(".//rulename").getValue();
          var index = assetAdminNode.selectSingleNode(".//order").getValue();
          var policyValid = assetAdminNode.selectSingleNode(".//valid").getValue();
          var jsxAssetAdminSessionPolicyType = assetAdminNode.selectSingleNode(".//allow").getValue();

          if (jsxAssetAdminSessionPolicyType == "true") {
            jsxAssetAdminSessionPolicyType = "Allow";
          } else {
            jsxAssetAdminSessionPolicyType = "Deny";
          }

          if (chkForExistence(jsxtext, "NULL", "mtxpolicyAssetAdminSessionList")) {
            var o = new Object();
            o.jsxid = assetAdminNID;
            o.index = index;
            o.NID = assetAdminNID;
            o.jsxtext = jsxtext;
            if (policyValid == "true") {
              o.jsximg = "images/icons/ok16.png";
            } else {
              o.jsximg = "images/icons/off16.png";
            }
            o.jsxAssetAdminSessionPolicyType = jsxAssetAdminSessionPolicyType;

            this.BlkJSX.insertRecord(o, null, false);
          }
        }
      }
    }
  }
  /********************* Policy Delete START *********************/   
  
  function cmpBlkPolicyDelete() {
    var selectedRecords = null;
    this.deleteRequestCounter = 0;
    if (this.SelectedPolicy == "PortAccessPolicy") {
      selectedRecords = cmpMtxPortAccessPolicyList.BlkJSX.getSelectedNodes();
      this.ObjectRule = "AssetAdminPolicyRule";
      this.ObjectRuleUpdate = "AssetAdminPolicyRuleDataUpdate";
    }
    else if (this.SelectedPolicy == "SessionAccessPolicy") {
      selectedRecords = cmpMtxSessionAccessPolicyList.BlkJSX.getSelectedNodes();
      this.ObjectRule = "AssetAdminPolicyRule";
      this.ObjectRuleUpdate = "AssetAdminPolicyRuleDataUpdate";
    }
    else if (this.SelectedPolicy == "AssetAdminSessionPolicy") {
      selectedRecords = cmpMtxAssetAdminSessionList.BlkJSX.getSelectedNodes();
      this.ObjectRule = "AssetAdminPolicyRule";
      this.ObjectRuleUpdate = "AssetAdminPolicyRuleDataUpdate";
    }    
    else if (this.SelectedPolicy == "AssetAdminPolicy") {
      selectedRecords = cmpMtxAssetAdminList.BlkJSX.getSelectedNodes();
      this.ObjectRule = "AssetAdminPolicyRule";
      this.ObjectRuleUpdate = "AssetAdminPolicyRuleDataUpdate";
    }
    else if(this.SelectedPolicy == "PolicyAdminPolicyRule"){
      selectedRecords = cmpMtxPolicyAdminPolicyList.BlkJSX.getSelectedNodes();
      this.ObjectRule = "PolicyAdminPolicyRule";
      this.ObjectRuleUpdate = "PolicyAdminPolicyRuleDataUpdate";    
    }
    else if(this.SelectedPolicy == "RecordingAccessPolicy"){
      selectedRecords = cmpMtxRecordingAccessPolicyList.BlkJSX.getSelectedNodes();
      this.ObjectRule = "MediaStoreAdminPolicyRule";
      this.ObjectRuleUpdate = "MediaStoreAdminPolicyRuleDataUpdate";      
    }
    else if(this.SelectedPolicy == "MediaAccessPolicy"){
      selectedRecords = cmpMtxMediaAccessPolicyList.BlkJSX.getSelectedNodes();
      this.ObjectRule = "MediaStoreAdminPolicyRule";
      this.ObjectRuleUpdate = "MediaStoreAdminPolicyRuleDataUpdate";      
    }    
    else if(this.SelectedPolicy == "MediaStorePolicy"){
      selectedRecords = cmpMtxMediaStoreAdminList.BlkJSX.getSelectedNodes();
      this.ObjectRule = "MediaStoreAdminPolicyRule";
      this.ObjectRuleUpdate = "MediaStoreAdminPolicyRuleDataUpdate";      
    }
    else if(this.SelectedPolicy == "UserAdminPolicyRule"){
      selectedRecords = cmpMtxUserAdminList.BlkJSX.getSelectedNodes();
      this.ObjectRule = "UserAdminPolicyRule";
      this.ObjectRuleUpdate = "UserAdminPolicyRuleDataUpdate";      
    }
    else if(this.SelectedPolicy == "MediaStreamFilter"){
      selectedRecords = cmpMtxMediaStreamFilterList.BlkJSX.getSelectedNodes();
      this.ObjectRule = "MediaStreamFilter";
      this.ObjectRuleUpdate = "MediaStreamFilterDataUpdate";      
    }
    else if (this.SelectedPolicy == "MediaRoomPolicyRule") {
      selectedRecords = cmpMtxCollabList.BlkJSX.getSelectedNodes();
      this.ObjectRule = "MediaRoomPolicyRule";
      this.ObjectRuleUpdate = "MediaRoomPolicyRuleDataUpdate";
    } 
    else if (this.SelectedPolicy == "NetworkAddressRule") {
      selectedRecords = cmpMtxNATList.BlkJSX.getSelectedNodes();
      this.ObjectRule = "NetworkAddressRule";
      this.ObjectRuleUpdate = "NetworkAddressRuleDataUpdate";
    }     
        
    var agree = confirm('Are you sure you want to delete selected Policy/Policies? ');
    if(agree) {
      for (var it = selectedRecords.iterator(); it.hasNext(); ) {
        ++this.deleteRequestCounter;
        var selRecord = it.next();
        this.SelectedNID = selRecord.getAttribute("jsxid");
                                                               
        var reqDelPolicy = new ReqDeleteObjectNID(this.SelectedNID, this.ObjectRule, 
                                                       {action:"FNCCALL", context:this,
                                                          fnc:function(respCode, respVal) {
                                                                if(respCode == "SUCCESS" && respVal != null) {
                                                                  --this.deleteRequestCounter < 0 ? 0 : this.deleteRequestCounter;
                                                                  deleteFilterFromCache(this.ObjectRule, this.SelectedNID);
                                                                  if (this.deleteRequestCounter == 0) {
                                                                    topic_Publish(tApp, this.ObjectRuleUpdate, "DELETE");
                                                                  }
                                                                } else {
                                                                    --this.deleteRequestCounter < 0 ? 0 : this.deleteRequestCounter;
                                                                    var codeErrorMsg = "";
                                                                    try{
                                                                      codeErrorMsg = CommonErrorCodeArr[respCode].errorDesc;
                                                                    }
                                                                    catch(e){
                                                                    }
                                                                   var ErrorMsg = "Error: Delete request failed \n";
                                                                   ErrorMsg += "Error description: " + codeErrorMsg + "\n";
                                                                   ErrorMsg += "Error code: " + respCode;
                                                                   alert(ErrorMsg);
                                                                }
                                                              }
                                                       }
                                                     );
        reqDelPolicy.sendRequest();
      }       
    }
  }
  /********************* Policy Delete END ************************/  
  

  /********************* OBJECTFILTER Functions SART ************************/
  
    function cmpBlkObjectFilterDelete() {
    var selRecord = cmpMtxObjectFilterList.BlkJSX.getRecord(this.SelectedNID);
    var SelectedObjectFilterName = selRecord.jsxtext;
    var agree = confirm('Are you sure you want to delete policy admin filter: "'+ SelectedObjectFilterName + '" ? ');
    if(agree) {
      var reqDelObjectFilter = new ReqDeleteObjectNID(this.SelectedNID, "ObjectFilter", 
                                                       {action:"FNCCALL", context:this,
                                                         fnc:function(respCode, respVal) {
                                                               if(respCode == "SUCCESS" && respVal != null) {
                                                                 
                                                               } else {
                                                                   var codeErrorMsg = CommonErrorCodeArr[respCode].errorDesc;
                                                                   var ErrorMsg = "Error: PolicyAdmin policy request failed \n";
                                                                   ErrorMsg += "Error description: " + codeErrorMsg + "\n";
                                                                   ErrorMsg += "Error code: " + respCode;
                                                                   alert(ErrorMsg);
                                                               }
                                                             }
                                                       }
                                                     );
      reqDelObjectFilter.sendRequest();
    }
  }
  
  function cmpObjectFilterProperties_update() {
    changeImagePropertyState("chkObjectFilterPropertyTags","0");
    changeImagePropertyState("chkObjectFilterPropertyFilters","0");
    changeImagePropertyState("chkObjectFilterPropertyPolicies","0");   

    var SelectedNID = topic_Get(tApp, "ObjectFilterSelected");
    var selRecord = cmpMtxObjectFilterList.BlkJSX.getRecord(SelectedNID);
    if(selRecord != null) {
      var tagFilterNameSel = selRecord.jsxtext;
      
      if( (tagFilterNameSel != "NONE") || (tagFilterNameSel != "") ) {
        var filterNameNIDListNode = topic_GetNode(tApp, "//ObjectFilterNIDList");
        if( filterNameNIDListNode != null ) {
          var filterNameNIDListIter = filterNameNIDListNode.selectNodeIterator(".//ObjectNID");
          while( filterNameNIDListIter.hasNext() ) {
            var filterNameNID = filterNameNIDListIter.next().getValue();
            var filterNameNode = topic_GetNode(tCache, "//*[@NID='" + filterNameNID + "']");
            var filterName = filterNameNode.selectSingleNode(".//Name").getValue();
            if( tagFilterNameSel == filterName ) {
              var filterPropertyType = filterNameNode.selectSingleNode(".//TagObjectType").getValue();
              switch(filterPropertyType) {
                case "GroupTagdef":
                    changeImagePropertyState("chkObjectFilterPropertyTags","1");
                    break;
                case "Filter":
                    changeImagePropertyState("chkObjectFilterPropertyFilters","1");
                    break;
                case "PolicyRule":
                    changeImagePropertyState("chkObjectFilterPropertyPolicies","1");
                    break;
              }    
            }
          }
        }
      }
    }
  }

  function deleteFilterFromCache(Policyrule, SelectedNID){
     var filterListNode = getObjectCacheListNode(Policyrule); 
     var filterNode = filterListNode.selectSingleNode("//"+Policyrule+"[@NID='"+SelectedNID+"']");
     if(!jsx3.util.strEmpty(filterNode)){
        filterListNode.removeChild(filterNode);
     }
  }
  /********************* OBJECTFILTER Functions END ************************/


  /********************* TAGOBJECTFILTER Functions SART ************************/
  
    function cmpBlkTagObjectFilterDelete() {
    var usedInPolicies = "\nThis filter is being used in the following policy/policies: \n";
    var selRecord = cmpMtxTagObjectFilterList.BlkJSX.getRecord(this.SelectedNID);
    var SelectedTagObjectFilterName = selRecord.jsxtext;
    var flag = 0;
    var policyNameArr = [{PolicyType:"MediaStreamFilter",PolicyIter:".//MediaStreamFilter"},
                         {PolicyType:"UserAdminPolicyRule",PolicyIter:".//UserAdminPolicyRule"},
                         {PolicyType:"AssetAdminPolicyRule",PolicyIter:".//AssetAdminPolicyRule"},
                         {PolicyType:"MediaStoreAdminPolicyRule",PolicyIter:".//MediaStoreAdminPolicyRule"},
                         {PolicyType:"PolicyAdminPolicyRule",PolicyIter:".//PolicyAdminPolicyRule"},
                         {PolicyType:"NetworkAddressRule",PolicyIter:".//NetworkAddressRule"},
                         {PolicyType:"MediaRoomPolicyRule",PolicyIter:".//MediaRoomPolicyRule"}];
    for(var i=0; i< policyNameArr.length; i++){
      var policyNameArrEvent = policyNameArr[i];
      var ListNode = getObjectCacheListNode(policyNameArrEvent.PolicyType);
      var ListIter = ListNode.selectNodeIterator(policyNameArrEvent.PolicyIter);
      while( ListIter.hasNext() ) {
        var ListIterNode = ListIter.next();
        var ListIterNodeString = ListIterNode.toString();
        var matchNID = ListIterNodeString.search(this.SelectedNID);
        if( matchNID != -1 ) {
           flag = 1;
          var PolicyName = "";
          if(i == 0) {
            PolicyName = ListIterNode.selectSingleNode(".//Name").getValue();
          } else {
            PolicyName = ListIterNode.selectSingleNode(".//rulename").getValue();
          }
          usedInPolicies += PolicyName + " \n";    
        }
      }
      
    }        
    if( flag == 1){
      alert("Cannot delete this filter." + usedInPolicies);
      return false;
    }
    var agree = confirm('Are you sure you want to delete filter: "'+ SelectedTagObjectFilterName + '" ? ');
    if(agree) {
      var reqDelTagObjectFilter = new ReqDeleteObjectNID(this.SelectedNID, "TagObjectFilter", 
                                                           {action:"FNCCALL", context:this,
                                                             fnc:function(respCode, respVal) {
                                                                   if(respCode == "SUCCESS" && respVal != null) {
                                                                     topic_Publish(tApp, "TagObjectFilterDataUpdate", "DELETE");
                                                                   } else {
                                                                       var codeErrorMsg = CommonErrorCodeArr[respCode].errorDesc;
                                                                       var ErrorMsg = "Error: Filter delete request failed \n";
                                                                       ErrorMsg += "Error description: " + codeErrorMsg + "\n";
                                                                       ErrorMsg += "Error code: " + respCode;
                                                                       alert(ErrorMsg);
                                                                   }
                                                                 }
                                                           }
                                                        );
      reqDelTagObjectFilter.sendRequest();
    }
  }
  
  function cmpTagObjectFilterProperties_update() {
    changeImagePropertyState("chkFilterPropertyAsset","0");
    changeImagePropertyState("chkFilterPropertyUser","0");
    changeImagePropertyState("chkFilterPropertyMediaRoom","0");
    changeImagePropertyState("chkFilterPropertyProfile","0");
    changeImagePropertyState("chkFilterPropertyResource","0");
    changeImagePropertyState("chkFilterPropertyMedia","0");    

    var SelectedNID = topic_Get(tApp, "TagObjectFilterSelected");
    var selRecord = cmpMtxTagObjectFilterList.BlkJSX.getRecord(SelectedNID);
    if(selRecord != null) {
      var tagFilterNameSel = selRecord.jsxtext;
      
      if( (tagFilterNameSel != "NONE") || (tagFilterNameSel != "") ) {
        var filterNameNIDListNode = topic_GetNode(tApp, "//TagObjectFilterNIDList");
        if( filterNameNIDListNode != null ) {
          var filterNameNIDListIter = filterNameNIDListNode.selectNodeIterator(".//ObjectNID");
          while( filterNameNIDListIter.hasNext() ) {
            var filterNameNID = filterNameNIDListIter.next().getValue();
            var filterNameNode = topic_GetNode(tCache, "//*[@NID='" + filterNameNID + "']");
            var filterName = filterNameNode.selectSingleNode(".//Name").getValue();
            if( tagFilterNameSel == filterName ) {
              var filterPropertyListIter = filterNameNode.selectNodeIterator(".//TagObjectTypeList/TagObjectType");
              while( filterPropertyListIter.hasNext() ) {
                var filterPropertyNode = filterPropertyListIter.next();
                var filterPropertyType = filterPropertyNode.getValue();
                switch(filterPropertyType) {
                  case "User":
                    changeImagePropertyState("chkFilterPropertyUser","1");
                    break;
                  case "AssetResource":
                    changeImagePropertyState("chkFilterPropertyAsset","1");
                    break;
                  case "MediaStreamPortResource":
                  case "MediaStorePortResource":
                    changeImagePropertyState("chkFilterPropertyResource","1");
                    break;
                  case "MediaStreamProfile":
                    changeImagePropertyState("chkFilterPropertyProfile","1");
                    break;
                  case "MediaRoom":
                    changeImagePropertyState("chkFilterPropertyMediaRoom", "1");
                    break;
                  case "MediaStreamFileResource":
                  case "MediaStreamDirResource":
                    changeImagePropertyState("chkFilterPropertyMedia","1");                    
                    break;
                }
              }   
            }
          }
        }
      }
    }
  }
  /********************* TAGOBJECTFILTER Functions END ************************/  
  
  /********************* GROUPTAGDEF Functions Start ************************/
  function cmpBlkGroupTagDelete() {
    var selRecord = cmpMtxGroupTagList.BlkJSX.getRecord(this.SelectedNID);
    var SelectedGroupTagName = selRecord.jsxtext;
    var agree = confirm('Are you sure you want to delete tag: "'+ SelectedGroupTagName + '" ? ');
    if(agree) {
      var reqDelGroupTag = new ReqDeleteObjectNID(this.SelectedNID, "GroupTagDef", 
                                                   {action:"FNCCALL", context:this,
                                                      fnc:function(respCode, respVal) {
                                                            if(respCode == "SUCCESS" && respVal != null) {
                                                              topic_Publish(tApp, "GroupTagDefDataUpdate", "DELETE");
                                                            } else {
                                                               var codeErrorMsg = CommonErrorCodeArr[respCode].errorDesc;
                                                               var ErrorMsg = "Error: TagName delete request failed \n";
                                                               ErrorMsg += "Error description: " + codeErrorMsg + "\n";
                                                               ErrorMsg += "Error code: " + respCode;
                                                               alert(ErrorMsg);
                                                            }
                                                          }
                                                   }
                                                 );
      reqDelGroupTag.sendRequest();
    }
  }

  function cmpBlkGroupTagValueDelete() {

    var SelectedGroupTagName = topic_Get(tApp, "GroupTagValueSelected");
    var agree = confirm('Are you sure you want to delete tag value: "'+ SelectedGroupTagName + '" ? ');
    if(agree) {
      var GroupTagSelectedNID = topic_Get(tApp, "GroupTagSelected");
      var SetMode = "Replace";
      // setup GroupTagValue map rule info
      var GroupTagValueNode = "";
      var GroupTagValueMapRule = new Object;
      GroupTagValueMapRule.state = false;
      GroupTagValueMapRule.objtype = "GroupTagDef";
      GroupTagValueMapRule.rule = "";
      GroupTagValueNode = deleteTagValueRequestXML(GroupTagSelectedNID);
      if( GroupTagValueNode == null ) return;
      // set NID in GroupTagValue node if replace mode

      GroupTagValueNode.setAttribute("NID", GroupTagSelectedNID);
          // do set request for GroupTagValue              
      var reqSetGroupTagValueObject = new ReqSetObject(SetMode, GroupTagValueMapRule.objtype, GroupTagValueNode,
                                                        {action:"FNCCALL", context:this,
                                                          fnc:function(respCode, respVal) {
                                                                if(respCode == "SUCCESS" && respVal != null) {
                                                                  topic_Publish(tApp, "GroupTagValueUpdate", "DONE");
                                                                  //topic_Publish(tApp, "GroupTagValueSelected", "UNSELECTED");
                                                                } else {
                                                                   var codeErrorMsg = CommonErrorCodeArr[respCode].errorDesc;
                                                                   var ErrorMsg = "Error: TagValue delete request failed \n";
                                                                   ErrorMsg += "Error description: " + codeErrorMsg + "\n";
                                                                   ErrorMsg += "Error code: " + respCode;
                                                                   alert(ErrorMsg);
                                                                }
                                                              }
                                                        }  
                                                      );
      if( reqSetGroupTagValueObject != null) {
        reqSetGroupTagValueObject.sendRequest(this);
      }
    }
  }

  function deleteTagValueRequestXML(GroupTagNID){
    var CacheGroupTagListNode = (getObjectCacheDefaultNode("GroupTagDef", GroupTagNID)).cloneNode(true); 
    var groupTagName = CacheGroupTagListNode.selectSingleNode(".//TagName").getValue();
    groupTagValueList=CacheGroupTagListNode.selectSingleNode(".//TagValueList/TagValue").cloneNode(true);			
    var groupTagValueListParent  = CacheGroupTagListNode.selectSingleNode(".//TagValue").getParent();

    var oldTagValue = topic_Get(tApp, "GroupTagValueSelected");
    var groupTagValueListIter  = CacheGroupTagListNode.selectNodeIterator(".//TagValueList/TagValue");
    while( groupTagValueListIter .hasNext() ) {
      var groupTagValueNode = groupTagValueListIter .next();
      var groupTagValueName = groupTagValueNode.getValue();
      if(groupTagValueName == oldTagValue ) {
        groupTagValueListParent.removeChild(groupTagValueNode);
      }
    }   

    return  CacheGroupTagListNode;
  }
  
  function cmpGroupTagProperties_update() {
    changeImagePropertyState("chkTagPropertyAsset1","0");
    changeImagePropertyState("chkTagPropertyUser1","0");
    changeImagePropertyState("chkTagPropertyMediaRoom1","0");
    changeImagePropertyState("chkTagPropertyProfile1","0");
    changeImagePropertyState("chkTagPropertyResource1","0");
    changeImagePropertyState("chkTagPropertyMedia1","0");

    var SelectedNID = topic_Get(tApp, "GroupTagSelected");
    var selRecord = cmpMtxGroupTagList.BlkJSX.getRecord(SelectedNID);
    if(selRecord != null) {
      var tagGroupNameSel = selRecord.jsxtext;

      if( (tagGroupNameSel != "NONE") || (tagGroupNameSel != "") ) {
        var groupTagNameNIDListNode = topic_GetNode(tApp, "//GroupTagNIDList");
        if( groupTagNameNIDListNode != null ) {
          var groupTagNameNIDListIter = groupTagNameNIDListNode.selectNodeIterator(".//ObjectNID");
          while( groupTagNameNIDListIter.hasNext() ) {
            var groupTagNameNID = groupTagNameNIDListIter.next().getValue();
            var groupTagNameNode = topic_GetNode(tCache, "//*[@NID='" + groupTagNameNID + "']");
            var groupTagName = groupTagNameNode.selectSingleNode(".//TagName").getValue();
            if( tagGroupNameSel == groupTagName ) {
              var groupTagPropertyListIter = groupTagNameNode.selectNodeIterator(".//TagObjectTypeList/TagObjectType");
              while( groupTagPropertyListIter.hasNext() ) {
                var groupTagPropertyNode = groupTagPropertyListIter.next();
                var groupTagPropertyType = groupTagPropertyNode.getValue();
                switch(groupTagPropertyType) {
                  case "User":
                    changeImagePropertyState("chkTagPropertyUser1","1");
                    break;
                  case "AssetResource":
                    changeImagePropertyState("chkTagPropertyAsset1","1");
                    break;
                  case "MediaStreamPortResource":
                  case "MediaStorePortResource":
                    changeImagePropertyState("chkTagPropertyResource1","1");
                    break;
                  case "MediaStreamProfile":
                    changeImagePropertyState("chkTagPropertyProfile1","1");
                    break;
                  case "MediaRoom":
                    changeImagePropertyState("chkTagPropertyMediaRoom1","1");
                  case "MediaStreamFileResource":
                  case "MediaStreamDirResource":
                    changeImagePropertyState("chkTagPropertyMedia1","1");                    
                    break;
                }
              }   
            }
          }
        }
      }
    }
  }
  /********************* GROUPTAGDEF Functions END ************************/ 
  

} // end cmpBlkPolicy_Create

function cmpBlkPolicy_Destroy() {
  var blkPolicyBlkJSX = PNameSpace.getJSXByName("blkPolicy");
  blkPolicyBlkJSX.removeChildren();
} 

  function showPolicyAdminPolicySpyGlassPopUp(objJSX, strRecordId) {
  
    if( strRecordId == null ) return false;
    var policyAdminPolicyNode = topic_GetNIDNode(tCache, strRecordId);
    if(policyAdminPolicyNode == null) return false;
    //var pos = PNameSpace.getJSXByName("mtxpolicyAdminPolicyList").getAbsolutePosition(document.body);
    var pos = objJSX.getAbsolutePosition();
    var strHTML = GetspyGlassConatinerDIV(pos.L, pos.T, pos.W, pos.H);
    strHTML += spyGlassStartDIV;
    strHTML += spyGlassTableDIV;

    var serviceOpListIter  = policyAdminPolicyNode.selectNodeIterator(".//ServiceOperatorFilterSet/ServiceOperatorFilter");
    strHTML += spyGlassHeaderDIV + "User Filters" + spyGlassHeaderDIVClose;
    strHTML += spyGlassRowContainerDIV;
    while( serviceOpListIter.hasNext() ) {
      var serviceOpListIterNode = serviceOpListIter.next();
      var UserFilterNID = serviceOpListIterNode.selectSingleNode(".//UserFilterNID").getValue();
      var ResourceFilterNID = serviceOpListIterNode.selectSingleNode(".//ResourceFilterNID").getValue();
      if( (UserFilterNID != "") && (ResourceFilterNID != "") ) {
        var UserFilterName = getTagObjectFilterName(UserFilterNID);
        var ResourceFilterName = getTagObjectFilterName(ResourceFilterNID);
        strHTML += spyGlassRowEnabledSpan + UserFilterName + "@" + ResourceFilterName + spyGlassRowEnabledSpanClose + spyGlassSpanNewLine;
      }
    }
    strHTML += spyGlassRowContainerDIVClose;
    strHTML += spyGlassTableDIVClose;
    
    
    strHTML += spyGlassStartDIVClose;
    strHTML += spyGlassConatinerDIVClose;
    return strHTML;  

  } // end showPolicyAdminPolicySpyGlassPopUp