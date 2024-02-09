/*
 * Copyright (c) IP Video Systems Inc.
 * Use, modification, and distribution subject to terms of license.
 */

/**
 * Mixin interface that provides methods for accessing and manipulating a document in SessionInfo schema.
 * <p/>
 * This interface is modeled on the lines of the CDF interface provided/used by GI
 */
jsx3.Class.defineInterface("com.ipvs.api.SessionInfo", null, function(SessionInfo, SessionInfo_prototype) {

  /** @private @jsxobf-clobber */
  SessionInfo._LOG = jsx3.util.Logger.getLogger(SessionInfo.jsxclass.getName());

  /**
  * {String}
  * @final @jsxobf-final
  */
  SessionInfo.ELEM_ROOT = '<SessionInfo id="" name=""/>';

  SessionInfo.DIR_IDPREFIX = "Dir_";
  SessionInfo.GRPFILE_IDPREFIX = "GrpFile_";
  SessionInfo.ENTITY_IDPREFIX = "Entity_";
  SessionInfo.INSTANCE_IDPREFIX = "Instance_";
  SessionInfo.CONNGRP_IDPREFIX = "ConnGrp_";
  SessionInfo.CONN_IDPREFIX = "Conn_";

  SessionInfo.DIRID_DEFAULTREC = "Dir_DefaultRec";
  SessionInfo.GRPFILEID_DEFAULTREC = "GrpFile_DefaultRec";

  SessionInfo.GRPFILETYPE_SYNCREC = "SyncRec";
  SessionInfo.GRPFILETYPE_SYNCPLY = "SyncPly";

  SessionInfo.ENTITYTYPE_USER = "User";
  SessionInfo.ENTITYTYPE_SRCPORT = "SrcPort";
  SessionInfo.ENTITYTYPE_DSTPORT = "DstPort";
  SessionInfo.ENTITYTYPE_RELAYPORT = "RelayPort";
  SessionInfo.ENTITYTYPE_STOREPORT = "StorePort";
  SessionInfo.ENTITYTYPE_PLYFILE = "PlyFile";
  SessionInfo.ENTITYTYPE_RECFILE = "RecFile";
  SessionInfo.ENTITYTYPE_PVRFILE = "PVRFile";

  SessionInfo.ENTITYSTREAMTYPE_V2D = "V2D";
  SessionInfo.ENTITYSTREAMTYPE_RTP = "RTP";
  SessionInfo.ENTITYSTREAMTYPE_UDP = "UDP";
  SessionInfo.ENTITYSTREAMTYPE_MPEGTS = "MPEGTS";
  SessionInfo.ENTITYSTREAMTYPE_MULTI = "MULTI";

  //STREAM is deprecated not in use new values View and Playback
  SessionInfo.CONNTYPE_STREAM = "STREAM";

  SessionInfo.CONNTYPE_VIEW = "VIEW";
  SessionInfo.CONNTYPE_PLAYBACK = "PLAYBACK";
  SessionInfo.CONNTYPE_RECORD = "RECORD";
  SessionInfo.CONNTYPE_PVR = "PVR";
  SessionInfo.CONNTYPE_PARTIAL = "PARTIAL";

  SessionInfo.SESSIONTYPE_LIVE = "LIVE";
  SessionInfo.SESSIONTYPE_PLAYBACK = "PLAYBACK";
  SessionInfo.SESSIONTYPE_MIXED = "MIXED";

  /*
  * GSets the SessionInfo date created
  * @param strName {String}
  */
  SessionInfo_prototype.getDateCreated = function() {
    this.selectSingleNode(".//Properties/DateCreated").getValue();
  }
  /*
  * Sets the SessionInfo date created
  * @param strName {String}
  */
  SessionInfo_prototype.setDateCreated = function() {
    var dateCreated = new Date().getTime();
    this.selectSingleNode(".//Properties/DateCreated").setValue(dateCreated);
  }

  /**
  * Gets the SessionInfo Type
  * @return {String}
  */
  SessionInfo_prototype.getSessionType = function() {
    return this.selectSingleNode(".//SessionInfo").getAttribute("type");
  }
  /*
  * Sets the SessionInfo Type
  * @param sessionType {String}
  */
  SessionInfo_prototype.setSessionType = function(sessionType) {
    this.selectSingleNode(".//SessionInfo").setAttribute("type", sessionType);
  }
  /*
  * Updates the SessionInfo Type based on the src entity types of the connections
  * @return {String}
  */
  SessionInfo_prototype._updateSessionType = function() {
    // get the current SessionInfo type to start with
    var sessionType = this.getSessionType();

    // iterate over all the connections
    var iterConn = this.getConnIter();
    while(iterConn.hasNext()) {
      var nodeConn = iterConn.next();
      // check the src entity type
      var srcEntityId = this.getConnSrcEntityId(nodeConn);
      if(jsx3.util.strEmpty(srcEntityId)) continue;
      var srcEntityType = this.getEntityType(srcEntityId);
      // if it is a live port - set to LIVE / MIXED
      if(srcEntityType == SessionInfo.ENTITYTYPE_SRCPORT) {
        if(jsx3.util.strEmpty(sessionType)) {
          sessionType = SessionInfo.SESSIONTYPE_LIVE;
        }
        else if(sessionType == SessionInfo.SESSIONTYPE_PLAYBACK) {
          sessionType = SessionInfo.SESSIONTYPE_MIXED;
          // once mixed - no need to look anymore
          break;
        }
      }
      // if it is a file - set to PLAYBACK / MIXED
      else if(srcEntityType == SessionInfo.ENTITYTYPE_PLYFILE) {
        if(jsx3.util.strEmpty(sessionType)) {
          sessionType = SessionInfo.SESSIONTYPE_PLAYBACK;
        }
        else if(sessionType == SessionInfo.SESSIONTYPE_LIVE) {
          sessionType = SessionInfo.SESSIONTYPE_MIXED;
          // once mixed - no need to look anymore
          break;
        }
      }
      // others - do nothing for now - TBD
      else {
      }
    }

    // finally set the new SessionInfo type
    this.setSessionType(sessionType);

    return sessionType;
  }

  /*
  * Gets the DefaultRec Dir Id
  * @return objDir {Object}
  */
  SessionInfo_prototype.getDefaultRecDir = function() {
    return this.getDirObj(SessionInfo.DIRID_DEFAULTREC);
  }
  /*
  * Sets the DefaultRec Dir - adds a dir node if not exist already
  * @param objDir {Object}
  */
  SessionInfo_prototype.setDefaultRecDir = function(objDir) {
    // try to update the default rec dir
    if(!this.updateDir(SessionInfo.DIRID_DEFAULTREC, objDir)) {
      // if not exist - add it
      objDir.id = SessionInfo.DIRID_DEFAULTREC;
      this.addDir(objDir);
      // update any entities that have blank dirid
      var objFile = new Object();
      objFile.dirId = objDir.id;
      var iterRecFileEntity = this.getEntityIterByType(SessionInfo.ENTITYTYPE_RECFILE);
      while(iterRecFileEntity.hasNext()) {
        var nodeEntity = iterRecFileEntity.next();
        this._updateEntityNode(nodeEntity, objFile);
      }
    }
  }
  /*
  * Clears the DefaultRec Dir - deletes the dir node and clears refs in entities
  * @return
  */
  SessionInfo_prototype.clrDefaultRecDir = function() {
    if(this.getDirObj(SessionInfo.DIRID_DEFAULTREC)) {
      // if exist - delete it
      this.delDir(SessionInfo.DIRID_DEFAULTREC);
      // update any entities that have this dirid
      var objFile = new Object();
      objFile.dirId = "";
      var iterRecFileEntity = this.getEntityIterByType(SessionInfo.ENTITYTYPE_RECFILE);
      while(iterRecFileEntity.hasNext()) {
        var nodeEntity = iterRecFileEntity.next();
        this._updateEntityNode(nodeEntity, objFile);
      }
    }
  }
  /*
  * Adds a Dir to the SessionInfo.
  * Typically not called directly. (called from add/updateEntity).
  * @param {String} dirTitle
  * @param {String} dirNID
  * @return {String} dirId
  */
  SessionInfo_prototype.addDir = function(objDir) {
    // get a new dir node
    var nodeDir = this._getNewDirNode();
    // setup a new dir id - if needed
    if(!objDir.id) {
      objDir.id = SessionInfo.DIR_IDPREFIX + getUUID();
    }

    // setup values in this dir - if err return
    if(!this._updateDirNode(nodeDir, objDir)) {
      return null;
    }

    // if success - append the new node to the list  
    var nodeDirList = this.selectSingleNode(".//DirList");
    nodeDirList.appendChild(nodeDir);

    // return the new dir id 
    return objDir.id;
  }
  /*
  * Updates a Dir to the SessionInfo.
  * Typically not called directly. (called from add/updateEntity).
  * @param {String} dirId
  * @param {Object} objDir
  * @return {String} dirId
  */
  SessionInfo_prototype.updateDir = function(dirId, objDir) {
    // get the dir Node
    var nodeDir = this.selectSingleNode(".//Dir[@id='" + dirId + "']");
    if(!nodeDir) return false;

    // setup values in this dir - if err return
    if(!this._updateDirNode(nodeDir, objDir)) {
      return null;
    }

    // if success - append the new node to the list  
    var nodeDirList = this.selectSingleNode(".//DirList");
    nodeDirList.appendChild(nodeDir);

    // return the new dir id 
    return dirId;
  }
  /*
  * Deletes a Dir from the SessionInfo.
  * Typically not called directly. (called from add/updateEntity).
  * @param {String} dirId
  * @return {Boolean}
  */
  SessionInfo_prototype.delDir = function(dirId) {
    // get the dir Node
    var nodeDir = this.selectSingleNode(".//Dir[@id='" + dirId + "']");
    if(!nodeDir) return false;

    // delete it from its parent node
    nodeDir.getParent().removeChild(nodeDir);

    // return success
    return true;
  }
  /*
  * GCs a Dir from the SessionInfo if not used anymore.
  * Typically not called directly. (called from add/updateEntity).
  * @param {String} dirId
  * @return {Boolean}
  */
  SessionInfo_prototype._gcDir = function(dirId) {
    // check for use of the dir Id
    var nodeDir = this.selectSingleNode(".//*[@dirId='" + dirId + "']");
    if(nodeDir) return false;

    // check if there are any files related to this dir
    var nodeEntityFile = this.selectSingleNode(".//Entity/File[@dirId='" + dirId + "']");
    // if none left - delete the dir
    if(!nodeEntityFile) {
      nodeDir = this.selectSingleNode(".//Dir[@id='" + dirId + "']");
      nodeDir.getParent().removeChild(nodeDir);
      return true;
    }
    // return false to indicate it still exists
    return false;
  }
  /*
  * Helper function to update dir
  * @param nodeDir {jsx3.xml.Entity}
  * @param objDir {Object}
  * @return {String} dirId
  */
  SessionInfo_prototype._updateDirNode = function(nodeDir, objDir) {
    if(objDir.dirTitle) {
      nodeDir.setAttribute("resourceTitle", objDir.dirTitle);
    }
    if(objDir.dirNID) {
      nodeDir.setAttribute("resourceNID", objDir.dirNID);
    }
    if(objDir.id) {
      nodeDir.setAttribute("id", objDir.id);
    }
    // return id
    return nodeDir.getAttribute("id");
  }
  /*
  * Helper function to check dir existence
  * @param dirTitle {String} and/or dirNID {String}
  * @return {String} dirId
  */
  SessionInfo_prototype.getDir = function(dirTitle, dirNID, dirId) {
    // form xpath to look for entity node
    var whereXPath = "";
    if(dirTitle) {
      whereXPath += "./@resourceTitle='" + dirTitle + "'";
    }
    if(dirNID) {
      if(!jsx3.util.strEmpty(whereXPath)) whereXPath += " and ";
      whereXPath += "./@resourceNID='" + dirNID + "'";
    }
    if(dirId) {
      if(!jsx3.util.strEmpty(whereXPath)) whereXPath += " and ";
      whereXPath += "./@id='" + dirId + "'";
    }
    // get the dir node
    var nodeDir = this.selectSingleNode(".//Dir[" + whereXPath + "]");
    // if not found return null
    if(!nodeDir) {
      return null;
    }
    // else return id
    return nodeDir.getAttribute("id");
  }
  /*
  * Gets a Dir obj from the SessionInfo.
  * Typically not called directly. (called from add/updateEntity).
  * @param {String} dirId
  * @return {Object}
  */
  SessionInfo_prototype.getDirObj = function(dirId) {
    // get the dir Node
    var nodeDir = this.selectSingleNode(".//Dir[@id='" + dirId + "']");
    if(!nodeDir) return null;

    // get info from dir node
    var objDir = new Object();
    objDir.id = nodeDir.getAttribute("id");
    objDir.dirNID = nodeDir.getAttribute("resourceNID");
    objDir.dirTitle = nodeDir.getAttribute("resourceTitle");

    return objDir;
  }
  /*
  * Gets the DefaultRec grpFile Id
  * @return objGrpFile {Object}
  */
  SessionInfo_prototype.getDefaultRecGrpFile = function() {
    return this.getGrpFileObj(SessionInfo.GRPFILEID_DEFAULTREC);
  }
  /*
  * Sets the DefaultRec grpFile - adds a grpFile node if not exist already
  * @param objGrpFile {Object}
  */
  SessionInfo_prototype.setDefaultRecGrpFile = function(objGrpFile) {
    // try to update the default rec grpfile
    if(!this.updateGrpFile(SessionInfo.GRPFILEID_DEFAULTREC, objGrpFile)) {
      objGrpFile.id = SessionInfo.GRPFILEID_DEFAULTREC;
      objGrpFile.grpFileType = SessionInfo.GRPFILETYPE_SYNCREC;
      this.addGrpFile(objGrpFile);
      // update any entities that have blank grpfileid
      var objFile = new Object();
      objFile.grpFileId = objGrpFile.id;
      var iterRecFileEntity = this.getEntityIterByType(SessionInfo.ENTITYTYPE_RECFILE);
      while(iterRecFileEntity.hasNext()) {
        var nodeEntity = iterRecFileEntity.next();
        this._updateEntityNode(nodeEntity, objFile);
      }
    }
  }
  /*
  * Updates the DefaultRec grpFile 
  * @param objGrpFile {Object}
  */
  SessionInfo_prototype.updateDefaultRecGrpFile = function(objGrpFile) {
    this.updateGrpFile(SessionInfo.GRPFILEID_DEFAULTREC, objGrpFile);
  }
  /*
  * Clears the DefaultRec grpFile - deletes the grpFile node and clears refs in entities
  * @return
  */
  SessionInfo_prototype.clrDefaultRecGrpFile = function() {
    if(this.getGrpFileObj(SessionInfo.GRPFILEID_DEFAULTREC)) {
      // if exist - delete it
      this.delGrpFile(SessionInfo.GRPFILEID_DEFAULTREC);
      // update any entities that have this grpfileid
      var objFile = new Object();
      objFile.grpFileId = "";
      var iterRecFileEntity = this.getEntityIterByType(SessionInfo.ENTITYTYPE_RECFILE);
      while(iterRecFileEntity.hasNext()) {
        var nodeEntity = iterRecFileEntity.next();
        this._updateEntityNode(nodeEntity, objFile);
      }
    }
  }


  /*
  * Updates a GrpFile to the SessionInfo.
  * Typically not called directly. (called from add/updateEntity).
  * @param {Object} objGrpFile
  * @return {String} grpFileId
  */
  SessionInfo_prototype.updateGrpFile = function(grpFileId, objGrpFile) {
    // get the GrpFile node
    var nodeGrpFile = this.selectSingleNode(".//GrpFile[@id='" + grpFileId + "']");
    if(!nodeGrpFile) return false;

    // setup values in this grpFile - if err return
    if(!this._updateGrpFileNode(nodeGrpFile, objGrpFile)) {
      return null;
    }

    // return the new grpFile id 
    return grpFileId;
  }
  /*
  * Deletes a GrpFile from the SessionInfo.
  * Typically not called directly. (called from add/updateEntity).
  * @param {String} grpFileId
  * @return {Boolean}
  */
  SessionInfo_prototype.delGrpFile = function(grpFileId) {
    // get the GrpFile node
    var nodeGrpFile = this.selectSingleNode(".//GrpFile[@id='" + grpFileId + "']");
    if(!nodeGrpFile) return false;

    // delete it from its parent node
    nodeGrpFile.getParent().removeChild(nodeGrpFile);

    // return success
    return true;
  }
  /*
  * GCs a GrpFile from the SessionInfo if not used anymore.
  * Typically not called directly. (called from add/updateEntity/deleteEntity).
  * @param {String} grpFileId
  * @return {Boolean}
  */
  SessionInfo_prototype._gcGrpFile = function(grpFileId) {
    // check for use of the grpFile Id
    var nodeGrpFile = this.selectSingleNode(".//*[@id='" + grpFileId + "']");
    if(!nodeGrpFile) return false;

    // check if there are any files related to this grpfile
    var nodeEntityFile = this.selectSingleNode(".//Entity/File[@grpFileId='" + grpFileId + "']");
    // if none left - delete the grpfile
    if(!nodeEntityFile) {
      nodeGrpFile.getParent().removeChild(nodeGrpFile);
      return true;
    }
    // return false to indicate it still exists
    return false;
  }
  /*
  * Helper function to update grpFile
  * @param nodeGrpFile {jsx3.xml.Entity}
  * @param objGrpFile {Object}
  * @return {String} grpFileId
  */
  SessionInfo_prototype._updateGrpFileNode = function(nodeGrpFile, objGrpFile) {
    if(objGrpFile.grpFileTitle) {
      nodeGrpFile.setAttribute("resourceTitle", objGrpFile.grpFileTitle);
    }
    if(objGrpFile.grpFileNID) {
      nodeGrpFile.setAttribute("resourceNID", objGrpFile.grpFileNID);
    }
    if(objGrpFile.grpFileType) {
      nodeGrpFile.setAttribute("type", objGrpFile.grpFileType);
    }
    if(objGrpFile.id) {
      nodeGrpFile.setAttribute("id", objGrpFile.id);
    }
    // return id
    return nodeGrpFile.getAttribute("id");
  }
  /*
  * Helper function to check grpFile existence
  * @param grpFileTitle {String} and/or grpFileNID {String}
  * @return {String} grpFileId
  */
  SessionInfo_prototype.getGrpFile = function(grpFileTitle, grpFileNID, grpFileId) {
    // form xpath to look for entity node
    var whereXPath = "";
    if(grpFileTitle) {
      whereXPath += "./@resourceTitle='" + grpFileTitle + "'";
    }
    if(grpFileNID) {
      if(!jsx3.util.strEmpty(whereXPath)) whereXPath += " and ";
      whereXPath += "./@resourceNID='" + grpFileNID + "'";
    }
    if(grpFileId) {
      if(!jsx3.util.strEmpty(whereXPath)) whereXPath += " and ";
      whereXPath += "./@id='" + grpFileId + "'";
    }
    // get the grpFile node
    var nodeGrpFile = this.selectSingleNode(".//GrpFile[" + whereXPath + "]");
    // if not found return null
    if(!nodeGrpFile) {
      return null;
    }
    // else return id
    return nodeGrpFile.getAttribute("id");
  }

  /*
  * Helper function to get the grpFileId from the entity
  * @param entityId {String}
  * @return {String} grpFileId
  */
  SessionInfo_prototype.getGrpFileId = function(entityId) {
    var nodeEntity = this.selectSingleNode(".//Entity[@id='" + entityId + "']");
    if(!nodeEntity) return null;

    var grpFileId = nodeEntity.selectSingleNode(".//File").getAttribute("grpFileId");
    if(!grpFileId) return null;
    return grpFileId;
  }

  /*
  * Gets a GrpFile obj from the SessionInfo.
  * Typically not called directly. (called from add/updateEntity).
  * @param {String} grpFileId
  * @return {Object}
  */
  SessionInfo_prototype.getGrpFileObj = function(grpFileId) {
    // get the GrpFile node
    var nodeGrpFile = this.selectSingleNode(".//GrpFile[@id='" + grpFileId + "']");
    if(!nodeGrpFile) return null;

    // get info from grpFile node
    var objGrpFile = new Object();
    objGrpFile.id = nodeGrpFile.getAttribute("id");
    objGrpFile.grpFileType = nodeGrpFile.getAttribute("type");
    objGrpFile.grpFileNID = nodeGrpFile.getAttribute("resourceNID");
    objGrpFile.grpFileTitle = nodeGrpFile.getAttribute("resourceTitle");

    return objGrpFile;
  }
  /*
  * Internal utility function to get new grpFile nodes
  * @return {jsx3.xml.Entity} nodeGrpFile
  */
  SessionInfo_prototype._getNewGrpFileNode = function(grpFileType) {
    // get a template from the cache OR model/objectXml
    var nodeSession = SessionInfo_prototype._getSessionObjectXML();

    // get a grpFile node
    var nodeGrpFile = nodeSession.selectSingleNode(".//GrpFile").cloneNode(true);
    // set the grpFileType
    nodeGrpFile.setAttribute("type", grpFileType);

    return nodeGrpFile;
  }
  /*
  * Adds a GrpFile to the SessionInfo.
  * Typically not called directly. (called from add/updateEntity).
  * @param {Object} objGrpFile
  * @return {String} grpFileId
  */
  SessionInfo_prototype.addGrpFile = function(objGrpFile) {
    // get a new grpFile node
    var nodeGrpFile = SessionInfo_prototype._getNewGrpFileNode(objGrpFile.grpFileType);
    // setup a new grpFile id - if needed
    if(!objGrpFile.id) {
      objGrpFile.id = SessionInfo.GRPFILE_IDPREFIX + getUUID();
    }

    // setup values in this grpFile - if err return
    if(!this._updateGrpFileNode(nodeGrpFile, objGrpFile)) {
      return null;
    }

    // if success - append the new node to the list  
    var nodeGrpFileList = this.selectSingleNode(".//GrpFileList");
    nodeGrpFileList.appendChild(nodeGrpFile);

    // return the new grpFile id 
    return objGrpFile.id;
  }
  /*
  * Internal utility function to get new dir nodes
  * @return {jsx3.xml.Entity} nodeEntity
  */
  SessionInfo_prototype._getNewDirNode = function() {
    // get a template from the cache OR model/objectXml
    var nodeSession = SessionInfo_prototype._getSessionObjectXML();

    // get the default dir node
    var nodeDir = nodeSession.selectSingleNode(".//Dir").cloneNode(true);

    return nodeDir;
  }

  /*
  * Adds an entity to the SessionInfo
  * @param {Object} objEntity
  * @return {String} entityId
  */
  SessionInfo_prototype.addEntity = function(objEntity) {
    // create a new entity node based on type
    var nodeEntity = SessionInfo._getNewEntityNode(objEntity.type);
    if(!nodeEntity) return null;

    // generate a new entityId - if needed
    var entityId = objEntity.id;
    if(!entityId) {
      entityId = objEntity.type + '_' + getUUID();
      objEntity.id = entityId;
    }

    // set the values in the new node 
    this._updateEntityNode(nodeEntity, objEntity);

    // append it to the list in the current collab xml in the topic
    var nodeEntityList = this.selectSingleNode(".//EntityList");
    nodeEntityList.appendChild(nodeEntity);

    // return the id of the new entity
    return entityId;
  }
  /*
  * Deletes an Entity from the SessionInfo.
  * Typically not called directly. (called from add/updateConnGroup).
  * @param {String} entityId
  * @return {Boolean}
  */
  SessionInfo_prototype.delEntity = function(entityId) {
    // get the entity Node
    var nodeEntity = this.selectSingleNode(".//Entity[@id='" + entityId + "']");
    if(!nodeEntity) return false;

    // find the entity type.
    var entityType = this.getEntityType(entityId);
    var grpFileId = null;
    //  If its PlyFile then get the grpFileId
    if(entityType == SessionInfo.ENTITYTYPE_PLYFILE) {
      grpFileId = this.getGrpFileId(entityId);
    }

    // delete it from its parent node
    nodeEntity.getParent().removeChild(nodeEntity);

    // If its PlyFile and the grpFileId is not "" or null then do gcGrpFile
    if((entityType == SessionInfo.ENTITYTYPE_PLYFILE) && (!jsx3.util.strEmpty(grpFileId))) {
      this._gcGrpFile(grpFileId);
    }

    // return success
    return true;
  }
  /*
  * Updates an entity in the SessionInfo
  * @param {String} entityId
  * @param {Object} objEntity
  * @return {Boolean}
  */
  SessionInfo_prototype.updateEntity = function(entityId, objEntity) {
    // get the entity Node
    var nodeEntity = this.selectSingleNode(".//Entity[@id='" + entityId + "']");
    if(!nodeEntity) return false;

    // update entity node
    return this._updateEntityNode(nodeEntity, objEntity);
  }
  /*
  * GCs an Entity from the SessionInfo if not used anymore.
  * Typically not called directly. (called from add/updateConn).
  * @param {String} entityId
  * @return {Boolean}
  */
  SessionInfo_prototype._gcEntity = function(entityId) {
    // get the entity Node
    var nodeEntity = this.selectSingleNode(".//Entity[@id='" + entityId + "']");
    if(!nodeEntity) return true;

    // check if there are any conns using this entity
    var nodeConn = this.selectSingleNode(".//Connection[@srcEntityId='" + entityId + " or " + "@dstEntityId='" + entityId + "']");
    // if none left - delete the entity
    if(!nodeConn) {
      nodeEntity.getParent().removeChild(nodeEntity);
      return true;
    }
    // return false to indicate it still exists
    return false;
  }
  /*
  * Checks the entity in the Connection List.
  * If entityId is used in any of the connections then returns true else false
  * @param (String) entityId
  */
  SessionInfo_prototype.checkEntityInConnectionList = function(entityId) {
    // check if entity is peresnet in any of the connection
    if(this.selectSingleNode(".//Connection[@dstEntityId='" + entityId + "'" +
                                             " or @srcEntityId='" + entityId + "']")) {
      return true;
    }
    return false;
  }
  /*
  * Internal method to updates an entity Node in the SessionInfo
  * @param {jsx3.xml.Entity} nodeEntity
  * @param {Object} objEntity
  * @return {Boolean}
  */
  SessionInfo_prototype._updateEntityNode = function(nodeEntity, objEntity) {
    // set the values in the node - skip anything unspecified
    if(objEntity.id) nodeEntity.setAttribute("id", objEntity.id);
    if(objEntity.name) nodeEntity.setAttribute("name", objEntity.name);
    if(objEntity.streamType) nodeEntity.setAttribute("streamType", objEntity.streamType);
    if(objEntity.presence) nodeEntity.setAttribute("presence", objEntity.presence);

    var nodeEntityUser = nodeEntity.selectSingleNode("./User");
    if(nodeEntityUser) {
      if(objEntity.userNID) nodeEntityUser.setAttribute("userNID", objEntity.userNID);
      if(objEntity.userJID) nodeEntityUser.setAttribute("userJID", objEntity.userJID);
      if(objEntity.userTitle) {
        nodeEntityUser.setAttribute("username", objEntity.userTitle);
        if(!objEntity.name) nodeEntity.setAttribute("name", objEntity.userTitle);
      }
    }
    var nodeEntityPort = nodeEntity.selectSingleNode("./Port");
    if(nodeEntityPort) {
      if(objEntity.portNID) nodeEntityPort.setAttribute("resourceNID", objEntity.portNID);
      if(objEntity.portTitle) {
        nodeEntityPort.setAttribute("resourceTitle", objEntity.portTitle);
        if(!objEntity.name) nodeEntity.setAttribute("name", objEntity.portTitle);
      }
      if(objEntity.maxInstanceCount) {
        var nodeInstanceList = nodeEntityPort.selectSingleNode("./InstanceList");
        if(nodeInstanceList) nodeInstanceList.setAttribute("maxInstanceCount", objEntity.maxInstanceCount);
      }
    }

    var nodeEntityPVR = nodeEntity.selectSingleNode("./PVR");
    if (nodeEntityPVR && !jsx3.util.strEmpty(objEntity.PVREnable)) {
      nodeEntityPVR.setAttribute("enable", objEntity.PVREnable);
    }
    var nodeEntityFile = nodeEntity.selectSingleNode("./File");
    if(nodeEntityFile) {
      // update file
      if(objEntity.fileNID) nodeEntityFile.setAttribute("resourceNID", objEntity.fileNID);
      if(objEntity.fileTitle) {
        nodeEntityFile.setAttribute("resourceTitle", objEntity.fileTitle);
        if(!objEntity.name) nodeEntity.setAttribute("name", objEntity.fileTitle);
      }
      // update dir
      if(objEntity.dirId || objEntity.dirTitle || objEntity.dirNID) {
        // save the current val of dirId
        var oldDirId = nodeEntityFile.getAttribute("dirId");
        // check and reuse dir if already exists
        objEntity.dirId = this.getDir(objEntity.dirTitle, objEntity.dirNID, objEntity.dirId);
        if(!objEntity.dirId) {
          // if not exist - add it to dir list
          var objDir = new Object;
          objDir.id = objEntity.dirId;
          objDir.dirTitle = objEntity.dirTitle;
          objDir.dirNID = objEntity.dirNID;

          objEntity.dirId = this.addDir(objDir);
        }
        nodeEntityFile.setAttribute("dirId", objEntity.dirId);
        // finally GC the old dir if needed
        if(!jsx3.util.strEmpty(oldDirId)) {
          this._gcDir(oldDirId);
        }
      }
      // update grp file
      if(objEntity.grpFileId || objEntity.grpFileTitle || objEntity.grpFileNID) {
        // save the current val of grpFileId
        var oldGrpFileId = nodeEntityFile.getAttribute("grpFileId");
        // check and reuse grpfile if already exists
        objEntity.grpFileId = this.getGrpFile(objEntity.grpFileTitle, objEntity.grpFileNID, objEntity.grpFileId);
        if(!objEntity.grpFileId) {
          // if not exist - add it to grpfile list - only if either name or title is specified
          if(objEntity.grpFileTitle || objEntity.grpFileNID) {
            if(!objEntity.grpFileType) {
              if(nodeEntity.getAttribute("type") == SessionInfo.ENTITYTYPE_RECFILE) {
                objEntity.grpFileType = SessionInfo.GRPFILETYPE_SYNCREC;
              }
              else {
                objEntity.grpFileType = SessionInfo.GRPFILETYPE_SYNCPLY;
              }
            }
            objEntity.grpFileId = this.addGrpFile(objEntity.grpFileType, objEntity.grpFileTitle, objEntity.grpFileNID, objEntity.grpFileId);
          }
          //TBD - log error here : grpfileId specified for non existent grp file
        }
        nodeEntityFile.setAttribute("grpFileId", objEntity.grpFileId);
        // finally GC the old grpfile if needed
        if(!jsx3.util.strEmpty(oldGrpFileId)) {
          this._gcGrpFile(oldGrpFileId);
        }
      }
      // update clipId
      if(objEntity.clipId) {
        if(jsx3.util.strEmpty(objEntity.clipId)) {
          objEntity.clipId = SessionInfo.ENTITYTYPE_RECFILE + "_" + getUUID();
        }
        nodeEntityFile.setAttribute("clipId", objEntity.clipId);
      }
      // update the recSessionConfigNID if its there()
      // generally it will be there if plyfile is added as an entity
      if(objEntity.recSessionConfigNID) {
        if(!jsx3.util.strEmpty(objEntity.recSessionConfigNID)) {
          nodeEntityFile.setAttribute("recSessionConfigNID", objEntity.recSessionConfigNID);
        }
      }
    }

    // return success
    return true;
  }
  /*
  * Adds an Instance to an Entity.
  * Checks to see if maxInstanceCount is exceeded before adding instance
  * (TBD: Currently uses external function to get new Instance node)
  * @param {String} entityId
  * @param {String} instanceId (optional)
  * @return {String} instanceId
  */
  SessionInfo_prototype.addEntityInstance = function(entityId, instanceId) {
    // get the entity Node
    var nodeEntity = this.selectSingleNode(".//Entity[@id='" + entityId + "']");
    if(!nodeEntity) return null;

    // generate a new InstanceId - if needed
    if(!instanceId) instanceId = entityId + "::" + SessionInfo.INSTANCE_IDPREFIX + getUUID();

    // check maxInstanceCount
    var maxInstanceCount = nodeEntity.selectSingleNode("./*/InstanceList/@maxInstanceCount").getValue();
    if(!jsx3.util.strEmpty(maxInstanceCount)) {
      var currInstanceCount = nodeEntity.selectNodes(".//Instance").size();
      if(currInstanceCount >= maxInstanceCount) {
        return null;
      }
    }
    // create a new entity Instance node 
    var nodeInstance = SessionInfo._getNewEntityInstanceNode();
    if(!nodeInstance) return null;

    // set the value in the new node
    nodeInstance.selectSingleNode("./@id").setValue(instanceId ? instanceId : "");

    // append it to the instance list in the entity node
    var nodeInstanceList = nodeEntity.selectSingleNode(".//InstanceList");
    nodeInstanceList.appendChild(nodeInstance);

    // return the id of the new instance
    return instanceId;
  }
  /*
  * Gets an Iterator over the Entities
  * 
  * @return {jsx3.util.Iterator} Entity Nodes
  */
  SessionInfo_prototype.getEntityIter = function() {
    return this.selectNodeIterator(".//Entity");
  }
  /*
  * Gets an Iterator over the Entities of the specified type
  * @param entityType {String}
  * @return {jsx3.util.Iterator} Entity Nodes
  */
  SessionInfo_prototype.getEntityIterByType = function(entityType) {
    var whereXPath = "";
    if(entityType) {
      whereXPath = "@type='" + entityType + "'";
    }
    return this.selectNodeIterator(".//Entity[" + whereXPath + "]");
  }

  /*
  * Helper function to check entity existence
  * @param entityType {String} and/or entityNID {String}
  * @return {String} entityId
  */
  // 
  SessionInfo_prototype.getEntity = function(entityType, entityNID, entityId) {
    // form xpath to look for entity node
    var whereXPath = "";
    if(entityType) {
      whereXPath += "./@type='" + entityType + "'";
    }
    if(entityType == SessionInfo.ENTITYTYPE_USER) {
      if(!jsx3.util.strEmpty(whereXPath)) whereXPath += " and ";
      whereXPath += "./User/@resourceNID='" + entityNID + "'";
    }
    else if(entityType == SessionInfo.ENTITYTYPE_SRCPORT ||
              entityType == SessionInfo.ENTITYTYPE_DSTPORT ||
              entityType == SessionInfo.ENTITYTYPE_RELAYPORT ||
              entityType == SessionInfo.ENTITYTYPE_STOREPORT) {
      if(!jsx3.util.strEmpty(whereXPath)) whereXPath += " and ";
      whereXPath += "./Port/@resourceNID='" + entityNID + "'";
    }
    else if(entityType == SessionInfo.ENTITYTYPE_PLYFILE ||
              entityType == SessionInfo.ENTITYTYPE_RECFILE ||
              entityType == SessionInfo.ENTITYTYPE_PVRFILE) {
      if(!jsx3.util.strEmpty(whereXPath)) whereXPath += " and ";
      whereXPath += "./File/@resourceNID='" + entityNID + "'";
    }
    if(entityId) {
      if(!jsx3.util.strEmpty(whereXPath)) whereXPath += " and ";
      whereXPath += "./@id='" + entityId + "'";
    }
    // get the entity node
    var nodeEntity = this.selectSingleNode(".//Entity[" + whereXPath + "]");
    // if not found return null
    if(!nodeEntity) {
      return null;
    }
    // else return id
    return nodeEntity;
  }

  /*
  * Gets the id of an entity
  * @ param nodeEntity
  * @return {String}
  */
  SessionInfo_prototype.getEntityId = function(nodeEntity) {
    return nodeEntity.getAttribute("id");
  }
  /*
  * Gets the name of an entity
  * @ param entityId
  * @return {String}
  */
  SessionInfo_prototype.getEntityName = function(entityId) {
    // get the entity Node
    var nodeEntity = this.selectSingleNode(".//Entity[@id='" + entityId + "']");
    if(!nodeEntity) return null;

    return nodeEntity.getAttribute("name");
  }
  /*
  * Gets the entity by name
  * @ param entityId
  * @return {entity node}
  */
  SessionInfo_prototype.getEntityByName = function(entityName) {
    // get the entity Node
    var nodeEntity = this.selectSingleNode(".//Entity[@name='" + entityName + "']");
    if(!nodeEntity) return null;

    return nodeEntity;
  }
  /*
  * Gets the entity by NID
  * @ param entityId
  * @return {entity node}
  */
  SessionInfo_prototype.getEntityByNID = function(portNID) {
    // get the entity Node
    var nodeEntity = this.selectSingleNode(".//Entity[Port/@resourceNID='" + portNID + "']");
    if(!nodeEntity) return null;

    return nodeEntity;
  }
  /*
  * Gets the entity by type
  * @ param entityType
  * @return {entity node}
  */
  SessionInfo_prototype.getEntityByType = function(entityType) {
    // get the entity Node
    var nodeEntity = this.selectSingleNode(".//Entity[@type='" + entityType + "'] ");
    if(!nodeEntity) return null;

    return nodeEntity;
  }
  /*
  * Gets the presence of an entity
  * @ param nodeEntity
  * @return {String}
  */
  SessionInfo_prototype.getEntityPresence = function(nodeEntity) {
    return nodeEntity.getAttribute("presence");
  }
  /*
  * Gets the type of an entity
  * @ param entityId
  * @return {String}
  */
  SessionInfo_prototype.getEntityType = function(entityId) {
    // get the entity Node
    var nodeEntity = this.selectSingleNode(".//Entity[@id='" + entityId + "']");
    if(!nodeEntity) return null;

    return nodeEntity.getAttribute("type");
  }
  /*
  * Gets the streamType of an entity
  * @ param entityId
  * @return {String}
  */
  SessionInfo_prototype.getEntityStreamType = function(entityId) {
    // get the entity Node
    var nodeEntity = this.selectSingleNode(".//Entity[@id='" + entityId + "']");
    if(!nodeEntity) return null;

    return nodeEntity.getAttribute("streamType");
  }
  /*
  * Gets the pvrEnable of an entity
  * @ param nodeEntity
  * @return {String}
  */
  SessionInfo_prototype.getEntityPVREnable = function(nodeEntity) {
    return getNodeValue(nodeEntity.selectSingleNode("./PVR/@enable"), null);
  }

  /*
  * Gets the sessionId from the pvr entity
  * @ param nodeEntity
  * @return {String}
  */
  SessionInfo_prototype.getEntityPVRSessionId = function(nodeEntity) {
    return getNodeValue(nodeEntity.selectSingleNode("./PVR/@sessionId"), null);
  }

  /*
  * Gets the trimlength from the pvr entity
  * @ param nodeEntity
  * @return {String}
  */
  SessionInfo_prototype.getEntityPVRTrimLength = function(nodeEntity) {
    return getNodeValue(nodeEntity.selectSingleNode("./PVR/@trimLength"), null);
  }

  /*
  * Gets the startTC from the pvr entity
  * @ param nodeEntity
  * @return {String}
  */
  SessionInfo_prototype.getEntityPVRStartTC = function(nodeEntity) {
    return getNodeValue(nodeEntity.selectSingleNode("./PVR/@startTC"), null);
  }

  /*
  * Gets the connId from the pvr entity
  * @ param nodeEntity
  * @return {String}
  */
  SessionInfo_prototype.getEntityPVRConnId = function(nodeEntity) {
    return getNodeValue(nodeEntity.selectSingleNode("./PVR/@connId"), null);
  }

  /*
  * Gets the dirId from the pvr entity
  * @ param nodeEntity
  * @return {String}
  */
  SessionInfo_prototype.getEntityPVRDirId = function(nodeEntity) {
    return getNodeValue(nodeEntity.selectSingleNode("./PVR/@dirId"), null);
  }

  /*
  * Gets the playlength of an entity
  * @ param nodeEntity
  * @return {String}
  */
  SessionInfo_prototype.getEntityPlaylength = function(nodeEntity) {
    return getNodeValue(nodeEntity.selectSingleNode("./File/@playLength"), null);
  }

  /*
  * Gets the mediaGroupId of an entity
  * @ param nodeEntity
  * @return {String}
  */
  SessionInfo_prototype.getEntityMediaGroupId = function(nodeEntity) {
    return getNodeValue(nodeEntity.selectSingleNode("./File/@mediaGroupId"), null);
  }

  /*
  * Gets the mediaGroupFileNID
  * @ param mediaGroupId
  * @return {String}
  */
  SessionInfo_prototype.getEntityMediaGroupFileNID = function(mediaGroupId) {
    var grpFileNode = this.selectSingleNode(".//GrpFile[@mediaGroupId='" + mediaGroupId + "']");
    if(jsx3.util.strEmpty(grpFileNode)) return null;
    return getNodeValue(grpFileNode.selectSingleNode("//GrpFile/@resourceNID"), null);
  }

  /*
  * Gets the fileNID of an entity
  * @ param nodeEntity
  * @return {String}
  */
  SessionInfo_prototype.getEntityFileNID = function(nodeEntity) {
    return getNodeValue(nodeEntity.selectSingleNode("./File/@resourceNID"), null);
  }
  /*
  * Gets an entity object for an entityId
  * @ param entityId
  * @return {Object} objEntity
  */
  SessionInfo_prototype.getEntityObj = function(entityId) {
    // get the entity Node
    var nodeEntity = this.selectSingleNode(".//Entity[@id='" + entityId + "']");
    if(!nodeEntity) return null;

    // get the values from the entity Node
    var objEntity = new Object();
    objEntity.id = getNodeValue(nodeEntity.selectSingleNode("./@id"), null);
    objEntity.name = getNodeValue(nodeEntity.selectSingleNode("./@name"), null);
    objEntity.type = getNodeValue(nodeEntity.selectSingleNode("./@type"), null);
    objEntity.streamType = getNodeValue(nodeEntity.selectSingleNode("./@streamType"), null);
    objEntity.presence = getNodeValue(nodeEntity.selectSingleNode("./@presence"), null);

    objEntity.userNID = getNodeValue(nodeEntity.selectSingleNode("./User/@resourceNID"), null);
    objEntity.userTitle = getNodeValue(nodeEntity.selectSingleNode("./User/@resourceTitle"), null);

    objEntity.portNID = getNodeValue(nodeEntity.selectSingleNode("./Port/@resourceNID"), null);
    objEntity.portTitle = getNodeValue(nodeEntity.selectSingleNode("./Port/@resourceTitle"), null);
    objEntity.maxInstanceCount = getNodeValue(nodeEntity.selectSingleNode("./Port/InstanceList/@maxInstanceCount"), null);

    objEntity.PVREnable = getNodeValue(nodeEntity.selectSingleNode("./PVR/@enable"), null);
    objEntity.plyEntityId = getNodeValue(nodeEntity.selectSingleNode("./PVR/@plyEntityId"), null);
    objEntity.pvrConnId = getNodeValue(nodeEntity.selectSingleNode("./PVR/@pvrConnId"), null);
    objEntity.recEntityId = getNodeValue(nodeEntity.selectSingleNode("./PVR/@recEntityId"), null);
    objEntity.trimLength = getNodeValue(nodeEntity.selectSingleNode("./PVR/@trimLength"), null);

    objEntity.fileNID = getNodeValue(nodeEntity.selectSingleNode("./File/@resourceNID"), null);
    objEntity.fileTitle = getNodeValue(nodeEntity.selectSingleNode("./File/@resourceTitle"), null);
    objEntity.playLength = getNodeValue(nodeEntity.selectSingleNode("./File/@playLength"), null);
    objEntity.dateCreated = getNodeValue(nodeEntity.selectSingleNode("./File/@dateCreated"), null);
    var objDir = this.getDirObj(getNodeValue(nodeEntity.selectSingleNode("./File/@dirId"), ""));
    if(objDir) {
      objEntity.dirNID = objDir.dirNID;
      objEntity.dirTitle = objDir.dirTitle;
    }
    var objGrpFile = this.getGrpFileObj(getNodeValue(nodeEntity.selectSingleNode("./File/@grpFileId"), ""));
    if(objGrpFile) {
      objEntity.grpFileNID = objGrpFile.grpFileNID;
      objEntity.grpFileTitle = objGrpFile.grpFileTitle;
    }
    // return the entity object
    return objEntity;
  }
  /*
  * Gets the effective NID for an entityId
  * Typically this should not be used.
  * @ param entityId
  * @return {String} NID
  */
  SessionInfo_prototype.getEntityNID = function(entityId) {
    // get the entity Node
    var nodeEntity = this.selectSingleNode(".//Entity[@id='" + entityId + "']");
    if(!nodeEntity) return null;

    // get entity Type
    var entityType = nodeEntity.selectSingleNode("./@type").getValue();

    // get NID - based on type
    var entityNID = null;
    if(entityType == SessionInfo.ENTITYTYPE_USER) {
      entityNID = nodeEntity.selectSingleNode("./User/@resourceNID").getValue();
    }
    else if(entityType == SessionInfo.ENTITYTYPE_SRCPORT ||
              entityType == SessionInfo.ENTITYTYPE_DSTPORT ||
              entityType == SessionInfo.ENTITYTYPE_RELAYPORT ||
              entityType == SessionInfo.ENTITYTYPE_STOREPORT) {
      entityNID = nodeEntity.selectSingleNode("./Port/@resourceNID").getValue();
    }
    else if(entityType == SessionInfo.ENTITYTYPE_PLYFILE ||
              entityType == SessionInfo.ENTITYTYPE_RECFILE ||
              entityType == SessionInfo.ENTITYTYPE_PVRFILE) {
      entityNID = nodeEntity.selectSingleNode("./File/@resourceNID").getValue();
    }
    // return NID
    return entityNID;
  }
  /*
  * Internal utility function to get new entity instance nodes
  * @ param entityType
  * @return {jsx3.xml.Entity} nodeEntityInstance
  */
  SessionInfo_prototype._getNewEntityInstanceNode = function() {
    // get a template from the cache OR model/objectXml
    var nodeSession = SessionInfo_prototype._getSessionObjectXML();

    // get an entity instance node 
    var nodeEntityInstance = null;
    var nodeInstanceList = nodeSession.selectSingleNode(".//InstanceList");
    if(nodeInstanceList) {
      nodeEntityInstance = nodeInstanceList.selectSingleNode(".//Instance").cloneNode(true);
    }

    return nodeEntityInstance;
  }
  /*
  * Internal utility function to get new entity nodes
  * @ param entityType
  * @return {jsx3.xml.Entity} nodeEntity
  */
  SessionInfo_prototype._getNewEntityNode = function(entityType) {
    // get a template from the cache OR model/objectXml
    var nodeSession = SessionInfo_prototype._getSessionObjectXML();

    // get an entity node
    var nodeEntity = nodeSession.selectSingleNode(".//Entity").cloneNode(true);
    // if instance support - remove all instances
    var nodeInstanceList = nodeEntity.selectSingleNode(".//InstanceList");
    if(nodeInstanceList) {
      nodeInstanceList.removeChildren();
    }
    // set the entityType
    nodeEntity.setAttribute("type", entityType);
    // modify the node contents as needed based on entityType
    var nodeEntityUser = nodeEntity.selectSingleNode(".//User");
    var nodeEntityPort = nodeEntity.selectSingleNode(".//Port");
    var nodeEntityFile = nodeEntity.selectSingleNode(".//File");
    var nodeEntityPVR = nodeEntity.selectSingleNode(".//PVR");
    var nodeEntityChat = nodeEntity.selectSingleNode(".//Chat");
    var nodeEntityAuthCtrl = nodeEntity.selectSingleNode(".//AuthCtrl");
    var nodeEntityMediaCtrl = nodeEntity.selectSingleNode(".//MediaCtrl");

    if(entityType == SessionInfo.ENTITYTYPE_USER) {
      nodeEntity.removeChild(nodeEntityPort);
      nodeEntity.removeChild(nodeEntityFile);
      nodeEntity.removeChild(nodeEntityPVR);
    }
    else if(entityType == SessionInfo.ENTITYTYPE_SRCPORT) {
      nodeEntity.removeChild(nodeEntityFile);
      nodeEntity.removeChild(nodeEntityChat);
      nodeEntity.removeChild(nodeEntityAuthCtrl);
      nodeEntity.removeChild(nodeEntityMediaCtrl);
    }
    else if(entityType == SessionInfo.ENTITYTYPE_DSTPORT) {
      nodeEntity.removeChild(nodeEntityFile);
      nodeEntity.removeChild(nodeEntityPVR);
      nodeEntity.removeChild(nodeEntityChat);
      nodeEntity.removeChild(nodeEntityAuthCtrl);
      nodeEntity.removeChild(nodeEntityMediaCtrl);
    }
    else if(entityType == SessionInfo.ENTITYTYPE_RELAYPORT ||
             entityType == SessionInfo.ENTITYTYPE_STOREPORT) {
      nodeEntity.removeChild(nodeEntityUser);
      nodeEntity.removeChild(nodeEntityFile);
      nodeEntity.removeChild(nodeEntityPVR);
      nodeEntity.removeChild(nodeEntityChat);
      nodeEntity.removeChild(nodeEntityAuthCtrl);
      nodeEntity.removeChild(nodeEntityMediaCtrl);
    }
    else if(entityType == SessionInfo.ENTITYTYPE_PLYFILE ||
              entityType == SessionInfo.ENTITYTYPE_RECFILE ||
              entityType == SessionInfo.ENTITYTYPE_PVRFILE) {
      nodeEntity.removeChild(nodeEntityUser);
      nodeEntity.removeChild(nodeEntityPVR);
      nodeEntity.removeChild(nodeEntityChat);
      nodeEntity.removeChild(nodeEntityAuthCtrl);
      nodeEntity.removeChild(nodeEntityMediaCtrl);
    }

    return nodeEntity;
  }
  /*
  * Adds an entity to the SessionInfo
  * @param {Object} objEntity
  * @return {String} entityId
  */
  SessionInfo_prototype.addEntity = function(objEntity) {
    // create a new entity node based on type
    var nodeEntity = SessionInfo_prototype._getNewEntityNode(objEntity.type);
    if(!nodeEntity) return null;

    // generate a new entityId - if needed
    var entityId = objEntity.id;
    if(!entityId) {
      entityId = objEntity.type + '_' + getUUID();
      objEntity.id = entityId;
    }

    // set the values in the new node 
    this._updateEntityNode(nodeEntity, objEntity);

    // append it to the list in the current collab xml in the topic
    var nodeEntityList = this.selectSingleNode(".//EntityList");
    nodeEntityList.appendChild(nodeEntity);

    // return the id of the new entity
    return entityId;
  }

  /*
  * function to get GrpFileList
  * @return  nodeGrpFileList
  */
  SessionInfo_prototype.getGrpFileIter = function() {
    return this.selectNodeIterator(".//GrpFile");;
  }
  /*
  * function to set GrpFileList
  * @param {nodeListIter} GrpFileListIter
  */
  SessionInfo_prototype.setGrpFileList = function(GrpFileListIter) {
    var nodeGrpFileList = this.selectSingleNode(".//GrpFileList");
    while(GrpFileListIter.hasNext()) {
      var grpFileNode = GrpFileListIter.next();
      nodeGrpFileList.appendChild(grpFileNode);
    }
  }

  /*
  * function to get getGrpFileNumber
  * @return  grpFileNumber
  */
  SessionInfo_prototype.getGrpFiles = function() {
    var nodeGrpFileList = this.selectNodes(".//GrpFileList/GrpFile");
    var grpFileNumber = nodeGrpFileList == null ? 0 : nodeGrpFileList.getLength();
    return grpFileNumber;
  }

  /*
  * function to get DirList
  * @return  nodeDirList
  */
  SessionInfo_prototype.getDirIter = function() {
    return this.selectNodeIterator(".//Dir");
  }
  /*
  * function to set DirList
  * @param {nodeListIter} DirListIter
  */
  SessionInfo_prototype.setDirList = function(DirListIter) {
    var nodeDirList = this.selectSingleNode(".//DirList");
    while(DirListIter.hasNext()) {
      var dirNode = DirListIter.next();
      nodeDirList.appendChild(dirNode);
    }
  }

  /*
  * function to set EntityList
  * @param {nodeList} EntityListIter
  */
  SessionInfo_prototype.setEntityList = function(EntityListIter) {
    var nodeEntityList = this.selectSingleNode(".//EntityList");
    while(EntityListIter.hasNext()) {
      var entityNode = EntityListIter.next();
      nodeEntityList.appendChild(entityNode);
    }
  }

  /******************************** CONNECTIONS **************************/

  /*
  * function to set ConnList
  * @param {nodeList} ConnListIter
  */
  SessionInfo_prototype.setConnList = function(ConnListIter) {
    var nodeConnList = this.selectSingleNode(".//ConnectionList");
    while(ConnListIter.hasNext()) {
      var connNode = ConnListIter.next();
      nodeConnList.appendChild(connNode);
    }
  }

  /*
  * Adds a connection grp to the SessionInfo.
  * Typically only one of src or dst Entity Id is provided. (the other is added via updateConnGroup).
  * @param {String} srcEntityId
  * @param {String} dstEntityId
  * @return {String} connGrpId
  */
  SessionInfo_prototype.addConnGroup = function(srcEntityId, dstEntityId) {
    // setup a new conn grp id
    var connGrpId = SessionInfo.CONNGRP_IDPREFIX + getUUID();

    // add a new conn node - with this grp id
    if(!this._addConn(connGrpId, srcEntityId, dstEntityId)) {
      return null;
    }
    // check/update the SessionInfo type
    this._updateSessionType();

    // return the new conngrpid
    return connGrpId;
  }
  /*
  * Deletes a connection grp from the SessionInfo.
  * Deletes all connections that are part of this connGroup. Also cleans up assoc Entities
  * @param {String} connGrpId
  * @return {Boolean}
  */
  SessionInfo_prototype.delConnGroup = function(connGrpId) {
    // delete the RECORD connection
    this.updateConnGroupDelRec(connGrpId);
    // go thru all the conns in the same grp
    var iterConn = this.getConnIterByGrpId(connGrpId);
    while(iterConn.hasNext()) {
      var nodeConn = iterConn.next();
      var connId = nodeConn.getAttribute("id");
      // delete each conn
      if(!this._delConn(connId)) {
        return false;
      }
    }
    // check/update the SessionInfo type
    this._updateSessionType();

    return true;
  }
  /*
  * Updates the name of a connection grp in the SessionInfo.
  * Renames all connections that are part of this connGroup. 
  * @param {String} connGrpId
  * @param {String} name
  * @return {Boolean}
  */
  SessionInfo_prototype.updateConnGroupName = function(connGrpId, name) {
    // go thru all the conns in the same grp
    var iterConn = this.getConnIterByGrpId(connGrpId);
    while(iterConn.hasNext()) {
      var nodeConn = iterConn.next();
      nodeConn.setAttribute("name", name);
    }
    return true;
  }
  /*
  * Updates a the src of a connection grp in the SessionInfo.
  * Used to switch src for all connections of an existing connGroup.
  * @param {String} connGrpId
  * @param {String} srcEntityId
  * @return {Boolean}
  */
  SessionInfo_prototype.updateConnGroupSrc = function(connGrpId, srcEntityId) {
    // go thru all the conns in the same grp
    var iterConn = this.getConnIterByGrpId(connGrpId);
    while(iterConn.hasNext()) {
      var nodeConn = iterConn.next();
      // replace src in this conn - if err return
      if(!this._updateConnNode(nodeConn, srcEntityId, null)) {
        return false;
      }
    }
    // check/update the SessionInfo type
    this._updateSessionType();

    return true;
  }
  /*
  * Adds a conn dest to a connection grp in the SessionInfo.
  * Used to add dests (one connection for each dest) to an existing connGroup.
  * @param {String} connGrpId
  * @param {String} dstEntityId
  * @return {Boolean}
  */
  SessionInfo_prototype.updateConnGroupAddDst = function(connGrpId, dstEntityId) {
    var nodeConn;
    // check if duplicate - if so return
    if(this.selectSingleNode(".//Connection[@gid='" + connGrpId + "'" +
                                             " and @dstEntityId='" + dstEntityId + "']")) {
      return false;
    }
    // next look for a conn in the grp with a blank dest id
    nodeConn = this.selectSingleNode(".//Connection[@gid='" + connGrpId + "'" +
                                                        " and @dstEntityId='']");
    // if found - 
    if(nodeConn != null) {
      // put this dest in it
      if(!this._updateConnNode(nodeConn, null, dstEntityId)) {
        return false;
      }
    }
    // if not found - 
    else {
      // get the source entity for this conn grp from an existing conn
      var srcEntityId = this.selectSingleNode(".//Connection[@gid='" + connGrpId + "']").getAttribute("srcEntityId");
      // add a new conn node - with this src id and dst id
      if(!this._addConn(connGrpId, srcEntityId, dstEntityId)) {
        return false;
      }
    }
    return true;
  }
  /*
  * Deletes a conn dest from a connection grp in the SessionInfo.
  * Used to del dests (one connection for each dest) from an existing connGroup.
  * @param {String} connGrpId
  * @param {String} dstEntityId
  * @return {Boolean}
  */
  SessionInfo_prototype.updateConnGroupDelDst = function(connGrpId, dstEntityId) {
    // get the conn for this dst entity in this conn grp
    var nodeConn = this.selectSingleNode(".//Connection[@gid='" + connGrpId + "'" +
                                             " and @dstEntityId='" + dstEntityId + "']");
    if(!nodeConn) return false;
    var connId = nodeConn.getAttribute("id");
    // check the number of conn in the conn grp
    var connCount = (this.selectNodes(".//Connection[@gid='" + connGrpId + "']")).size();
    // if more than one - delte the whole connection
    // otherwise - just remove the dst - to preserve src info for this conngrp
    if(connCount > 1) {
      this._delConn(connId);
    }
    else {
      this._delConnDst(connId);
      nodeConn.setAttribute("type", SessionInfo.CONNTYPE_PARTIAL);
    }
    // return success
    return true;
  }
  /*
  * Adds a Recording connection (and the required dstEntity) to the SessionInfo.
  * Uses the srcEntity name to derive filename if recName is not gvien
  * @param {String} connGrpId
  * @param {Object} objFile (optional)
  * @return {String} connId
  */
  SessionInfo_prototype.updateConnGroupAddRec = function(connGrpId, objFile) {
    // get the source entity for this conn grp from an existing conn
    var srcEntityId = this.selectSingleNode(".//Connection[@gid='" + connGrpId + "']").getAttribute("srcEntityId");
    if(jsx3.util.strEmpty(srcEntityId)) {
      return null;
    }
    // setup to add a recfile entity
    var objEntity = new Object();
    objEntity.type = SessionInfo.ENTITYTYPE_RECFILE;

    if(objFile && !jsx3.util.strEmpty(objFile.dirNID)) {
      objEntity.dirNID = objFile.dirNID;
    }
    // use the DefaultRec dir if none specified
    if(objFile && (!(objEntity.dirId || objFile.dirName || objFile.dirNID))) {
      var objDir = this.getDefaultRecDir();
      if(objDir) {
        objEntity.dirId = objDir.id;
      }
    }
    // use the DefaultRec group file if none specified
    if(objFile && (!(objEntity.grpFileId || objFile.grpFileName || objFile.grpFileNID))) {
      var objGrpFile = this.getDefaultRecGrpFile();
      if(!objGrpFile) {
        var newObjGrpFile = new Object();
        newObjGrpFile.grpFileTitle = this.getName();;
        this.setDefaultRecGrpFile(newObjGrpFile);
        objGrpFile = this.getDefaultRecGrpFile();
      }
      objEntity.grpFileId = objGrpFile.id;
    }

    // set the fileName and entityName - use srcname as default
    var srcEntityName = this.getEntityName(srcEntityId);
    objEntity.fileTitle = (objFile && objFile.fileTitle) ? objFile.fileTitle : (objEntity.type + '_' + srcEntityName);
    objEntity.name = objEntity.fileTitle;
    // set the clipId
    objEntity.clipId = SessionInfo.ENTITYTYPE_RECFILE + "_" + getUUID();
    // set the streamtype - based on the src
    objEntity.streamType = this.getEntityStreamType(srcEntityId);

    // add to entity list
    var dstEntityId = this.addEntity(objEntity);
    if(!dstEntityId) return null;

    //    // add a new conn node - with this src id and dst id
    //    var connId = this._addConn(connGrpId, srcEntityId, dstEntityId);
    //    if( ! connId ) return null;
    //    
    //    // return the new conn id 
    //    return connId;

    // update the connection
    if(this.updateConnGroupAddDst(connGrpId, dstEntityId)) {
      return true;
    }
    return false;
  }
  /*
  * Deletes a Recording connection (and the required dstEntity) from the SessionInfo.
  * Deletes all the Record Connections and there respective Dest entity 
  * @param {String} connGrpId
  * @param {String} connId (optional)
  * @return {Boolean}
  */
  SessionInfo_prototype.updateConnGroupDelRec = function(connGrpId, connId) {
    // get a recording conn for this conn grp from an existing conn
    var iterConn;
    if(connId) {
      iterConn = this.selectNodeIterator(".//Connection[@id='" + connId + "']");
    }
    else {
      iterConn = this.selectNodeIterator(".//Connection[@gid='" + connGrpId + "' and @type='" + SessionInfo.CONNTYPE_RECORD + "']");
    }
    if(!iterConn.hasNext()) return false;

    // get the number of connections for this connGrpId
    var connCount = (this.selectNodes(".//Connection[@gid='" + connGrpId + "']")).size();

    // if more than 1 means there are other destinations also
    if(connCount > 1) {
      // delete the entity and the connection
      while(iterConn.hasNext()) {
        var nodeConn = iterConn.next();
        // delete the connection
        connId = nodeConn.getAttribute("id");
        var dstEntityId = nodeConn.getAttribute("dstEntityId");
        this._delConn(connId);
        // delete RecFile entity from the entitylist
        this.delEntity(dstEntityId);
      }
    }
    // if only 1 connection with type=RECORD then delete the entity and clear the dstentityid and set type=PARTIAL
    else {
      var nodeConn = this.selectSingleNode(".//Connection[@gid='" + connGrpId + "' and @type='" + SessionInfo.CONNTYPE_RECORD + "']");
      var connId = nodeConn.getAttribute("id");
      var dstEntityId = nodeConn.getAttribute("dstEntityId");
      this._delConnDst(connId);
      // delete RecFile entity from the entitylist
      this.delEntity(dstEntityId);
      nodeConn.setAttribute("type", SessionInfo.CONNTYPE_PARTIAL);
    }


    // return success
    return true;
  }
  /*
  * Updates the title/NID info of the file/dir entity of a Recording connection from the SessionInfo.
  * @param {String} connGrpId
  * @param {String} connId (optional)
  * @param {String} fileTitle (optional)
  * @param {String} dirTitle (optional)
  * @param {String} dirNID (optional)
  * @return {Boolean}
  */
  SessionInfo_prototype.updateConnGroupRecInfo = function(connGrpId, connId, objFile) {
    // get a recording conn for this conn grp from an existing conn
    var nodeConn;
    if(connId) {
      nodeConn = this.selectSingleNode(".//Connection[@id='" + connId + "']");
    }
    else {
      nodeConn = this.selectSingleNode(".//Connection[@gid='" + connGrpId + "' and @type='" + SessionInfo.CONNTYPE_RECORD + "']");
    }
    if(!nodeConn) return false;

    // get the dest entity ID and update it
    var dstEntityId = nodeConn.getAttribute("dstEntityId");
    if(jsx3.util.strEmpty(dstEntityId)) return false;

    // use the file/dir info to update the dest entity    
    return this.updateEntity(dstEntityId, objFile);
  }

  /*
  * Deletes a connection from the SessionInfo.
  * Deletes Src and Dst Entities also - if not in use anymore
  * Typically not called directly. (called from add/updateConnGroup).
  * @param {String} connId
  * @return {Boolean}
  */
  SessionInfo_prototype._delConn = function(connId) {
    // get the connection node
    var nodeConn = this.selectSingleNode(".//Connection[@id='" + connId + "']");
    if(!nodeConn) return false;

    // del the src / dst entity for this conn
    this._delConnSrc(connId);
    this._delConnDst(connId);

    // delete it from its parent node
    nodeConn.getParent().removeChild(nodeConn);

    // return success
    return true;
  }
  /*
  * Deletes the src entity from a connection from the SessionInfo.
  * Deletes Src Entities also - if not in use anymore
  * Typically not called directly. (called from add/updateConnGroup).
  * @param {String} connId
  * @return {Boolean}
  */
  SessionInfo_prototype._delConnSrc = function(connId) {
    // get the connection node
    var nodeConn = this.selectSingleNode(".//Connection[@id='" + connId + "']");
    if(!nodeConn) return false;

    // get the src entity for this conn
    var srcEntityId = nodeConn.getAttribute("srcEntityId");

    // clear it 
    nodeConn.setAttribute("srcEntityId", "");

    // TBD: gc the entity

    // return success
    return true;
  }
  /*
  * Deletes the dst entity from a connection from the SessionInfo.
  * Deletes Dst Entities also - if not in use anymore
  * Typically not called directly. (called from add/updateConnGroup).
  * @param {String} connId
  * @return {Boolean}
  */
  SessionInfo_prototype._delConnDst = function(connId) {
    // get the connection node
    var nodeConn = this.selectSingleNode(".//Connection[@id='" + connId + "']");
    if(!nodeConn) return false;

    // get the dst entity for this conn
    var dstEntityId = nodeConn.getAttribute("dstEntityId");

    // clear it 
    nodeConn.setAttribute("dstEntityId", "");

    // TBD: gc the entity

    // return success
    return true;
  }
  /*
  * Updates a connection in the SessionInfo.
  * Typically not called directly.
  * @param {String} connId
  * @param {String} srcEntityId
  * @param {String} dstEntityId
  * @return {Boolean}
  */
  SessionInfo_prototype._updateConn = function(connId, srcEntityId, dstEntityId) {
    // get the connection node
    var nodeConn = this.selectSingleNode(".//Connection[@id='" + connId + "']");
    if(!nodeConn) return false;
    // setup values in this conn - if err return
    if(!this._updateConnNode(nodeConn, srcEntityId, dstEntityId)) {
      return false;
    }
    // return success
    return true;
  }
  /** @private @jsxobf-clobber
  * Sets values in a connection node
  * Not called directly. (called from add/updateConnection).
  * @param {Node} nodeConn
  * @param {String} srcEntityId
  * @param {String} dstEntityId
  * @return {Boolean}
  */
  SessionInfo_prototype._updateConnNode = function(nodeConn, srcEntityId, dstEntityId) {
    var srcEntityIdFlag = false;
    if(!jsx3.util.strEmpty(srcEntityId)) {
      srcEntityIdFlag = true;
    }
    else {
      srcEntityId = nodeConn.getAttribute("srcEntityId");
      if(!jsx3.util.strEmpty(srcEntityId)) {
        srcEntityIdFlag = true;
      }
    }

    var dstEntityIdFlag = false;
    if(!jsx3.util.strEmpty(dstEntityId)) {
      dstEntityIdFlag = true;
    }
    else {
      dstEntityId = nodeConn.getAttribute("dstEntityId");
      if(!jsx3.util.strEmpty(dstEntityId)) {
        dstEntityIdFlag = true;
      }
    }

    // if both src and dst available - check for stream type compatibility
    if((!jsx3.util.strEmpty(srcEntityId)) &&
        (!jsx3.util.strEmpty(dstEntityId))) {
      if(!this._isCompatibleStreamType(srcEntityId, dstEntityId)) {
        return false;
      }
    }

    // if src specified - 
    if(srcEntityIdFlag) {
      // set srcEntityId
      nodeConn.setAttribute("srcEntityId", srcEntityId);
      // set connName if needed - based on srcEntityName
      var connName = nodeConn.getAttribute("name");
      if(jsx3.util.strEmpty(connName)) {
        var srcEntityName = this.getEntityName(srcEntityId);
        if(!jsx3.util.strEmpty(srcEntityName)) {
          //connName = SessionInfo.CONN_IDPREFIX + srcEntityName;
          connName = srcEntityName;
        }
        else {
          connName = nodeConn.getAttribute("id");
        }
        nodeConn.setAttribute("name", connName);
      }
      // set the conn streamtype
      var srcEntityStreamType = this.getEntityStreamType(srcEntityId);
      nodeConn.setAttribute("streamType", srcEntityStreamType);
    }

    // if dst specified - 
    if(dstEntityIdFlag) {
      // set dstEntityId
      nodeConn.setAttribute("dstEntityId", dstEntityId);
    }

    // set the connection type when both src and dest entity are present
    if(srcEntityIdFlag && dstEntityIdFlag) {
      var connType = "";
      // set the connection type : if the src entity type of plyfile
      var srcEntityType = this.getEntityType(srcEntityId);
      var dstEntityType = this.getEntityType(dstEntityId);

      if(srcEntityType == SessionInfo.ENTITYTYPE_PLYFILE) {
        connType = SessionInfo.CONNTYPE_PLAYBACK;
      }
      else if(dstEntityType == SessionInfo.ENTITYTYPE_DSTPORT) {
        connType = SessionInfo.CONNTYPE_VIEW;
      }
      else if(dstEntityType == SessionInfo.ENTITYTYPE_RECFILE) {
        connType = SessionInfo.CONNTYPE_RECORD;
      }
      else if(dstEntityType == SessionInfo.ENTITYTYPE_PVRFILE) {
        connType = SessionInfo.CONNTYPE_PVR;
      }

      nodeConn.setAttribute("type", connType);
    }

    // return success
    return true;
  }
  /*
  * Validates streamType compatability between src and dst entityID
  * Rules for passing:
  * 1)dst is of type file OR
  * 2)destination port stream type is same as source OR 
  * 2)destination port stream type is "MULTI" but source is not "UDP"
  * @param {String} srcEntityId
  * @param {String} dstEntityId
  * @return {String} connGrpId
  */
  SessionInfo_prototype._isCompatibleStreamType = function(srcEntityId, dstEntityId) {
    var srcEntityStreamType = this.getEntityStreamType(srcEntityId);
    var dstEntityStreamType = this.getEntityStreamType(dstEntityId);
    var dstEntityType = this.getEntityType(dstEntityId);
    var dstEntityStreamTypeSplit = dstEntityStreamType.split(" ");
    if((dstEntityType == SessionInfo.ENTITYTYPE_RECFILE) ||
        (dstEntityStreamTypeSplit[0] == srcEntityStreamType) ||
        (dstEntityStreamTypeSplit[1] == srcEntityStreamType) ||
        (dstEntityStreamTypeSplit[2] == srcEntityStreamType) ||
        ((dstEntityStreamType == SessionInfo.ENTITYSTREAMTYPE_MULTI) &&
         (srcEntityStreamType != SessionInfo.ENTITYSTREAMTYPE_UDP))) {
      return true;
    }
    else {
      return false;
    }
  }
  /*
  * Validates destEntityID is being used in any connection
  * Rules for passing:
  * 1)destination type is "MULTI" 
  * 2) destination not in use in any connection
  * @param {String} dstEntityId
  */
  SessionInfo_prototype.isDestEntityAvailable = function(dstEntityId) {
    var dstEntityStreamType = this.getEntityStreamType(dstEntityId);
    if((dstEntityStreamType == SessionInfo.ENTITYSTREAMTYPE_MULTI)) {
      return true;
    }

    var nodeConn = this.selectSingleNode(".//Connection[@dstEntityId='" + dstEntityId + "']");
    if(!jsx3.util.strEmpty(nodeConn)) return false;

    return true;

  }
  /*
  * Validates srcEntityID is being used in any connection
  * Rules for passing:
  * 1) source not in use in any connection
  * @param {String} srcEntityId
  */
  SessionInfo_prototype.isSrcEntityAvailable = function(srcEntityId) {
    var srcEntityStreamType = this.getEntityStreamType(srcEntityId);

    var nodeConn = this.selectSingleNode(".//Connection[@srcEntityId='" + srcEntityId + "']");
    if(!jsx3.util.strEmpty(nodeConn)) return false;

    return true;

  }
  /*
  * Gets an Iterator over the Connections
  * 
  * @return {jsx3.util.Iterator} Connection Nodes
  */
  SessionInfo_prototype.getConnIter = function() {
    return this.selectNodeIterator(".//Connection");
  }
  /****** FUNCTION DEPRECATED *******/
  /*
  * Gets an Iterator over the Connections of type=STREAM
  * 
  * @return {jsx3.util.Iterator} Connection Nodes
  */
  SessionInfo_prototype.getConnStreamIter = function() {
    return this.selectNodeIterator(".//Connection[@type='STREAM']");
  }

  /*
  * Gets an Iterator over the Connections of type=VIEW
  * 
  * @return {jsx3.util.Iterator} Connection Nodes
  */
  SessionInfo_prototype.getConnViewIter = function() {
    return this.selectNodeIterator(".//Connection[@type='VIEW' or @type='STREAM' or @type='PLAYBACK' or @type='PARTIAL' or @type='RECORD']");
  }
  /*
  * Gets an Iterator over the Connections of the specified connGrpId
  * @param connGrpId {String}
  * @return {jsx3.util.Iterator} Connection Nodes
  */
  SessionInfo_prototype.getConnIterByGrpId = function(connGrpId) {
    return this.selectNodeIterator(".//Connection[@gid='" + connGrpId + "']");
  }
  /*
  * Gets an Iterator over the Connections of the specified dstEntityId
  * @param dstEntityId {String}
  * @return {jsx3.util.Iterator} Connection Nodes
  */
  SessionInfo_prototype.getConnIterByDstEntityId = function(dstEntityId) {
    return this.selectNodeIterator(".//Connection[@dstEntityId='" + dstEntityId + "']");
  }
  /*
  * Gets the Connection of the specified connGrpId
  * @param connGrpId {String}
  * @return {jsx3.util.Iterator} Connection Nodes
  */
  SessionInfo_prototype.getConnByGrpId = function(connGrpId) {
    return this.selectSingleNode(".//Connection[@gid='" + connGrpId + "']");
  }
  /*
  * Gets the Record Connection of the specified connGrpId
  * @param connGrpId {String}
  * @return {jsx3.util.Iterator} Connection Nodes
  */
  SessionInfo_prototype.getRecordConnByGrpId = function(connGrpId) {
    return this.selectSingleNode(".//Connection[@gid='" + connGrpId + "' and @type='RECORD']");
  }
  /*
  * Gets an Iterator over the Connections of the specified type
  * @param connType {String|List} of CONNTYPEs
  * @return {jsx3.util.Iterator} Connection Nodes
  */
  SessionInfo_prototype.getConnIterByType = function(connType) {
    var whereXPath = "";
    if(connType instanceof jsx3.util.List) {
      var iterConnType = connType.iterator();
      while(iterConnType.hasNext()) {
        if(!jsx3.util.strEmpty(whereXPath)) {
          whereXPath += " or ";
        }
        whereXPath += "@type='" + iterConnType.next() + "'";
      }
    }
    else {
      whereXPath = "@type='" + connType + "'";
    }
    return this.selectNodeIterator(".//Connection[" + whereXPath + "]");
  }
  /*
  * Gets the id of an connection
  * @ param nodeConn
  * @return {String}
  */
  SessionInfo_prototype.getConnId = function(nodeConn) {
    return nodeConn.getAttribute("id");
  }

  /*
  * Gets the sessionid of an pvr connection
  * @ param pvrnodeConn
  * @return {String}
  */
  SessionInfo_prototype.getShareId = function(nodeConn) {
    return nodeConn.getAttribute("sessionId");
  }

  /*
  * Gets the sharedid of an pvr connection
  * @ param pvrnodeConn
  * @return {String}
  */
  SessionInfo_prototype.getTrimLength = function(nodeConn) {
    return nodeConn.getAttribute("trimLength");
  }

  /*
  * Gets the sharedid of an pvr connection
  * @ param pvrnodeConn
  * @return {String}
  */
  SessionInfo_prototype.getStartTC = function(nodeConn) {
    return nodeConn.getAttribute("startTC");
  }

  /*
  * Gets the gid of an connection
  * @ param nodeConn
  * @return {String}
  */
  SessionInfo_prototype.getConnGrpId = function(nodeConn) {
    return nodeConn.getAttribute("gid");
  }
  /*
  * Gets the name of an connection
  * @ param nodeConn
  * @return {String}
  */
  SessionInfo_prototype.getConnName = function(nodeConn) {
    return nodeConn.getAttribute("name");
  }
  /*
  * Gets the type of an connection
  * @ param nodeConn
  * @return {String}
  */
  SessionInfo_prototype.getConnType = function(nodeConn) {
    return nodeConn.getAttribute("type");
  }
  /*
  * Gets the srcEntityId of an connection
  * @ param nodeConn
  * @return {String}
  */
  SessionInfo_prototype.getConnSrcEntityId = function(nodeConn) {
    return nodeConn.getAttribute("srcEntityId");
  }
  /*
  * Gets the dstEntityId of an connection
  * @ param nodeConn
  * @return {String}
  */
  SessionInfo_prototype.getConnDstEntityId = function(nodeConn) {
    return nodeConn.getAttribute("dstEntityId");
  }
  /*
  * Gets the state of an connection
  * @ param nodeConn
  * @return {String}
  */
  SessionInfo_prototype.getConnState = function(nodeConn) {
    return nodeConn.getAttribute("state");
  }

  /*
  * Gets the error details of an connection
  * @ param nodeConn
  * @return {objError} JSON
  */
  SessionInfo_prototype.getConnError = function(nodeConn) {
    if(!jsx3.util.strEmpty(nodeConn.getAttribute("errorCode"))) {
      var objError = new Object();
      objError.errorCode = nodeConn.getAttribute("errorCode");
      objError.errorData = nodeConn.getAttribute("errorData");
      return objError;
    }
    return null;
  }

  /*
  * Gets connection by connId
  * @ param connId
  * @return {connection node}
  */
  SessionInfo_prototype.getConnById = function(connId) {
    // get the connection Node
    var nodeConn = this.selectSingleNode(".//Connection[@id='" + connId + "']");
    if(!nodeConn) return null;

    return nodeConn;
  }
  /*
  * Gets connection by srcEntityId, dstEntityId, windowId
  * @ param srcEntityId, dstEntityId, windowId
  * @return {connection node}
  */
  SessionInfo_prototype.getConn = function(srcEntityId, dstEntityId, windowId) {
    var whereXPath = "";
    if(srcEntityId) {
      whereXPath += "./@srcEntityId='" + srcEntityId + "'";
    }

    if(dstEntityId) {
      if(!jsx3.util.strEmpty(whereXPath)) whereXPath += " and ";
      whereXPath += "./@dstEntityId='" + dstEntityId + "'";
    }

    if(windowId) {
      if(!jsx3.util.strEmpty(whereXPath)) whereXPath += " and ";
      whereXPath += "./@windowId='" + windowId + "'";
    }
    // get the connection node
    var nodeConn = this.selectSingleNode(".//Connection[" + whereXPath + "]");
    // if not found return null
    if(!nodeConn) {
      return null;
    }
    return nodeConn;
  }
  /*
  * Gets the canvasId of an connection
  * @ param nodeConn
  * @return {String}
  */
  SessionInfo_prototype.getConnCanvasId = function(nodeConn) {
    return nodeConn.getAttribute("canvasId");
  }
  /*
  * Gets the windowId of an connection
  * @ param nodeConn
  * @return {String}
  */
  SessionInfo_prototype.getConnWindowId = function(nodeConn) {
    return nodeConn.getAttribute("windowId");
  }
  /*
  * Internal utility function to get new entity connection nodes
  * @ param connType (optional)
  * @return {jsx3.xml.Entity} nodeConn
  */
  SessionInfo_prototype._getNewConnectionNode = function(connType) {
    // get a template from the cache OR model/objectXml
    var nodeSession = SessionInfo_prototype._getSessionObjectXML();

    // set PARTIAL by default
    if(!connType) {
      connType = SessionInfo.CONNTYPE_PARTIAL;
    }
    // get a connection node
    var nodeConn = nodeSession.selectSingleNode(".//Connection").cloneNode(true);
    // clean out any connsegs
    nodeConn.removeChildren();
    // set the connType
    nodeConn.setAttribute("type", connType);

    return nodeConn;
  }
  /*
  * Adds a connection to the SessionInfo.
  * Typically not called directly. (called from add/updateConnGroup).
  * @param {String} connGrpId
  * @param {String} srcEntityId
  * @param {String} dstEntityId
  * @return {String} connId
  */
  SessionInfo_prototype._addConn = function(connGrpId, srcEntityId, dstEntityId) {
    // get a new connection node
    var nodeConn = SessionInfo_prototype._getNewConnectionNode();
    // setup a new conn id
    var connId = SessionInfo.CONN_IDPREFIX + getUUID();
    nodeConn.setAttribute("id", connId);
    // set the conn grp id
    nodeConn.setAttribute("gid", connGrpId);
    // setup values in this conn - if err return
    if(!this._updateConnNode(nodeConn, srcEntityId, dstEntityId)) {
      return null;
    }

    // if success - append the new node to the list  
    var nodeConnList = this.selectSingleNode(".//ConnectionList");
    nodeConnList.appendChild(nodeConn);

    // return the new conn id 
    return connId;
  }
  /*
  * Internal utility function to get get a template SessionConfig
  * Looks in the cache first - if not found inits it from model/objectXml
  * @return {jsx3.xml.Entity} nodeSessionConfig
  */
  SessionInfo_prototype._getSessionObjectXML = function() {
    // get template from cache
    var nodeSessionConfig = getObjectCacheDefaultNode("SessionConfig", "DEFAULT_SESSIONCONFIG_NID");
    if(nodeSessionConfig == null) {
      setObjectCacheDefaultXML("SessionConfig", "JSXAPPS/IPVS-Common/model/objectXml/SessionConfig.xml", "DEFAULT_SESSIONCONFIG_NID");
      nodeSessionConfig = getObjectCacheDefaultNode("SessionConfig", "DEFAULT_SESSIONCONFIG_NID");
    }
    return nodeSessionConfig;
  }
});
