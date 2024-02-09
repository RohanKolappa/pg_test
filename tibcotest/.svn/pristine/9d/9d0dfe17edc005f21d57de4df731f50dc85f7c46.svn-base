  // a topic / listner for messaging in it
  var tMediaController = "MediaController_" + jsx3.xml.CDF.getKey(); ;
  var tMediaControllerDoc = null;
  var eventMediaController = null;
  // the top level block
  var winMediaController = null;

  //load medialistController from WEB client
  function cmpWebController_Create() {  
    // create a topic / listner for messaging in it
    tMediaControllerDoc = topic_Create(tMediaController);
    eventMediaController = new EventListner(tMediaController);

    // save the callers topic/name in a gloabl static var
    // this will be used to target events later
    tCaller = tApp;
    var cmpBlkMediaController = new CmpContainer("blkMediaController", [], null, 
                                                  { action: "FNCCALL",
                                                    context: null,
                                                    fnc: function() {
                                                      cmpBlkMediaController_Destory();
                                                    }
                                                  });
    cmpBlkMediaController_Destory = function() {
      cmpBlkMediaController.BlkJSX.removeChildren();
      cmpBlkMediaController.BlkJSX = null;
      // destroy any eventlistners
      eventMediaController.destroy();
      eventMediaController = null;
      // destroy the local topic
      tMediaControllerDoc = null;
      topic_Destroy(tMediaController);
    }
    cmpBlkMediaController.BlkJSX.load("components/mediaController.xml");
    log.info("cmpBlkWebController::loaded mediaController.xml");
    var lytCtrlMain = getJsxByName("lytMain");
    //hide the Header for WEB client
    lytCtrlMain.setRows("0,42,*");
    cmpBlkMediaController.eventMediaController = eventMediaController;
    cmpBlkMediaController.tMediaController = tMediaController;  
    cmpBlkMediaController.window = window;

    jsx3.sleep(function() {
                            cmpMediaControllerSubArr_Create.call(this);
                          }
               , null, cmpBlkMediaController); 
    return cmpBlkMediaController
  }



