package com.ipvs.mediaroomservice.impl;

import java.io.BufferedReader;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.net.URLDecoder;
import java.net.URLEncoder;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.appserver.AppServerAgentUtils;
import com.ipvs.assetadminservice.handler.AMConst;
import com.ipvs.assetadminservice.handler.AssetManagementRequestHandler;
import com.ipvs.assetadminservice.impl.AssetAdminServiceUtils;
import com.ipvs.client.MRClientAgent;
import com.ipvs.client.ServiceAgentUtils;
import com.ipvs.client.ServiceClient;
import com.ipvs.common.MRConst;
import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.MRRequestCache;
import com.ipvs.common.MRRequestI;
import com.ipvs.common.XMPPI;
import com.ipvs.common.XMPPTransaction;
import com.ipvs.common.db.DBServiceUtils;
import com.ipvs.common.db.MRDBHelper;
import com.ipvs.common.db.MRXMLDBUtils;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.MRClientUtils;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRMessage;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRRequestHandlerI;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.mediaroomservice.handler.AddStreamActionRequestHandler;
import com.ipvs.mediaroomservice.handler.CreateRecordingRequestHandler;
import com.ipvs.mediaroomservice.handler.DeleteMediaRoomRequestHandler;
import com.ipvs.mediastoreservice.impl.MediaStoreAdminServerAgent;
import com.ipvs.mediastoreservice.impl.MediaStoreServiceConst;
import com.ipvs.mediastoreservice.impl.MediaStoreServiceExceptionCodes;
import com.ipvs.mediastoreservice.impl.MediaStoreServiceUtils;
import com.ipvs.utils.Utils;


public class MediaRoomServiceUtils {
    public static final String VERSIONFILE = "/com/ipvs/app/version.xml";
    public static final String VERSIONXPATH = "//version";
    public static final String ACTION_INSTANCE_XML = "/com/ipvs/mediaroomservice/impl/ActionInstance.xml";
    public static final String TRACKACTION_INSTANCE_XML = "/com/ipvs/mediaroomservice/impl/TrackActionInstance.xml";
    public static final String UPDATE_STREAM_STATE_XML = "/com/ipvs/mediaroomservice/xml/UpdateStreamStateRequest.xml";
    public static final String UPDATE_STREAM_STATE_REQUEST = "UpdateStreamStateRequest";
    public static final String TRACK_STATEMACHINE_XML = "/com/ipvs/mediaroomservice/impl/TrackStateMachineTransitions.xml";
    public static final String UPDATEDESTFILEPROFILE_REQUEST = "UpdateDestFileProfileRequest";
    public static final String GETCURRENTTIMESTAMP_REQUEST_XML = "GetCurrentTimeStampRequest.xml";
    public static final String DETACH_DIR_REQUEST_XML = "DetachDirRequest.xml";
    public static final String DETACH_DIR_REQUEST = "DetachDirRequest";
    public static final String ATTACH_DIR_REQUEST_XML = "AttachDirRequest.xml";
    public static final String ATTACH_DIR_REQUEST = "AttachDirRequest";
    public static final String GETOFFSETATTIMCODE_REQUEST_XML = "GetOffsetAtTimecodeRequest.xml";
    public static final String GETTIMECODEATOFFSET_REQUEST_XML = "GetTimeCodeAtOffsetRequest.xml";
    public static final String UPDATEDESTFILEPROFILE_REQUEST_XML = UPDATEDESTFILEPROFILE_REQUEST + ".xml";
    public static final String ADDTRACKACTION_REQUEST_XML = "AddTrackActionRequest.xml";
    public static final String ADDSTREAMACTION_REQUEST_XML = "AddStreamActionRequest.xml";
    public static final String SS_EVENT_INSTANCE_XML = "/com/ipvs/mediaroomservice/impl/SSEventInstance.xml";
    public static final String DEST_EVENT_INSTANCE_XML = "/com/ipvs/mediaroomservice/agent/DestEventInstance.xml";
    public static final String SET_SERVICE_DOMAIN_XML = "/com/ipvs/systemservice/xml/SetServiceDomainRequest.xml";
    public static final String GET_MEDIAROOM_INVITES_XML = "/com/ipvs/mediaroomservice/xml/GetMediaRoomInvitesRequest.xml";
    public static final String ADDTRACK_ACTION_XML = "/com/ipvs/mediaroomservice/xml/AddTrackActionRequest.xml";
    public static final String UPDATEBOOKMARK_XML = "/com/ipvs/mediaroomservice/xml/UpdateBookmarkRequest.xml";
    private static Document gTrackStateMachine = null;

    private static Document gConnectionStateList = null;
    public static Document getConnectionStateList(MRLog log) throws Exception  {
    	if(MediaRoomServiceUtils.gConnectionStateList == null) {
    		MediaRoomServiceUtils.gConnectionStateList = MRXMLUtils.loadXMLResource(CollaborationManager.CONNECTION_STATELIST_XML, log);
    	}
    	return MediaRoomServiceUtils.gConnectionStateList;
    }
    
    public static String getPlayLength(MRRequest request, MRLog log, MRDB db, String roomNID, String xpath,boolean sync) throws Exception {    	
    	MediaRoom mediaroom = RoomDB.getRoomDB().getRoom(roomNID, request, log, db);
    	Document roomDoc = mediaroom.getRoomDoc();    	
    	List<Long> list = new ArrayList<Long>();
        List<String> listOfPlayLengths = MRXMLUtils.getValuesXpath(roomDoc, xpath);
        for (String val : listOfPlayLengths) {        	
        	list.add(Utils.getLongValue(val, Long.parseLong(MediaRoomServiceConst.DEFAULT_TRIMLENGTH)));
        }
        String retVal = String.valueOf(Collections.min(list));
        if(sync) {
        	retVal = String.valueOf(Collections.max(list));
        	log.addInfo("Sync playback true pick max playlength "+ roomNID +" playlength :="+retVal);
        } else {
        	log.addInfo("Sync playback false pick min playlength "+ roomNID +" playlength :="+retVal);
        }

    	return retVal;
    }
    
	public static String getStateStr(MRLog log, int state) {
		String stateStr = null;
		try {
			Element connectionState = MRXMLUtils.getElementXpath(
					MediaRoomServiceUtils.getConnectionStateList(log), 
					"//ConnectionState[@streamstate='" + String.valueOf(state) + "']");
			if(connectionState != null)
				stateStr = MRXMLUtils.getAttributeValue(connectionState,"state");
			else 
				stateStr = "ConnectionStateNotFoundFor streamstate=" + String.valueOf(state);
		}
		catch(Exception e) {
			stateStr = "ConnectionStateNotFoundFor streamstate=" + String.valueOf(state);
			e.printStackTrace();
		}
		return stateStr;
	}

    public static String getPortNID(String sourceNID, MRRequest request, MRDB db, MRLog log) {
        String storePortNID = null;

        try {
            Document sourcedoc = MRXMLDBUtils.readElement(request,log,  db, sourceNID);

            if (sourcedoc != null) {                

            	if(sourcedoc.getRootElement().getName().equals(AMConst.MEDIA_STREAM_FILE_RESOURCE)) {
            		// If get Port call is using for other than source then please change this logic
            	   storePortNID = MediaStoreServiceUtils.getMediaStorePortResourceNIDFromDirRead(sourcedoc, request, db, log);
            	}

                if ((storePortNID != null) && (storePortNID.trim().length() != 0)) {
                    return storePortNID;
                }
            }
        } catch (Exception e) {
            e.printStackTrace();
        }

        return sourceNID;
    }

    /*public static String getRelayPortNIDFromStorePort(String destNID, String userJID, MRRequest request, MRDB db, MRLog log) {
        String relayPortNID = "";

        try {
            String portNID = getPortNID(destNID, request, db, log);
            Document portDocument = MRXMLUtils.stringToDocument(MRXMLDBUtils.getObjectData(request, db, log, portNID));
            String assetResourceNID = MRXMLUtils.getValueXpath(portDocument, "//AssetResourceNID", "");

            if ((assetResourceNID == null) || (assetResourceNID.length() == 0)) {
                return "";
            }

            Document deviceDoc = ServiceAgentUtils.getDeviceDoc(assetResourceNID, userJID, db, log);  
            if(deviceDoc ==null) {
            	return "";
            }
            relayPortNID = MRXMLUtils.getValueXpath(deviceDoc, "//MediaStreamRelayPortResourceNID/NIDValue", "");
        } catch (Exception e) {
            e.printStackTrace();
        }

        return relayPortNID;
    }*/

    public static String getSourceType(String sourceNID, MRRequest request, MRDB db, MRLog log) {
        String sourceType = MediaRoomServiceConst.INPUT_ENCODER;
        String portNID = MediaRoomServiceUtils.getPortNID(sourceNID, request, db, log);

        if (!portNID.equals(sourceNID)) {
            sourceType = MediaRoomServiceConst.INPUT_MEDIAFILE;
        }

        return sourceType;
    }

    public static String getDestType(String destNID, MRDB db, MRRequest request, MRLog log) {
        try {
            if ((destNID != null) && (destNID.trim().length() != 0)) {
                Element response = MRXMLUtils.newElement("ResponseXML");

                //perf-logs  log.addInfo("MediaRoomServiceUtils::=" + request.getUserJID()) + " readElement DestType- Start", MRLog.OK, MRLog.NOTIFY);
                MRXMLDBUtils.readElement(1, null, null, destNID, -1, response, null, request, db, log);

                //perf-logs  log.addInfo("MediaRoomServiceUtils::=" + request.getUserJID()) + " readElement DestType- end", MRLog.OK, MRLog.NOTIFY);
                String fileType = null;
                fileType = MRXMLUtils.getValueXpath(MRXMLUtils.getFirstChild(response), "Info/MediaStoreFileInfo/Type", "");

                if ((fileType != null) && (fileType.trim().length() != 0)) {
                    if (fileType.equals("MediaClip") || fileType.equals("MediaGroup")) {
                        return MediaRoomServiceConst.OUTPUT_RECORD;
                    } else if (fileType.equals("PVRMediaClip")) {
                        return MediaRoomServiceConst.OUTPUT_PVR;
                    }
                }
            }
        } catch (Exception e) {
            e.printStackTrace();
        }

        return MediaRoomServiceConst.OUTPUT_STREAM;
    }

    /*public static String getContactJIDForMedia(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory, String NID)
        throws MRException {
        try {
            //perf-logs  log.addInfo("MediaRoomServiceUtils::=" + request.getUserJID()) + " getPortNID - Start", MRLog.OK, MRLog.NOTIFY);
            NID = MediaRoomServiceUtils.getPortNID(NID, request, db, log);

            //perf-logs  log.addInfo("MediaRoomServiceUtils::=" + request.getUserJID()) + " getPortNID - End", MRLog.OK, MRLog.NOTIFY);
            return getContactJIDForPort(request, log, db, NID);
        } catch (Exception e) {
            throw new MRException(MediaRoomServiceExceptionCodes.CONTACTNOTFOUND,MRXMLUtils.generateErrorXML("nid", NID) );
        }
    }*/
    
    public static String getAgentJID (MRRequest request, Document portResourceDoc) throws MRException,Exception {
    	String destAgentJID;
    	String portState  =MRXMLUtils.getValueXpath(portResourceDoc, "//State/StateFlag", "");    	
    	if(portState.equals(MRClientUtils.ASSET_STATE_READY)){
     	   destAgentJID = MRXMLUtils.getValueXpath(portResourceDoc, "//Contact/OwnerUserJID", "");
     	} else {
     		destAgentJID = "";
     	}
    	//if unmanaged then ignore state flag
    	String streamURL = MRXMLUtils.getValueXpath(portResourceDoc, "//MediaStreamURL", "");
		if(!"".equals(streamURL)) { // Generate the agentJID based on the title
	    	String portTitle  =MRXMLUtils.getValueXpath(portResourceDoc, "//Info/Title", "");
	    	portTitle=portTitle.replaceAll("\\s+", "");
	    	String userJID = request.getUserJID();
	    	if (userJID.equals(request.getAppServerJID())) {
	    		destAgentJID = portTitle;
	    	} else {
	    		int index = userJID.indexOf("@");
		    	destAgentJID = portTitle + userJID.substring(index);
	    	}	    	
	    	// destAgentJID = MRXMLUtils.getValueXpath(portResourceDoc, "//Contact/OwnerUserJID", "");
		}
    	return destAgentJID;
    }


    public static String getContactJIDForPort(MRRequest request, MRLog log, MRDB db, String NID)
        throws MRException, Exception {
        Document sourceDoc = MRXMLDBUtils.readElement(request, log, db, NID );
        if (sourceDoc == null) {
            return null;
        }
        return getAgentJID(request, sourceDoc);
    }
    public static Document getAssetAdminResponse(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory, String serviceName,
            String requestName, Element whereAssistXML, String whereXPath, String Depth, String startCount , String countToFetch)
            throws Exception {
    	return getAssetAdminResponse(request, xmpp, log, db, resourceFactory, serviceAgentFactory, serviceName, requestName, whereAssistXML, whereXPath, Depth, startCount, countToFetch, null,null);    	
    }
    
    public static Document getAssetAdminResponse(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory, String serviceName,
            String requestName, Element whereAssistXML, String whereXPath, String sortByXpath , String sortOrder)
            throws Exception {
    	return getAssetAdminResponse(request, xmpp, log, db, resourceFactory, serviceAgentFactory, serviceName, requestName, whereAssistXML, whereXPath, null, null, null, sortByXpath,sortOrder);    	
    }
    
    //#1
    public static Document getAssetAdminResponse(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory, String serviceName,
        String requestName, Element whereAssistXML, String whereXPath, String Depth, String startCount , String countToFetch, String sortByXpath,String sortOrder)
        throws Exception {
XMPPTransaction.startBlock("getAssetAdminResponse");

        Document doc = AssetAdminServiceUtils.getAssetAdminRequest(log, requestName);
        //System.out.println("Dpoc:="+doc.asXML());

        if (whereXPath != null) {
            Element whereXPathElement = MRXMLUtils.getElementXpath(doc, "//WhereXPath");
            whereXPathElement.setText(whereXPath);
        }
		//not supported in new api. server neglects in old api
        /*
        if (whereAssistXML != null) {
            Element whereAssistXMLElement = MRXMLUtils.getElementXpath(doc, "//WhereAssistXML");
            whereAssistXMLElement.add(whereAssistXML.detach());
        }

        if (Depth != null) {
            Element NodeDepthXMLElement = MRXMLUtils.getElementXpath(doc, "//NodeDepth");
            NodeDepthXMLElement.add(Depth);
        }
		*/

        Element startCountXML = MRXMLUtils.getElementXpath(doc, "//StartFrom");
        if(startCount == null) {
        	startCountXML.setText("0");
        }	
        else {
        	startCountXML.setText(startCount);
        }
        
       
        if(sortByXpath != null) {
             MRXMLUtils.setValueXpath(doc, "//SortByXPathList/SortByXpath", sortByXpath); 
        }
        
        if(sortOrder != null) {
        	 MRXMLUtils.setValueXpath(doc, "//SortOrder", sortOrder);
        }
        
        Element countToFetchXML = MRXMLUtils.getElementXpath(doc, "//CountToFetch");
        if(countToFetch == null) {
        	countToFetchXML.setText("-1");
        }
        else {
        	countToFetchXML.setText(countToFetch);
        }
        Element assetdata = (Element) MRXMLUtils.getElementXpath(doc, "//Data").detach();

        Element mroot =  request.getMessage().getDocument().getRootElement();
        MRXMLUtils.removeChild(mroot,"data");
        MRXMLUtils.addElement(mroot,assetdata);

        MRRequestHandlerI handler = new AssetManagementRequestHandler();
        Document assetResult = handler.getResponse(request, xmpp, log, db, resourceFactory, serviceAgentFactory);
        
XMPPTransaction.endBlock("getAssetAdminResponse");

        return request.createResponseData((Element) (MRXMLUtils.getElementXpath(assetResult, "//GetResponse")).detach());
    }

    // get asset admin response for update and get operations based on nid - by padmaja
    public static Document getAssetAdminResponse(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory, String serviceName,
        String requestName, String nid, String depth, Element OperationData, String operation)
        throws Exception {
        Document assetRequestDoc = AssetAdminServiceUtils.createAssetAdminServiceRequest(requestName, nid, depth, OperationData,  log);

        /*Element assetdata = (Element) MRXMLUtils.getElementOXpath(doc, "//data").detach();
        
        Element mroot = ((MRMessage) request.getMessage()).getDocument().getRootElement();
        mroot.removeChild("data");
        mroot.add(assetdata);
        */
        String clientData = URLEncoder.encode(MRXMLUtils.documentToString(request.getMessage().getDocument()), "UTF-8");
        String assetRequestName = MRRequest.getRequestName(AssetManagementRequestHandler.class.getName());
        MRRequest assetRequest = MRRequest.createRequest(request, assetRequestDoc,assetRequestName , ServiceClient.ASSETADMINSERVICE,clientData);
        assetRequest.setCache(request.getCache());
        assetRequest.setSuperUser(true);
        MRRequestHandlerI handler = new AssetManagementRequestHandler();
        Document assetResult = handler.getResponse(assetRequest, xmpp, log, db, resourceFactory, serviceAgentFactory);
        if(assetResult == null){
        	return null;
        }

        if (operation.equals("Add")) {
            return request.createResponseData((Element) (MRXMLUtils.getElementXpath(assetResult, "//AddResponse")).detach());
        } else if (operation.equals("Replace")) {
            return request.createResponseData((Element) (MRXMLUtils.getElementXpath(assetResult, "//ReplaceResponse")).detach());
        } else if (operation.equals("Get")) {
            return request.createResponseData((Element) (MRXMLUtils.getElementXpath(assetResult, "//ResponseXML")).detach());
        } else if (operation.equals("Delete")) {
            return request.createResponseData((Element) (MRXMLUtils.getElementXpath(assetResult, "//DeleteResponse")).detach());
        }

        return null;
    }   

    public static void modifyNID(Document doc, String NID) {
        // It works only with assetadmin
        if (NID == null) {
            return;
        }

        try {
            Element NodeDepthXMLElement = MRXMLUtils.getElementXpath(doc, "//GetRequest/NID");

            if (NodeDepthXMLElement != null) {
                Element parent = MRXMLUtils.getParentElement(NodeDepthXMLElement);
                NodeDepthXMLElement.detach();
                MRXMLUtils.addElement(parent, "NID", NID);

                return;
            }
        } catch (Exception ex) {
        }

        try {
            Element NodeDepthXMLElement = MRXMLUtils.getElementXpath(doc, "//ReplaceRequest/NID");

            if (NodeDepthXMLElement != null) {
                Element parent = MRXMLUtils.getParentElement(NodeDepthXMLElement);
                NodeDepthXMLElement.detach();
                MRXMLUtils.addElement(parent, "NID", NID);

                return;
            }
        } catch (Exception ex) {
        }

        try {
            Element NodeDepthXMLElement = MRXMLUtils.getElementXpath(doc, "//AddRequest/ParentNID");

            if (NodeDepthXMLElement != null) {
                Element parent = NodeDepthXMLElement.getParent();
                NodeDepthXMLElement.detach();
                MRXMLUtils.addElement(parent, "ParentNID", NID);

                //  System.out.println("Modified ParentNID");
                //  System.out.println(MRXMLUtils.documentToString(doc));
                return;
            }
        } catch (Exception ex) {
        }

        try {
            Element NodeDepthXMLElement = MRXMLUtils.getElementXpath(doc, "//DeleteRequest/NID");

            if (NodeDepthXMLElement != null) {
                Element parent = NodeDepthXMLElement.getParent();
                NodeDepthXMLElement.detach();
                MRXMLUtils.addElement(parent, "NID", NID);

                return;
            }
        } catch (Exception ex) {
        }
    }

    public static Document getAssetAdminResponse(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory, String serviceName,
            String requestName, Element whereAssistXML, String whereXPath) throws Exception {
            return MediaRoomServiceUtils.getAssetAdminResponse(request, xmpp, log, db, resourceFactory, serviceAgentFactory, serviceName, requestName, whereAssistXML, whereXPath, null,null,null,null,null);
    }

    //Newly added method
    public static Element getOperator(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory)
        throws Exception {
        Element operator = getOperator(request, log, db);

        return operator;
    }

    public static Element getOperator(MRRequest request, MRLog log, MRDB db)
        throws Exception {
    	
    	Element operator = request.getOperatorGroups();
    	if(operator != null) {
    		return operator;
    	}
    	
        Element user = DBServiceUtils.getUser(request, db, log);
        Element resource = DBServiceUtils.getResource(request, db, log);
        operator = MRXMLUtils.stringToElement("<Operator/>");

        if (user != null) {
            operator.add(user.detach());
        }

        if (resource != null) {
            operator.add(resource.detach());
        }
        
        request.setOperatorGroups(operator);

        return operator;
    }

    public static Element getDestination(MRRequest request, String NID, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory)
        throws Exception {
    	MRRequestCache cache = request.getCache();
    	Element groups = null;          
    	String key = "DestinationGroup" +"."+NID;
        if(cache != null && cache.containsKey(key)) {
            groups = (Element) cache.get(key);
            //return ca copy if not null
            if (groups != null) {
                return groups.createCopy();
            }
        }

        groups = DBServiceUtils.getUserAndResource(request, NID, "Destination", xmpp, log, db, resourceFactory, serviceAgentFactory);
        if(cache !=null) {
        	cache.put(key, groups);
        }
        
        return groups;
    }

    public static Element getRelayDestination(MRRequest request, String NID, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory)
        throws Exception {
        Element groups = DBServiceUtils.getUserAndResource(request, NID, "Destination", xmpp, log, db, resourceFactory, serviceAgentFactory);

        return groups;
    }

    public static Element getProfile(MRRequest request, String NID, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory)
        throws Exception {
        Element whereAssistXML = null;

        /*Document response = MediaRoomServiceUtils.getAssetAdminResponse(request, xmpp, log, db, resourceFactory, serviceAgentFactory,
                request.getServiceName(), "V2DMediaStreamProfile" + "_BASE_ALL_GET_LIST.xml", whereAssistXML,
                "//" + "V2DMediaStreamProfile" + "[@NID = '" + NID + "']");
            */

        /*Document response = MediaRoomServiceUtils.getAssetAdminResponse(request, xmpp, log,  db, resourceFactory, serviceAgentFactory, request.getServiceName(), "V2DMediaStreamProfile" + "_BASE_ALL_GET_LIST.xml", NID,"3", null);
            System.out.println(MRXMLUtils.documentToString(response));
        Element profileGroups = MRXMLUtils.getElementXpath(response, "//Groups");
        profileGroups.setName("ProfileGroups");
        System.out.println(MRXMLUtils.elementToString(profileGroups));
        return profileGroups;*/
        Element response = MRXMLUtils.newElement("ResponseXML");
        int whereSelectNodeDepth = 5;
        String[] whereXPathArray = null;
        MRXMLDBUtils.readElement(whereSelectNodeDepth, whereAssistXML, whereXPathArray, NID, response, request, log, db);

        //Element profileGroups = response.element("V2DMediaStreamProfile").element("Groups");
        Element profileGroups = response.element("V2DMediaStreamProfile");
        Element profile = MRXMLUtils.newElement("Profile");

        if (profileGroups != null) {
            profile.add(profileGroups.detach());
        }

        return profile;
    }

    public static String getVersion(MRLog log) {
        String version = null;
        Document doc = null;

        try {
            doc = MRXMLUtils.loadXMLResource(VERSIONFILE, log);
        } catch (Exception e) {
            version = "Unknown";
        }

        if (doc != null) {
            try {
                version = MRXMLUtils.getValueXpath(doc, VERSIONXPATH, "Unknown");
            } catch (Exception e) {
                version = "Unknown";
            }
        }

        return version;
    }

    public static Element getSource(MRRequest request, String NID, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory)
        throws Exception {
        //perf-logs//log.addInfo("MediaRoomServiceUtils:" + request.getUserJID()) + " getSourceType - Start", MRLog.OK, MRLog.NOTIFY);
        String sourceType = MediaRoomServiceUtils.getSourceType(NID, request, db, log);

        return MediaRoomServiceUtils.getSourceWithSourceType(request, NID, xmpp, log, db, resourceFactory, serviceAgentFactory, sourceType);
    }

    public static Element getSourceWithSourceType(MRRequest request, String NID, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory,
        String sourceType) throws Exception {
        Element groups = null;
        MRRequestCache cache = request.getCache();
        String key = "SourceGroup" +"."+NID;
        if(cache != null && cache.containsKey(key)) {
            groups = (Element) cache.get(key);
            //return ca copy if not null
            if (groups != null) {
                return groups.createCopy();
            }
        }

        if ((sourceType != null) && sourceType.equals(MediaRoomServiceConst.INPUT_ENCODER)) {
            groups = DBServiceUtils.getUserAndResource(request, NID, "Source", xmpp, log, db, resourceFactory, serviceAgentFactory);
        } else {
            String portNID = MediaRoomServiceUtils.getPortNID(NID, request, db, log);
            groups = DBServiceUtils.getUserAndResource(request, NID, portNID, "MediaStorePortResource", "Source", xmpp, log, db, resourceFactory, serviceAgentFactory);
       }

        if(cache !=null) {
        	cache.put(key, groups);
        }
        
        return groups;
    }

    public static Document getPolicyAdminRequest(MRLog log, String serviceName, String requestName)
        throws Exception {
        String xmlFilePath = "/com/ipvs/policyadminservice/xml/" + requestName + ".xml";

        return MRXMLUtils.loadXMLResource(xmlFilePath, log);
    }

    public static Document getMediaRoomRequest(MRLog log, String serviceName, String requestName)
        throws Exception {
        String xmlFilePath = "/com/ipvs/mediaroomservice/xml/" + requestName + ".xml";

        return MRXMLUtils.loadXMLResource(xmlFilePath, log);
    }

    public static Document getPolicyAdminResponse(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory, String serviceName,
        String requestName, Document requestData) throws Exception {

XMPPTransaction.startBlock("getPolicyAdminResponse");
        Document doc = MediaRoomServiceUtils.getPolicyAdminRequest(log, serviceName, requestName);
        Element mroot = doc.getRootElement();
        MRXMLUtils.removeChild(mroot,"data");

        Element temp = requestData.getRootElement();
        mroot.add((Element)temp.detach());

        Document policyRequestDoc = MRXMLUtils.elementToDocument(mroot);
        MRMessage policyMessage = new MRMessage(policyRequestDoc);
        MRRequest policyRequest = MRRequest.createRequest(request.getUserJID(), request.getLogLevel(), request.getNID(), serviceName, requestName, policyMessage, request.getResourceGroup());
        policyRequest.setCache(request.getCache());
        String handlerName = "com.ipvs.policyadminservice.handler." + requestName + "Handler";
        policyRequest.setSuperUser(request.isSuperUser());

        MRRequestHandlerI handler = (MRRequestHandlerI) Class.forName(handlerName).newInstance();
        Document policyResult = handler.getResponse(policyRequest, xmpp, log, db, resourceFactory, serviceAgentFactory);
XMPPTransaction.endBlock("getPolicyAdminResponse");

        return policyResult;
    }

   
    public static Document getMediaRoomResponse(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory, String serviceName,
            String requestName, Document requestData) throws Exception {
            Document doc = MediaRoomServiceUtils.getMediaRoomRequest(log, serviceName, requestName);
            Element mroot = doc.getRootElement();
            MRXMLUtils.removeChild(mroot,"data");

            Element temp = requestData.getRootElement();
            mroot = MRXMLUtils.addElement(mroot,(Element)temp.detach());

            Document policyRequestDoc = MRXMLUtils.elementToDocument(mroot);
            MRMessage policyMessage = new MRMessage(policyRequestDoc);
            MRRequest policyRequest = MRRequest.createRequest(request.getUserJID(),request.getLogLevel(), request.getNID(), request.getServiceName(), requestName, policyMessage , request.getResourceGroup(), request.getAppServerJID(), request.getUserAgentName(), request.getClientData());
            policyRequest.setCache(request.getCache());
            String handlerName = "com.ipvs.mediaroomservice.handler." + requestName + "Handler";
            //policyRequest.setSuperUser(request.isSuperUser());
            MRRequestHandlerI handler = (MRRequestHandlerI) Class.forName(handlerName).newInstance();
// System.out.println("MediaRoomServiceUtils:getMediaRoomResponse request#=" + request.hashCode() + " policyRequest=" + policyRequest.hashCode());

            Document policyResult = handler.getResponse(policyRequest, xmpp, log, db, resourceFactory, serviceAgentFactory);

            return policyResult;
    }

    /**********************/
    public static Document getAssetAdminResponse(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory, String serviceName,
        String requestName, Document requestDoc) throws Exception {
        MRMessage policyMessage = new MRMessage(requestDoc);
        MRRequest policyRequest = MRRequest.createRequest(request.getUserJID(), request.getLogLevel(), request.getNID(), serviceName, requestName, policyMessage, request.getResourceGroup());
        policyRequest.setCache(request.getCache());
        String handlerName = "com.ipvs.assetadminservice.handler." + requestName + "Handler";
        policyRequest.setSuperUser(true);

        MRRequestHandlerI handler = (MRRequestHandlerI) Class.forName(handlerName).newInstance();
        Document policyResult = handler.getResponse(policyRequest, xmpp, log, db, resourceFactory, serviceAgentFactory);

        return policyResult;
    }

    public static Element cloneDataElement(MRRequest request) throws Exception {
    	//System.out.println(request.getMessage().getDocument());
    	//System.out.println(request.getRequestDataXPath());
        Element rqdata = MRXMLUtils.getElementXpath(request.getMessage().getDocument(), request.getRequestDataXPath());
        Element data = ((Element) rqdata.clone());
        //data.setName("Data");

        return data;
    }
    
    public static void executeSystemCommand(String systemCmd) throws Exception {
    	String[] cdArr = {systemCmd} ;
    	executeSystemCommand( cdArr);
    }

    public static int executeSystemCommand(String[] systemCmd) throws Exception {
        Runtime runtime = Runtime.getRuntime();
        Process proc = runtime.exec(systemCmd);
        try {
        	int procResult = proc.waitFor();             
            if (procResult != 0) {
            	InputStream inputstream = proc.getErrorStream();
                InputStreamReader inputstreamreader = new InputStreamReader(inputstream);
                BufferedReader bufferedreader = new BufferedReader(inputstreamreader);

                StringBuffer sb = new StringBuffer();
             
                String line = null;

                while ((line = bufferedreader.readLine()) != null) {
                    sb.append(line.trim());
                    sb.append("\n");
                }
                
                System.out.println("result of system command :"+sb.toString());
                System.err.println("exit value = " + proc.exitValue() + "\n Command is=" + systemCmd[0]);
            }
            return procResult;
        } catch (InterruptedException e) {
        	e.printStackTrace();
            System.err.println(e);
        }
        return -1;
    }

    public static String executeCLICommand(String[] cmdArray) throws Exception {
        Runtime runtime = Runtime.getRuntime();

        //Testing on windows
        /* String[] cmdarr = {"cmd", "/c" ,"type Z:\\work\\mediaroom\\etc\\" + cmdArray[0] +".out"};
          Process proc = runtime.exec(cmdarr);*/
        cmdArray[0] = "/V2O/bin/" + cmdArray[0];

        Process proc = runtime.exec(cmdArray);

        //put a BufferedReader on the ls output
        InputStream inputstream = proc.getInputStream();
        InputStreamReader inputstreamreader = new InputStreamReader(inputstream);
        BufferedReader bufferedreader = new BufferedReader(inputstreamreader);

        StringBuffer sb = new StringBuffer();
        sb.append(cmdArray[0]);
        sb.append("\n");

        String line = null;

        while ((line = bufferedreader.readLine()) != null) {
            sb.append(line.trim());
            sb.append("\n");
        }

        try {
            if (proc.waitFor() != 0) {
                System.err.println("exit value = " + proc.exitValue() + "\n Command is=" + cmdArray[0]);
            	throw new MRException(MediaRoomServiceExceptionCodes.EXECUTION_OF_COMMANDS_FIALED,"Execution of command failed" );
            }
        } catch (InterruptedException e) {
            System.err.println(e);
        } finally {
            bufferedreader.close();
        }

        return sb.toString();
    }
    
    public static String getTrackNID(String roomNID, String trackName) {
    	return roomNID + "." + trackName;
    }
    

    public static final Document getTrackStateMachine(MRLog log) throws Exception {
        if (gTrackStateMachine == null) {
            gTrackStateMachine = MRXMLUtils.loadXMLResource(MediaRoomServiceUtils.TRACK_STATEMACHINE_XML, log);

            // System.out.println(MRXMLUtils.documentToString(gTrackStateMachine));
        }

        return gTrackStateMachine;
    }

    public static Element createActionElement(String streamNID, String profileXML, String profileNID, String updateAction, MRLog log)
        throws Exception {
        Document doc = MRXMLUtils.loadXMLResource(MediaRoomServiceUtils.ACTION_INSTANCE_XML, log);
        MRXMLUtils.getElementXpath(doc, "//Action/data/streamNID").setText(streamNID);
        MRXMLUtils.getElementXpath(doc, "//Action/data/profileXML").setText(profileXML);
        MRXMLUtils.getElementXpath(doc, "//Action/data/profileNID").setText(profileNID);
        MRXMLUtils.getElementXpath(doc, "//Action/data/updateAction").setText(updateAction);

        Element actionElement = MRXMLUtils.getElementXpath(doc, "//Action");

        return actionElement;
    }

    public static Element createTrackActionElement(String trackNID, String updateAction, String actiondata, MRLog log)
        throws Exception {
        Document doc = MRXMLUtils.loadXMLResource(MediaRoomServiceUtils.TRACKACTION_INSTANCE_XML, log);
        MRXMLUtils.getElementXpath(doc, "//TrackAction/data/trackNID").setText(trackNID);
        MRXMLUtils.getElementXpath(doc, "//TrackAction/data/updateAction").setText(updateAction);
        MRXMLUtils.getElementXpath(doc, "//TrackAction/data/actiondata").setText(actiondata);

        Element actionElement = MRXMLUtils.getElementXpath(doc, "//TrackAction");

        return actionElement;
    }

    public static Document createAddDestFileActionRequest(String action, String actionData, MRLog log)
        throws MRException, Exception {
        Document requestDoc = MRXMLUtils.loadXMLResource("/com/ipvs/mediaroomservice/xml/AddDestFileActionRequest.xml", log);

        Element data = MRXMLUtils.stringToElement(actionData);
        Element dataEL = MRXMLUtils.getElementXpath(requestDoc, "//" + MRXMLUtils.getAttributeValueXpath(requestDoc,MRConst.HEADER_XPATH, MRConst.REQUEST_NAME, "") + "Data");
        //System.out.println("test ->" + MRXMLUtils.getAttributeValueXpath(requestDoc,MRConst.HEADER_XPATH, MRConst.REQUEST_NAME, ""));
        MRXMLUtils.replaceContent(dataEL, data);
        return requestDoc;
    }

    public static Document createSSEventMessage(String actiondata,MRLog log) throws Exception {
    	Document SSEventDoc = MRXMLUtils.loadXMLResource(MediaRoomServiceUtils.SS_EVENT_INSTANCE_XML, log);
    	//MRXMLUtils.getElementXpath(SSEventDoc, "//SSEventActionData").setContent(MRXMLUtils.stringToElement(actiondata));
        Element el = MRXMLUtils.getElementXpath(SSEventDoc, "//SSEventActionData");
        el.clearContent();
        el.add(MRXMLUtils.stringToElement(actiondata));
    	MRXMLUtils.getElementXpath(SSEventDoc, "//action").setText(MediaRoomServiceConst.SSEVENT);
    	return SSEventDoc;
    }

    

    public static String generateErrorXML(int code, String data) {
        return ("<Exception><Code>" + code + "</Code><Description>" + data + "</Description></Exception>");
    }

    public static void sendMessageToAppServerAgent(XMPPI xmpp, String userJID, Document requestDoc, MRLog log, String requestName, ServiceAgentFactoryI serviceAgentFactory)
    throws Exception {
    	 String fromAgentClassName = MRClientAgent.class.getName();
    	 String appserverAgentID =  AppServerAgentUtils.getAppServerAgentJID(userJID,serviceAgentFactory);
    	sendMessageToAppServerAgent(xmpp, userJID, requestDoc, log, requestName, fromAgentClassName,appserverAgentID);
    }
    public static void sendMessageToAppServerAgent(XMPPI xmpp, String userJID, Document requestDoc, MRLog log, String requestName,String fromAgentClassName,String appserverAgentID)
        throws Exception {
        String requestNID = "none";
        String roomNID = null;
        String agentClassName = AppServerAgent.class.getName();        
        String info = fromAgentClassName + ":=" + userJID + " > Send " + requestName + " to AppSeverAgent:=";       
        ServiceAgentUtils.sendAgentRequestMessage(xmpp, log, fromAgentClassName, roomNID, appserverAgentID, agentClassName, requestDoc, info, requestNID);
    }

    public static Document loadInstanceListXML(MRLog log, String serviceName, String requestName)
        throws Exception {
        String xmlFilePath = "/com/ipvs/mediaroomservice/impl/" + requestName;

        return MRXMLUtils.loadXMLResource(xmlFilePath, log);
    }
    

    public static Document requestStreamAction(String userJID, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory, String streamNID,
        String action) throws Exception {
        MRRequestHandlerI handler = new AddStreamActionRequestHandler();
        String xmlFilePath = "/com/ipvs/mediaroomservice/xml/" + SourceStreamAgent.ADDSTREAMACTION_REQUEST_XML;
        Document requestDoc = MRXMLUtils.loadXMLResource(xmlFilePath, log);
        String serviceName = MRXMLUtils.getAttributeValueXpath(requestDoc,MRConst.HEADER_XPATH, MRConst.SERVICE_NAME, "");
        String requestName = MRXMLUtils.getAttributeValueXpath(requestDoc,MRConst.HEADER_XPATH, MRConst.REQUEST_NAME, "");

        MRXMLUtils.setValueXpath(requestDoc, "//streamNID",streamNID);
        MRXMLUtils.setValueXpath(requestDoc, "//updateAction",action);

        MRMessage message = new MRMessage(requestDoc);
        String resourceGroup = "";
        MRRequest request = MRRequest.createRequest(userJID, MRLog.ERROR, "requestNID", serviceName, requestName, message, resourceGroup);
        Document response = handler.getResponse(request, xmpp, log, db, resourceFactory, serviceAgentFactory);

        return response;
    }
    
    public static String getObjectData(String roomNID, MRRequestI request, MRDB db, MRLog log, String NID)
        throws Exception {
        MediaRoom room = RoomDB.getRoomDB().getRoom(roomNID, request, log, db);
        return room.getObjectData(request, db, log, NID);
    }

    public static void setObjectData(String roomNID, MRRequestI request, MRDB db, MRLog log, String NID, String data)
        throws Exception {
        MediaRoom room = RoomDB.getRoomDB().getRoom(roomNID, request, log, db);
        room.setObjectData(request, db, log, NID, data);
    }

    public static void setObjectData(String roomNID, MRRequestI request, MRDB db, MRLog log, String NID, Element dataElement)
    throws Exception {
        MediaRoom room = RoomDB.getRoomDB().getRoom(roomNID, request, log, db);
        room.setObjectData(request, db, log, NID, dataElement);
    }

    public static boolean objectExists(String roomNID, MRRequestI request, MRDB db, MRLog log, String NID)
        throws Exception {
    	try {
	        MediaRoom room = RoomDB.getRoomDB().getRoom(roomNID, request, log, db);
	        return room.objectExists(request, db, log, NID);
    	}
    	catch(Exception e) {
    		return false;
    	}
    }
    
    public static String getRoomNID(MRRequestI request, XMPPI xmpp, MRLog log, MRDB db, String NID)
    throws Exception {
    	String xpath = "//*[@NID='" + NID + "']";
    	Document roomList = RoomDB.getRoomDB().getMediaRoomList(request, xmpp, log, db, xpath, false);
		Element element = MRXMLUtils.getElementXpath(roomList, "//MediaRoom");
    	if(element == null)
    		throw new MRException(MediaRoomServiceExceptionCodes.NOTFOUND, "<NID>" + NID + "</NID>");
		return MRXMLUtils.getAttributeValue(element,"NID");
    }

    public static String getRoomNIDWithXPath(MRRequestI request, XMPPI xmpp, MRLog log, MRDB db, String xpath)
    throws Exception {
    	Document roomList = RoomDB.getRoomDB().getMediaRoomList(request, xmpp, log, db, xpath, false);    	
		Element element = MRXMLUtils.getElementXpath(roomList, "//MediaRoom");
    	if(element == null)
    		throw new MRException(MediaRoomServiceExceptionCodes.NOTFOUND, "<xpath>" + xpath + "</xpath>");
		return MRXMLUtils.getAttributeValue(element,"NID");
    }


    public static Document readElement(String roomNID, MRRequestI request, MRLog log, MRDB db, String NID)
        throws Exception {
XMPPTransaction.startBlock("RoomDBReadElement");
        MediaRoom room = RoomDB.getRoomDB().getRoom(roomNID, request, log, db);
        Document result = room.readElement(request, log, db, NID);
XMPPTransaction.endBlock("RoomDBReadElement");
		return result;
    }

    public static Document readElementByXpath(String roomNID, MRRequestI request, MRLog log, MRDB db, 
    		String xpath) throws Exception {
        MediaRoom room = RoomDB.getRoomDB().getRoom(roomNID, request, log, db);
        return room.readElementByXpath(request, log, db, xpath);
    }

    public static void deleteElement(String roomNID, XMPPI xmpp, MRRequestI request, MRLog log, MRDB db, String NID)
        throws Exception {
    	if(roomNID.equals(NID))
    		RoomDB.getRoomDB().deleteRoom(roomNID, xmpp, request, log, db);
    	else {
            MediaRoom room = RoomDB.getRoomDB().getRoom(roomNID, request, log, db);
    		room.deleteElement(request, log, db, NID);
    	}
    }

    public static Document getMediaRoomList(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory, Element whereAssistXML,
        String whereXPath, boolean includeObserverData) throws Exception {
        return RoomDB.getRoomDB().getMediaRoomList(request, xmpp, log, db, whereXPath, includeObserverData);
    }
    
    public static Document getSourceStreamDataDoc(String sourceNID,String userMediaSourceNID,XMPPI xmpp, MRRequest request, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory)
    throws Exception {
    	String whereXpath = "//MediaRoom[//MediaSource/data[sourceNID='"+sourceNID+"' and userRoomMediaSourceNID='"+userMediaSourceNID+"']]";
    	Document roomListDoc = MediaRoomServiceUtils.getMediaRoomList(request, xmpp, log, db, resourceFactory, serviceAgentFactory, null, whereXpath, false);
    	if(roomListDoc == null) {
    		return null;
    	}

    	List<?> roomList = MRXMLUtils.getListXpath(roomListDoc, "//MediaRoom");
    	
    	if( roomList==null || roomList.isEmpty()) {
    		return null;
    	}
    	
    	Document roomDoc = null;
    	String mediaSourceDataXpath = "//MediaSource[data/sourceNID='"+sourceNID+"' and data/userRoomMediaSourceNID='"+userMediaSourceNID+"']";
    	Element mediaSourceEL = null;
    	String mediaSourceNID = null;
    	String mediaSourceAgentJID = null;
    	String streamXpath = null;
    	Element streamEL = null;
    	for(Object object : roomList) {
    		roomDoc = MRXMLUtils.elementToDocument((Element)object);
    		mediaSourceEL = MRXMLUtils.getElementXpath(roomDoc, mediaSourceDataXpath);    	
    		mediaSourceNID = MRXMLUtils.getAttributeValue(mediaSourceEL, "NID", "");    		
    		mediaSourceAgentJID = MRXMLUtils.getValueXpath(mediaSourceEL, "data/sourceAgentJID", "");    		
    		streamXpath = "//Stream[data/sourceAgentJID='"+mediaSourceAgentJID+"' and data/mediaSourceNID='"+mediaSourceNID+"']";    		
    		streamEL = MRXMLUtils.getElementXpath(roomDoc, streamXpath);    		
    		if(streamEL != null) {
    			return MRXMLUtils.elementToDocument(streamEL);
    		}
    	}        
    	return null;
    }
    

    public static Document getMediaFileListRequestDoc(String recordID, MRLog log)
        throws MRException, Exception {
        String xPath = "//MetaData[Tag[@Name='ID']='" + recordID + "']";
        Document requestDoc = MRXMLUtils.loadXMLResource(CreateRecordingRequestHandler.MEDIAROOM_XMLFILEPATH + "GetSrcMediaFileListRequest.xml", log);
        MRXMLUtils.setValueXpath(requestDoc, "//xPath", xPath);

        return requestDoc;
    }

    public static Element getMediaStreamFileResource(String recordID, MRRequest request, String userJID, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
        ServiceAgentFactoryI serviceAgentFactory) throws MRException, Exception {
        Document requestDoc = MediaRoomServiceUtils.getMediaFileListRequestDoc(recordID, log);

        Document responseDoc = getMediaRoomResponse(request, userJID, xmpp, log, db, resourceFactory, serviceAgentFactory, ServiceClient.MEDIAROOMSERVICE, "GetSrcMediaFileListRequest", requestDoc);
        Element fileEL = MRXMLUtils.getElementXpath(responseDoc, "//" + AMConst.MEDIA_STREAM_FILE_RESOURCE);

        return fileEL;
    }

    public static Document getMediaRoomResponse(MRRequest request, String userJID, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory,
        String serviceName, String requestName, Document requestDoc) throws Exception {
        MRRequest mediaroomRequest = MRRequest.createRequest(userJID, request.getLogLevel(), request.getNID(), serviceName, requestName, new MRMessage(requestDoc), request.getResourceGroup(),request.getAppServerJID());
        mediaroomRequest.setCache(request.getCache());
        String handlerName = "com.ipvs.mediaroomservice.handler." + requestName + "Handler";
        MRRequestHandlerI handler = (MRRequestHandlerI) Class.forName(handlerName).newInstance();
        Document responseDoc = handler.getResponse(mediaroomRequest, xmpp, log, db, resourceFactory, serviceAgentFactory);

        return responseDoc;
    }

    public static String getMediaStreamFileResourceNID(String recordID, MRRequest request, String userJID, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
        ServiceAgentFactoryI serviceAgentFactory) throws MRException, Exception {
        Element fileEL = MediaRoomServiceUtils.getMediaStreamFileResource(recordID, request, userJID, xmpp, log, db, resourceFactory, serviceAgentFactory);       
        if (fileEL == null) {
            return null;
        }
        String fileNID = MRXMLUtils.getAttributeValue(fileEL, "NID", "");
        return fileNID;
    }

    public static void checkAvailableDiskSpace(String sourceAgentJID,String fileNID,
    		String parentDirNID) throws MRException, Exception {
       
        try {
             MediaStoreAdminServerAgent mediaStoreAdminServerAgent = MediaStoreAdminServerAgent.getMediaStoreAgent(sourceAgentJID);         	
             mediaStoreAdminServerAgent.checkAvailableDiskSpace(parentDirNID);
           
        } catch (MRException exp) {
            exp.printStackTrace();
            throw new MRException(exp.getCode(), exp.getData());
        } catch (Exception exp) {
            exp.printStackTrace();
            throw new MRException(MediaStoreServiceExceptionCodes.GET_AVAILABLE_DISK_SPACE_FAILED, exp.toString());
        }
    }
    
    public static void updateDestFileStreamProfile(
    		String fileNID, String streamType, Element profileXML, 
    		String roomNID, String roomName, String roomStartWallClock, XMPPI xmpp, MRLog log,MRDB db, String userJID,MRRequest request,ServiceAgentFactoryI serviceAgentFactory,MRResourceFactoryI resourceFactory) throws Exception {
        String xmlFilePath = "/com/ipvs/mediaroomservice/xml/" + MediaRoomServiceUtils.UPDATEDESTFILEPROFILE_REQUEST_XML;
        Document requestDoc = MRXMLUtils.loadXMLResource(xmlFilePath, log);
        
        MRXMLUtils.getElementXpath(requestDoc, "//MediaStreamFileResourceNID").setText(fileNID);
        MRXMLUtils.getElementXpath(requestDoc, "//streamType").setText(streamType);
        MRXMLUtils.copyContent(requestDoc, "//profileXML" , profileXML);
        MRXMLUtils.getElementXpath(requestDoc, "//roomNID").setText(roomNID);
        MRXMLUtils.getElementXpath(requestDoc, "//roomName").setText(roomName);
        MRXMLUtils.getElementXpath(requestDoc, "//roomStartWallClock").setText(roomStartWallClock);
        
        String clientData = request.getClientData();
        clientData = String.valueOf(System.currentTimeMillis());
        MRRequest mediaroomRequest = MRRequest.createRequest(request.getAppServerJID(), request.getLogLevel(), request.getNID(), ServiceClient.MEDIAROOMSERVICE, "UpdateDestFileProfileRequest", new MRMessage(requestDoc), request.getResourceGroup(), request.getAppServerJID(), request.getUserAgentName(), clientData);
        mediaroomRequest.setCache(request.getCache());
        MediaRoomServiceUtils.getMediaRoomResponse(mediaroomRequest, xmpp, log, db, resourceFactory, serviceAgentFactory, "mediaroomservice","UpdateDestFileProfileRequest",requestDoc);

       /* String agentJID = MRClientAgent.getAppServerAgentJID(userJID);
        String agentClassName = AppServerAgent.class.getName();
        String fromAgentClassName = agentClassName;
        String info = "MediaRoomServiceUtils::updateDestFileProfile > Sending Request UpdateDestFileProfile";
        String requestNID = "none";
        
        // System.out.println(MRXMLUtils.documentToString(requestDoc));
        ServiceAgentUtils.sendAgentRequestMessage(xmpp, log, fromAgentClassName, null, agentJID, agentClassName, requestDoc, info, requestNID);*/
    }
    
    public static void updateTrackSyncData(String roomNID, String mediaSourceNID, MRRequest request, MRDB db, MRLog log) throws Exception {
        Document mediaSourceDataDoc = MRXMLUtils.stringToDocument(
        		MediaRoomServiceUtils.getObjectData(roomNID, request, db, log, mediaSourceNID));
        String trackNID = MRXMLUtils.getValueXpath(mediaSourceDataDoc, "//trackNID", "");
        Document trackDataDoc = MRXMLUtils.stringToDocument(
        		MediaRoomServiceUtils.getObjectData(roomNID, request, db, log, trackNID));
        MRXMLUtils.getElementXpath(trackDataDoc, "//" + MediaRoomServiceConst.SYNCDATA).setText(mediaSourceNID);
        MediaRoomServiceUtils.setObjectData(roomNID, request, db, log, trackNID, MRXMLUtils.documentToString(trackDataDoc));
    }

    public static String encodeToFileSDPTemplate(String sdp) throws Exception  {
    	return URLEncoder.encode(sdp, "UTF-8");
    }

    public static String decodeFromFileSDPTemplate(String sdp) throws Exception  {
    	return URLDecoder.decode(sdp, "UTF-8");
    }

    
    
     public static List<?> getStreamNIDList (MRRequest request , MRLog log, MRDB db, String roomNID , String xpath) throws Exception {
        MediaRoom room = RoomDB.getRoomDB().getRoom(roomNID, request, log, db);
    	Document roomDoc = room.readElement(request, log, db, roomNID);
    	return MRXMLUtils.getListXpath(roomDoc, xpath);
    }
     
     public static void setMediaUUIDInDataElement(Document streamDataDoc,MRRequest request,Element data,MRLog log,MRDB db) throws MRException,Exception{
    	 String mediaSourceNID = MRXMLUtils.getValueXpath(streamDataDoc, "//mediaSourceNID", "");
    	 String roomNID = MRXMLUtils.getValueXpath(streamDataDoc, "//roomNID", "");
    	 Document mediaSourceDataDoc = MRXMLUtils.stringToDocument(MediaRoomServiceUtils.getObjectData(roomNID, request, db, log, mediaSourceNID));
    	 String sourceType = MRXMLUtils.getValueXpath(mediaSourceDataDoc, "//sourceType", "");
    	 String mediaStreamFileNID = null;
    	 if( MediaRoomServiceConst.INPUT_MEDIAFILE.equals(sourceType)) {
    		 mediaStreamFileNID = MRXMLUtils.getValueXpath(streamDataDoc, "//sourceNID", "");
    	 } else {          
    		throw new MRException(MediaRoomServiceExceptionCodes.ACTION_NOT_SUPPORTED,"Action Not Supported in Live Mode");
    	 }
    	 Document mediaStreamFileDoc   = MRXMLDBUtils.readElement(request, log, db, mediaStreamFileNID);
    	 String mediaStreamFileUUID = MediaStoreServiceUtils.getMediaStoreFileNID(mediaStreamFileDoc, request, log, db);
    	 String parentDirNID = MediaStoreServiceUtils.getParentDirNID(mediaStreamFileDoc, request, log, db);

    	 MRXMLUtils.addElement(data,MRXMLUtils.newElement("mediaUUID"));
    	 MRXMLUtils.addElement(data,MRXMLUtils.newElement("prentDirNID"));
    	 MRXMLUtils.setValueXpath(data, "mediaUUID", mediaStreamFileUUID);
    	 MRXMLUtils.setValueXpath(data, "prentDirNID", parentDirNID);
     }
     
     public static Document handleGetRequest(String pemXPath,String dbObjectName,String serviceName, MRRequest request,XMPPI xmpp,MRLog log,MRDB db,ServiceAgentFactoryI serviceAgentFactory,MRResourceFactoryI  resourceFactory) throws MRException,Exception {
    	 Element getRequestEL = MRXMLUtils.getElementXpath(request.getMessage().getDocument(),"//"+AMConst.GET_REQUEST);
    	 
    	 Document responseDoc = request.createResponseData();
    	 MRDBHelper dbHelper = new MRDBHelper();
    	 dbHelper.doInit(serviceName, dbObjectName, responseDoc , request, xmpp, log, db,resourceFactory, serviceAgentFactory);
    	 dbHelper.doGet(getRequestEL, AMConst.GET_RESPONSE, dbObjectName, pemXPath);
    	 return responseDoc;
     }

     public static void updateStreamState(XMPPI xmpp, MRRequest request, MRDB db, MRLog log, String roomNID, String streamNID, String xpath, String state) throws Exception {
    	 RoomDB.getRoomDB().updateStreamState(xmpp, request, db, log, roomNID, streamNID, state);
     }

     public static void updatePresence(XMPPI xmpp, MRRequest request, MRLog log, MRDB db, String roomNID, String NID, String presence) throws Exception {
    	 RoomDB.getRoomDB().updatePresence(xmpp, request, log, db, roomNID, NID, presence);
     }

     public static void updateStreamStatus(XMPPI xmpp, MRRequest request, MRDB db, MRLog log, 
     		String roomNID, String streamNID, Element streamStatus) throws Exception {
    	 RoomDB.getRoomDB().updateStreamStatus(xmpp, request, db, log, roomNID, streamNID, streamStatus);
     }
     
     public static void deleteStream(XMPPI xmpp, MRRequest request, MRLog log, MRDB db,   
    		 String roomNID, String streamNID) throws Exception {
     	 RoomDB.getRoomDB().deleteStream(xmpp, request, db, log, roomNID, streamNID);
      }
     
     public static void sendRequestToMediaStoreToUpdateMediaPlayLength(Document streamDoc,MRRequest request, XMPPI xmpp, MRLog log,
    		MRDB db, MRResourceFactoryI resourceFactory,
    		ServiceAgentFactoryI serviceAgentFactory) throws MRException,Exception {
    	 String  recordingFileNID = MRXMLUtils.getValueXpath(streamDoc, "//destNID", "");
    	 if(recordingFileNID == null || recordingFileNID.isEmpty()) {
    		 return;
    	 }
    	 String  mediaStoreAgentJID  = MRXMLUtils.getValueXpath(streamDoc, "//destAgentJID", "");
    	 String  recordingDirNID = MRXMLUtils.getValueXpath(streamDoc, "//destFileParentDirNID", "");
    	 String  recordingFileUUID = MRXMLUtils.getValueXpath(streamDoc, "//destFileUUID", "");
    	 MediaStoreServiceUtils.sendRequestToMediaStoreAgent(mediaStoreAgentJID, recordingFileUUID, MediaStoreServiceConst.UPDATE_MEDIA_PLAYLENGTH_REQUEST, recordingFileNID, recordingDirNID, null, MRRequest.createRequest(request.getUserJID()), xmpp, log);
     }

     public static void printlog(String logString){
    	 if(MediaRoomServiceConst.ENABLE_LOG) {
    		 System.out.println(logString);
    	 }	 
     }
     
     public static void deleteMediaRoom(XMPPI xmpp, MRDB db, MRLog log,ServiceAgentFactoryI serviceAgentFactory,MRResourceFactoryI  resourceFactory,String roomNID,String userJID) throws Exception {

         String xmlFilePath = "/com/ipvs/mediaroomservice/xml/" + SourceStreamAgent.DELETEMEDIAROOM_REQUEST_XML;
         Document requestDoc = MRXMLUtils.loadXMLResource(xmlFilePath, log);
         MRXMLUtils.getElementXpath(requestDoc, "//roomNID").setText(roomNID);
         MRXMLUtils.getElementXpath(requestDoc, "//roomGC").setText(MRConst.TRUE);
         MRRequest request = MRRequest.createRequest(userJID, requestDoc);
         MediaRoomServiceUtils.getMediaRoomResponse(request, xmlFilePath, xmpp, log, db, resourceFactory, serviceAgentFactory, ServiceClient.MEDIAROOMSERVICE, MRRequest.getRequestName(DeleteMediaRoomRequestHandler.class.getName()), requestDoc);
     }
     

}
