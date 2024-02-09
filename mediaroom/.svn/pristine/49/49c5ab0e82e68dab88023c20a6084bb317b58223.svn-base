package com.ipvs.mediastoreservice.handler;

import java.util.List;

import org.apache.commons.lang.StringUtils;
import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.assetadminservice.handler.AMConst;
import com.ipvs.client.ServiceClient;
import com.ipvs.common.MRAuthorization;
import com.ipvs.common.MRConst;
import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.db.MRConfig;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRRequestHandlerI;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.mediaroomservice.datamodel.MediaRoomAuthorization;
import com.ipvs.mediaroomservice.datamodel.MediaRoomObject;
import com.ipvs.mediaroomservice.impl.ConfigStreamAgent;
import com.ipvs.mediaroomservice.impl.MediaRoomPEM;
import com.ipvs.mediastoreservice.impl.MediaStoreAdminServerAgent;
import com.ipvs.mediastoreservice.impl.MediaStoreDirectoryInfo;
import com.ipvs.mediastoreservice.impl.MediaStoreServerI;
import com.ipvs.mediastoreservice.impl.MediaStoreServiceConst;
import com.ipvs.mediastoreservice.impl.MediaStoreServiceExceptionCodes;
import com.ipvs.mediastoreservice.impl.MediaStoreServiceUtils;
import com.ipvs.mediastoreservice.impl.MediaVersionManager;
import com.ipvs.utils.Utils;


public class SynchMediaStoreDirRequestHandler implements MRRequestHandlerI {
    public MRAuthorization authorizeRequest(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI profileFactory)
        throws Exception {
        log.addTraceBegin(request);

        int authType = MediaRoomObject.ROOM_TYPEID;
        int role = MediaRoomPEM.VISITOR_ROLE;
        String token = "testCommonToken";
        String authority = "testAuthority";
        String roomPolicyNID = MRConst.NULL;
        String streamPolicyNID = MRConst.NULL;
        MediaRoomAuthorization auth = new MediaRoomAuthorization(MRConst.NULL, MRConst.NULL, authType, role, roomPolicyNID, streamPolicyNID, token, authority);
        log.addTraceEnd(request);

        return auth;
    }

    
    public Document getResponse(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI profileFactory)
        throws MRException, Exception {
        Document requestDoc = request.getMessage().getDocument();
        String storeDirName = MRXMLUtils.getValueXpath(requestDoc, "//" + MediaStoreServiceConst.DIR_NAME_EL_NAME, "");
        MediaStoreAdminServerAgent storeAdminAgent = MediaStoreAdminServerAgent.getMediaStoreAgent(request.getUserJID());
        MediaStoreDirectoryInfo storeDirInfo = storeAdminAgent.getMediaStoreDirectoryInfo(storeDirName);
        MediaStoreServerI storeServer = storeAdminAgent.getMediaStoreServer();
        String deviceNID = storeAdminAgent.getDeviceNID();
        String presence = MRXMLUtils.getValueXpath(requestDoc, "//" + MediaStoreServiceConst.STATE_EL_NAME, "");

        if (presence.equals(AMConst.AVAILABLE)) {
            handleDirAvailablePresence(deviceNID, storeDirInfo, storeServer, request, xmpp, log, db, resourceFactory, profileFactory);
        } else if (presence.equals(AMConst.UNAVAILABLE)) {
            handleDirUnAvailablePresence(storeDirInfo, request, xmpp, log, db, resourceFactory, profileFactory);
        } else if (presence.equals(MediaStoreServiceConst.DELETE)) {
            String mediaStorePortNID = storeAdminAgent.getMediaStorePortResourceNID();
            handleNASDeleted(mediaStorePortNID, storeDirName, request, xmpp, log, db, resourceFactory, profileFactory);
        } else if (presence.equals(MediaStoreServiceConst.STATE_UPDATE)) {
            handleStateUpdate(storeDirInfo, request, xmpp, log, db, resourceFactory, profileFactory);
        }

        return null;
    }
    

    private void handleStateUpdate(MediaStoreDirectoryInfo storeDirInfo, MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI profileFactory)
        throws MRException, Exception {
        Document mediaStreamDirDoc = MediaStoreServiceUtils.getMediaStreamDirDoc(storeDirInfo.getMediaStoreDirNID(), request, db, log);
        String dirState = MRXMLUtils.getValueXpath(mediaStreamDirDoc, "//" + AMConst.STATEFLAG, "");
        long diskFullThreshold = Utils.getLongValue(MRXMLUtils.getAttributeValueXpath(mediaStreamDirDoc,"//"+MediaStoreServiceConst.MEDIA_DIR_SPANNING, 
        		MediaStoreServiceConst.MEDIA_DIR_DSIK_FULL_THRESHOLD, ""), 0);
        storeDirInfo.setDiskFullThreshold(diskFullThreshold);

        //dir state is BUSY and the store state is busy means synching the dir  no need to do any action
        if (storeDirInfo.getMediaStoreState().equals(MediaStoreServiceConst.BUSY) && dirState.equals(MediaStoreServiceConst.BUSY)) {
            return;
        }

        //dir state is READY and the store state is busy means synching the dir  no need to do any action
        if (storeDirInfo.getMediaStoreState().equals(MediaStoreServiceConst.READY) && dirState.equals(MediaStoreServiceConst.READY)) {
            return;
        }

        handleDirStateMachine(dirState, mediaStreamDirDoc, storeDirInfo, request, xmpp, log, db, resourceFactory, profileFactory);
    }

    private void handleDirStateMachine(String dirState, Document mediaStreamDirDoc, MediaStoreDirectoryInfo storeDirInfo, MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI profileFactory)
        throws MRException, Exception {
        //start the synch process only when 
        if (dirState.equals(MediaStoreServiceConst.OFFLINE) || dirState.equals(MediaStoreServiceConst.ERROR)) {
            log.addInfo("Dir State Either OFFLINE or ERROR. Start the synch Process", MRLog.INFO, MRLog.NOTIFY);
            MRXMLUtils.setAttributeValueXPath(mediaStreamDirDoc, "//" + MediaStoreServiceConst.MEDIA_STORE_PORT_RESOURCE_NID_LIST_EL_NAME, MediaStoreServiceConst.SYNCH_MEDIA_STORE_PORT_RESOURCE_NID_EL_NAME, storeDirInfo.getMediaStorePortResourceNID());
            MediaStoreServiceUtils.updateMediaStreamDirResourceState(mediaStreamDirDoc, MediaStoreServiceConst.BUSY, request, xmpp, log, db, resourceFactory, profileFactory);
            storeDirInfo.setMediaStoreState(MediaStoreServiceConst.START_SYNCH);
        } else {
        	storeDirInfo.setMediaStoreState(dirState);
        }
    }

    private void handleNASDeleted(String mediaStorePortNID, String storeDirName, MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI profileFactory)
        throws MRException, Exception {
        String xPath = "//" + MediaStoreServiceConst.MEDIA_STORE_PORT_RESOURCE_NID_EL_NAME + " [.='" + mediaStorePortNID + "' " + "and @" + MediaStoreServiceConst.DEVICE_ELEMENT_ID_ATTRIBUTE_NAME + "='" + storeDirName + "'" + "]";
        Element mediaStreamDirEL = MediaStoreServiceUtils.getMediaStreamDirFromDataBase(xPath, request, xmpp, log, db, resourceFactory, profileFactory);

        if (mediaStreamDirEL == null) {
            return;
        }

        Document mediaStreamDirDoc = MRXMLUtils.elementToDocument(mediaStreamDirEL);
        Element mediaStorePortEL = MRXMLUtils.getElementXpath(mediaStreamDirDoc, xPath);
        mediaStorePortEL.detach();

        MediaStoreServiceUtils.updateDirState(mediaStreamDirDoc);

        MediaStoreServiceUtils.updateMediaStreamDirDocInDB(mediaStreamDirDoc, request, xmpp, log, db, resourceFactory, profileFactory);
        log.addInfo("Delete Media Store Entry from Dir.", MRLog.INFO, MRLog.NOTIFY);
    }

    private void handleDirUnAvailablePresence(MediaStoreDirectoryInfo storeDirInfo, MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI profileFactory)
        throws MRException, Exception {
        Document mediaStreamDirDoc = MediaStoreServiceUtils.getMediaStreamDirDoc(storeDirInfo.getMediaStoreDirNID(), request, db, log);
        updatePresenceUnavailableDir(storeDirInfo, mediaStreamDirDoc, request, xmpp, log, db, resourceFactory, profileFactory);
    }

    private void handleDirAvailablePresence(String deviceNID, MediaStoreDirectoryInfo storeDirInfo, MediaStoreServerI storeServer, MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI profileFactory)
        throws MRException, Exception {
	String currentServiceVersion = MediaVersionManager.getServiceVersion(log);
        String dirServiceVersion = storeServer.getMediaStoreDirServiceVersion(currentServiceVersion, storeDirInfo.getMediaStoreRootPath());
        storeDirInfo.setDirServiceVer(dirServiceVersion);

        if (!MediaVersionManager.equals(dirServiceVersion,log)) {
        	log.addInfo("ServiceVersion Mismatch. StoreServiceVersion:="+dirServiceVersion +" CurrentServiceVersion:="+currentServiceVersion);
            storeDirInfo.setWritable(false);
        }

        //get the dir UUID from the drive.
        String dirUUID = storeServer.getMediaStoreDirUUID(storeDirInfo.getMediaStoreRootPath());
        Element dirElement = null;

        if ((dirUUID == null) && storeDirInfo.isReadOnly()) {
            throw new MRException(MediaStoreServiceExceptionCodes.NO_WRITE_PERMISSIONS, "Can not Write to Disk. Read Only System");
        }

        //if the dir does not contain the UUID then Create UUID file in the directory
        if (dirUUID == null) {
            if (storeDirInfo.isWritable()) {
                dirUUID = Utils.getUUID();
                storeServer.setupMediaStoreDir(storeDirInfo.getMediaStoreRootPath(), dirUUID);
            } else {
                int index = deviceNID.lastIndexOf("_");

                if (index < 0) {
                    throw new MRException(MediaStoreServiceExceptionCodes.NO_DIR_UUID_FOUND, "No Dir UUID Found");
                } else {
                    dirUUID = deviceNID.substring(index + 1);
                }
            }
        }

        //read the dir based on dir UUID
        String whereXpath = "//" + AMConst.MEDIA_STREAM_DIR_RESOURCE + "/" + MediaStoreServiceConst.INFO_ELEMENT_NAME + "[" + MediaStoreServiceConst.UUID + " ='" + dirUUID + "']";
        dirElement = MediaStoreServiceUtils.getMediaStreamDirFromDataBase(whereXpath, request, xmpp, log, db, resourceFactory, profileFactory);
        storeDirInfo.setDirUUID(dirUUID);

        String dirNID = null;
        Element pvrDirElement = null;
        Element recycBinElement = null;

        // if the directory is not there in the database add that directory. Create default two sub directories in database
        // one is RECYCLEBIN and the other one is PVR dir.
        String exportFilePath =  storeDirInfo.getMediaStoreRootPath();
		String exportFile  = storeServer.getExportFile(exportFilePath);
		String nid = null;
		String state = MediaStoreServiceConst.READY;
		
		String parentDirPath = exportFilePath + "/" + MediaStoreServiceConst.MEDIA_STORE_BLOB_DIR;
		String [] mediaList = storeServer.getListOfMediaStoreFiles(parentDirPath);
        if (dirElement == null) {
        	if(mediaList==null || mediaList.length == 0) {
        		state = MediaStoreServiceConst.READY;
        		if(exportFile!=null) {
        			log.addInfo("Found tgz file but did not find media. Deleting tgz file...." +  exportFilePath +  exportFile, MRLog.ERROR, MRLog.ERROR);
        			System.out.println("Found tgz file but did not find media. Deleting tgz file" + exportFilePath +  exportFile);
        			storeServer.deleteFile(exportFilePath + exportFile);
        		}
        		
        	} else {
        		if(storeDirInfo.isReadOnly()) {
        			state = MediaStoreServiceConst.READY;
        		} else if (exportFile!=null) {
        			state = MediaStoreServiceConst.DETACHED;
       				nid = exportFile.replace(".tar", "").trim();
       				// System.out.println("SyncMediaStoreDirRequestHandler:handleDirAvailablePresence dirNID=" + nid);
        		} else {
        			state = MediaStoreServiceConst.START_SYNCH;
        		}
        	}
        	storeDirInfo.setMediaStoreState(state);
        	String dirTitle = storeDirInfo.getStoreName();
        	if(storeDirInfo.isInternal()) {
        		dirTitle= getDirTitle(request, xmpp, log, db, resourceFactory, profileFactory,storeDirInfo.getStoreName(),storeDirInfo.getMediaStorePortResourceNID());
        	}
            dirElement = createMediaStreamDirResourceInDataBase(true, storeDirInfo, dirTitle, MediaStoreServiceConst.DEFAULT_DIR, nid,  state, request, xmpp, log, db, resourceFactory, profileFactory);
            dirNID = MRXMLUtils.getAttributeValue(dirElement, MediaStoreServiceConst.NID, "");
            recycBinElement = createMediaStreamDirResourceInDataBase(false, storeDirInfo, MediaStoreServiceConst.RECYCLE_BIN_DIR_NAME, dirNID, null, MediaStoreServiceConst.READY, request, xmpp, log, db, resourceFactory, profileFactory);
            storeDirInfo.setRecycleBinNID(MRXMLUtils.getAttributeValue(recycBinElement, MediaStoreServiceConst.NID, ""));
            pvrDirElement = createMediaStreamDirResourceInDataBase(false, storeDirInfo, MediaStoreServiceConst.PVR_DIR_NAME, dirNID, null, MediaStoreServiceConst.READY,  request, xmpp, log, db, resourceFactory, profileFactory);
            storeDirInfo.setPvrDirNID(MRXMLUtils.getAttributeValue(pvrDirElement, MediaStoreServiceConst.NID, ""));
        } else {
            dirNID = MRXMLUtils.getAttributeValue(dirElement, MediaStoreServiceConst.NID, "");

            Document mediaStreamDirDoc = MRXMLUtils.elementToDocument(dirElement);

            //update the dir with store port info
            mediaStreamDirDoc = updateMediaStoreInfoInDir(storeDirInfo, mediaStreamDirDoc, request, xmpp, log, db, resourceFactory, profileFactory);

            List<?> subDirList = MediaStoreServiceUtils.getSubDirList(dirNID, request, xmpp, log, db, resourceFactory, profileFactory);

            //If the dir contains the media store entry then update the state of the PVR dir and the dir state as BUSY (Synch process started).            	
            pvrDirElement = getSubDirElement(subDirList, MediaStoreServiceConst.PVR_DIR_NAME);
       
            if (pvrDirElement == null) {
                pvrDirElement = createMediaStreamDirResourceInDataBase(false, storeDirInfo, MediaStoreServiceConst.PVR_DIR_NAME, dirNID, null, MediaStoreServiceConst.READY, request,
                        xmpp, log, db, resourceFactory, profileFactory);
            }

            if (pvrDirElement != null) {
                storeDirInfo.setPvrDirNID(MRXMLUtils.getAttributeValue(pvrDirElement, MediaStoreServiceConst.NID, ""));
            }

            recycBinElement = getSubDirElement(subDirList, MediaStoreServiceConst.RECYCLE_BIN_DIR_NAME);

            if (recycBinElement == null) {
                recycBinElement = createMediaStreamDirResourceInDataBase(false, storeDirInfo, MediaStoreServiceConst.RECYCLE_BIN_DIR_NAME, dirNID, null,
                        MediaStoreServiceConst.READY, request, xmpp, log, db, resourceFactory, profileFactory);
            }

            if (recycBinElement != null) {
                storeDirInfo.setRecycleBinNID(MRXMLUtils.getAttributeValue(recycBinElement, MediaStoreServiceConst.NID, ""));
            }

            MRXMLUtils.setValueXpath(mediaStreamDirDoc, "//" + MediaStoreServiceConst.MEDIA_DIR_SIZE_USED, String.valueOf(storeDirInfo.getSizeUsed()));
            MRXMLUtils.setValueXpath(mediaStreamDirDoc, "//" + MediaStoreServiceConst.MEDIA_DIR_SIZE, String.valueOf(storeDirInfo.getSize()));

            String dirState = MRXMLUtils.getValueXpath(mediaStreamDirDoc, "//" + AMConst.STATEFLAG, "");
            
            //special case for restore database
            //when the dir state is not detached and tgz file is there in the blob dir then update the dir state to detached
            if(exportFile != null && !dirState.equals(MediaStoreServiceConst.DETACHED) && !(mediaList==null || mediaList.length == 0 )) {
                MediaStoreServiceUtils.updateMediaStreamDirResourceState(mediaStreamDirDoc, MediaStoreServiceConst.DETACHED, request, xmpp, log, db, resourceFactory, profileFactory);
                dirState = MediaStoreServiceConst.DETACHED;
            }
            else if(exportFile == null && dirState.equals(MediaStoreServiceConst.DETACHED)){
            	MediaStoreServiceUtils.updateMediaStreamDirResourceState(mediaStreamDirDoc, MediaStoreServiceConst.OFFLINE, request, xmpp, log, db, resourceFactory, profileFactory);
            	dirState = MediaStoreServiceConst.OFFLINE;
            }
            
            if (storeDirInfo.isReadOnly()) {
                storeDirInfo.setMediaStoreState(MediaStoreServiceConst.READY);
                // Set the dir state as ready on read only mode if the dir state is not busy.
                if (!MediaStoreServiceConst.BUSY.equalsIgnoreCase(dirState)) {
                    MediaStoreServiceUtils.updateMediaStreamDirResourceState(mediaStreamDirDoc, MediaStoreServiceConst.READY, request, xmpp, log, db, resourceFactory, profileFactory);
                }
            } else {
                handleDirStateMachine(dirState, mediaStreamDirDoc, storeDirInfo, request, xmpp, log, db, resourceFactory, profileFactory);
            }
            
        }
        storeDirInfo.setMediaStoreDirNID(dirNID);

        long diskFullThreshold = Utils.getLongValue(MRXMLUtils.getAttributeValueXpath(dirElement,"//"+MediaStoreServiceConst.MEDIA_DIR_SPANNING, 
        		MediaStoreServiceConst.MEDIA_DIR_DSIK_FULL_THRESHOLD, ""), 0);
        storeDirInfo.setDiskFullThreshold(diskFullThreshold);

    }

    private String getDirTitle(MRRequest request, XMPPI xmpp, MRLog log,MRDB db, MRResourceFactoryI resourceFactory,
    		ServiceAgentFactoryI profileFactory, String storeName, String mediaStorePortResourceNID) throws MRException,Exception {
    	/**
    	 * This code can be reused when synch code moved to appserver
    	 */
    	/*	String query = " let $storeNID := '_STORE_NID_'" +"" + 
		"	\n		let $portDoc :=   doc(concat('/db/ipvs.default.', "+ 
		"   \n                         substring-before( substring-after( $storeNID, 'default.') , '.'  ) , '/', " + 
		"   \n 							substring-after( substring-after( $storeNID, 'default.'),'.')))/*[@NID=$storeNID] "+ 
		"   \n      let $assetDoc := for $asset in collection('ipvs.default.assetresourcelist')/*    | collection('ipvs.default.assetresourcelist')/*" +
		"   \n     						 where $asset//@NID = $portDoc//AssetResourceNID "+
		"   \n      				return $asset "+ 
		"   \n		return $assetDoc" ;

    	 query  = query.replace("_STORE_NID_",mediaStorePortResourceNID);
    	 String responseXML  = MRXMLDBUtils.readQuery(MRXMLDBUtils.TIME_OUT, request, log, db, query, new HashMap<String, String>(), null);
    	 if(responseXML ==null || responseXML.isEmpty()) {
    		 return "";
    	 }
    	 Document assetdoc = MRXMLUtils.stringToDocument(responseXML);
    	 String devicetype = MRXMLUtils.getValueXpath(assetdoc, "//"+AMConst.TYPE, "");
    	 String title = MRXMLUtils.getValueXpath(assetdoc, "//"+AMConst.TITLE, "");*/
    	
    	ConfigStreamAgent cfgAgent  = (ConfigStreamAgent)profileFactory.getServiceAgent(ConfigStreamAgent.class.getName());
    	String devicetype =cfgAgent.getDeviceType();
    	String title = cfgAgent.getTitle();
    	if(devicetype.equals(AMConst.DMS_1500_DEVICE_OBJECTYPE)){
    		String index = StringUtils.substringAfter(storeName, "/data");
    		if(index != null && !index.isEmpty()) {
    			title+="_"+index;
    		}
    	}
    	return title;
    }


	//returns the given sub dir element for given dir nid.
    private Element getSubDirElement(List<?> subDirList, String subDirName)
        throws MRException, Exception {
        Element subDirEL = null;
        String title = null;

        for (Object object : subDirList) {
            subDirEL = (Element) object;

            title = MRXMLUtils.getValueXpath(MRXMLUtils.elementToDocument(subDirEL), "//" + MediaStoreServiceConst.MEDIA_TITLE_ELEMENT_NAME, "");

            if (subDirName.equals(title)) {
                return subDirEL;
            }
        }

        return null;
    }

    //update the media store info in given directory
    private Document updateMediaStoreInfoInDir(MediaStoreDirectoryInfo storeDirInfo, Document mediaStreamDirDoc, MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI profileFactory)
        throws MRException, Exception {
        Element mediaStorePortResourceNIDListEL = MRXMLUtils.getElementXpath(mediaStreamDirDoc, "//" + MediaStoreServiceConst.MEDIA_STORE_PORT_RESOURCE_NID_LIST_EL_NAME);

        //check store prot is there in the dir or not. If not add the element
        String xpath = "//" + MediaStoreServiceConst.MEDIA_STORE_PORT_RESOURCE_NID_EL_NAME + "[@" + MediaStoreServiceConst.DEVICE_ELEMENT_ID_ATTRIBUTE_NAME + "='" + storeDirInfo.getStoreDeviceElementID() + "' and .='" + storeDirInfo.getMediaStorePortResourceNID() + "']";
        Element mediaStorePortResourceNIDEL = MRXMLUtils.getElementXpath(mediaStreamDirDoc, xpath);
        int priority = MediaStoreServiceConst.DEFAULT_PRIORITY;
        if (mediaStorePortResourceNIDEL == null) {
        	 //if it is internal directory reset the title to new store port
            if(storeDirInfo.isInternal() && !mediaStorePortResourceNIDListEL.elements().isEmpty()) {
            	MRXMLUtils.setValueXpath(mediaStreamDirDoc, "//"+MediaStoreServiceConst.MEDIA_TITLE_ELEMENT_NAME, getDirTitle(request, xmpp, log, db, resourceFactory, profileFactory, storeDirInfo.getStoreName(), storeDirInfo.getMediaStorePortResourceNID()));
            }
            mediaStorePortResourceNIDEL = (Element) MRXMLUtils.getElementXpath(MRConfig.getObjectXMLDocument(ServiceClient.ASSETADMINSERVICE, AMConst.MEDIA_STREAM_DIR_RESOURCE, log), "//" + MediaStoreServiceConst.MEDIA_STORE_PORT_RESOURCE_NID_EL_NAME).clone();
        } else {
            mediaStorePortResourceNIDEL.detach();
            priority = Utils.getIntValue(MRXMLUtils.getAttributeValue(mediaStorePortResourceNIDEL, MediaStoreServiceConst.PRIORITY_ATTRIBUTE_NAME, ""),MediaStoreServiceConst.DEFAULT_PRIORITY);
        }

        updateStorePortInfo(storeDirInfo, mediaStorePortResourceNIDEL,priority);
        //if it is internal directory remove the content of List
        if(!mediaStorePortResourceNIDListEL.elements().isEmpty()) {
        	if(storeDirInfo.isInternal()) {
        		MRXMLUtils.removeContent(mediaStorePortResourceNIDListEL);
        	} else {
        		List<?> mediaStorePortResourceNIDList = MRXMLUtils.getListXpath(mediaStorePortResourceNIDListEL, "//MediaStorePortResourceNID[@presence='" + AMConst.UNAVAILABLE + "']");
        		if(mediaStorePortResourceNIDList != null && !mediaStorePortResourceNIDList.isEmpty()) {
        			for(Object obj:mediaStorePortResourceNIDList) {
        				Element mediaStorePortResourceNIDElement = (Element)obj;
        				mediaStorePortResourceNIDElement.detach();
        			}
        		}
        	}
        }
        MRXMLUtils.addElement(mediaStorePortResourceNIDListEL, mediaStorePortResourceNIDEL);
        
        MRXMLUtils.setValueXpath(mediaStreamDirDoc, "//" + MediaStoreServiceConst.MEDIA_DIR_SIZE_USED, String.valueOf(storeDirInfo.getSizeUsed()));

        if (storeDirInfo.isWritable()) {
            MRXMLUtils.setValueXpath(mediaStreamDirDoc, "//" + MediaStoreServiceConst.WRITE, MediaStoreServiceConst.TRUE);
        } else {
            MRXMLUtils.setValueXpath(mediaStreamDirDoc, "//" + MediaStoreServiceConst.WRITE, MediaStoreServiceConst.FALSE);
        }

        MediaStoreServiceUtils.updateMediaStreamDirDocInDB(mediaStreamDirDoc, request, xmpp, log, db, resourceFactory, profileFactory);
        log.addInfo("Update MediaStreamDirResoource With portNID:=" + storeDirInfo.getMediaStorePortResourceNID() + " storePortElementID :=" + storeDirInfo.getStoreDeviceElementID(), MRLog.INFO, MRLog.NOTIFY);

        return mediaStreamDirDoc;
    }

    private void updatePresenceUnavailableDir(MediaStoreDirectoryInfo storeDirInfo, Document mediaStreamDirDoc, MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI profileFactory)
        throws MRException, Exception {
        String xPath = "//" + MediaStoreServiceConst.MEDIA_STORE_PORT_RESOURCE_NID_EL_NAME + " [.='" + storeDirInfo.getMediaStorePortResourceNID() + "' " + "and @" + MediaStoreServiceConst.DEVICE_ELEMENT_ID_ATTRIBUTE_NAME + "='" + storeDirInfo.getStoreDeviceElementID() + "'" + "]";

        Element mediaStorePortEL = MRXMLUtils.getElementXpath(mediaStreamDirDoc, xPath);

        MRXMLUtils.setAttributeValue(mediaStorePortEL, MediaStoreServiceConst.PRESENCE_ATTRIBUTE_NAME, AMConst.UNAVAILABLE);

        MediaStoreServiceUtils.updateDirState(mediaStreamDirDoc);

        MediaStoreServiceUtils.updateMediaStreamDirDocInDB(mediaStreamDirDoc, request, xmpp, log, db, resourceFactory, profileFactory);
    }

    //Creates the media stream dir in the database.
    private Element createMediaStreamDirResourceInDataBase(boolean isParent, MediaStoreDirectoryInfo storeDirInfo, String title, String parentDirNID, String dirNID, String state, MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI profileFactory)
        throws MRException, Exception {
        Document mediaStreamDirDoc = MRConfig.getObjectXMLDocument(ServiceClient.ASSETADMINSERVICE, AMConst.MEDIA_STREAM_DIR_RESOURCE, log);

        if(dirNID != null) {
        	MRXMLUtils.setAttributeValue(mediaStreamDirDoc.getRootElement(), "NID", dirNID);
        }
        
        MRXMLUtils.setValueXpath(mediaStreamDirDoc, "//" + MediaStoreServiceConst.MEDIA_TITLE_ELEMENT_NAME, title);
        MRXMLUtils.setValueXpath(mediaStreamDirDoc, "//" + MediaStoreServiceConst.PARENT_DIR_NID_EL_NAME, parentDirNID);

        long xmpp_time = xmpp.getCurrentTimeMillis();
        MRXMLUtils.setValueXpath(mediaStreamDirDoc, "//" + MediaStoreServiceConst.DATE_CREATED, String.valueOf(xmpp_time));
        MRXMLUtils.setValueXpath(mediaStreamDirDoc, "//" + MediaStoreServiceConst.DATE_MODIFIED, String.valueOf(xmpp_time));

        MRXMLUtils.setValueXpath(mediaStreamDirDoc, "//" + MediaStoreServiceConst.OWNER_EL_NAME, request.getUserJID());

        MRXMLUtils.setValueXpath(mediaStreamDirDoc, "//" + MediaStoreServiceConst.READ, MediaStoreServiceConst.TRUE);
        MRXMLUtils.setValueXpath(mediaStreamDirDoc, "//" + MediaStoreServiceConst.XFER, MediaStoreServiceConst.TRUE);
        MRXMLUtils.setValueXpath(mediaStreamDirDoc, "//" + MediaStoreServiceConst.STREAM, MediaStoreServiceConst.TRUE);
        MRXMLUtils.setValueXpath(mediaStreamDirDoc, "//" + MediaStoreServiceConst.WRITE, MediaStoreServiceConst.TRUE);

        if (isParent) {
            if (!storeDirInfo.isWritable()) {
                MRXMLUtils.setValueXpath(mediaStreamDirDoc, "//" + MediaStoreServiceConst.WRITE, MediaStoreServiceConst.FALSE);
            }
            Element mediaStorePortResourceNIDEL = MRXMLUtils.getElementXpath(mediaStreamDirDoc, "//" + MediaStoreServiceConst.MEDIA_STORE_PORT_RESOURCE_NID_EL_NAME);
            updateStorePortInfo(storeDirInfo, mediaStorePortResourceNIDEL,MediaStoreServiceConst.DEFAULT_PRIORITY);
            MRXMLUtils.setValueXpath(mediaStreamDirDoc, "//" + MediaStoreServiceConst.UUID, storeDirInfo.getDirUUID());

            if (!storeDirInfo.isReadOnly()) {
            	//Set the synch media store port - based on review board comment from Padmaja
            	MRXMLUtils.setAttributeValue(MRXMLUtils.getElementXpath(mediaStreamDirDoc, "//" + MediaStoreServiceConst.MEDIA_STORE_PORT_RESOURCE_NID_LIST_EL_NAME), MediaStoreServiceConst.SYNCH_MEDIA_STORE_PORT_RESOURCE_NID_EL_NAME, storeDirInfo.getMediaStorePortResourceNID());
            }
            /*if (!storeDirInfo.isReadOnly()) {
                //Set the synch media store port
                MRXMLUtils.setAttributeValue(MRXMLUtils.getElementXpath(mediaStreamDirDoc, "//" + MediaStoreServiceConst.MEDIA_STORE_PORT_RESOURCE_NID_LIST_EL_NAME), MediaStoreServiceConst.SYNCH_MEDIA_STORE_PORT_RESOURCE_NID_EL_NAME, storeDirInfo.getMediaStorePortResourceNID());
                MRXMLUtils.setValueXpath(mediaStreamDirDoc, "//" + AMConst.STATE + "/" + AMConst.STATEFLAG, MediaStoreServiceConst.BUSY);
            } else {
                MRXMLUtils.setValueXpath(mediaStreamDirDoc, "//" + AMConst.STATE + "/" + AMConst.STATEFLAG, MediaStoreServiceConst.READY);
            }*/
            if(state.equals(MediaStoreServiceConst.START_SYNCH)){
            	state = MediaStoreServiceConst.BUSY;
            }
            MRXMLUtils.setValueXpath(mediaStreamDirDoc, "//" + AMConst.STATE + "/" + AMConst.STATEFLAG, state);
            
            //Set the disk full threshold as 10% of total disk space.
            long diskFullThreshold = (long) (storeDirInfo.getSize() * 0.1);
            
            MRXMLUtils.setValueXpath(mediaStreamDirDoc, "//" + MediaStoreServiceConst.MEDIA_DIR_SIZE, String.valueOf(storeDirInfo.getSize()));
            MRXMLUtils.setValueXpath(mediaStreamDirDoc, "//" + MediaStoreServiceConst.MEDIA_DIR_SIZE_USED, String.valueOf(storeDirInfo.getSizeUsed()));
            MRXMLUtils.setAttributeValueXPath(mediaStreamDirDoc, "//"+MediaStoreServiceConst.MEDIA_DIR_SPANNING, MediaStoreServiceConst.MEDIA_DIR_DSIK_FULL_THRESHOLD, String.valueOf(diskFullThreshold));     
            //MRXMLUtils.setValueXpath(mediaStreamDirDoc, "//" + MediaStoreServiceConst.MEDIA_DIR_DSIK_FULL_THRESHOLD, String.valueOf(diskFullThreshold));
            
        } else {        	
            MRXMLUtils.setValueXpath(mediaStreamDirDoc, "//" + AMConst.STATE + "/" + AMConst.STATEFLAG, MediaStoreServiceConst.READY);
            MRXMLUtils.setValueXpath(mediaStreamDirDoc, "//" + MediaStoreServiceConst.MEDIA_DIR_SIZE, String.valueOf(0));
            MRXMLUtils.setValueXpath(mediaStreamDirDoc, "//" + MediaStoreServiceConst.MEDIA_DIR_SIZE_USED, String.valueOf(0));
            MRXMLUtils.setAttributeValueXPath(mediaStreamDirDoc, "//"+MediaStoreServiceConst.MEDIA_DIR_SPANNING, MediaStoreServiceConst.MEDIA_DIR_DSIK_FULL_THRESHOLD, String.valueOf(0));  
           // MRXMLUtils.setValueXpath(mediaStreamDirDoc, "//" + MediaStoreServiceConst.MEDIA_DIR_DSIK_FULL_THRESHOLD, String.valueOf(0));
        }

        request.setSuperUser(true);
        log.addInfo("Create MediaStreamDirResource in Database.  DirName :=" + title, MRLog.INFO, MRLog.NOTIFY);

        return MediaStoreServiceUtils.addAssetAdminObjectInDB(AMConst.MEDIA_STREAM_DIR_RESOURCE, request, (Element)mediaStreamDirDoc.getRootElement().detach(), log, db, xmpp, resourceFactory, profileFactory);
    }

    // update the media store port entry in dir 
    private void updateStorePortInfo(MediaStoreDirectoryInfo storeDirInfo, Element mediaStorePortResourceNIDEL,int priority)
        throws MRException, Exception {
        MRXMLUtils.setValue(mediaStorePortResourceNIDEL, storeDirInfo.getMediaStorePortResourceNID());
        MRXMLUtils.setAttributeValue(mediaStorePortResourceNIDEL, MediaStoreServiceConst.DEVICE_ELEMENT_ID_ATTRIBUTE_NAME, storeDirInfo.getStoreDeviceElementID());
        MRXMLUtils.setAttributeValue(mediaStorePortResourceNIDEL, MediaStoreServiceConst.READ_ONLY_ATTRIBUTE_NAME, String.valueOf(storeDirInfo.isReadOnly()));
        MRXMLUtils.setAttributeValue(mediaStorePortResourceNIDEL, MediaStoreServiceConst.PRESENCE_ATTRIBUTE_NAME, AMConst.UNAVAILABLE);
        MRXMLUtils.setAttributeValue(mediaStorePortResourceNIDEL, MediaStoreServiceConst.PRIORITY_ATTRIBUTE_NAME, String.valueOf(priority));
    }
}
