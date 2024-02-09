
jsx3.require("com.ipvs.gui.BlockButtonPanel");

/**
 * An object-oriented interface onto a Block containg a left-right,top-down sequential list of Port Buttons
 * Each button represents a record in the attached CDF document and is decorated based on the following attributes in that record
 * <ul>
 * <li>Presence : jsxdevpresence</li>
 * <li>Title    : jsxtext</li>
 * <li>State    : jsxsessionstatus</li>
 * <li>Type     : jsxdevtype</li>
 * </ul>
 */

jsx3.Class.defineClass('com.ipvs.gui.PortButtonPanel', com.ipvs.gui.BlockButtonPanel, null, function(PortButtonPanel, PortButtonPanel_prototype) {
  var Event = jsx3.gui.Event;
  var Interactive = jsx3.gui.Interactive;
  var Form = jsx3.gui.Form;

  // create Logger for this class
  PortButtonPanel._LOG = jsx3.util.Logger.getLogger(PortButtonPanel.jsxclass.getName());
  /**
   * {String} Buttons Fields
   */
  PortButtonPanel.FORM_FIELD_VIEW = new Object;

  // Port Presence 
  PortButtonPanel.FORM_FIELD_VIEW['Presence'] = {ATTRIBUTES : {VAL : null,
                                                               IMG : 'jsxdevpresence',
                                                               LBL : 'jsxpresence'}, 
                                                     VALUES : {available  :{LBL:'available',
                                                                            IMG:'jsxaddin://user!ipvsCommon/images/icons/icon_greenBall.png'},
                                                               unavailable:{LBL:'unavailable',
                                                                            IMG:'jsxaddin://user!ipvsCommon/images/icons/icon_grayBall.png'},
                                                               DEFAULT    :{LBL:null,
                                                                            IMG:'jsxaddin://user!ipvsCommon/images/icons/icon_lightgrayBall.png'}}};

  // Port Conn State     
  PortButtonPanel.FORM_FIELD_VIEW["State"] = {ATTRIBUTES : {VAL : null,
                                                             IMG : 'jsxsessionstatus',
                                                             LBL : null}, 
                                                   VALUES : {DEFAULT    :{LBL:null,
                                                                          IMG:'jsxaddin://user!ipvsCommon/images/icons/icon_network_off.png'}}};
                                                             
    
  // Port Stream Type
  PortButtonPanel.FORM_FIELD_VIEW["Type"] = {ATTRIBUTES : {VAL : null,
                                                           IMG : 'jsxdevtype',
                                                           LBL : 'jsxdevStreamType'}, 
                                                 VALUES : {V2D    :{LBL:'V2D',
                                                                    IMG:'jsxaddin://user!ipvsCommon/images/icons/icon_v2d.png'},
                                                           MPEGTS :{LBL:'MPEGTS',
                                                                    IMG:'jsxaddin://user!ipvsCommon/images/icons/icon_mpegts.png'},
                                                           UDP    :{LBL:'UDP',
                                                                    IMG:'jsxaddin://user!ipvsCommon/images/icons/icon_udp.png'},
                                                           RTP    :{LBL:'RTP',
                                                                    IMG:'jsxaddin://user!ipvsCommon/images/icons/icon_rtp.png'},
                                                           VNC    :{LBL:'VNC',
                                                                    IMG:'jsxaddin://user!ipvsCommon/images/icons/icon_vnc.png'},
                                                           MULTI  :{LBL:'MULTI',
                                                                    IMG:'jsxaddin://user!ipvsCommon/images/icons/icon_multi.png'},
                                                           DEFAULT:{LBL:'V2D',
                                                                    IMG:'jsxaddin://user!ipvsCommon/images/icons/icon_v2d.png'}}};
   
  /**
   * The instance initializer.
   * Chain to super
   */
  PortButtonPanel_prototype.init = function() {
    this.jsxsuper.apply(arguments);
  };
  
  /**
   * Method to return a handle to the Form Field View Array
   */
  PortButtonPanel_prototype.getFormFieldViewArr = function() {
    return PortButtonPanel.FORM_FIELD_VIEW;
  };

  /**
   * The nested XML Template for the Form Fields on each Button in this Button Panel
   *
   * @return {String}
   */
  PortButtonPanel_prototype.getButtonFieldsTemplateXML = function() {
                                                                  var returnVal = ['',
                                                                                   '<var id="buttondevpresence">this.getCurrentFieldImgURL("Presence", $$target)</var>',
                                                                                   '<var id="buttondevtitle">this.getCurrentLabelText($$target)</var>',
                                                                                   '<var id="buttonsessionstatus">this.getCurrentFieldImgURL("State", $$target)</var>',
                                                                                   '<var id="buttondevtype">this.getCurrentFieldImgURL("Type", $$target)</var>',
                                                                                   '<div u:id="buttonfields" style="position:absolute;left:0px;top:0px;width:100%;height:100%;overflow:hidden;">',
                                                                                        // presence
                                                                                    ' <span u:id="presence" style="position:absolute;width:16px;height:16px;left:2px;top:3px;overflow:hidden;">',
                                                                                    '    <img src="{buttondevpresence}" width="16" height="16"/>',
                                                                                    '  </span>',
                                                                                        // title
                                                                                    '  <span u:id="title" style="position:absolute;width:',this.getLabelWidth(),'px;left:',this.getLabelLeftPosition(),'px;top:',this.getLabelTopPosition(),'px;font-size:',this.getLabelFontSize(),'px;overflow:hidden;font-weight:normal;white-space:nowrap;">',
                                                                                    '    <text>{buttondevtitle}</text>',
                                                                                    '  </span>',
                                                                                        // state
                                                                                    '  <span u:id="state" style="position:absolute;width:16px;height:16px;left:3px;top:22px;overflow:hidden;">',
                                                                                    '    <img src="{buttonsessionstatus}" width="16" height="16"/>',
                                                                                    '  </span>',
                                                                                        // type
                                                                                    '  <span u:id="type" style="position:absolute;width:35px;height:16px;left:22px;top:22px;overflow:hidden;">',
                                                                                    '    <img src="{buttondevtype}" width="35" height="16"/>',
                                                                                    '  </span>',
                                                                                    '</div>'].join("");
                                                                    PortButtonPanel._LOG.debug(" PortButtonPanel.getButtonFieldsTemplateXML::(returnVal):" + returnVal.toString());
                                                                    return returnVal;
                                                         }

});
