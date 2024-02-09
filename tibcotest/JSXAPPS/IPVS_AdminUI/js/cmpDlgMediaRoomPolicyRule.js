// this code has some potential issues 
// - w.r.t object reference counts - when objects created in here go out of scope etc
// - loading sync issues - dlg may not be loaded before the object init methods are called

function dlgEditCollab_Create(CollabNID) {

  // deal with defaults for add
  if( CollabNID == null ) CollabNID = "DEFAULT_COLLAB_NID";
  
  //closeDialogIfExists("DialogCollab");
  // load view - dialog component file
  var DlgFile = "components/policy/newCollaborationPolicy.xml";
  var BlkJSX = PNameSpace.getBodyBlock().load(DlgFile);

  // setup a local topic for dlg specific messaging       
  var tDlg = "Dialog_" + jsx3.xml.CDF.getKey()
  topic_Create(tDlg);

  // setup a listner for it     
  var eventDlg = new EventListner(tDlg);
  var orgOrderNumber = "";

  // setup a private/dlg specific app event handler
  dlgEditCollab_eventAppTagObject = new EventListner( tApp );
  dlgEditCollab_eventAppGroupTag = new EventListner( tApp );
  dlgEditCollab_eventAppMediaStream = new EventListner( tApp );  
  
  var dlgText = "Add Connection Policy";
  if( CollabNID != "DEFAULT_COLLAB_NID" ){
    dlgText = "Edit Connection Policy";
  }
  var windowBarBlkJSX = getJsxByName([BlkJSX, "windowBar"]);
  windowBarBlkJSX.setText(dlgText, true);
  closeDialogIfExists(dlgText);

  var cmpBlkMediaRoomFilterMatrix = new CmpBlock([BlkJSX,"mtxCollabUserFilterSel"]);
  cmpBlkMediaRoomFilterMatrix.BlkJSX.setXMLTransformers("xsl/PolicyUserFilterToCDF.xsl");
  cmpBlkMediaRoomFilterMatrix.BlkJSX.setSelectionModel(jsx3.gui.Matrix.SELECTION_ROW);

  var cmpBlkMediaRoomSrcFilterMatrix = new CmpBlock([BlkJSX,"mtxCollabSourceFilterSel"]);
  cmpBlkMediaRoomSrcFilterMatrix.BlkJSX.setXMLTransformers("xsl/PolicySrcFilterToCDF.xsl");
  cmpBlkMediaRoomSrcFilterMatrix.BlkJSX.setSelectionModel(jsx3.gui.Matrix.SELECTION_ROW);

  var cmpBlkMediaRoomDstFilterMatrix = new CmpBlock([BlkJSX,"mtxCollabDstFilterSel"]);
  cmpBlkMediaRoomDstFilterMatrix.BlkJSX.setXMLTransformers("xsl/PolicyDstFilterToCDF.xsl");
  cmpBlkMediaRoomDstFilterMatrix.BlkJSX.setSelectionModel(jsx3.gui.Matrix.SELECTION_ROW);

  var cmpBlkMediaRoomRuleKeyMatrix = new CmpBlock([BlkJSX,"mtxCollabPolicyRuleKey"]);
  cmpBlkMediaRoomRuleKeyMatrix.BlkJSX.setXMLTransformers("xsl/PolicyRuleKeyFilterToCDF.xsl");
  cmpBlkMediaRoomRuleKeyMatrix.BlkJSX.setSelectionModel(jsx3.gui.Matrix.SELECTION_ROW);

  var cmpCDFMediaRoomPolicy = new Component([BlkJSX,"cdfDlgMediaRoomPolicy"]);
  cmpCDFMediaRoomPolicy.BlkJSX.setXMLTransformers("xsl/MediaRoomPolicy.xsl");

  function dlgEditCollab_Destroy() {
    // destroy any local eventlistners
    eventDlg.destroy();
    dlgEditCollab_eventAppTagObject.destroy();
    dlgEditCollab_eventAppMediaStream.destroy();
    dlgEditCollab_eventAppGroupTag.destroy();    
    
    // destroy all local components
    cmpDlgEditCollab.destroy();
    
    // destroy the local topic
    topic_Destroy(tDlg);
    
    // close dialog
    BlkJSX.doClose();
  }

  // event subscribtion: if new tagobjectfilter is added from select box and update all the
  // select boxes in the dialog after add is finished
  dlgEditCollab_eventAppTagObject.subscribe("TagObjectFilterDataState", null, this,
                                              function() {
                                                updateSelectTagObjectFilters();
                                              } );
  
  dlgEditCollab_eventAppMediaStream.subscribe("MediaStreamFilterDataState", null, this,
                                                function() {
                                                  updateSelectStreamFilter();
                                                } );
  
  // event subscribtion: if new grouptag is added from select box and update all the
  // select boxes in the dialog after add is finished                      
  dlgEditCollab_eventAppGroupTag.subscribe("GroupTagDataState", null, this,
                                            function() {
                                              updateSelectGroupTag();
                                            } );
        
  var cmpSelUserFilter = new CmpSelect([BlkJSX,"selCollabPolicyUserFilter"],
                                               null,
                                               {action:"FNCCALL", 
                                               context:null, 
                                               fnc:function() {
                                                  var selVal = cmpSelUserFilter.BlkJSX.getValue();
                                                  if (jsx3.util.strEmpty(selVal)) return;
                                                  var strXMLCacheId = cmpSelUserFilter.BlkJSX.getXMLId();
                                                  var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
                                                  if (jsx3.util.strEmpty(objXML)) return;
                                                  var record = objXML.selectSingleNode("//record[@jsxid='" + selVal + "']");
                                                  checkAddForNewFilter(record, "User");
                                               }
                                              });

  var cmpSelResourceFilter = new CmpSelect([BlkJSX,"selCollabPolicyResourceFilter"],
                                               null,
                                               {action:"FNCCALL", 
                                               context:null, 
                                               fnc:function() {
                                                  var selVal = cmpSelResourceFilter.BlkJSX.getValue();
                                                  if (jsx3.util.strEmpty(selVal)) return;
                                                  var strXMLCacheId = cmpSelResourceFilter.BlkJSX.getXMLId();
                                                  var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
                                                  if (jsx3.util.strEmpty(objXML)) return;
                                                  var record = objXML.selectSingleNode("//record[@jsxid='" + selVal + "']");
                                                  checkAddForNewFilter(record, "Resource");
                                               }
                                              });

  var cmpBtnAddMediaRoomFilter = new CmpButton([BlkJSX,"btnAddCollabUserFilter"],
                                                 null, 
                                                 {action:"FNCCALL", 
                                                  context:null, 
                                                  fnc:function() {
                                                    addDatatoMatrix(this.userFilterBlkJSX, this.resourceFilterBlkJSX, this.mediaRoomFilterMatrix);
                                                  }
                                                 });
  cmpBtnAddMediaRoomFilter.userFilterBlkJSX = cmpSelUserFilter.BlkJSX;
  cmpBtnAddMediaRoomFilter.resourceFilterBlkJSX = cmpSelResourceFilter.BlkJSX;
  cmpBtnAddMediaRoomFilter.mediaRoomFilterMatrix = cmpBlkMediaRoomFilterMatrix.BlkJSX;

  var cmpSelSrcUserFilter = new CmpSelect([BlkJSX,"selCollabSrc_Users"],
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

  var cmpSelSrcResourceFilter = new CmpSelect([BlkJSX,"selCollabSrc_Resources"],
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

  var cmpBtnAddMediaRoomSrcFilter = new CmpButton([BlkJSX,"btnAddCollabrSrc"],
                                                 null, 
                                                 {action:"FNCCALL", 
                                                  context:null, 
                                                  fnc:function() {
                                                      addDatatoMatrix(this.userFilterBlkJSX, this.resourceFilterBlkJSX, this.mediaRoomSrcFilterMatrix);
                                                  }
                                                 });
  cmpBtnAddMediaRoomSrcFilter.userFilterBlkJSX = cmpSelSrcUserFilter.BlkJSX;
  cmpBtnAddMediaRoomSrcFilter.resourceFilterBlkJSX = cmpSelSrcResourceFilter.BlkJSX;
  cmpBtnAddMediaRoomSrcFilter.mediaRoomSrcFilterMatrix = cmpBlkMediaRoomSrcFilterMatrix.BlkJSX;
  
  var cmpSelDstUserFilter = new CmpSelect([BlkJSX,"selCollabDst_Users"],
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

  var cmpSelDstResourceFilter = new CmpSelect([BlkJSX,"selCollabDst_Resources"],
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

  var cmpBtnAddMediaRoomDstFilter = new CmpButton([BlkJSX,"btnAddCollabDst"],
                                                 null, 
                                                 {action:"FNCCALL", 
                                                  context:null, 
                                                  fnc:function() {
                                                      addDatatoMatrix(this.userFilterBlkJSX, this.resourceFilterBlkJSX, this.mediaRoomDstFilterMatrix);
                                                  }
                                                 });
  cmpBtnAddMediaRoomDstFilter.userFilterBlkJSX = cmpSelDstUserFilter.BlkJSX;
  cmpBtnAddMediaRoomDstFilter.resourceFilterBlkJSX = cmpSelDstResourceFilter.BlkJSX;
  cmpBtnAddMediaRoomDstFilter.mediaRoomDstFilterMatrix = cmpBlkMediaRoomDstFilterMatrix.BlkJSX;
  
  var cmpSelRuleKeyLHTag = new CmpSelect([BlkJSX,"selCollabRuleKeyGroupTagLH"],
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
  var cmpSelRuleKeyRHTag = new CmpSelect([BlkJSX,"selCollabRuleKeyGroupTagRH"],
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

  var cmpBtnAddMediaRoomRuleKey = new CmpButton([BlkJSX,"btnAddCollabPolicyRuleKey"],
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
                                                          this.mediaRoomRuleKeyMatrix.insertRecord(o, null, false);
                                                        }
                                                      }
                                                      this.mediaRoomRuleKeyMatrix.repaintData();
                                                      this.LHFilterRuleKeyBlkJSX.setValue("Select LH Rule");
                                                      this.LHTagRuleKeyBlkJSX.setValue("Select Tag Name");
                                                      this.OperatorRuleKeyBlkJSX.setValue("Select Operation");
                                                      this.RHFilterRuleKeyBlkJSX.setValue("Select RH Rule");
                                                      this.RHTagRuleKeyBlkJSX.setValue("Select Tag Name");
                                                  }
                                                 });
  cmpBtnAddMediaRoomRuleKey.LHFilterRuleKeyBlkJSX = getJsxByName([BlkJSX,"selCollabRuleKeyLH"]);
  cmpBtnAddMediaRoomRuleKey.LHTagRuleKeyBlkJSX = cmpSelRuleKeyLHTag.BlkJSX;
  cmpBtnAddMediaRoomRuleKey.OperatorRuleKeyBlkJSX = getJsxByName([BlkJSX,"selCollabRuleKeyOp"]);
  cmpBtnAddMediaRoomRuleKey.RHFilterRuleKeyBlkJSX = getJsxByName([BlkJSX,"selCollabRuleKeyRH"]);
  cmpBtnAddMediaRoomRuleKey.RHTagRuleKeyBlkJSX = cmpSelRuleKeyRHTag.BlkJSX;
  cmpBtnAddMediaRoomRuleKey.mediaRoomRuleKeyMatrix = cmpBlkMediaRoomRuleKeyMatrix.BlkJSX;
  
  var cmpBtnEditCollabClose = new CmpButton([BlkJSX,"btnDialogCloseX"],
                                             null, 
                                             {action:"FNCCALL", 
                                              context:null, 
                                              fnc:function() {
                                                dlgEditCollab_Destroy();
                                              }
                                             });
            
  var cmpBtnEditCollabCancel = new CmpButton([BlkJSX,"btnCollabCancel"],
                                               null, 
                                               {action:"FNCCALL", 
                                                context:null, 
                                                fnc:function() {
                                                  dlgEditCollab_Destroy();
                                                }
                                               });
            
  var cmpBtnEditCollabDone = new CmpButton([BlkJSX,"btnCollabDone"],
                                             null, 
                                             {action:"FNCCALL", 
                                              context:null, 
                                              fnc:function() {
                                                cmpDlgEditCollabSetCollab.call(this, CollabNID);
                                              }
                                             });
                                                                                         
  var cmpDlgEditCollab = new CmpContainer([BlkJSX,"DialogCollab"],
                                          [ {cmp:cmpBtnEditCollabClose},
                                            {cmp:cmpBtnEditCollabCancel},
                                            {cmp:cmpBtnEditCollabDone},
                                            {cmp:cmpBlkMediaRoomFilterMatrix},
                                            {cmp:cmpBlkMediaRoomSrcFilterMatrix},
                                            {cmp:cmpBlkMediaRoomDstFilterMatrix},
                                            {cmp:cmpBlkMediaRoomRuleKeyMatrix},
                                            {cmp:cmpSelUserFilter},
                                            {cmp:cmpSelResourceFilter},
                                            {cmp:cmpSelSrcUserFilter},
                                            {cmp:cmpSelDstUserFilter},
                                            {cmp:cmpSelSrcResourceFilter},
                                            {cmp:cmpSelDstResourceFilter},
                                            {cmp:cmpSelRuleKeyLHTag},
                                            {cmp:cmpSelRuleKeyRHTag},
                                            {cmp:cmpBtnAddMediaRoomFilter},
                                            {cmp:cmpBtnAddMediaRoomSrcFilter},
                                            {cmp:cmpBtnAddMediaRoomDstFilter},
                                            {cmp:cmpBtnAddMediaRoomRuleKey},
                                            {cmp:cmpCDFMediaRoomPolicy}
                                          ],
                                          [{action:"TOPICEVENT", topic:tDlg, eventvar:"EditCollabDataState", eventval:CollabNID}]);                            
                
  cmpDlgEditCollab.BlkJSX = BlkJSX;      
  cmpDlgEditCollab.ObjectNID = CollabNID;   
  cmpDlgEditCollab.getBlkData =  cmpDlgEditCollabGetCollab;
  
  // init and repaint happens after the dialog is loaded
  jsx3.sleep( (function(){
                 this.init();
                 this.getBlkData(this.ObjectNID);
                 }
               ),
               null, cmpDlgEditCollab );  
                     
  // fnc to get dlg data
  function cmpDlgEditCollabGetCollab(CollabNID) { 
    var collabNode = topic_GetNode(tCache, "//*[@NID='" + CollabNID + "']");
    if( collabNode == null ) {
      var reqData = new ReqGetObjectNID(CollabNID, "MediaRoomPolicyRule",
                                         {action:"FNCCALL", context:this, 
                                           fnc:function(respCode, respVal) {
                                                 if(respCode == "SUCCESS" && respVal != null) {
                                                   cmpDlgEditCollabGetCollab(CollabNID);
                                                 } else {
                                                   var codeErrorMsg = CommonErrorCodeArr[respCode].errorDesc;
                                                   var ErrorMsg = "Error: MediaRoom polict get data request failed \n";
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
      //cmpDlgEditCollab.setMapRuleData("Collab", "IN", "NID", CollabNID);
      //topic_Publish(tDlg, "EditCollabDataState", CollabNID);

      // using the old repaint of the dialog
      DisplayEditValuesForCollab(CollabNID);
    }
  }
       
  // fnc to set dlg data - runs in the context of the OK button
  function cmpDlgEditCollabSetCollab(CollabNID) {
    // validate input 
    if( cmpDlgEditCollabValidate() != true ) return;
    // decide add or update
    var SetMsg = "Edit MediaRoom policy";
    var SetMode = "Replace";
    if( CollabNID == "DEFAULT_COLLAB_NID" ) {
      SetMsg = "Add MediaRoom policy";
      SetMode = "Add";
    }

    createMediaRoomRoleDoc();

    // get Collab node info from UI
    var collabNode = null;
    
    // write the data back to the CDF
    cmpCDFMediaRoomPolicy.BlkJSX.write();

    var strXMLCacheId = cmpCDFMediaRoomPolicy.BlkJSX.getXMLId();
    var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);      
    var xslfile = PNameSpace.loadInclude("JSXAPPS/IPVS_AdminUI/xsl/MediaRoomPolicyRequest.xsl","MediaRoomPolicyRequest","xsl");
    var xslDoc = jsx3.xml.XslDocument.wrap(xslfile);
    var mediaRoomPolicyConfigXML = xslDoc.transform(objXML);
    collabNode = new jsx3.xml.Document();
    collabNode.loadXML(mediaRoomPolicyConfigXML);

    var strXMLCacheId = cmpBlkMediaRoomFilterMatrix.BlkJSX.getXMLId();
    var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
    var xslfile = PNameSpace.loadInclude("jsxuser:///JSXAPPS/IPVS_AdminUI/xsl/CDFToPolicyUserFilter.xsl","ServiceOperatorFilterSetNode","xsl");
    var xslDoc = jsx3.xml.XslDocument.wrap(xslfile);
    var newServiceOperatorFilterSetXML = xslDoc.transform(objXML);        
    var newServiceOperatorFilterSetNode = new jsx3.xml.Document();
    newServiceOperatorFilterSetNode.loadXML(newServiceOperatorFilterSetXML);
    
    var oldServiceOperatorFilterSetNode = collabNode.selectSingleNode(".//ServiceOperatorFilterSet");              
    var serviceOperatorFilterSetParentNode = collabNode.selectSingleNode(".//ServiceOperatorFilterSet").getParent();
    serviceOperatorFilterSetParentNode.replaceNode(newServiceOperatorFilterSetNode,oldServiceOperatorFilterSetNode);

    var mediaRoomAdminRoleSetNode = PNameSpace.getCache().getDocument("MediaRoomRoleSetDoc");
    var xslfile = PNameSpace.loadInclude("jsxuser:///JSXAPPS/IPVS_AdminUI/xsl/MediaRoomRoleSet.xsl","MediaRoomRoleSetNode","xsl");
    var xslDoc = jsx3.xml.XslDocument.wrap(xslfile);
    var newMediaRoomAdminRoleSetXML = xslDoc.transform(mediaRoomAdminRoleSetNode);        
    var newMediaRoomAdminRoleSetNode = new jsx3.xml.Document();
    newMediaRoomAdminRoleSetNode.loadXML(newMediaRoomAdminRoleSetXML);

    var oldMediaRoomAdminRoleSetNode = collabNode.selectSingleNode(".//MediaRoomRoleSet");              
    var mediaRoomAdminRoleSetParentNode = collabNode.selectSingleNode(".//MediaRoomRoleSet").getParent();
    mediaRoomAdminRoleSetParentNode.replaceNode(newMediaRoomAdminRoleSetNode,oldMediaRoomAdminRoleSetNode);

    var strXMLCacheId = cmpBlkMediaRoomSrcFilterMatrix.BlkJSX.getXMLId();
    var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
    var xslfile = PNameSpace.loadInclude("jsxuser:///JSXAPPS/IPVS_AdminUI/xsl/CDFToPolicySrcFilter.xsl","SrcFilterSetNode","xsl");
    var xslDoc = jsx3.xml.XslDocument.wrap(xslfile);
    var newSrcFilterSetXML = xslDoc.transform(objXML);        
    var newSrcFilterSetNode = new jsx3.xml.Document();
    newSrcFilterSetNode.loadXML(newSrcFilterSetXML);
    
    var oldSrcFilterSetNode = collabNode.selectSingleNode(".//SrcFilterSet");              
    var srcFilterSetParentNode = collabNode.selectSingleNode(".//SrcFilterSet").getParent();
    srcFilterSetParentNode.replaceNode(newSrcFilterSetNode,oldSrcFilterSetNode);
    
    var strXMLCacheId = cmpBlkMediaRoomDstFilterMatrix.BlkJSX.getXMLId();
    var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
    var xslfile = PNameSpace.loadInclude("jsxuser:///JSXAPPS/IPVS_AdminUI/xsl/CDFToPolicyDstFilter.xsl","DrcFilterSetNode","xsl");
    var xslDoc = jsx3.xml.XslDocument.wrap(xslfile);
    var newDstFilterSetXML = xslDoc.transform(objXML);        
    var newDstFilterSetNode = new jsx3.xml.Document();
    newDstFilterSetNode.loadXML(newDstFilterSetXML);
    
    var oldDstFilterSetNode = collabNode.selectSingleNode(".//DstFilterSet");              
    var dstFilterSetParentNode = collabNode.selectSingleNode(".//DstFilterSet").getParent();
    dstFilterSetParentNode.replaceNode(newDstFilterSetNode,oldDstFilterSetNode);
    
    var strXMLCacheId = cmpBlkMediaRoomRuleKeyMatrix.BlkJSX.getXMLId();
    var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
    var xslfile = PNameSpace.loadInclude("jsxuser:///JSXAPPS/IPVS_AdminUI/xsl/CDFToPolicyRuleKeyFilter.xsl","RuleKeyTagFilterSetNode","xsl");
    var xslDoc = jsx3.xml.XslDocument.wrap(xslfile);
    var newRuleKeyTagFilterSetXML = xslDoc.transform(objXML);        
    var newRuleKeyTagFilterSetNode = new jsx3.xml.Document();
    newRuleKeyTagFilterSetNode.loadXML(newRuleKeyTagFilterSetXML);
    
    var oldRuleKeyTagFilterSetNode = collabNode.selectSingleNode(".//RuleKeyTagFilterSet");              
    var ruleKeyTagFilterSetParentNode = collabNode.selectSingleNode(".//RuleKeyTagFilterSet").getParent();
    ruleKeyTagFilterSetParentNode.replaceNode(newRuleKeyTagFilterSetNode,oldRuleKeyTagFilterSetNode);

    if( collabNode == null ) return;
    // set NID in collab node if replace mode
    if( SetMode == "Replace" ) {
      collabNode.setAttribute("NID", CollabNID);
    }
    // set the objectfiletre value _ALL_ nid
    var tagObjectFilterListNode = getObjectCacheListNode("TagObjectFilter");
    var allTagObjectNode = tagObjectFilterListNode.selectSingleNode(".//TagObjectFilter[.//Name='_ALL_']");
    var allTagObjectFilterNid = "";
    if(!jsx3.util.strEmpty(allTagObjectNode)) {
      allTagObjectFilterNid = allTagObjectNode.getAttribute("NID");
    }
    var mediaObjectFilterNode = collabNode.selectSingleNode(".//MediaRoomObjectFilterNID");
    if(!jsx3.util.strEmpty(mediaObjectFilterNode)) {
      mediaObjectFilterNode.setValue(allTagObjectFilterNid);
    }
    else{
      var mediaObjectFilterSet = collabNode.selectSingleNode(".//MediaRoomObjectFilterSet");
      mediaObjectFilterNode = mediaObjectFilterSet.createNode(jsx3.xml.Entity.TYPEELEMENT, "MediaRoomObjectFilterNID").setValue(allTagObjectFilterNid);
      mediaObjectFilterSet.appendChild(mediaObjectFilterNode);
    }
    // do set request for Collab              
    var reqSetCollabObject = new ReqSetObject(SetMode, "MediaRoomPolicyRule", collabNode, 
                                                {action:"FNCCALL", context:this, 
                                                   fnc:function(respCode, respVal) {
                                                         if(respCode == "SUCCESS" && respVal != null) {
                                                           dlgEditCollab_Destroy();
                                                           topic_Publish(tApp, "MediaRoomPolicyRuleDataUpdate", "ADD");
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
    if( reqSetCollabObject != null) {
      reqSetCollabObject.sendRequest(this);
    }
  } 
      
  function cmpDlgEditCollabValidate() {
    var valCollabPolicyName = getJsxByName([BlkJSX,"txtCollabPolicyName"]).getValue();
    var valCollabPolicyOrder = getJsxByName([BlkJSX,"txtCollabPolicyOrder"]).getValue();

    var policyName = checkValue(valCollabPolicyName , [BlkJSX,"txtCollabPolicyName"], "Please enter connection policy name");
    if(!policyName){
      return false;
    }
    var checkOrder = checkValue(valCollabPolicyOrder , [BlkJSX,"txtCollabPolicyOrder"] , "Please enter policy order" );
    if(!checkOrder){
      return false;
    }
    var chkDuplicateName = "";
    var chkDuplicatePriority = "";
    
    if( CollabNID == "DEFAULT_COLLAB_NID" ) {
      if(policyName) {
        chkDuplicateName = chkForNameExistence(valCollabPolicyName, [BlkJSX,"txtCollabPolicyName"], "mtxpolicyCollabList", "Duplicate Policy name. Please enter some other Policy name" );
        if(!chkDuplicateName){
          checkOrder = false;
          return false;
        }
      }
      if(chkDuplicateName) {
        checkOrder = checkIntergerValue(valCollabPolicyOrder , [BlkJSX,"txtCollabPolicyOrder"], "Please enter an integer policy order" );
      }
      if(checkOrder) {
        chkDuplicatePriority = chkForOrderExistence(valCollabPolicyOrder, [BlkJSX,"txtCollabPolicyOrder"], "mtxpolicyCollabList", "Policy Order already exists");
      }
      if(chkDuplicatePriority) {
        return true;
      }      
    } else {
      checkOrder = checkIntergerValue(valCollabPolicyOrder , [BlkJSX,"txtCollabPolicyOrder"], "Please enter an integer policy order" );
      if(checkOrder) {
        if( orgOrderNumber != valCollabPolicyOrder) {
          chkDuplicatePriority = chkForOrderExistence(valCollabPolicyOrder, [BlkJSX,"txtCollabPolicyOrder"], "mtxpolicyCollabList", "Policy Order already exists");
        } else {
          return true;
        }
      }
      if(chkDuplicatePriority) {
        return true;
      }      
    }
      
    return false;
  }

  function DisplayEditValuesForCollab(CollabNID){
    var collabNode = topic_GetNode(tCache, ".//*[@NID='" + CollabNID + "']");
    
    /*var version = collabNode.getAttribute("Ver");
    var versionBlkJSX = getJsxByName([BlkJSX,"txtMediaRoomPolicyXMLVersion"]);
    versionBlkJSX.setValue(version);
    
    var revision = collabNode.getAttribute("Rev");
    var revisionBlkJSX = getJsxByName([BlkJSX,"txtMediaRoomPolicyXMLRevision"]);
    revisionBlkJSX.setValue(revision);

    var serviceVer = collabNode.getAttribute("serviceVer");
    var serviceVerBlkJSX = getJsxByName([BlkJSX,"txtMediaRoomPolicyXMLServiceVer"]);
    serviceVerBlkJSX.setValue(serviceVer);    
    
    var jsxtext = collabNode.selectSingleNode(".//rulename").getValue();
    var filterNameBlkJSX = getJsxByName([BlkJSX,"txtCollabPolicyName"]);
    filterNameBlkJSX.setValue(jsxtext);    

    var index = collabNode.selectSingleNode(".//order").getValue();
    var filterOrderBlkJSX = getJsxByName([BlkJSX,"txtCollabPolicyOrder"]);
    filterOrderBlkJSX.setValue(index);      

    //
    var policyValid = collabNode.selectSingleNode(".//valid").getValue();
    var filterActiveBlkJSX = getJsxByName([BlkJSX,"chkCollabPolicyActive"]);    
    if(policyValid == "true") {
      filterActiveBlkJSX.setChecked(jsx3.gui.CheckBox.CHECKED);
    } else {
      filterActiveBlkJSX.setChecked(jsx3.gui.CheckBox.UNCHECKED);
    }

    //
    var jsxUserPolicyType = collabNode.selectSingleNode(".//allow").getValue();
    var filterTypeAllowBlkJSX = getJsxByName([BlkJSX,"radioCollabPolicyTypeAllow"]);
    filterTypeAllowBlkJSX.setSelected(jsx3.gui.RadioButton.SELECTED);  
//    var filterTypeDenyBlkJSX = getJsxByName([BlkJSX,"radioCollabPolicyTypeDeny"]);  
//    if(jsxUserPolicyType == "true") {
//      filterTypeAllowBlkJSX.setSelected(jsx3.gui.RadioButton.SELECTED);
//      filterTypeDenyBlkJSX.setSelected(jsx3.gui.RadioButton.UNSELECTED);
//    } else {
//      filterTypeAllowBlkJSX.setSelected(jsx3.gui.RadioButton.UNSELECTED);
//      filterTypeDenyBlkJSX.setSelected(jsx3.gui.RadioButton.SELECTED);
//    }
    
    //
    var jsxCollabStreamFilterSpec = collabNode.selectSingleNode(".//MediaRoomStreamFilterSpec/MediaRoomStreamFilterListLookupMode").getValue();
    var filterSpecToListBlkJSX = getJsxByName([BlkJSX,"radioCollabPolicyRestrictToList"]);
    var filterSpecSeqBlkJSX = getJsxByName([BlkJSX,"radioCollabPolicySequence"]);      
    if( jsxCollabStreamFilterSpec == "RestrictToList" ) {
      filterSpecToListBlkJSX.setSelected(jsx3.gui.RadioButton.SELECTED);
      filterSpecSeqBlkJSX.setSelected(jsx3.gui.RadioButton.UNSELECTED);    
    } else {
      filterSpecToListBlkJSX.setSelected(jsx3.gui.RadioButton.UNSELECTED);
      filterSpecSeqBlkJSX.setSelected(jsx3.gui.RadioButton.SELECTED);        
    }
    
    //
    var xAuthRuleListIter  = collabNode.selectNodeIterator(".//ExternalCondition/XAuth/RuleKey");
    while( xAuthRuleListIter.hasNext() ) {
      var xAuthRuleListIterNode = xAuthRuleListIter.next();
      var conditionAction = xAuthRuleListIterNode.getValue();

      if(conditionAction == "MEDIAROOM_RULE_CONFCREATE_USER")
        getJsxByName([BlkJSX,"chkXAuthUseServiceOperator"]).setChecked("1");
      if(conditionAction == "MEDIAROOM_RULE_STREAM_SRC_RESOURCE")
        getJsxByName([BlkJSX,"chkXAuthUseSource"]).setChecked("1");
      if(conditionAction == "MEDIAROOM_RULE_STREAM_DST_RESOURCE")
        getJsxByName([BlkJSX,"chkXAuthUseDest"]).setChecked("1");

    }

    //
    var xAuthServiceOpListIter  = collabNode.selectNodeIterator(".//ExternalCondition/XAuth/ServiceXAuthOperatorFilterSet/ServiceOperatorFilter");
    var mtxXAuthUserListSelBlkJSX = getJsxByName([BlkJSX,"mtxXAuthUserListSel"]);
    mtxXAuthUserListSelBlkJSX.resetCacheData();
    while( xAuthServiceOpListIter.hasNext() ) {
      var xAuthServiceOpListIterNode = xAuthServiceOpListIter.next();
      var UserFilterNID = xAuthServiceOpListIterNode.selectSingleNode(".//UserFilterNID").getValue();
      var ResourceFilterNID = xAuthServiceOpListIterNode.selectSingleNode(".//ResourceFilterNID").getValue();
      if( (UserFilterNID != "") && (ResourceFilterNID != "")) {
        var UserFilterName = getTagObjectFilterName(UserFilterNID);
        var ResourceFilterName = getTagObjectFilterName(ResourceFilterNID);
    
        var o = new Object();
        o.jsxid = jsx3.xml.CDF.getKey();
        o.jsxtextOne = UserFilterName ;
        o.jsxtextOneNID = UserFilterNID ;
        o.jsxtextTwo = ResourceFilterName ;
        o.jsxtextTwoNID = ResourceFilterNID ;
        o.jsxAt = "@";

        mtxXAuthUserListSelBlkJSX.insertRecord(o, null, false);
      }
    }
    mtxXAuthUserListSelBlkJSX.repaintData();

    //
    var streamFilterListIter  = collabNode.selectNodeIterator(".//MediaRoomStreamFilterSet/MediaStreamFilterNID");
    var mtxCollabStreamFilterSelBlkJSX = getJsxByName([BlkJSX,"mtxCollabStreamFilterSel"]);
    mtxCollabStreamFilterSelBlkJSX.resetCacheData();
    while( streamFilterListIter.hasNext() ) {
      var streamFilterListIterNode = streamFilterListIter.next();
      var streamFilterNID = streamFilterListIterNode.getValue();
      if(streamFilterNID != "") {
      var streamFilterName = getTagObjectFilterName(streamFilterNID);
      var o = new Object();
      o.jsxid = jsx3.xml.CDF.getKey();
      o.jsxtextOne = streamFilterName ;
      o.jsxtextOneNID = streamFilterNID ;

      mtxCollabStreamFilterSelBlkJSX.insertRecord(o, null, false);
      }
    }
    mtxCollabStreamFilterSelBlkJSX.repaintData();

    //
    var targetFilterListIter  = collabNode.selectNodeIterator(".//MediaRoomObjectFilterSet/MediaRoomObjectFilterNID");
    var mtxCollabObjectFilterSelBlkJSX = getJsxByName([BlkJSX,"mtxCollabObjectFilterSel"]);
    mtxCollabObjectFilterSelBlkJSX.resetCacheData();
    while( targetFilterListIter.hasNext() ) {
      var targetFilterListIterNode = targetFilterListIter.next();
      var targetFilterNID = targetFilterListIterNode.getValue();
      if(targetFilterNID != "") {
      var targetFilterName = getTagObjectFilterName(targetFilterNID);
      var o = new Object();
      o.jsxid = jsx3.xml.CDF.getKey();
      o.jsxtextOne = targetFilterName ;
      o.jsxtextOneNID = targetFilterNID ;

      mtxCollabObjectFilterSelBlkJSX.insertRecord(o, null, false);
      }
    }
    mtxCollabObjectFilterSelBlkJSX.repaintData();

    //
    var roleListIter  = collabNode.selectNodeIterator(".//MediaRoomRoleSet/MediaRoomObjectRole");
    while( roleListIter.hasNext() ) {
      var roleListIterNode = roleListIter.next();
      var roleObject = roleListIterNode.selectSingleNode(".//RoleObject").getValue();
      var roleAction = roleListIterNode.selectSingleNode(".//RoleAction").getValue();
            
      if( roleAction == "PVR")
        getJsxByName([BlkJSX,"chkCollabPolicyRolePVR"]).setChecked("1");
      if( roleAction == "Record")
        getJsxByName([BlkJSX,"chkCollabPolicyRoleRecord"]).setChecked("1");
      if (roleAction == "View")
        getJsxByName([BlkJSX, "chkCollabPolicyRoleView"]).setChecked("1");
      if (roleAction == "Playback")
        getJsxByName([BlkJSX, "chkCollabPolicyRoleViewPlayback"]).setChecked("1");        
    }*/

    var mediaRoomNID = collabNode.getAttribute("NID");
    var strXMLCacheId = cmpCDFMediaRoomPolicy.BlkJSX.getXMLId();
    var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
    objXML.loadXML(collabNode.toString());
    cmpCDFMediaRoomPolicy.BlkJSX.setSourceXML(objXML);         
    cmpCDFMediaRoomPolicy.BlkJSX.setCDFId(mediaRoomNID);

    var serviceOpListIter  = collabNode.selectNodeIterator(".//ServiceOperatorFilterSet/ServiceOperatorFilter");
    var objDoc = jsx3.xml.CDF.Document.newDocument();
    while( serviceOpListIter.hasNext() ) {
      var serviceOpListIterNode = serviceOpListIter.next();
      var UserFilterNID = serviceOpListIterNode.selectSingleNode(".//UserFilterNID").getValue();
      var ResourceFilterNID = serviceOpListIterNode.selectSingleNode(".//ResourceFilterNID").getValue();
      if( (UserFilterNID != "") && (ResourceFilterNID != "")) {
        var UserFilterName = getTagObjectFilterName(UserFilterNID);
        var ResourceFilterName = getTagObjectFilterName(ResourceFilterNID);
    
        var o = new Object();
        o.jsxid = jsx3.xml.CDF.getKey(); 
        o.UserFilterName = UserFilterName;
        o.UserFilterNID = UserFilterNID;
        o.ResourceFilterName = ResourceFilterName;
        o.ResourceFilterNID = ResourceFilterNID;
        objDoc.insertRecord(o);
      }
    }
    var strXMLCacheId = cmpBlkMediaRoomFilterMatrix.BlkJSX.getXMLId();
    var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
    objXML.loadXML(objDoc.toString());
    cmpBlkMediaRoomFilterMatrix.BlkJSX.setSourceXML(objXML);
    cmpBlkMediaRoomFilterMatrix.BlkJSX.repaintData();

    var collabSrcListIter  = collabNode.selectNodeIterator(".//SrcFilterSet/SrcFilter");
    var objSrcDoc = jsx3.xml.CDF.Document.newDocument();
    while( collabSrcListIter.hasNext() ) {
      var collabSrcListIterNode = collabSrcListIter.next();
      var UserFilterNID = collabSrcListIterNode.selectSingleNode(".//UserFilterNID").getValue();
      var ResourceFilterNID = collabSrcListIterNode.selectSingleNode(".//MediaStreamResourceFilterNID").getValue();
      if( (UserFilterNID != "") && (ResourceFilterNID != "")) {
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
    var strXMLCacheId = cmpBlkMediaRoomSrcFilterMatrix.BlkJSX.getXMLId();
    var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
    objXML.loadXML(objSrcDoc.toString());
    cmpBlkMediaRoomSrcFilterMatrix.BlkJSX.setSourceXML(objXML);
    cmpBlkMediaRoomSrcFilterMatrix.BlkJSX.repaintData();
    
    //  
    var collabDstListIter  = collabNode.selectNodeIterator(".//DstFilterSet/DstFilter");
    var objDstDoc = jsx3.xml.CDF.Document.newDocument();
    while( collabDstListIter.hasNext() ) {
      var collabDstListIterNode = collabDstListIter.next();
      var UserFilterNID = collabDstListIterNode.selectSingleNode(".//UserFilterNID").getValue();
      var ResourceFilterNID = collabDstListIterNode.selectSingleNode(".//MediaStreamResourceFilterNID").getValue();
      if( (UserFilterNID != "") && (ResourceFilterNID != "")) {
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
    var strXMLCacheId = cmpBlkMediaRoomDstFilterMatrix.BlkJSX.getXMLId();
    var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
    objXML.loadXML(objDstDoc.toString());
    cmpBlkMediaRoomDstFilterMatrix.BlkJSX.setSourceXML(objXML);
    cmpBlkMediaRoomDstFilterMatrix.BlkJSX.repaintData();

    var ruleKeyListIter  = collabNode.selectNodeIterator(".//RuleKeyTagFilterSet/RuleKeyTagFilter");
    var objRuleKeyDoc = jsx3.xml.CDF.Document.newDocument();
    while( ruleKeyListIter.hasNext() ) {
      var ruleKeyListIterNode = ruleKeyListIter.next();
      var LHName = ruleKeyListIterNode.selectSingleNode(".//RuleKeyLHTagName/RuleKey").getValue();
      var LHTagName = ruleKeyListIterNode.selectSingleNode(".//RuleKeyLHTagName/TagName").getValue();
      var Operator = ruleKeyListIterNode.selectSingleNode(".//MatchOp").getValue();
      var RHName = ruleKeyListIterNode.selectSingleNode(".//RuleKeyRHTagName/RuleKey").getValue();
      var RHTagName = ruleKeyListIterNode.selectSingleNode(".//RuleKeyRHTagName/TagName").getValue();
      if( LHName != "") {
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
    var strXMLCacheId = cmpBlkMediaRoomRuleKeyMatrix.BlkJSX.getXMLId();
    var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
    objXML.loadXML(objRuleKeyDoc.toString());
    cmpBlkMediaRoomRuleKeyMatrix.BlkJSX.setSourceXML(objXML);
    cmpBlkMediaRoomRuleKeyMatrix.BlkJSX.repaintData();

    createMediaRoomRoleDoc();

    // populate the select with the default values
    updateSelectTagObjectFilters();
    
    // populate the select with the default values
    updateSelectStreamFilter();
    
    // populate the select rule key select with the default value
    updateSelectGroupTag();
    
    if( CollabNID != "DEFAULT_COLLAB_NID" ) {
      getJsxByName([BlkJSX,"txtCollabPolicyName"]).setEnabled(jsx3.gui.Form.STATEDISABLED, true); 
    }
    orgOrderNumber = getJsxByName([BlkJSX,"txtCollabPolicyOrder"]).getValue();
    getJsxByName([BlkJSX,"txtCollabPolicyName"]).focus();
  }
  
  function updateSelectTagObjectFilters() {
    //getFilterToSelectElement([BlkJSX,"selXAuthUserFilter"],"User");
    //getFilterToSelectElement([BlkJSX,"selXAuthResourceFilter"],"Resource");
    getFilterToSelectElement([BlkJSX,"selCollabPolicyUserFilter"],"User");
    getFilterToSelectElement([BlkJSX,"selCollabPolicyResourceFilter"],"Resource");
    //getFilterToSelectElement([BlkJSX,"selCollabPolicyObjectFilter"],"Resource");
    getFilterToSelectElement([BlkJSX,"selCollabSrc_Users"],"User");
    getFilterToSelectElement([BlkJSX,"selCollabSrc_Resources"],"Resource");
    getFilterToSelectElement([BlkJSX,"selCollabDst_Users"],"User");
    getFilterToSelectElement([BlkJSX,"selCollabDst_Resources"],"Resource");
  }

  function updateSelectStreamFilter() {
    getStreamFilterToSelectElement([BlkJSX,"selCollabStreamFilter"]);  
  }
  
  function updateSelectGroupTag() {
    getTagDefToSelectElement([BlkJSX,"selCollabRuleKeyGroupTagLH"],"ALL");
    getTagDefToSelectElement([BlkJSX,"selCollabRuleKeyGroupTagRH"],"ALL");  
  }

  //creating the MediaRoomAdmin roleDoc
  function createMediaRoomRoleDoc() {
    var RoleObjectArr = new Array();
    RoleObjectArr[0] = "MediaRoom";

    var roleCacheDoc = PNameSpace.getCache().getDocument("MediaRoomRoleSetDoc");
    if (roleCacheDoc) {
      delete roleCacheDoc;
    }

    var MediaRoomAdminRoleSet = new jsx3.xml.CDF.Document.newDocument();
    PNameSpace.Cache.setDocument("MediaRoomRoleSetDoc", MediaRoomAdminRoleSet);
    
    //updateReadPolicy(BlkJSX, "chkCollabPolicyRolePVR", "chkCollabPolicyRoleRecord", "chkCollabPolicyRoleView", "chkCollabPolicyRoleViewPlayback");
    var rolePVR = getJsxByName([BlkJSX, "chkCollabPolicyRolePVR"]).getChecked();
    var roleRecord = getJsxByName([BlkJSX, "chkCollabPolicyRoleRecord"]).getChecked();
    var roleView = getJsxByName([BlkJSX, "chkCollabPolicyRoleView"]).getChecked();
    var roleViewPlayback = getJsxByName([BlkJSX, "chkCollabPolicyRoleViewPlayback"]).getChecked();

    if (rolePVR) {
      for (i = 0; i < RoleObjectArr.length; i++) {
        var o = new Object();
        o.jsxid = jsx3.xml.CDF.getKey();
        o.jsxroleObject = RoleObjectArr[i];
        o.jsxroleAction = "PVR";
        MediaRoomAdminRoleSet.insertRecord(o, null, false);
      }
    }

    if (roleRecord) {
      for (i = 0; i < RoleObjectArr.length; i++) {
        var o = new Object();
        o.jsxid = jsx3.xml.CDF.getKey();
        o.jsxroleObject = RoleObjectArr[i];
        o.jsxroleAction = "Record";
        MediaRoomAdminRoleSet.insertRecord(o, null, false);
      }
    }

    if (roleView) {
      for (i = 0; i < RoleObjectArr.length; i++) {
        var o = new Object();
        o.jsxid = jsx3.xml.CDF.getKey();
        o.jsxroleObject = RoleObjectArr[i];
        o.jsxroleAction = "View";
        MediaRoomAdminRoleSet.insertRecord(o, null, false);
      }
    }
    if (roleViewPlayback) {
      for (i = 0; i < RoleObjectArr.length; i++) {
        var o = new Object();
        o.jsxid = jsx3.xml.CDF.getKey();
        o.jsxroleObject = RoleObjectArr[i];
        o.jsxroleAction = "Playback";
        MediaRoomAdminRoleSet.insertRecord(o, null, false);
      }
    }
  }
}

