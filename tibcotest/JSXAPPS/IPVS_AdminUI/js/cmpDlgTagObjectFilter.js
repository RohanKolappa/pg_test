// this code has some potential issues 
// - w.r.t object reference counts - when objects created in here go out of scope etc
// - loading sync issues - dlg may not be loaded before the object init methods are called

function dlgEditTagObjectFilter_Create(TagObjectFilterNID) {

  // deal with defaults for add
  if( TagObjectFilterNID == null ) TagObjectFilterNID = "DEFAULT_TAGOBJECTFILTER_NID";
  
  //closeDialogIfExists("DialogTagObjectFilter");
  // load view - dialog component file
  var DlgFile = "components/policy/newPolicyFilter.xml";
  var BlkJSX = PNameSpace.getBodyBlock().load(DlgFile);
  
  // setup a local topic for dlg specific messaging
  var tDlg = "Dialog_" + jsx3.xml.CDF.getKey()
  topic_Create(tDlg);
  
  // setup a listner for it
  var eventDlg = new EventListner(tDlg);
  
  // setup a private/dlg specific app event handler
  dlgEditTagObjectFilter_eventApp = new EventListner( tApp );
  
  var dlgText = "Add Filter";
  if( TagObjectFilterNID != "DEFAULT_TAGOBJECTFILTER_NID" ){
    dlgText = "Edit Filter";
  }
  var windowBarBlkJSX = getJsxByName([BlkJSX, "windowBar"]);
  windowBarBlkJSX.setText(dlgText, true);
  closeDialogIfExists(dlgText);
  
  var cmpBlkFilterMatrix = new CmpBlock([BlkJSX,"mtxPolicyFilterRule"]);
  cmpBlkFilterMatrix.BlkJSX.setXMLTransformers("xsl/PolicyFilterToCDF.xsl");
  cmpBlkFilterMatrix.BlkJSX.setSelectionModel(jsx3.gui.Matrix.SELECTION_ROW);
  
  var cmpCDFFilterPolicy = new Component([BlkJSX,"cdfDlgFilterPolicy"]);
  cmpCDFFilterPolicy.BlkJSX.setXMLTransformers("xsl/FilterPolicy.xsl");

  function dlgEditTagObjectFilter_Destroy() {
    // destroy any local eventlistners
    eventDlg.destroy();
    dlgEditTagObjectFilter_eventApp.destroy();
    
    // destroy all local components
    cmpDlgEditTagObjectFilter.destroy();
    
    // destroy the local topic
    topic_Destroy(tDlg);
    
    // close dialog
    BlkJSX.doClose();
  }

  
  // event subscribtion: if new grouptag is added from select box
  dlgEditTagObjectFilter_eventApp.subscribe("GroupTagDataState", null, this,
                                            function() {
                                              updateGroupTagDefSelect();
                                            } );   

  var cmpChkEditTagObjectFilterMedia = new CmpCheckBox([BlkJSX,"chkPolicyFilterTypeMedia"],
                                                    null,
                                                    {action:"FNCCALL", 
                                                     context:null, 
                                                     fnc:function() {
                                                       updateGroupTagDefSelect();
                                                     }
                                                   });
                                                                                                               
  var cmpChkEditTagObjectFilterPorts = new CmpCheckBox([BlkJSX,"chkPolicyFilterTypeResource"],
                                                    null,
                                                    {action:"FNCCALL", 
                                                     context:null, 
                                                     fnc:function() {
                                                       updateGroupTagDefSelect();
                                                     }
                                                   });

  var cmpChkEditTagObjectFilterProfile = new CmpCheckBox([BlkJSX,"chkPolicyFilterTypeProfile"],
                                                    null,
                                                    {action:"FNCCALL", 
                                                     context:null, 
                                                     fnc:function() {
                                                       updateGroupTagDefSelect();
                                                     }
                                                   });

  var cmpChkEditTagObjectFilterMediaRoom = new CmpCheckBox([BlkJSX, "chkPolicyFilterTypeMediaRoom"],
                                                    null,
                                                    {action:"FNCCALL", 
                                                     context:null, 
                                                     fnc:function() {
                                                        updateGroupTagDefSelect();
                                                     }
                                                   });

  var cmpChkEditTagObjectFilterUser = new CmpCheckBox([BlkJSX,"chkPolicyFilterTypeUsers"],
                                                    null,
                                                    {action:"FNCCALL", 
                                                     context:null, 
                                                     fnc:function() {
                                                       updateGroupTagDefSelect();
                                                     }
                                                   });

  var cmpChkEditTagObjectFilterAssets = new CmpCheckBox([BlkJSX,"chkPolicyFilterTypeAsset"],
                                                    null,
                                                    {action:"FNCCALL", 
                                                     context:null, 
                                                     fnc:function() {
                                                       updateGroupTagDefSelect();
                                                     }
                                                   });
        
  var cmpBtnEditTagObjectFilterClose = new CmpButton([BlkJSX,"btnDialogCloseX"],
                                               null, 
                                               {action:"FNCCALL", 
                                                context:null, 
                                                fnc:function() {
                                                  dlgEditTagObjectFilter_Destroy();
                                                }
                                               });
            
  var cmpBtnEditTagObjectFilterCancel = new CmpButton([BlkJSX,"btnTagObjectFilterCancel"],
                                               null, 
                                               {action:"FNCCALL", 
                                                context:null, 
                                                fnc:function() {
                                                  dlgEditTagObjectFilter_Destroy();
                                                }
                                               });
            
  var cmpBtnEditTagObjectFilterDone = new CmpButton([BlkJSX,"btnTagObjectFilterDone"],
                                               null, 
                                               {action:"FNCCALL", 
                                                context:null, 
                                                fnc:function() {
                                                  cmpDlgEditTagObjectFilterSetTagObjectFilter.call(this, TagObjectFilterNID);
                                                }
                                               });
  var cmpSelFilterResource = new CmpSelect([BlkJSX,"selPolicyFilterResource"],
                                               null,
                                               {action:"FNCCALL", 
                                               context:null, 
                                               fnc:function() {
                                                  var selVal = cmpSelFilterResource.BlkJSX.getValue();
                                                  if (jsx3.util.strEmpty(selVal)) return;
                                                  var strXMLCacheId = cmpSelFilterResource.BlkJSX.getXMLId();
                                                  var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
                                                  if (jsx3.util.strEmpty(objXML)) return;
                                                  var record = objXML.selectSingleNode("//record[@jsxid='" + selVal + "']");
                                                  checkAddForNewTagName(record);
                                                  if(record.getAttribute("NID") != "DEFAULT_GROUPTAG_NID") {
                                                    topic_Publish(tApp, "FilterTagDefNameSelected", record.getAttribute("NID"));
                                                  }
                                               }
                                              });

   var cmpSelFilterRegExp = new CmpNIDElementList([BlkJSX,"cmbSelPolicyFilterRegExp"],
                                                  "FilterTagDefNameSelected",
                                                  ".//TagValueList/TagValue",
                                                  {
                                                   jsxtext:{objxpath:"\".\""}
                                                  },
                                                  [{action:"APPEVENT", eventvar:"FilterTagDefNameSelected", eventval:null}],
                                                  {persist:{col:"jsxtext",initval:false},
                                                   action:"APPEVENT", topic:tDlg, eventvar:"FilterTagDefValueSelected", eventval:"jsxtext"
                                                  }
                                                 );

  var cmpBtnEditTagObjectFilterAddFilter = new CmpButton([BlkJSX,"btnAddPolicyFilterRule"],
                                          null,
                                          { action:"FNCCALL", 
                                            context:null,
                                            fnc:function() {
                                              var RIDSelectOne = this.GroupTagNameBlkJSX.getValue();
                                              if(RIDSelectOne!=""){
                                                var mtachResult = RIDSelectOne.search(/Select /);

                                                if(mtachResult != -1){
                                                  alert("Please "+RIDSelectOne);
                                                  return false;
                                                } else {
                                                  var SelectOneNode = this.GroupTagNameBlkJSX.getRecordNode(RIDSelectOne);
                                                  var FirstFilterName= SelectOneNode.getAttribute("jsxtext");
                                                }

                                                var RIDSelectTwo = this.MatchOperationBlkJSX.getValue();
                                                var mtachResult = RIDSelectTwo.search(/Select /);

                                                if(mtachResult != -1){
                                                  alert("Please "+RIDSelectTwo);
                                                  return false;
                                                } else {
                                                  var SelectTwoNode = this.MatchOperationBlkJSX.getRecordNode(RIDSelectTwo);
                                                  var SecondFilterName= SelectTwoNode.getAttribute("jsxtext");
                                                }
                                                
                                                var valueRecord = this.MatchStringBlkJSX.getValue();
                                                var selValueRecord = this.MatchStringBlkJSX.getRecord(valueRecord);
                                                if( selValueRecord == null) {
                                                  groupTagValue = valueRecord;
                                                } else {
                                                  groupTagValue = selValueRecord.jsxtext;
                                                }
                                                if( groupTagValue == "" || groupTagValue == null) {
                                                  alert("Please enter/select tag value");
                                                  return false;
                                                }                                              

                                                var o = new Object();
                                                o.jsxid = FirstFilterName + groupTagValue;
                                                o.jsxtextOne = FirstFilterName;
                                                o.jsxtextTwo = groupTagValue;
                                                o.jsxtextOperation = SecondFilterName

                                                this.FilterRuleListBlkJSX.insertRecord(o, null, false);
                                              }
                                              this.FilterRuleListBlkJSX.repaintData();
                                              //Setting the select boxes and textbox to default state(In order to stop user from using add button more than once)
                                              this.GroupTagNameBlkJSX.setValue("Select Tag Name");
                                              this.MatchOperationBlkJSX.setValue("Select Operation");
                                              this.MatchStringBlkJSX.setValue(null);
                                            }
                                          });
                                          
  cmpBtnEditTagObjectFilterAddFilter.GroupTagNameBlkJSX = cmpSelFilterResource.BlkJSX;
  cmpBtnEditTagObjectFilterAddFilter.MatchOperationBlkJSX = getJsxByName([BlkJSX,"selPolicyFilterOperation"]);
  cmpBtnEditTagObjectFilterAddFilter.MatchStringBlkJSX = cmpSelFilterRegExp.BlkJSX;
  cmpBtnEditTagObjectFilterAddFilter.FilterRuleListBlkJSX = cmpBlkFilterMatrix.BlkJSX;
                                                                                         
  var cmpDlgEditTagObjectFilter = new CmpContainer([BlkJSX, "DialogTagObjectFilter"],
                                                    [{ cmp: cmpBtnEditTagObjectFilterClose },
                                                      { cmp: cmpBtnEditTagObjectFilterCancel },
                                                      { cmp: cmpBtnEditTagObjectFilterDone },
                                                      { cmp: cmpBtnEditTagObjectFilterAddFilter },
                                                      { cmp: cmpChkEditTagObjectFilterAssets },
                                                      { cmp: cmpChkEditTagObjectFilterUser },
                                                      { cmp: cmpChkEditTagObjectFilterProfile },
                                                      { cmp: cmpChkEditTagObjectFilterPorts },
                                                      { cmp: cmpChkEditTagObjectFilterMedia },
                                                      { cmp: cmpSelFilterResource },
                                                      { cmp: cmpSelFilterRegExp },
                                                      { cmp: cmpBlkFilterMatrix },
                                                      { cmp: cmpCDFFilterPolicy }
                                                      //{ cmp: cmpChkEditTagObjectFilterMediaRoom }
                                                    ],
                                                    [{ action: "TOPICEVENT", topic: tDlg, eventvar: "TagObjectFilterEditDataState", eventval: TagObjectFilterNID}]);                            
                      
  cmpDlgEditTagObjectFilter.BlkJSX = BlkJSX;      
  cmpDlgEditTagObjectFilter.ObjectNID = TagObjectFilterNID;
  cmpDlgEditTagObjectFilter.getBlkData =  cmpDlgEditTagObjectFilterGetTagObjectFilter;
  
  // init and repaint happens after the dialog is loaded
  jsx3.sleep( (function(){
                 this.init();
                 
                //disable the Filter Name text Box
                if(this.ObjectNID != "DEFAULT_TAGOBJECTFILTER_NID") {
                  var cmpTxtFilterNameBlkJSX = getJsxByName([BlkJSX,"txtPolicyFilterName"]);
                  cmpTxtFilterNameBlkJSX.setEnabled(jsx3.gui.Form.STATEDISABLED, true); 
                }
                 
                 this.getBlkData(this.ObjectNID);
                 }
               ),
               null, cmpDlgEditTagObjectFilter );



  

      
  // fnc to get dlg data
  function cmpDlgEditTagObjectFilterGetTagObjectFilter(TagObjectFilterNID) { 
    var TagObjectFilterNode = topic_GetNode(tCache, "//*[@NID='" + TagObjectFilterNID + "']");
    if( TagObjectFilterNode == null ) {
      var reqData = new ReqGetObjectNID(TagObjectFilterNID, "TagObjectFilter",
                                          {action:"FNCCALL", context:this, 
                                             fnc:function(respCode, respVal) {
                                                   if(respCode == "SUCCESS" && respVal != null) {
                                                     cmpDlgEditTagObjectFilterGetTagObjectFilter(TagObjectFilterNID);
                                                   } else {
                                                     var codeErrorMsg = CommonErrorCodeArr[respCode].errorDesc;
                                                     var ErrorMsg = "Error: Filter get data request failed \n";
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
      //cmpDlgEditTagObjectFilter.setMapRuleData("TagObjectFilter", "IN", "NID", TagObjectFilterNID);
      //topic_Publish(tDlg, "EditTagObjectFilterDataState", TagObjectFilterNID);

      // using the old repaint of the dialog
      DisplayEditValuesForTagObjectFilter(TagObjectFilterNID);
      
      //Resource consist of assetResource and MediaStreamPortResource
      //getTagDefToSelectElement("selPolicyFilterResource","Resource");
      updateGroupTagDefSelect();
      if( TagObjectFilterNID != "DEFAULT_TAGOBJECTFILTER_NID" ) {
        getJsxByName([BlkJSX,"txtPolicyFilterName"]).setEnabled(jsx3.gui.Form.STATEDISABLED, true);
        getJsxByName([BlkJSX,"chkPolicyFilterTypeUsers"]).setEnabled(jsx3.gui.Form.STATEDISABLED, true);
        getJsxByName([BlkJSX,"chkPolicyFilterTypeAsset"]).setEnabled(jsx3.gui.Form.STATEDISABLED, true);            
        getJsxByName([BlkJSX,"chkPolicyFilterTypeResource"]).setEnabled(jsx3.gui.Form.STATEDISABLED, true);            
        getJsxByName([BlkJSX,"chkPolicyFilterTypeProfile"]).setEnabled(jsx3.gui.Form.STATEDISABLED, true);            
        getJsxByName([BlkJSX,"chkPolicyFilterTypeMediaRoom"]).setEnabled(jsx3.gui.Form.STATEDISABLED, true);            
        getJsxByName([BlkJSX,"chkPolicyFilterTypeMedia"]).setEnabled(jsx3.gui.Form.STATEDISABLED, true);       
      }  
      topic_Publish(tDlg, "TagObjectFilterEditDataState", TagObjectFilterNID);
    }
  }
       
  // fnc to set dlg data - runs in the context of the OK button
  function cmpDlgEditTagObjectFilterSetTagObjectFilter(TagObjectFilterNID) {
    // validate input 
    if( cmpDlgEditTagObjectFilterValidate() != true ) return;
    // decide add or update
    var SetMsg = "Edit Filter";
    var SetMode = "Replace";
    if( TagObjectFilterNID == "DEFAULT_TAGOBJECTFILTER_NID" ) {
      SetMsg = "Add Filter";
      SetMode = "Add";
    }
    //generate CDF doc for the tag type used in the map rule
    createTagObjectTypeDoc();

    // get TagObjectFilter node info from UI
    var TagObjectFilterNode = null;
    
    // write the data back to the CDF
    cmpCDFFilterPolicy.BlkJSX.write();

    var strXMLCacheId = cmpCDFFilterPolicy.BlkJSX.getXMLId();
    var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);      
    var xslfile = PNameSpace.loadInclude("JSXAPPS/IPVS_AdminUI/xsl/FilterPolicyRequest.xsl","FilterPolicyRequest","xsl");
    var xslDoc = jsx3.xml.XslDocument.wrap(xslfile);
    var filterPolicyConfigXML = xslDoc.transform(objXML);
    TagObjectFilterNode = new jsx3.xml.Document();
    TagObjectFilterNode.loadXML(filterPolicyConfigXML);

    var tagObjectTypeNode = PNameSpace.getCache().getDocument("TagObjectTypeSetDoc");
    var xslfile = PNameSpace.loadInclude("jsxuser:///JSXAPPS/IPVS_AdminUI/xsl/TagObjectTypeSet.xsl","TagObjectTypeListNode","xsl");
    var xslDoc = jsx3.xml.XslDocument.wrap(xslfile);
    var newFilterSetXML = xslDoc.transform(tagObjectTypeNode);        
    var newFilterSetNode = new jsx3.xml.Document();
    newFilterSetNode.loadXML(newFilterSetXML);

    var oldFilterSetNode = TagObjectFilterNode.selectSingleNode(".//TagObjectTypeList");              
    var filterSetParentNode = TagObjectFilterNode.selectSingleNode(".//TagObjectTypeList").getParent();
    filterSetParentNode.replaceNode(newFilterSetNode,oldFilterSetNode);

    var strXMLCacheId = cmpBlkFilterMatrix.BlkJSX.getXMLId();
    var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
    var xslfile = PNameSpace.loadInclude("jsxuser:///JSXAPPS/IPVS_AdminUI/xsl/CDFToPolicyFilter.xsl","TagFilterSetNode","xsl");
    var xslDoc = jsx3.xml.XslDocument.wrap(xslfile);
    var newFilterSetXML = xslDoc.transform(objXML);        
    var newFilterSetNode = new jsx3.xml.Document();
    newFilterSetNode.loadXML(newFilterSetXML);
    
    var oldFilterSetNode = TagObjectFilterNode.selectSingleNode(".//TagFilterSet");              
    var filterSetParentNode = TagObjectFilterNode.selectSingleNode(".//TagFilterSet").getParent();
    filterSetParentNode.replaceNode(newFilterSetNode,oldFilterSetNode);

    if( TagObjectFilterNode == null ) return;
    // set NID in TagObjectFilter node if replace mode
    if( SetMode == "Replace" ) {
      TagObjectFilterNode.setAttribute("NID", TagObjectFilterNID);
    }

    // do set request for TagObjectFilter              
    var reqSetTagObjectFilterObject = new ReqSetObject(SetMode, "TagObjectFilter", TagObjectFilterNode, 
                                                        {action:"FNCCALL", context:this, 
                                                           fnc:function(respCode, respVal) {
                                                                 if(respCode == "SUCCESS" && respVal != null) {
                                                                   dlgEditTagObjectFilter_Destroy();
                                                                   topic_Publish(tApp, "TagObjectFilterDataUpdate", "ADD");
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
    if( reqSetTagObjectFilterObject != null) {
      reqSetTagObjectFilterObject.sendRequest(this);
    }
  } 
      
  function cmpDlgEditTagObjectFilterValidate() {
    var valFilterPolicyName = getJsxByName([BlkJSX,"txtPolicyFilterName"]).getValue();

    var policyName = checkValue(valFilterPolicyName , [BlkJSX,"txtPolicyFilterName"] , "Please enter filter name");
    if(policyName){
      if( TagObjectFilterNID == "DEFAULT_TAGOBJECTFILTER_NID" ) {
        duplicateCheck = chkForNameExistence(valFilterPolicyName,[BlkJSX,"txtPolicyFilterName"],"mtxFilterName", "Duplicate Filter name. Please enter some other Filter name");
        if(duplicateCheck) {
          return true;
        }
      } else {
        return true;
      } 
    }
          
    return false;
  }

  function DisplayEditValuesForTagObjectFilter(TagObjectFilterNID) {
    var filterNode = topic_GetNode(tCache, ".//*[@NID='" + TagObjectFilterNID + "']");

    /*var version = filterNode.getAttribute("Ver");
    var versionBlkJSX = getJsxByName([BlkJSX,"txtTagObjectXMLVersion"]);
    versionBlkJSX.setValue(version);
    
    var revision = filterNode.getAttribute("Rev");
    var revisionBlkJSX = getJsxByName([BlkJSX,"txtTagObjectXMLRevision"]);
    revisionBlkJSX.setValue(revision);

    var serviceVer = filterNode.getAttribute("serviceVer");
    var serviceVerBlkJSX = getJsxByName([BlkJSX,"txtTagObjectXMLServiceVersion"]);
    serviceVerBlkJSX.setValue(serviceVer);    

    var filterName = filterNode.selectSingleNode(".//Name").getValue();
    var filterNameBlkJSX = getJsxByName([BlkJSX,"txtPolicyFilterName"]);
    filterNameBlkJSX.setValue(filterName);*/

    var filterNID = filterNode.getAttribute("NID");
    var strXMLCacheId = cmpCDFFilterPolicy.BlkJSX.getXMLId();
    var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
    objXML.loadXML(filterNode.toString());
    cmpCDFFilterPolicy.BlkJSX.setSourceXML(objXML);         
    cmpCDFFilterPolicy.BlkJSX.setCDFId(filterNID);

    var objDoc = jsx3.xml.CDF.Document.newDocument();
    var filterValueListIter  = filterNode.selectNodeIterator(".//TagFilterSet/TagFilter");
    while( filterValueListIter.hasNext() ) {
      var filterValueNode = filterValueListIter.next();
      try {
        var LHTagName = filterValueNode.selectSingleNode(".//LHTagName").getValue();
        var MatchOp = filterValueNode.selectSingleNode(".//MatchOp").getValue();
        var RHRegexp = filterValueNode.selectSingleNode(".//RHRegexp").getValue();
        if( LHTagName != "") {
          var o = new Object();
          o.jsxid = splitTagName(LHTagName) + RHRegexp;
          o.LHTagName = splitTagName(LHTagName);
          o.MatchOp = MatchOp;
          o.RHRegexp = RHRegexp;
          
          objDoc.insertRecord(o, null, false); 
        }
      } catch(e) {
      }
    }
    var strXMLCacheId = cmpBlkFilterMatrix.BlkJSX.getXMLId();
    var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
    objXML.loadXML(objDoc.toString());
    cmpBlkFilterMatrix.BlkJSX.setSourceXML(objXML);
    cmpBlkFilterMatrix.BlkJSX.repaintData(); 
    
    /*var filterPropertyListIter = filterNode.selectNodeIterator(".//TagObjectTypeList/TagObjectType");
    while( filterPropertyListIter.hasNext() ) {
      var filterPropertyNode = filterPropertyListIter.next();
      var filterPropertyType = filterPropertyNode.getValue();
      if(filterPropertyType != "") {
        switch(filterPropertyType) {
          case "User":
            changeCheckBoxState([BlkJSX,"chkPolicyFilterTypeUsers"],"1");           
            break;
          case "AssetResource":
            changeCheckBoxState([BlkJSX,"chkPolicyFilterTypeAsset"],"1");
            break;
          case "MediaStreamPortResource":
          case "MediaStorePortResource":
            changeCheckBoxState([BlkJSX,"chkPolicyFilterTypeResource"],"1");
            break;
          case "MediaStreamProfile":
            changeCheckBoxState([BlkJSX,"chkPolicyFilterTypeProfile"],"1");
            break;
          case "MediaRoom":
            changeCheckBoxState([BlkJSX, "chkPolicyFilterTypeMediaRoom"], "1");
            break;
          case "MediaStreamFileResource":
          case "MediaStreamDirResource":
            changeCheckBoxState([BlkJSX,"chkPolicyFilterTypeMedia"],"1");
            break;
        }
      }
    }*/    
 
  } // end DisplayEditValuesForTagObjectFilter
  
  function updateGroupTagDefSelect() {
    var selPolicyFilterResourceBlkJSX = getJsxByName([BlkJSX,"selPolicyFilterResource"]);
 
    var pAsset = "0";
    var pUser = "0";
    var pStreamPortResource = "0"; 
    var pStorePortResource = "0";
    var pStreamProfile = "0";
    var pMediaroom = "0";
    var pStreamFileResource = "0"; 
    var pStreamDirResource = "0";
    
    var tAsset = "0";
    var tUser = "0";
    var tStreamPortResource = "0"; 
    var tStorePortResource = "0";
    var tStreamProfile = "0";
    var tMediaroom = "0";
    var tStreamFileResource = "0"; 
    var tStreamDirResource = "0";    
    
    if(getJsxByName([BlkJSX,"chkPolicyFilterTypeAsset"]).getChecked()) 
      pAsset = "1";
    if(getJsxByName([BlkJSX,"chkPolicyFilterTypeUsers"]).getChecked()) 
      pUser = "1";
    if(getJsxByName([BlkJSX,"chkPolicyFilterTypeResource"]).getChecked()) {
      pStreamPortResource = "1";
      pStorePortResource = "1";              
    }
    if(getJsxByName([BlkJSX,"chkPolicyFilterTypeProfile"]).getChecked()) 
      pStreamProfile = "1";
    if(getJsxByName([BlkJSX,"chkPolicyFilterTypeMediaRoom"]).getChecked()) 
      pMediaroom = "1";
    if(getJsxByName([BlkJSX,"chkPolicyFilterTypeMedia"]).getChecked()) {
      pStreamFileResource = "1";
      pStreamDirResource = "1";              
    }    

    selPolicyFilterResourceBlkJSX.resetCacheData();
    var groupTagNameNIDListNode = topic_GetNode(tApp, "//GroupTagNIDList");
    if( groupTagNameNIDListNode != null ) {
      var groupTagNameNIDListIter = groupTagNameNIDListNode.selectNodeIterator(".//ObjectNID");
      while( groupTagNameNIDListIter.hasNext() ) {
        var groupTagNameNID = groupTagNameNIDListIter.next().getValue();
        var groupTagNameNode = topic_GetNode(tCache, "//*[@NID='" + groupTagNameNID + "']");
        var groupTagName = groupTagNameNode.selectSingleNode(".//TagName").getValue();
        var groupTagPropertyListIter =     groupTagNameNode.selectNodeIterator(".//TagObjectTypeList/TagObjectType");
        while( groupTagPropertyListIter.hasNext() ) {
          var groupTagPropertyNode = groupTagPropertyListIter.next();
          var groupTagPropertyType = groupTagPropertyNode.getValue();

          if(groupTagPropertyType == "AssetResource"){
            if(pAsset == "1") {
              tAsset = "1";
            }
          }
          if(groupTagPropertyType == "User") {
            if(pUser == "1") {
              tUser = "1";
            }
          }
          if( (groupTagPropertyType == "MediaStreamPortResource") || 
              (groupTagPropertyType == "MediaStorePortResource") ) {
              if(pStreamPortResource == "1") {
                tStreamPortResource = "1";
                tStorePortResource = "1";
              }
          }             
          if(groupTagPropertyType == "MediaStreamProfile") {
            if(pStreamProfile == "1") {
              tStreamProfile = "1";
            }
          }
          if(groupTagPropertyType == "MediaRoom") {
            if(pMediaroom == "1") {
              tMediaroom = "1";
            }
          }
          if( (groupTagPropertyType == "MediaStreamFileResource") || 
              (groupTagPropertyType == "MediaStreamDirResource") ) {
              if(pStreamFileResource == "1") {
                tStreamFileResource = "1";
                tStreamDirResource = "1"; 
              }
          }                     
        }
        if(pAsset == "0" &&  
           pUser == "0" &&
           pStreamPortResource == "0" &&
           pStorePortResource == "0" &&
           pStreamProfile == "0" &&
           pMediaroom == "0" &&
           pStreamFileResource == "0" &&
           pStreamDirResource == "0" ) {
        } else {
          if( pAsset == tAsset &&
              pUser == tUser &&
              pStreamPortResource == tStreamPortResource &&
              pStorePortResource == tStorePortResource &&
              pStreamProfile == tStreamProfile &&
              pMediaroom == tMediaroom &&
              pStreamFileResource == tStreamFileResource &&
              pStreamDirResource == tStreamDirResource ) { 
            insertElementInSelectBox(groupTagNameNID, groupTagName, [BlkJSX,"selPolicyFilterResource"]);
          }
        }
        tAsset = "0";
        tUser = "0";
        tStreamPortResource = "0"; 
        tStorePortResource = "0";
        tStreamProfile = "0";
        tMediaroom = "0";
        tStreamFileResource = "0"; 
        tStreamDirResource = "0";    
      }
    }
    insertElementInSelectBox("SelectGroupTag" , "Select Tag Name", [BlkJSX,"selPolicyFilterResource"] );
    insertElementInSelectBox("DEFAULT_GROUPTAG_NID" , "--Add Tag Name", [BlkJSX,"selPolicyFilterResource"] );
    selPolicyFilterResourceBlkJSX.setValue("Select Tag Name");
    selPolicyFilterResourceBlkJSX.paint();
    //selPolicyFilterResourceBlkJSX.setEvent("cmpDlgEditTagObjectFilter_TagNameSelect();", jsx3.gui.Interactive.SELECT);
    //getJsxByName([BlkJSX,"txtPolicyFilterName"]).focus();
  } // end updateGroupTagDefSelect
  


  function createTagObjectTypeDoc() {
    var roleCacheDoc = PNameSpace.getCache().getDocument("TagObjectTypeSetDoc");
    if(roleCacheDoc ) {
      delete roleCacheDoc;
    }
          
    var GroupTagTypeSet = new jsx3.xml.CDF.Document.newDocument();
    PNameSpace.Cache.setDocument("TagObjectTypeSetDoc", GroupTagTypeSet);
  
    var typeMedia =  getJsxByName([BlkJSX,"chkPolicyFilterTypeMedia"]).getChecked();
    var typeMediaRoom = getJsxByName([BlkJSX,"chkPolicyFilterTypeMediaRoom"]).getChecked();
    var typeProfile = getJsxByName([BlkJSX,"chkPolicyFilterTypeProfile"]).getChecked();
    var typeSrcPortResource = getJsxByName([BlkJSX,"chkPolicyFilterTypeResource"]).getChecked();
    var typeUser = getJsxByName([BlkJSX,"chkPolicyFilterTypeUsers"]).getChecked();
    var typeAsset = getJsxByName([BlkJSX,"chkPolicyFilterTypeAsset"]).getChecked();

    if(typeMedia) {
        var o = new Object();
        o.jsxid = jsx3.xml.CDF.getKey();
        o.jsxroleObject = "MediaStreamFileResource";
        GroupTagTypeSet.insertRecord(o, null, false);
        o.jsxid = jsx3.xml.CDF.getKey();
        o.jsxroleObject = "MediaStreamDirResource";
        GroupTagTypeSet.insertRecord(o, null, false);        
    }
    if(typeSrcPortResource) {
        var o = new Object();
        o.jsxid = jsx3.xml.CDF.getKey();
        o.jsxroleObject = "MediaStreamPortResource";
        GroupTagTypeSet.insertRecord(o, null, false);
        o.jsxid = jsx3.xml.CDF.getKey();
        o.jsxroleObject = "MediaStorePortResource";
        GroupTagTypeSet.insertRecord(o, null, false);        
    }    
    if(typeProfile) {
        var o = new Object();
        o.jsxid = jsx3.xml.CDF.getKey();
        o.jsxroleObject = "MediaStreamProfile";
        GroupTagTypeSet.insertRecord(o, null, false);      
    }    
    if(typeMediaRoom) {
        var o = new Object();
        o.jsxid = jsx3.xml.CDF.getKey();
        o.jsxroleObject = "MediaRoom";
        GroupTagTypeSet.insertRecord(o, null, false);      
    }
    if(typeUser) {
        var o = new Object();
        o.jsxid = jsx3.xml.CDF.getKey();
        o.jsxroleObject = "User";
        GroupTagTypeSet.insertRecord(o, null, false);      
    }
    if(typeAsset) {
        var o = new Object();
        o.jsxid = jsx3.xml.CDF.getKey();
        o.jsxroleObject = "AssetResource";
        GroupTagTypeSet.insertRecord(o, null, false);      
    }                   
         
  }// end createTagObjectTypeDoc 


     


}