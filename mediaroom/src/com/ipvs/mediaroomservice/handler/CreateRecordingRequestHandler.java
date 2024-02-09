package com.ipvs.mediaroomservice.handler;

import java.util.Iterator;
import java.util.List;

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
import com.ipvs.mediaroomservice.impl.MediaRoomServiceExceptionCodes;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceUtils;
import com.ipvs.mediastoreservice.impl.MediaStoreServiceConst;
import com.ipvs.mediastoreservice.impl.MediaStoreServiceExceptionCodes;
import com.ipvs.mediastoreservice.impl.MediaStoreServiceUtils;
import com.ipvs.policyadminservice.utils.PolicyAdminConstants;


public class CreateRecordingRequestHandler implements MRRequestHandlerI {
    public static final String MEDIAROOM_XMLFILEPATH = "/com/ipvs/mediaroomservice/xml/";

    public MRAuthorization authorizeRequest(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory)
        throws Exception {
        log.addTraceBegin(request);

        int authType = MediaRoomObject.STREAM_TYPEID;
        int role = MediaRoomPEM.VISITOR_ROLE;
        String token = "testCommonToken";
        String authority = "testAuthority";
        String roomPolicyNID = MRConst.NULL;
        String streamPolicyNID = MRConst.NULL;

        // If destNID is empty then use the request.getResourceGroup
        // as the destGroup to find the matching policy
        MediaRoomAuthorization auth = new MediaRoomAuthorization(MRConst.NULL, MRConst.NULL, authType, role, roomPolicyNID, streamPolicyNID, token, authority);
        log.addTraceEnd(request);

        return auth;
    }

    public Document getResponse(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory)
        throws Exception {
        Document requestdoc = request.getMessage().getDocument();
       // Element metaDataEL = MRXMLUtils.getElementXpath(requestdoc, "//MetaData");

        String destNID = null;
        String recordID = MRXMLUtils.getValueXpath(requestdoc, "//recordID", "");
        String userJID = request.getUserJID();
        String title = MRXMLUtils.getValueXpath(requestdoc, "//title", "");
        Document responseDoc = null;
        String destDirNID =null;

        try {
            Element fileEL = MediaRoomServiceUtils.getMediaStreamFileResource(recordID, request, userJID, xmpp, log, db, resourceFactory, serviceAgentFactory);

            // If file exist in the system  before appending check the file status is stopped or not. 
            // If File status is not stopped then throw exception recording in process. - by padmaja to fix 5162 bug
            
            
            if (fileEL != null) {            	
                String recordFileStatus = MRXMLUtils.getValueXpath(fileEL, "State/StateFlag", "");

                if (!recordFileStatus.isEmpty() && recordFileStatus.equals(MediaStoreServiceConst.BUSY)) {
                    throw new MRException(MediaStoreServiceExceptionCodes.RECORDING_IN_PROGRESS,
                        "Recording Started for Given RecordID:="+ recordID +".  OwnerJID:=" + MRXMLUtils.getValueXpath(fileEL, "Info/Properties/Owner", "") + " and RecordingStatus:=" + recordFileStatus);
                }
                
                if ((!recordFileStatus.isEmpty()) && recordFileStatus.equals(MediaStoreServiceConst.ERROR)) {
                    throw new MRException(MediaStoreServiceExceptionCodes.MEDIA_STORE_STATE_ERROR, "Media Stream File State is Error, Can not proceed");
                }

                destNID = MRXMLUtils.getAttributeValue(fileEL, "NID", "");
                destDirNID = MRXMLUtils.getValueXpath(fileEL, "Info/Properties/ParentMediaDirNID", "");
            }
            
            String sourceNID = getSourceNID(userJID, title, db, log);
            Element profileXMLEL = getProfileXML(request, sourceNID, userJID, xmpp, log, db, resourceFactory, serviceAgentFactory);

            if (profileXMLEL == null) {
                responseDoc = createResponseDoc("", "", "", "","",recordID, MediaRoomServiceExceptionCodes.NO_MATCHING_PROFILE, "No Profile Found", request);
                return responseDoc;
            }
            String profileXML = MRXMLUtils.elementToString(profileXMLEL); 

            Element destDirEL = getDestDir(request, userJID, sourceNID,destDirNID, xmpp, log, db, resourceFactory, serviceAgentFactory);

            if (destDirEL == null) {
                responseDoc = createResponseDoc("", "", "", "", "",recordID, MediaRoomServiceExceptionCodes.NO_MATCHING_MEDIA_DIRECTORIES, "No Matching Directory Found", request);
                return responseDoc;
            }
            String destDir = MRXMLUtils.elementToString(destDirEL);
            
            //find store contact JID.
            String storeOwnerJID = getStoreOwenerJID(destDirEL, userJID, title, db, log);
            if(storeOwnerJID == null || storeOwnerJID.isEmpty()) {
                responseDoc = createResponseDoc("", "", "", "","", recordID, MediaRoomServiceExceptionCodes.AGENT_NOT_FOUND, "No Store Agent Found", request);
                return responseDoc;
            }
     
            responseDoc = createResponseDoc( sourceNID, destNID, profileXML, destDir, storeOwnerJID,  recordID, MRLog.OK, "", request);
            
        } catch (MRException exp) {
            exp.printStackTrace();
            responseDoc = createResponseDoc("", "", "", "", "",recordID, exp.getCode(), exp.getData(), request);
        } catch (Exception exp) {
            exp.printStackTrace();
            responseDoc = createResponseDoc("", "", "", "","",recordID, MediaStoreServiceExceptionCodes.START_RECORDING_REQUEST_FAILED, "Start Recording Request Failed Exception:" + exp.toString(), request);
        }

        return responseDoc;
    }
    
    
    private String getStoreOwenerJID(Element destDirEL,String userJID, String title, MRDB db, MRLog log) throws MRException,Exception {
        Element mediaStorePortResourceNIDElement = MediaStoreServiceUtils.getMediaStorePortResourceElementFromDir(MRXMLUtils.elementToDocument(destDirEL), true,db, log);
		String mediaStorePortResourceNID = MRXMLUtils.getValue(mediaStorePortResourceNIDElement);
		return  MediaRoomServiceUtils.getContactJIDForPort(MRRequest.createRequest(userJID), log, db,mediaStorePortResourceNID);
    }

    private Document createResponseDoc(String sourceNID, String destNID, String profileXML, String destDir,String storeOwnerJID, String recordID, int errorCode, String message, MRRequest request)
    throws MRException, Exception {
    	String rdname = request.getResponseDataName();
    	Document responseDoc = null;
    	StringBuffer responseStr = new StringBuffer();
    	responseStr.append("<" + rdname + ">");
    	responseStr.append(MRXMLUtils.generateXML("sourceNID", sourceNID));
    	responseStr.append(MRXMLUtils.generateXML("destNID", destNID));
    	responseStr.append(MRXMLUtils.generateXML("profileXML", profileXML));
    	responseStr.append(MRXMLUtils.generateXML("destDir", destDir));
    	responseStr.append(MRXMLUtils.generateXML("recordID", recordID));
    	responseStr.append(MRXMLUtils.generateXML("storeOwnerJID", storeOwnerJID));
    	responseStr.append(MRXMLUtils.generateXML("status", MRXMLUtils.generateXML("Code", String.valueOf(errorCode)) + MRXMLUtils.generateXML("Description", message)));
    	responseStr.append("</" + rdname + ">");
    	responseDoc = MRXMLUtils.stringToDocument(responseStr.toString());
    	return responseDoc;
    }
    

    private String getSourceNID(String userJID, String title, MRDB db, MRLog log) throws MRException, Exception {
        String[] whereXPath = { "//MediaStreamSrcPortResource/.[Info/Title = '" + title + "']" };
        
        MRConfig config = MRConfig.getInstance(ServiceClient.ASSETADMINSERVICE, PolicyAdminConstants.MEDIA_STREAM_SRC_PORT_RESOURCE, log);
        String parentNID = db.getServiceDomainRootNID(config.getParentNIDKey(log));
        Element responseEL = MRXMLUtils.newElement(PolicyAdminConstants.MEDIA_STREAM_SRC_PORT_RESOURCE + "List");
        Element whereAssistXML = MRXMLUtils.newElement("whereAssistXML");

        MRXMLDBUtils.findChildElements(parentNID, responseEL, null, 1, whereAssistXML, whereXPath, MRRequest.createRequest(userJID), db, log);

        String mediaStreamSrcPortResourceNID = MRXMLUtils.getAttributeValue(MRXMLUtils.getChild(responseEL,"MediaStreamSrcPortResource"),"NID");

        return mediaStreamSrcPortResourceNID;
    }

    private Element getProfileXML(MRRequest request, String mediaStreamSrcPortResourceNID, String userJID, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
        ServiceAgentFactoryI serviceAgentFactory) throws MRException, Exception {
        Document requestDoc = MRXMLUtils.loadXMLResource(MEDIAROOM_XMLFILEPATH + "GetStreamProfileListRequest.xml", log);

        MRXMLUtils.setValueXpath(requestDoc, "//sourceNID", mediaStreamSrcPortResourceNID);

        Document streamProfileResponseDoc = MediaRoomServiceUtils.getMediaRoomResponse(request, userJID, xmpp, log, db, resourceFactory, serviceAgentFactory, ServiceClient.MEDIAROOMSERVICE,
                "GetStreamProfileListRequest", requestDoc);

        Element streamProfileEL = MRXMLUtils.getElementXpath(streamProfileResponseDoc, "//" + AMConst.V2D_MEDIA_STREAM_PROFILE_OBJECTTYPE);

        return streamProfileEL;
    }

    private Element getDestDir(MRRequest request, String userJID, String sourceNID, String destDirNID,XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory)
        throws MRException, Exception {
        Document msgdoc = MRXMLUtils.loadXMLResource(MEDIAROOM_XMLFILEPATH + "GetDestMediaDirListRequest.xml", log);
        //Element requestEl = msgdoc.getRootElement().element("data");
        MRXMLUtils.setValueXpath(msgdoc, "//sourceNID", sourceNID);
    	MRXMLUtils.setValueXpath(msgdoc, "//SortOrder", "ascending");
		MRXMLUtils.setValueXpath(msgdoc, "//SortByXpath", "$VAR/min(//MediaStorePortResourceNIDList/MediaStorePortResourceNID[@priority!='']/@priority)");
		
        Document responseDoc = MediaRoomServiceUtils.getMediaRoomResponse(request, userJID, xmpp, log, db, resourceFactory, serviceAgentFactory, ServiceClient.MEDIAROOMSERVICE,
                "GetDestMediaDirListRequest", msgdoc);

        //check for permission to append to the recording file. 
        //If the file Directory NID is not there in the PEM retrieved directory list 
        //then throw exception user don't have permissions to record media. - by Padmaja to fix bug 5162
        Element destDirEL = null;
        if(destDirNID != null){
        	List<?> dirList = MRXMLUtils.getListXpath(responseDoc, "//" + AMConst.MEDIA_STREAM_DIR_RESOURCE);
        	Iterator<?> dirListIterator = dirList.iterator();
        	String dirNID = null;
        	boolean isPermitted = false;
        	while(dirListIterator.hasNext()){
        		destDirEL = (Element) dirListIterator.next();
        		dirNID = MRXMLUtils.getAttributeValue(destDirEL, "NID", "");
        		if(dirNID.equals(destDirNID)){
        		   isPermitted = true;
        		   break;	
        		}
        	}      
        	if (!isPermitted) {
        		throw new MRException(MediaRoomServiceExceptionCodes.NO_PERMISSION_TO_RECORD_MEDIA_ON_THIS_DIRECTORY, "No Permissions available to record media on this directory");
        	}
        	
        }
        destDirEL = MRXMLUtils.getElementXpath(responseDoc, "//" + AMConst.MEDIA_STREAM_DIR_RESOURCE);

        return destDirEL;
    }

}
