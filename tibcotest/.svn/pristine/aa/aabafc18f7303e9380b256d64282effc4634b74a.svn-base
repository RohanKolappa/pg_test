/*
* Copyright (c) IP Video Systems Inc.
* Use, modification, and distribution subject to terms of license.
*/

/**
* Version class reads version.xml from xml folder and updates label
* <p/>
* This class uses jsx3.gui.Block.
*/
jsx3.require("jsx3.gui.Block");

jsx3.Class.defineClass("com.ipvs.gui.authtool.Version", jsx3.gui.Block, null, function(Version, Version_prototype) {
  
  /** @private @jsxobf-clobber */
  Version_prototype._LOG = jsx3.util.Logger.getLogger(Version.jsxclass.getName());
  
  Version_prototype.init = function(objParent, objServer) {
    this.lblVersion = this.getDescendantOfName("lblVersion");
    //Read version file and display in label
    var versionData = "0.0.r0.b0";
    var nodeVersionData = objServer.getServer().loadInclude("JSXAPPS/IPVS_AuthTool/xml/version.xml", "Version_xml", "xml", true);
    var nodeVersion = nodeVersionData.selectSingleNode(".//version");
    if( nodeVersion != null ) {
      versionData = nodeVersion.getValue();
    }    
    this.lblVersion.setText(versionData, true);
  }

   /**
   * Called during deserialization of this object. This method provides a hook for initializing
   * an object during deserialization since init() is not called. Called after this object has been instantiated and
   * after its fields and children have been assembled.This method is called before this object is attached to the
   * DOM, therefore &lt;code&gt;getParent()&lt;/code&gt;, &lt;code&gt;getServer()&lt;/code&gt;, &lt;code&gt;getXML()&lt;/code&gt;, etc. return &lt;code&gt;null&lt;/code&gt;.
   * @param objParent {jsx3.app.Model} the parent of this object once it is attached to the DOM.
   * @param objServer {jsx3.app.Server} the server that this DOM object will attach to.
   * @protected
   */
  Version_prototype.onAfterAssemble = function(objParent, objServer) {
    this.init(objParent, objServer);
  };

});