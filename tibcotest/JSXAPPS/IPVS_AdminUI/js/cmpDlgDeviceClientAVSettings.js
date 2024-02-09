// this code has some potential issues 
// - w.r.t object reference counts - when objects created in here go out of scope etc
// - loading sync issues - dlg may not be loaded before the object init methods are called

function dlgEditClientDeviceAV_Create(AssetNID, selectedAssetType, selectedAssetName, selectedAssetStatus, selectedPort) {
  
  var DeviceNID = "";
  var DeviceResourceNIDArr = [];
  
  closeDialogIfExists("DialogAVSettings");
  // load view - dialog component file
  var DlgFile = "components/assets/assetClientAVSettingsDialog.xml";    
  var BlkJSX = PNameSpace.getBodyBlock().loadAndCache(DlgFile);
  
  var tDlg = "Dialog_" + jsx3.xml.CDF.getKey()
  topic_Create(tDlg);
  
  // setup a listner for it    
  var eventDlg = new EventListner(tDlg);
  
  function dlgEditAsset_Destroy() {
    // destroy any local eventlistners
    eventDlg.destroy();
    
    // destroy all local components
    cmpDlgEditAsset.destroy();

    // destroy the local topic                  
    topic_Destroy(tDlg);
    
    // close dialog    
    BlkJSX.doClose();
  }

  topic_Set(tDlg, "DisplayProfileGetData", "NONE" );
  topic_Set(tDlg, "DisplayProfileWhereXPath", "");
  
  var cmpCDFEditAssetAVSettings = new Component([BlkJSX,"cdfDlgAssetAVSettings"]);

  
  
  var cmpSelDisplayProfileList = new CmpCustomPaginationList([BlkJSX,"selDisplayProfileList"],
                                                             [{action:"TOPICEVENT", topic:tDlg, eventvar:"DisplayProfileGetData", eventval:null}],
                                                             {persist:{col:"jsxid",initval:false},
                                                              action:"TOPICEVENT", topic:tDlg, eventvar:"DisplayProfileSelected", eventval:"jsxid"},
                                                             null,
                                                             null,
                                                             null
                                                             );   
  
  //translate($b/Info/Title ,'ABCDEFGHIJKLMNOPQRSTUVWXYZ','abcdefghijklmnopqrstuvwxyz')
  cmpSelDisplayProfileList.SortPathArr = {jsxtext:"upper-case($b/Title)",
                                          jsxPriority:"$b/Priority/number(.)",
                                          jsxDescription:"upper-case($b/Description)"
                                         };
                                     
  cmpSelDisplayProfileList.cmpReqParamArr = { StartFrom: "Op_Select_ListSpec_StartFrom",
                                              CountToFetch: "Op_Select_ListSpec_CountToFetch",
                                              SortOrder: "Op_Select_ListSpec_SortOrder",
                                              ObjectCount: "ObjectCount",
                                              SortByXpath: "Op_Select_ListSpec_SortByXPathList_SortByXpath"
                                            };                                  
                                  
  cmpSelDisplayProfileList.DataUpdatedEvent = {action:"TOPICEVENT", topic:tDlg, eventvar:"DisplayProfileDataUpdated", eventval:"LOADED"};                       

  cmpSelDisplayProfileList.cmpReq = new cmpReq_GetListReadQuery(tDlg, "JSXAPPS/IPVS_AdminUI/query/DisplayProfile.xml",
                                                               [{VarName:"_WHEREXPATH_", VarRef:"DisplayProfileWhereXPath"}
                                                               ],
                                                               {action: "FNCCALL", context: cmpSelDisplayProfileList,
                                                                fnc: function(respCode, respVal) {
                                                                       if(respCode == "SUCCESS" && respVal != null) {
//                                                                         var defRecord = '<record jsxid="NONE" jsxtext="Select Display Profile" jsxselected="1"/>';
//                                                                         var defRecordDoc = new jsx3.xml.Document();
//                                                                         var defRecordNode = defRecordDoc.loadXML(defRecord.toString());
//                                                                         respVal.appendChild(defRecordNode);
                                                                         this.onResponse(respCode, respVal);
                                                                         this.repaint();
                                                                       } else {
                                                                         log.info("GetDisplayProfileRequest failed. Error Code :" + respCode);
                                                                         this.onResponseErrorHandle("GetDisplayProfile",respCode, respVal);
                                                                       }                                                                       
                                                                     }
                                                               } 
                                                             );  
  
  
  
  var cmpDisplayProfileSetting = new CmpCanvasEditorContainer([BlkJSX,"blkDisplayProfileCanvasSetup"],
                                                              [[BlkJSX,"BlkDisplayProfileCanvas"],
                                                               [BlkJSX,"masterWindowCDF"],
                                                               [BlkJSX,"WindowList"],
                                                               [BlkJSX,"txtWindowId"],
                                                               [BlkJSX,"txtWindowLeft"],
                                                               [BlkJSX,"txtWindowTop"],
                                                               [BlkJSX,"txtWindowWidth"],
                                                               [BlkJSX,"txtWindowHeight"],
                                                               [BlkJSX,"txtWindowTransparency"],
                                                               [BlkJSX,"txtWindowZIndex"],
                                                               [BlkJSX,"btnAddCanvasWindow"],
                                                               [BlkJSX,"txtDisplayWidth"],
                                                               [BlkJSX,"txtDisplayHeight"],
                                                               [BlkJSX,"chkWindowBorder"],
                                                               [BlkJSX,"chkWindowHeader"],
                                                               [BlkJSX,"blkWindowFullScreen"],
                                                               [BlkJSX,"blkFullScreenCtr"],
                                                               [BlkJSX,"blkCanvasCtr"],
                                                               [BlkJSX,"blkCanvasContainer"],
                                                               [BlkJSX,"selCanvasLayout"],
                                                               [BlkJSX,"chkWindow1:1Aspect"],
                                                               [BlkJSX,"btnScrollBar"],
                                                               [BlkJSX,"btnBack"],
                                                               [BlkJSX,"blkPropertiesCtr"],
                                                               [BlkJSX,"blkScrollCtr"],
                                                               [BlkJSX,"blkWindow"],
                                                               [BlkJSX,"blkVideo"],
                                                               [BlkJSX,"blkProperties"],
                                                               [BlkJSX,"txtScrollTop"],
                                                               [BlkJSX,"txtScrollLeft"]
                                                              ],
                                                              null,
                                                              null,
                                                              null,
                                                              "AdminEditCanavsConfig"
                                                              );  
  
  
  var cmpBlkDisplayProfile = new CmpContainer([BlkJSX,"blkAssetClientDisplayProfile"],
                                              [{cmp:cmpSelDisplayProfileList},
                                               {cmp:cmpDisplayProfileSetting}],
                                              null
                                             );
    
  var cmpBlkDisplayProfileAVUI = new CmpContainer( [BlkJSX,"addV2DDetail"],
                                                    [{cmp:cmpBlkDisplayProfile,
                                                       sel:"topic_Get(\"" + tDlg + "\", 'EditClientDisplayProfileSelection') == 'DISPLAYPROFILE' || topic_Get(\"" + tDlg + "\", 'EditClientDisplayProfileSelection') == null"}
                                                    ],
                                                    [{action:"TOPICEVENT", topic:tDlg, eventvar:"EditClientDisplayProfileSelection", eventval:null}] 
                                                  );

  var cmpLblAssetEditDisplayProfile = new CmpLabelHighlight([BlkJSX,"lblClientDisplayProfile"], null, 
                                                            [{action:"TOPICEVENT", topic:tDlg, eventvar:"EditClientDisplayProfileSelection", eventval:null}],
                                                             {action:"TOPICEVENT", topic:tDlg, eventvar:"EditClientDisplayProfileSelection", eventval:"DISPLAYPROFILE"} );
        
  var cmpBlkEditAssetPort = new CmpContainer( [BlkJSX,"addV2DNav"],
                                              [ {cmp:cmpLblAssetEditDisplayProfile,
                                                  sel:"topic_Get(\"" + tDlg + "\", 'EditClientDisplayProfileSelection') == 'V2dTxVideoParam' || topic_Get(\"" + tDlg + "\", 'EditClientDisplayProfileSelection') == null"},
                                                {cmp:cmpBlkDisplayProfileAVUI}
                                              ],
                                              null
                                             );
  

  var cmpBtnEditAssetClose = new CmpButton([BlkJSX,"btnDialogCloseX"],
                                           null, 
                                           {action:"FNCCALL", 
                                            context:null, 
                                            fnc:function() {
                                              dlgEditAsset_Destroy();
                                            }
                                           });
            
  var cmpBtnEditAssetCancel = new CmpButton([BlkJSX,"btnCancelAddAssetPorts"],
                                           null, 
                                           {action:"FNCCALL", 
                                            context:null, 
                                            fnc:function() {
                                              dlgEditAsset_Destroy();
                                            }
                                           });
            
  var cmpBtnEditAssetOK = new CmpButton([BlkJSX,"btnAddAssetPorts"],
                                           null, 
                                           {action:"FNCCALL", 
                                            context:null, 
                                            fnc:function() {
                                              eventDlg.subscribe("EditAssetSetAssetStatePorts", "DONE", this,
                                                                function() {
                                                                  dlgEditAsset_Destroy();
                                                                });
                                              cmpDlgEditAssetSetAsset.call(this, AssetNID);
                                            }
                                           });

  var cmpBlkAssetPortSettings = new Component([BlkJSX,"blkAssetPortSettings"]);
                                                                                         
  var cmpDlgEditAsset = new CmpContainer([BlkJSX,"DialogAVSettings"],
                                         [ {cmp:cmpBtnEditAssetOK},
                                           {cmp:cmpBtnEditAssetCancel},
                                           {cmp:cmpBtnEditAssetClose},
                                           {cmp:cmpBlkEditAssetPort},
                                           {cmp:cmpCDFEditAssetAVSettings},
                                           {cmp:cmpBlkAssetPortSettings}
                                         ],
                                         [{action:"TOPICEVENT", topic:tDlg, eventvar:"EditAssetDataState", eventval:AssetNID}]
                                        );                            
                                          
  cmpDlgEditAsset.BlkJSX = BlkJSX;
  cmpDlgEditAsset.ObjectNID = AssetNID;
  // fnc cmpReq_GetCompleteDeviceWithReadQuery defined in cmpReq.js
  cmpDlgEditAsset.getBlkDevice = cmpReq_GetDevice;
  cmpDlgEditAsset.tDlg = tDlg;
  
  // init and repaint happens after the dialog is loaded
  jsx3.sleep( (function(){
                 this.init();
                 this.getBlkDevice(this.tDlg, this.ObjectNID, "RAW");
                 }
               ),
               null, cmpDlgEditAsset );

  
  // event published from func defined in cmpReq.js                  
  eventDlg.subscribe("GetCompleteDeviceDone", AssetNID, this,
                      function() {
                        cmpDlgEditAssetGetAsset(AssetNID);
                      }
                    ); // subscribe end    
                    
  // On selecting a DisplayProfile from drop down
  eventDlg.subscribe("DisplayProfileSelected", null, this,
                          function(eventVar, eventVal) {
                            if (eventVal != "NONE") {
                                // get the selected value
                                var nameRecord = cmpSelDisplayProfileList.BlkJSX.getValue();
                                //get the complete record
                                var selNameRecord = cmpSelDisplayProfileList.BlkJSX.getRecordNode(nameRecord);
                                // get the Tag node which contains the TagValues
                                var canvasNode = selNameRecord.selectSingleNode(".//Canvas");
                                //get the xsl
                                var xslUriRes = null;
                                var UriRes = cmpSelDisplayProfileList.BlkJSX.getUriResolver();
                                if( jsx3.util.strEmpty(UriRes) ) return;
                                xslUriRes = UriRes.resolveURI("xsl/Canvas.xsl");
                                if( jsx3.util.strEmpty(xslUriRes) ) return;

                                var XSLFile = PNameSpace.loadInclude(xslUriRes, "Canvas" , "xsl");
                                if( XSLFile ) {
                                  var XSLDoc = jsx3.xml.XslDocument.wrap(XSLFile);
                                  var docXformResp = XSLDoc.transformToObject(canvasNode);
                                }                                
                                // set/send the cdf doc to the panel
                                cmpDisplayProfileSetting.onResponse("SUCCESS", docXformResp.toString()); 
                            }
                          }
                        );
                        
                                            
  // fnc to get dlg data
  function cmpDlgEditAssetGetAsset(AssetNID) {

    var reqStatus = topic_Get(tDlg, "GetCompleteDeviceStatus");
    if(reqStatus != "SUCCESS") {
      dlgEditAsset_Destroy();  
    }
    else {
      // send the request to get the displayprofile list
      cmpSelDisplayProfileList.ResetToStart(1,5000,0);
      topic_Publish(tDlg, "DisplayProfileGetData", (newServerDate()).getTime() );
      
      
      var deviceNode = getDialogDataNode(tDlg, "Device");
      var AssetName = deviceNode.getAttribute("title");
      var AssetType = deviceNode.getAttribute("type"); 
      
      DeviceNID = deviceNode.getAttribute("NID");
      var displayConfigNodeElement = "IPVSClientDisplayConfig";
      var mrDecoder = deviceNode.selectSingleNode(".//MRDecoderDisplayConfig");
      if( !jsx3.util.strEmpty(mrDecoder)) {
        displayConfigNodeElement = "MRDecoderDisplayConfig";
      }
      var deviceDisplayConfigNode = deviceNode.selectSingleNode(".//DeviceConfig/AVCodecElementsConfig//" + displayConfigNodeElement);
      
      // genrate the displatProfileCanvas CDF
      var xslfile = PNameSpace.loadInclude("jsxuser:///JSXAPPS/IPVS_AdminUI/xsl/IPVSClientDisplayConfig.xsl","DisplayProfile","xsl");
      var xslDoc = jsx3.xml.XslDocument.wrap(xslfile);
      var displayProfileCanvas = xslDoc.transform(deviceDisplayConfigNode);  
      var defRecordDoc = new jsx3.xml.Document();
      var defRecordNode = defRecordDoc.loadXML(displayProfileCanvas.toString());      
      var canvasNode = defRecordNode.selectSingleNode(".//record/Canvas");      
      var canvasHRes = canvasNode.getAttribute("hres");
      if( jsx3.util.strEmpty(canvasHRes) ){
        canvasNode.setAttribute("hres","1920");
      }
      var canvasVRes = canvasNode.getAttribute("vres");
      if( jsx3.util.strEmpty(canvasVRes) ){
        canvasNode.setAttribute("vres","1080");
      }      
      cmpDisplayProfileSetting.onResponse("SUCCESS", defRecordNode.toString());      


      // set the Device Config data
//      var strXMLCacheId = cmpCDFEditAssetAVSettings.BlkJSX.getXMLId();
//      var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
//      objXML.loadXML(deviceNode.toString());
//      cmpCDFEditAssetAVSettings.BlkJSX.setSourceXML(objXML);         
//      cmpCDFEditAssetAVSettings.BlkJSX.setCDFId(DeviceNID);

      // Finish and repaint rule data
      topic_Publish(tDlg, "EditAssetDataState", AssetNID);       
      topic_Publish(tDlg ,"EditClientDisplayProfileSelection", "DISPLAYPROFILE");
          
    }    

  }// end get dlg data function


  //Basic Validation 
  function cmpDlgEditAssetSetAssetValidate() {
    return true;
  }
      
  // fnc to set dlg data - runs in the context of the OK button
  function cmpDlgEditAssetSetAsset(AssetNID) {
  
    if( cmpDlgEditAssetSetAssetValidate() != true ) return;
       
    
    var newCanvasNodeXML = cmpDisplayProfileSetting.getCanvasXML();
    var newCanvasNode = newCanvasNodeXML.selectSingleNode(".//Canvas"); 
    //Adding default Id to a Canvas Profile
    var newCanvasId = "Canvas_"+ getUUID();
    newCanvasNode.setAttribute("id",newCanvasId);

    var deviceNode = getDialogDataNode(tDlg, "Device");
    var portNode = deviceNode.selectSingleNode(".//PortList/Port");
    var dstPortNID = portNode.getAttribute("NID");
  
    // create the set request data
    var setPortDisplayNode = topic_Set(tDlg, "SetPortDisplayConfig", null);
    setPortDisplayNode.removeChildren();
    setPortDisplayNode.setAttribute("byNID", dstPortNID);
    setPortDisplayNode.setAttribute("byTitle", "");
    setPortDisplayNode.appendChild(newCanvasNode);

    //send therequest
    var setPortDisplayReq = new ReqSessionHandleWindowing(setPortDisplayNode, "SetPortDisplayConfig",
                                                             { action: "FNCCALL", context: this,
                                                               fnc: function (respCode, respVal) {
                                                                 if (respCode == "SUCCESS" && respVal != null) {
                                                                   topic_Publish(tDlg, "EditAssetSetAssetStatePorts", "DONE");
                                                                 }
                                                                 else {
                                                                   if (respVal.match(/ResponseXML/)) {
                                                                     var ResponseXML = (respVal.replace(/\n/g, ' ')).replace(/.*ResponseXML:(.*)/, "$1");
                                                                     var responseDoc = new jsx3.xml.Document;
                                                                     responseDoc.loadXML(ResponseXML);
                                                                     var ipvsErrorNode = responseDoc.selectSingleNode(".//ipvserror");
                                                                     if (!jsx3.util.strEmpty(ipvsErrorNode)) {
                                                                       ErrCode = ipvsErrorNode.getAttribute("code");
                                                                       ErrDesc = ipvsErrorNode.selectSingleNode(".//description").getValue();
                                                                     }
                                                                     //alert the user
                                                                     var ErrorMsg = "Error: SetPortDisplayConfig request failed \n";
                                                                     ErrorMsg += "Error description: " + ErrDesc + "\n";
                                                                     ErrorMsg += "Error code: " + ErrCode;
                                                                     alert(ErrorMsg);
                                                                   }
                                                                 }

                                                               }
                                                             }
                                                         );


    setPortDisplayReq.sendRequest(this);    
  } // end cmpDlgEditAssetSetAsset
       
} // end create
