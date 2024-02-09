// this code has some potential issues 
// - w.r.t object reference counts - when objects created in here go out of scope etc
// - loading sync issues - dlg may not be loaded before the object init methods are called

function dlgEditMediaStreamFilter_Create(MediaStreamFilterNID) {

  // deal with defaults for add
  if( MediaStreamFilterNID == null ) MediaStreamFilterNID = "DEFAULT_MEDIASTREAMFILTER_NID";
  
  //closeDialogIfExists("DialogMediaStreamFilter");
  // load view - dialog component file
  var DlgFile = "components/policy/policyStreamFilterDialog.xml";
  var BlkJSX = PNameSpace.getBodyBlock().load(DlgFile);
  
  // setup a local topic for dlg specific messaging  
  var tDlg = "Dialog_" + jsx3.xml.CDF.getKey()
  topic_Create(tDlg);
  
  // setup a listner for it  
  var eventDlg = new EventListner(tDlg);

  // setup a private/dlg specific app event handler
  dlgEditTagObjectFilter_eventAppTagObject = new EventListner( tApp );
  dlgEditTagObjectFilter_eventAppGroupTag = new EventListner( tApp );
  
  var dlgText = "Add Routing Policy";
  if( MediaStreamFilterNID != "DEFAULT_MEDIASTREAMFILTER_NID" ){
    dlgText = "Edit Routing Policy";
  }
  var windowBarBlkJSX = getJsxByName([BlkJSX, "windowBar"]);
  windowBarBlkJSX.setText(dlgText, true);
  closeDialogIfExists(dlgText);
        
  var cmpBlkRoutingSrcFilterMatrix = new CmpBlock([BlkJSX,"mtxStreamSourceFilterSel"]);
  cmpBlkRoutingSrcFilterMatrix.BlkJSX.setXMLTransformers("xsl/PolicySrcFilterToCDF.xsl");
  cmpBlkRoutingSrcFilterMatrix.BlkJSX.setSelectionModel(jsx3.gui.Matrix.SELECTION_ROW);

  var cmpBlkRoutingDstFilterMatrix = new CmpBlock([BlkJSX,"mtxStreamDstFilterSel"]);
  cmpBlkRoutingDstFilterMatrix.BlkJSX.setXMLTransformers("xsl/PolicyDstFilterToCDF.xsl");
  cmpBlkRoutingDstFilterMatrix.BlkJSX.setSelectionModel(jsx3.gui.Matrix.SELECTION_ROW);

  var cmpBlkStreamProfileFilterMatrix = new CmpBlock([BlkJSX,"mtxStreamProfileFilterSel"]);
  cmpBlkStreamProfileFilterMatrix.BlkJSX.setXMLTransformers("xsl/PolicyStreamProfileFilterToCDF.xsl");
  cmpBlkStreamProfileFilterMatrix.BlkJSX.setSelectionModel(jsx3.gui.Matrix.SELECTION_ROW);

  var cmpBlkRoutingRelayFilterMatrix = new CmpBlock([BlkJSX,"mtxStreamRelayFilterSel"]);
  cmpBlkRoutingRelayFilterMatrix.BlkJSX.setXMLTransformers("xsl/PolicyRelayFilterToCDF.xsl");
  cmpBlkRoutingRelayFilterMatrix.BlkJSX.setSelectionModel(jsx3.gui.Matrix.SELECTION_ROW);

  var cmpBlkRoutingRuleKeyMatrix = new CmpBlock([BlkJSX,"mtxStreamRuleKeySel"]);
  cmpBlkRoutingRuleKeyMatrix.BlkJSX.setXMLTransformers("xsl/PolicyRuleKeyFilterToCDF.xsl");
  cmpBlkRoutingRuleKeyMatrix.BlkJSX.setSelectionModel(jsx3.gui.Matrix.SELECTION_ROW);
  
  var cmpCDFRoutingPolicy = new Component([BlkJSX,"cdfDlgRoutingPolicy"]);
  cmpCDFRoutingPolicy.BlkJSX.setXMLTransformers("xsl/RoutingPolicy.xsl");

  function dlgEditMediaStreamFilter_Destroy() {
    // destroy any local eventlistners
    eventDlg.destroy();
    dlgEditTagObjectFilter_eventAppTagObject.destroy();
    dlgEditTagObjectFilter_eventAppGroupTag.destroy();
    
    // destroy all local components
    cmpDlgEditMediaStreamFilter.destroy();
    
    // destroy the local topic
    topic_Destroy(tDlg);
    
    // close dialog
    BlkJSX.doClose();    
  }


  // event subscribtion: if new tagobjectfilter is added from select box and update all the
  // select boxes in the dialog after add is finished
  dlgEditTagObjectFilter_eventAppTagObject.subscribe("TagObjectFilterDataState", null, this,
                                                      function() {
                                                        updateSelectTagObjectFilters();
                                                      } );
  
  // event subscribtion: if new grouptag is added from select box and update all the
  // select boxes in the dialog after add is finished                      
  dlgEditTagObjectFilter_eventAppGroupTag.subscribe("GroupTagDataState", null, this,
                                                      function() {
                                                        updateSelectGroupTag();
                                                      } );

  var cmpSelSrcUserFilter = new CmpSelect([BlkJSX,"selStreamFilterSrc_Users"],
                                               null,
                                               {action:"FNCCALL", 
                                               context:null, 
                                               fnc:function() {
                                                  var selVal = cmpSelSrcUserFilter.BlkJSX.getValue();
                                                  if (jsx3.util.strEmpty(selVal)) return;
                                                  var strXMLCacheId = cmpSelSrcUserFilter.BlkJSX.getXMLId();
                                                  var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
                                                  if (jsx3.util.strEmpty(objXML)) return;
                                                  var record = objXML.selectSingleNode("//record[@jsxid='" + selVal + "']");
                                                  checkAddForNewFilter(record, "User");
                                               }
                                              });

  var cmpSelSrcResourceFilter = new CmpSelect([BlkJSX,"selStreamFilterSrc_Resources"],
                                               null,
                                               {action:"FNCCALL", 
                                               context:null, 
                                               fnc:function() {
                                                  var selVal = cmpSelSrcResourceFilter.BlkJSX.getValue();
                                                  if (jsx3.util.strEmpty(selVal)) return;
                                                  var strXMLCacheId = cmpSelSrcResourceFilter.BlkJSX.getXMLId();
                                                  var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
                                                  if (jsx3.util.strEmpty(objXML)) return;
                                                  var record = objXML.selectSingleNode("//record[@jsxid='" + selVal + "']");
                                                  checkAddForNewFilter(record, "Resource");
                                               }
                                              });

  var cmpBtnAddRoutingSrcFilter = new CmpButton([BlkJSX,"btnAddStreamFilterSrc"],
                                                 null, 
                                                 {action:"FNCCALL", 
                                                  context:null, 
                                                  fnc:function() {
                                                      addDatatoMatrix(this.userFilterBlkJSX, this.resourceFilterBlkJSX, this.routingSrcFilterMatrix);
                                                  }
                                                 });
  cmpBtnAddRoutingSrcFilter.userFilterBlkJSX = cmpSelSrcUserFilter.BlkJSX;
  cmpBtnAddRoutingSrcFilter.resourceFilterBlkJSX = cmpSelSrcResourceFilter.BlkJSX;
  cmpBtnAddRoutingSrcFilter.routingSrcFilterMatrix = cmpBlkRoutingSrcFilterMatrix.BlkJSX;
  
  var cmpSelDstUserFilter = new CmpSelect([BlkJSX,"selStreamFilterDst_Users"],
                                               null,
                                               {action:"FNCCALL", 
                                               context:null, 
                                               fnc:function() {
                                                  var selVal = cmpSelDstUserFilter.BlkJSX.getValue();
                                                  if (jsx3.util.strEmpty(selVal)) return;
                                                  var strXMLCacheId = cmpSelDstUserFilter.BlkJSX.getXMLId();
                                                  var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
                                                  if (jsx3.util.strEmpty(objXML)) return;
                                                  var record = objXML.selectSingleNode("//record[@jsxid='" + selVal + "']");
                                                  checkAddForNewFilter(record, "User");
                                               }
                                              });

  var cmpSelDstResourceFilter = new CmpSelect([BlkJSX,"selStreamFilterDst_Resources"],
                                               null,
                                               {action:"FNCCALL", 
                                               context:null, 
                                               fnc:function() {
                                                  var selVal = cmpSelDstResourceFilter.BlkJSX.getValue();
                                                  if (jsx3.util.strEmpty(selVal)) return;
                                                  var strXMLCacheId = cmpSelDstResourceFilter.BlkJSX.getXMLId();
                                                  var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
                                                  if (jsx3.util.strEmpty(objXML)) return;
                                                  var record = objXML.selectSingleNode("//record[@jsxid='" + selVal + "']");
                                                  checkAddForNewFilter(record, "Resource");
                                               }
                                              });

  var cmpBtnAddRoutingDstFilter = new CmpButton([BlkJSX,"btnAddStreamFilterDst"],
                                                 null, 
                                                 {action:"FNCCALL", 
                                                  context:null, 
                                                  fnc:function() {
                                                      addDatatoMatrix(this.userFilterBlkJSX, this.resourceFilterBlkJSX, this.routingDstFilterMatrix);
                                                  }
                                                 });
  cmpBtnAddRoutingDstFilter.userFilterBlkJSX = cmpSelDstUserFilter.BlkJSX;
  cmpBtnAddRoutingDstFilter.resourceFilterBlkJSX = cmpSelDstResourceFilter.BlkJSX;
  cmpBtnAddRoutingDstFilter.routingDstFilterMatrix = cmpBlkRoutingDstFilterMatrix.BlkJSX;
  
  var cmpSelStreamProfileFilter = new CmpSelect([BlkJSX,"selStreamFilterProfile"],
                                               null,
                                               {action:"FNCCALL", 
                                               context:null, 
                                               fnc:function() {
                                                  var selVal = cmpSelStreamProfileFilter.BlkJSX.getValue();
                                                  if (jsx3.util.strEmpty(selVal)) return;
                                                  var strXMLCacheId = cmpSelStreamProfileFilter.BlkJSX.getXMLId();
                                                  var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
                                                  if (jsx3.util.strEmpty(objXML)) return;
                                                  var record = objXML.selectSingleNode("//record[@jsxid='" + selVal + "']");
                                                  checkAddForNewFilter(record, "Resource");
                                               }
                                              });
  
  var cmpBtnAddStreamProfileFilter = new CmpButton([BlkJSX,"btnAddStreamFilterProfile"],
                                                 null, 
                                                 {action:"FNCCALL", 
                                                  context:null, 
                                                  fnc:function() {
                                                      var streamProfileRecord = this.streamProfileFilterBlkJSX.getValue();
                                                      if(streamProfileRecord!=""){
                                                        var mtachResult = streamProfileRecord.search(/Select /);

                                                        if(mtachResult != -1){
                                                          alert("Please "+streamProfileRecord);
                                                          return false;
                                                        } else {
                                                          var streamProfileNode = this.streamProfileFilterBlkJSX.getRecord(streamProfileRecord);
                                                          var streamProfileName= streamProfileNode.jsxtext;
                                                          var streamProfileNID = streamProfileNode.NID;
                                                        }
                                                        var o = new Object();
                                                        o.jsxid = jsx3.xml.CDF.getKey();
                                                        o.jsxtextOneNID = streamProfileNID;
                                                        o.jsxtextOne = streamProfileName;
                                                        this.streamProfileFilterMatrix.insertRecord(o, null, false);
                                                      }
                                                      this.streamProfileFilterMatrix.repaintData();
                                                      this.streamProfileFilterBlkJSX.setValue("Select MediaStreamProfile Filter");
                                                  }
                                                 });
  cmpBtnAddStreamProfileFilter.streamProfileFilterBlkJSX = cmpSelStreamProfileFilter.BlkJSX;
  cmpBtnAddStreamProfileFilter.streamProfileFilterMatrix = cmpBlkStreamProfileFilterMatrix.BlkJSX;
  
  var cmpSelRelayUserFilter = new CmpSelect([BlkJSX,"selStreamFilterRelay_Users"],
                                               null,
                                               {action:"FNCCALL", 
                                               context:null, 
                                               fnc:function() {
                                                  var selVal = cmpSelRelayUserFilter.BlkJSX.getValue();
                                                  if (jsx3.util.strEmpty(selVal)) return;
                                                  var strXMLCacheId = cmpSelRelayUserFilter.BlkJSX.getXMLId();
                                                  var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
                                                  if (jsx3.util.strEmpty(objXML)) return;
                                                  var record = objXML.selectSingleNode("//record[@jsxid='" + selVal + "']");
                                                  checkAddForNewFilter(record, "User");
                                               }
                                              });

  var cmpSelRelayResourceFilter = new CmpSelect([BlkJSX,"selStreamFilterRelay_Resources"],
                                               null,
                                               {action:"FNCCALL", 
                                               context:null, 
                                               fnc:function() {
                                                  var selVal = cmpSelRelayResourceFilter.BlkJSX.getValue();
                                                  if (jsx3.util.strEmpty(selVal)) return;
                                                  var strXMLCacheId = cmpSelRelayResourceFilter.BlkJSX.getXMLId();
                                                  var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
                                                  if (jsx3.util.strEmpty(objXML)) return;
                                                  var record = objXML.selectSingleNode("//record[@jsxid='" + selVal + "']");
                                                  checkAddForNewFilter(record, "Resource");
                                               }
                                              });

  var cmpBtnAddRoutingRelayFilter = new CmpButton([BlkJSX,"btnAddStreamFilterRelay"],
                                                 null, 
                                                 {action:"FNCCALL", 
                                                  context:null, 
                                                  fnc:function() {
                                                      addDatatoMatrix(this.userFilterBlkJSX, this.resourceFilterBlkJSX, this.routingRelayFilterMatrix);
                                                  }
                                                 });
  cmpBtnAddRoutingRelayFilter.userFilterBlkJSX = cmpSelRelayUserFilter.BlkJSX;
  cmpBtnAddRoutingRelayFilter.resourceFilterBlkJSX = cmpSelRelayResourceFilter.BlkJSX;
  cmpBtnAddRoutingRelayFilter.routingRelayFilterMatrix = cmpBlkRoutingRelayFilterMatrix.BlkJSX;

  var cmpSelRuleKeyLHTag = new CmpSelect([BlkJSX,"selStreamFilterRuleKeyGroupTagLH"],
                                               null,
                                               {action:"FNCCALL", 
                                               context:null, 
                                               fnc:function() {
                                                  var selVal = cmpSelRuleKeyLHTag.BlkJSX.getValue();
                                                  if (jsx3.util.strEmpty(selVal)) return;
                                                  var strXMLCacheId = cmpSelRuleKeyLHTag.BlkJSX.getXMLId();
                                                  var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
                                                  if (jsx3.util.strEmpty(objXML)) return;
                                                  var record = objXML.selectSingleNode("//record[@jsxid='" + selVal + "']");
                                                  checkAddForNewTagName(record);
                                               }
                                              });

  var cmpSelRuleKeyRHTag = new CmpSelect([BlkJSX,"selStreamFilterRuleKeyGroupTagRH"],
                                               null,
                                               {action:"FNCCALL", 
                                               context:null, 
                                               fnc:function() {
                                                  var selVal = cmpSelRuleKeyRHTag.BlkJSX.getValue();
                                                  if (jsx3.util.strEmpty(selVal)) return;
                                                  var strXMLCacheId = cmpSelRuleKeyRHTag.BlkJSX.getXMLId();
                                                  var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
                                                  if (jsx3.util.strEmpty(objXML)) return;
                                                  var record = objXML.selectSingleNode("//record[@jsxid='" + selVal + "']");
                                                  checkAddForNewTagName(record);
                                               }
                                              });

  var cmpBtnAddRoutingRuleKey = new CmpButton([BlkJSX,"btnAddStreamFilterRuleKey"],
                                                 null, 
                                                 {action:"FNCCALL", 
                                                  context:null, 
                                                  fnc:function() {
                                                      var ruleKeyLHRecord = this.LHFilterRuleKeyBlkJSX.getValue();
                                                      if(ruleKeyLHRecord!=""){
                                                        var LHSRuleKey = SelectRuleKeyCheck(this.LHFilterRuleKeyBlkJSX);
                                                        if(LHSRuleKey)
                                                          var LHSTagName = SelectRuleKeyCheck(this.LHTagRuleKeyBlkJSX);
                                                        if(LHSTagName)
                                                          var MatchOperation = SelectRuleKeyCheck(this.OperatorRuleKeyBlkJSX);
                                                        if(MatchOperation)
                                                          var RHSRuleKey = SelectRuleKeyCheck(this.RHFilterRuleKeyBlkJSX);
                                                        if(RHSRuleKey)
                                                          var RHSTagName = SelectRuleKeyCheck(this.RHTagRuleKeyBlkJSX);
                                                        if(RHSTagName) {
                                                          var o = new Object();
                                                          o.jsxid = jsx3.xml.CDF.getKey() ;
                                                          o.jsxtextLHRuleKey = LHSRuleKey;
                                                          o.jsxtextLHTagName = LHSTagName;
                                                          o.jsxtextOperation = MatchOperation;
                                                          o.jsxtextRHRuleKey = RHSRuleKey;
                                                          o.jsxtextRHTagName = RHSTagName;
                                                          this.routingRuleKeyMatrix.insertRecord(o, null, false);
                                                        }
                                                      }
                                                      this.routingRuleKeyMatrix.repaintData();
                                                      this.LHFilterRuleKeyBlkJSX.setValue("Select LH Rule");
                                                      this.LHTagRuleKeyBlkJSX.setValue("Select Tag Name");
                                                      this.OperatorRuleKeyBlkJSX.setValue("Select Operation");
                                                      this.RHFilterRuleKeyBlkJSX.setValue("Select RH Rule");
                                                      this.RHTagRuleKeyBlkJSX.setValue("Select Tag Name");
                                                  }
                                                 });
  cmpBtnAddRoutingRuleKey.LHFilterRuleKeyBlkJSX = getJsxByName([BlkJSX,"selStreamFilterRuleKeyLH"]);
  cmpBtnAddRoutingRuleKey.LHTagRuleKeyBlkJSX = cmpSelRuleKeyLHTag.BlkJSX;
  cmpBtnAddRoutingRuleKey.OperatorRuleKeyBlkJSX = getJsxByName([BlkJSX,"selStreamFilterRuleKeyOp"]);
  cmpBtnAddRoutingRuleKey.RHFilterRuleKeyBlkJSX = getJsxByName([BlkJSX,"selStreamPolicyRuleKeyRH"]);
  cmpBtnAddRoutingRuleKey.RHTagRuleKeyBlkJSX = cmpSelRuleKeyRHTag.BlkJSX;
  cmpBtnAddRoutingRuleKey.routingRuleKeyMatrix = cmpBlkRoutingRuleKeyMatrix.BlkJSX;
                              
  var cmpBtnEditMediaStreamFilterClose = new CmpButton([BlkJSX,"btnDialogCloseX"],
                                                         null, 
                                                         {action:"FNCCALL", 
                                                          context:null, 
                                                          fnc:function() {
                                                            dlgEditMediaStreamFilter_Destroy();
                                                          }
                                                         });
            
  var cmpBtnEditMediaStreamFilterCancel = new CmpButton([BlkJSX,"btnMediaStreamFilterCancel"],
                                                         null, 
                                                         {action:"FNCCALL", 
                                                          context:null, 
                                                          fnc:function() {
                                                            dlgEditMediaStreamFilter_Destroy();
                                                          }
                                                         });
            
  var cmpBtnEditMediaStreamFilterDone = new CmpButton([BlkJSX,"btnMediaStreamFilterDone"],
                                                       null, 
                                                       {action:"FNCCALL", 
                                                        context:null, 
                                                        fnc:function() {
                                                          cmpDlgEditMediaStreamFilterSetMediaStreamFilter.call(this, MediaStreamFilterNID);
                                                        }
                                                       });
                                                                                         
  var cmpDlgEditMediaStreamFilter = new CmpContainer([BlkJSX,"DialogMediaStreamFilter"],
                                                      [ {cmp:cmpBtnEditMediaStreamFilterClose},
                                                        {cmp:cmpBtnEditMediaStreamFilterCancel},
                                                        {cmp:cmpBtnEditMediaStreamFilterDone},
                                                        {cmp:cmpBlkRoutingSrcFilterMatrix},
                                                        {cmp:cmpBlkRoutingDstFilterMatrix},
                                                        {cmp:cmpBlkStreamProfileFilterMatrix},
                                                        {cmp:cmpBlkRoutingRelayFilterMatrix},
                                                        {cmp:cmpBlkRoutingRuleKeyMatrix},
                                                        {cmp:cmpSelSrcUserFilter},
                                                        {cmp:cmpSelSrcResourceFilter},
                                                        {cmp:cmpSelDstUserFilter},
                                                        {cmp:cmpSelDstResourceFilter},
                                                        {cmp:cmpSelStreamProfileFilter},
                                                        {cmp:cmpSelRelayUserFilter},
                                                        {cmp:cmpSelRelayResourceFilter},
                                                        {cmp:cmpSelRuleKeyLHTag},
                                                        {cmp:cmpSelRuleKeyRHTag},
                                                        {cmp:cmpBtnAddRoutingSrcFilter},
                                                        {cmp:cmpBtnAddRoutingDstFilter},
                                                        {cmp:cmpBtnAddStreamProfileFilter},
                                                        {cmp:cmpBtnAddRoutingRelayFilter},
                                                        {cmp:cmpBtnAddRoutingRuleKey},
                                                        {cmp:cmpCDFRoutingPolicy}
                                                      ],
                                                      [{action:"TOPICEVENT", topic:tDlg, eventvar:"EditMediaStreamFilterDataState", eventval:MediaStreamFilterNID}]);                            
                        
  cmpDlgEditMediaStreamFilter.BlkJSX = BlkJSX; 
  cmpDlgEditMediaStreamFilter.ObjectNID = MediaStreamFilterNID;   
  cmpDlgEditMediaStreamFilter.getBlkData =  cmpDlgEditMediaStreamFilterGetMediaStreamFilter;
  
  // init and repaint happens after the dialog is loaded
  jsx3.sleep( (function(){
                 this.init();
                 this.getBlkData(this.ObjectNID);
                 }
               ),
               null, cmpDlgEditMediaStreamFilter );
      
  // fnc to get dlg data
  function cmpDlgEditMediaStreamFilterGetMediaStreamFilter(MediaStreamFilterNID) { 
    var MediaStreamFilterNode = topic_GetNode(tCache, "//*[@NID='" + MediaStreamFilterNID + "']");
    if( MediaStreamFilterNode == null ) {
      var reqData = new ReqGetObjectNID(MediaStreamFilterNID, "MediaStreamFilter",
                                         {action:"FNCCALL", context:this, 
                                           fnc:function(respCode, respVal) {
                                                 if(respCode == "SUCCESS" && respVal != null) {
                                                   cmpDlgEditMediaStreamFilterGetMediaStreamFilter(MediaStreamFilterNID);
                                                 } else {
                                                   var codeErrorMsg = CommonErrorCodeArr[respCode].errorDesc;
                                                   var ErrorMsg = "Error: MediaStream filter get data request failed \n";
                                                   ErrorMsg += "Error description: " + codeErrorMsg + "\n";
                                                   ErrorMsg += "Error code: " + respCode;
                                                   alert(ErrorMsg);
                                                 }                                                 
                                               }
                                         } 
                                       );
      reqData.sendRequest();
    }
    else {
      //cmpDlgEditMediaStreamFilter.setMapRuleData("MediaStreamFilter", "IN", "NID", MediaStreamFilterNID);
      //topic_Publish(tDlg, "EditMediaStreamFilterDataState", MediaStreamFilterNID);

      // using the old repaint of the dialog
      DisplayEditValuesForMediaStreamFilter(MediaStreamFilterNID);
    }
  }
       
  // fnc to set dlg data - runs in the context of the OK button
  function cmpDlgEditMediaStreamFilterSetMediaStreamFilter(MediaStreamFilterNID) {
    // validate input 
    if( cmpDlgEditMediaStreamFilterValidate() != true ) return;
    // decide add or update
    var SetMsg = "Edit MediaStream filter";
    var SetMode = "Replace";
    if( MediaStreamFilterNID == "DEFAULT_MEDIASTREAMFILTER_NID" ) {
      SetMsg = "Add MediaStream filter";
      SetMode = "Add";
    }

    // get MediaStreamFilter node info from UI
    var MediaStreamFilterNode = null;
    
    // write the data back to the CDF
    cmpCDFRoutingPolicy.BlkJSX.write();

    var strXMLCacheId = cmpCDFRoutingPolicy.BlkJSX.getXMLId();
    var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);      
    var xslfile = PNameSpace.loadInclude("JSXAPPS/IPVS_AdminUI/xsl/RoutingPolicyRequest.xsl","RoutingPolicyRequest","xsl");
    var xslDoc = jsx3.xml.XslDocument.wrap(xslfile);
    var routingPolicyConfigXML = xslDoc.transform(objXML);
    MediaStreamFilterNode = new jsx3.xml.Document();
    MediaStreamFilterNode.loadXML(routingPolicyConfigXML);

    var strXMLCacheId = cmpBlkRoutingSrcFilterMatrix.BlkJSX.getXMLId();
    var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
    var xslfile = PNameSpace.loadInclude("jsxuser:///JSXAPPS/IPVS_AdminUI/xsl/CDFToPolicySrcFilter.xsl","SrcFilterSetNode","xsl");
    var xslDoc = jsx3.xml.XslDocument.wrap(xslfile);
    var newSrcFilterSetXML = xslDoc.transform(objXML);        
    var newSrcFilterSetNode = new jsx3.xml.Document();
    newSrcFilterSetNode.loadXML(newSrcFilterSetXML);
    
    var oldSrcFilterSetNode = MediaStreamFilterNode.selectSingleNode(".//SrcFilterSet");              
    var srcFilterSetParentNode = MediaStreamFilterNode.selectSingleNode(".//SrcFilterSet").getParent();
    srcFilterSetParentNode.replaceNode(newSrcFilterSetNode,oldSrcFilterSetNode);
    
    var strXMLCacheId = cmpBlkRoutingDstFilterMatrix.BlkJSX.getXMLId();
    var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
    var xslfile = PNameSpace.loadInclude("jsxuser:///JSXAPPS/IPVS_AdminUI/xsl/CDFToPolicyDstFilter.xsl","DrcFilterSetNode","xsl");
    var xslDoc = jsx3.xml.XslDocument.wrap(xslfile);
    var newDstFilterSetXML = xslDoc.transform(objXML);        
    var newDstFilterSetNode = new jsx3.xml.Document();
    newDstFilterSetNode.loadXML(newDstFilterSetXML);
    
    var oldDstFilterSetNode = MediaStreamFilterNode.selectSingleNode(".//DstFilterSet");              
    var dstFilterSetParentNode = MediaStreamFilterNode.selectSingleNode(".//DstFilterSet").getParent();
    dstFilterSetParentNode.replaceNode(newDstFilterSetNode,oldDstFilterSetNode);

    var strXMLCacheId = cmpBlkStreamProfileFilterMatrix.BlkJSX.getXMLId();
    var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
    var xslfile = PNameSpace.loadInclude("jsxuser:///JSXAPPS/IPVS_AdminUI/xsl/CDFToPolicyStreamProfileFilter.xsl","ProfileFilterSetNode","xsl");
    var xslDoc = jsx3.xml.XslDocument.wrap(xslfile);
    var newStreamProfileFilterSetXML = xslDoc.transform(objXML);        
    var newStreamProfileFilterSetNode = new jsx3.xml.Document();
    newStreamProfileFilterSetNode.loadXML(newStreamProfileFilterSetXML);
    
    var oldStreamProfileFilterSetNode = MediaStreamFilterNode.selectSingleNode(".//ProfileFilterSet");              
    var streamProfileFilterSetParentNode = MediaStreamFilterNode.selectSingleNode(".//ProfileFilterSet").getParent();
    streamProfileFilterSetParentNode.replaceNode(newStreamProfileFilterSetNode,oldStreamProfileFilterSetNode);

    var strXMLCacheId = cmpBlkRoutingRelayFilterMatrix.BlkJSX.getXMLId();
    var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
    var xslfile = PNameSpace.loadInclude("jsxuser:///JSXAPPS/IPVS_AdminUI/xsl/CDFToPolicyRelayFilter.xsl","RelayFilterSetNode","xsl");
    var xslDoc = jsx3.xml.XslDocument.wrap(xslfile);
    var newRelayFilterSetXML = xslDoc.transform(objXML);        
    var newRelayFilterSetNode = new jsx3.xml.Document();
    newRelayFilterSetNode.loadXML(newRelayFilterSetXML);
    
    var oldRelayFilterSetNode = MediaStreamFilterNode.selectSingleNode(".//RelayFilterSet");              
    var relayFilterSetParentNode = MediaStreamFilterNode.selectSingleNode(".//RelayFilterSet").getParent();
    relayFilterSetParentNode.replaceNode(newRelayFilterSetNode,oldRelayFilterSetNode);
    
    var strXMLCacheId = cmpBlkRoutingRuleKeyMatrix.BlkJSX.getXMLId();
    var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
    var xslfile = PNameSpace.loadInclude("jsxuser:///JSXAPPS/IPVS_AdminUI/xsl/CDFToPolicyRuleKeyFilter.xsl","RuleKeyTagFilterSetNode","xsl");
    var xslDoc = jsx3.xml.XslDocument.wrap(xslfile);
    var newRuleKeyTagFilterSetXML = xslDoc.transform(objXML);        
    var newRuleKeyTagFilterSetNode = new jsx3.xml.Document();
    newRuleKeyTagFilterSetNode.loadXML(newRuleKeyTagFilterSetXML);
    
    var oldRuleKeyTagFilterSetNode = MediaStreamFilterNode.selectSingleNode(".//RuleKeyTagFilterSet");              
    var ruleKeyTagFilterSetParentNode = MediaStreamFilterNode.selectSingleNode(".//RuleKeyTagFilterSet").getParent();
    ruleKeyTagFilterSetParentNode.replaceNode(newRuleKeyTagFilterSetNode,oldRuleKeyTagFilterSetNode);


    if( MediaStreamFilterNode == null ) return;
    // set NID in MediaStreamFilter node if replace mode
    if( SetMode == "Replace" ) {
      MediaStreamFilterNode.setAttribute("NID", MediaStreamFilterNID);
    }

    // do set request for MediaStreamFilter              
    var reqSetMediaStreamFilterObject = new ReqSetObject(SetMode, "MediaStreamFilter", MediaStreamFilterNode, 
                                                          {action:"FNCCALL", context:this, 
                                                             fnc:function(respCode, respVal) {
                                                                   if(respCode == "SUCCESS" && respVal != null) {
                                                                     dlgEditMediaStreamFilter_Destroy();
                                                                     topic_Publish(tApp, "MediaStreamFilterDataUpdate", "ADD");
                                                                   } else {
                                                                     var codeErrorMsg = CommonErrorCodeArr[respCode].errorDesc;
                                                                     var ErrorMsg = "Error: "+ SetMsg +" request failed \n";
                                                                     ErrorMsg += "Error description: " + codeErrorMsg + "\n";
                                                                     ErrorMsg += "Error code: " + respCode;
                                                                     alert(ErrorMsg);
                                                                   }
                                                                 }
                                                          }                                                           
                                                        );
                                                      
    if( reqSetMediaStreamFilterObject != null) {
      reqSetMediaStreamFilterObject.sendRequest(this);
    }
  } 
      
  function cmpDlgEditMediaStreamFilterValidate() {
    var valStreamFilterName = getJsxByName([BlkJSX,"txtPolicyStreamFilterName"]).getValue();

    var policyName = checkValue(valStreamFilterName , [BlkJSX,"txtPolicyStreamFilterName"] , "Please enter policy name");
    if(policyName) {
      if( MediaStreamFilterNID == "DEFAULT_MEDIASTREAMFILTER_NID" ) {
        var chkDuplicateName = chkForNameExistence(valStreamFilterName, [BlkJSX,"txtPolicyStreamFilterName"], "mtxpolicyMediaStreamFilterList", "Duplicate StreamFilter name. Please enter some other StreamFilter name" );
        if(chkDuplicateName) {
          return true;
        }
      } else {
        return true;
      }
    }
          
    return false;
  }
  


  function DisplayEditValuesForMediaStreamFilter(MediaStreamFilterNID) {       
    var streamNode = topic_GetNode(tCache, ".//*[@NID='" + MediaStreamFilterNID + "']");

    /*var version = streamNode.getAttribute("Ver");
    var versionBlkJSX = getJsxByName([BlkJSX,"txtStreamFilterXMLVersion"]);
    versionBlkJSX.setValue(version);
    
    var revision = streamNode.getAttribute("Rev");
    var revisionBlkJSX = getJsxByName([BlkJSX,"txtStreamFilterXMLRevision"]);
    revisionBlkJSX.setValue(revision);

    var serviceVer = streamNode.getAttribute("serviceVer");
    var serviceVerBlkJSX = getJsxByName([BlkJSX,"txtStreamFilterXMLServiceVersion"]);
    serviceVerBlkJSX.setValue(serviceVer);    
    
    var streamFilterName = streamNode.selectSingleNode(".//Name").getValue();
    var filterNameBlkJSX = getJsxByName([BlkJSX,"txtPolicyStreamFilterName"]);
    filterNameBlkJSX.setValue(streamFilterName);*/

    var routingNID = streamNode.getAttribute("NID");
    var strXMLCacheId = cmpCDFRoutingPolicy.BlkJSX.getXMLId();
    var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
    objXML.loadXML(streamNode.toString());
    cmpCDFRoutingPolicy.BlkJSX.setSourceXML(objXML);         
    cmpCDFRoutingPolicy.BlkJSX.setCDFId(routingNID);

    var sourceOpListIter  = streamNode.selectNodeIterator(".//SrcFilterSet/SrcFilter");
    var objSrcDoc = jsx3.xml.CDF.Document.newDocument();
    while( sourceOpListIter.hasNext() ) {
      var sourceOpListIterNode = sourceOpListIter.next();
      var UserFilterNID = sourceOpListIterNode.selectSingleNode(".//UserFilterNID").getValue();
      var ResourceFilterNID = sourceOpListIterNode.selectSingleNode(".//MediaStreamResourceFilterNID").getValue();
      if( (UserFilterNID != "") && (ResourceFilterNID != "") ) {
        var UserFilterName = getTagObjectFilterName(UserFilterNID);
        var ResourceFilterName = getTagObjectFilterName(ResourceFilterNID);

        var o = new Object();
        o.jsxid = jsx3.xml.CDF.getKey(); 
        o.UserFilterName = UserFilterName;
        o.UserFilterNID = UserFilterNID;
        o.ResourceFilterName = ResourceFilterName;
        o.ResourceFilterNID = ResourceFilterNID;
        objSrcDoc.insertRecord(o);
      }
    }
    var strXMLCacheId = cmpBlkRoutingSrcFilterMatrix.BlkJSX.getXMLId();
    var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
    objXML.loadXML(objSrcDoc.toString());
    cmpBlkRoutingSrcFilterMatrix.BlkJSX.setSourceXML(objXML);
    cmpBlkRoutingSrcFilterMatrix.BlkJSX.repaintData();

    var destOpListIter  = streamNode.selectNodeIterator(".//DstFilterSet/DstFilter");
    var objDstDoc = jsx3.xml.CDF.Document.newDocument();
    while( destOpListIter.hasNext() ) {
      var destOpListIterNode = destOpListIter.next();
      var UserFilterNID = destOpListIterNode.selectSingleNode(".//UserFilterNID").getValue();
      var ResourceFilterNID = destOpListIterNode.selectSingleNode(".//MediaStreamResourceFilterNID").getValue();
      if( (UserFilterNID != "") && (ResourceFilterNID != "") ) {
        var UserFilterName = getTagObjectFilterName(UserFilterNID);
        var ResourceFilterName = getTagObjectFilterName(ResourceFilterNID);

        var o = new Object();
        o.jsxid = jsx3.xml.CDF.getKey(); 
        o.UserFilterName = UserFilterName;
        o.UserFilterNID = UserFilterNID;
        o.ResourceFilterName = ResourceFilterName;
        o.ResourceFilterNID = ResourceFilterNID;
        objDstDoc.insertRecord(o);
      }
    }
    var strXMLCacheId = cmpBlkRoutingDstFilterMatrix.BlkJSX.getXMLId();
    var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
    objXML.loadXML(objDstDoc.toString());
    cmpBlkRoutingDstFilterMatrix.BlkJSX.setSourceXML(objXML);
    cmpBlkRoutingDstFilterMatrix.BlkJSX.repaintData();

    var profileOpListIter  = streamNode.selectNodeIterator(".//ProfileFilterSet/ProfileFilter");
    var objProfileDoc = jsx3.xml.CDF.Document.newDocument();
    while( profileOpListIter.hasNext() ) {
      var profileOpListIterNode = profileOpListIter.next();
      var UserFilterNID = profileOpListIterNode.selectSingleNode(".//MediaStreamProfileFilterNID").getValue();          
      if( (UserFilterNID != "")  ) {
        var UserFilterName = getTagObjectFilterName(UserFilterNID);
        var o = new Object();
        o.jsxid = jsx3.xml.CDF.getKey();
        o.streamProfileFilterName = UserFilterName ;
        o.streamProfileNID = UserFilterNID ;
        objProfileDoc.insertRecord(o);
      }
    }
    var strXMLCacheId = cmpBlkStreamProfileFilterMatrix.BlkJSX.getXMLId();
    var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
    objXML.loadXML(objProfileDoc.toString());
    cmpBlkStreamProfileFilterMatrix.BlkJSX.setSourceXML(objXML);
    cmpBlkStreamProfileFilterMatrix.BlkJSX.repaintData();
    
    var relayListIter  = streamNode.selectNodeIterator(".//RelayFilterSet/RelayFilter");
    var objRelayDoc = jsx3.xml.CDF.Document.newDocument();
    while( relayListIter.hasNext() ) {
      var relayListIterNode = relayListIter.next();
      var UserFilterNID = relayListIterNode.selectSingleNode(".//UserFilterNID").getValue();
      var ResourceFilterNID = relayListIterNode.selectSingleNode(".//MediaStreamResourceFilterNID").getValue();
      if( (UserFilterNID != "") && (ResourceFilterNID != "") ) {
        var UserFilterName = getTagObjectFilterName(UserFilterNID);
        var ResourceFilterName = getTagObjectFilterName(ResourceFilterNID);
        var o = new Object();
        o.jsxid = jsx3.xml.CDF.getKey(); 
        o.UserFilterName = UserFilterName;
        o.UserFilterNID = UserFilterNID;
        o.ResourceFilterName = ResourceFilterName;
        o.ResourceFilterNID = ResourceFilterNID;
        objRelayDoc.insertRecord(o);
      }
    }
    var strXMLCacheId = cmpBlkRoutingRelayFilterMatrix.BlkJSX.getXMLId();
    var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
    objXML.loadXML(objRelayDoc.toString());
    cmpBlkRoutingRelayFilterMatrix.BlkJSX.setSourceXML(objXML);
    cmpBlkRoutingRelayFilterMatrix.BlkJSX.repaintData();

    /*var pvrListIter  = streamNode.selectNodeIterator(".//PVRFilterSet/PVRFilter");
    var mtxStreamPVRFilterSelBlkJSX = getJsxByName([BlkJSX,"mtxStreamPVRFilterSel"]);
    mtxStreamPVRFilterSelBlkJSX.resetCacheData();
    while( pvrListIter.hasNext() ) {
      var pvrListIterNode = pvrListIter.next();
      var UserFilterNID = pvrListIterNode.selectSingleNode(".//UserFilterNID").getValue();
      var ResourceFilterNID = pvrListIterNode.selectSingleNode(".//MediaStreamResourceFilterNID").getValue();
      if( (UserFilterNID != "") && (ResourceFilterNID != "") ) {
        var UserFilterName = getTagObjectFilterName(UserFilterNID);
        var ResourceFilterName = getTagObjectFilterName(ResourceFilterNID);
        o = new Object();
        o.jsxid = jsx3.xml.CDF.getKey();
        o.jsxtextOne = UserFilterName ;
        o.jsxtextOneNID = UserFilterNID ;
        o.jsxtextTwo = ResourceFilterName ;
        o.jsxtextTwoNID = ResourceFilterNID ;
        o.jsxAt = "@";

        mtxStreamPVRFilterSelBlkJSX.insertRecord(o, null, false);
      }
    }
    mtxStreamPVRFilterSelBlkJSX.repaintData();*/    

    var ruleKeyListIter  = streamNode.selectNodeIterator(".//RuleKeyTagFilterSet/RuleKeyTagFilter");
    var objRuleKeyDoc = jsx3.xml.CDF.Document.newDocument();
    while( ruleKeyListIter.hasNext() ) {
      var ruleKeyListIterNode = ruleKeyListIter.next();
      var LHName = ruleKeyListIterNode.selectSingleNode(".//RuleKeyLHTagName/RuleKey").getValue();
      var LHTagName = ruleKeyListIterNode.selectSingleNode(".//RuleKeyLHTagName/TagName").getValue();
      var Operator = ruleKeyListIterNode.selectSingleNode(".//MatchOp").getValue();
      var RHName = ruleKeyListIterNode.selectSingleNode(".//RuleKeyRHTagName/RuleKey").getValue();
      var RHTagName = ruleKeyListIterNode.selectSingleNode(".//RuleKeyRHTagName/TagName").getValue();
      if( LHTagName != "") {
        var o = new Object();
        o.jsxid = jsx3.xml.CDF.getKey();
        o.LHName = LHName;
        o.LHTagName = splitTagName(LHTagName);
        o.Operator = Operator;
        o.RHName = RHName;
        o.RHTagName = splitTagName(RHTagName);
        objRuleKeyDoc.insertRecord(o);
      }
    }
    var strXMLCacheId = cmpBlkRoutingRuleKeyMatrix.BlkJSX.getXMLId();
    var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
    objXML.loadXML(objRuleKeyDoc.toString());
    cmpBlkRoutingRuleKeyMatrix.BlkJSX.setSourceXML(objXML);
    cmpBlkRoutingRuleKeyMatrix.BlkJSX.repaintData(); 
    
    // populate the select with the default values
    updateSelectTagObjectFilters();
    updateSelectGroupTag();
    
    if( MediaStreamFilterNID != "DEFAULT_MEDIASTREAMFILTER_NID" ) {
      getJsxByName([BlkJSX,"txtPolicyStreamFilterName"]).setEnabled(jsx3.gui.Form.STATEDISABLED, true);
    }
    else{
      getJsxByName([BlkJSX,"txtPolicyStreamFilterName"]).focus();
    }

  } // end DisplayEditValuesForMediaStreamFilter
  
  function updateSelectTagObjectFilters() {
    getFilterToSelectElement([BlkJSX,"selStreamFilterSrc_Users"],"User");
    getFilterToSelectElement([BlkJSX,"selStreamFilterSrc_Resources"],"Resource");
    getFilterToSelectElement([BlkJSX,"selStreamFilterDst_Users"],"User");
    getFilterToSelectElement([BlkJSX,"selStreamFilterDst_Resources"],"Resource");
    getFilterToSelectElement([BlkJSX,"selStreamFilterProfile"],"MediaStreamProfile");
    //getFilterToSelectElement([BlkJSX,"selStreamFilterPVR_Users"],"User");
    //getFilterToSelectElement([BlkJSX,"selStreamFilterPVR_Resources"],"Resource");
    getFilterToSelectElement([BlkJSX,"selStreamFilterRelay_Users"],"User");
    getFilterToSelectElement([BlkJSX,"selStreamFilterRelay_Resources"],"Resource");  
  }
  
  function updateSelectGroupTag() {
    getTagDefToSelectElement([BlkJSX,"selStreamFilterRuleKeyGroupTagLH"],"ALL");
    getTagDefToSelectElement([BlkJSX,"selStreamFilterRuleKeyGroupTagRH"],"ALL");  
  }
  
}

