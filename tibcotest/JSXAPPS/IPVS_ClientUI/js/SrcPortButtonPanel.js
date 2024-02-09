
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

jsx3.Class.defineClass('com.ipvs.gui.SrcPortButtonPanel', com.ipvs.gui.BlockButtonPanel, null, function (SrcPortButtonPanel, SrcPortButtonPanel_prototype) {
  var Event = jsx3.gui.Event;
  var Interactive = jsx3.gui.Interactive;
  var Form = jsx3.gui.Form;

  // create Logger for this class
  SrcPortButtonPanel._LOG = jsx3.util.Logger.getLogger(SrcPortButtonPanel.jsxclass.getName());

  /**
  * {String} Buttons Fields
  */
  SrcPortButtonPanel.FORM_FIELD_VIEW = new Object;

  // Port Presence 
  SrcPortButtonPanel.FORM_FIELD_VIEW['Presence'] = { ATTRIBUTES: { VAL: null,
                                                                   IMG: 'jsxdevpresence',
                                                                   LBL: 'jsxpresence'
                                                                 },
                                                     VALUES: { available: { LBL: 'available',
                                                                            IMG: 'jsxaddin://user!ipvsCommon/images/icons/icon_greenBall.png'
                                                                          },
                                                               unavailable: { LBL: 'unavailable',
                                                                              IMG: 'jsxaddin://user!ipvsCommon/images/icons/icon_grayBall.png'
                                                                            },
                                                               DEFAULT: { LBL: null,
                                                                          IMG: 'jsxaddin://user!ipvsCommon/images/icons/icon_lightgrayBall.png'
                                                                        } 
                                                             }
                                                   };

  // Port Conn State     
  SrcPortButtonPanel.FORM_FIELD_VIEW["State"] = { ATTRIBUTES: { VAL: null,
                                                                IMG: 'jsxbtnsessionstatus',
                                                                LBL: null
                                                              },
                                                  VALUES: { DEFAULT: { LBL: null,
                                                                       IMG: 'jsxaddin://user!ipvsCommon/images/icons/icon_network_off.png'
                                                                     }
                                                          }
                                                };


  // Port Stream Type
  SrcPortButtonPanel.FORM_FIELD_VIEW["Type"] = { ATTRIBUTES: { VAL: null,
                                                               IMG: 'jsxdevtype',
                                                               LBL: 'jsxdevStreamType'
                                                             },
                                                 VALUES: { V2D: { LBL: 'V2D',
                                                                  IMG: 'jsxaddin://user!ipvsCommon/images/icons/icon_v2d.png'
                                                                },
                                                           MPEGTS: { LBL: 'MPEGTS',
                                                                     IMG: 'jsxaddin://user!ipvsCommon/images/icons/icon_mpegts.png'
                                                                   },
                                                           UDP: { LBL: 'UDP',
                                                                  IMG: 'jsxaddin://user!ipvsCommon/images/icons/icon_udp.png'
                                                                },
                                                           RTP: { LBL: 'RTP',
                                                                  IMG: 'jsxaddin://user!ipvsCommon/images/icons/icon_rtp.png'
                                                                },
                                                           VNC: { LBL: 'VNC',
                                                                  IMG: 'jsxaddin://user!ipvsCommon/images/icons/icon_vnc.png'
                                                                },
                                                           MULTI: { LBL: 'MULTI',
                                                                    IMG: 'jsxaddin://user!ipvsCommon/images/icons/icon_multi.png'
                                                                  },
                                                           DEFAULT: { LBL: 'V2D',
                                                                      IMG: 'jsxaddin://user!ipvsCommon/images/icons/icon_v2d.png'
                                                                    }
                                                         }
                                               };

  /** {String} @private */
  SrcPortButtonPanel_prototype.jsxTimerID = null;

  /**
  * The instance initializer.
  * Chain to super
  */
  SrcPortButtonPanel_prototype.init = function () {
    this.jsxsuper.apply(arguments);
  };

  /**
  * Stores the current timerid.
  * @timerID {String}
  */
  SrcPortButtonPanel_prototype.setTimerId = function (timerID) {
    this.jsxTimerID = timerID;
  }

  /**
  * Returns the current timerid.
  * @return {String}
  */
  SrcPortButtonPanel_prototype.getTimerId = function () {
    return this.jsxTimerID;
  }

  /**
  * Method to return a handle to the Form Field View Array
  */
  SrcPortButtonPanel_prototype.getFormFieldViewArr = function () {
    return SrcPortButtonPanel.FORM_FIELD_VIEW;
  };

  /**
  *It will start/stop the timer to reload thumbnail images of ports
  */
  SrcPortButtonPanel_prototype.setDisplay = function (DISPLAY, bUpdateView) {
    this.jsxsuper();
    var timerId = this.getTimerId();
    if (DISPLAY != jsx3.gui.Block.DISPLAYNONE) {
      if (jsx3.util.strEmpty(timerId)) {
        timerId = setInterval(jsx3.$F(function () { this.handleThumbnails(); }).bind(this), 4000);
        this.setTimerId(timerId);
      }
    }
    else {
      if (!jsx3.util.strEmpty(timerId)) {
        clearInterval(timerId);
        this.setTimerId(null);
      }
    }
  }

  SrcPortButtonPanel_prototype.handleThumbnails = function () {
    var renderedHtml = this.getRendered();
    var recIter = this.getIterableRecords()
    while (recIter.hasNext()) {
      var rec = recIter.next();
      if (rec.getAttribute('jsxdevstate') == "Ready") {
        //append timestamp to avoid image caching
        renderedHtml.ownerDocument.getElementById(rec.getAttribute('jsxid')).src = rec.getAttribute('jsxthumbnailURL') + "&newImage=" + new Date().getTime();
      }
    }
  }

  /**
  * The nested XML Template for each Button in this Button Panel
  * @return {String}
  */
  SrcPortButtonPanel_prototype.getButtonTemplateXML = function () {
    var returnVal = ['',
      '<var id="buttonid">this.getId() + \'_\' + $$target.getAttribute(\'jsxid\')</var>',
      '<var id="buttonimgclass">this.getCurrentClass(false, false, $$target)</var>',
      '<var id="imgThumbnail">$$target.getAttribute(\'jsxthumbnailURL\')</var>',
      '<var id="imgthumbnailid">$$target.getAttribute(\'jsxid\')</var>',
      '<inlinebox u:id="button" id="{buttonid}" style="position:relative;float:left;margin:20px;cursor:pointer; font-family:Verdana, Geneva, sans-serif " onclick="{onClick}" onmouseover="{onMouseOver}" onmouseout="{onMouseOut}" onmousedown="{onMouseDown}" onmouseup="{onMouseUp}" class="{buttonimgclass}">',
      ' <div style="position:relative;left:0px;top:0px;width:', this.getButtonWidth(), 'px;height:', this.getButtonHeight() - 50, 'px;">',
      '   <span u:id="thumb" style="position:absolute;width:100%;height:100%;left:0px;top:0px;">',
      '     <img id="{imgthumbnailid}" src="{imgThumbnail}" width="100%" height="100%"/>',
      '   </span>',
      ' </div>',
      '  <div style="position:relative;left:0px;top:0px;width:', this.getButtonWidth(), 'px;height:50', 'px;overflow:hidden;">',
    // button fields
           this.getButtonFieldsTemplateXML(),
      '  </div>',
      '</inlinebox>'].join("");
    this._LOG.info("BlockButtonPanel.getButtonTemplateXML::::::(returnVal):" + returnVal.toString());
    return returnVal;
  }

  /**
  * The nested XML Template for the Form Fields on each Button in this Button Panel
  *
  * @return {String}
  */
  SrcPortButtonPanel_prototype.getButtonFieldsTemplateXML = function () {
    var returnVal = ['',
                                                                      '<var id="buttondevpresence">this.getCurrentFieldImgURL("Presence", $$target)</var>',
                                                                      '<var id="buttondevtitle">this.getCurrentLabelText($$target)</var>',
                                                                      '<var id="buttonsessionstatus">this.getCurrentFieldImgURL("State", $$target)</var>',
                                                                      '<var id="buttondevtype">this.getCurrentFieldLblTxt("Type", $$target)</var>',
                                                                      '<div u:id="buttonfields" style="position:absolute;left:0px;top:0px;width:100%;height:100%;overflow:hidden;">',
                                                                        // title
                                                                      '  <span u:id="title" style="position:absolute;width:', this.getLabelWidth(), 'px;left:', this.getLabelLeftPosition(), 'px;top:', this.getLabelTopPosition(), 'px;font-size:', this.getLabelFontSize(), 'px;overflow:hidden;font-weight:100;white-space:nowrap;text-align:left;color:#fff;">',
                                                                      '    <text>{buttondevtitle}</text>',
                                                                      '  </span>',
// state
                                                                      '  <span u:id="state" style="position:absolute;width:',this.getButtonWidth(),'px;height:5px;left:0px;top:0px;overflow:hidden;">',
                                                                      '    <img src="{buttonsessionstatus}" width="100%" height="100%"/>',
                                                                      '  </span>',
// type
                                                                      '  <span u:id="type" style="position:absolute;width:50px;height:16px;left:0px;top:25px;text-align:left;overflow:hidden;font-size:10px;color:#999;">',
                                                                      '    <text>{buttondevtype}</text>',
                                                                      '  </span>',
                                                                      '</div>'].join("");
SrcPortButtonPanel._LOG.debug(" SrcPortButtonPanel.getButtonFieldsTemplateXML::(returnVal):" + returnVal.toString());
    return returnVal;
  }

});
