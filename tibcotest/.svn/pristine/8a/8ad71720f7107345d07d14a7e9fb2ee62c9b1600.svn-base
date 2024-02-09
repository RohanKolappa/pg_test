// Function to Create Streaming Options Block .
function cmpBlkHLS_Create() {

    var cmpChkHLS = new CmpCheckBox(["blkHLS", "chkEnableHLS"], null,
                                            { action: "FNCCALL", context: null,
                                                fnc: function () {

                                                }
                                            });

    var cmpBtnSetHLSConfig = new CmpButton(["blkHLS", "btnSetHLS"], null,
                                             { action: "FNCCALL", context: null,
                                                 fnc: function () {
                                                     cmpSetHLSConfig();
                                                 }
                                             });



    var cmpHttpLiveStreaming = new CmpContainer("blkHLS",
                                          [{ cmp: cmpChkHLS },
                                           { cmp: cmpBtnSetHLSConfig}]);

   eventApp.subscribe("SystemAdminMenuSelection", "HLS", this,
                      function () {
                          cmpGetHLSConfig();
                      }
                    );

// function to get HLS config
function cmpGetHLSConfig() {
    var reqGetSystemConfig = new ReqSessionGetRequest("GetSystemConfig",
                                                      { action: "FNCCALL", context: this,
                                                          fnc: function (respCode, respVal) {
                                                              if (respCode == "SUCCESS" && respVal != null) {
                                                                  var hlsConfigNode = respVal;
                                                                  cmpGetHLS(hlsConfigNode);
                                                              }
                                                              else {
                                                                  log.error("GetServiceConfig request failed:");
                                                              }
                                                          }
                                                      });
 reqGetSystemConfig.ServiceName = "SessionService";
 var reqParams = new Object;
 reqParams["ObjectList_WhereConfig"] = "WriterOptions";
 reqGetSystemConfig.reqParams = reqParams;
  // send the request
 reqGetSystemConfig.sendRequest(this);
} //end cmpGetHLSConfig


// function to set HLS config
function cmpSetHLSConfig() {
    var reqSetDevice = new ReqSessionGetRequest("SetWriterOptions",
                                              { action: "FNCCALL", context: null,
                                                  fnc: function (respCode, respVal) {
                                                      if (respCode == "SUCCESS" && respVal != null) {
                                                          var enableHLS = cmpChkHLS.BlkJSX.getChecked() == jsx3.gui.CheckBox.CHECKED ? "true" : "false";
                                                          if (enableHLS == 'true')
                                                              cmpChkHLS.BlkJSX.setChecked(1);
                                                           else
                                                              cmpChkHLS.BlkJSX.setChecked(0);
                                                      } else {
                                                          log.error("SetWriterOptions request failed:");
                                                      }
                                                  }
                                              });

    reqSetDevice.ServiceName = "SessionService";

    var reqParams = new Object;
    reqParams["ObjectList_Enable"] = cmpChkHLS.BlkJSX.getChecked() == jsx3.gui.CheckBox.CHECKED ? true : false;
    reqSetDevice.reqParams = reqParams;

    // send the request
    reqSetDevice.sendRequest(this);  
} //end cmpSetHLSConfig


// function to Set the HLS parameters in the Form
function cmpGetHLS(hlsConfigNode) {
    var cdfID = hlsConfigNode.selectSingleNode(".//Device").getAttribute("jid");
    var hlsNode = hlsConfigNode.selectSingleNode(".//Device/WriterOptions");
    var hlsMode = hlsNode.getAttribute("enableHLS") == "true" ? jsx3.gui.CheckBox.CHECKED : jsx3.gui.CheckBox.UNCHECKED;
    cmpChkHLS.BlkJSX.setChecked(hlsMode);
} //end cmpGetHLS

   return cmpHttpLiveStreaming;
}