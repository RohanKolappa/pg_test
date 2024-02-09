
function cmpBlkButton_Create(winMediaList, btnDataInfo, tMediaList, selEventVar, blkBtnInfo, btnProperties) {

    var winMediaListBlkJSX = winMediaList.BlkJSX;
    var blkBtnName = blkBtnInfo.blkBtnName;
    var blkBtnContainer = blkBtnInfo.blkBtnContainer;
    var BtnType = btnProperties.BtnType;
    var BtnOnCSS = btnProperties.BtnOnCSS;
    var BtnOffCSS = btnProperties.BtnOffCSS;
    
    var orgBtnBlkJSX = getJsxByName([winMediaListBlkJSX, blkBtnName]);
    var newBtnBlkJSX = orgBtnBlkJSX.doClone(jsx3.app.Model.PERSISTEMBED);
    // attach it to the Room list
    var BtnContainerBlkJSX = getJsxByName([winMediaListBlkJSX, blkBtnContainer]);
    BtnContainerBlkJSX.adoptChild(newBtnBlkJSX, true);
    newBtnBlkJSX.setDisplay(jsx3.gui.Block.DISPLAYBLOCK, true);


    var cmpBlkBtn = new CmpButton([newBtnBlkJSX],
                                            [{ action: "TOPICEVENT", topic: tMediaList, eventvar: "btnTitle", eventval: null },
                                             { action: "TOPICEVENT", topic: tMediaList, eventvar: "roomCount", eventval: null}],
                                            { action: "FNCCALL",
                                              context: this,
                                              fnc: function() {                                             
                                                if (cmpBlkBtn.selectState == false) {
                                                    topic_Publish(tMediaList, "ClearSelectedBtn", cmpBlkBtn.BtnType);
                                                    cmpBlkBtn.BlkJSX.setClassName(BtnOnCSS).repaint();
                                                    cmpBlkBtn.selectState = true;
                                                  }
                                                  else {
                                                    cmpBlkBtn.BlkJSX.setClassName(BtnOffCSS).repaint();
                                                    cmpBlkBtn.selectState = false;
                                                  }
                                                topic_Publish(tMediaList, selEventVar, cmpBlkBtn.btnDataInfo);
                                              }
                                            }
                                          );
    cmpBlkBtn.cmpButtonRepaint = cmpBlkBtn.repaint;
    cmpBlkBtn.BtnType = BtnType;
    cmpBlkBtn.repaint = function(EventVar, EventVal) {
      EventVar && (EventVar == btnTitle ? this.btnDataInfo.btnTitle = EventVal : this.btnDataInfo.roomCount = EventVal);
      this.Txt = this.btnDataInfo.btnTitle + (this.btnDataInfo.roomCount > 0 ? (' ('+this.btnDataInfo.roomCount+')') :'');
      if( this.Txt != null ) {
        this.BlkJSX.setText(this.Txt, true);
      }
      this.cmpButtonRepaint.apply(this, arguments);
    }
    
    cmpBlkBtn.cmpButtonDestroy = cmpBlkBtn.destroy;
    cmpBlkBtn.destroy = function (cmpBlkBtn) {
        // get a handle to the current BlkJSX
        var BtnBlkJSX = this.BlkJSX;
        var BtnContainerBlkJSX = BtnBlkJSX.getParent();

        // finally destroy the DOM element
       BtnContainerBlkJSX.removeChild(BtnBlkJSX);
       BtnContainerBlkJSX.repaint();
        
        this.cmpButtonDestroy.apply(this, arguments);
    }
   
    cmpBlkBtn.selectState = false;
    cmpBlkBtn.btnDataInfo = btnDataInfo;
    
    cmpBlkBtn.getData = function() {
      cmpBlkBtn.repaint();
    }

    // init and show it - in its own context
    jsx3.sleep((function() {
        this.init();
        this.show();
        this.getData();

    }
    ), null, cmpBlkBtn);

    // return a handle to it
    return cmpBlkBtn;

}