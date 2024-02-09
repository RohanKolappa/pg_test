package com.ipvs.assetadminservice.impl;

import java.net.URLDecoder;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.assetadminservice.handler.AMConst;
import com.ipvs.assetadminservice.handler.AMExceptionCodes;
import com.ipvs.client.ServiceAgentUtils;
import com.ipvs.client.ServiceClient;
import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.MRRequestI;
import com.ipvs.common.XMPPI;
import com.ipvs.common.db.CreateRequestXmlUtil;
import com.ipvs.common.db.MRConfig;
import com.ipvs.common.db.MRDBHelper;
import com.ipvs.common.db.MRXMLDBUtils;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.policyadminservice.utils.PolicyAdminServiceUtils;
import com.ipvs.policyadminservice.utils.PolicyAdminConstants.StreamTypeList;
import com.ipvs.sessionservice.impl.SessionServiceConst;


public class AssetAdminServiceUtils {
    public static Document getInstanceListDocument(String objectType, MRLog log)
        throws MRException, Exception {
        if ((objectType == null) || objectType.equals("")) {
            return null;
        }

        Document objectDoc = MRConfig.getObjectXMLDocument(ServiceClient.ASSETADMINSERVICE, objectType, log);

        if (objectDoc == null) {
            return null;
        }

        return MRXMLUtils.getDocumentXpath(objectDoc, "//InstanceList");
    }

    public static Document createAssetAdminServiceRequest(String requestName, String nid, Element OperationData, MRLog log)
        throws Exception {
        return createAssetAdminServiceRequest(requestName, nid, null, OperationData, null, log);
    }

    public static Document createAssetAdminServiceRequest(String requestName, String nid, String depth, Element OperationData, MRLog log)
        throws Exception {
        return createAssetAdminServiceRequest(requestName, nid, depth, OperationData, null, log);
    }

    public static Document createAssetAdminServiceRequest(String requestName, String whereXpath, MRLog log)
        throws Exception {
        return createAssetAdminServiceRequest(requestName, null, null, null, whereXpath, log);
    }

    public static Document createAssetAdminServiceRequest(String requestName, Element operationData, MRLog log)
        throws Exception {
        return createAssetAdminServiceRequest(requestName, null, null, operationData, null, log);
    }

    public static Document createAssetAdminServiceRequest(String requestName, String nid, String depth, Element OperationData, String whereXPath, MRLog log)
        throws Exception {
        Document requestDoc = AssetAdminServiceUtils.getAssetAdminRequest(log, requestName);

        if (nid != null) {
            Element NIDElement = MRXMLUtils.getElementXpath(requestDoc, "//NID");
            NIDElement.setText(nid);
        }

        if (OperationData != null) {
            Element operationDataElement = MRXMLUtils.getElementXpath(requestDoc, "//OperationData");
            MRXMLUtils.replaceContentWithElement(operationDataElement,OperationData, false);
        }

        //not supported in new api. server neglects in old api
        /*if (depth != null) {
            Element NodeDepthXMLElement = MRXMLUtils.getElementXpath(requestDoc, "//NodeDepth");
            NodeDepthXMLElement.add(depth);
        }*/
        if (whereXPath != null) {
            Element whereXPathElement = MRXMLUtils.getElementXpath(requestDoc, "//WhereXPath");
            whereXPathElement.setText(whereXPath);
        }

        return requestDoc;
    }

    public static Document createEmptyRecycleBinRequest(String dirNID,String action, MRLog log)
        throws MRException, Exception {
        //Document requestDoc = getAssetAdminRequest(log, AMConst.EMPTY_RECYCLE_BIN_REQUEST + ".xml");
        Document requestDoc = MRXMLUtils.loadXMLResource("/com/ipvs/assetadminservice/xml/" + AMConst.EMPTY_DIR_REQUEST + ".xml", log);
        MRXMLUtils.setValueXpath(requestDoc, "//" + com.ipvs.sessionservice.resource.handler.EmptyDirRequestHandler.ACTION_REQUEST_ELEMENT_NAME, action);
        MRXMLUtils.setValueXpath(requestDoc, "//" + AMConst.MEDIA_STREAM_DIR_NID, dirNID);

        return requestDoc;
    }
    
    public static Document getAssetAdminRequest(MRLog log,String requestName) throws Exception {
    	return getAssetAdminRequest(log, "" , requestName);
    }

    public static Document getAssetAdminRequest(MRLog log, String subServiceName,  String requestName)
        throws Exception {
        //todo: clean up: test after deleting all request xmls. 
        try {
            if (requestName.indexOf("_BASE_") != -1) {
                //long t0= System.currentTimeMillis();
                Document doc = CreateRequestXmlUtil.getInstance().loadXMLResource(requestName);

                //System.out.println(( System.currentTimeMillis() - t0) + " " + requestName );//+ " "+ MRXMLUtils.documentToString(doc));
                if (doc != null) {
                    return doc;
                }
            }
        } catch (Exception ex) {
            ex.printStackTrace();
        }

        String xmlFilePath = "/com/ipvs/assetadminservice/" ;
        if(subServiceName !=null && !subServiceName.isEmpty()) {
        	xmlFilePath += subServiceName +"/";
        }
        xmlFilePath += "xml/"+requestName;        

        return MRXMLUtils.loadXMLResource(xmlFilePath, log);
    }

    //returns the handler name
    public static String getHandlerClassName(String requestName) {
        return "com.ipvs." + ServiceClient.ASSETADMINSERVICE + ".handler." + requestName + "Handler";
    }

    //create event xml
    public static String createEmptyRecycleBinEventMessage(String dirNID,String action, int eventCode) {
        return "<data>" +"<MediaStreamDirNID>" + dirNID + "</MediaStreamDirNID>" + "<action>" + action + "</action>" + "<eventCode>" + eventCode + "</eventCode>" + "</data>";
    }

    public static Document getAssetResource(String userJID, MRDB db, MRLog log)
        throws MRException, Exception {
        String assetResourceTitle = ServiceAgentUtils.getResourceTitle(userJID);

        if (assetResourceTitle == null) {
            return null;
        }

        String[] whereXpath = { "//" + AMConst.AssetResource + "[Info/Title='" + assetResourceTitle + "']" };
        Element response = MRXMLUtils.newElement("AssetResourceList");
        MRConfig config = MRConfig.getInstance(ServiceClient.ASSETADMINSERVICE, AMConst.AssetResource, log);
        String parentNID = db.getServiceDomainRootNID(config.getParentNIDKey(log));

        MRXMLDBUtils.findChildElements(parentNID, response, null, 0, null, whereXpath, MRRequest.createRequest(userJID), db, log);

        Document assetDoc = MRXMLUtils.getDocumentXpath(MRXMLUtils.elementToDocument(response), "//" + AMConst.AssetResource);

        return assetDoc;
    }

    public static boolean isDMSDevice(String userJID, MRDB db, MRLog log) throws MRException, Exception {
        Document assetResourceDoc = getAssetResource(userJID, db, log);

        if (assetResourceDoc == null) {
            return false;
        }

        return isDMSDevice(assetResourceDoc);
    }

    public static boolean isDMSDevice(Document assetResourceDoc) throws MRException, Exception {
        if (assetResourceDoc == null) {
            return false;
        }

        String type = MRXMLUtils.getValueXpath(assetResourceDoc, "//Type", "");

        if (type.equals(AMConst.DMS_1000_DEVICE_OBJECTYPE) || type.equals(AMConst.DMS_100_DEVICE_OBJECTYPE) || type.equals(AMConst.DMS_25_DEVICE_OBJECTYPE)  || type.equals(AMConst.DMS_200_DEVICE_OBJECTYPE) || type.equals(AMConst.DMS_120_DEVICE_OBJECTYPE) || type.equals(AMConst.DMS_1500_DEVICE_OBJECTYPE)) {
            return true;
        }

        return false;
    }  

    //Update Object in Data Base
    public static final Document updateAssetAdminObjectInDB(String objectType, String objectNID, Element objectEL, MRRequest request, MRLog log, MRDB db, XMPPI xmpp, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI profileFactory)
        throws MRException, Exception {
        Document assetRequestDoc = AssetAdminServiceUtils.createAssetAdminServiceRequest(objectType + "_BASE_ALL_UPDATE.xml", objectNID, (Element) objectEL.clone(), log);
        MRDBHelper amUtil = new MRDBHelper();

        Document assetResponseDoc = MRXMLUtils.getDocument();
        MRXMLUtils.addElement(assetResponseDoc, request.getResponseDataName());
        	       
        MRRequest updateDBRequest = MRRequest.createRequest(request, assetRequestDoc, request.getRequestName(), ServiceClient.ASSETADMINSERVICE);
        updateDBRequest.setSuperUser(request.isSuperUser());
        
        amUtil.doInit(ServiceClient.ASSETADMINSERVICE, objectType, assetResponseDoc, updateDBRequest, xmpp, log, db, resourceFactory, profileFactory);

        Element actionEL = MRXMLUtils.getElementXpath(assetRequestDoc, "//" + AMConst.REPLACE_REQUEST);

        //get the pem xpath 
        String pemXpath = PolicyAdminServiceUtils.getAssetAdminUpdatePEMXPath(actionEL, objectType, updateDBRequest, xmpp, log, db, resourceFactory, profileFactory);
        request.setOperatorGroups(updateDBRequest.getOperatorGroups());
        amUtil.doUpdate(actionEL, AMConst.REPLACE_RESPONSE, objectType, pemXpath);
        objectEL = MRXMLUtils.getElementXpath(assetResponseDoc, "//" + objectType);

        if (objectEL == null) {
            throw new MRException(AMExceptionCodes.UPDATE_MEDIA_STREAM_FILE_FAILED, "Update " + objectType + " Failed");
        }

        return assetResponseDoc;
    }

    public static List<?> getListOfPorts(Document deviceDoc,String assetResourceNID,MRRequest request,MRLog log,MRDB db) throws MRException, Exception {
    	if(deviceDoc!=null)
    	{
    		return MRXMLUtils.getListXpath(deviceDoc, "//ResourcePointers/*/NIDValue");
    	} 
    	
    	String query = "let $assetResourceNID :='"+assetResourceNID+"'"
    			+ "    			return<list>{for $portDoc in  collection('ipvs.default.mediastreamsrcportresourcelist')/*[.//AssetResourceNID = $assetResourceNID] |   "
    			+ "									   collection('ipvs.default.mediastreamdstportresourcelist')/* [.//AssetResourceNID =  $assetResourceNID]|"
    			+ "                                    collection('ipvs.default.mediastreamioportresourcelist')/*[.//AssetResourceNID =  $assetResourceNID] |"
    			+ "                                      collection('ipvs.default.mediastreamrelayportresourcelist')/*[.//AssetResourceNID =   $assetResourceNID] |"
    			+ "                                       collection('ipvs.default.mediastoreportresourcelist')/*[.//AssetResourceNID =   $assetResourceNID]"
    			+ "     return $portDoc}</list>";
    	
    	String result = MRXMLDBUtils.readQuery(MRXMLDBUtils.TIME_OUT, request, log, db, query, new HashMap<String, String>(), null);
    	Document resultDoc = MRXMLUtils.stringToDocument(result);
    	List<?> children = MRXMLUtils.getChildren(resultDoc.getRootElement());
    	List<Element> listOfPortsNode = new ArrayList<Element>();
    	//temporary hack to return ports information in device resource pointer format
    	for(Object object:children){
    		Element objectEl = (Element) object;
    		Element nidEl = MRXMLUtils.newElement(objectEl.getName()+"NID");
    		Element nidValueEl = MRXMLUtils.addElement("NIDValue",MRXMLUtils.getAttributeValue(objectEl, "NID",""));
    		nidEl.add(nidValueEl);
    		listOfPortsNode.add(nidValueEl);
    	}
    	return listOfPortsNode;
    }

   
	public static Document getDeviceResourcePointers(String assetResourceNID, MRRequest request, MRDB db, MRLog log, XMPPI xmpp)
        throws MRException, Exception {
        String whereXPath = "AssetResourceNID[.='"+assetResourceNID+"']";
        String selectXML = "<Device>" +"{root($VAR)/*/@NID}{$VAR//ResourcePointers}" + "</Device>";

    	Element responseEL = getChildBasedOnSelectXML(AMConst.Device,selectXML, whereXPath, request, db, log, xmpp);    	
    	if(MRXMLUtils.getElementXpath(responseEL, "./Device")!=null) 
    		return MRXMLUtils.elementToDocument(MRXMLUtils.getElementXpath(responseEL, "./Device"));
    	return null;
    		    
    }
    
    
    public static Document getDeviceIPAddressInformation(String assetResourceNID, MRRequest request, MRDB db, MRLog log, XMPPI xmpp)
    throws MRException, Exception {
    String whereXPath = "AssetResourceNID[.='"+assetResourceNID+"']";
    String selectXML = "<Device>" +"{root($VAR)/*/@NID}{$VAR//DNSClientConfig}{$VAR//NATZoneTableConfig}{$VAR//EthernetPortTableStatus}" + "</Device>";

	Element responseEL = getChildBasedOnSelectXML(AMConst.Device,selectXML, whereXPath, request, db, log, xmpp);    	
	if(MRXMLUtils.getElementXpath(responseEL, "./Device")!=null) 
		return MRXMLUtils.elementToDocument(MRXMLUtils.getElementXpath(responseEL, "./Device"));
	return null;
		    
}
    
    public static Element getChildBasedOnSelectXML(String objectType ,String selectXML,String whereXPath,MRRequest request, MRDB db, MRLog log, XMPPI xmpp) throws MRException,Exception {
      
    	long DBCallTimeOut = MRXMLDBUtils.TIME_OUT;        
    	String listName ="";
    	String[] arrWhereXpath={whereXPath};
    	String[] sortByXpath={};
    	String sortOrder ="";
    	String nid=null;
    	String childXpath ="";    	
    	Element  responseEL= MRXMLUtils.addElement("Response");
    	int startFrom = 0;
    	int countToFetch = -1;   	
    	
    	 return MRXMLDBUtils.readList(DBCallTimeOut, request, log, db, listName, objectType, nid, childXpath, arrWhereXpath, sortByXpath, 
    			sortOrder, selectXML, responseEL, startFrom, countToFetch);
    }
    
    public static Element getChildBasedOnSelectXMLNID(String nid ,String selectXML,String whereXPath,MRRequest request, MRDB db, MRLog log, XMPPI xmpp) throws MRException,Exception {
        
    	long DBCallTimeOut = MRXMLDBUtils.TIME_OUT;        
    	String listName ="";
    	String[] arrWhereXpath={whereXPath};
    	String[] sortByXpath={};
    	String sortOrder ="";
    	String objectType=null;
    	String childXpath ="";    	
    	Element  responseEL= MRXMLUtils.addElement("Response");
    	int startFrom = 0;
    	int countToFetch = -1;   	
    	
    	 return MRXMLDBUtils.readList(DBCallTimeOut, request, log, db, listName, objectType, nid, childXpath, arrWhereXpath, sortByXpath, 
    			sortOrder, selectXML, responseEL, startFrom, countToFetch);
    }

    public static Document getDeviceDoc(String deviceNID, MRDB db, MRLog log, String userJID) throws MRException, Exception {
        if ((deviceNID == null) || deviceNID.isEmpty()) {
            return null;
        }

        return MRXMLDBUtils.readElement(MRRequest.createRequest(userJID), log, db, deviceNID);
    }

   
    public static boolean isMediaRoomRequest(String clientData) {
        if ((clientData == null) || clientData.isEmpty()) {
            return false;
        }

        try {
            MRXMLUtils.stringToDocument(URLDecoder.decode(clientData, "UTF-8"));

            return true;
        } catch (Exception exp) {
            return false;
        }
    }
    
    public static Element getChild(String nid,String childXpath,MRRequestI request, MRLog log, MRDB db, XMPPI xmpp, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws MRException,Exception {
    	long DBCallTimeOut = MRXMLDBUtils.TIME_OUT;
    	String listName ="";
    	String objectType="";
    	String[] arrWhereXpath={};
    	String[] sortByXpath={};
    	String selectXML ="";
    	String sortOrder ="";
    	Element  responseEL= MRXMLUtils.addElement("Response");
    	int startFrom = 0;
    	int countToFetch = -1;   	
    	
    	MRXMLDBUtils.readList(DBCallTimeOut, request, log, db, listName, objectType, nid, childXpath, arrWhereXpath, sortByXpath, 
    			sortOrder, selectXML, responseEL, startFrom, countToFetch);
    	
    	return responseEL;
    }
    
    
    public static void deleteChild(String nid,String childXpath,MRRequestI request, MRLog log, MRDB db, XMPPI xmpp, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws MRException,Exception {
    	long DBCallTimeOut = MRXMLDBUtils.TIME_OUT;
    	String listName ="";
    	String objectType="";
    	String[] arrWhereXpath={};
    		
    	MRXMLDBUtils.delete(DBCallTimeOut, request, log, db, listName, objectType, nid, childXpath, arrWhereXpath);
    	
    }
    
    public static Document addChild(String nid,String childXpath,Element childEL,MRRequestI request, MRLog log, MRDB db, XMPPI xmpp, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws MRException,Exception {
    	long DBCallTimeOut = MRXMLDBUtils.TIME_OUT;
    	String listName ="";
    	String objectType="";
    	String[] arrWhereXpath={};
     	
    	return MRXMLDBUtils.add(DBCallTimeOut, request, log, db, listName, objectType, nid, childXpath, arrWhereXpath, childEL);
    }

    public static Document updateChild(String nid,String childXpath,Element childEL,MRRequestI request, MRLog log, MRDB db, XMPPI xmpp, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws MRException,Exception {
    	long DBCallTimeOut = MRXMLDBUtils.TIME_OUT;
    	String listName ="";
    	String objectType="";
    	String[] arrWhereXpath={};
     	
    	return MRXMLDBUtils.replace(DBCallTimeOut, request, log, db, listName, objectType, nid, childXpath, arrWhereXpath, childEL);
    }
    
    public static String getPortResourceTypeFromDevice(String elementName) throws MRException,Exception {
    	if(elementName.equals(AMConst.MEDIA_STREAM_SRC_PORT_RESOURCE_NID)) {
    		return AMConst.MEDIA_STREAM_SRC_PORT_RESOURCE_OBJECTTYPE;
    	} else if(elementName.equals(AMConst.MEDIA_STORE_PORT_RESOURCE_NID)) {
    		return AMConst.MEDIA_STORE_PORT_RESOURCE;
    	} else if(elementName.equals(AMConst.MEDIA_STREAM_RELAY_PORT_RESOURCE_NID)) {
    		return AMConst.MEDIA_STREAM_RELAY_PORT_RESOURCE_OBJECTTYPE;
    	}  else if(elementName.equals(AMConst.MEDIA_STREAM_DST_PORT_RESOURCE_NID)) {
    		return AMConst.MEDIA_STREAM_DST_PORT_RESOURCE_OBJECTTYPE;
    	} else if(elementName.equals(AMConst.MEDIA_STREAM_IO_PORT_RESOURCE_NID)) {
    		return AMConst.MEDIA_STREAM_IO_PORT_RESOURCE_OBJECTTYPE;
    	} 
    	return null;
    }
    /*public static boolean sendGetOffsetRequestToSourceStreamAgentForAddUpdateBookmark(MRRequest request, XMPPI xmpp, MRDB db, MRLog log, ServiceAgentFactoryI serviceAgentFactory, MRResourceFactoryI resourceFactory, String responseHandlerName)
        throws MRException, Exception {
        Document requestDoc = request.getMessage().getDocument();

        //get the media stream file doc and from document get the agentJID
        String mediaStreamFileNID = MRXMLUtils.getValueXpath(requestDoc, "//" + MediaStoreServiceConst.MEDIA_STREAM_FILE_RESOURCE_NID, "");
        Document mediaStreamFileDoc = MediaStoreServiceUtils.getMediaStreamFileDoc(mediaStreamFileNID, request, log, db);
        String timeCode = MRXMLUtils.getValueXpath(requestDoc, "//AtTC", "");

        String mediaType = MRXMLUtils.getValueXpath(mediaStreamFileDoc, "//" + MediaStoreServiceConst.MEDIA_TYPE_ELEMENT_NAME, "");

        if (mediaType.equals(MediaRoomServiceConst.MEDIAGROUP) || mediaType.equals(MediaRoomServiceConst.PVRMEDIACLIP)) {
            MRXMLUtils.setValueXpath(requestDoc, "//Offset", timeCode);

            return false;
        } else if (mediaType.equals(MediaRoomServiceConst.PLAYLIST)) {
            //TBD 
            return false;
        }

        String mediaStorePortNID = MediaStoreServiceUtils.getMediaStorePortResourceNIDFromDirWrite(mediaStreamFileDoc, request, db, log); 
        	//MRXMLUtils.getValueXpath(mediaStreamFileDoc, "//MediaStorePortResourceNID", "");
        
        String mediaStoreStreamAgentJID = MediaRoomServiceUtils.getContactJIDForPort(request, log, db, mediaStorePortNID);

        //get the media uuid and time code of bookmark
        String mediaUUID = MediaStoreServiceUtils.getMediaStoreFileNID(mediaStreamFileDoc, request, log, db);
        
        //get the Parent Dir NID
        String parentDirNID = MediaStoreServiceUtils.getParentDirNID(mediaStreamFileDoc, request, log, db);


        //create get offset request and send request to source stream agent
        String getOffsetAtTCRequestName = MRRequest.getRequestName(GetOffsetAndTCRequestHandler.class.getName());
        Document mediaroomRequestDoc = MediaRoomServiceUtils.getMediaRoomRequest(log, ServiceClient.MEDIAROOMSERVICE, getOffsetAtTCRequestName);
        String clientData = URLEncoder.encode(MRXMLUtils.documentToString(request.getMessage().getDocument()), "UTF-8");

        String fromAgentName = MediaRoomServiceConst.APPSERVERAGENT;
        String toAgentJID = mediaStoreStreamAgentJID;
        String toAgentName = MediaRoomServiceConst.SOURCESTREAMAGENT;
        MRRequest mediaroomRequest = MRRequest.createRequest(request, mediaroomRequestDoc, getOffsetAtTCRequestName, ServiceClient.MEDIAROOMSERVICE, clientData);
        Element getOffsetAtTimeCodeRequestDataEL = MRXMLUtils.getElementXpath(mediaroomRequestDoc, mediaroomRequest.getRequestDataXPath());
        MRXMLUtils.addElement(getOffsetAtTimeCodeRequestDataEL, MRXMLUtils.newElement("mediaUUID"));
        MRXMLUtils.addElement(getOffsetAtTimeCodeRequestDataEL, MRXMLUtils.newElement("prentDirNID"));        
        MRXMLUtils.setValueXpath(mediaroomRequestDoc, "//timecode", timeCode);
        MRXMLUtils.setValueXpath(mediaroomRequestDoc, "//mediaUUID", mediaUUID);
        MRXMLUtils.setValueXpath(mediaroomRequestDoc, "//prentDirNID", parentDirNID);


        String dataStr = MRXMLUtils.elementToString(getOffsetAtTimeCodeRequestDataEL);
        AppServerAgent.sendAgentRequestForUserRequest(mediaroomRequest, xmpp, log, db, resourceFactory, serviceAgentFactory, fromAgentName, toAgentJID, toAgentName, dataStr, responseHandlerName);

        return true;
    }*/
    public static void setSourceDefaultParameters(Element portResourceElement, String mode, MRLog log) throws Exception{
        String streamType = MRXMLUtils.getValueXpath(portResourceElement, "//StreamType", "");
        String profileObjectName = streamType+AMConst.MEDIA_STREAM_PROFILE_OBJECTTYPE; 
    	Document defaultProfileDoc = MRConfig.getObjectXMLDocument(ServiceClient.ASSETADMINSERVICE, profileObjectName, log);
    	if(!streamType.equals(StreamTypeList.RTP.toString()) && !streamType.equals(StreamTypeList.MPEGTS.toString()) && !streamType.equals(StreamTypeList.UDP.toString()))
    		MRXMLUtils.setValueXpath(defaultProfileDoc, "//"+SessionServiceConst.ISMULTICAST, "true");  
    	Element profileXmlEl = MRXMLUtils.getElementXpath(defaultProfileDoc, "//Info");
    	Element srcPortProfileEl = (Element)profileXmlEl.clone();
    	String profileName = streamType+SessionServiceConst.MEDIASTREAMPROFILEINFO;
    	srcPortProfileEl.setName(profileName);
    	Element el = MRXMLUtils.getElementXpath(portResourceElement,".//"+SessionServiceConst.MEDIASTREAMPROFILEINFO+"/"+profileName);
    	el.detach();
    	Element mediaStreamProfileInfo = MRXMLUtils.getElementXpath(portResourceElement, ".//"+SessionServiceConst.MEDIASTREAMPROFILEINFO);
    	mediaStreamProfileInfo.add(srcPortProfileEl);
    	MRXMLUtils.setAttributeValueXPath(portResourceElement, ".//Instance", SessionServiceConst.MODE, mode); //story #51643
    }
}
