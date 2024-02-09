// this code has some potential issues 
// - w.r.t object reference counts - when objects created in here go out of scope etc
// - loading sync issues - dlg may not be loaded before the object init methods are called

function dlgEditMediaSDR_Create(MediaDirNID) {

  // deal with defaults for add
  if( MediaDirNID == null ) MediaDirNID = "DEFAULT_MEDIASTREAMDIR_NID";

  closeDialogIfExists("DialogMediaDir");
  // load view - dialog component file
  var DlgFile = "components/media/mediaAddDirectoryDialog.xml";
  var BlkJSX = PNameSpace.getBodyBlock().load(DlgFile);

  // setup a local topic for dlg specific messaging
  var tDlg = "Dialog_" + jsx3.xml.CDF.getKey()
  topic_Create(tDlg);

  // setup a listner for it
  var eventDlg = new EventListner(tDlg);

  function dlgEditMediaSDR_Destroy() {
    // destroy any local eventlistners
    eventDlg.destroy();
    
    // destroy all local components
    cmpDlgEditMediaSDR.destroy();
    
    // destroy the local topic
    topic_Destroy(tDlg);
    
    // close dialog
    BlkJSX.doClose();
  }


  var cmpBtnEditMediaDirAddGroupTag = new CmpButton("btnMediaDirAddGroupTag",
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
                                      
  cmpBtnEditMediaDirAddGroupTag.GroupTagNameBlkJSX = PNameSpace.getJSXByName("cmbMediaDirTagName");
  cmpBtnEditMediaDirAddGroupTag.GroupTagValueBlkJSX = PNameSpace.getJSXByName("cmbMediaDirTagValue");
  cmpBtnEditMediaDirAddGroupTag.GroupTagListBlkJSX = PNameSpace.getJSXByName("mtxMediaDirGroupTags");
  
  var cmpBlkEditMediaDirGroupMatrix = new CmpBlock([BlkJSX,"mtxMediaDirGroupTags"]);
  
  var cmpCmbSelMediaDirTagDefName = new CmpCustomPaginationList([BlkJSX, "cmbMediaDirTagName"],
                                                                 [{action:"TOPICEVENT", topic: tDlg, eventvar:"MediaDirTagDefNameState", eventval:null}],
                                                                   { persist: { col: "jsxtext", initval: false },
                                                                       action: "TOPICEVENT", topic: tDlg, eventvar: "MediaDirTagDefNameSelected", eventval: "jsxid"
                                                                   },
                                                                null,
                                                                null,
                                                                "xsl/TagNameList.xsl"
                                                                );

  cmpCmbSelMediaDirTagDefName.cmpReqParamArr = { StartFrom: "ObjectList_StartFrom",
                                                  CountToFetch: "ObjectList_CountToFetch",
                                                  SortOrder: "ObjectList_SortOrder",
                                                  ObjectCount: "ObjectCount",
                                                  SortByXpath: "ObjectList_SortBy",
                                                  WhereObjectType: "ObjectList_WhereObjectType"
                                              };

  topic_Set(tDlg, "MediaDirTagNameResponseSelectFormat", "NameValueTree");
  topic_Set(tDlg, "MediaDirTagNameResponseObjectType", "Media");

  cmpCmbSelMediaDirTagDefName.cmpReq = new cmpReq_GetSessionAPIPaginationList(tDlg, "GetTags",
                                                                              [{ VarName: "ObjectList_SelectFormat", VarRef: "MediaDirTagNameResponseSelectFormat"},
                                                                               { VarName: "ObjectList_WhereObjectType", VarRef: "MediaDirTagNameResponseObjectType"}],
                                                                              { action: "FNCCALL", context: cmpCmbSelMediaDirTagDefName,
                                                                                  fnc: function(respCode, respVal) {
                                                                                      if (respCode == "SUCCESS" && respVal != null) {
                                                                                          this.onResponse(respCode, respVal);
                                                                                      }
                                                                                      else {
                                                                                          log.info("GetTagsRequest failed. Error Code :" + respCode);
                                                                                          this.onResponseErrorHandle("GetTags",respCode, respVal);
                                                                                      }
                                                                                  }
                                                                              }
                                                                            );

  var cmpCmbSelMediaDirTagDefValue = new Component([BlkJSX, "cmbMediaDirTagValue"]);
  cmpCmbSelMediaDirTagDefValue.BlkJSX.setXMLTransformers("xsl/TagValueList.xsl");

  eventDlg.subscribe("MediaDirTagDefNameSelected", null, this,
                      function(eventVar, eventVal) {
                        if (eventVal != "NONE") {
                          cmpCmbSelMediaDirTagDefValue.BlkJSX.setValue("");
                          var nameRecord = cmpCmbSelMediaDirTagDefName.BlkJSX.getValue();
                          var selNameRecord = cmpCmbSelMediaDirTagDefName.BlkJSX.getRecordNode(nameRecord);
                          var tagNode = selNameRecord.selectSingleNode(".//Tag");
                          var strXMLValueCacheId = cmpCmbSelMediaDirTagDefValue.BlkJSX.getXMLId();
                          var objXML = PNameSpace.getCache().getDocument(strXMLValueCacheId);
                          objXML.loadXML(tagNode.toString());
                          cmpCmbSelMediaDirTagDefValue.BlkJSX.setSourceXML(objXML); 
                        }
                      }
                    );

  var cmpBtnEditMediaDirOK = new CmpButton(["DialogMediaDir","btnDoneMediaSDR"],
                                           null, 
                                           {action:"FNCCALL", 
                                            context:null, 
                                            fnc:function() {
                                              cmpDlgEditMediaSDRSetMediaSDR.call(this, MediaDirNID);
                                            }
                                           });

  var cmpBtnEditMediaDirClose = new CmpButton(["DialogMediaDir","btnDialogCloseX"],
                                               null, 
                                               {action:"FNCCALL", 
                                                context:null, 
                                                fnc:function() {
                                                  dlgEditMediaSDR_Destroy();
                                                }
                                               });
            
  var cmpBtnEditMediaDirCancel = new CmpButton(["DialogMediaDir","btnCancelMediaSDR"],
                                               null, 
                                               {action:"FNCCALL", 
                                                context:null, 
                                                fnc:function() {
                                                  dlgEditMediaSDR_Destroy();
                                                }
                                              });

  var cmpBlkEditMediaDirMediaStoreMatrix = new CmpBlock([BlkJSX, "mtxMediaDirMediaStore"]);

  var cmpTxtMediaDirectoryName = new Component([BlkJSX, "txtMediaDirectoryName"]);
  var cmpTxtMediaDirectoryDescription = new Component([BlkJSX, "txtMediaDirectoryDescription"]);

  var cmpDlgEditMediaSDR = new CmpContainer("DialogMediaDir",
                                [{ cmp: cmpBtnEditMediaDirClose },
                                  { cmp: cmpBtnEditMediaDirCancel },
                                  { cmp: cmpBtnEditMediaDirOK },
                                  { cmp: cmpCmbSelMediaDirTagDefName },
                                  { cmp: cmpCmbSelMediaDirTagDefValue },
                                  { cmp: cmpBtnEditMediaDirAddGroupTag },
                                  { cmp: cmpBlkEditMediaDirGroupMatrix },
                                  { cmp: cmpTxtMediaDirectoryName },
                                  { cmp: cmpTxtMediaDirectoryDescription }

                                ],
                                [{ action: "TOPICEVENT", topic: tDlg, eventvar: "EditMediaDirDataState", eventval: MediaDirNID}]);                            
  
  cmpDlgEditMediaSDR.BlkJSX = BlkJSX;
  cmpDlgEditMediaSDR.ObjectNID = MediaDirNID;
  cmpDlgEditMediaSDR.getBlkData =  cmpReq_GetDir;
  cmpDlgEditMediaSDR.tDlg = tDlg;

  // init and repaint happens after the dialog is loaded
  jsx3.sleep( (function(){
                 this.init();
                 this.getBlkData(this.tDlg, this.ObjectNID);
                 topic_Publish(tDlg, "MediaDirTagDefNameState", (newServerDate()).getTime());
                 }
               ),
               null, cmpDlgEditMediaSDR );

  // event published from func ( cmpReq_GetDir ) defined in cmpReq.js                  
  eventDlg.subscribe("GetCompleteMediaDirDone", MediaDirNID, this,
                      function() {
                        cmpDlgEditMediaSDRGetMediaSDR(MediaDirNID);
                      }
                    ); // subscribe end 
  // fnc to get dlg data
  function cmpDlgEditMediaSDRGetMediaSDR(MediaDirNID) { 
    // fnc to process dlg data
    var reqStatus = topic_Get(tDlg, "GetCompleteMediaDirStatus");
    if(reqStatus != "SUCCESS") {
      dlgEditMediaSDR_Destroy();  
    } 
    var mediaDirNode = getDialogDataNode(tDlg, "Dir");      
    if( mediaDirNode == null ) {
      alert("Internal error: Not able to find media directory");
      dlgEditMediaSDR_Destroy();
    } else {
      // deleting empty group tag name and value from the default xml
      deleteEmptyGroupTag(mediaDirNode);
      if(cmpBlkEditMediaDirGroupMatrix.BlkJSX.clearXmlData != null) {
        cmpBlkEditMediaDirGroupMatrix.BlkJSX.clearXmlData();
      }

      var GroupIter = mediaDirNode.selectNodeIterator(".//Tag");
      while( GroupIter.hasNext() ) {
        var tagNode = GroupIter.next();
        var TagName = tagNode.getAttribute("Name");
        var TagValue = tagNode.getAttribute("Value");
        var o = new Object;
        o.jsxid = TagName + TagValue;
        o.jsxTagName = TagName;
        o.jsxTagValue = TagValue;
        cmpBlkEditMediaDirGroupMatrix.BlkJSX.insertRecord(o, null, false);
        cmpBlkEditMediaDirGroupMatrix.BlkJSX.repaintData();
      }

      var dirName = mediaDirNode.getAttribute("title");
      cmpTxtMediaDirectoryName.BlkJSX.setValue(dirName, true);

      var dirDesc = mediaDirNode.selectSingleNode("./description").getValue();
      cmpTxtMediaDirectoryDescription.BlkJSX.setValue(dirDesc, true);

      topic_Publish(tDlg, "EditMediaDirDataState", MediaDirNID);
    }

  } // end cmpDlgEditMediaSDRGetMediaSDR

  // fnc to set dlg data - runs in the context of the OK button
  function cmpDlgEditMediaSDRSetMediaSDR(MediaDirNID) {
    // validate input 
    if( cmpDlgEditMediaSDRValidate() != true ) return;
    // decide add or update
     var SetMsg = "Edit Media dir";
    var SetMode = "Replace";
    if( MediaDirNID == "DEFAULT_MEDIASTREAMDIR_NID" ) {
      SetMode = "Add";
    }
    
    var AddEditNode = null;
    var updateFileGroup = false;
    if( SetMode == "Replace" ) {
      var dirResourceNodeList = topic_GetNode(tCache, "//MediaStreamDirResourceList");
      var orgMediaDirNode = dirResourceNodeList.selectSingleNode("//MediaStreamDirResource[(@NID='" + MediaDirNID + "')] ");
           
      var orgMediaDirGroupNode = orgMediaDirNode.selectSingleNode(".//Info/Groups");

      var title = cmpTxtMediaDirectoryName.BlkJSX.getValue();
      var description = cmpTxtMediaDirectoryDescription.BlkJSX.getValue();

      var dateModified = new Date();

      //get the groups
      var strXMLCacheId = cmpBlkEditMediaDirGroupMatrix.BlkJSX.getXMLId();
      var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
      //convert the matrix data to Tag nodes
      var xslfile = PNameSpace.loadInclude("jsxuser:///JSXAPPS/IPVS_AdminUI/xsl/NewCDFTagToTag.xsl", "TagNode", "xsl");
      var xslDoc = jsx3.xml.XslDocument.wrap(xslfile);
      var groupTagsXML = xslDoc.transform(objXML);
      var groupNode = new jsx3.xml.Document();
      groupNode.loadXML(groupTagsXML);
      
      orgMediaDirNode.selectSingleNode(".//Info/Title").setValue(title);
      orgMediaDirNode.selectSingleNode(".//Info/Description").setValue(description);
      
      orgMediaDirNode.selectSingleNode(".//Info/Properties/DateModified").setValue(dateModified);

      orgMediaDirGroupNode.getParent().replaceNode(groupNode, orgMediaDirGroupNode);
     
      //Check groups got changed or not
      var orgMediaDirGroupNodeStr = orgMediaDirGroupNode.toString();
      orgMediaDirGroupNodeStr = orgMediaDirGroupNodeStr.replace(/<\?.*\?>/,"").replace(/[\n\r]/g,"").replace(/>\s+</g, "><");
      
      var groupNodeStr = groupNode.cloneNode(true);
      var groupNodeStr = groupNodeStr.toString();
      groupNodeStr = groupNodeStr.replace(/<\?.*\?>/,"").replace(/[\n\r]/g,"").replace(/>\s+</g, "><");

      if( orgMediaDirGroupNodeStr != groupNodeStr ) {
        var agree = confirm('Do you want to update groups for all media files in this directory?');
        if( agree ) {
          updateFileGroup = true;
        }
      }
      
      AddEditNode = orgMediaDirNode;                
    } else {
      SetMsg = "Add Media dir";
      var dirNID = topic_Get(tApp, "MediaSDRSelected");
      var matchResult = dirNID.search(/jsx/);

      if(matchResult != -1) {
      } else {
        mediaDirNode.selectSingleNode(".//Info/Properties/ParentMediaDirNID").setValue(dirNID);
      }
      AddEditNode = mediaDirNode;
    }
    
    //delete empty group tag node
    deleteEmptyGroupTag(AddEditNode);
    // do set request for 
    var reqSetMediaDirObject = new ReqSetObject(SetMode, "MediaStreamDirResource", AddEditNode, 
                                                 //{action:"TOPICEVENT", topic:tDlg, eventvar:"EditMediaDirSetMediaDirState", eventval:"DONE"}
                                                  {action:"FNCCALL", context:this, 
                                                     fnc:function(respCode, respVal) {
                                                           if(respCode == "SUCCESS" && respVal != null) {
                                                             if( updateFileGroup ) {
                                                               cmpDlgEditMediaSDRUpdateMediaSFRGroups(MediaDirNID, groupNode);
                                                             }
                                                             else {
                                                               dlgEditMediaSDR_Destroy();
                                                             }
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
    reqSetMediaDirObject.sendRequest();
  }  // end cmpDlgEditMediaSDRSetMediaSDR

  function cmpDlgEditMediaSDRUpdateMediaSFRGroups(mediaDirNID, groupNode) {
    if( mediaDirNID == null ) return;   
    var reqSetMediaSFRObject = new ReqSetObject( "Replace", "MediaStreamFileResource", groupNode, 
                                                 {action:"FNCCALL", context:this, 
                                                   fnc:function(respCode, respVal) {
                                                         if(respCode == "SUCCESS" && respVal != null) {
                                                           dlgEditMediaSDR_Destroy();
                                                         } else {
                                                           var codeErrorMsg = CommonErrorCodeArr[respCode].errorDesc;
                                                           var ErrorMsg = "Error: Media Files update group request failed \n";
                                                           ErrorMsg += "Error description: " + codeErrorMsg + "\n";
                                                           ErrorMsg += "Error code: " + respCode;
                                                           alert(ErrorMsg);
                                                         }
                                                       }
                                                 }                                                 
                                               );
    var reqParams = new Object;
    reqParams["Op_Where_WhereXPath"] = "//ParentMediaDirNID='" + mediaDirNID + "'";
    reqParams["Op_In_ChildXPath"] = "/MediaStreamFileResource/Info/Groups";
    reqSetMediaSFRObject.reqParams = reqParams;
    reqSetMediaSFRObject.sendRequest();
  }

  function cmpDlgEditMediaSDRValidate() {
    var valFullName = PNameSpace.getJSXByName("txtMediaDirectoryName").getValue();

    var fullName = checkValue(jsx3.util.strTrim(valFullName) , "txtMediaDirectoryName" , "Please enter directory name");
    if(fullName) {
      // after checking dir name charaters check for duplicate name.
      //get the dir list from the tCache
      var dirResourceNodeList = topic_GetNode(tCache, "//MediaStreamDirResourceList");
      //get currently selected dir
      var dirNID = topic_Get(tApp, "MediaSDRSelected");
      var objDirNameNodes = null;
      
      objDirNameNodes = dirResourceNodeList.selectNodes("//MediaStreamDirResource[(@NID!='"+ dirNID +"') and (.//Info/Title='" + valFullName +"')] ");
      // id objDir == null that means no duplicate otherwise display alert/message
      if( objDirNameNodes.size() > 0) {
        alert("Duplicate directory name. Please enter some other directory name");
        PNameSpace.getJSXByName("txtMediaDirectoryName").focus();
        return false;
      }
 
      return true;
    }  
    return false;
  } //end cmpDlgEditMediaSDRValidate
  
}
