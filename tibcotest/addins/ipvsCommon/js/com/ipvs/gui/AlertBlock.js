/*
* Copyright (c) IP Video Systems Inc.
* Use, modification, and distribution subject to terms of license.
*/

/**
* AlertBlock class.
* <p/>
*/
jsx3.require("jsx3.gui.Alerts", "jsx3.gui.Block");
jsx3.Class.defineClass("com.ipvs.gui.AlertBlock", jsx3.gui.Block, [jsx3.gui.Alerts], function (AlertBlock, AlertBlock_prototype) {

  /**
  * Implements necessary method for the <code>Alerts</code> interface.
  * @return {jsx3.gui.Block} this object.
  */
  AlertBlock_prototype.getAlertsParent = function () {
    return this;
  };

});