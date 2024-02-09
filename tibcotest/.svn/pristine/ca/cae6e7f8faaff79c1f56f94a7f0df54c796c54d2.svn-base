var dialogCellRefreshArr = new Object;
var dialogLocalArr = new Object;

function cmpBlkDevice_Create() {
  var blkAssetsBlkJSX = PNameSpace.getJSXByName("blkAssets");
  blkAssetsBlkJSX.load("components/assets/assets.xml");
  
  var getAssetsXSLfile = PNameSpace.loadInclude("JSXAPPS/IPVS_AdminUI/xsl/AssetList.xsl", "AssetList", "xsl");
  var getAssetsXSLDoc = jsx3.xml.XslDocument.wrap(getAssetsXSLfile);


  var MenuListNode = loadXMLFile("JSXAPPS/IPVS_AdminUI/xml/selections/DeviceMenuList.xml"); 
  var deviceTypeListNode = loadXMLFile("JSXAPPS/IPVS_AdminUI/xml/selections/DeviceType.xml");
  var DeviceMenuListRoot = topic_Set(tCache, "DeviceMenuListRoot", null);
  DeviceMenuListRoot.appendChild(MenuListNode);

  // set device refresh status time interval
  topic_Set(tApp, "DeviceStatusRefreshInterval",30);
  topic_Set(tApp, "AssetResourceGetDataState", "NONE");
  topic_Set(tApp, "AssetsCustomize", "NONE");
  topic_Set(tApp, "AssetWhereXPath", "");  
  topic_Set(tApp, "AssetsFilter", "NONE");
  topic_Set(tApp, "AssetResourceObjectType", "AssetResource");
  topic_Set(tApp, "AssetWhereTextSubStr", ""); 
  topic_Set(tApp, "AssetWhereTagExpr", ""); 
  topic_Set(tApp, "AssetSelectFormat", "BASIC");
  topic_Set(tApp, "SearchTextSubStrIn", "");

  topic_Set(tApp, "DeviceTagNameObjectType", "Device");

  var AssetCountToFetch = 20;
  
  var cmpMtxAssetList = new CmpCustomPaginationList(["blkAssetList","mtxAssetList"],
                                                    [{action:"APPEVENT", eventvar:"AssetResourceGetData", eventval:null}],
                                                    {persist:{col:"jsxid",initval:false},
                                                     action:"APPEVENT", eventvar:"AssetSelected", eventval:"jsxid"},
                                                    [{ColumnName:"deviceMemory", FncName:"convertDeviceMemoryToMB"},
                                                     {ColumnName:"deviceConnection", FncName:"deviceConnectionStatus"},
                                                     {ColumnName:"deviceHAStatus", FncName:"deviceHAStatus"}
                                                     ],
                                                    "showDeviceErrorDetails"
                                                   );

  cmpMtxAssetList.allowMultiSelection = true;
  cmpMtxAssetList.deleteRequestCounter = 0;
  cmpMtxAssetList.upgradeRequestCounter = 0;
                                             
  //translate($b/Info/Title ,'ABCDEFGHIJKLMNOPQRSTUVWXYZ','abcdefghijklmnopqrstuvwxyz')
  cmpMtxAssetList.SortPathArr =  {jsxtext:"title",
                                  assetType:"type",
                                  assetStatus:"state",
                                  assetDescription:"description"
                                  //jsxTagValue:"string-join ( $b/Info/Groups/Tag[@Name='_TAGNAME_']/@Value, ',')"
                                 };
                               
  cmpMtxAssetList.cmpReqParamArr = {StartFrom: "ObjectList_StartFrom",
                                    CountToFetch: "ObjectList_CountToFetch",
                                    SortOrder: "ObjectList_SortOrder",
                                    ObjectCount: "ObjectCount",
                                    SortByXpath: "ObjectList_SortBy"
                                   };                               
                                                        
  cmpMtxAssetList.DataUpdatedEvent = {action:"TOPICEVENT", topic:tApp, eventvar:"AssetResourceDataUpdated", eventval:"LOADED"};

  cmpMtxAssetList.cmpReq = new cmpReq_GetSessionAPIPaginationList(tApp, "GetDevice",
                                                                  [{ VarName: "ObjectList_SelectFormat", VarRef: "AssetSelectFormat" },
                                                                   { VarName: "ObjectList_WhereTextSubStr", VarRef: "AssetWhereTextSubStr" },
                                                                   { VarName: "ObjectList_SearchTextSubStrIn", VarRef: "SearchTextSubStrIn" },
                                                                   { VarName: "ObjectList_WhereTagExpr", VarRef: "AssetWhereTagExpr" }
                                                                  ],
                                                                  { action: "FNCCALL", context: cmpMtxAssetList,
                                                                    fnc: function (respCode, respVal) {
                                                                      if (respCode == "SUCCESS" && respVal != null) {
                                                                        var deviceSoftwareVersion = topic_Get(tApp, "MsDeviceSoftwareVersion");
                                                                        getAssetsXSLDoc.setParam("jsximgdevicesoftwareversion", deviceSoftwareVersion);
                                                                        var currSoftwareVersion = topic_Get(tApp, "MSSoftwareVersion");
                                                                        getAssetsXSLDoc.setParam("jsximgmssoftwareversion", currSoftwareVersion);
                                                                        var respValNode = getAssetsXSLDoc.transform(respVal);
                                                                        var assetsNode = new jsx3.xml.Document();
                                                                        assetsNode.loadXML(respValNode);
                                                                        this.onResponse(respCode, assetsNode);
                                                                        cmpBlkAssetSelection();
                                                                      }
                                                                      else {
                                                                        log.info("GetDeviceRequest failed. Error Code :" + respCode);
                                                                        this.onResponseErrorHandle("GetDevice", respCode, respVal);
                                                                      }
                                                                    }
                                                                  }
                                                                 );

  cmpMtxAssetList.cmpItemByNIDReqFnc = function () {
                                         topic_Set(tApp, "WhereAssetNID", cmpMtxAssetList.WhereNIDArr.join(","));
                                         return new cmpReq_GetSessionAPIPaginationList(tApp, "GetDevice",
                                                                                      [{ VarName: "ObjectList_WhereNID", VarRef: "WhereAssetNID"},
                                                                                       { VarName: "ObjectList_SelectFormat", VarRef: "AssetSelectFormat"},
                                                                                       { VarName: "ObjectList_WhereTextSubStr", VarRef: "AssetWhereTextSubStr"},
                                                                                       { VarName: "ObjectList_SearchTextSubStrIn", VarRef: "SearchTextSubStrIn"},
                                                                                       { VarName: "ObjectList_WhereTagExpr", VarRef: "AssetWhereTagExpr"}
                                                                                      ],
                                                                                      {action: "FNCCALL", context: cmpMtxAssetList,
                                                                                       fnc: function(respCode, respVal) {
                                                                                              if (respCode == "SUCCESS" && respVal != null) {
                                                                                                var respValNode = getAssetsXSLDoc.transform(respVal);
                                                                                                var assetsNode = new jsx3.xml.Document();
                                                                                                assetsNode.loadXML(respValNode);
                                                                                                this.onResponseUpdateRecord(respCode, assetsNode);
                                                                                                cmpBlkAssetSelection();
                                                                                              }
                                                                                              else{
                                                                                                log.info("GetDeviceRequest failed. Error Code :" + respCode);
                                                                                                this.onResponseErrorHandle("GetDevice", respCode, respVal);
                                                                                              }
                                                                                            }
                                                                                      }, "ItemByNID"
                                                                                     );
                                       }  
                                    
  cmpMtxAssetList.columnDataState = null;
  cmpMtxAssetList.columnMtxDoc = new jsx3.xml.CDF.Document.newDocument();
  cmpMtxAssetList.screenName = "Asset";  
  cmpMtxAssetList.ColumnDataUpdatedEvent = {action:"TOPICEVENT", topic:tApp, eventvar:"AssetResourceColumnDataUpdated", eventval:"DATE"};
  cmpMtxAssetList.InsertBeforeColumnName = "deviceCPU";
  cmpMtxAssetList.columnCmpReq = new cmpReq_GetSessionAPIPaginationList(tApp, "GetTags",
                                                                  [{ VarName: "ObjectList_SelectFormat", VarRef: "TagNameResponseSelectFormat" },
                                                                   { VarName: "ObjectList_WhereObjectType", VarRef: "DeviceTagNameObjectType"}],
                                                                  { action: "FNCCALL", context: cmpMtxAssetList,
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
                                                               
  cmpMtxAssetList.filterTagDataState = null;
  cmpMtxAssetList.filterTagDoc = new jsx3.xml.CDF.Document.newDocument();
  cmpMtxAssetList.filterTagDataUpdatedEvent = { action: "TOPICEVENT", topic: tApp, eventvar: "AssetResourceFilterTagDataUpdated", eventval: "LOADED" };
  cmpMtxAssetList.filterTagCmpReq = new cmpReq_GetSessionAPIPaginationList(tApp, "GetTags",
                                                                  [{ VarName: "ObjectList_SelectFormat", VarRef: "TagNameResponseSelectFormat" },
                                                                   { VarName: "ObjectList_WhereObjectType", VarRef: "DeviceTagNameObjectType"}],
                                                                  { action: "FNCCALL", context: cmpMtxAssetList,
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
                                                                 
  cmpMtxAssetList.RefreshData = {type:"agentobjtype", objtype:"DeviceStatus_Full", xpath:null, agentJID:"assetContactOwnerJID", status:"assetStatusOrginal", refreshInterval:"30"};
  cmpMtxAssetList.RefreshColArr = [{objtype:"DeviceStatus_Full", objColumnName:"deviceCPU", objXPath:"\".//SystemElementsStatus/CPUStatus/Parameters/Parameter[@name='LoadAverage']\""},
                                   {objtype:"DeviceStatus_Full", objColumnName:"deviceUptime", objXPath:"\".//SystemElementsStatus/CPUStatus/Parameters/Parameter[@name='Uptime']\""},
                                   {objtype:"DeviceStatus_Full", objColumnName:"deviceMemory", objXPath:"\".//SystemElementsStatus/MemoryStatus/Parameters/Parameter[@name='Free']\""},
                                   {objtype:"DeviceStatus_Full", objColumnName:"deviceDisk", objXPath:"\".//StorageElementsStatus/HardDiskTableStatus/ParametersList/Parameters/Parameter[@name='PercentUsed']\""},
//                                   {objtype:"DeviceStatus_Full", objColumnName:"deviceSWVersion", objXPath:"\".//SystemElementsStatus/SoftwareImageStatus/Parameters/Parameter[@name='Version']\""},
                                   {objtype:"DeviceStatus_Full", objColumnName:"deviceFWVersion", objXPath:"\".//SystemElementsStatus/FirmwareImageStatus/ParametersList/Parameters/Parameter[@name='Board Firmware']\""},
                                   {objtype:"DeviceStatus_Full", objColumnName:"deviceNTPClient", objXPath:"\".//SystemElementsStatus/SystemClockStatus/Parameters/Parameter[@name='NTPStatus']\""},
                                   {objtype:"DeviceStatus_Full", objColumnName:"deviceConnection", objXPath:"\".//ServiceElementsStatus/XMPPAgentServiceStatus/Parameters/Parameter[@name='Status']\""},
                                   {objtype:"DeviceStatus_Full", objColumnName:"deviceHAStatus", objXPath:"\".//ServiceElementsStatus/HAServiceStatus/HAServerStatus/Parameters/Parameter[@name='Peer Status']\""},
                                   {objtype:"DeviceStatus_Full", objColumnName:"jsximgDeviceStatus", objXPath:"\".//DeviceStatus/@StateImage\""},
                                   {objtype:"DeviceStatus_Full", objColumnName:"assetStatus", objXPath:"\".//DeviceStatus/@Status\""}
                                   
                                  ];
  
  cmpMtxAssetList.MenuEvent = {action:"APPEVENT", eventvar:"AssetMenuSelected", eventval:"jsxid"};
 
  var cmpAssetCustomize = new cmpCustomizeContainer(["blkAssets","blkAssetsCustomize"],
                                                    [["blkAssetsCustomize","mtxCustomize"],
                                                     ["blkAssetsCustomize","btnCustomizeApply"],
                                                     ["blkAssetsCustomize","btnCustomizeCancel"],
                                                     ["blkAssetsCustomize","btnRefreshColumnList"],
                                                     ["blkAssetsCustomize", "btnCustomizeClose"],
                                                     ["blkAssetsCustomize", "mtxMetaDataTable"],
                                                     ["blkAssetsCustomize", "imgbtnAddMetaData"],
                                                     ["blkAssetsCustomize", "imgbtnDeleteMetaData"],
                                                     ["blkAssetsCustomize", "CustomizeLayout"],
                                                     ["blkAssetsCustomize", "blkMetaData"],
                                                     ["HideMetaData"]
                                                    ],
                                                    cmpMtxAssetList,
                                                    tApp,
                                                    {action:"TOPICEVENT", topic:tApp, eventvar:"AssetsCustomize", eventval:"NONE"},
                                                    {action:"FNCCALL", context:cmpAssetCustomize,
                                                     fnc:function() {
                                                           var strXMLCacheId = cmpAssetCustomize.cmpMtxCustomize.BlkJSX.getXMLId();
                                                           var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
                                                           cmpAssetCustomize.MtxObjectList.columnMtxDoc = objXML;
                                                           //save the columns doc in the Perferences
                                                           setLocalPreferences("GLOBAL", ".//ClientApp[@name='IPVSAdmin']/CustomizeList/Asset", objXML);
                                                           //get the noAutoDisplayTags val if not there then add the attribute
                                                           var noAutoDisplayTags = getLocalPreferences("GLOBAL", ".//ClientApp[@name='IPVSAdmin']/CustomizeList/Asset/@noAutoDisplayTags", null);
                                                           if (jsx3.util.strEmpty(noAutoDisplayTags)) {
                                                             var addSuccess = addAttributeLocalPreferences("GLOBAL", ".//ClientApp[@name='IPVSAdmin']/CustomizeList/Asset", "noAutoDisplayTags", "false");
                                                             log.info("Added noAutoDisplayTags for Asset in Preferences.xml :" + addSuccess); 
                                                           }
                                                           cmpAssetCustomize.MtxObjectList.setMtxColumn();
                                                           //cmpAssetCustomize.MtxObjectList.doRepaint();
                                                           topic_Publish(tApp, "AssetsCustomize", "NONE");
                                                         }
                                                    },
                                                    [{action:"APPEVENT", eventvar:"AssetResourceColumnDataUpdated", eventval:null}],
                                                    null,
                                                    null
                                                   );
                                                   
  var cmpBlkAssetCustomize = new CmpContainer("blkAssets", 
                                              [{cmp:cmpAssetCustomize, sel: "topic_Get(tApp, 'AssetsCustomize') != 'NONE'"}],
                                              [{ action:"APPEVENT", eventvar: "AssetsCustomize", eventval: null }],
                                              null ); 
                                               
  var cmpBtnAssetCustomize = new CmpButton("btnCustomizeAsset",
                                             null,
                                             { action: "FNCCALL", context: null,
                                                 fnc: function () {
                                                     topic_Publish(tApp, "AssetResourceColumnDataUpdated", (newServerDate()).getTime() );
                                                     jsx3.sleep(function () {
                                                         topic_Publish(tApp, "AssetsCustomize", "OPEN");
                                                     }, null, this); 
                                               }
                                             }
                                           );  


  var cmpAssetFilter = new cmpFilterContainer(["blkAssets","blkAssetsFilter"],
                                              [["blkAssetsFilter","blkTagOrg"],
                                               ["blkAssetsFilter","blkTagList"],
                                               ["blkAssetsFilter","txtNameSearch"],
                                               ["blkAssetsFilter","chkNameSearch"],
                                               ["blkAssetsFilter","chkDescriptionSearch"],
                                               ["blkAssetsFilter","btnFilterCancel"],
                                               ["blkAssetsFilter","btnFilterApply"],
                                               ["blkAssetsFilter","btnFilterClearAll"],
                                               ["blkAssetsFilter", "btnFilterClose"],
                                               ["blkAssetsFilter", "btnFilterRefresh"],
                                               ["blkAssetsFilter", "btnFilterRefreshSpinner"],
                                               ["blkAssetsFilter", "btnFilterRefreshNotify"],
                                               ["blkAssetsFilter", "chkBookmarksSearch"],
                                               ["blkAssetsFilter", "chkMetaDataSearch"]
                                              ],
                                              cmpMtxAssetList,
                                              tApp,
                                              {action:"TOPICEVENT", topic:tApp, eventvar:"AssetsFilter", eventval:"NONE"},
                                              {action:"FNCCALL", context:cmpAssetFilter,
                                                 fnc:function() {
                                                       var searchTextSubStrIn = "";
                                                       var searchName = cmpAssetFilter.cmpTagList.getChkName();
                                                       var searchDescription = cmpAssetFilter.cmpTagList.getChkDescription();
                                                       
                                                       if(searchTextSubStrIn == "" && searchName == "1"){
                                                         searchTextSubStrIn += "TITLE";
                                                       }
                                                       else if(searchTextSubStrIn != "" && searchName == "1"){
                                                         searchTextSubStrIn += ",TITLE";
                                                       }
                                                       if(searchTextSubStrIn == "" && searchDescription == "1"){
                                                         searchTextSubStrIn += "DESCRIPTION";
                                                       }
                                                       else if(searchTextSubStrIn != "" && searchDescription== "1") {
                                                         searchTextSubStrIn += ",DESCRIPTION";
                                                       }
                                                       
                                                       topic_Set(tApp, "SearchTextSubStrIn", searchTextSubStrIn);
                                                       var AssetWhereTextSubStr = cmpAssetFilter.cmpTagList.getTextSubStr();
                                                       topic_Set(tApp, "AssetWhereTextSubStr", AssetWhereTextSubStr);
                                                       
                                                       var AssetWhereTagExpr = cmpAssetFilter.cmpTagList.getAPITagExpr();
                                                       topic_Set(tApp, "AssetWhereTagExpr", AssetWhereTagExpr);

                                                       cmpMtxAssetList.ResetToStart(1,AssetCountToFetch,0);
                                                       cmpAssetFilter.MtxObjectList.doRepaint();
                                                       topic_Publish(tApp, "AssetsFilter", "NONE");
                                                     }
                                              },
                                              [{action:"APPEVENT", eventvar:"AssetResourceFilterTagDataUpdated", eventval:null}],
                                              null,
                                              null
                                             );
                                                   
  var cmpBlkAssetFilter = new CmpContainer("blkAssets", 
                                              [{cmp:cmpAssetFilter, sel: "topic_Get(tApp, 'AssetsFilter') != 'NONE'"}],
                                              [{ action:"APPEVENT", eventvar: "AssetsFilter", eventval: null }],
                                              null ); 
                                              
  var cmpBtnAssetFilter = new CmpButton("btnFilterAsset",
                                             null,
                                             { action: "FNCCALL", context: null,
                                               fnc: function() {
                                                   //cmpAssetFilter.cmpBtnClearAll.clearAllSelection();
                                                   jsx3.sleep(function () {
                                                       topic_Publish(tApp, "AssetsFilter", "OPEN");
                                                   }, null, this); 
                                               }
                                             }
                                           );
                                                                                      
  var cmpAssestNavControls = new CmpPageNavButtonsContainer(["blkAssets","blkAssetFooter"],
                                                      [["blkAssets", "btnPageFirst"],
                                                       ["blkAssets", "btnPagePrev"],
                                                       ["blkAssets", "btnPageNext"],
                                                       ["blkAssets", "btnPageLast"],
                                                       ["blkAssets", "lblRecordInfo"],
                                                       ["blkAssets", "txtCurrentRecordNumber"],
                                                       ["blkAssets", "btnPageRefresh"],
                                                       ["blkAssets", "btnPageRefreshSpinner"],
                                                       ["blkAssets", "btnPageRefreshNotify"]
                                                      ],
                                                      cmpMtxAssetList,
                                                      [{action:"TOPICEVENT", topic:tApp, eventvar:"AssetResourceDataUpdated", eventval:null}],
                                                      null
                                                     ); 
                                                     
  eventApp.subscribe("AssetMenuSelected", null, this,
                      function() {
                        // check if edit asset is allowed or not
                        if (clsUserPermission.getPermission('Asset', 'modify') == "false") {
                          return;                        
                        }
                        var AssetMenuSelected = topic_Get(tApp, 'AssetMenuSelected');
                        var cmpMtxAssetListRecord = cmpMtxAssetList.BlkJSX.getRecordNode(AssetMenuSelected);
                        var AssetType = cmpMtxAssetListRecord.getAttribute("assetTypeOrginal");
                        var AssetStatusType = cmpMtxAssetListRecord.getAttribute("assetStatusOrginal");
                        var TemplateAssetResourceNID = cmpMtxAssetListRecord.getAttribute("jsxTemplateAssetResourceNID");
                        var MSStatus = cmpMtxAssetListRecord.getAttribute("assetMSStatusOriginal");
                        var dmsDeviceType = deviceTypeListNode.selectSingleNode(".//device[@jsxid='DigitalMediaServer']/record[@jsxid='"+ AssetType +"']");
                        var clientDeviceTypeNode = deviceTypeListNode.selectSingleNode(".//device[@jsxid='Client']/record[@jsxid='"+ AssetType +"']");
                        var clientDeviceName = "";
                        if(!jsx3.util.strEmpty(clientDeviceTypeNode)){
                          clientDeviceName = clientDeviceTypeNode.getAttribute("jsxtext");
                        }
                        if( (clientDeviceName == "Web Client") && 
                            (TemplateAssetResourceNID != "TEMPLATE") ){
                          // no rightclick option so publishing a diff type  
                          topic_Publish(tApp, "AssetMenuTypeSelected", "NoEditAllowed");    
                        }
                        else if( !jsx3.util.strEmpty(dmsDeviceType) &&
                                 MSStatus == "standalone" ) {
                          topic_Publish(tApp, "AssetMenuTypeSelected", "DMS-MS-Device");
                        }
                        // for ummanaged devices we show only 2 right click options
                        else if( AssetStatusType == "Unmanaged" ){
                          topic_Publish(tApp, "AssetMenuTypeSelected", "Unmanaged-Device");
                        }
                        else {
                          topic_Publish(tApp, "AssetMenuTypeSelected", AssetType);
                        }
                      }
                    ); 
  eventApp.subscribe("AssetMenuActionSelected", null, this,
                      function() {
                        var AssetMenuActionSelected = topic_Get(tApp, 'AssetMenuActionSelected');
                        this.SelectedAssetNID = topic_Get(tApp, "AssetSelected");                       

                        var selRecord = cmpMtxAssetList.BlkJSX.getRecord(this.SelectedAssetNID);
                        if(selRecord == null) {
                          alert("Please select device");
                          return false;                        
                        }

                        var assetName1 = selRecord.jsxtext;
                        var assetType = selRecord.assetType;
                        var assetType1 = selRecord.assetTypeOrginal;
                        var assetStatus1 = selRecord.assetStatusOrginal;
                        var assetOwnerUserJID = selRecord.assetContactOwnerJID;
                        var assetFeature  = selRecord.jsxAssetFeature;
                        if(assetFeature == "true"){
                          assetFeature = "UnManned";
                        }
                        else{
                          assetFeature = "";
                        }
                        this.SelectedAssetStatus = assetStatus1;
                        this.DevicePortNIDs = selRecord.devicePortNIDs;                       
                        this.currentPortNIDsArray = this.DevicePortNIDs.split(',');
                        var deviceTypeNode = MenuListNode.selectSingleNode(".//record[@jsxid='"+ assetType1 +"']");
                        this.isMNADevice = deviceTypeNode.getAttribute("jsxMNADevice");
                        
                        var deviceSaveToFlashCommand = topic_Get(assetOwnerUserJID, "DeviceSaveToFlashCommand");
                        if(deviceSaveToFlashCommand == "PROCESS"){
                          alert("Save to Flash command is under process");
                          return;
                        }
                        if(AssetMenuActionSelected == "Streaming Status...") {                         
                            dlgShowDevice_Status(this.SelectedAssetNID, assetOwnerUserJID, assetType1, assetName1, assetStatus1);                        
                        } else if(AssetMenuActionSelected == "Storage Status...") {                         
                            dlgShowStorage_Status(this.SelectedAssetNID, assetOwnerUserJID, assetType1, assetName1, assetStatus1);                        
                        } else if( AssetMenuActionSelected == "Edit Info..." ) {
                          this.SelectedAssetEdit = "EditInfo";
                          dlgEditDeviceInfo_Create(this.SelectedAssetNID, assetType1, assetName1, assetStatus1, assetType);
                        } else if( AssetMenuActionSelected == "Network Configuration..." ) {
                          this.SelectedAssetEdit = "Network Configuration";                        
                          dlgEditDeviceNetwork_Create(this.SelectedAssetNID, assetType1, assetName1, assetStatus1);      
                        } else if( AssetMenuActionSelected == "System Configuration..." ) {
                          this.SelectedAssetEdit = "System Configuration";                        
                          dlgEditDeviceSystem_Create(this.SelectedAssetNID, assetType1, assetName1, assetStatus1);                                              
                        } else if( AssetMenuActionSelected == "Encoder/Decoder Configuration..." ) {
                          this.SelectedAssetEdit = "EditAVS";
                          dlgEditDeviceAV_Create(this.SelectedAssetNID, assetType1, 
                                                    assetName1, assetStatus1, this.SelectedAssetEdit);                         
                        } else if( AssetMenuActionSelected == "Edit Display Profile..."  ) {
                          this.SelectedAssetEdit = "1";
                          if( assetFeature == "" && !jsx3.util.strEmpty(this.currentPortNIDsArray[0]) && jsx3.util.strEmpty(this.currentPortNIDsArray[1])){
                            alert("Operation cannot be performed on the selected device: " + assetName1 + "\n" +
                                  "Device is not an UnManned device." );                            
                            return false;
                          }
                          else if(assetFeature == "UnManned" && !jsx3.util.strEmpty(this.currentPortNIDsArray[0]) && jsx3.util.strEmpty(this.currentPortNIDsArray[1])) {
                            dlgEditClientDeviceAV_Create(this.SelectedAssetNID, assetType1, 
                                                          assetName1, assetStatus1, this.SelectedAssetEdit);
                          }
                          else if(!jsx3.util.strEmpty(this.currentPortNIDsArray[1]) && !jsx3.util.strEmpty(this.currentPortNIDsArray[2])){
                            dlgEditDeviceAV_Create(this.SelectedAssetNID, assetType1, 
                                                      assetName1, assetStatus1, this.SelectedAssetEdit);                         
                          }
                        } else if( AssetMenuActionSelected == "(EAVS)Channel 1..." ) {
                          this.SelectedAssetEdit = "1";
                          dlgEditDeviceAV_Create(this.SelectedAssetNID, assetType1, 
                                                    assetName1, assetStatus1, this.SelectedAssetEdit);                         
                        }else if( AssetMenuActionSelected == "(EAVS)Channel 2..." ) {
                          this.SelectedAssetEdit = "2";
                          dlgEditDeviceAV_Create(this.SelectedAssetNID, assetType1, 
                                                    assetName1, assetStatus1, this.SelectedAssetEdit);                         
                        } else if( AssetMenuActionSelected == "Streaming Configuration..." ) {
                          this.SelectedAssetEdit = "Streaming Configuration";                        
                          dlgEditDeviceStreams_Create(this.SelectedAssetNID, assetType1, assetName1, assetStatus1);
                        } else if( AssetMenuActionSelected == "Shutdown..." ) {
                          this.SelectedAssetCommand = "Shutdown";                    
                          cmpBlkAssetCommand.call(this);                          
                        } else if( AssetMenuActionSelected == "Save To Flash..." ) {
                          this.SelectedAssetCommand = "Save To Flash";                        
                          cmpBlkAssetCommand.call(this);                          
                        } else if( AssetMenuActionSelected == "Reboot..." ) {
                          this.SelectedAssetCommand = "Reboot";                         
                          cmpBlkAssetCommand.call(this);                          
                        } else if( AssetMenuActionSelected == "Revert To Defaults..." ) {
                          this.SelectedAssetCommand = "Revert To Defaults";                         
                          cmpBlkAssetCommand.call(this);                                                                              
                        } else if( AssetMenuActionSelected == "Edit Port(EP)..." ) {
                          this.SelectedAssetEdit = "1";   
                          dlgEditDeviceEditPort_Create(this.SelectedAssetNID, assetType1, assetName1, 
                                                           assetStatus1, this.currentPortNIDsArray[0], this.SelectedAssetEdit, this.isMNADevice);
                        } else if( AssetMenuActionSelected == "(EP)Channel 1..." ) {
                          this.SelectedAssetEdit = "1";                         
                          dlgEditDeviceEditPort_Create(this.SelectedAssetNID, assetType1, assetName1, 
                                                           assetStatus1, this.currentPortNIDsArray[0], this.SelectedAssetEdit, this.isMNADevice);
                        } else if( AssetMenuActionSelected == "(EP)Channel 2..." ) {
                          this.SelectedAssetEdit = "2";                         
                          dlgEditDeviceEditPort_Create(this.SelectedAssetNID, assetType1, assetName1, 
                                                           assetStatus1, this.currentPortNIDsArray[1], this.SelectedAssetEdit, this.isMNADevice);
                        } else if( AssetMenuActionSelected == "(EP)Channel 3..." ) {
                          this.SelectedAssetEdit = "3";                         
                          dlgEditDeviceEditPort_Create(this.SelectedAssetNID, assetType1, assetName1, 
                                                           assetStatus1, this.currentPortNIDsArray[2], this.SelectedAssetEdit, this.isMNADevice);
                        } else if( AssetMenuActionSelected == "(EP)Channel 4..." ) {
                          this.SelectedAssetEdit = "4";                         
                          dlgEditDeviceEditPort_Create(this.SelectedAssetNID, assetType1, assetName1, 
                                                           assetStatus1, this.currentPortNIDsArray[3], this.SelectedAssetEdit, this.isMNADevice);
                        } else if( AssetMenuActionSelected == "(EP)Channel 5..." ) {
                          this.SelectedAssetEdit = "5";                         
                          dlgEditDeviceEditPort_Create(this.SelectedAssetNID, assetType1, assetName1, 
                                                           assetStatus1, this.currentPortNIDsArray[4], this.SelectedAssetEdit, this.isMNADevice);
                        } else if( AssetMenuActionSelected == "(EP)Media Store" ) {
                          this.SelectedAssetEdit = "1";                         
                          dlgEditDeviceEditPort_Create(this.SelectedAssetNID, assetType1, assetName1, 
                                                           assetStatus1, this.SelectedAssetEdit);
                        } else if( AssetMenuActionSelected == "(EP)Media Relay" ) {
                          this.SelectedAssetEdit = "2";                         
                          dlgEditDeviceEditPort_Create(this.SelectedAssetNID, assetType1, assetName1, 
                                                           assetStatus1, this.SelectedAssetEdit);
                        } else if( AssetMenuActionSelected == "NAS Configuration..." ) {
                          this.SelectedAssetEdit = "NAS Configuration";                         
                          dlgEditDeviceEditStorage_Create(this.SelectedAssetNID, assetType1, assetName1, 
                                                           assetStatus1);
                        } else if( AssetMenuActionSelected == "HA Config..." ) {
                          this.SelectedAssetEdit = "HAConfig";                         
                          dlgEditHAConfig_Create(this.SelectedAssetNID, assetType1, assetName1, 
                                                           assetStatus1);
                        } else if( AssetMenuActionSelected == "HA Status..." ) {
                          this.SelectedAssetEdit = "HAStatus";                         
                          dlgHAStatus_Create(this.SelectedAssetNID, assetOwnerUserJID);
                        } else if( AssetMenuActionSelected == "Restore Primary..." ) {
                          this.SelectedAssetCommand = "RestorePrimary";                    
                          cmpBlkAssetCommand.call(this);                          
                        }

                      }
                    );
  
  var cmpMenuAssetList = new CmpMenu("menuAssetList",
                                      null, null, "DeviceMenuListRoot//record[@jsxid='_DEVICETYPE_']", cmpMtxAssetList, 
                                      [{action:"APPEVENT", eventvar:"AssetMenuTypeSelected", eventval:null}],
                                      {action:"APPEVENT", eventvar:"AssetMenuActionSelected", eventval:"jsxtext"}
                                    );

  var cmpBtnDeleteAsset = new CmpImageButton("btnDeleteAsset", null,
                                            { action: "FNCCALL", context: this,
                                              fnc: function () {
                                                var selRecordNodes = cmpMtxAssetList.BlkJSX.getSelectedNodes();
                                                var agree = confirm('Are you sure you want to delete selected Devices?');
                                                cmpMtxAssetList.deleteRequestCounter = 0;
                                                if (agree) {
                                                  for (var it = selRecordNodes.iterator(); it.hasNext(); ) {
                                                    ++cmpMtxAssetList.deleteRequestCounter;
                                                    var selRecordNode = it.next();
                                                    this.SelectedAssetNID = selRecordNode.getAttribute("jsxid");
                                                    cmpBlkAssetDelete.call(this);
                                                  }
                                                }
                                              }
                                            });
                                   
                                   
  var cmpBtnAddAsset = new CmpButton("btnAddAsset",
                                     null, 
                                     {action:"FNCCALL", 
                                      context:this,
                                          fnc:function() {
                                             dlgEditDeviceInfo_Create("DEFAULT_ASSETRESOURCE_NID");
                                          }
                                     });     
                                                                            
  var cmpBtnUpgradeDevice = new CmpImageButton("btnUpgradeDevice",
                                               null, 
                                               {action:"FNCCALL", 
                                                context:this,
                                                fnc:function() {
                                                    // For Selected Devices (upgrade btn enabled only for selected ie. Not Offline,Not Unammanged and not WebClients),formulate upgrade req
                                                    var selRecordNodes = cmpMtxAssetList.BlkJSX.getSelectedNodes();
                                                    cmpMtxAssetList.upgradeRequestCounter = 0;
                                                    for (var it = selRecordNodes.iterator(); it.hasNext(); ) {
                                                      ++cmpMtxAssetList.upgradeRequestCounter;
                                                      var selRecordNode = it.next();
                                                      this.SelectedAssetNID = selRecordNode.getAttribute("jsxid");
                                                      sendUpgradeDeviceRequest.call(this);
                                                    }
                                                }
                                               });

  var cmpBlkAddDAsset = new CmpContainer("blkAddAsset",
                                        [{ cmp: cmpBtnAddAsset}],
                                        null,
                                        null);
                                                                                      
  var cmpBlkDeleteAsset = new CmpContainer("blkDeleteAsset",
                                            [{ cmp: cmpBtnDeleteAsset,
                                                sel: "(clsUserPermission.getPermission('Asset', 'delete') == 'true') && " +
                                                    "(topic_Get(tApp, 'AssetDeviceStatus') != null && " +
                                                    "topic_Get(tApp, 'AssetDeviceStatus') != 'NONE' && " +
                                                    "(topic_Get(tApp, 'AssetDeviceStatus') == 'Offline' || " +
                                                    "topic_Get(tApp, 'AssetDeviceStatus') == 'Unmanaged' || " +
                                                    "topic_Get(tApp, 'AssetDeviceStatus') == 'NeedsUpgrade' || " +
                                                    "topic_Get(tApp, 'AssetDeviceStatus') == 'Template') && " +
                                                    //"topic_Get(tApp, 'AssetDeviceStatus') != 'NeedsUpgrade' && " + //#35723. Unble to delete the Device, when the device has upgrade image in status column
                                                    "topic_Get(tApp, 'AssetDeviceStatus') != 'Ready')"}],
                                            null,
                                            null);

  var cmpBlkUpgradeDevice = new CmpContainer("blkUpgradeAsset",
                                            [{ cmp: cmpBtnUpgradeDevice,
                                               sel: "topic_Get(tApp, 'AssetUpgradeDeviceStatus') != null && " +
                                                    "topic_Get(tApp, 'AssetUpgradeDeviceStatus') != 'NONE' && " +
                                                    "(topic_Get(tApp, 'AssetUpgradeDeviceStatus') != 'Offline' && " +
                                                    "topic_Get(tApp, 'AssetUpgradeDeviceStatus') != 'Unmanaged' && " +
                                                    "topic_Get(tApp, 'AssetUpgradeDeviceStatus') != 'Template') && " +
                                                    "(topic_Get(tApp, 'AssetUpgradeDeviceStatus') == 'Ready' ||" +
                                                    "topic_Get(tApp, 'AssetUpgradeDeviceStatus') == 'NeedsUpgrade')"}],
                                            null,
                                            null);
                                                                                                                                                                            
  //  cmpBtnDeleteAsset and cmpBtnUpgradeDevice listen to Publish indicating if the selcted device are deletable and selectable                                     
  var cmpBlkDeviceList = new CmpContainer("blkAssets", 
                                      [{ cmp: cmpBlkAddDAsset,
                                         sel: "clsUserPermission.getPermission('Asset', 'add') == 'true'"},
                                       { cmp: cmpBlkDeleteAsset,
                                         sel: "clsUserPermission.getPermission('Asset', 'delete') == 'true'"},
                                       {cmp:cmpMenuAssetList},
                                       {cmp:cmpAssestNavControls},
                                       {cmp:cmpBlkUpgradeDevice}                                      
                                      ],
                                      [{ action: "APPEVENT", eventvar: "AssetUpgradeDeviceStatus", eventval: null },
                                      { action: "APPEVENT", eventvar: "AssetDeviceStatus", eventval: null }
                                      ],
                                      null );
  
  var cmpBlkDevice = new CmpContainer("blkAssets",
                                      [{ cmp: cmpMtxAssetList },
                                       {cmp:cmpBlkDeviceList},
                                       {cmp:cmpBtnAssetCustomize},
                                       {cmp:cmpBlkAssetCustomize},
                                       {cmp:cmpBtnAssetFilter},
                                       {cmp:cmpBlkAssetFilter}                                     
                                      ],
                                      null,
                                      null );
                      
  eventApp.subscribe("MENUUIDATA", "DEVICE", this,
                      function(EventVar, EventVal) {
                        if( EventVal == "DEVICE" ) {
                          var getDataState = topic_Get(tApp, "AssetResourceGetDataState");
                          if(getDataState == "NONE") {
                            setObjectCacheDefaultXML("Asset", "JSXAPPS/IPVS-Common/model/objectXml/AssetResource.xml", "DEFAULT_ASSETRESOURCE_NID"); 
                            setObjectCacheDefaultXML("MediaStreamSrcPortResource", "JSXAPPS/IPVS-Common/model/objectXml/MediaStreamSrcPortResource.xml", "DEFAULT_MEDIASTREAMSRCPORTRESOURCE_NID");
                            setObjectCacheDefaultXML("MediaStreamDstPortResource", "JSXAPPS/IPVS-Common/model/objectXml/MediaStreamDstPortResource.xml", "DEFAULT_MEDIASTREAMDSTPORTRESOURCE_NID");
                            setObjectCacheDefaultXML("MediaStorePortResource", "JSXAPPS/IPVS-Common/model/objectXml/MediaStorePortResource.xml", "DEFAULT_MEDIASTOREPORTRESOURCE_NID");
                            setObjectCacheDefaultXML("MediaStreamRelayPortResource", "JSXAPPS/IPVS-Common/model/objectXml/MediaStreamRelayPortResource.xml", "DEFAULT_MEDIASTREAMRELAYPORTRESOURCE_NID");
                            setObjectCacheDefaultXML("MediaStreamIOPortResource", "JSXAPPS/IPVS-Common/model/objectXml/MediaStreamIOPortResource.xml", "DEFAULT_MEDIASTREAMIOPORTRESOURCE_NID");
                            /*setObjectCacheDefaultXML("Device", "JSXAPPS/IPVS-Common/model/objectXml/V2D-Tx-Device.xml", "DEFAULT_V2D-TX-DEVICE_NID");
                            setObjectCacheDefaultXML("Device", "JSXAPPS/IPVS-Common/model/objectXml/V2D-Rx-Device.xml", "DEFAULT_V2D-RX-DEVICE_NID");
                            setObjectCacheDefaultXML("Device", "JSXAPPS/IPVS-Common/model/objectXml/V2D-XP200-Device.xml", "DEFAULT_V2D-XP200-DEVICE_NID");
                            setObjectCacheDefaultXML("Device", "JSXAPPS/IPVS-Common/model/objectXml/V2D-XP200S-Device.xml", "DEFAULT_V2D-XP200S-DEVICE_NID");
                            setObjectCacheDefaultXML("Device", "JSXAPPS/IPVS-Common/model/objectXml/V2D-XP220-Device.xml", "DEFAULT_V2D-XP220-DEVICE_NID");
                            setObjectCacheDefaultXML("Device", "JSXAPPS/IPVS-Common/model/objectXml/V2D-XP220S-Device.xml", "DEFAULT_V2D-XP220S-DEVICE_NID");
                            setObjectCacheDefaultXML("Device", "JSXAPPS/IPVS-Common/model/objectXml/V2D-XP100-Device.xml", "DEFAULT_V2D-XP100-DEVICE_NID");
                            setObjectCacheDefaultXML("Device", "JSXAPPS/IPVS-Common/model/objectXml/V2D-XPi-Device.xml", "DEFAULT_V2D-XPi-DEVICE_NID");
                            setObjectCacheDefaultXML("Device", "JSXAPPS/IPVS-Common/model/objectXml/DMS-100-Device.xml", "DEFAULT_DMS-100-DEVICE_NID");
                            setObjectCacheDefaultXML("Device", "JSXAPPS/IPVS-Common/model/objectXml/DMS-200-Device.xml", "DEFAULT_DMS-200-DEVICE_NID");
                            setObjectCacheDefaultXML("Device", "JSXAPPS/IPVS-Common/model/objectXml/DMS-1000-Device.xml", "DEFAULT_DMS-1000-DEVICE_NID");
                            setObjectCacheDefaultXML("Device", "JSXAPPS/IPVS-Common/model/objectXml/DMS-1200-Device.xml", "DEFAULT_DMS-1200-DEVICE_NID");
                            setObjectCacheDefaultXML("Device", "JSXAPPS/IPVS-Common/model/objectXml/DMS-1500-Device.xml", "DEFAULT_DMS-1500-DEVICE_NID");
                            setObjectCacheDefaultXML("Device", "JSXAPPS/IPVS-Common/model/objectXml/DMS-1600-Device.xml", "DEFAULT_DMS-1600-DEVICE_NID");
                            setObjectCacheDefaultXML("Device", "JSXAPPS/IPVS-Common/model/objectXml/DMS-1700-Device.xml", "DEFAULT_DMS-1700-DEVICE_NID");
                            setObjectCacheDefaultXML("Device", "JSXAPPS/IPVS-Common/model/objectXml/DMS-25-Device.xml", "DEFAULT_DMS-25-DEVICE_NID");
                            setObjectCacheDefaultXML("Device", "JSXAPPS/IPVS-Common/model/objectXml/RTP-Tx-Device.xml", "DEFAULT_RTP-TX-DEVICE_NID");
                            setObjectCacheDefaultXML("Device", "JSXAPPS/IPVS-Common/model/objectXml/RTP-Rx-Device.xml", "DEFAULT_RTP-RX-DEVICE_NID");
                            setObjectCacheDefaultXML("Device", "JSXAPPS/IPVS-Common/model/objectXml/MPEGTS-Tx-Device.xml", "DEFAULT_MPEGTS-TX-DEVICE_NID");
                            setObjectCacheDefaultXML("Device", "JSXAPPS/IPVS-Common/model/objectXml/MPEGTS-Rx-Device.xml", "DEFAULT_MPEGTS-RX-DEVICE_NID");
                            setObjectCacheDefaultXML("Device", "JSXAPPS/IPVS-Common/model/objectXml/UDP-Tx-Device.xml", "DEFAULT_UDP-TX-DEVICE_NID");
                            setObjectCacheDefaultXML("Device", "JSXAPPS/IPVS-Common/model/objectXml/UDP-Rx-Device.xml", "DEFAULT_UDP-RX-DEVICE_NID");
                            setObjectCacheDefaultXML("Device", "JSXAPPS/IPVS-Common/model/objectXml/MS-Device.xml", "DEFAULT_DEVICE_MS_NID");
                            setObjectCacheDefaultXML("Device", "JSXAPPS/IPVS-Common/model/objectXml/MS-25-Device.xml", "DEFAULT_DEVICE_MS-25_NID");
                            setObjectCacheDefaultXML("Device", "JSXAPPS/IPVS-Common/model/objectXml/IPVSClient-Device.xml", "DEFAULT_IPVSClient-Device_NID");
                            setObjectCacheDefaultXML("Device", "JSXAPPS/IPVS-Common/model/objectXml/IPVSClient-Device.xml", "DEFAULT_DEVICE_TEMPLATE_NID");*/
                            topic_Set(tApp, "EditAssetDataState", "DEFAULT_ASSETRESOURCE_NID");
                                                    
                            topic_Set(tApp, "AssetResourceGetDataState", (newServerDate()).getTime() );
                            cmpMtxAssetList.ResetToStart(1,AssetCountToFetch,0);
                            topic_Publish(tApp, "AssetResourceGetData", (newServerDate()).getTime() ); 
                            //topic_Publish(tApp, "AssetColumnGetData", (newServerDate()).getTime() ); 
                          }
                        }
                      }
                    );   

  eventApp.subscribe("AssetSelected", null, this,
                    function() {
                      cmpBlkAssetSelection();
                    }
                  );

  function cmpBlkAssetSelection() {
    //Selection of Delete Button : Delete not Allowed for Devices that are Ready or in NeedsUpgarde State
     var deviceStatus = "Offline";
     var isDeviceSelected = false;
     var upgradeDeviceStatus = "Ready";
     var isUpgradeDeviceSelected = false;
     var currSoftwareVersion = topic_Get(tApp, "MSSoftwareVersion");
     var currDeviceSoftwareVersion = topic_Get(tApp, "MsDeviceSoftwareVersion");
     var MenuSelection = topic_Get(tApp, 'MenuSelection');
     if(MenuSelection == "DEVICE" ) {
       var selRecordNodes = cmpMtxAssetList.BlkJSX.getSelectedNodes();
       for (var it = selRecordNodes.iterator(); it.hasNext(); ) {
         var selRecordNode = it.next();
         isDeviceSelected = true;
         this.SelectedAssetNID = selRecordNode.getAttribute("jsxid");
         var assetStatus = selRecordNode.getAttribute("assetStatusOrginal");
         var templateAssetResourceNID = selRecordNode.getAttribute("jsxTemplateAssetResourceNID");
         var selectedDeviceTitle = selRecordNode.getAttribute("jsxtext");
         deviceStatus = assetStatus;
         if(assetStatus == "Template"){
           var strXMLCacheId = cmpMtxAssetList.BlkJSX.getXMLId();
           var objXMLNode = PNameSpace.getCache().getDocument(strXMLCacheId);
           var templateAssetResourceNode = objXMLNode.selectSingleNode(".//record[@jsxTemplateAssetResourceNID='"+ this.SelectedAssetNID +"']");
           if(!jsx3.util.strEmpty(templateAssetResourceNode)){
             deviceStatus = "InUse";
             break;
           }
         }
         else if (!jsx3.util.strEmpty(templateAssetResourceNID)) {
           deviceStatus = "InUse";
           break;
         }

         if((assetStatus != "Offline") &&
            (assetStatus != "Unmanaged") &&
            (assetStatus != "Template")){
           deviceStatus = assetStatus;
           break;
         }
       }

       if (selectedDeviceTitle == "web-client") {
         deviceStatus = "NONE";
       }
       //Selection of Upgrade Button : Upgrades not Allowed for Devices that are Offline,Unmanaged, Webclients and same software version as MSSoftwareVersion
       for (var it = selRecordNodes.iterator(); it.hasNext(); ) {
         var selRecordNode = it.next();
         isUpgradeDeviceSelected = true;
         var assetType = selRecordNode.getAttribute("assetTypeOrginal");
         var assetStatus = selRecordNode.getAttribute("assetStatusOrginal");
         var assetMSStatus = selRecordNode.getAttribute("assetMSStatusOriginal");
         var assetSoftwareVersion = selRecordNode.getAttribute("deviceSWVersion");
         var templateAssetResourceNID = selRecordNode.getAttribute("jsxTemplateAssetResourceNID");
         upgradeDeviceStatus = assetStatus;
         var clientDeviceTypeNode = deviceTypeListNode.selectSingleNode(".//device[@jsxid='Client']/record[@jsxid='"+ assetType +"']");
         var clientDeviceName = "";
         if(!jsx3.util.strEmpty(clientDeviceTypeNode)){
           clientDeviceName = clientDeviceTypeNode.getAttribute("jsxtext");
         }
         if(clientDeviceName == "Media Client"){
           upgradeDeviceStatus = "NONE";
           break;
         }
         else if((assetStatus != "Ready") &&
            (assetStatus != "NeedsUpgrade")){
           upgradeDeviceStatus = assetStatus;
           break;
         }
         else if(assetMSStatus =="standalone" || assetMSStatus =="primary-active"){
           upgradeDeviceStatus = "NONE";
           break;
         }
         else if (assetStatus == "Ready" && assetType != "V2D-XPi-Device" && currSoftwareVersion == assetSoftwareVersion) {
           upgradeDeviceStatus = "NONE";
           break;
         }
         else if (assetStatus == "Ready" && assetType == "V2D-XPi-Device" && currDeviceSoftwareVersion == assetSoftwareVersion) {
           upgradeDeviceStatus = "NONE";
           break;
         }
         else if (!jsx3.util.strEmpty(templateAssetResourceNID)) {
           upgradeDeviceStatus = "NONE";
           break;
         }
       }
       //Publish for hide show of Upgrade,Delete btns
       if(isDeviceSelected){
         topic_Publish(tApp, "AssetDeviceStatus", deviceStatus);
       }
       else{
         topic_Publish(tApp, "AssetDeviceStatus", "NONE");
       }
       if(isUpgradeDeviceSelected){
         topic_Publish(tApp, "AssetUpgradeDeviceStatus", upgradeDeviceStatus);
       }
       else{
         topic_Publish(tApp, "AssetUpgradeDeviceStatus", "NONE");
       }
     }
  }

  function compareVersion(str1, str2) {
    //str2: "<span class='swUpgrade'>4.0.r9.b81407</span>"
    if(!jsx3.util.strEmpty(str1) && !jsx3.util.strEmpty(str2)){
      var strArr1 = str1.split('.');
      var strArr2 = str2.match(/\d{1,2}\.\d{1,3}\.r\d{1,3}\.b\d{1,}/);
      if (strArr1.length == strArr2.length) return false;
      for (var i = 0; i < strArr1.length; i++) {
        if (strArr1[i] != strArr2[i]) {
          return false;
        }
      }
      return true; 
    }
    else {
      return str1.trim() == str2.trim();
    }
  
  }

  eventApp.subscribe("DBUpdate", null, this,
                     function(EventVar, EventVal) {
                       if( jsx3.util.strEmpty(EventVal) ) return;
                       if( EventVal.objType == "AssetResource" ) {
                         cmpMtxAssetList.handleNotification(EventVal);
                       }
                       if (EventVal.objType == "GroupTagDef") {
                         cmpAssetFilter.handleNotification(EventVal);
                       }                                   
                     });
  
  eventApp.subscribe("AssetDataUpdate", null, this,
                      function(EventVar, EventVal) {
                        if( EventVal == "ADD" || EventVal == "DELETE" ||  EventVal == "EDIT") {
                          //cmpMtxAssetList.getColumnsFromDB();
                          cmpMtxAssetList.doRepaint();
                        }
                      }
                    );                                                           
      
  return cmpBlkDevice; 
  
  
  function cmpBlkAssetDelete() {
    // delete Device
    var reqDeleteDevice = new ReqSessionGetRequest("DeleteDevice",
                                                  { action: "FNCCALL", context: null,
                                                    fnc: function (respCode, respVal) {
                                                      if (respCode == "SUCCESS" && respVal != null) {
                                                        --cmpMtxAssetList.deleteRequestCounter < 0 ? 0 : cmpMtxAssetList.deleteRequestCounter;
                                                        topic_Publish(tApp, "AssetDeviceStatus", "NONE");
                                                        if (cmpMtxAssetList.deleteRequestCounter == 0) {
                                                          topic_Publish(tApp, "AssetDataUpdate", "DELETE");
                                                        }
                                                      } else {
                                                        --cmpMtxAssetList.deleteRequestCounter < 0 ? 0 : cmpMtxAssetList.deleteRequestCounter;
                                                        var codeErrorMsg = CommonErrorCodeArr[respCode].errorDesc;
                                                        var ErrorMsg = "Error: Device->Asset delete request failed \n";
                                                        ErrorMsg += "Error description: " + codeErrorMsg + "\n";
                                                        ErrorMsg += "Error code: " + respCode;
                                                        alert(ErrorMsg);
                                                      }
                                                    }
                                                  }
                                                 );
                                           
    reqDeleteDevice.ServiceName = "SessionService";
    
    var reqParams = new Object;
    reqParams["ObjectList_WhereNID"] = this.SelectedAssetNID;
    reqDeleteDevice.reqParams = reqParams;
    
    // send the request
    reqDeleteDevice.sendRequest(this);        
    
  } // end cmpBlkAssetDelete
  
  function getDeviceCommandType(SelectedAssetType, AssetCommand) {
    var DeviceCommandNodeType = "";
    switch(SelectedAssetType) {
      case "MS-Device":
        DeviceCommandNodeType = "MS-DeviceCommand" + AssetCommand;
        break;
      default:
        return;
    }    
    return DeviceCommandNodeType;
  } // end getDeviceCommandType
  
  function getDeviceCommandNodeType(selectedAssetCommand) {
    var DeviceCommandString = "";
    switch(selectedAssetCommand) {
      case "Shutdown": 
        DeviceCommandString = "ShutdownDevice";
        break;
      case "Save To Flash":
        DeviceCommandString = "SaveConfigDevice";
        break;
      case "Reboot":
        DeviceCommandString = "RebootDevice";
        break;
      case "Revert To Defaults":
        DeviceCommandString = "RevertToDefaultDevice";
        break;
      case "RestorePrimary":
        DeviceCommandString = "HATakeoverDevice";
        //DeviceCommandString = "<HATakeoverCommand></HATakeoverCommand>";
        break;
      default:
        return;
    }       
    
    return DeviceCommandString;
  } // end getDeviceCommandNodeType
  
  function cmpBlkAssetCommand() {
    var selRecord = "";
    var SelectedAssetName = "";
    var SelectedAssetType = "";
    var SelectedAssetStatus = "";
    var SelectedAssetAgentJID = "";
    
    var selectedAssetNID = this.SelectedAssetNID;
    var selectedAssetCommand = this.SelectedAssetCommand;
    var selectedAssetStatus = this.SelectedAssetStatus;
    
    function cmpBlkAssetProcessCommand(){
      SelectedAssetAgentJID =  selRecord.assetContactOwnerJID;
      if( SelectedAssetAgentJID != null ) {
        var deviceCommandString = getDeviceCommandNodeType(selectedAssetCommand);
        
        if(selectedAssetCommand == "Save To Flash"){
          topic_Set(SelectedAssetAgentJID, "DeviceSaveToFlashCommand", "PROCESS");
        }
        var reqDeviceCommand = new ReqSessionGetRequest(deviceCommandString,
                                                        { action:"FNCCALL", context:this,
                                                          fnc:function(respCode, respVal) {
                                                                if(respCode == "SUCCESS" && respVal != null) {
                                                                  if(selectedAssetCommand == "Save To Flash" ){
                                                                    topic_Set(SelectedAssetAgentJID, "DeviceSaveToFlashCommand", "SUCCESS");
                                                                  }
                                                                } else {
                                                                  var codeErrorMsg = CommonErrorCodeArr[respCode].errorDesc;
                                                                  var ErrorMsg = "Error: Device->" + deviceCommandString + " request failed \n";
                                                                  ErrorMsg += "Error description: " + codeErrorMsg + "\n";
                                                                  ErrorMsg += "Error code: " + respCode;
                                                                  alert(ErrorMsg);
                                                                }
                                                              }
                                                        });
        
        reqDeviceCommand.ServiceName = "SessionService";
        if (selectedAssetCommand != "RestorePrimary") {
          var reqParams = new Object;
          reqParams["ObjectList_ByNID"] = selectedAssetNID;
          reqParams["ObjectList_ByTitle"] = SelectedAssetName;
          reqDeviceCommand.reqParams = reqParams;
        }
        reqDeviceCommand.sendRequest();
      }
    } // end cmpBlkAssetProcessCommand
    
    // main code here
    if( selectedAssetNID != null ) {
      selRecord = cmpMtxAssetList.BlkJSX.getRecord(selectedAssetNID);
      SelectedAssetName = selRecord.jsxtext;
      SelectedAssetType = selRecord.assetTypeOrginal;
      SelectedAssetStatus = selRecord.assetStatusOrginal;
      if(SelectedAssetStatus != "Ready") {
        SelectedAssetStatus = "NotReady/Offline";
      } 

      //Handle RestorePrimary command for MS-Device
      if(selectedAssetCommand == "RestorePrimary" && SelectedAssetStatus == "Ready") {
        if( selRecord.assetMSStatusOriginal != "primary-standby" ) {
          alert("Operation cannot be performed on the selected device: " + SelectedAssetName + "\n" +
                "The management server status is " + selRecord.assetMSStatusOriginal + " not primary-standby");
          return false;    
        }
      } 
      
      if( SelectedAssetStatus == "Ready") {
        var agree = confirm('Are you sure you want to ' + selectedAssetCommand + ' device: "'+ SelectedAssetName + '" ? ');
        if(agree) {
          cmpBlkAssetProcessCommand();
        }
      } else {
        alert("Operation cannot be performed on the selected device: " + SelectedAssetName + "\n" +
              "Device status is " + SelectedAssetStatus
             );
        return false;        
      }
    } else {
      alert("Please select device");
      return false;
    } 
    
  } // end cmpBlkAssetCommand
  
  //Send Upgrade Device Request for Devices which are upgradable and selected by the user
  function sendUpgradeDeviceRequest(){
    var reqUpgradeDevice = new ReqSessionGetRequest("UpgradeDevice",
                                                   { action: "FNCCALL", context: this,
                                                     fnc: function (respCode, respVal) {
                                                       if (respCode == "SUCCESS" && respVal != null) {
                                                         --cmpMtxAssetList.upgradeRequestCounter < 0 ? 0 : cmpMtxAssetList.upgradeRequestCounter;
                                                         if (cmpMtxAssetList.upgradeRequestCounter == 0) {
                                                           //Disable upgrade button 
                                                           topic_Publish(tApp, "AssetUpgradeDeviceStatus", "NONE");
                                                         }
                                                       } else {
                                                         --cmpMtxAssetList.upgradeRequestCounter < 0 ? 0 : cmpMtxAssetList.upgradeRequestCounter;
                                                         log.error("UpgradeDeviceRequest Failed : error:" + respVal);
                                                         //TBD later to handle multiple responses. Currently just loggin the request.
                                                         var codeErrorMsg = "Unkown Error";
                                                         var description = '';
                                                         if (respVal.match(/ResponseXML/)) {
                                                           var ResponseXML = (respVal.replace(/\n/g, ' ')).replace(/.*ResponseXML:(.*)/, "$1");
                                                           var responseDoc = new jsx3.xml.Document;
                                                           responseDoc.loadXML(ResponseXML);
                                                           try {
                                                             description = responseDoc.selectSingleNode(".//Description").getValue();
                                                           } catch (e) {
                                                             log.error("Upgrade MS Request failed.Error : no error description" + e);
                                                           }
                                                         }
                                                         if (description != "Unkown Error") {
                                                           codeErrorMsg = description;
                                                         }
                                                         else {
                                                           var objError = CommonErrorCodeArr[respCode];
                                                           if (!jsx3.util.strEmpty(objError)) {
                                                             codeErrorMsg = objError.errorDesc;
                                                           }
                                                         }
                                                         var ErrorMsg = "Error:UpgradeDevice request failed \n";
                                                         ErrorMsg += "Error description: " + codeErrorMsg + "\n";
                                                         ErrorMsg += "Error code: " + respCode;
                                                         alert(ErrorMsg);
                                                       }
                                                     }
                                                   }
                                               );
      reqUpgradeDevice.ServiceName = "SessionService";
      var reqParams = new Object;
      reqParams["ObjectList_ByNID"] = this.SelectedAssetNID;
      reqParams["ObjectList_ByTitle"] = "";
      reqUpgradeDevice.reqParams = reqParams;
      reqUpgradeDevice.sendRequest();
  }
  
  function checkMSDevice(a_NID, a_Msg) {
    var assetNode1 = topic_GetNode(tCache, "//*[@NID='" + a_NID + "']");
    var assetName1 = assetNode1.selectSingleNode(".//Info/Title").getValue();
    var assetType1 = assetNode1.selectSingleNode(".//Info/Type").getValue();
    if(assetType1 == "MS-Device") {
      alert(a_Msg + assetName1);
      return false;
    }
    return true;
  }

} // end create
       
function cmpBlkDevice_Destroy() {
  var blkAssetsBlkJSX = PNameSpace.getJSXByName("blkAssets");
  blkAssetsBlkJSX.removeChildren(); 
}  
  
function convertDeviceMemoryToMB(element, cdfkey, matrix, column, rownumber, server) {
  element.innerHTML = convertBytesToMegaBytes(element.innerText);
}

function deviceConnectionStatus(element, cdfkey, matrix, column, rownumber, server) {
  var deviceConnStatus = jsx3.util.strTrim(element.innerText);
  if(deviceConnStatus == "True" ){
    deviceConnStatus = "Reachable";
  }
  else if( deviceConnStatus == "False" ){
    deviceConnStatus = "Unreachable";
  }
   
   element.innerHTML = deviceConnStatus;
}

function deviceHAStatus(element, cdfkey, matrix, column, rownumber, server) {
  var deviceHAStatus = jsx3.util.strTrim(element.innerText);
  if(deviceHAStatus == "True" ){
    deviceHAStatus = "Up";
  }
  else if( deviceHAStatus == "False" ){
    deviceHAStatus = "Down";
  } 
    
  if( deviceHAStatus == "Up" || deviceHAStatus == "Down"){
    element.innerHTML = deviceHAStatus;
  }
}

function showDeviceErrorDetails(objJSX, strRecordId){
  var ObjRecord = objJSX.getRecord(strRecordId);
  if( jsx3.util.strEmpty(ObjRecord) ) return;
  var assetStatus = ObjRecord.assetStatusOrginal;
  var deviceStatusImg = ObjRecord.jsximgDeviceStatus;
  if( ( (assetStatus == "Ready") && (deviceStatusImg == "images/icons/icon_orangeBall.png" || deviceStatusImg == "images/icons/icon_redBall.png" ) ) ||
      ( (assetStatus == "NeedsUpgrade") && (deviceStatusImg == "images/icons/icon_orangeBall.png" || deviceStatusImg == "images/icons/icon_redBall.png" ) ) 
    ) {
    var assetOwnerUserJID = ObjRecord.assetContactOwnerJID;
    var deviceStatusNode = topic_GetNode(assetOwnerUserJID, ".//DeviceStatus");

    var SystemElementsStatusNode = deviceStatusNode.selectSingleNode(".//SystemElementsStatus");
    var SystemElementsStatusVal = "Ok";
    if( !jsx3.util.strEmpty(SystemElementsStatusNode) ){
      SystemElementsStatusVal = SystemElementsStatusNode.getAttribute("State");
    }
    var ServiceElementsStatusNode = deviceStatusNode.selectSingleNode(".//ServiceElementsStatus");
    var ServiceElementsStatusNodeVal = "Ok";
    if( !jsx3.util.strEmpty(ServiceElementsStatusNode) ){
      ServiceElementsStatusNodeVal = ServiceElementsStatusNode.getAttribute("State");
    }    
    
    var NetworkElementsStatusNode = deviceStatusNode.selectSingleNode(".//NetworkElementsStatus");
    var NetworkElementsStatusNodeVal = "Ok";
    if( !jsx3.util.strEmpty(NetworkElementsStatusNode) ){
      NetworkElementsStatusNodeVal = NetworkElementsStatusNode.getAttribute("State");
    } 
    
    var HardDiskTableStatusNode = deviceStatusNode.selectSingleNode(".//HardDiskTableStatus");
    var HardDiskTableStatusNodeVal = "Ok";
    if( !jsx3.util.strEmpty(HardDiskTableStatusNode) ){
      HardDiskTableStatusNodeVal = HardDiskTableStatusNode.getAttribute("State");
    }
    
    var NASStatusNode = deviceStatusNode.selectSingleNode(".//NASStatus");
    var NASStatusNodeVal = "Ok";
    if( !jsx3.util.strEmpty(NASStatusNode) ){
      NASStatusNodeVal = NASStatusNode.getAttribute("State");
    }

    var strSpyHTML = '<div id="outerDiv" style="margin: -4px; background-color: #ffffcc;">';
    strSpyHTML = strSpyHTML + '<table width="200" height="100" border="0">';
    strSpyHTML = strSpyHTML + '<tr><td style="font-family:Arial;font-size:10px;"><b>System Status: </B></td><td style="font-family:Arial;font-size:10px;">' + SystemElementsStatusVal + '</td></tr>';
    strSpyHTML = strSpyHTML + '<tr><td style="font-family:Arial;font-size:10px;"><b>Service Status: </B></td><td style="font-family:Arial;font-size:10px;">' + ServiceElementsStatusNodeVal + '</td></tr>';
    strSpyHTML = strSpyHTML + '<tr><td style="font-family:Arial;font-size:10px;"><b>Network Status: </B></td><td style="font-family:Arial;font-size:10px;">' + NetworkElementsStatusNodeVal + '</td></tr>';
    strSpyHTML = strSpyHTML + '<tr><td style="font-family:Arial;font-size:10px;"><b>HardDisk Status: </B></td><td style="font-family:Arial;font-size:10px;">' + HardDiskTableStatusNodeVal + '</td></tr>';
    strSpyHTML = strSpyHTML + '<tr><td style="font-family:Arial;font-size:10px;"><b>NAS Status: </B></td><td style="font-family:Arial;font-size:10px;">' + NASStatusNodeVal + '</td></tr></table></div>';
    return strSpyHTML;
  }
  else if( (assetStatus == "NotReady") ) {
    var strSpyHTML = '<div id="outerDiv" style="margin: -4px; background-color: #ffffcc;">';
    strSpyHTML = strSpyHTML + '<table width="200" hight="200" border="0">';
    strSpyHTML = strSpyHTML + '<tr><td style="font-family:Arial;font-size:10px;">Device Status: </td><td style="font-family:Arial;font-size:10px;">Not Ready</td></tr></table></div>';
    return strSpyHTML;  
  }
  return;
} // end showDeviceErrorDetails