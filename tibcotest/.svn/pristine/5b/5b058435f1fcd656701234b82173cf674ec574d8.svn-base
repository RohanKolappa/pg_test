/*Common code for show/hide spinner*/

function cmpMask_show(blkHide) {
  var blkShow = topic_Get(tApp, "showMask");
  topic_Set(tApp, "showMask", blkHide);
  var blkName=PNameSpace.getJSXByName(blkHide);
  var blkSplash= PNameSpace.getJSXByName("blkSpinner");
  var MASKBGCOLOR="#0066CC";
  blkName.showMask("");
  var blkDim = blkName.getAbsolutePosition();
  var bWidth = blkDim.W;
  var bHeight = blkDim.H;
  var bLeft = blkDim.L;
  var bTop = blkDim.T;
  blkName.getLastChild().setCursor("default",true);
  blkName.getLastChild().setBackgroundColor(MASKBGCOLOR,true);
  blkSplash.setVisibility(jsx3.gui.Block.VISIBILITYVISIBLE, true);
  bLeft = bLeft + (bWidth/4);
  bTop = bTop + (bHeight/2);
  blkSplash.setLeft(bLeft, true);
  blkSplash.setTop(bTop, true);
}

function cmpMask_hide() {
  var blkShow = topic_Get(tApp, "showMask");
  if((blkShow == "NONE") || (blkShow == null)) {
    return;
  }
  var blkName=PNameSpace.getJSXByName(blkShow);
  var blkSplash= PNameSpace.getJSXByName("blkSpinner");
  blkName.hideMask();
  blkSplash.setVisibility(jsx3.gui.Block.VISIBILITYHIDDEN, true);
  topic_Set(tApp, "showMask", "NONE");
}