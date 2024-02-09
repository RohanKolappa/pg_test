package com.ipvs.mediastoreservice.handler;

import java.util.HashMap;
import java.util.List;
import java.util.Set;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.assetadminservice.handler.AMConst;
import com.ipvs.assetadminservice.handler.GetMediaStreamFileMaxRevisionRequestHandler;
import com.ipvs.assetadminservice.impl.AssetAdminServiceUtils;
import com.ipvs.client.ServiceClient;
import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.db.MRConfig;
import com.ipvs.common.db.MRXMLDBUtils;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.AgentResponseHandlerI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRResponseHandlerI;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceConst;
import com.ipvs.mediastoreservice.impl.MediaStoreAdminServerAgent;
import com.ipvs.mediastoreservice.impl.MediaStoreDirectoryInfo;
import com.ipvs.mediastoreservice.impl.MediaStoreServerI;
import com.ipvs.mediastoreservice.impl.MediaStoreServiceConst;
import com.ipvs.mediastoreservice.impl.MediaStoreServiceExceptionCodes;
import com.ipvs.mediastoreservice.impl.MediaStoreServiceUtils;
import com.ipvs.mediastoreservice.impl.MediaVersionManager;
import com.ipvs.utils.Utils;


public class GetMediaStreamFileMaxRevisionResponseHandler implements AgentResponseHandlerI {
    public void handleAgentResponse(MRResponseHandlerI responseHandler, MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory)
        throws Exception {
        Document responseDoc = request.getResult();
        String mediaStoreRevision = null;
        String mediaStreamFileMaxRevision = null;
        String mediaStreamDirNID = null;
        MediaStoreDirectoryInfo storeInfo = null;

        try {
            MediaStoreServiceUtils.validateMediaStoreResponse(responseDoc, MediaStoreServiceExceptionCodes.GET_MEDIA_FILE_MAX_NUMBER_REQUEST_FAILED);
            mediaStoreRevision = MRXMLUtils.getValueXpath(responseDoc, "//" + GetMediaStreamFileMaxRevisionRequestHandler.MEDIA_STORE_REVISION_ELEMENT, "");
            mediaStreamFileMaxRevision = MRXMLUtils.getValueXpath(responseDoc, "//" + GetMediaStreamFileMaxRevisionRequestHandler.MEDIA_STREAM_FILE_MAX_REVISION_ELEMENT, "");
            mediaStreamDirNID = MRXMLUtils.getValueXpath(responseDoc, "//" + GetMediaStreamFileMaxRevisionRequestHandler.MEDIA_STREAM_DIR_NID, "");

            MediaStoreAdminServerAgent agent = MediaStoreAdminServerAgent.getMediaStoreAgent(request.getUserJID());

            storeInfo = agent.getMediaStoreInfoBasedOnDirNID(mediaStreamDirNID);

            int storeRevision = Utils.getIntValue(mediaStoreRevision, MediaStoreServiceConst.DEFAULT_REVISION);
            int streamFileMaxRevision = Utils.getIntValue(mediaStreamFileMaxRevision, 0);

            //if media store and media stream revision are equal then set the media store state as ready.
            if (storeInfo.isWritable() && (storeRevision == streamFileMaxRevision)) {
                repairBusyOrErrorFiles(null, storeInfo, request, xmpp, db, log, serviceAgentFactory, resourceFactory);
                synchMediaStreamWithMediaStoreFiles(agent, storeInfo, request, xmpp, db, log, serviceAgentFactory, resourceFactory);

                log.addInfo("Media Store and Media Stream are in synch.", MRLog.INFO, MRLog.NOTIFY);
                setMediaStoreState(MediaStoreServiceConst.READY, storeInfo, request, xmpp, log, db, resourceFactory, serviceAgentFactory);

                return;
            }

            log.addInfo("Media Store and Media Stream are out of synch", MRLog.INFO, MRLog.NOTIFY);

            List<?> mediaStreamFileList = MediaStoreServiceUtils.getListOfMediaStreamFilesInDir(mediaStreamDirNID, storeInfo.getPvrDirNID(),storeInfo.getRecycleBinNID(), request.getUserJID(), log, db);          
            HashMap<String, Document> mediaStoreAndStreamMapping = getMediaStoreAndMediaStreamMapping(mediaStreamFileList);
            syncMediaStoreWithMediaStreamFiles(mediaStoreAndStreamMapping, agent, storeInfo, streamFileMaxRevision, storeRevision, request, xmpp, db, log, serviceAgentFactory, resourceFactory);

            // If no files found then set the store revision to same as stream revision and set the state as ready.
            if (storeInfo.getNotSynchCount() == 0) {
                agent.getMediaStoreServer().updateMediaStoreInfo(storeInfo.getMediaStoreRootPath(), streamFileMaxRevision);
                log.addInfo("No Files Found to Synch. Update the media store revision with stream revision. MediaStoreRevision:=" + streamFileMaxRevision, MRLog.INFO, MRLog.NOTIFY);
            } else {
                // if files already syn'ed then set the not synch count as zero
                storeInfo.setNotSynchCount(0);
            }

            //Repair media stream files which are in busy state
            repairBusyOrErrorFiles(mediaStoreAndStreamMapping, storeInfo, request, xmpp, db, log, serviceAgentFactory, resourceFactory);

            //synch database with store
            synchMediaStreamWithMediaStoreFiles(agent, storeInfo, request, xmpp, db, log, serviceAgentFactory, resourceFactory);

            setMediaStoreState(MediaStoreServiceConst.READY, storeInfo, request, xmpp, log, db, resourceFactory, serviceAgentFactory);
        } catch (Exception exp) {
            exp.printStackTrace();

            if (storeInfo != null) {
                setMediaStoreState(MediaStoreServiceConst.ERROR, storeInfo, request, xmpp, log, db, resourceFactory, serviceAgentFactory);
            }
        }
    }

    //compares list of files in db and store. If db has some entries that are not there in store then it will delete those files.
    private void synchMediaStreamWithMediaStoreFiles(MediaStoreAdminServerAgent agent, MediaStoreDirectoryInfo storeInfo, MRRequest request, XMPPI xmpp, MRDB db, MRLog log, ServiceAgentFactoryI serviceAgentFactory, MRResourceFactoryI resourceFactory)
        throws MRException, Exception {
        String parentDirPath = storeInfo.getMediaStoreRootPath() + "/" + MediaStoreServiceConst.MEDIA_STORE_BLOB_DIR;

        String[] listOfMediaStoreFiles = agent.getMediaStoreServer().getListOfMediaStoreFiles(parentDirPath);

        synchMediaStreamWithMediaStoreFiles(listOfMediaStoreFiles, storeInfo.getMediaStoreDirNID(), request, xmpp, db, log, serviceAgentFactory, resourceFactory);
        synchMediaStreamWithMediaStoreFiles(listOfMediaStoreFiles, storeInfo.getPvrDirNID(), request, xmpp, db, log, serviceAgentFactory, resourceFactory);
        log.addInfo("Synch the Media Stream Files with blob.",MRLog.INFO,MRLog.NOTIFY);
    }

    private void synchMediaStreamWithMediaStoreFiles(String[] listOfMediaStoreFiles, String parentDirNID, MRRequest request, XMPPI xmpp, MRDB db, MRLog log, ServiceAgentFactoryI serviceAgentFactory, MRResourceFactoryI resourceFactory)
        throws MRException, Exception {
        List<?> mediaStreamFileList = MediaStoreServiceUtils.getMediaStreamFileResourceListWithoutGroupsAndPlaylists(parentDirNID, request.getUserJID(), log, db);

        if ((mediaStreamFileList == null) || mediaStreamFileList.isEmpty()) {
            return;
        }

        Document mediaStreamFileDocRoot = null;
        String mediaStoreFileUUID = null;

        for (Object object : mediaStreamFileList) {
        	try {
        		mediaStreamFileDocRoot = MRXMLUtils.elementToDocument((Element) object);
        		mediaStoreFileUUID = MRXMLUtils.getValueXpath(mediaStreamFileDocRoot, "//" + MediaStoreServiceConst.UUID, "");
        		if (!isMediaStorePresent(mediaStoreFileUUID, listOfMediaStoreFiles)) {
        			MediaStoreServiceUtils.synchDeleteMediaStreamFile(mediaStreamFileDocRoot, request.getUserJID(), log, db, xmpp);
        		}
        	}catch(Exception e) {
        		log.addInfo("Exception encountered when synchronizing file with MediaStoreFileNID:=" + mediaStoreFileUUID , MRLog.INFO, MRLog.NOTIFY);
                log.addInfo("Exception:=" + e.toString(), MRLog.INFO, MRLog.NOTIFY);
        	}
        }
    }

    private boolean isMediaStorePresent(String mediaStoreFileUUID, String[] listOfMediaStoreFiles) {
        if ((listOfMediaStoreFiles == null) || (listOfMediaStoreFiles.length == 0)) {
            return false;
        }

        for (String storeUUID : listOfMediaStoreFiles) {
            if (storeUUID.equals(mediaStoreFileUUID)) {
                return true;
            }
        }

        return false;
    }

    private void setMediaStoreState(String state, MediaStoreDirectoryInfo storeDirInfo, MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory)
        throws MRException, Exception {
        storeDirInfo.setMediaStoreState(state);
        MediaStoreServiceUtils.updateMediaStreamDirResourceState(storeDirInfo.getMediaStoreDirNID(), state, request, xmpp, log, db, resourceFactory, serviceAgentFactory);
    }

    public void synchTheBlobMedia(MediaStoreServerI storeServer, int mediaStreamFileRevision, HashMap<String, Document> mediaStoreAndStreamMapping, MediaStoreAdminServerAgent serverAgent, int storeRevision, MediaStoreDirectoryInfo storeInfo, Element dirGroupsEL, MRRequest request, XMPPI xmpp, MRDB db, MRLog log,
        ServiceAgentFactoryI serviceAgentFactory, MRResourceFactoryI resourceFactory, String userJID, Document mediaStreamFileObjectDoc)
        throws MRException, Exception {
        String rootDir = storeInfo.getMediaStoreRootPath();
        String parentDirPath = rootDir + "/" + MediaStoreServiceConst.MEDIA_STORE_BLOB_DIR;

        String[] mediaStoreFileArray = storeServer.getListOfMediaStoreFiles(parentDirPath);

        if ((mediaStoreFileArray == null) || (mediaStoreFileArray.length == 0)) {
            return;
        }

        Document mediaStoreFileDoc = null;
        int mediaStoreFileRevision = 0;
        String revisionValue = null;

        String mediaVersion = "0";
        MediaStoreServerI versionStoreServer = null;
        Document mediaStreamFileDoc = null;

        //this will synch the blob media
        for (String mediaStoreFileNID : mediaStoreFileArray) {
        	try {
        		mediaVersion = storeServer.getVersionOfMediaStoreFile(rootDir, mediaStoreFileNID);
        		versionStoreServer = serverAgent.getMediaStoreServer(mediaVersion);
        		mediaStoreFileDoc = versionStoreServer.getMediaStoreFileDoc(mediaStoreFileNID, rootDir);
        		
        		if ((mediaStoreFileDoc == null) || (mediaStoreFileDoc.getRootElement() == null)) {        			
        			continue;
        		}

        		revisionValue =MRXMLUtils.getAttributeValue( mediaStoreFileDoc.getRootElement(),MediaStoreServiceConst.REVISION);
        		mediaStoreFileRevision = Utils.getIntValue(revisionValue, storeRevision);
        		mediaStreamFileDoc = mediaStoreAndStreamMapping.get(mediaStoreFileNID);       		
        		
        		if ((mediaStoreFileRevision <= mediaStreamFileRevision) && MediaVersionManager.equals(mediaVersion,log) && (mediaStreamFileDoc!= null)) {        		
        			continue;
        		}       		
        		
        		boolean sendUpdateMediaGroupPlaylenghtRequest = true;
        		
        		//no file in the db add that file.
        		if (mediaStreamFileDoc == null) {        			
        			// if the media is current version update the blob with new revision.
        			if (MediaVersionManager.equals(mediaVersion,log)) {        				
        				versionStoreServer.updateSynMediaStoreFile(mediaStoreFileNID, versionStoreServer.getNewRevisionNumber(storeInfo.getMediaStoreRootPath()), mediaStoreFileDoc, storeInfo.getMediaStoreRootPath());
        			}
        			mediaStreamFileDoc = handleAddMediaStreamFile(versionStoreServer, mediaStoreFileDoc, mediaStreamFileObjectDoc, mediaStoreFileNID, storeInfo, dirGroupsEL, storeRevision, mediaStoreAndStreamMapping, userJID, log, db, xmpp, serviceAgentFactory, resourceFactory);
        			//sendUpdateMediaGroupPlaylenghtRequest = true;
         		} else if ((mediaStreamFileDoc != null) && (!MediaVersionManager.equals(mediaVersion,log))) {
        			if(!handleUpdatePermissionOfMediaStreamFile(versionStoreServer, mediaStoreFileDoc, mediaStreamFileDoc, mediaStoreFileNID, storeInfo, storeRevision, mediaStoreAndStreamMapping, userJID, log, db, xmpp, serviceAgentFactory, resourceFactory)) {
        				sendUpdateMediaGroupPlaylenghtRequest = false;
        			}
        		} else if ((mediaStreamFileDoc != null)) {
        			handleUpdateMediaStreamFile(mediaVersion, versionStoreServer, mediaStoreFileDoc, mediaStreamFileDoc, mediaStoreFileNID, storeInfo, dirGroupsEL, storeRevision, mediaStoreAndStreamMapping, userJID, log, db, xmpp, serviceAgentFactory, resourceFactory);
        		}
        		
        		//If media stream file contains media  group  id  and group does not exist in db send add media group request to appserver.
    			//If media group exist then send update playlength and size request to appserver 
    			if (!MediaVersionManager.is2dot4Media(mediaVersion) && !MediaVersionManager.is3dot2Media(mediaVersion)) {
    				mediaStreamFileDoc = MRXMLUtils.getDocumentXpath(mediaStreamFileDoc, "//" + AMConst.MEDIA_STREAM_FILE_RESOURCE);
    				String mediaGroupId = MRXMLUtils.getValueXpath(mediaStoreFileDoc, "//" + MediaStoreServiceConst.MEDIA_GROUP_ID_EL_NAME, "");
    				if(!mediaGroupId.isEmpty()) {
    					 Document mediaGroupDoc = MediaStoreServiceUtils.getMediaGroupFileDoc(mediaGroupId, userJID, log, db);
    					 String mediaStreamFileNID = MRXMLUtils.getAttributeValue(mediaStreamFileDoc.getRootElement(), MediaStoreServiceConst.NID);
    					 if(mediaGroupDoc == null) {        						 
    						 MediaStoreServiceUtils.sendAddMediaGroupFileFromMediaClipRequestToAppserver(mediaStreamFileNID, xmpp, db, log, userJID, serviceAgentFactory);
    					 } else  if(sendUpdateMediaGroupPlaylenghtRequest) {
    						 MediaStoreServiceUtils.sendUpdateMediaGroupPlayLenghtAndSizeRequesToAppserver(mediaStreamFileNID, xmpp, db, log, userJID, serviceAgentFactory);
    					 }
    				}
    			}
    			
        	}catch(Exception e) {
        		e.printStackTrace();
        		log.addInfo("Exception encountered when synchronizing file with MediaStoreFileNID:=" + mediaStoreFileNID, MRLog.INFO, MRLog.NOTIFY);
                log.addInfo("Exception:=" + e.toString(), MRLog.INFO, MRLog.NOTIFY);
                handleSyncException(storeServer, mediaStoreAndStreamMapping, storeInfo, dirGroupsEL,  mediaStoreFileNID,  request,  xmpp,  db,  log,
                serviceAgentFactory,  resourceFactory,  userJID,  mediaStreamFileObjectDoc, mediaStoreFileDoc, mediaVersion);
        	}
        }

        log.addInfo("Synch the blob Media", MRLog.INFO, MRLog.NOTIFY);
    }

    public void synchTheOlderMediaGroupsAndPlayLists(MediaStoreServerI storeServer, int mediaStreamFileRevision, HashMap<String, Document> mediaStoreAndStreamMapping, MediaStoreAdminServerAgent serverAgent, int storeRevision, MediaStoreDirectoryInfo storeInfo, Element dirGroupsEL, MRRequest request, XMPPI xmpp,
        MRDB db, MRLog log, ServiceAgentFactoryI serviceAgentFactory, MRResourceFactoryI resourceFactory, String userJID, Document mediaStreamFileObjectDoc)
        throws MRException, Exception {
        String rootDir = storeInfo.getMediaStoreRootPath();
        String parentDirPath = rootDir + "/" + MediaStoreServiceConst.MEDIA_STORE_DB_DIR;

        String[] mediaStoreFileArray = storeServer.getListOfMediaStoreFiles(parentDirPath);

        if ((mediaStoreFileArray == null) || (mediaStoreFileArray.length == 0)) {
            return;
        }

        Document mediaStoreFileDoc = null;
        MediaStoreServerI versionStoreServer = null;
        Document mediaStreamFileDoc = null;
        versionStoreServer = serverAgent.getMediaStoreServer(MediaVersionManager.MEDIA_STORE_VERSION_2_DOT_4);

        String mediaVersion = "0";

        for (String mediaStoreFileNID : mediaStoreFileArray) {
        	try {
        		mediaStoreFileDoc = versionStoreServer.getMediaStoreFileDoc(mediaStoreFileNID, rootDir);

        		mediaStreamFileDoc = mediaStoreAndStreamMapping.get(mediaStoreFileNID);

        		//no file in the db add that file.
        		if (mediaStreamFileDoc == null) {
        			handleAddMediaStreamFile(versionStoreServer, mediaStoreFileDoc, mediaStreamFileObjectDoc, mediaStoreFileNID, storeInfo, dirGroupsEL, storeRevision, mediaStoreAndStreamMapping, userJID, log, db, xmpp, serviceAgentFactory, resourceFactory);
        		} else {
        			mediaVersion = storeServer.getVersionOfMediaStoreFile(rootDir, mediaStoreFileNID);

        			// make sure file is already upgraded to latest version or not
        			if (MediaVersionManager.is2dot4Media(mediaVersion)) {        			    
        				handleUpdatePermissionOfMediaStreamFile(versionStoreServer, mediaStoreFileDoc, mediaStreamFileDoc, mediaStoreFileNID, storeInfo, storeRevision, mediaStoreAndStreamMapping, userJID, log, db, xmpp, serviceAgentFactory, resourceFactory);
        			}
        		}
        	} catch(Exception e) {
        		e.printStackTrace();
        		log.addInfo("Exception encountered when synchronizing file with MediaStoreFileNID:=" + mediaStoreFileNID, MRLog.INFO, MRLog.NOTIFY);
                log.addInfo("Exception:=" + e.toString(), MRLog.INFO, MRLog.NOTIFY);
                handleSyncException(storeServer, mediaStoreAndStreamMapping, storeInfo, dirGroupsEL,  mediaStoreFileNID,  request,  xmpp,  db,  log,
                serviceAgentFactory,  resourceFactory,  userJID,  mediaStreamFileObjectDoc, mediaStoreFileDoc, mediaVersion);
        	}
        		
        }

        log.addInfo("Synch the older Media Groups and PlayList", MRLog.INFO, MRLog.NOTIFY);
    }

    private void handleSyncException(MediaStoreServerI storeServer, HashMap<String, Document> mediaStoreAndStreamMapping, MediaStoreDirectoryInfo storeInfo, Element dirGroupsEL, String mediaStoreFileNID, MRRequest request, XMPPI xmpp, MRDB db, MRLog log,
            ServiceAgentFactoryI serviceAgentFactory, MRResourceFactoryI resourceFactory, String userJID, Document mediaStreamFileObjectDoc, Document mediaStoreFileDoc, String mediaVersion) throws Exception {
    	Document mediaStreamFileDoc = null;
    	String title = "SyncFailed";
        mediaStreamFileDoc = mediaStoreAndStreamMapping.get(mediaStoreFileNID);
        if(mediaStreamFileDoc == null) {
        	if(mediaStoreFileDoc!= null) {
        		if(MediaVersionManager.is2dot4Media(mediaVersion)) {        		    
        			title = MRXMLUtils.getValueXpath(mediaStoreFileDoc, "//TITLE", title);
        		} else {
        			title = MRXMLUtils.getValueXpath(mediaStoreFileDoc, "//Title", title);
        		}
        	} 
        	mediaStreamFileDoc = (Document)mediaStreamFileObjectDoc.clone();
        	MRXMLUtils.setValueXpath(mediaStreamFileDoc, "//UUID", mediaStoreFileNID);
        	        	MRXMLUtils.setValueXpath(mediaStreamFileDoc, "//Write", "true");
        	MRXMLUtils.setValueXpath(mediaStreamFileDoc, "//" + AMConst.STATEFLAG, MediaStoreServiceConst.ERROR);
        	long xmpp_date = xmpp.getCurrentTimeMillis();
            MRXMLUtils.setValueXpath(mediaStreamFileDoc, "//Info/Properties/DateCreated", String.valueOf(xmpp_date));
            MRXMLUtils.setValueXpath(mediaStreamFileDoc, "//Info/Properties/DateModified", String.valueOf(xmpp_date));
            MRXMLUtils.setValueXpath(mediaStreamFileDoc, "//"+MediaStoreServiceConst.PARENT_DIR_NID_EL_NAME,storeInfo.getMediaStoreDirNID());
            MRXMLUtils.setValueXpath(mediaStreamFileDoc, "//"+MediaStoreServiceConst.MEDIA_TITLE_ELEMENT_NAME,title);
        	MediaStoreServiceUtils.synchAddMediaStreamFile(mediaStreamFileDoc, mediaStoreFileNID, mediaStoreAndStreamMapping, storeInfo, userJID, log, db, xmpp, resourceFactory, serviceAgentFactory);
        } else {
        	String mediaStreamFileNID = MediaStoreServiceUtils.getMediaStreamFileNID(mediaStreamFileDoc);
        	mediaStreamFileDoc = MRXMLDBUtils.readElement(request, log, db, mediaStreamFileNID);
        	MRXMLUtils.setValueXpath(mediaStreamFileDoc, "//" + AMConst.STATEFLAG, MediaStoreServiceConst.ERROR);
        	MediaStoreServiceUtils.synchUpdateMediaStreamFile(mediaStreamFileDoc, mediaStoreFileNID, mediaStoreAndStreamMapping, storeInfo, userJID,  log,  db,  xmpp, serviceAgentFactory);
        }
    }
    
    // handle the update of permissions- If it is older media make sure file is in ready only mode
    private boolean handleUpdatePermissionOfMediaStreamFile(MediaStoreServerI versionStoreServer, Document mediaStoreFileDoc, Document mediaStreamFileDocRoot, String mediaStoreFileNID, MediaStoreDirectoryInfo storeInfo, int storeRevision, HashMap<String, Document> mediaStoreAndStreamMapping,
        String userJID, MRLog log, MRDB db, XMPPI xmpp, ServiceAgentFactoryI serviceAgentFactory, MRResourceFactoryI resourceFactory)
        throws MRException, Exception {
    	
   		String NID = MediaStoreServiceUtils.getMediaStreamFileNID(mediaStreamFileDocRoot); 	
    	Document mediaStreamFileDoc = MRXMLDBUtils.readElement(MRRequest.createRequest(userJID), log, db, NID);
    	String strWritevalue = MRXMLUtils.getValueXpath(mediaStreamFileDoc, "//" + MediaStoreServiceConst.PERMISSION_EL_NAME + "/" + MediaStoreServiceConst.WRITE, "");
    	if(strWritevalue.isEmpty()){
    		strWritevalue = String.valueOf(true);
    	}
        boolean writePermission = Utils.getBoolean(strWritevalue, true);      
        if (!writePermission) {
            return false;
        }

        String mediaType = MRXMLUtils.getValueXpath(mediaStreamFileDoc, "//" + MediaStoreServiceConst.MEDIA_TYPE_ELEMENT_NAME, "");

        if (mediaType.equals(MediaRoomServiceConst.MEDIAGROUP) || mediaType.equals(MediaRoomServiceConst.PLAYLIST)) {
            return false;
        }

        //MediaStoreServiceUtils.updateRevision(mediaStreamFileDoc, storeRevision);      
        int mediaStoreFileRevision = Utils.getIntValue(MRXMLUtils.getAttributeValue(mediaStoreFileDoc.getRootElement(), MediaStoreServiceConst.REVISION, ""), storeRevision);
        MediaStoreServiceUtils.updateRevision(mediaStreamFileDoc, mediaStoreFileRevision);

        MediaStoreServiceUtils.updateFilePermissions(mediaStreamFileDoc, MediaStoreServiceConst.WRITE, MediaStoreServiceConst.FALSE);

        String mediaStreamFileNID = MRXMLUtils.getAttributeValue(mediaStreamFileDoc.getRootElement(), MediaStoreServiceConst.NID, "");
        MediaStoreServiceUtils.updateMediaStreamFileDocInDB(mediaStreamFileDoc, mediaStreamFileNID, userJID, xmpp, log, db, serviceAgentFactory);
        log.addInfo("Update File Permission as Read Only. MediastoreFileNID :=" + mediaStoreFileNID, MRLog.INFO, MRLog.NOTIFY);
        return true;
    }

    // if the file already there in database and stream file and store file are out of synch then handle the update of media stream
    private void handleUpdateMediaStreamFile(String mediaVersion, MediaStoreServerI versionStoreServer, Document mediaStoreFileDoc, Document mediaStreamFileDocRoot, String mediaStoreFileNID, MediaStoreDirectoryInfo storeInfo, Element dirGroupsEL, int storeRevision,
        HashMap<String, Document> mediaStoreAndStreamMapping, String userJID, MRLog log, MRDB db, XMPPI xmpp, ServiceAgentFactoryI serviceAgentFactory, MRResourceFactoryI resourceFactory)
        throws MRException, Exception {
        String NID = MediaStoreServiceUtils.getMediaStreamFileNID(mediaStreamFileDocRoot);
    	Document mediaStreamFileDoc = MRXMLDBUtils.readElement(MRRequest.createRequest(userJID), log, db, NID); 
    	 if (MediaVersionManager.equals(mediaVersion,log)) {
         	  versionStoreServer.updateSynMediaStoreFile(mediaStoreFileNID, versionStoreServer.getNewRevisionNumber(storeInfo.getMediaStoreRootPath()), mediaStoreFileDoc, storeInfo.getMediaStoreRootPath());
         }
        Document newMediaStreamfileDoc = versionStoreServer.generateMediaStreamFileDocFromStoreFile(mediaStoreFileDoc, mediaStreamFileDoc, mediaStoreFileNID, storeInfo, dirGroupsEL, storeRevision, false, mediaStoreAndStreamMapping);
       

        MediaStoreServiceUtils.synchUpdateMediaStreamFile(newMediaStreamfileDoc, mediaStoreFileNID, mediaStoreAndStreamMapping, storeInfo, userJID, log, db, xmpp, serviceAgentFactory);
    }

    //handle the case where media stream file is not there in the data 
    private Document handleAddMediaStreamFile(MediaStoreServerI versionStoreServer, Document mediaStoreFileDoc, Document mediaStreamFileObjectXML, String mediaStoreFileNID, MediaStoreDirectoryInfo storeInfo, Element dirGroupsEL, int storeRevision, HashMap<String, Document> mediaStoreAndStreamMapping,
        String userJID, MRLog log, MRDB db, XMPPI xmpp, ServiceAgentFactoryI serviceAgentFactory, MRResourceFactoryI resourceFactory)
        throws MRException, Exception {
        Document mediaFileDoc = versionStoreServer.generateMediaStreamFileDocFromStoreFile(mediaStoreFileDoc, mediaStreamFileObjectXML, mediaStoreFileNID, storeInfo, dirGroupsEL, storeRevision, true, mediaStoreAndStreamMapping);

        if (mediaFileDoc == null) {
            return null;
        }

        MediaStoreServiceUtils.synchAddMediaFile(mediaFileDoc, mediaStoreFileNID, mediaStoreAndStreamMapping, storeInfo, userJID, log, db, xmpp, resourceFactory, serviceAgentFactory);
        
        return mediaFileDoc;
    }

    private void syncMediaStoreWithMediaStreamFiles(HashMap<String, Document> mediaStoreAndStreamMapping, MediaStoreAdminServerAgent serverAgent, MediaStoreDirectoryInfo storeInfo, int streamFileMaxRevision, int storeRevision, MRRequest request, XMPPI xmpp, MRDB db, MRLog log,
        ServiceAgentFactoryI serviceAgentFactory, MRResourceFactoryI resourceFactory)
        throws MRException, Exception {
        /* MediaStoreServerI mediaStoreServer = null;
         HashMap<String, Document> mediaStoreFileList = null;
        
         for (Integer mediaVersion : MediaStoreServiceConst.versionList) {
             mediaStoreServer = serverAgent.getMediaStoreServer(mediaVersion);
             mediaStoreFileList = mediaStoreServer.getMediaStoreFileListWhichOrNotInSynch(streamFileMaxRevision, storeInfo.getMediaStoreRootPath(), mediaStoreAndStreamMapping);
        
             if (mediaStoreFileList == null) {
                 continue;
             }
        
             mediaStoreServer.makeMediaStreamFileAndMediaStoreFileSynch(mediaStoreAndStreamMapping, mediaStoreFileList, storeRevision, storeInfo);
         }*/
        Document mediaStreamFileObjectDoc = MRConfig.getObjectXMLDocument(ServiceClient.ASSETADMINSERVICE, AMConst.MEDIA_STREAM_FILE_RESOURCE, log);

        MediaStoreServerI mediaStoreServer = serverAgent.getMediaStoreServer();
        Element dirGroupsEL = MediaStoreServiceUtils.getMediaStreamDirGroupsEL(storeInfo.getMediaStoreDirNID(), request, db, log);
        synchTheBlobMedia(mediaStoreServer, streamFileMaxRevision, mediaStoreAndStreamMapping, serverAgent, storeRevision, storeInfo, dirGroupsEL, request, xmpp, db, log, serviceAgentFactory, resourceFactory, request.getUserJID(), mediaStreamFileObjectDoc);
        synchTheOlderMediaGroupsAndPlayLists(mediaStoreServer, streamFileMaxRevision, mediaStoreAndStreamMapping, serverAgent, storeRevision, storeInfo, dirGroupsEL, request, xmpp, db, log, serviceAgentFactory, resourceFactory, request.getUserJID(), mediaStreamFileObjectDoc);
    }



    private void repairBusyOrErrorFiles(HashMap<String, Document> mediaStoreAndStreamMapping, MediaStoreDirectoryInfo storeInfo, MRRequest request, XMPPI xmpp, MRDB db, MRLog log, ServiceAgentFactoryI serviceAgentFactory, MRResourceFactoryI resourceFactory)
        throws MRException, Exception {
      
	List<?> mediaStreamFileList = MediaStoreServiceUtils.getBusyOrErrorFiles(storeInfo.getMediaStoreDirNID(),storeInfo.getPvrDirNID(),storeInfo.getMediaStoreDirNID(), request.getUserJID(), log, db);
        mediaStoreAndStreamMapping = getMediaStoreAndMediaStreamMapping(mediaStreamFileList);
      
        Set<String> mediaStoreFileNIDList = mediaStoreAndStreamMapping.keySet();
        Document mediaStreamFileDocRoot = null;
        String mediaStreamFileNID = null;
        String mediaStreamFileDirNID = null;
        Element mediaStreamFileEl = null;
        for (String mediaStoreFileNID : mediaStoreFileNIDList) {
        	try {
        		mediaStreamFileDocRoot = mediaStoreAndStreamMapping.get(mediaStoreFileNID);
        		mediaStreamFileNID = MediaStoreServiceUtils.getMediaStreamFileNID(mediaStreamFileDocRoot);
        		mediaStreamFileEl = AssetAdminServiceUtils.getChild(mediaStreamFileNID, "//"+MediaStoreServiceConst.PARENT_DIR_NID_EL_NAME, request, log, db, xmpp, resourceFactory, serviceAgentFactory);
        		mediaStreamFileDirNID = MRXMLUtils.getValueXpath(mediaStreamFileEl, ".//"+MediaStoreServiceConst.PARENT_DIR_NID_EL_NAME, "");
        		MediaStoreServiceUtils.callRepairMedia(mediaStreamFileNID, mediaStoreFileNID, mediaStreamFileDirNID, request, xmpp, db, log, resourceFactory, serviceAgentFactory);
        	} catch(Exception e) {
        		e.printStackTrace();
        		log.addInfo("Exception encountered when synchronizing file with MediaStoreFileNID:=" + mediaStoreFileNID + " , MediaStreamFileNID:= " + mediaStreamFileNID, MRLog.INFO, MRLog.NOTIFY);
                        log.addInfo("Exception:=" + e.toString(), MRLog.INFO, MRLog.NOTIFY);
                        if(mediaStreamFileEl!=null) {
                                Document mediaStreamFileDoc = MRXMLDBUtils.readElement(request, log, db, mediaStreamFileNID);                        
                        	MRXMLUtils.setValueXpath(mediaStreamFileDoc, "//" + AMConst.STATEFLAG, MediaStoreServiceConst.ERROR);
                        	MediaStoreServiceUtils.synchUpdateMediaStreamFile(mediaStreamFileDoc, mediaStoreFileNID, mediaStoreAndStreamMapping, storeInfo, request.getUserJID(),  log,  db,  xmpp, serviceAgentFactory);
                        }	
        	}
        }
        
        log.addInfo("Repair Media Stream File Which are in Busy/Error State.", MRLog.INFO, MRLog.NOTIFY);
        
    }
    
    public static String register2dot4MediaFile(String mediaStoreFileNID, String title, String mediaStreamDirNID, String userJID, 
            Element dirGroups, int storeRevision, MediaStoreDirectoryInfo storeInfo, MediaStoreServerI versionStoreServer,
            XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws MRException, Exception {
        String mediaStreamFileNID = null;
        Document mediaStreamFileDoc = MRConfig.getObjectXMLDocument(ServiceClient.ASSETADMINSERVICE, AMConst.MEDIA_STREAM_FILE_RESOURCE, log);
        HashMap<String, Document> mediaStoreAndStreamMapping = new HashMap<String, Document>();
        Document mediaStoreFileDoc = versionStoreServer.getMediaStoreFileDoc(mediaStoreFileNID, storeInfo.getMediaStoreRootPath());
        mediaStoreFileDoc = versionStoreServer.generateMediaStreamFileDocFromStoreFile(mediaStoreFileDoc, mediaStreamFileDoc, mediaStoreFileNID, storeInfo, dirGroups, storeRevision, true, mediaStoreAndStreamMapping);
        //if successful MEDIA_STREAM_FILE_RESOURCE.... add to db 
        if (mediaStoreFileDoc == null) {
            log.addInfo("mediaFileConverterEventType mediaStoreFileDoc: isNULL " + (mediaStoreFileDoc == null), MRLog.INFO, MRLog.NOTIFY);
            throw new Exception("Failed to generate Media Stream File Doc From Store File ='" + mediaStoreFileNID + "' storeRootPath='" + storeInfo.getMediaStoreRootPath() + "'");
        } else {
            //call sync media : this is common code 
            String mediaGroupID = MRXMLUtils.getValueXpath(mediaStoreFileDoc, "//" + MediaStoreServiceConst.MEDIA_GROUP_ID_EL_NAME, "");
            MediaStoreServiceUtils.updateMediaGroupID(mediaStoreFileDoc, mediaGroupID);
            mediaStreamFileNID = MediaStoreServiceUtils.synchAddMediaFile(mediaStoreFileDoc, mediaStoreFileNID, mediaStoreAndStreamMapping, storeInfo, userJID, log, db, xmpp, resourceFactory, serviceAgentFactory);
            if (mediaStreamFileNID == null) {
                log.addInfo("MediaStoreServiceUtils.register2dot4MediaFile mediaStreamFileNID: isNULL " + (mediaStreamFileNID == null), MRLog.INFO, MRLog.NOTIFY);
                throw new Exception("Failed to synchAddMediaFile file='" + title + "' uuid='" + mediaStoreFileNID + "'");
            }
        }
        return mediaStreamFileNID;
    }
    
    private HashMap<String, Document> getMediaStoreAndMediaStreamMapping(List<?> mediaStreamFileList)
        throws MRException, Exception {
        HashMap<String, Document> mediaStoreAndStreamMapping = new HashMap<String, Document>();

        if (mediaStreamFileList == null) {
            return mediaStoreAndStreamMapping;
        }

        String mediaStoreFileUUID = null;
        Document mediaStreamFileDoc = null;

        for (Object mediaStreamFileObject : mediaStreamFileList) {
            mediaStreamFileDoc = MRXMLUtils.elementToDocument((Element) mediaStreamFileObject);
            mediaStoreFileUUID = MRXMLUtils.getValueXpath(mediaStreamFileDoc, "//UUID", "");
            mediaStoreAndStreamMapping.put(mediaStoreFileUUID, mediaStreamFileDoc);
        }

        return mediaStoreAndStreamMapping;
    }
}
