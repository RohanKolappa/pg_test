if (window.com && com.ipvs.cmp){
  delete com.ipvs.api.SessionConfig;
}

jsx3.require("jsx3.xml.Document");

/*
 * Copyright (c) IP Video Systems Inc.
 * Use, modification, and distribution subject to terms of license.
 */

/**
 * Mixin interface that provides methods for accessing and manipulating a document in SessionConfig schema.
 * <p/>
 * This interface is modeled on the lines of the CDF interface provided/used by GI
 */
jsx3.Class.defineInterface("com.ipvs.api.SessionConfig", com.ipvs.api.SessionInfo,function(SessionConfig, SessionConfig_prototype) {

  /** @private @jsxobf-clobber */
  SessionConfig._LOG = jsx3.util.Logger.getLogger(SessionConfig.jsxclass.getName());
  
  /**
   * Gets the SessionConfig Name
   * @return {String}
   */
  SessionConfig_prototype.getName = function() {
    return this.selectSingleNode(".//Title").getValue();
  }
  /*
   * Sets the SessionConfig Name
   * @param strName {String}
   */
  SessionConfig_prototype.setName = function(strName) {
    this.selectSingleNode(".//Title").setValue(strName);
  }
  /**
   * Gets the SessionConfig Description
   * @return {String}
   */
  SessionConfig_prototype.getDescription = function() {
    return this.selectSingleNode(".//Description").getValue();
  }
  /*
   * Sets the SessionConfig Description
   * @param strText {String}
   */
  SessionConfig_prototype.setDescription = function(strText) {
    this.selectSingleNode(".//Description").setValue(strText);
  }
  /**
   * Gets the SessionConfig GroupsNode
   * @return {String}
   */
  SessionConfig_prototype.getGroupsNode = function() {
    return this.selectSingleNode(".//Groups");
  }
  /*
   * Sets/Replace the SessionConfig GroupsNode
   * @param Groups Node {Node}
   */
  SessionConfig_prototype.setGroupsNode = function(newGroupsNode) {
    var oldGroupsNode = this.selectSingleNode(".//Groups");
    this.replaceNode(newGroupsNode,oldGroupsNode);
  }  
  /*
   * Gets the Playback recVersion
   * @return recVersion option {String}
   */
  SessionConfig_prototype.getRecVersion = function() {
    return this.selectSingleNode(".//Settings/Playback").getAttribute("recVersion");
  }
  /*
   * Sets the Playback recVersion
   * @param recVersion(DEFAULT/ADHOC/LATEST) {String}
   */
  SessionConfig_prototype.setRecVersion = function(recVersion) {
    this.selectSingleNode(".//Settings/Playback").setAttribute("recVersion", recVersion);
  }  
  /*
   * Gets the Playback syncentityid
   * @return syncEntityId {String}
   */
  SessionConfig_prototype.getPlySyncEntityId = function() {
    return this.selectSingleNode(".//Settings/Playback").getAttribute("syncEntityId");
  }
  /*
   * Sets the Rec Playback syncentityid
   * @param syncEntityId {String}
   */
  SessionConfig_prototype.setPlySyncEntityId = function(syncEntityId) {
    this.selectSingleNode(".//Settings/Playback").setAttribute("syncEntityId", syncEntityId);
  }
  /*
   * Gets the Rec autoStart flag
   * @return bEnable {Boolean}
   */
  SessionConfig_prototype.getRecAutoStart = function() {
    return this.selectSingleNode(".//Settings/Record").getAttribute("autoStart");
  }
  /*
   * Sets the Rec autoStart flag
   * @param bEnable {Boolean}
   */
  SessionConfig_prototype.setRecAutoStart = function(bEnable) {
    this.selectSingleNode(".//Settings/Record").setAttribute("autoStart", bEnable);
  }      
  /*
   * Gets the PVR trimlength
   * @return trimLength {int}
   */
  SessionConfig_prototype.getPVRTrimLength = function() {
    return this.selectSingleNode(".//Settings/PVR").getAttribute("trimLength");
  }
  /*
   * Sets the PVR trimlength
   * @param trimLength {int}
   */
  SessionConfig_prototype.setPVRTrimLength = function(trimLength) {
    this.selectSingleNode(".//Settings/PVR").setAttribute("trimLength", trimLength);
    // update all existing SrcPort entities
    var iterSrcPortEntity = this.getEntityIterByType(com.ipvs.api.SessionInfo.ENTITYTYPE_SRCPORT);
    while( iterSrcPortEntity.hasNext() ) {
      var nodeEntity = iterSrcPortEntity.next();
      this._updateEntityNode(nodeEntity, (new Object()));
    }    
  }
  /*
   * Gets the PVR Enable flag
   * @return bEnable {Boolean}
   */
  SessionConfig_prototype.getPVREnable = function() {
    return this.selectSingleNode(".//Settings/PVR").getAttribute("enable");
  }
  /*
   * Sets the PVR Enable flag
   * @param bEnable {Boolean}
   */
  SessionConfig_prototype.setPVREnable = function(bEnable) {
    this.selectSingleNode(".//Settings/PVR").setAttribute("enable", bEnable);
    // update all existing SrcPort entities
    var iterSrcPortEntity = this.getEntityIterByType(com.ipvs.api.SessionInfo.ENTITYTYPE_SRCPORT);
    while( iterSrcPortEntity.hasNext() ) {
      var nodeEntity = iterSrcPortEntity.next();
      this._updateEntityNode(nodeEntity, (new Object()));
    }
  }
  /*
   * Sets the PVR Enable flag and the trinlLength
   * @param bEnable {Boolean}
   * @param trimLength {int}
   */
  SessionConfig_prototype.setPVREnable_PVRTrimLength = function(bEnable, trimLength) {
    this.selectSingleNode(".//Settings/PVR").setAttribute("enable", bEnable);
    this.selectSingleNode(".//Settings/PVR").setAttribute("trimLength", trimLength);
    // update all existing SrcPort entities
    var iterSrcPortEntity = this.getEntityIterByType(com.ipvs.api.SessionInfo.ENTITYTYPE_SRCPORT);
    while( iterSrcPortEntity.hasNext() ) {
      var nodeEntity = iterSrcPortEntity.next();
      this._updateEntityNode(nodeEntity, (new Object()));
    }
  }  
  /*
   * Adds a Tag record to the Groups
   * @param tagName {String}
   * @param tagValue {String}
   */
  SessionConfig_prototype.addGroupTag = function(tagName, tagValue) {
    var nodeGroups = this.selectSingleNode(".//Groups");
    var nodeGroupTag = nodeGroups.createNode(jsx3.xml.Entity.TYPEELEMENT, "Tag");
    nodeGroupTag.setAttribute("Name", tagName);
    nodeGroupTag.setAttribute("Value", tagValue);
    nodeGroups.appendChild(nodeGroupTag);
  }
  /*
   * Gets an Iterator over the Group tags
   * 
   * @return {jsx3.util.Iterator} group tag Nodes
   */
  SessionConfig_prototype.getGroupTagIter = function() {
    return this.selectNodeIterator(".//Groups/Tag");
  }     
        
   
  
  
  /*
   * Internal utility function to get new empty SessionConfig
   * @return {jsx3.xml.Entity} nodeSessionConfig
   */
  SessionConfig._getNewSessionConfig = function() {
    // get the template from the cache OR model/objectXml
    var nodeSessionConfig = com.ipvs.api.SessionInfo.prototype._getSessionObjectXML();
    
    // clean out NID,
    nodeSessionConfig.setAttribute("NID", "");
    // clean out all dirs
    var nodeDirList = nodeSessionConfig.selectSingleNode(".//DirList");
    nodeDirList.removeChildren();
    // clean out all grpfiles
    var nodeGrpFileList = nodeSessionConfig.selectSingleNode(".//GrpFileList");
    nodeGrpFileList.removeChildren();
    // clean out all entities
    var nodeEntityList = nodeSessionConfig.selectSingleNode(".//EntityList");
    nodeEntityList.removeChildren();
    // clean out all profiles
    var nodeProfileList = nodeSessionConfig.selectSingleNode(".//ProfileList");
    nodeProfileList.removeChildren();
    // clean out all auth
    //var nodeAuthList = nodeSessionConfig.selectSingleNode(".//AuthList");
    //nodeAuthList.removeChildren();
    // clean out all connections
    var nodeConnList = nodeSessionConfig.selectSingleNode(".//ConnectionList");
    nodeConnList.removeChildren();
    // clean out all connection segments
    var nodeConnSegList = nodeSessionConfig.selectSingleNode(".//ConnSegList");
    nodeConnSegList.removeChildren();
    // clean out all connection groups
    var nodeConnGrpList = nodeSessionConfig.selectSingleNode(".//ConnGrpList");
    nodeConnGrpList.removeChildren();

    return nodeSessionConfig;
  }
  
  /*
   * Internal utility function to get get a template SessionConfig
   * Looks in the cache first - if not found inits it from model/objectXml
   * @return {jsx3.xml.Entity} nodeSessionConfig
   */
//  SessionConfig._getSessionObjectXML = function() {
//    // get template from cache
//    var nodeSessionConfig = getObjectCacheDefaultNode("SessionConfig", "DEFAULT_SESSIONCONFIG_NID");
//    if( nodeSessionConfig == null ) {
//      setObjectCacheDefaultXML("SessionConfig", "JSXAPPS/IPVS-Common/model/objectXml/SessionConfig.xml", "DEFAULT_SESSIONCONFIG_NID");
//      nodeSessionConfig = getObjectCacheDefaultNode("SessionConfig", "DEFAULT_SESSIONCONFIG_NID");
//    }
    
//    return nodeSessionConfig;
//  }
  
});

/**
 * A subclass of <code>jsx3.xml.Document</code> that implements the SessionConfig interface. This class simply exposes the SessionConfig
 * convenience methods on an XML document.
 */
jsx3.Class.defineClass("com.ipvs.api.SessionConfig.Document", jsx3.xml.Document, [com.ipvs.api.SessionConfig], function(Document, Document_prototype) {
  /**
   * Creates a new XML document that represents an empty SessionConfig document. The XML source of the new document
   * is based on the SessionConfig.xml under model/ObjectXML
   *
   * @return {jsx3.xml.SessionConfig.Document} the newly created document.
   */
  Document.newDocument = function() {
    var nodeXML = com.ipvs.api.SessionConfig._getNewSessionConfig();
    var objXML = new Document(nodeXML);
    return objXML;
  };
  /**
   * @param objXML {jsx3.xml.Document}
   * @return {com.ipvs.api.SessionConfig.Document}
   */
  Document.wrap = function(objXML) {
    return new Document(objXML.getNativeDocument());
  };
  
  Document_prototype.cloneDocument = function() {
    return Document.wrap(this.jsxsuper());
  };

});