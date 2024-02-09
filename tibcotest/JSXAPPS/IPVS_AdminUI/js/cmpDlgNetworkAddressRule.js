// this code has some potential issues 
// - w.r.t object reference counts - when objects created in here go out of scope etc
// - loading sync issues - dlg may not be loaded before the object init methods are called

function dlgEditNetworkAddress_Create(NetworkAddressNID) {

  // deal with defaults for add
  if( NetworkAddressNID == null ) NetworkAddressNID = "DEFAULT_NAT_NID";
  
  //closeDialogIfExists("DialogNetworkAddress");
  // load view - dialog component file
  var DlgFile = "components/policy/newNATPolicy.xml";
  var BlkJSX = PNameSpace.getBodyBlock().load(DlgFile);

  // setup a local topic for dlg specific messaging     
  var tDlg = "Dialog_" + jsx3.xml.CDF.getKey();
  topic_Create(tDlg);

  // setup a listner for it      
  var eventDlg = new EventListner(tDlg);
  var orgOrderNumber = "";  

  // setup a private/dlg specific app event handler
  dlgEditNetworkAddress_eventAppTagObject = new EventListner( tApp );
  
  var dlgText = "Add Network Address Translation Policy";
  if( NetworkAddressNID != "DEFAULT_NAT_NID" ){
    dlgText = "Edit Network Address Translation Policy";
  }
  var windowBarBlkJSX = getJsxByName([BlkJSX, "windowBar"]);
  windowBarBlkJSX.setText(dlgText, true);
  closeDialogIfExists(dlgText);
    
  var cmpBlkNATLocalFilterMatrix = new CmpBlock([BlkJSX,"mtxNATPolicy_Local"]);
  cmpBlkNATLocalFilterMatrix.BlkJSX.setXMLTransformers("xsl/PolicyLocalFilterToCDF.xsl");
  cmpBlkNATLocalFilterMatrix.BlkJSX.setSelectionModel(jsx3.gui.Matrix.SELECTION_ROW);

  var cmpBlkNATRemoteFilterMatrix = new CmpBlock([BlkJSX,"mtxNATPolicy_Remote"]);
  cmpBlkNATRemoteFilterMatrix.BlkJSX.setXMLTransformers("xsl/PolicyRemoteFilterToCDF.xsl");
  cmpBlkNATRemoteFilterMatrix.BlkJSX.setSelectionModel(jsx3.gui.Matrix.SELECTION_ROW);

  var cmpBlkNATRuleKeyMatrix = new CmpBlock([BlkJSX,"mtxNATPolicy_RuleKeySel"]);
  cmpBlkNATRuleKeyMatrix.BlkJSX.setXMLTransformers("xsl/PolicyRuleKeyFilterToCDF.xsl");
  cmpBlkNATRuleKeyMatrix.BlkJSX.setSelectionModel(jsx3.gui.Matrix.SELECTION_ROW);

  var cmpBlkNATTableMatrix = new CmpBlock([BlkJSX,"mtxNATTable"]);
  cmpBlkNATTableMatrix.BlkJSX.setXMLTransformers("xsl/PolicyNATTableToCDF.xsl");
  cmpBlkNATTableMatrix.BlkJSX.setSelectionModel(jsx3.gui.Matrix.SELECTION_ROW);

  var cmpCDFNATPolicy = new Component([BlkJSX,"cdfDlgNATPolicy"]);
  cmpCDFNATPolicy.BlkJSX.setXMLTransformers("xsl/NATPolicy.xsl");

  function dlgEditNetworkAddress_Destroy() {
    // destroy any local eventlistners
    eventDlg.destroy();
    dlgEditNetworkAddress_eventAppTagObject.destroy();
    
    // destroy all local components
    cmpDlgEditNetworkAddress.destroy();
    
    // destroy the local topic
    topic_Destroy(tDlg);
    
    // close dialog
    BlkJSX.doClose();
  }
  
  // event subscribtion: if new MediaStreamFilter is added from select box and update all the
  // select boxes in the dialog after add is finished
  dlgEditNetworkAddress_eventAppTagObject.subscribe("TagObjectFilterDataState", null, this,
                                                        function() {
                                                          updateSelectTagObjectFilters();
                                                        } );
  
  var cmpBtnEditNetworkAddressAddNATEntry = new CmpButton([BlkJSX,"btnNATPolicy_AddNATEntry"],
                                                              null,
                                                              { action:"FNCCALL", 
                                                                context:null,
                                                                fnc:function() {
                                                                  var network = this.NATEntryNetworkBlkJSX.getValue();
                                                                  var translatedNetwork = this.NATEntryTranslatedBlkJSX.getValue();
                                                                  var netmask = this.NATEntryNetmaskBlkJSX.getValue();
                                                                  
                                                                  if(network == "" ) {
                                                                    alert("Please enter network value");
                                                                    this.NATEntryNetworkBlkJSX.focus();
                                                                    return false;
                                                                  }
                                                                  if(translatedNetwork == "" ) {
                                                                    alert("Please enter translated network value");
                                                                    this.NATEntryTranslatedBlkJSX.focus();
                                                                    return false;
                                                                  }                                                                  
                                                                  if(netmask == "" ) {
                                                                    alert("Please enter netmask value");
                                                                    this.NATEntryNetmaskBlkJSX.focus();
                                                                    return false;
                                                                  }
                                                                  
                                                                  var o = new Object;
                                                                  o.jsxid = jsx3.xml.CDF.getKey();
                                                                  o.jsxNetwork = network;
                                                                  o.jsxTranslatedNetwork = translatedNetwork;
                                                                  o.jsxNetmask = netmask;
                                                                  this.mtxNATEntryBlkJSX.insertRecord(o, null, true);
                                                                  this.NATEntryNetworkBlkJSX.setValue("");
                                                                  this.NATEntryTranslatedBlkJSX.setValue("");
                                                                  this.NATEntryNetmaskBlkJSX.setValue("");                                                                  
                                                                }
                                                              });
  cmpBtnEditNetworkAddressAddNATEntry.NATEntryNetworkBlkJSX = getJsxByName([BlkJSX,"textNATPolicy_NATEntryNetwork"]);
  cmpBtnEditNetworkAddressAddNATEntry.NATEntryTranslatedBlkJSX = getJsxByName([BlkJSX,"textNATPolicy_NATEntryTranslated"]);
  cmpBtnEditNetworkAddressAddNATEntry.NATEntryNetmaskBlkJSX = getJsxByName([BlkJSX,"textNATPolicy_NATEntryNetmask"]);
  cmpBtnEditNetworkAddressAddNATEntry.mtxNATEntryBlkJSX = cmpBlkNATTableMatrix.BlkJSX;
                                                                
  var cmpSelRuleKeyLHTag = new CmpSelect([BlkJSX,"selNATPolicy_RuleKeyLHTag"],
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
  var cmpSelRuleKeyRHTag = new CmpSelect([BlkJSX,"selNATPolicy_RuleKeyRHTag"],
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

  var cmpBtnEditNetworkAddressAddRuleKey = new CmpButton([BlkJSX,"btnAddNATPolicy_RuleKey"],
                                                            null,
                                                            { action:"FNCCALL", 
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
                                                                    this.NATRuleKeyMatrix.insertRecord(o, null, false);
                                                                  }
                                                                }
                                                                this.NATRuleKeyMatrix.repaintData();
                                                                this.LHFilterRuleKeyBlkJSX.setValue("Select LH Rule");
                                                                this.LHTagRuleKeyBlkJSX.setValue("Select Tag Name");
                                                                this.OperatorRuleKeyBlkJSX.setValue("Select Operation");
                                                                this.RHFilterRuleKeyBlkJSX.setValue("Select RH Rule");
                                                                this.RHTagRuleKeyBlkJSX.setValue("Select Tag Name");
                                                              }
                                                            });
  cmpBtnEditNetworkAddressAddRuleKey.LHFilterRuleKeyBlkJSX = getJsxByName([BlkJSX,"selNATPolicy_RuleKeyLHFilter"]);
  cmpBtnEditNetworkAddressAddRuleKey.LHTagRuleKeyBlkJSX = cmpSelRuleKeyLHTag.BlkJSX;
  cmpBtnEditNetworkAddressAddRuleKey.OperatorRuleKeyBlkJSX = getJsxByName([BlkJSX,"selNATPolicy_RuleKeyOp"]);
  cmpBtnEditNetworkAddressAddRuleKey.RHFilterRuleKeyBlkJSX = getJsxByName([BlkJSX,"selNATPolicy_RuleKeyRHFilter"]);
  cmpBtnEditNetworkAddressAddRuleKey.RHTagRuleKeyBlkJSX = cmpSelRuleKeyRHTag.BlkJSX;
  cmpBtnEditNetworkAddressAddRuleKey.NATRuleKeyMatrix = cmpBlkNATRuleKeyMatrix.BlkJSX;

  var cmpSelNATRemoteFilter = new CmpSelect([BlkJSX,"selNATPolicy_Remote"],
                                               null,
                                               {action:"FNCCALL", 
                                               context:null, 
                                               fnc:function() {
                                                  var selVal = cmpSelNATRemoteFilter.BlkJSX.getValue();
                                                  if (jsx3.util.strEmpty(selVal)) return;
                                                  var strXMLCacheId = cmpSelNATRemoteFilter.BlkJSX.getXMLId();
                                                  var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
                                                  if (jsx3.util.strEmpty(objXML)) return;
                                                  var record = objXML.selectSingleNode("//record[@jsxid='" + selVal + "']");
                                                  checkAddForNewFilter(record, "Resource");
                                               }
                                              });
                                                           
  var cmpBtnEditNetworkAddressAddRemote = new CmpButton([BlkJSX,"btnNATPolicy_AddRemote"],
                                                            null,
                                                            { action:"FNCCALL", 
                                                              context:null,
                                                              fnc:function() {
                                                                var remoteRecord = this.natRemoteFilterBlkJSX.getValue();
                                                                if(remoteRecord!=""){
                                                                  var mtachResult = remoteRecord.search(/Select /);

                                                                  if(mtachResult != -1){
                                                                    alert("Please "+remoteRecord);
                                                                    return false;
                                                                  } else {
                                                                    var remoteNode = this.natRemoteFilterBlkJSX.getRecord(remoteRecord);
                                                                    var remoteName= remoteNode.jsxtext;
                                                                    var remoteNID = remoteNode.NID;
                                                                  }
                                                                  var o = new Object();
                                                                  o.jsxid = jsx3.xml.CDF.getKey();
                                                                  o.jsxtextOneNID = remoteNID;
                                                                  o.jsxtextOne = remoteName;
                                                                  this.natRemoteFilterMatrix.insertRecord(o, null, false);
                                                                }
                                                                this.natRemoteFilterMatrix.repaintData();
                                                                this.natRemoteFilterBlkJSX.setValue("Select Resource Filter");
                                                              }
                                                            });
  cmpBtnEditNetworkAddressAddRemote.natRemoteFilterBlkJSX = cmpSelNATRemoteFilter.BlkJSX;
  cmpBtnEditNetworkAddressAddRemote.natRemoteFilterMatrix = cmpBlkNATRemoteFilterMatrix.BlkJSX;
                                                            
  var cmpSelNATLocalFilter = new CmpSelect([BlkJSX,"selNATPolicy_Local"],
                                               null,
                                               {action:"FNCCALL", 
                                               context:null, 
                                               fnc:function() {
                                                  var selVal = cmpSelNATLocalFilter.BlkJSX.getValue();
                                                  if (jsx3.util.strEmpty(selVal)) return;
                                                  var strXMLCacheId = cmpSelNATLocalFilter.BlkJSX.getXMLId();
                                                  var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
                                                  if (jsx3.util.strEmpty(objXML)) return;
                                                  var record = objXML.selectSingleNode("//record[@jsxid='" + selVal + "']");
                                                  checkAddForNewFilter(record, "Resource");
                                               }
                                              });

  var cmpBtnEditNetworkAddressAddLocal = new CmpButton([BlkJSX,"btnNATPolicy_AddLocal"],
                                                            null,
                                                            { action:"FNCCALL", 
                                                              context:null,
                                                              fnc:function() {
                                                                var localRecord = this.natLocalFilterBlkJSX.getValue();
                                                                if(localRecord!=""){
                                                                  var mtachResult = localRecord.search(/Select /);

                                                                  if(mtachResult != -1){
                                                                    alert("Please "+localRecord);
                                                                    return false;
                                                                  } else {
                                                                    var localNode = this.natLocalFilterBlkJSX.getRecord(localRecord);
                                                                    var localName= localNode.jsxtext;
                                                                    var localNID = localNode.NID;
                                                                  }
                                                                  var o = new Object();
                                                                  o.jsxid = jsx3.xml.CDF.getKey();
                                                                  o.jsxtextOneNID = localNID;
                                                                  o.jsxtextOne = localName;
                                                                  this.natLocalFilterMatrix.insertRecord(o, null, false);
                                                                }
                                                                this.natLocalFilterMatrix.repaintData();
                                                                this.natLocalFilterBlkJSX.setValue("Select Resource Filter");
                                                              }
                                                            });
  cmpBtnEditNetworkAddressAddLocal.natLocalFilterBlkJSX = cmpSelNATLocalFilter.BlkJSX;
  cmpBtnEditNetworkAddressAddLocal.natLocalFilterMatrix = cmpBlkNATLocalFilterMatrix.BlkJSX;
                                                                                                                                                                                      
  var cmpBtnRadioDNSName = new CmpRadioButton([BlkJSX, "radioNATPolicy_DNS"], null,
                                               { action: "FNCCALL",
                                                 context: null,
                                                 fnc: function(){
                                                    cmpBtnRadioDNSName.BlkJSX.setSelected(jsx3.gui.RadioButton.SELECTED);
                                                    cmpBtnRadioIP.BlkJSX.setSelected(jsx3.gui.RadioButton.UNSELECTED);
                                                    cmpBtnRadioNATZone.BlkJSX.setSelected(jsx3.gui.RadioButton.UNSELECTED);
                                                 }
                                               });
  var cmpBtnRadioIP = new CmpRadioButton([BlkJSX, "radioNATPolicy_IP"], null,
                                               { action: "FNCCALL",
                                                 context: null,
                                                 fnc: function(){
                                                    cmpBtnRadioDNSName.BlkJSX.setSelected(jsx3.gui.RadioButton.UNSELECTED);
                                                    cmpBtnRadioIP.BlkJSX.setSelected(jsx3.gui.RadioButton.SELECTED);
                                                    cmpBtnRadioNATZone.BlkJSX.setSelected(jsx3.gui.RadioButton.UNSELECTED);
                                                 }
                                               });
  var cmpBtnRadioNATZone = new CmpRadioButton([BlkJSX, "radioNATPolicy_NATZone"], null,
                                               { action: "FNCCALL",
                                                 context: null,
                                                 fnc: function(){
                                                    cmpBtnRadioDNSName.BlkJSX.setSelected(jsx3.gui.RadioButton.UNSELECTED);
                                                    cmpBtnRadioIP.BlkJSX.setSelected(jsx3.gui.RadioButton.UNSELECTED);
                                                    cmpBtnRadioNATZone.BlkJSX.setSelected(jsx3.gui.RadioButton.SELECTED);
                                                 }
                                               });

  var cmpBtnEditNetworkAddressClose = new CmpButton([BlkJSX,"btnDialogCloseX"],
                                                       null, 
                                                       {action:"FNCCALL", 
                                                        context:null, 
                                                        fnc:function() {
                                                          dlgEditNetworkAddress_Destroy();
                                                        }
                                                       });
            
  var cmpBtnEditNetworkAddressCancel = new CmpButton([BlkJSX,"btnNATPolicyCancel"],
                                                       null, 
                                                       {action:"FNCCALL", 
                                                        context:null, 
                                                        fnc:function() {
                                                          dlgEditNetworkAddress_Destroy();
                                                        }
                                                       });
            
  var cmpBtnEditNetworkAddressDone = new CmpButton([BlkJSX,"btnNATPolicyDone"],
                                                     null, 
                                                     {action:"FNCCALL", 
                                                      context:null, 
                                                      fnc:function() {
                                                        cmpDlgEditNetworkAddressSetPublish.call(this, NetworkAddressNID);
                                                      }
                                                     });
                                                                                         
  var cmpDlgEditNetworkAddress = new CmpContainer([BlkJSX,"DialogNetworkAddress"],
                                                    [ {cmp:cmpBtnEditNetworkAddressClose},
                                                      {cmp:cmpBtnEditNetworkAddressCancel},
                                                      {cmp:cmpBtnEditNetworkAddressDone},
                                                      {cmp:cmpBtnEditNetworkAddressAddRuleKey},
                                                      {cmp:cmpBtnEditNetworkAddressAddRemote},
                                                      {cmp:cmpBtnEditNetworkAddressAddNATEntry},
                                                      {cmp:cmpBtnEditNetworkAddressAddLocal},
                                                      {cmp:cmpSelNATRemoteFilter},
                                                      {cmp:cmpSelNATLocalFilter},
                                                      {cmp:cmpSelRuleKeyLHTag},
                                                      {cmp:cmpSelRuleKeyRHTag},
                                                      {cmp:cmpBtnRadioDNSName},
                                                      {cmp:cmpBtnRadioIP},
                                                      {cmp:cmpBtnRadioNATZone},
                                                      {cmp:cmpBlkNATLocalFilterMatrix},
                                                      {cmp:cmpBlkNATRemoteFilterMatrix},
                                                      {cmp:cmpBlkNATRuleKeyMatrix},
                                                      {cmp:cmpBlkNATTableMatrix},
                                                      {cmp:cmpCDFNATPolicy}
                                                    ],
                                                    [{action:"TOPICEVENT", topic:tDlg, eventvar:"EditNetworkAddressDataState", eventval:NetworkAddressNID}]);                            
                      
  cmpDlgEditNetworkAddress.BlkJSX = BlkJSX;      
  cmpDlgEditNetworkAddress.ObjectNID = NetworkAddressNID;   
  cmpDlgEditNetworkAddress.getBlkData =  cmpDlgEditNetworkAddressGetPublish;
  
  // init and repaint happens after the dialog is loaded
  jsx3.sleep( (function(){
                 this.init();
                 this.getBlkData(this.ObjectNID);
                 }
               ),
               null, cmpDlgEditNetworkAddress ); 

  // fnc to get dlg data
  function cmpDlgEditNetworkAddressGetPublish(NetworkAddressNID) { 
    var NetworkAddressNode = topic_GetNode(tCache, "//*[@NID='" + NetworkAddressNID + "']");
    if( NetworkAddressNode == null ) {
      var reqData = new ReqGetObjectNID(NetworkAddressNID, "NetworkAddressRule",
                                         {action:"FNCCALL", context:this, 
                                           fnc:function(respCode, respVal) {
                                                 if(respCode == "SUCCESS" && respVal != null) {
                                                   cmpDlgEditNetworkAddressGetPublish(NetworkAddressNID);
                                                 } else {
                                                   var codeErrorMsg = CommonErrorCodeArr[respCode].errorDesc;
                                                   var ErrorMsg = "Error: NetworkAddress policy get data request failed \n";
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
      // using the old repaint of the dialog
      DisplayEditValuesForNetworkAddress(NetworkAddressNID);
    }
  }  // end cmpDlgEditNetworkAddressGetPublish

  // fnc to set dlg data - runs in the context of the OK button
  function cmpDlgEditNetworkAddressSetPublish(NetworkAddressNID) {
    // validate input 
    if( cmpDlgEditNetworkAddressValidate() != true ) return;
    // decide add or update
    var SetMsg = "Edit NetworkAddress policy";
    var SetMode = "Replace";
    if( NetworkAddressNID == "DEFAULT_NAT_NID" ) {
      SetMsg = "Add NetworkAddress policy";
      SetMode = "Add";
    }

    // get NetworkAddress node info from UI
    var NetworkAddressNode = null;

    // write the data back to the CDF
    cmpCDFNATPolicy.BlkJSX.write();

    var strXMLCacheId = cmpCDFNATPolicy.BlkJSX.getXMLId();
    var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);      
    var xslfile = PNameSpace.loadInclude("JSXAPPS/IPVS_AdminUI/xsl/NATPolicyRequest.xsl","NATPolicyRequest","xsl");
    var xslDoc = jsx3.xml.XslDocument.wrap(xslfile);
    var natConfigXML = xslDoc.transform(objXML);
    NetworkAddressNode = new jsx3.xml.Document();
    NetworkAddressNode.loadXML(natConfigXML);

    var strXMLCacheId = cmpBlkNATLocalFilterMatrix.BlkJSX.getXMLId();
    var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
    var xslfile = PNameSpace.loadInclude("jsxuser:///JSXAPPS/IPVS_AdminUI/xsl/CDFToPolicyLocalFilter.xsl","LocalNetworkResourceFilterSetNode","xsl");
    var xslDoc = jsx3.xml.XslDocument.wrap(xslfile);
    var newNATLocalFilterSetXML = xslDoc.transform(objXML);        
    var newNATLocalFilterSetNode = new jsx3.xml.Document();
    newNATLocalFilterSetNode.loadXML(newNATLocalFilterSetXML);
    
    var oldNATLocalFilterSetNode = NetworkAddressNode.selectSingleNode(".//LocalNetworkResourceFilterSet");              
    var NATLocalFilterSetParentNode = NetworkAddressNode.selectSingleNode(".//LocalNetworkResourceFilterSet").getParent();
    NATLocalFilterSetParentNode.replaceNode(newNATLocalFilterSetNode,oldNATLocalFilterSetNode);

    var strXMLCacheId = cmpBlkNATRemoteFilterMatrix.BlkJSX.getXMLId();
    var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
    var xslfile = PNameSpace.loadInclude("jsxuser:///JSXAPPS/IPVS_AdminUI/xsl/CDFToPolicyRemoteFilter.xsl","RemoteNetworkResourceFilterSetNode","xsl");
    var xslDoc = jsx3.xml.XslDocument.wrap(xslfile);
    var newNATRemoteFilterSetXML = xslDoc.transform(objXML);        
    var newNATRemoteFilterSetNode = new jsx3.xml.Document();
    newNATRemoteFilterSetNode.loadXML(newNATRemoteFilterSetXML);
    
    var oldNATRemoteFilterSetNode = NetworkAddressNode.selectSingleNode(".//RemoteNetworkResourceFilterSet");              
    var NATRemoteFilterSetParentNode = NetworkAddressNode.selectSingleNode(".//RemoteNetworkResourceFilterSet").getParent();
    NATRemoteFilterSetParentNode.replaceNode(newNATRemoteFilterSetNode,oldNATRemoteFilterSetNode);
    
    var strXMLCacheId = cmpBlkNATRuleKeyMatrix.BlkJSX.getXMLId();
    var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
    var xslfile = PNameSpace.loadInclude("jsxuser:///JSXAPPS/IPVS_AdminUI/xsl/CDFToPolicyRuleKeyFilter.xsl","RuleKeyTagFilterSetNode","xsl");
    var xslDoc = jsx3.xml.XslDocument.wrap(xslfile);
    var newRuleKeyTagFilterSetXML = xslDoc.transform(objXML);        
    var newRuleKeyTagFilterSetNode = new jsx3.xml.Document();
    newRuleKeyTagFilterSetNode.loadXML(newRuleKeyTagFilterSetXML);
    
    var oldRuleKeyTagFilterSetNode = NetworkAddressNode.selectSingleNode(".//RuleKeyTagFilterSet");              
    var ruleKeyTagFilterSetParentNode = NetworkAddressNode.selectSingleNode(".//RuleKeyTagFilterSet").getParent();
    ruleKeyTagFilterSetParentNode.replaceNode(newRuleKeyTagFilterSetNode,oldRuleKeyTagFilterSetNode);

    if(cmpBtnRadioDNSName.BlkJSX.getSelected() == jsx3.gui.RadioButton.SELECTED){
      NetworkAddressNode.selectSingleNode(".//UseDNSName").setValue("true");              
    }
    else if(cmpBtnRadioIP.BlkJSX.getSelected() == jsx3.gui.RadioButton.SELECTED){
      NetworkAddressNode.selectSingleNode(".//UseLANIP").setValue("true");              
    }
    else if(cmpBtnRadioNATZone.BlkJSX.getSelected() == jsx3.gui.RadioButton.SELECTED){
      NetworkAddressNode.selectSingleNode(".//UseNATZoneIP").setValue("true");              
    }
    
    var strXMLCacheId = cmpBlkNATTableMatrix.BlkJSX.getXMLId();
    var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
    var xslfile = PNameSpace.loadInclude("jsxuser:///JSXAPPS/IPVS_AdminUI/xsl/CDFToPolicyNATTable.xsl","NATTableNode","xsl");
    var xslDoc = jsx3.xml.XslDocument.wrap(xslfile);
    var newNATTableXML = xslDoc.transform(objXML);        
    var newNATTableNode = new jsx3.xml.Document();
    newNATTableNode.loadXML(newNATTableXML);
    
    var oldNATTableNode = NetworkAddressNode.selectSingleNode(".//NATTable");              
    var NATTableParentNode = NetworkAddressNode.selectSingleNode(".//NATTable").getParent();
    NATTableParentNode.replaceNode(newNATTableNode,oldNATTableNode);
    
    if( NetworkAddressNode == null ) return;
    // set NID in NetworkAddressNode if replace mode
    if( SetMode == "Replace" ) {
      NetworkAddressNode.setAttribute("NID", NetworkAddressNID);
    }

    // do set request for NetworkAddress              
    var reqSetObject = new ReqSetObject(SetMode, "NetworkAddressRule", NetworkAddressNode, 
                                        {action:"FNCCALL", context:this, 
                                           fnc:function(respCode, respVal) {
                                                 if(respCode == "SUCCESS" && respVal != null) {
                                                   dlgEditNetworkAddress_Destroy();
                                                   topic_Publish(tApp, "NetworkAddressRuleDataUpdate", "ADD");
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
    if( reqSetObject != null) {
      reqSetObject.sendRequest(this);
    }
  } // end cmpDlgEditNetworkAddressSetPublish
  
  function cmpDlgEditNetworkAddressValidate() {
    var valPolicyName = getJsxByName([BlkJSX,"txtNATPolicyName"]).getValue();
    var valPolicyOrder = getJsxByName([BlkJSX,"txtNATPolicyOrder"]).getValue();

    var policyName = checkValue(valPolicyName , [BlkJSX,"txtNATPolicyName"] , "Please enter policy name");
    if(!policyName){
      return false;
    }
    var checkOrder = checkValue(valPolicyOrder , [BlkJSX,"txtNATPolicyOrder"] , "Please enter policy order" );
    if(!checkOrder){
      return false;
    }
    var chkDuplicateName = "";
    var chkDuplicatePriority = "";
    
    if( NetworkAddressNID == "DEFAULT_NAT_NID" ) {
      if(policyName) {
        chkDuplicateName = chkForNameExistence(valPolicyName, [BlkJSX,"txtNATPolicyName"], "mtxpolicyNATList", "Duplicate Policy name. Please enter some other Policy name" );
        if(!chkDuplicateName){
          checkOrder = false;
          return false;
        }
      }
      if(chkDuplicateName) {
        checkOrder = checkIntergerValue(valPolicyOrder , [BlkJSX,"txtNATPolicyOrder"], "Please enter an integer policy order" );
      }
      if(checkOrder) {
        chkDuplicatePriority = chkForOrderExistence(valPolicyOrder, [BlkJSX,"txtNATPolicyOrder"], "mtxpolicyNATList", "Policy Order already exists");
      }
      if(chkDuplicatePriority) {
        return true;
      }    
    } else {
      checkOrder = checkIntergerValue(valPolicyOrder , [BlkJSX,"txtNATPolicyOrder"], "Please enter an integer policy order" );
      if(checkOrder) {
        if( orgOrderNumber != valPolicyOrder) {
          chkDuplicatePriority = chkForOrderExistence(valPolicyOrder, [BlkJSX,"txtNATPolicyOrder"], "mtxpolicyNATList", "Policy Order already exists");
        } else {
          return true;
        }
      }
      if(chkDuplicatePriority) {
        return true;
      }    
    }
          
    return false;
  }  // end cmpDlgEditNetworkAddressValidate
    
  function DisplayEditValuesForNetworkAddress(NetworkAddressNID) {
    var objectNode = topic_GetNode(tCache, ".//*[@NID='" + NetworkAddressNID + "']");

    /*var version = objectNode.getAttribute("Ver");
    var versionBlkJSX = getJsxByName([BlkJSX,"txtXMLVer"]);
    versionBlkJSX.setValue(version);
    
    var revision = objectNode.getAttribute("Rev");
    var revisionBlkJSX = getJsxByName([BlkJSX,"txtXMLRev"]);
    revisionBlkJSX.setValue(revision);    

    var serviceVer = objectNode.getAttribute("serviceVer");
    var serviceVerBlkJSX = getJsxByName([BlkJSX,"txtXMLServiceVersion"]);
    serviceVerBlkJSX.setValue(serviceVer);    

    var jsxtext = objectNode.selectSingleNode(".//rulename").getValue();
    var nameBlkJSX = getJsxByName([BlkJSX,"txtNATPolicyName"]);
    nameBlkJSX.setValue(jsxtext);
    
    var index = objectNode.selectSingleNode(".//order").getValue();
    var orderBlkJSX = getJsxByName([BlkJSX,"txtNATPolicyOrder"]);
    orderBlkJSX.setValue(index);     

    //
    var policyValid = objectNode.selectSingleNode(".//valid").getValue();
    var activeBlkJSX = getJsxByName([BlkJSX,"chkNATPolicyActive"]);
    if(policyValid == "true") {
      activeBlkJSX.setChecked("1");
    } else {
      activeBlkJSX.setChecked("0");
    }*/

    var natNID = objectNode.getAttribute("NID");
    var strXMLCacheId = cmpCDFNATPolicy.BlkJSX.getXMLId();
    var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
    objXML.loadXML(objectNode.toString());
    cmpCDFNATPolicy.BlkJSX.setSourceXML(objXML);         
    cmpCDFNATPolicy.BlkJSX.setCDFId(natNID);

    //
    var localFilterIter  = objectNode.selectNodeIterator(".//LocalNetworkResourceFilterSet/LocalNetworkResourceFilter");
    var objLocalDoc = jsx3.xml.CDF.Document.newDocument();
    while( localFilterIter.hasNext() ) {
      var localFilterIterNode = localFilterIter.next();
      var streamResourceFilterNID = localFilterIterNode.selectSingleNode(".//MediaStreamResourceFilterNID").getValue();
      if( streamResourceFilterNID != "" ) {
        var streamResourceFilterName = getTagObjectFilterName(streamResourceFilterNID);
    
        var o = new Object();
        o.jsxid = jsx3.xml.CDF.getKey();
        o.ResourceFilterName = streamResourceFilterName ;
        o.ResourceFilterNID = streamResourceFilterNID ;

        objLocalDoc.insertRecord(o);
      }
    }
    var strXMLCacheId = cmpBlkNATLocalFilterMatrix.BlkJSX.getXMLId();
    var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
    objXML.loadXML(objLocalDoc.toString());
    cmpBlkNATLocalFilterMatrix.BlkJSX.setSourceXML(objXML);
    cmpBlkNATLocalFilterMatrix.BlkJSX.repaintData();

    //
    var remoteFilterIter  = objectNode.selectNodeIterator(".//RemoteNetworkResourceFilterSet/RemoteNetworkResourceFilter");
    var objRemoteDoc = jsx3.xml.CDF.Document.newDocument();
    while( remoteFilterIter.hasNext() ) {
      var remoteFilterIterNode = remoteFilterIter.next();
      var streamResourceFilterNID = remoteFilterIterNode.selectSingleNode(".//MediaStreamResourceFilterNID").getValue();
      if( streamResourceFilterNID != "" ) {
        var streamResourceFilterName = getTagObjectFilterName(streamResourceFilterNID);
    
        var o = new Object();
        o.jsxid = jsx3.xml.CDF.getKey();
        o.ResourceFilterName = streamResourceFilterName ;
        o.ResourceFilterNID = streamResourceFilterNID ;

        objRemoteDoc.insertRecord(o);
      }
    }
    var strXMLCacheId = cmpBlkNATRemoteFilterMatrix.BlkJSX.getXMLId();
    var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
    objXML.loadXML(objRemoteDoc.toString());
    cmpBlkNATRemoteFilterMatrix.BlkJSX.setSourceXML(objXML);
    cmpBlkNATRemoteFilterMatrix.BlkJSX.repaintData();
    
    //
    var ruleKeyListIter  = objectNode.selectNodeIterator(".//RuleKeyTagFilterSet/RuleKeyTagFilter");
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
    var strXMLCacheId = cmpBlkNATRuleKeyMatrix.BlkJSX.getXMLId();
    var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
    objXML.loadXML(objRuleKeyDoc.toString());
    cmpBlkNATRuleKeyMatrix.BlkJSX.setSourceXML(objXML);
    cmpBlkNATRuleKeyMatrix.BlkJSX.repaintData();
    
    //
    var useDNSName = objectNode.selectSingleNode(".//UseDNSName").getValue();
    var useLANIP = objectNode.selectSingleNode(".//UseLANIP").getValue();
    var useNATTraversalIP = objectNode.selectSingleNode(".//UseNATZoneIP").getValue();

    cmpBtnRadioDNSName.BlkJSX.setSelected(jsx3.gui.RadioButton.UNSELECTED);
    cmpBtnRadioIP.BlkJSX.setSelected(jsx3.gui.RadioButton.UNSELECTED);
    cmpBtnRadioNATZone.BlkJSX.setSelected(jsx3.gui.RadioButton.UNSELECTED);
    if(useDNSName == "true") {
      cmpBtnRadioDNSName.BlkJSX.setSelected(jsx3.gui.RadioButton.SELECTED);
    } 
    if( useLANIP == "true" ) {
      cmpBtnRadioIP.BlkJSX.setSelected(jsx3.gui.RadioButton.SELECTED);
    }
    if( useNATTraversalIP == "true" ) {
      cmpBtnRadioNATZone.BlkJSX.setSelected(jsx3.gui.RadioButton.SELECTED);
    }
    
    /*var ethernetPortEntryID = objectNode.selectSingleNode(".//EthernetPortEntryID").getValue();
    var selNATPublishLANIDBlkJSX = getJsxByName([BlkJSX,"selNATLANID"]);
    if( !jsx3.util.strEmpty(ethernetPortEntryID) ) {
      selNATPublishLANIDBlkJSX.setValue(ethernetPortEntryID);
    }
    var natZoneEntryName = objectNode.selectSingleNode(".//NATZoneEntryName").getValue();
    natZoneEntryNameBlkJSX = getJsxByName([BlkJSX,"textNATPolicy_NATZone"]);
    natZoneEntryNameBlkJSX.setValue(natZoneEntryName);*/
    
    //
    var natEntryIter  = objectNode.selectNodeIterator(".//NATTable/NATEntry");
    var objNATTableDoc = jsx3.xml.CDF.Document.newDocument();
    while( natEntryIter.hasNext() ) {
      var natEntryIterNode = natEntryIter.next();
      var network = natEntryIterNode.selectSingleNode(".//Network").getValue();
      var transNetwork = natEntryIterNode.selectSingleNode(".//TranslatedNetwork").getValue();
      var netmask = natEntryIterNode.selectSingleNode(".//Netmask").getValue();
      if( network != "" && transNetwork != "" && netmask != "" ) {
        var o = new Object();
        o.jsxid = jsx3.xml.CDF.getKey();
        o.Network = network;
        o.TranslatedNetwork = transNetwork;
        o.Netmask = netmask;

        objNATTableDoc.insertRecord(o, null, false);
      }
    }
    var strXMLCacheId = cmpBlkNATTableMatrix.BlkJSX.getXMLId();
    var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
    objXML.loadXML(objNATTableDoc.toString());
    cmpBlkNATTableMatrix.BlkJSX.setSourceXML(objXML);
    cmpBlkNATTableMatrix.BlkJSX.repaintData();
              
    // populate the select with the default values
    updateSelectTagObjectFilters();
    
    // populate the select rule key select with the default value
    updateSelectGroupTag();
    
    if( NetworkAddressNID != "DEFAULT_NAT_NID" ) {
      getJsxByName([BlkJSX,"txtNATPolicyName"]).setEnabled(jsx3.gui.Form.STATEDISABLED, true); 
    }
    
    orgOrderNumber = getJsxByName([BlkJSX,"txtNATPolicyOrder"]).getValue();
    getJsxByName([BlkJSX,"txtNATPolicyName"]).focus();
  } // end  DisplayEditValuesForNetworkAddress
  
  function updateSelectTagObjectFilters() {
    getFilterToSelectElement([BlkJSX,"selNATPolicy_Local"],"Resource");
    getFilterToSelectElement([BlkJSX,"selNATPolicy_Remote"],"Resource");
  }  
  function updateSelectGroupTag() {
    getTagDefToSelectElement([BlkJSX,"selNATPolicy_RuleKeyLHTag"],"ALL");
    getTagDefToSelectElement([BlkJSX,"selNATPolicy_RuleKeyRHTag"],"ALL");  
  }     
                
} // end create                                                

//global function
function getNATDNSNameRadio(){
  return getSingleRadioButtonValue("radioNATPolicy_DNS");
}
function getNATLANIPRadio(){
  return getSingleRadioButtonValue("radioNATPolicy_IP");
}
function getNATZoneRadio(){
  return getSingleRadioButtonValue("radioNATPolicy_NATZone");
}