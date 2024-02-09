function cmpSessionMonitor_Create(cmpCaller, tCaller, eventCaller, windlgCaller){
  // init the topics

  var cmpCallerBlkJSX = cmpCaller.BlkJSX;
  if(windlgCaller == "DIALOG") {
    cmpCallerBlkJSX = cmpCaller;
  }
  
  
  var xslfile = PNameSpace.loadInclude("JSXAPPS/IPVS_ClientUI/xsl/formatXML.xsl","formatXML","xsl");        
  var xslDoc = jsx3.xml.XslDocument.wrap(xslfile);
   
 var cmpBtnSessionRefresh = new CmpButton([cmpCallerBlkJSX, "blkSessionDetails", "btnSessionRefresh"], null,
                                            { action: "FNCCALL", context: null,
                                              fnc: function() {
                                                var sessionID = topic_Get(tCaller,"SessionID");
                                                if( (!jsx3.util.strEmpty(sessionID)) && 
                                                    (sessionID != "NONE") ){
                                                  cmpMtxSessionConnSeg.doRepaint();
                                                }
                                              }
                                            });

  var cmpMtxSessionConnSrc = new Component([cmpCallerBlkJSX, "blkSessionDetails", "mtxSessionSrcDetail"]
                                          );
  cmpMtxSessionConnSrc.BlkJSX.setXMLTransformers("../IPVS-Common/xsl/SessionConnSrc.xsl"); 
                                           
  var cmpMtxSessionConnDest = new Component([cmpCallerBlkJSX, "blkSessionDetails", "mtxSessionDestDetail"]
                                           );
  cmpMtxSessionConnDest.BlkJSX.setXMLTransformers("../IPVS-Common/xsl/SessionConnDest.xsl");                                          
  
  var cmpMtxSessionConnPvrRec = new Component([cmpCallerBlkJSX, "blkSessionDetails", "mtxSessionPVRDetail"]
                                             );                                                                                    
  cmpMtxSessionConnPvrRec.BlkJSX.setXMLTransformers("../IPVS-Common/xsl/SessionConnPvrRec.xsl");
       
  var cmpTxtaSessionXML = new Component([cmpCallerBlkJSX, "blkSessionDetails", "txtaSessionXML"]
                                       );

  //topic_Publish(tCaller, "SessionConnData", (newServerDate()).getTime() );
  var cmpMtxSessionConnSeg = new CmpCustomPaginationList([cmpCallerBlkJSX, "blkSessionDetails","mtxSessionConnections"],
                                                          [{action:"TOPICEVENT", topic: tCaller, eventvar:"SessionConnData", eventval:null}],
                                                          {persist:{col:"jsxid",initval:false}},
                                                          null,
                                                          "showConnErrorDetails"
                                                         ); 

  cmpMtxSessionConnSeg.cmpReqParamArr = {StartFrom: "ObjectList_StartFrom",
                                         CountToFetch: "ObjectList_CountToFetch",
                                         SortOrder: "ObjectList_SortOrder",
                                         ObjectCount: "ObjectCount",
                                         SortByXpath: "ObjectList_SortBy"
                                        };

                                                                                             
  cmpMtxSessionConnSeg.DataUpdatedEvent = { action: "TOPICEVENT", topic: tCaller, eventvar: "SessionConnDataUpadted", eventval: "DATE" };
  cmpMtxSessionConnSeg.BlkJSX.setXMLTransformers("../IPVS-Common/xsl/SessionConn.xsl");
  
  topic_Set(tCaller, "SessionConnSelectFormat", "EXTENDED");

    
  cmpMtxSessionConnSeg.cmpReq = new cmpReq_GetSessionAPIPaginationList(tCaller, "GetSession",
                                                                        [{ VarName: "ObjectList_SelectFormat", VarRef: "SessionConnSelectFormat"},
                                                                         { VarName: "ObjectList_WhereID", VarRef: "SessionID"}
                                                                        ],
                                                                        { action: "FNCCALL", context: cmpMtxSessionConnSeg,
                                                                          fnc: function(respCode, respVal) {
                                                                            if (respCode == "SUCCESS" && respVal != null) {
                                                                              // clone response for the src entities
                                                                              var DataNodeForSrc = respVal.cloneNode(true);
                                                                              setMatrixSourceXML(DataNodeForSrc, cmpMtxSessionConnSrc);
                                                                              // clone response for the dest entities
                                                                               var DataNodeForDest = respVal.cloneNode(true);
                                                                               setMatrixSourceXML(DataNodeForDest, cmpMtxSessionConnDest);
                                                                              // clone response for the pvr/rec entities
                                                                               var DataNodeForPvrRec = respVal.cloneNode(true);
                                                                               setMatrixSourceXML(DataNodeForPvrRec, cmpMtxSessionConnPvrRec);
                                                                              // set the xml in the text area
                                                                              var DataXMLNode = respVal.cloneNode(true);
                                                                              setTextAreaXML(DataXMLNode, cmpTxtaSessionXML);
                                                                              // data for the connections
                                                                              this.onResponse(respCode, respVal);
                                                                            }
                                                                            else {
                                                                              log.info("IPVSClientUI:GetSessionRequest failed. Error Code :" + respCode);
                                                                              this.onResponseErrorHandle("GetSession",respCode, respVal);
                                                                            }
                                                                          }
                                                                        }
                                                                      ); 


  var cmpBlkSessionDetails = new CmpContainer([cmpCallerBlkJSX, "blkSessionDetails"],
                                              [{cmp:cmpMtxSessionConnSeg},
                                               {cmp:cmpMtxSessionConnSrc},
                                               {cmp:cmpMtxSessionConnDest},
                                               {cmp:cmpMtxSessionConnPvrRec},
                                               {cmp:cmpTxtaSessionXML},
                                               {cmp:cmpBtnSessionRefresh}
                                              ],
                                              null
                                              );
                                                
  var cmpBlkSessionMonitor = new CmpContainer([cmpCallerBlkJSX, "blkSessionMonitorUI"],  
                                              [{cmp:cmpBlkSessionDetails}
                                              ],
                                              null ); 


  function setMatrixSourceXML(dataNode, mtxObj) {
    var strXMLCacheId = mtxObj.BlkJSX.getXMLId();
    var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
    objXML.loadXML(dataNode.toString());

    mtxObj.BlkJSX.setSourceXML(objXML);
    mtxObj.BlkJSX.repaint();  
    var objXML2 = PNameSpace.getCache().getDocument(strXMLCacheId);
  } // end   setMatrixSourceXML

  function setTextAreaXML(dataNode, textaObj) {
    var xmlData = "";
    textaObj.BlkJSX.setValue(xmlData);

    var sessionNode = dataNode.selectSingleNode(".//Session");    
    if(!jsx3.util.strEmpty(sessionNode)) {                                                                              
      xmlData = xslDoc.transform(sessionNode);
      xmlData = xmlData.toString();
    }
    textaObj.BlkJSX.setValue(xmlData);    
  } // end setTextAreaXML

  return cmpBlkSessionMonitor;

} // end cmpSessionStatus_Create

function showConnErrorDetails(objJSX, strRecordId){
  var ObjRecord = objJSX.getRecord(strRecordId);
  var conStatus = ObjRecord.jsxSessionConnectionStatus;
  if( (conStatus=="SETUP_ERROR") ||
      (conStatus=="START_ERROR") ||
      (conStatus=="SOURCE_ERROR") ||
      (conStatus=="MEDIA_ERROR")  ||
      (conStatus=="error") ) {
    var ErrCode =  ObjRecord.jsxSessionConnectionErrorCode;
    var ErrDesc = ObjRecord.jsxSessionConnectionErrorDesc;
    var strSpyHTML = '<div id="outerDiv" style="margin: -4px; background-color: #ffffcc;">';
    strSpyHTML = strSpyHTML + '<table width="200" hight="200" border="0">';
    strSpyHTML = strSpyHTML + '<tr><td style="font-family:Arial;font-size:10px;">Error Code: </td><td style="font-family:Arial;font-size:10px;">' + ErrCode + '</td></tr>';
    strSpyHTML = strSpyHTML + '<tr><td style="font-family:Arial;font-size:10px;">Error Desc: </td><td style="font-family:Arial;font-size:10px;">' + ErrDesc + '</td></tr></table></div>';
    return strSpyHTML;
  }
  return;
} // end showConnErrorDetails

function closeDialogSessionMonitor(sessionID){
  var objParent = PNameSpace.getServer().getJSXByName("blkMediaControllerBody");
  if (jsx3.gui.Dialog){
    try {
      var dialogBoxArr = objParent.getDescendantsOfType(jsx3.gui.Dialog, false);
      for (var i = 0; i < dialogBoxArr.length; i++) {
        var dlgName = dialogBoxArr[i].getName();
        if( jsx3.util.strEmpty(sessionID) ){
          dialogBoxArr[i].getDescendantOfName("btnDialogCloseX").doExecute();
        }
        else if( dlgName == sessionID ) {
          //If we use dialogBoxArr[i].doClose(), dialog is closing but it is not calling destory method
          dialogBoxArr[i].getDescendantOfName("btnDialogCloseX").doExecute();
          break;
        }
      }
    } catch (e) {
    }
  }  
} // end closeDialogSessionMonitor