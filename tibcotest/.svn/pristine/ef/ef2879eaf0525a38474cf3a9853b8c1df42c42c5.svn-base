// this code has some potential issues 
// - w.r.t object reference counts - when objects created in here go out of scope etc
// - loading sync issues - dlg may not be loaded before the object init methods are called

function dlgEditCanvas_Create(responseXML) {

  // load view - dialog component file
  var DlgFile = "components/canvasEditDisplayDialog.xml";
  var umcName = responseXML.selectSingleNode(".//record").getAttribute("jsxtext");
  var dlgHeaderName = umcName + " Display Profile";
  // check if the dialog is already open  
  var objParent = PNameSpace.getServer().getJSXByName("blkMediaControllerBody");

  if (jsx3.gui.Dialog) {
    try {
      var dialogBoxArr = objParent.getDescendantsOfType(jsx3.gui.Dialog, false);
      for (var i = 0; i < dialogBoxArr.length; i++) {
        var dlgName = dialogBoxArr[i].getName();
        if (dlgName == dlgHeaderName) {
          dialogBoxArr[i].focus();
          return;
        }
      }
    } catch (e) {
    }
  }
  //load the dialog file
  var BlkJSX =  objParent.load(DlgFile);
  BlkJSX.setName(dlgHeaderName);
  BlkJSX.getChild("windowBar").setTip(dlgHeaderName, "true");
  BlkJSX.getChild("windowBar").setText(dlgHeaderName, "true");
   
  
  
  var tDlg = "Dialog_" + jsx3.xml.CDF.getKey()
  topic_Create(tDlg);
  var eventDlg = new EventListner(tDlg);
  
  function dlgEditCanvas_Destroy() {
    // destroy any local eventlistners
    eventDlg.destroy();
    
    // destroy all local components
    cmpDlgEditCanvas.destroy();
    
    // destroy the local topic
    topic_Destroy(tDlg);
    
    // close dialog
    BlkJSX.doClose();    
  }

 
  var cmpBtnEditCanvasClose = new CmpButton([BlkJSX,"btnDialogCloseX"],
                                                     null, 
                                                     {action:"FNCCALL", 
                                                      context:null, 
                                                      fnc:function() {
                                                        dlgEditCanvas_Destroy();
                                                      }
                                                     });
            
  var cmpBtnEditCanvasCancel = new CmpButton([BlkJSX,"btnEditCanvasCancel"],
                                                       null, 
                                                       {action:"FNCCALL", 
                                                        context:null, 
                                                        fnc:function() {
                                                          dlgEditCanvas_Destroy();
                                                        }
                                                       });
            
  var cmpBtnEditCanavsOK = new CmpButton([BlkJSX,"btnEditCanvasDone"],
                                                   null, 
                                                   {action:"FNCCALL", 
                                                    context:null, 
                                                    fnc:function() {
                                                      var ObjXML= cmpBlkCanvasEditor.getCanvasXML();
                                                      cmpDlgEditCanvasSetCanvas.call(this,ObjXML);
                                                      dlgEditCanvas_Destroy(); 
                                                    }
                                                   });

  var cmpBlkCanvasEditor = new CmpCanvasEditorContainer([BlkJSX,"blkCanvasEditorSetup"],
                                                              [[BlkJSX,"BlkDisplayEditCanvas"],
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
                                                              BlkJSX,
                                                              "ClientEditCanavsStatus"
                                                              );
                                                                                                                                                               
  var cmpDlgEditCanvas = new CmpContainer([BlkJSX,"DialogCanvas"],
                                          [ {cmp:cmpBtnEditCanvasClose},
                                            {cmp:cmpBtnEditCanvasCancel},
                                            {cmp:cmpBtnEditCanavsOK},
                                            {cmp:cmpBlkCanvasEditor}
                                           ],
                                           null
                                         );                            
                
  cmpDlgEditCanvas.BlkJSX = BlkJSX;   
  cmpDlgEditCanvas.responseXML = responseXML;
  cmpDlgEditCanvas.getBlkData =  cmpDlgEditCanvasGetCanvas;
  
  // init and repaint happens after the dialog is loaded
  jsx3.sleep( (function(){
                this.init();
                this.getBlkData(this.responseXML);
               }
              ),
            null, cmpDlgEditCanvas );
      
    // fnc to get dlg data
    function cmpDlgEditCanvasGetCanvas(responseXML) {  
      cmpBlkCanvasEditor.onResponse('SUCCESS',responseXML.toString());
      cmpBlkCanvasEditor.cmpTxtDisplayHeight.BlkJSX.setEnabled(jsx3.gui.Form.STATEDISABLED, true);
      cmpBlkCanvasEditor.cmpTxtDisplayWidth.BlkJSX.setEnabled(jsx3.gui.Form.STATEDISABLED, true);
    } 
    
    //fnc to set the Canvas
    function cmpDlgEditCanvasSetCanvas(responseXML) {
     var displayNode =responseXML.selectSingleNode(".//record/Display").cloneNode(true);
     var canvasNode =responseXML.selectSingleNode(".//record/Canvas").cloneNode(true);
     var congifNode = topic_Set(tMediaController, "IPVSDisplayConfig",null); 
     congifNode.removeChildren();
     // not send ing the display node as nothing has changed for the node. It will save the 2 request/notifications to process
     //congifNode.appendChild(displayNode);
     congifNode.appendChild(canvasNode);
     topic_Publish(tMediaController, "SetDisplayPortStatus", (new Date()).getTime()); 
    } 
} // end create

