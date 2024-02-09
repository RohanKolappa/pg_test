function cmpBlkDeviceDiscovery_Create() {

  var cmpBlkEditDicoveryMatrix = new CmpBlock(["blkDeviceDiscovery","mtxDiscoveryTagSelected"]);
  cmpBlkEditDicoveryMatrix.BlkJSX.setSelectionModel(jsx3.gui.Matrix.SELECTION_ROW);
  cmpBlkEditDicoveryMatrix.BlkJSX.setXMLTransformers("xsl/TagToCDFTag.xsl");

  var cmpChkSelfRegister = new Component(["blkDeviceDiscovery", "chkSelfRegister"]);

  var cmpCmbSelDiscoveryTagDefName = new CmpCustomPaginationList(["blkDeviceDiscovery", "cmbDiscoveryAddTagDefName"],
                                                           [{action:"TOPICEVENT", topic: tApp, eventvar:"DiscoveryTagDefNameState", eventval:null}],
                                                             { persist: { col: "jsxtext", initval: false },
                                                                 action: "TOPICEVENT", topic: tApp, eventvar: "DiscoveryTagDefNameSelected", eventval: "jsxid"
                                                             },
                                                          null,
                                                          null,
                                                          "xsl/TagNameList.xsl"
                                                          );

  cmpCmbSelDiscoveryTagDefName.cmpReqParamArr = { StartFrom: "ObjectList_StartFrom",
                                              CountToFetch: "ObjectList_CountToFetch",
                                              SortOrder: "ObjectList_SortOrder",
                                              ObjectCount: "ObjectCount",
                                              SortByXpath: "ObjectList_SortBy",
                                              WhereObjectType: "ObjectList_WhereObjectType"
                                          };

  topic_Set(tApp, "DiscoveryTagNameResponseSelectFormat", "NameValueTree");
  topic_Set(tApp, "DiscoveryTagNameResponseObjectType", "Device");

  cmpCmbSelDiscoveryTagDefName.cmpReq = new cmpReq_GetSessionAPIPaginationList(tApp, "GetTags",
                                                                          [{ VarName: "ObjectList_SelectFormat", VarRef: "DiscoveryTagNameResponseSelectFormat" },
                                                                           { VarName: "ObjectList_WhereObjectType", VarRef: "DiscoveryTagNameResponseObjectType"}],
                                                                          { action: "FNCCALL", context: cmpCmbSelDiscoveryTagDefName,
                                                                            fnc: function (respCode, respVal) {
                                                                              if (respCode == "SUCCESS" && respVal != null) {
                                                                                this.onResponse(respCode, respVal);
                                                                                cmpCmbSelDiscoveryTagDefName.BlkJSX.setValue(null);
                                                                              }
                                                                              else {
                                                                                log.info("GetTagsRequest failed. Error Code :" + respCode);
                                                                                this.onResponseErrorHandle("GetTags", respCode, respVal);
                                                                              }
                                                                            }
                                                                          }
                                                                        );

  var cmpCmbSelDiscoveryTagDefValue = new Component(["blkDeviceDiscovery", "cmbDiscoveryAddTagDefValue"]);
  cmpCmbSelDiscoveryTagDefValue.BlkJSX.setXMLTransformers("xsl/TagValueList.xsl");

  eventApp.subscribe("DiscoveryTagDefNameSelected", null, this,
                      function(eventVar, eventVal) {
                        if (eventVal != "NONE") {
                          cmpCmbSelDiscoveryTagDefValue.BlkJSX.setValue("");
                          var nameRecord = cmpCmbSelDiscoveryTagDefName.BlkJSX.getValue();
                          var selNameRecord = cmpCmbSelDiscoveryTagDefName.BlkJSX.getRecordNode(nameRecord);
                          var tagNode = selNameRecord.selectSingleNode(".//Tag");
                          var strXMLValueCacheId = cmpCmbSelDiscoveryTagDefValue.BlkJSX.getXMLId();
                          var objXML = PNameSpace.getCache().getDocument(strXMLValueCacheId);
                          objXML.loadXML(tagNode.toString());
                          cmpCmbSelDiscoveryTagDefValue.BlkJSX.setSourceXML(objXML); 
                        }
                      }
                    );

  var cmpBtnEditDiscoveryAdd = new CmpButton(["blkDeviceDiscovery","btnDiscoveryValueTagAdd"],
                                              null,
                                              { action:"FNCCALL", 
                                                context:null,
                                                fnc:function() {
                                                
                                                  var nameRecord = this.GroupTagNameBlkJSX.getValue();
                                                  var selNameRecord = this.GroupTagNameBlkJSX.getRecord(nameRecord);
                                                  var groupTagName = "";
                                                  var groupTagValue = "";
                                                  if( selNameRecord == null) {
                                                    groupTagName = nameRecord
                                                  } else {
                                                    groupTagName = selNameRecord.jsxtext;
                                                  }
                                                  if( groupTagName == "" || groupTagName == null ) {
                                                    alert("Please enter/select tag name");
                                                    return false;                                          
                                                  }
                                                  
                                                  var valueRecord = this.GroupTagValueBlkJSX.getValue();
                                                  var selValueRecord = this.GroupTagValueBlkJSX.getRecord(valueRecord);
                                                  if( selValueRecord == null) {
                                                    groupTagValue = valueRecord;
                                                  } else {
                                                    groupTagValue = selValueRecord.jsxtext;
                                                  }
                                                  if( groupTagValue == "" || groupTagValue == null) {
                                                    alert("Please enter/select tag value");
                                                    return false;
                                                  }
                                                                                            
                                                  var o = new Object;
                                                  o.jsxid = groupTagName + groupTagValue;
                                                  o.jsxTagName = groupTagName;
                                                  o.jsxTagValue = groupTagValue;
                                                  this.GroupTagListBlkJSX.insertRecord(o, null, false);
                                                  this.GroupTagNameBlkJSX.setValue(null);
                                                  this.GroupTagValueBlkJSX.setValue(null);
                                                  this.GroupTagListBlkJSX.repaintData();
                                                }
                                              });
                                              
  cmpBtnEditDiscoveryAdd.GroupTagNameBlkJSX = cmpCmbSelDiscoveryTagDefName.BlkJSX;
  cmpBtnEditDiscoveryAdd.GroupTagValueBlkJSX = cmpCmbSelDiscoveryTagDefValue.BlkJSX;
  cmpBtnEditDiscoveryAdd.GroupTagListBlkJSX = cmpBlkEditDicoveryMatrix.BlkJSX;

  eventApp.subscribe("SystemAdminMenuSelection", "DEVICEDISCOVERY", this,
                      function() {
                        topic_Publish(tApp, "DiscoveryTagDefNameState", (newServerDate()).getTime());
                        cmpGetSystemDeviceDiscoveryConfiguartion()
                      }
                    );

  var cmpBtnSetDeviceDiscovery = new CmpButton(["blkDeviceDiscovery", "btnSetDeviceDiscovery"], null,
                                               { action: "FNCCALL", context: this,
                                                  fnc: function() {
                                                    cmpSetSystemDeviceDiscovery();
                                                  }
                                               });   
  
  var cmpBlkDeviceDiscovery = new CmpContainer("blkDeviceDiscovery",
                                              [{cmp:cmpChkSelfRegister },
                                               {cmp:cmpCmbSelDiscoveryTagDefName },
                                               {cmp:cmpCmbSelDiscoveryTagDefValue },
                                               {cmp:cmpBtnEditDiscoveryAdd },
                                               {cmp:cmpBlkEditDicoveryMatrix },
                                               {cmp:cmpBtnSetDeviceDiscovery}
                                              ],
                                              null);

  // function to get System Configuartion for device discovery                 
  function cmpGetSystemDeviceDiscoveryConfiguartion() {
    var reqGetSystemConfig = new ReqSessionGetRequest("GetSystemConfig",
                                                      { action: "FNCCALL", context: null,
                                                        fnc: function (respCode, respVal) {
                                                          if (respCode == "SUCCESS" && respVal != null) {
                                                            var configNode = respVal;
                                                            processSystemDeviceDiscoveryConfig(configNode);
                                                          }
                                                          else {
                                                            log.error("GetSystemConfig request failed");
                                                          }
                                                        }
                                                      }
                                                     );
    reqGetSystemConfig.ServiceName = "SessionService";
    var reqParams = new Object;
    reqParams["ObjectList_WhereConfig"] = "DeviceDiscovery";
    reqGetSystemConfig.reqParams = reqParams;
    // send the request
    reqGetSystemConfig.sendRequest(this);
  } // end cmpGetSystemDeviceDiscoveryConfiguartion

  // function to populate the discovery information of device
  function processSystemDeviceDiscoveryConfig(configNode) {
    var deviceDiscoveryNode = configNode.selectSingleNode(".//Device/DeviceDiscovery");
    if (jsx3.util.strEmpty(deviceDiscoveryNode)) return;
    var selfDiscoveryMode = deviceDiscoveryNode.getAttribute("enable") == "true" ? jsx3.gui.CheckBox.CHECKED : jsx3.gui.CheckBox.UNCHECKED;
    cmpChkSelfRegister.BlkJSX.setChecked(selfDiscoveryMode);
    var deviceGroupNode = deviceDiscoveryNode.selectSingleNode("./Groups");
    if (deviceGroupNode == null) return;
    var GroupIter = deviceGroupNode.selectNodeIterator(".//Tag");
    while (GroupIter.hasNext()) {
      var tagNode = GroupIter.next();
      var TagName = tagNode.getAttribute("Name");
      var TagValue = tagNode.getAttribute("Value");
      if (!jsx3.util.strEmpty(TagName) && !jsx3.util.strEmpty(TagValue)) {
        var o = new Object;
        o.jsxid = TagName + TagValue;
        o.jsxTagName = TagName;
        o.jsxTagValue = TagValue;
        cmpBlkEditDicoveryMatrix.BlkJSX.insertRecord(o, null, true);
      }
    }
  }// end processSystemDeviceDiscoveryConfig


  //function to Set DeviceDiscovery and send Request
  function cmpSetSystemDeviceDiscovery(){
   //get the groups
   var strXMLCacheId = cmpBlkEditDicoveryMatrix.BlkJSX.getXMLId();
   var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
   //convert the matrix data to Tag nodes
   var xslfile = PNameSpace.loadInclude("jsxuser:///JSXAPPS/IPVS_AdminUI/xsl/NewCDFTagToTag.xsl","TagNode","xsl");
   var xslDoc = jsx3.xml.XslDocument.wrap(xslfile);
   var groupTagsXML = xslDoc.transform(objXML);        
   var groupTagsNode = new jsx3.xml.Document();
   groupTagsNode.loadXML(groupTagsXML);
   var reqSetDevice = new ReqSessionGetRequest("SetAutoDiscovery",
                                              { action:"FNCCALL", context:null,
                                                 fnc:function(respCode, respVal) {
                                                       if(respCode == "SUCCESS" && respVal != null) {
                                                         var autoDiscoveryMode = cmpChkSelfRegister.BlkJSX.getChecked() == jsx3.gui.CheckBox.CHECKED ? "Enabled" : "Disabled";
                                                         if(autoDiscoveryMode == "Enabled"){
                                                           alert(autoDiscoveryMode + " device auto-discovery successfully. You need to reboot MS, for devices to be self registered.");
                                                         }
                                                         else{
                                                           alert(autoDiscoveryMode + " device auto-discovery successfully.");
                                                         }
                                                       } else {
                                                         var codeErrorMsg = CommonErrorCodeArr[respCode].errorDesc;
                                                         var ErrorMsg = "Error: Device->SetAutoDiscoveryRequest data request failed \n";
                                                         ErrorMsg += "Error description: " + codeErrorMsg + "\n";
                                                         ErrorMsg += "Error code: " + respCode;
                                                         alert(ErrorMsg);
                                                       }
                                                     }
                                               });
  
    reqSetDevice.ServiceName = "SessionService";
    var requestDataObjectArr = new Object;
    requestDataObjectArr["Groups"] = groupTagsNode;
    reqSetDevice.requestDataObjectArr = requestDataObjectArr;
    
    var reqParams = new Object;
    reqParams["ObjectList_Enable"] = cmpChkSelfRegister.BlkJSX.getChecked() == jsx3.gui.CheckBox.CHECKED ? true : false;
    reqSetDevice.reqParams = reqParams;
    
    // send the request
    reqSetDevice.sendRequest(this);    
  }// end cmpSetSystemDeviceDiscovery
 
  return cmpBlkDeviceDiscovery;
}// end cmpBlkPrefrences_Create