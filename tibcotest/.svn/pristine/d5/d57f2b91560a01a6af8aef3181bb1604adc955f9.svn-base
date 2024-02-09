// this code has some potential issues 
// - w.r.t object reference counts - when objects created in here go out of scope etc
// - loading sync issues - dlg may not be loaded before the object init methods are called

function dlgEditDisplayProfile_Create(DisplayProfileNID) {

  // deal with defaults for add
  if( DisplayProfileNID == null ) DisplayProfileNID= "DEFAULT_DISPLAYPROFILE_NID";
  
  // load view - dialog component file
  var DlgFile = "components/assets/profileDisplayDialog.xml";
  var BlkJSX = PNameSpace.getBodyBlock().load(DlgFile);
  
  var tDlg = "Dialog_" + jsx3.xml.CDF.getKey()
  topic_Create(tDlg);
  var eventDlg = new EventListner(tDlg);
  
  var dlgText = "Add Display Profile";
  if( DisplayProfileNID != "DEFAULT_DISPLAYPROFILE_NID" ){
    dlgText = "Edit Display Profile";
  }
  var windowBarBlkJSX = getJsxByName([BlkJSX, "windowBar"]);
  windowBarBlkJSX.setText(dlgText, true);
  closeDialogIfExists(dlgText);

  function dlgEditDisplayProfile_Destroy() {
    // destroy any local eventlistners
    eventDlg.destroy();
    
    // destroy all local components
    cmpDlgEditDisplayProfile.destroy();
    
    // destroy the local topic
    topic_Destroy(tDlg);
    
    // close dialog
    BlkJSX.doClose();    
  }

 
  var cmpBtnEditDisplayProfileClose = new CmpButton([BlkJSX,"btnDialogCloseX"],
                                                     null, 
                                                     {action:"FNCCALL", 
                                                      context:null, 
                                                      fnc:function() {
                                                        dlgEditDisplayProfile_Destroy();
                                                      }
                                                     });
            
  var cmpBtnEditDisplayProfileCancel = new CmpButton([BlkJSX,"btnDisplayProfileCancel"],
                                                       null, 
                                                       {action:"FNCCALL", 
                                                        context:null, 
                                                        fnc:function() {
                                                          dlgEditDisplayProfile_Destroy();
                                                        }
                                                       });
            
  var cmpBtnEditDisplayProfileOK = new CmpButton([BlkJSX,"btnDisplayProfileDone"],
                                                   null, 
                                                   {action:"FNCCALL", 
                                                    context:null, 
                                                    fnc:function() {
                                                      cmpDlgEditDisplayProfileSetDisplayProfile.call(this, DisplayProfileNID);  
                                                    }
                                                   });



      
  var cmpTxtDisplayProfileName = new Component([BlkJSX,"txtDisplayProfileName"]);
  var cmpTxtDisplayProfilePriority = new Component([BlkJSX,"txtDisplayProfilePriority"]);
  var cmpTxtDisplayProfileDesc = new Component([BlkJSX,"txtDisplayProfileDesc"]);
  
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
                                                                                                                                                               
  var cmpDlgEditDisplayProfile = new CmpContainer([BlkJSX,"DialogDisplayProfile"],
                                                  [ {cmp:cmpBtnEditDisplayProfileClose},
                                                    {cmp:cmpBtnEditDisplayProfileCancel},
                                                    {cmp:cmpBtnEditDisplayProfileOK},
                                                    {cmp:cmpTxtDisplayProfileName},
                                                    //{cmp:cmpTxtDisplayProfilePriority},
                                                    {cmp:cmpTxtDisplayProfileDesc},
                                                    {cmp:cmpDisplayProfileSetting}                                        ],
                                                  [{action:"TOPICEVENT", topic:tDlg, eventvar:"EditDisplayprofileDataState", eventval:DisplayProfileNID}]
                                                 );                            
                
  cmpDlgEditDisplayProfile.BlkJSX = BlkJSX;   
  cmpDlgEditDisplayProfile.ObjectNID = DisplayProfileNID;
  cmpDlgEditDisplayProfile.getBlkData =  cmpDlgEditDisplayProfileGetDisplayProfile;
  
  // init and repaint happens after the dialog is loaded
  jsx3.sleep( (function(){
                this.init();
                this.getBlkData(this.ObjectNID);
               }
              ),
            null, cmpDlgEditDisplayProfile );
 
      
    // fnc to get dlg data
    function cmpDlgEditDisplayProfileGetDisplayProfile(DisplayProfileNID) {  
      
      // helper function to process the displayprofile get data
      function processDisplayProfile(displayProfileNID){
        var displayprofileNode = topic_GetNode(tCache, "//*[@NID='" + displayProfileNID + "']");
        // deleting empty group tag name and value from the default xml
        deleteEmptyGroupTag(displayprofileNode);
        
        var profileName = displayprofileNode.selectSingleNode(".//Title").getValue();
        var profileDesc = displayprofileNode.selectSingleNode(".//Description").getValue();
        var profilePriority = displayprofileNode.selectSingleNode(".//Priority").getValue();
        
        cmpTxtDisplayProfileName.BlkJSX.setValue(profileName);
        cmpTxtDisplayProfileDesc.BlkJSX.setValue(profileDesc);
        //cmpTxtDisplayProfilePriority.BlkJSX.setValue(profilePriority);
        
        // genrate the displatProfileCanvas CDF
        var xslfile = PNameSpace.loadInclude("jsxuser:///JSXAPPS/IPVS_AdminUI/xsl/DisplayProfileCanvas.xsl","DisplayProfile","xsl");
        var xslDoc = jsx3.xml.XslDocument.wrap(xslfile);
        var displayProfileCanvas = xslDoc.transform(displayprofileNode);        
        cmpDisplayProfileSetting.onResponse("SUCCESS", displayProfileCanvas.toString());
      } // end processDisplayProfile
      
      if( DisplayProfileNID == "DEFAULT_DISPLAYPROFILE_NID" ){
        processDisplayProfile(DisplayProfileNID);
      }
      else {
        var reqDisplayProfileData = new ReqGetObjectNID(DisplayProfileNID, "DisplayProfile",
                                                {action:"FNCCALL", context:this, 
                                                  fnc:function(respCode, respVal) {
                                                        if(respCode == "SUCCESS" && respVal != null) {
                                                          processDisplayProfile(DisplayProfileNID);
                                                        } else {
                                                           var codeErrorMsg = CommonErrorCodeArr[respCode].errorDesc;
                                                           var ErrorMsg = "Error: Get DisplayProfile data request failed \n";
                                                           ErrorMsg += "Error description: " + codeErrorMsg + "\n";
                                                           ErrorMsg += "Error code: " + respCode;
                                                           alert(ErrorMsg);
                                                         }
                                                      }
                                                } 
                                             );
        reqDisplayProfileData.sendRequest();      
      }     
      
      topic_Publish(tDlg, "EditDisplayprofileDataState", DisplayProfileNID);
    } //  end cmpDlgEditDisplayProfileGetDisplayProfile
       
    //basic validation for name, priority 
    function cmpDlgEditDisplayProfileSetDisplayProfileValidate() {
      var profileName = cmpTxtDisplayProfileName.BlkJSX.getValue();
      var profilePriority = cmpTxtDisplayProfilePriority.BlkJSX.getValue();
      
      var chkProfileName = checkValue(profileName , [BlkJSX,"txtDisplayProfileName"] , "Please enter profile name");
      var chkProfilePriority = "";
      
      if(chkProfileName) {
        //chkProfilePriority = checkIntergerValue(profilePriority , [BlkJSX,"txtDisplayProfilePriority"] , "Please enter integer profile priority"); 
        chkProfilePriority = true;
        if(chkProfilePriority) {
          return true;
        } 
      }      
      return false;
    } // end cmpDlgEditDisplayProfileSetDisplayProfileValidate
     
    //fnc to set the deisplayProfile
    function cmpDlgEditDisplayProfileSetDisplayProfile(DisplayProfileNID) {
      if( cmpDlgEditDisplayProfileSetDisplayProfileValidate() != true ) return;
      
      var SetMsg = "Edit DisplayProfile";
      var SetMode = "Replace";
      if( DisplayProfileNID == "DEFAULT_DISPLAYPROFILE_NID" ) {
        SetMsg = "Add DisplayProfile";
        SetMode = "Add";
      }    

      var newCanvasNodeXML = cmpDisplayProfileSetting.getCanvasXML();
      var newCanvasNode = newCanvasNodeXML.selectSingleNode(".//Canvas");
      //Adding default Id to a Canvas Profile
      var defaultCanvasId = "Canvas_"+ getUUID();
      newCanvasNode.setAttribute("id",defaultCanvasId);
      var newLayoutId = "Layout_"+ getUUID();;
      var layoutNode = newCanvasNode.selectSingleNode(".//Layout");
      layoutNode.setAttribute("id",newLayoutId);
    
      var windowPropertiesNodeIter = newCanvasNode.selectNodeIterator(".//Window/Properties");
      while(windowPropertiesNodeIter.hasNext()) {
        var propertiesNode = windowPropertiesNodeIter.next();
        propertiesNode.setAttribute("layoutId",newLayoutId);
      }
      var displayPlayProfileNode = topic_GetNode(tCache, "//*[@NID='" + DisplayProfileNID + "']").cloneNode(true);
      var canvasName = cmpTxtDisplayProfileName.BlkJSX.getValue();
      var canvasDesc = cmpTxtDisplayProfileDesc.BlkJSX.getValue();
      //var canvasPriority = cmpTxtDisplayProfilePriority.BlkJSX.getValue();
      var canvasPriority = "1";
      displayPlayProfileNode.selectSingleNode(".//Title").setValue(canvasName);
      displayPlayProfileNode.selectSingleNode(".//Description").setValue(canvasDesc);
      displayPlayProfileNode.selectSingleNode(".//Priority").setValue(canvasPriority);
      var canvasParent = displayPlayProfileNode.selectSingleNode(".//Canvas").getParent();
      var canvasNode = displayPlayProfileNode.selectSingleNode(".//Canvas");
      canvasParent.replaceNode(newCanvasNode, canvasNode);
      
      
      if( SetMode == "Replace" ) {
        displayPlayProfileNode.setAttribute("NID", DisplayProfileNID);
      }
      else {
        displayPlayProfileNode.setAttribute("NID", "");
      }
            
      //delete empty group tag node
      deleteEmptyGroupTag(displayPlayProfileNode);      

      // do set request for User
      var reqSetDisplayProfileObject = new ReqSetObject(SetMode, "DisplayProfile", displayPlayProfileNode, 
                                                        {action:"FNCCALL", context:this, 
                                                           fnc:function(respCode, respVal) {
                                                                 if(respCode == "SUCCESS" && respVal != null) {
                                                                   //  publish to refresh the user list
                                                                   topic_Publish(tApp, "DisplayProfileDataUpdate", "ADD");
                                                                   dlgEditDisplayProfile_Destroy();
                                                                 } 
                                                                 else {
                                                                   try{
                                                                     var codeErrorMsg = CommonErrorCodeArr[respCode].errorDesc;
                                                                   } catch(e){
                                                                   }
                                                                   var ErrorMsg = "Error: " + SetMsg +" request failed \n";
                                                                   ErrorMsg += "Error description: " + codeErrorMsg + "\n";
                                                                   ErrorMsg += "Error code: " + respCode;
                                                                   alert(ErrorMsg);                                                           
                                                                 }
                                                               }
                                                        } 
                                                     );
                                                     
      reqSetDisplayProfileObject.sendRequest();

    } // end cmpDlgEditDisplayProfileSetDisplayProfile


       
} // end create

