package com.ipvs.assetadminservice.dir.handler;

import java.util.HashMap;
import java.util.List;

import org.apache.commons.lang.StringUtils;
import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.assetadminservice.handler.AMConst;
import com.ipvs.assetadminservice.handler.GetMediaStreamFileMaxRevisionRequestHandler;
import com.ipvs.client.ServiceAgentUtils;
import com.ipvs.client.ServiceClient;
import com.ipvs.common.MRAuthorization;
import com.ipvs.common.MRConst;
import com.ipvs.common.MRException;
import com.ipvs.common.MRExceptionCodes;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.db.MRConfig;
import com.ipvs.common.db.MRXMLDBUtils;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRRequestHandlerI;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.mediaroomservice.datamodel.MediaRoomAuthorization;
import com.ipvs.mediaroomservice.datamodel.MediaRoomObject;
import com.ipvs.mediaroomservice.impl.MediaRoomPEM;
import com.ipvs.mediastoreservice.impl.MediaStoreServiceConst;
import com.ipvs.mediastoreservice.impl.MediaStoreServiceUtils;
import com.ipvs.sessionservice.impl.AddObjectUtils;
import com.ipvs.sessionservice.impl.SessionServiceConst;
import com.ipvs.sessionservice.impl.UpdateObjectUtils;
import com.ipvs.utils.FileUtils;
import com.ipvs.utils.Utils;

public class PublishDirRequestHandler implements MRRequestHandlerI {
    
    public static final String GET_RESOURCES_XQUERY_PATH ="/com/ipvs/systemservice/handler/xquery/GetDirectoryListAndStorePortQuery.xml";
    public static final String _RESOURCETITLE_ = "_RESOURCETITLE_"; 
    public static final String _UUIDXPATH_ = "_UUIDXPATH_";
    public static final String _CHECKFORDIR_ = "_CHECKFORDIR_";

    @Override
    public MRAuthorization authorizeRequest(MRRequest request, XMPPI xmpp, MRLog log, MRDB db,
            MRResourceFactoryI resourceFactory, ServiceAgentFactoryI profileFactory) throws Exception {
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

    @Override
    public Document getResponse(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
            ServiceAgentFactoryI serviceAgentFactory) throws Exception {

        log.addInfo("PublishDirRequestHandler:getResponse");
        Document requestDoc = request.getMessage().getDocument();
        Element requestDataElement = MRXMLUtils.getElementXpath(requestDoc, request.getRequestDataXPath());
        String resourceTitle = ServiceAgentUtils.getAgentName(request.getUserJID());
        String deviceType = "";
        Document resourcesDoc = getDirectotylistAndStorePortInfo(request, xmpp, log, db, resourceFactory, serviceAgentFactory,resourceTitle,requestDataElement);
        List<?> dirList = MRXMLUtils.getListXpath(requestDataElement, ".//Dir");
        startSync(request, xmpp, log, db, resourceFactory, serviceAgentFactory, requestDataElement, resourcesDoc,dirList,resourceTitle,deviceType);
        
        return createResponse(request,requestDataElement); 
    }
    
    public void startSync(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory,   ServiceAgentFactoryI serviceAgentFactory,
            Element requestDataElement, Document resourcesDoc,List<?> dirList,String resourceTitle,String deviceType) throws Exception {
            String storePortNID = MRXMLUtils.getAttributeValue(MRXMLUtils.getElementXpath(resourcesDoc, "//MediaStorePortResource"), SessionServiceConst.NID,"");
            int index = 1;
            StringBuffer errorDescription = new StringBuffer();
            boolean needSynch = false;
            for(Object object: dirList) {
            Element dirElement = (Element) object;
            String dirUUID = MRXMLUtils.getAttributeValue(dirElement, MediaStoreServiceConst.UUID.toLowerCase(),"");
            Element dirElementInDB = MRXMLUtils.getElementXpath(resourcesDoc, "//MediaStreamDirResource[.//UUID='"+dirUUID+"']");
            String dirNID = MRXMLUtils.getAttributeValue(dirElementInDB, MediaStoreServiceConst.NID, "");       
            Element pvrDirElementInDB = MRXMLUtils.getElementXpath(resourcesDoc, "//SubDir/MediaStreamDirResource[ .//ParentMediaDirNID='"+dirNID+"' and .//Title='"+MediaStoreServiceConst.PVR_DIR_NAME+"']");
            Element recyclebinDirElementInDB = MRXMLUtils.getElementXpath(resourcesDoc, "//SubDir/MediaStreamDirResource[ .//ParentMediaDirNID='"+dirNID+"' and .//Title='"+MediaStoreServiceConst.RECYCLE_BIN_DIR_NAME+"']");
                boolean dirAdded = false;

            String pvrDirNID = MRXMLUtils.getAttributeValue(pvrDirElementInDB, MediaStoreServiceConst.NID, "");
            String recycleBinDirNID = MRXMLUtils.getAttributeValue(recyclebinDirElementInDB, MediaStoreServiceConst.NID, "");
            String state = MRXMLUtils.getAttributeValue(dirElement,SessionServiceConst.STATE,"");

                if(dirElementInDB == null) {
                    dirElementInDB = createMediaStreamDirResource(request, xmpp, log, db, resourceFactory, serviceAgentFactory, dirUUID, true, dirElement, storePortNID, 
                            MRXMLUtils.getAttributeValue(dirElement, "mediaStoreName",MediaStoreServiceConst.DEFAULT_MEDIA_STORE_NAME), MediaStoreServiceConst.DEFAULT_DIR, state);
                    dirNID = MRXMLUtils.getAttributeValue(dirElementInDB, MediaStoreServiceConst.NID, "");
                    recyclebinDirElementInDB = createMediaStreamDirResource(request, xmpp, log, db, resourceFactory, serviceAgentFactory, dirUUID, false, dirElement, 
                            storePortNID, MediaStoreServiceConst.RECYCLE_BIN_DIR_NAME, dirNID, MediaStoreServiceConst.READY); 
                    pvrDirElementInDB = createMediaStreamDirResource(request, xmpp, log, db, resourceFactory, serviceAgentFactory, dirUUID, false, dirElement, 
                            storePortNID, MediaStoreServiceConst.PVR_DIR_NAME, dirNID, MediaStoreServiceConst.READY);
                    dirAdded = true;
                    pvrDirNID = MRXMLUtils.getAttributeValue(pvrDirElementInDB, MediaStoreServiceConst.NID, "");
                    recycleBinDirNID = MRXMLUtils.getAttributeValue(recyclebinDirElementInDB, MediaStoreServiceConst.NID, "");               
                } else {
                    dirElementInDB = updateMediaStreamDirResource(request, xmpp, log, db, resourceFactory, serviceAgentFactory, dirElementInDB,
                            storePortNID, dirElement, index, deviceType, resourceTitle, dirNID,state);
                }
                dirElement.addAttribute("dirNID", dirNID);          
                dirElement.addAttribute("pvrDirNID", pvrDirNID);
                dirElement.addAttribute("recyclebinDirNID", recycleBinDirNID);
                dirElement.addAttribute(MediaStoreServiceConst.MEDIA_DIR_DSIK_FULL_THRESHOLD, MRXMLUtils.getAttributeValueXpath(dirElementInDB, ".//"+MediaStoreServiceConst.MEDIA_DIR_SPANNING,MediaStoreServiceConst.MEDIA_DIR_DSIK_FULL_THRESHOLD,""));
                String dirState = MRXMLUtils.getAttributeValue(dirElement, "state","");
                
                if(dirState.equals(MediaStoreServiceConst.START_SYNCH)) {
                    String storeRevision = MRXMLUtils.getAttributeValue(dirElement, "mediaStoreRevision","");
                    String dirNeedUpgrade = MRXMLUtils.getAttributeValue(dirElement, "needsUpgrade","");
                    boolean startUpgradeSync = dirNeedUpgrade.equals(MediaStoreServiceConst.TRUE);
                    if(!dirAdded && startUpgradeSync) {
                        //Optimization if all the files are in read only no need to start sync
                        List<?> listOfWritableFiles = MediaStoreServiceUtils.getWritableFiles(dirNID, pvrDirNID, recycleBinDirNID, request.getUserJID(), log, db);
                        if(listOfWritableFiles==null || listOfWritableFiles.isEmpty()) {
                            startUpgradeSync = false;
                        }
                    }

                    //if directory needs upgrade reject login request. we have to sync files to update them as read only if service version mismatch
                    if(!startUpgradeSync) {
                        String whereXpath = 
                            "//" + AMConst.MEDIA_STREAM_FILE_RESOURCE +"[(//"+MediaStoreServiceConst.PARENT_DIR_NID_EL_NAME+
                            "[.='"+dirNID+"' or .= '"+pvrDirNID+"'   or .= '"+recycleBinDirNID+"']) and " +
                            "(//"+AMConst.STATEFLAG+"[.='"+MediaStoreServiceConst.BUSY+"'])]";
                        List<?> busyFiles = MediaStoreServiceUtils.getMediaStreamFileResourceList(whereXpath, request.getUserJID(), log, db);
                        GetMediaStreamFileMaxRevisionRequestHandler getMaxRevisionRequestHandler = new GetMediaStreamFileMaxRevisionRequestHandler();
                        String maxRevision = getMaxRevisionRequestHandler.getMaxRevisionNumber(request, xmpp, log, db, resourceFactory, serviceAgentFactory, storeRevision, dirNID, recycleBinDirNID);
                        log.addInfo("DeviceLoginRequestHandler: MedaStoreRevision := "+storeRevision+" MaxRevision:="+maxRevision, MRLog.INFO, MRLog.NOTIFY);
                        if(maxRevision.equals(storeRevision)&& (busyFiles == null || busyFiles.size() == 0)) { //or files not in error/busy state
                        	MediaStoreServiceUtils.updateMediaStreamDirResourceStateAndPresence(MRXMLUtils.elementToDocument(dirElementInDB), storePortNID, MediaStoreServiceConst.READY,
                        			AMConst.AVAILABLE, request, xmpp, log, db, resourceFactory, serviceAgentFactory);
                        	MRXMLUtils.setAttributeValue(dirElement, SessionServiceConst.STATE, MediaStoreServiceConst.READY);
                        	resetDownloadStates(dirNID, pvrDirNID, recycleBinDirNID, request, xmpp, log, db, resourceFactory, serviceAgentFactory);
                        	continue;
                        }
                        MRXMLUtils.addAttribute(dirElement, "mediaStreamFileMaxRevision", maxRevision);
                    }  

                    //star sync - media store and media  stream are out of sync
                    MRXMLUtils.setAttributeValue(dirElement, SessionServiceConst.STATE, MediaStoreServiceConst.BUSY);
                    log.addInfo("DeviceLoginRequestHandler: MedaStore and MediaStream are outof sync. Start Sync process dirTitle:="+MRXMLUtils.getAttributeValue(dirElement,"mediaStoreName","")
                        , MRLog.INFO, MRLog.NOTIFY);


                    errorDescription.append(dirElement.asXML());
                    needSynch = true;
                }else{
                	log.addInfo("NO Synch Required");
                	resetDownloadStates(dirNID, pvrDirNID, recycleBinDirNID, request, xmpp, log, db, resourceFactory, serviceAgentFactory);
                }
            }


            if(needSynch) {
            	throw new MRException(MRExceptionCodes.CAN_NOT_LOGIN_DEVICE_DIR_NEED_SYNC, errorDescription.toString());
            }

        }

        private void resetDownloadStates(String dirNID,String pvrDirNID,String recycleBinDirNID,MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory,   ServiceAgentFactoryI serviceAgentFactory) throws Exception {
        	log.addInfo("NO Synch Required");
        	String query = FileUtils.readFileResource("/com/ipvs/assetadminservice/handler/xquery/UpdateFileDownloadStateQuery.xml", request.getClass());
    		query = StringUtils.replace(query, "_DIRNID_", dirNID);
    		query = StringUtils.replace(query, "_PVRDIRNID_", pvrDirNID);
    		query = StringUtils.replace(query, "_RECYCLEBINDIRNID_", recycleBinDirNID);
    		query = StringUtils.replace(query, "_STATE_", MediaStoreServiceConst.NOT_READY);
    		query = StringUtils.replace(query, "_STALE_", "");
    		MRXMLDBUtils.readQuery(MRXMLDBUtils.TIME_OUT, request, log, db, query, new HashMap<String, String>(), null);		
    		log.addInfo("Update file download State" ,MRLog.INFO,MRLog.NOTIFY);
        }

        //update the media store info in given directory
        private Element updateMediaStreamDirResource( MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI profileFactory,
                Element dirElementInDB,String storePortNID,Element dirElement,int index,String deviceType,String resourceTitle,String dirNID,String state)
                        throws MRException, Exception {
            Element mediaStorePortResourceNIDListEL = MRXMLUtils.getElementXpath(dirElementInDB, ".//" + MediaStoreServiceConst.MEDIA_STORE_PORT_RESOURCE_NID_LIST_EL_NAME);

            //check store prot is there in the dir or not. If not add the element
            String xpath = ".//" + MediaStoreServiceConst.MEDIA_STORE_PORT_RESOURCE_NID_EL_NAME + "[@" + MediaStoreServiceConst.DEVICE_ELEMENT_ID_ATTRIBUTE_NAME + "='" + 
                    MRXMLUtils.getAttributeValue(dirElement, "mediaStoreElementId", "")+ "' and .='" + storePortNID + "']";
            Element mediaStorePortResourceNIDEL = MRXMLUtils.getElementXpath(dirElementInDB, xpath);
            int priority = MediaStoreServiceConst.DEFAULT_PRIORITY;
            boolean internalDir = Utils.getBooleanValue(MRXMLUtils.getAttributeValue(dirElement, "internal", ""),true);
            if (mediaStorePortResourceNIDEL == null) {
                //if it is internal directory reset the title to new store port
                if(Utils.getBooleanValue(MRXMLUtils.getAttributeValue(dirElement, "internal", ""),true)  && !mediaStorePortResourceNIDListEL.elements().isEmpty()) {
                    MRXMLUtils.setValueXpath(dirElementInDB, ".//"+MediaStoreServiceConst.MEDIA_TITLE_ELEMENT_NAME,
                            MRXMLUtils.getAttributeValue(dirElement, "mediaStoreName",MediaStoreServiceConst.DEFAULT_MEDIA_STORE_NAME));
                }
                mediaStorePortResourceNIDEL = (Element) MRXMLUtils.getElementXpath(MRConfig.getObjectXMLDocument(ServiceClient.ASSETADMINSERVICE, AMConst.MEDIA_STREAM_DIR_RESOURCE, log), "//" + MediaStoreServiceConst.MEDIA_STORE_PORT_RESOURCE_NID_EL_NAME).clone();
            } else {
                mediaStorePortResourceNIDEL.detach();
                priority = Utils.getIntValue(MRXMLUtils.getAttributeValue(mediaStorePortResourceNIDEL, MediaStoreServiceConst.PRIORITY_ATTRIBUTE_NAME, ""),MediaStoreServiceConst.DEFAULT_PRIORITY);
            }
            updateStorePortInfo(dirElement, storePortNID, mediaStorePortResourceNIDEL, priority,state);

            //if it is internal directory remove the content of List
            if(!mediaStorePortResourceNIDListEL.elements().isEmpty()) {
                if(internalDir) {
                    MRXMLUtils.removeContent(mediaStorePortResourceNIDListEL);
                } else {
                    List<?> mediaStorePortResourceNIDList = MRXMLUtils.getListXpath(mediaStorePortResourceNIDListEL, ".//MediaStorePortResourceNID[@presence='" + AMConst.UNAVAILABLE + "']");
                    if(mediaStorePortResourceNIDList != null && !mediaStorePortResourceNIDList.isEmpty()) {
                        for(Object obj:mediaStorePortResourceNIDList) {
                            Element mediaStorePortResourceNIDElement = (Element)obj;
                            mediaStorePortResourceNIDElement.detach();
                        }
                    }
                }
            }
            MRXMLUtils.addElement(mediaStorePortResourceNIDListEL, mediaStorePortResourceNIDEL);
            String dirState = MRXMLUtils.getValueXpath(dirElementInDB, ".//" + AMConst.STATE + "/" + AMConst.STATEFLAG, state);

            //if dirstate is offline or error then only start sync
            if(dirState.equals(MediaStoreServiceConst.OFFLINE) || dirState.equals(MediaStoreServiceConst.ERROR)) {
                if(state.equals(MediaStoreServiceConst.START_SYNCH)){
                    //SET SYNC MEDIA STORE PORT NID AS CURRENT DEVICE 
                    MRXMLUtils.setAttributeValueXPath(dirElementInDB, ".//" + MediaStoreServiceConst.MEDIA_STORE_PORT_RESOURCE_NID_LIST_EL_NAME, MediaStoreServiceConst.SYNCH_MEDIA_STORE_PORT_RESOURCE_NID_EL_NAME, storePortNID);
                    dirState = MediaStoreServiceConst.BUSY;
                } else {
                    dirState = state;
                }
                MRXMLUtils.setValueXpath(dirElementInDB, ".//" + AMConst.STATE + "/" + AMConst.STATEFLAG, dirState);
                if (Utils.getBooleanValue(MRXMLUtils.getAttributeValue(dirElement, "needsUpgrade", ""),false)) {
                    MRXMLUtils.setValueXpath(dirElementInDB, ".//" + MediaStoreServiceConst.WRITE, MediaStoreServiceConst.FALSE);
                } else {
                    MRXMLUtils.setValueXpath(dirElementInDB, ".//" + MediaStoreServiceConst.WRITE, MediaStoreServiceConst.TRUE);
                }
		} else {
			// star sync only if dir state is offline or dir state is in error in database.
			MRXMLUtils.addAttribute(dirElement, "state", state);
			if (MediaStoreServiceConst.OFFLINE.equals(state)) {
				List<?> availableMediaStorePorteNIDList = MRXMLUtils.getListXpath(dirElementInDB, ".//" + MediaStoreServiceConst.MEDIA_STORE_PORT_RESOURCE_NID_LIST_EL_NAME + "/"
						+ MediaStoreServiceConst.MEDIA_STORE_PORT_RESOURCE_NID_EL_NAME + "[@" + MediaStoreServiceConst.PRESENCE_ATTRIBUTE_NAME + "=" + AMConst.AVAILABLE + "]");
				if (availableMediaStorePorteNIDList.size() == 0) {
					MRXMLUtils.setValueXpath(dirElementInDB, ".//" + AMConst.STATE + "/" + AMConst.STATEFLAG, state);
				}
			} else {
				String syncMediaStorePortNID = MRXMLUtils.getAttributeValueXpath(dirElementInDB, ".//" + MediaStoreServiceConst.MEDIA_STORE_PORT_RESOURCE_NID_LIST_EL_NAME,
						MediaStoreServiceConst.SYNCH_MEDIA_STORE_PORT_RESOURCE_NID_EL_NAME, "");
				if (syncMediaStorePortNID.equals(storePortNID)) {
					MRXMLUtils.setValueXpath(dirElementInDB, ".//" + AMConst.STATE + "/" + AMConst.STATEFLAG, state);
				}
			}
		}

            MRXMLUtils.setValueXpath(dirElementInDB, ".//" + MediaStoreServiceConst.MEDIA_DIR_SIZE_USED, MRXMLUtils.getAttributeValue(dirElement, "sizeUsed","0"));

            request.setSuperUser(true);
            UpdateObjectUtils.updateElement(request, xmpp, log, db, AMConst.MEDIA_STREAM_DIR_RESOURCE, dirNID, MRXMLUtils.elementToDocument(dirElementInDB),"");
            log.addInfo("DeviceLoginRequestHandler: Update MediaStreamDirResoource With portNID:=" + storePortNID + " storePortElementID :=" +MRXMLUtils.getAttributeValue(dirElement, "mediaStoreElementId", "")
                    , MRLog.INFO, MRLog.NOTIFY);

            return dirElementInDB;
        }


        private Element createMediaStreamDirResource( MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
                ServiceAgentFactoryI profileFactory,String dirUUID,boolean isParent, Element dirElement,String storePortNID, 
                String title, String parentDirNID, String state)
                        throws MRException, Exception {
            Document mediaStreamDirDoc = MRConfig.getObjectXMLDocument(ServiceClient.ASSETADMINSERVICE, AMConst.MEDIA_STREAM_DIR_RESOURCE, log);


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
                if (Utils.getBooleanValue(MRXMLUtils.getAttributeValue(dirElement, "needsUpgrade", "false"), false)) {
                    MRXMLUtils.setValueXpath(mediaStreamDirDoc, "//" + MediaStoreServiceConst.WRITE, MediaStoreServiceConst.FALSE);
                }
                Element mediaStorePortResourceNIDEL = MRXMLUtils.getElementXpath(mediaStreamDirDoc, "//" + MediaStoreServiceConst.MEDIA_STORE_PORT_RESOURCE_NID_EL_NAME);
                updateStorePortInfo(dirElement, storePortNID,mediaStorePortResourceNIDEL,MediaStoreServiceConst.DEFAULT_PRIORITY,state);
                MRXMLUtils.setValueXpath(mediaStreamDirDoc, "//" + MediaStoreServiceConst.UUID, dirUUID);

                if (!Utils.getBooleanValue(MRXMLUtils.getAttributeValue(dirElement, "readOnly","false"), false)) {
                    MRXMLUtils.setAttributeValue(MRXMLUtils.getElementXpath(mediaStreamDirDoc, "//" + MediaStoreServiceConst.MEDIA_STORE_PORT_RESOURCE_NID_LIST_EL_NAME), 
                            MediaStoreServiceConst.SYNCH_MEDIA_STORE_PORT_RESOURCE_NID_EL_NAME, storePortNID);
                }
                if(state.equals(MediaStoreServiceConst.START_SYNCH)){
                    MRXMLUtils.setAttributeValueXPath(mediaStreamDirDoc, "//" + MediaStoreServiceConst.MEDIA_STORE_PORT_RESOURCE_NID_LIST_EL_NAME, MediaStoreServiceConst.SYNCH_MEDIA_STORE_PORT_RESOURCE_NID_EL_NAME, storePortNID);
                    state = MediaStoreServiceConst.BUSY;
                }
                MRXMLUtils.setValueXpath(mediaStreamDirDoc, "//" + AMConst.STATE + "/" + AMConst.STATEFLAG, state);

                //Set the disk full threshold as 10% of total disk space.
                long diskFullThreshold = (long) (Utils.getLongValue(MRXMLUtils.getAttributeValue(dirElement, "size",""),0) * 0.1);

                MRXMLUtils.setValueXpath(mediaStreamDirDoc, "//" + MediaStoreServiceConst.MEDIA_DIR_SIZE, MRXMLUtils.getAttributeValue(dirElement, "size",""));
                MRXMLUtils.setValueXpath(mediaStreamDirDoc, "//" + MediaStoreServiceConst.MEDIA_DIR_SIZE_USED,MRXMLUtils.getAttributeValue(dirElement, "sizeUsed",""));
                MRXMLUtils.setAttributeValueXPath(mediaStreamDirDoc, "//"+MediaStoreServiceConst.MEDIA_DIR_SPANNING, MediaStoreServiceConst.MEDIA_DIR_DSIK_FULL_THRESHOLD, String.valueOf(diskFullThreshold));     
            } else {            
                MRXMLUtils.setValueXpath(mediaStreamDirDoc, "//" + AMConst.STATE + "/" + AMConst.STATEFLAG, MediaStoreServiceConst.READY);
                MRXMLUtils.setValueXpath(mediaStreamDirDoc, "//" + MediaStoreServiceConst.MEDIA_DIR_SIZE, String.valueOf(0));
                MRXMLUtils.setValueXpath(mediaStreamDirDoc, "//" + MediaStoreServiceConst.MEDIA_DIR_SIZE_USED, String.valueOf(0));
                MRXMLUtils.setAttributeValueXPath(mediaStreamDirDoc, "//"+MediaStoreServiceConst.MEDIA_DIR_SPANNING, MediaStoreServiceConst.MEDIA_DIR_DSIK_FULL_THRESHOLD, String.valueOf(0));  
            }

            request.setSuperUser(true);
            log.addInfo("DeviceLoginRequestHandler: Create MediaStreamDirResource in Database.  DirName :=" + title, MRLog.INFO, MRLog.NOTIFY);
            AddObjectUtils.addObjectInDatabase(request, xmpp, log, db, resourceFactory, profileFactory, mediaStreamDirDoc, AMConst.MEDIA_STREAM_DIR_RESOURCE);
            return mediaStreamDirDoc.getRootElement();
        }

        private void updateStorePortInfo(Element dirElement, String storePortNID, Element mediaStorePortResourceNIDEL,int priority,String state)
                throws MRException, Exception { 
            MRXMLUtils.setValue(mediaStorePortResourceNIDEL, storePortNID);
            MRXMLUtils.setAttributeValue(mediaStorePortResourceNIDEL, MediaStoreServiceConst.DEVICE_ELEMENT_ID_ATTRIBUTE_NAME, MRXMLUtils.getAttributeValue(dirElement, "mediaStoreElementId", ""));
            MRXMLUtils.setAttributeValue(mediaStorePortResourceNIDEL, MediaStoreServiceConst.READ_ONLY_ATTRIBUTE_NAME, MRXMLUtils.getAttributeValue(dirElement, "readOnly", ""));
            if(state.equals(MediaStoreServiceConst.READY)){
                MRXMLUtils.setAttributeValue(mediaStorePortResourceNIDEL, MediaStoreServiceConst.PRESENCE_ATTRIBUTE_NAME, AMConst.AVAILABLE);           
            } else {
                MRXMLUtils.setAttributeValue(mediaStorePortResourceNIDEL, MediaStoreServiceConst.PRESENCE_ATTRIBUTE_NAME, AMConst.UNAVAILABLE);
            }
            MRXMLUtils.setAttributeValue(mediaStorePortResourceNIDEL, MediaStoreServiceConst.PRIORITY_ATTRIBUTE_NAME, String.valueOf(priority));
        }   
      //get resources
        private Document getDirectotylistAndStorePortInfo(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory,String resourceTitle,Element requestDataElement) throws MRException,Exception {
            String query = FileUtils.readFileResource(GET_RESOURCES_XQUERY_PATH, request.getClass());
            query = query.replaceAll(_RESOURCETITLE_, resourceTitle);
            List<String> dirIDList = MRXMLUtils.getValuesXpath(requestDataElement, ".//Dir/@uuid");
            
            String whereXpath = "";
            String dirCheck = "false";
            if(dirIDList!=null && !dirIDList.isEmpty()) {
                dirCheck = "true";
                StringBuffer stringBuffer = new StringBuffer("[.//UUID[");
                String delim = "";
                for(String uuid: dirIDList) {
                    stringBuffer.append(delim+ ".='"+uuid+"' ");
                    delim = " or ";
                }
                stringBuffer.append("]]");
                whereXpath = stringBuffer.toString();
            }
            
            query = query.replaceAll(_UUIDXPATH_, whereXpath);
            query = query.replaceAll(_CHECKFORDIR_, dirCheck);
            
            String responseXML  = MRXMLDBUtils.readQuery(MRXMLDBUtils.TIME_OUT, request, log, db, query, new HashMap<String, String>(), null);      
            return MRXMLUtils.stringToDocument(responseXML);
        }
  
    
    private Document createResponse(MRRequest request,Element requestDataElement)throws Exception {
        Element responseDeviceElement = MRXMLUtils.newElement(AMConst.Device);
      //add directory information
        MRXMLUtils.addContent(responseDeviceElement, MRXMLUtils.getListXpath(requestDataElement, ".//Dir"));
        return request.createResponseData(responseDeviceElement);
        
    }


}
