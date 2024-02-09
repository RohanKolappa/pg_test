// this code has some potential issues 
// - w.r.t object reference counts - when objects created in here go out of scope etc
// - loading sync issues - dlg may not be loaded before the object init methods are called

function dlgEditObjectFilter_Create(ObjectFilterNID) {

  // deal with defaults for add
  if( ObjectFilterNID == null ) ObjectFilterNID = "DEFAULT_OBJECTFILTER_NID";
  
  // load view - dialog component file
  var DlgFile = "components/policy/newPolicyFilter.xml";
  var BlkJSX = PNameSpace.getBodyBlock().load(DlgFile);
  
  // setup a local topic for dlg specific messaging
  var tDlg = "Dialog_" + jsx3.xml.CDF.getKey()
  topic_Create(tDlg);
  
  // setup a listner for it
  var eventDlg = new EventListner(tDlg);
  
  // setup a private/dlg specific app event handler
  dlgEditObjectFilter_eventApp = new EventListner( tApp );
  
  function dlgEditObjectFilter_Destroy() {
    // destroy any local eventlistners
    eventDlg.destroy();
    dlgEditObjectFilter_eventApp.destroy();
    
    // destroy all local components
    cmpDlgEditObjectFilter.destroy();
    
    // destroy the local topic
    topic_Destroy(tDlg);
    
    // close dialog
    BlkJSX.doClose();
  }

  
  // event subscribtion: if new grouptag is added from select box
  dlgEditObjectFilter_eventApp.subscribe("GroupTagDataState", null, this,
                                            function() {
                                              updateGroupTagDefSelect();
                                            } );   

  var cmpChkEditObjectFilterMedia = new CmpCheckBox("chkPolicyFilterTypeMedia",
                                                    null,
                                                    {action:"FNCCALL", 
                                                     context:null, 
                                                     fnc:function() {
                                                       updateGroupTagDefSelect();
                                                     }
                                                   });
                                                                                                               
  var cmpChkEditObjectFilterPorts = new CmpCheckBox("chkPolicyFilterTypeResource",
                                                    null,
                                                    {action:"FNCCALL", 
                                                     context:null, 
                                                     fnc:function() {
                                                       updateGroupTagDefSelect();
                                                     }
                                                   });

  var cmpChkEditObjectFilterProfile = new CmpCheckBox("chkPolicyFilterTypeProfile",
                                                    null,
                                                    {action:"FNCCALL", 
                                                     context:null, 
                                                     fnc:function() {
                                                       updateGroupTagDefSelect();
                                                     }
                                                   });

  var cmpChkEditObjectFilterMediaRoom = new CmpCheckBox("chkPolicyFilterTypeMediaRoom",
                                                    null,
                                                    {action:"FNCCALL", 
                                                     context:null, 
                                                     fnc:function() {
                                                       updateGroupTagDefSelect();
                                                     }
                                                   });

  var cmpChkEditObjectFilterUser = new CmpCheckBox("chkPolicyFilterTypeUsers",
                                                    null,
                                                    {action:"FNCCALL", 
                                                     context:null, 
                                                     fnc:function() {
                                                       updateGroupTagDefSelect();
                                                     }
                                                   });

  var cmpChkEditObjectFilterAssets = new CmpCheckBox("chkPolicyFilterTypeAsset",
                                                    null,
                                                    {action:"FNCCALL", 
                                                     context:null, 
                                                     fnc:function() {
                                                       updateGroupTagDefSelect();
                                                     }
                                                   });
        
  var cmpBtnEditObjectFilterClose = new CmpButton(["dialogObjectFilter","btnObjectFilterClose"],
                                               null, 
                                               {action:"FNCCALL", 
                                                context:null, 
                                                fnc:function() {
                                                  dlgEditObjectFilter_Destroy();
                                                }
                                               });
            
  var cmpBtnEditObjectFilterCancel = new CmpButton(["dialogObjectFilter","btnObjectFilterCancel"],
                                               null, 
                                               {action:"FNCCALL", 
                                                context:null, 
                                                fnc:function() {
                                                  dlgEditObjectFilter_Destroy();
                                                }
                                               });
            
  var cmpBtnEditObjectFilterDone = new CmpButton(["dialogObjectFilter","btnObjectFilterDone"],
                                               null, 
                                               {action:"FNCCALL", 
                                                context:null, 
                                                fnc:function() {
                                                  cmpDlgEditObjectFilterSetObjectFilter.call(this, ObjectFilterNID);
                                                }
                                               });
  var cmpBtnEditObjectFilterAddFilter = new CmpButton("btnAddPolicyFilterRule",
                                          null,
                                          { action:"FNCCALL", 
                                            context:null,
                                            fnc:function() {
                                              var RIDSelectOne = this.GroupTagNameBlkJSX.getValue();
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

                                              var valMatchString = this.MatchStringBlkJSX.getValue().search(/match /);
                                              if(valMatchString != -1){
                                                alert("Please enter match string");
                                                return false;
                                              } else {
                                                MatchString = this.MatchStringBlkJSX.getValue();
                                              }

                                              var o = new Object();
                                              o.jsxid = jsx3.xml.CDF.getKey() ;
                                              o.jsxtextOne = FirstFilterName;
                                              o.jsxtextTwo = MatchString;
                                              o.jsxtextOperation = SecondFilterName

                                              this.FilterRuleListBlkJSX.insertRecord(o, null, false);
                                              this.FilterRuleListBlkJSX.repaintData();
                                              //Setting the select boxes and textbox to default state(In order to stop user from using add button more than once)
                                              resetSelectBoxValueToDefault(this.GroupTagNameBlkJSX);
                                              resetSelectBoxValueToDefault(this.MatchOperationBlkJSX);
                                              this.MatchStringBlkJSX.setValue("match string...");
                                            }
                                          });
                                          
  cmpBtnEditObjectFilterAddFilter.GroupTagNameBlkJSX = PNameSpace.getJSXByName("selPolicyFilterResource");
  cmpBtnEditObjectFilterAddFilter.MatchOperationBlkJSX = PNameSpace.getJSXByName("selPolicyFilterOperation");
  cmpBtnEditObjectFilterAddFilter.MatchStringBlkJSX = PNameSpace.getJSXByName("txtPolicyFilterRegExp");
  cmpBtnEditObjectFilterAddFilter.FilterRuleListBlkJSX = PNameSpace.getJSXByName("mtxPolicyFilterRule");                                                                                       
                                                                                         
  var cmpDlgEditObjectFilter = new CmpContainer("DialogObjectFilter",
                                [ {cmp:cmpBtnEditObjectFilterClose},
                                  {cmp:cmpBtnEditObjectFilterCancel},
                                  {cmp:cmpBtnEditObjectFilterDone},
                                  {cmp:cmpBtnEditObjectFilterAddFilter},
                                  {cmp:cmpChkEditObjectFilterAssets},
                                  {cmp:cmpChkEditObjectFilterUser},
                                  //{cmp:cmpChkEditObjectFilterMediaRoom},
                                  {cmp:cmpChkEditObjectFilterProfile},
                                  {cmp:cmpChkEditObjectFilterPorts},
                                  {cmp:cmpChkEditObjectFilterMedia}
                                ],
                                [{action:"TOPICEVENT", topic:tDlg, eventvar:"ObjectFilterEditDataState", eventval:ObjectFilterNID}]);                            
  
  cmpDlgEditObjectFilter.BlkJSX = BlkJSX;      
  cmpDlgEditObjectFilter.ObjectNID = ObjectFilterNID;
  cmpDlgEditObjectFilter.getBlkData =  cmpDlgEditObjectFilterGetObjectFilter;
  
  // init and repaint happens after the dialog is loaded
  jsx3.sleep( (function(){
                 this.init();
                   
                //disable the Filter Name text Box
                if(this.ObjectNID != "DEFAULT_OBJECTFILTER_NID") {
                  var cmpTxtFilterNameBlkJSX = PNameSpace.getJSXByName("txtPolicyFilterName");
                  cmpTxtFilterNameBlkJSX.setEnabled(jsx3.gui.Form.STATEDISABLED, true); 
                }
  
                 this.getBlkData(this.ObjectNID);
                 }
               ),
               null, cmpDlgEditObjectFilter );
  
      
  // fnc to get dlg data
  function cmpDlgEditObjectFilterGetObjectFilter(ObjectFilterNID) { 
    var ObjectFilterNode = topic_GetNode(tCache, "//*[@NID='" + ObjectFilterNID + "']");
    if( ObjectFilterNode == null ) {
      var reqData = new ReqGetObjectNID(ObjectFilterNID, "ObjectFilter",
                                         {action:"FNCCALL", context:this, 
                                           fnc:function(respCode, respVal) {
                                                 if(respCode == "SUCCESS" && respVal != null) {
                                                   cmpDlgEditObjectFilterGetObjectFilter(ObjectFilterNID);
                                                 } else {
                                                   var codeErrorMsg = CommonErrorCodeArr[respCode].errorDesc;
                                                   var ErrorMsg = "Error: PolicyAdmin policy get data request failed \n";
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
      //cmpDlgEditObjectFilter.setMapRuleData("ObjectFilter", "IN", "NID", ObjectFilterNID);
      //topic_Publish(tDlg, "EditObjectFilterDataState", ObjectFilterNID);

      // using the old repaint of the dialog
      DisplayEditValuesForObjectFilter(ObjectFilterNID);
      topic_Publish(tDlg, "ObjectFilterEditDataState", ObjectFilterNID);
    }
  }
       
  // fnc to set dlg data - runs in the context of the OK button
  function cmpDlgEditObjectFilterSetObjectFilter(ObjectFilterNID) {
    // validate input 
    if( cmpDlgEditObjectFilterValidate() != true ) return;
    // decide add or update
    var SetMsg = "Edit POlicyAdmin policy";
    var SetMode = "Replace";
    if( ObjectFilterNID == "DEFAULT_OBJECTFILTER_NID" ) {
      SetMsg = "Add POlicyAdmin policy";
      SetMode = "Add";
    }
    //generate CDF doc for the tag type used in the map rule
    createObjectTypeDoc();

    // setup ObjectFilter map rule info
    var ObjectFilterMapRule = new Object;
    ObjectFilterMapRule.state = false;
    ObjectFilterMapRule.objtype = "ObjectFilter";
    ObjectFilterMapRule.rule = "ObjectFilter";
        
    // get ObjectFilter node info from UI
    //var ObjectFilterNode = utilRuleGetOutboundDoc.call(this, ObjectFilterMapRule.rule);
    
    if( ObjectFilterNode == null ) return;
    // set NID in ObjectFilter node if replace mode
    if( SetMode == "Replace" ) {
      ObjectFilterNode.setAttribute("NID", ObjectFilterNID);
    }

    // do set request for ObjectFilter              
    var reqSetObjectFilterObject = new ReqSetObject(SetMode, ObjectFilterMapRule.objtype, ObjectFilterNode, 
                                                      {action:"FNCCALL", context:this, 
                                                         fnc:function(respCode, respVal) {
                                                               if(respCode == "SUCCESS" && respVal != null) {
                                                                 dlgEditObjectFilter_Destroy();
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
    if( reqSetObjectFilterObject != null) {
      reqSetObjectFilterObject.sendRequest(this);
    }
  } 
      
  function cmpDlgEditObjectFilterValidate() {
    var valFilterPolicyName = PNameSpace.getJSXByName("txtPolicyFilterName").getValue();

    var policyName = checkValue(valFilterPolicyName , "txtPolicyFilterName" , "Please enter filter name");
    if(policyName)
       return true;
          
    return false;
  }

  function DisplayEditValuesForObjectFilter(ObjectFilterNID) {
    var filterNode = topic_GetNode(tCache, ".//*[@NID='" + ObjectFilterNID + "']");

    var filterName = filterNode.selectSingleNode(".//Name").getValue();
    PNameSpace.getJSXByName("txtPolicyFilterName").setValue(filterName);

    var filterPropertyListIter = filterNode.selectNodeIterator(".//ObjectTypeList/ObjectType");
    while( filterPropertyListIter.hasNext() ) {
      var filterPropertyNode = filterPropertyListIter.next();
      var filterPropertyType = filterPropertyNode.getValue();
      if(filterPropertyType != "") {
        switch(filterPropertyType) {
          case "User":
            changeCheckBoxState("chkPolicyFilterTypeUsers","1");
            break;
          case "AssetResource":
            changeCheckBoxState("chkPolicyFilterTypeAsset","1");
            break;
          case "MediaStreamPortResource":
          case "MediaStorePortResource":
            changeCheckBoxState("chkPolicyFilterTypeResource","1");
            break;
          case "MediaStreamProfile":
            changeCheckBoxState("chkPolicyFilterTypeProfile","1");
            break;
          case "MediaRoom":
            changeCheckBoxState("chkPolicyFilterTypeMediaRoom","1");
          case "MediaStreamFileResource":
          case "MediaStreamDirResource":
            changeCheckBoxState("chkPolicyFilterTypeMedia","1");            
            break;
        }
      }
    }

    PNameSpace.getJSXByName("mtxPolicyFilterRule").resetCacheData();
    var filterValueListIter  = filterNode.selectNodeIterator(".//TagFilterSet/TagFilter");
    while( filterValueListIter.hasNext() ) {
      var filterValueNode = filterValueListIter.next();
      try {
        var LHTagName = filterValueNode.selectSingleNode(".//LHTagName").getValue();
        var MatchOp = filterValueNode.selectSingleNode(".//MatchOp").getValue();
        var RHRegexp = filterValueNode.selectSingleNode(".//RHRegexp").getValue();
        if( LHTagName != "") {
          var o = new Object();
          o.jsxid = Math.floor(Math.random()*100001) ;
          o.jsxtextOne = splitTagName(LHTagName);
          o.jsxtextOperation = MatchOp;
          o.jsxtextTwo = RHRegexp;
          
          PNameSpace.getJSXByName("mtxPolicyFilterRule").insertRecord(o, null, false); 
        }
      } catch(e) {
      }
    }
    PNameSpace.getJSXByName("mtxPolicyFilterRule").repaintData();

    //Respurce consist of asset resource and MediaStreamPortResource
    //getTagDefToSelectElement("selPolicyFilterResource","Resource");
    updateGroupTagDefSelect(); 
    
  } // end DisplayEditValuesForObjectFilter
  
  function updateGroupTagDefSelect() {
    var a_SelectName = "selPolicyFilterResource";

    PNameSpace.getJSXByName(a_SelectName).resetCacheData();
    var groupTagNameNIDListNode = topic_GetNode(tApp, "//GroupTagNIDList");
    if( groupTagNameNIDListNode != null ) {
      var groupTagNameNIDListIter = groupTagNameNIDListNode.selectNodeIterator(".//ObjectNID");
      while( groupTagNameNIDListIter.hasNext() ) {
        var groupTagNameNID = groupTagNameNIDListIter.next().getValue();
        var groupTagNameNode = topic_GetNode(tCache, "//*[@NID='" + groupTagNameNID + "']");
        var groupTagName = groupTagNameNode.selectSingleNode(".//TagName").getValue();
        var groupTagPropertyListIter =     groupTagNameNode.selectNodeIterator(".//ObjectTypeList/ObjectType");
        while( groupTagPropertyListIter.hasNext() ) {
          var groupTagPropertyNode = groupTagPropertyListIter.next();
          var groupTagPropertyType = groupTagPropertyNode.getValue();

          if(PNameSpace.getJSXByName("chkPolicyFilterTypeAsset").getChecked()) {
            if(groupTagPropertyType == "AssetResource")
              insertElementInSelectBox(groupTagNameNID, groupTagName, a_SelectName);
          }
          if(PNameSpace.getJSXByName("chkPolicyFilterTypeUsers").getChecked()) {
            if(groupTagPropertyType == "User")
              insertElementInSelectBox(groupTagNameNID, groupTagName, a_SelectName);
          }
          if(PNameSpace.getJSXByName("chkPolicyFilterTypeResource").getChecked()) {
            if(groupTagPropertyType == "MediaStreamPortResource")
              insertElementInSelectBox(groupTagNameNID, groupTagName, a_SelectName);
            if(groupTagPropertyType == "MediaStorePortResource")
              insertElementInSelectBox(groupTagNameNID, groupTagName, a_SelectName);              
          }
          if(PNameSpace.getJSXByName("chkPolicyFilterTypeProfile").getChecked()) {
            if(groupTagPropertyType == "MediaStreamProfile")
              insertElementInSelectBox(groupTagNameNID, groupTagName, a_SelectName);
          }
          if(PNameSpace.getJSXByName("chkPolicyFilterTypeMediaRoom").getChecked()) {
            if(groupTagPropertyType == "MediaRoom")
              insertElementInSelectBox(groupTagNameNID, groupTagName, a_SelectName);
          }
          if(PNameSpace.getJSXByName("chkPolicyFilterTypeMedia").getChecked()) {
            if(groupTagPropertyType == "MediaStreamFileResource")
              insertElementInSelectBox(groupTagNameNID, groupTagName, a_SelectName);
            if(groupTagPropertyType == "MediaStreamDirResource")
              insertElementInSelectBox(groupTagNameNID, groupTagName, a_SelectName);              
          }          

        }   
      }
    }
    insertElementInSelectBox("SelectGroupTag" , "Select GroupTag", a_SelectName );
    insertElementInSelectBox("DEFAULT_GROUPTAG_NID" , "--Add GroupTag", a_SelectName );
    PNameSpace.getJSXByName(a_SelectName).paint();
  } // end updateGroupTagDefSelect

  function createObjectTypeDoc() {
    var roleCacheDoc = PNameSpace.getCache().getDocument("ObjectTypeSetDoc");
    if(roleCacheDoc ) {
      delete roleCacheDoc;
    }
          
    var GroupTagTypeSet = new jsx3.xml.CDF.Document.newDocument();
    PNameSpace.Cache.setDocument("ObjectTypeSetDoc", GroupTagTypeSet);
  
    var typeMedia =  PNameSpace.getJSXByName("chkPolicyFilterTypeMedia").getChecked();
    var typeMediaRoom = PNameSpace.getJSXByName("chkPolicyFilterTypeMediaRoom").getChecked();
    var typeProfile = PNameSpace.getJSXByName("chkPolicyFilterTypeProfile").getChecked();
    var typeSrcPortResource = PNameSpace.getJSXByName("chkPolicyFilterTypeResource").getChecked();
    var typeUser = PNameSpace.getJSXByName("chkPolicyFilterTypeUsers").getChecked();
    var typeAsset = PNameSpace.getJSXByName("chkPolicyFilterTypeAsset").getChecked();

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
         
  }// end createObjectTypeDoc


     


}