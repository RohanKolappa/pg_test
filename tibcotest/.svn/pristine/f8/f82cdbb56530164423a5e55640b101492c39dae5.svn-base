//testAopFnc defined in logger.xml,the following are AOP function samples:
//com.ipvs.gui.PortButtonPanel
testPortButtonPanelAroundFnc = function  (aop, strURL, intTimeout) {
  intTimeout = 0;
  log.warn(" Calling testPortButtonPanelAroundFnc around  PortButtonPanel.getButtonFieldsTemplateXML()");
  var t1 = new Date().getTime();
  var rtVal = aop.proceed(strURL, intTimeout);
  var tTotal = new Date().getTime() - t1;
  log.warn(" called on " + this + " with URL " + strURL + " took " + tTotal + " ms and returned " + rtVal + ".");
  //alert("AOPLogger-around@" + t1);
  /*
  AOP can change a function return dynamically, i.e. change <img src="{buttondevpresence}" width="32" height="32"/>
  width and height from initial 16 to 32 will effect the PortButtonPanel view finally.
  */
  rtVal = '<var id="buttondevpresence">this.getCurrentFieldImgURL("Presence", $$target)</var>' +
                '<var id="buttondevtitle">this.getCurrentLabelText($$target)</var>' +
                '<var id="buttonsessionstatus">this.getCurrentFieldImgURL("State", $$target)</var>' +
                '<var id="buttondevtype">this.getCurrentFieldImgURL("Type", $$target)</var>' +
                '<div u:id="buttonfields" style="position:absolute;left:0px;top:0px;width:100%;height:100%;overflow:hidden;">' +
                '  <span u:id="presence" style="position:absolute;width:16px;height:16px;left:2px;top:3px;overflow:hidden;">' +
                '<img src="{buttondevpresence}" width="32" height="32"/>  </span>' +
                '<span u:id="title" style="position:absolute;width:106px;left:21px;top:3px;font-size:12px;overflow:hidden;font-weight:normal;white-space:nowrap;text-overflow:ellipsis;">    <text>{buttondevtitle}</text>  </span>  <span u:id="state" style="position:absolute;width:16px;height:16px;left:3px;top:22px;overflow:hidden;">    <img src="{buttonsessionstatus}" width="16" height="16"/>  </span>  <span u:id="type" style="position:absolute;width:35px;height:16px;left:22px;top:22px;overflow:hidden;">' +
                '<img src="{buttondevtype}" width="35" height="16"/>  </span></div>'
  return rtVal
}
  
testPortButtonPanelBeforeFnc = function (aop, strURL, intTimeout) {
  intTimeout = 0;
  log.warn(" Calling testPortButtonPanelBeforeFnc before PortButtonPanel.getButtonFieldsTemplateXML()");
}
testPortButtonPanelAfterFnc = function	(aop, strURL, intTimeout) {
  intTimeout = 0;
  log.warn(" Calling testPortButtonPanelAfterFnc after PortButtonPanel.getButtonFieldsTemplateXML()");
}


 //com.ipvs.gui.BlockButtonPanel
testBlockButtonPanelBeforeFnc = function(aop, strURL, intTimeout) {
  intTimeout = 0;
  log.warn(" Calling testBlockButtonPanelBeforeFnc before BlockButtonPanel.getButtonFieldsTemplateXML()");
}

testBlockButtonPanelAfterFnc = function (aop, strURL, intTimeout) {
  intTimeout = 0;
  log.warn(" Calling testBlockButtonPanelAfterFnc after BlockButtonPanel.getButtonFieldsTemplateXML()");
}

testBlockButtonPanelAroundFnc = function	 (aop, strURL, intTimeout) {
  intTimeout = 0;
  log.warn(" Calling testBlockButtonPanelAroundFnc around BlockButtonPanel.getButtonFieldsTemplateXML()");
}
	

