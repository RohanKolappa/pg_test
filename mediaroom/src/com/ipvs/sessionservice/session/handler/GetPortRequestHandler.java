package com.ipvs.sessionservice.session.handler;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.appserver.AppServerAgentUtils;
import com.ipvs.assetadminservice.handler.AMConst;
import com.ipvs.common.MRAuthorization;
import com.ipvs.common.MRConst;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.XMPPTransaction;
import com.ipvs.common.db.DBUtilsBase;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRRequestHandlerI;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.http.common.HttpAuthentication;
import com.ipvs.mediaroomservice.datamodel.MediaRoomAuthorization;
import com.ipvs.mediaroomservice.datamodel.MediaRoomObject;
import com.ipvs.mediaroomservice.impl.AppServerAgent;
import com.ipvs.mediaroomservice.impl.MediaRoomPEM;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceConst;
import com.ipvs.mediaroomservice.impl.SDPUtils;
import com.ipvs.mediastoreservice.impl.MediaStoreServiceConst;
import com.ipvs.sessionservice.handler.base.GetBaseObjectListRequestHandler;
import com.ipvs.sessionservice.impl.GetObjectUtils;
import com.ipvs.sessionservice.impl.SessionServiceConst;
import com.ipvs.sessionservice.resource.handler.SessionServiceResourceConst;

public class GetPortRequestHandler extends GetBaseObjectListRequestHandler implements MRRequestHandlerI{
	
	@Override
    public MRAuthorization authorizeRequest(MRRequest request, XMPPI xmpp,
			MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI profileFactory) throws Exception {
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
	
	// private static int requestcounter = 0 ;
	@Override
	public Document getResponse(MRRequest request, XMPPI xmpp, MRLog log,
			MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory) throws Exception {
		
			XMPPTransaction.startBlock("GetPortRequest");
			Document doc = request.getMessage().getDocument();
			Element getPortElement = MRXMLUtils.getElementXpath(doc, "//GetPort");
			String requestName = getPortElement.getName();
			HashMap<String, String> queryParams = this.getXmppRequestQueryParams(getPortElement);
			if(queryParams == null) {
				log.addInfo("Query parameters empty");
				return null;
			}
			
			// TBD MIYE added to avoid use of DBUtils
			boolean forceXMPP = DBUtilsBase.forceXMPP;
			
			Document responseDoc = generateObjectListResponse(request, xmpp, log, db, resourceFactory, serviceAgentFactory, queryParams, requestName, forceXMPP);
			return responseDoc;
	}
		
	public Document generateObjectListResponse(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory, HashMap<String, String> queryParams, String requestName, boolean forceXMPP) throws Exception {
		Document responseDoc = null;
		String selectFormatType = queryParams.containsKey("selectFormat") ? queryParams.get("selectFormat") : "BASIC";
		String xqueryFileName = SessionServiceConst.XQUERYPACKAGE + SessionServiceConst.PORT + SessionServiceConst.XQUERYXML;
		
		String whereXPath = "";
		String whereStreamTypeXPath = null;
		String modelObjectType = SessionServiceConst.MEDIA_STREAM_PORT_RESOURCE;
		String type = queryParams.containsKey("type") ? queryParams.get("type") : SessionServiceConst.MEDIA_STREAM_SRC_PORT_RESOURCE;
		String whereStreamType = queryParams.containsKey("whereStreamType") ? queryParams.get("whereStreamType") : "";
		String wherePVRMediaStreamDir = queryParams.containsKey("wherePVRMediaStreamDir") ? queryParams.get("wherePVRMediaStreamDir") : "";
		String sortBy = queryParams.containsKey("sortBy") ? queryParams.get("sortBy") : "";
		String whereState = queryParams.containsKey("whereState") ? queryParams.get("whereState") : "";
		String whereOperationMode = queryParams.containsKey("whereOperationMode") ? queryParams.get("whereOperationMode") : "";
		
		if(type.equalsIgnoreCase(SessionServiceConst.SRC_PORT)){
			modelObjectType = SessionServiceConst.MEDIA_STREAM_SRC_PORT_RESOURCE;
			whereXPath = "(//Type[.='StreamSrc' or .='StreamIO'])";
		} else if(type.equalsIgnoreCase(SessionServiceConst.DST_PORT)){
			modelObjectType = SessionServiceConst.MEDIA_STREAM_DST_PORT_RESOURCE;
			whereXPath = "(//Type[.='StreamDst' or .='StreamIO'])";
		} else if(type.equalsIgnoreCase(SessionServiceConst.RELAY_PORT)){
			modelObjectType = SessionServiceConst.MEDIA_STREAM_RELAY_PORT_RESOURCE;
			whereXPath = "(//Type != 'StreamSrc' and //Type != 'StreamDst')";
		} else if(type.equalsIgnoreCase(SessionServiceConst.STORE_PORT)){
			modelObjectType = SessionServiceConst.MEDIA_STORE_PORT_RESOURCE;
			whereXPath = "(//Type != 'StreamSrc' and //Type != 'StreamDst')";
		}
		
		if(whereStreamType != null && !"".equalsIgnoreCase(whereStreamType)) {
			whereStreamTypeXPath = GetObjectUtils.generateXPath(whereStreamType, SessionServiceConst.WHERE_STREAM_TYPE, SessionServiceConst.PORT, SessionServiceConst.STREAM_TYPE_CONST);
		}


		List<String> listOfWhereXpath = new ArrayList<String>();
		if(!modelObjectType.equals(SessionServiceConst.MEDIA_STREAM_PORT_RESOURCE)) {
			listOfWhereXpath.add(whereXPath);
		}
		if(whereStreamTypeXPath!=null && !"".equalsIgnoreCase(whereStreamTypeXPath)) {
			listOfWhereXpath.add(whereStreamTypeXPath);
		}
		
		if(!"".equals(whereState)){
			listOfWhereXpath.add(GetObjectUtils.generateXPath(whereState, SessionServiceConst.WHERE_STATE, SessionServiceConst.PORT, SessionServiceConst.STATE_CONST) );
		}
		
		if(whereOperationMode.equals(SessionServiceConst.LOCAL)) {
            listOfWhereXpath.add(GetObjectUtils.generateXPath("false", SessionServiceConst.WHERE_OPERATION_MODE, SessionServiceConst.PORT, SessionServiceConst.OPERATION_MODE_CONST) );
        } else if(whereOperationMode.equals(SessionServiceConst.REMOTE)) {
            listOfWhereXpath.add(GetObjectUtils.generateXPath("true", SessionServiceConst.WHERE_OPERATION_MODE, SessionServiceConst.PORT, SessionServiceConst.OPERATION_MODE_CONST) );
        }
		
		// Used only for internal purpose to get the PVR dirNID
		String wherePVRMediaStreamDirXPath = "";
		if (!wherePVRMediaStreamDir.isEmpty()) {
			wherePVRMediaStreamDirXPath = ".//" + SessionServiceResourceConst.PVR_INFO_ELEMENT + "/@" + 
			SessionServiceResourceConst.MEDIA_STREAM_DIR_RESOURCE_NID_ATTRIBUTE + "='" + wherePVRMediaStreamDir + "'";		
			listOfWhereXpath.add(wherePVRMediaStreamDirXPath);
		}
		
		if(sortBy.equalsIgnoreCase(SessionServiceConst.TITLE)) {
			sortBy = MediaStoreServiceConst.INFO_ELEMENT_NAME+"/"+MediaStoreServiceConst.MEDIA_TITLE_ELEMENT_NAME;
		} else if (sortBy.equalsIgnoreCase(SessionServiceConst.STREAM_TYPE)) {
			sortBy = MediaStoreServiceConst.MEDIA_STREAM_TYPE_ELEMENT_NAME;
		} else if (sortBy.equalsIgnoreCase(SessionServiceConst.STATE)) {
			sortBy = AMConst.STATEFLAG; 
		} else {
            sortBy = "";
        }
		//fix for bug #50336
		if (queryParams.containsKey("sortBy")) {
            queryParams.put("sortBy", sortBy);
        }
		
		XMPPTransaction.startBlock("getObjectListRequest");
		responseDoc = this.getObjectListRequest(request, xmpp, log, db, serviceAgentFactory, resourceFactory, SessionServiceConst.PORT, queryParams, xqueryFileName, listOfWhereXpath, modelObjectType, requestName, forceXMPP);
		
		AppServerAgent appServerAgent = AppServerAgentUtils.getAppServerAgent(serviceAgentFactory);
		Element apiKeyTableEl = appServerAgent.getAPIKey();
		
	    String expireTime = queryParams.containsKey(SessionServiceResourceConst.EXPIRE_TIME) ? queryParams.get(SessionServiceResourceConst.EXPIRE_TIME) : "";
	    String authScheme = HttpAuthentication.getAuthScheme(expireTime);
		if(expireTime.isEmpty()) {
	    	expireTime = MRXMLUtils.getAttributeValueXpath(apiKeyTableEl, "./" + SessionServiceResourceConst.API_KEY_ELEMENT_NAME, SessionServiceResourceConst.VALID_INTERVAL_ATTRIBUTE, String.valueOf(SessionServiceResourceConst.API_KEY_VALID_INTERVAL));
	    } 
	    String dstTitle = getDstTitle(MRXMLUtils.getElementXpath(request.getMessage().getDocument(), "//GetPort"), selectFormatType, request);
	    Element appResourceInfo = AppServerAgentUtils.getMSResourceInfo(log, xmpp, db, serviceAgentFactory, request, apiKeyTableEl, dstTitle, request.getUserJID(),authScheme);
	    
	    String msIPAddress =  MRXMLUtils.getAttributeValue(appResourceInfo, "ipAddress","");
	    String dstNID =MRXMLUtils.getAttributeValue(appResourceInfo, "dstNID","");
	    String dstTemplateTitle = MRXMLUtils.getAttributeValue(appResourceInfo, "templateTitle","");
	    if(!dstNID.isEmpty()) {
	    	String subscriptionID = MRXMLUtils.getAttributeValue( MRXMLUtils.getElementXpath(request.getMessage().getDocument(), "//GetPort"), SessionServiceConst.SUBSCRIPTION_ID,"");
	    	AppServerAgentUtils.getSubscriptionManager(serviceAgentFactory).setDestinationInformation(SessionServiceConst.PORT, request.getUserJID(),
	    			subscriptionID, dstNID, msIPAddress, dstTemplateTitle, authScheme,expireTime);		    	
	    }

	    long now = xmpp.getCurrentTimeMillis();
	
		//create thumbnailURL here
		List<?> portList = MRXMLUtils.getListXpath(responseDoc, "//Port");
		if(portList != null && !portList.isEmpty()) {
		  	
		    String timeStamp =(HttpAuthentication.INFINITE_EXPIRY.equals(expireTime))?HttpAuthentication.INFINITE_EXPIRY :String.valueOf(now + (Long.parseLong(expireTime) * 1000));
		    String signature = HttpAuthentication.generateSignature(apiKeyTableEl, timeStamp, authScheme, request, xmpp, log, db, resourceFactory, serviceAgentFactory);
		    String apiKey = MRXMLUtils.getAttributeValueXpath(apiKeyTableEl, "./" + SessionServiceResourceConst.API_KEY_ELEMENT_NAME, SessionServiceResourceConst.KEY_ATTRIBUTE, "");
		    
		    
			for (Object obj : portList) {
				Element portEl = (Element) obj;
				String portNID = MRXMLUtils.getAttributeValue(portEl, "NID");
				if (MRXMLUtils.getAttributeValue(portEl, "portType", "").equals(SessionServiceConst.STREAM_SRC)) {
					
				    Element thumbnailFeatureEl = MRXMLUtils.getElementXpath(portEl, ".//Feature[@name='"+SessionServiceConst.THUMBNAIL+"']");
					String state = MRXMLUtils.getAttributeValue(portEl, SessionServiceConst.STATE, "");
					
					// Return thumbnail URL only if Feature=Thumbnail and port is Ready.
					if(thumbnailFeatureEl != null && state.equalsIgnoreCase(SessionServiceConst.READY)) {
						String thumbnailURL = "";
						thumbnailURL = HttpAuthentication.generateThumbnailURL(portNID, "live", timeStamp, msIPAddress,authScheme, log,serviceAgentFactory, request, db,xmpp,resourceFactory,dstTemplateTitle);
						if (!thumbnailURL.isEmpty()) {
							MRXMLUtils.setAttributeValue(portEl, SessionServiceResourceConst.THUMBNAILURL_ATTRIBUTE, thumbnailURL);
						}
					}
					
					if (selectFormatType.equals(SessionServiceResourceConst.SELECTFORMAT_BASICURL) || (selectFormatType.equals(SessionServiceResourceConst.SELECTFORMAT_EXTENDEDURL))
							|| selectFormatType.equals(SessionServiceConst.SELECTFORMAT_BASICCONNECTIONINFO) || selectFormatType.equals(SessionServiceConst.SELECTFORMAT_EXTENDEDCONNECTIONINFO)) {

						String mediaURL = "";
						String protocol = MRXMLUtils.getValueXpath(request.getMessage().getDocument(), "//Header/Protocol", "http");
						//do not check for destination, send mediaURL anyway
						mediaURL = protocol+"://"+ msIPAddress + "/clients/player?requestName=View&SrcPort.portNID="	+ portNID;
					
						mediaURL = HttpAuthentication.appendSystemAuthenticationQueryParameters(mediaURL, apiKey, timeStamp, authScheme, signature, request.getUserJID(),dstTemplateTitle);
							
						if (!dstNID.isEmpty()) {
							mediaURL = mediaURL.concat("&DstPort.portNID="+ dstNID);
						}

						String fallback = "false";
						if (MRXMLUtils.getAttributeValue(portEl, SessionServiceConst.STREAM_TYPE, "").equals(MediaRoomServiceConst.RTP)) {
							String streamURL = MRXMLUtils.getAttributeValueXpath(portEl, "InstanceList//Instance/@streamURL", "");
							if(!streamURL.isEmpty()) {								
								//return fall back option only in case of global hls is enabled.
								if(appServerAgent.getServiceConfigManager().isHLSEnabled()) {
									fallback = String.valueOf(SDPUtils.isSourceHLSCapable(streamURL, log));
								}
							}
						}
						mediaURL = mediaURL.concat("&fallback="+ fallback);						

						MRXMLUtils.setAttributeValue(portEl, SessionServiceResourceConst.MEDIAURL_ATTRIBUTE, mediaURL);
					}
				}
				
				//Remove the instancelist and FeatureList if selectFormat=BASIC/BASICURL
				if(selectFormatType.equals(SessionServiceResourceConst.SELECTFORMAT_BASICURL) || selectFormatType.equals("BASIC")) {
					MRXMLUtils.removeChild(portEl, "InstanceList");
					MRXMLUtils.removeChild(portEl, "FeatureList");
				}
					
				MRXMLUtils.removeChildren(portEl, SessionServiceResourceConst.API_KEY_TABLE);
		    }
		}
		
		XMPPTransaction.endBlock("getObjectListRequest");
	
		XMPPTransaction.startBlock("subscribe");
	
		//subscribe to DBUpdateEvent
		this.subscribeUser(request, xmpp, log, db, serviceAgentFactory, resourceFactory,  MRXMLUtils.getElementXpath(request.getMessage().getDocument(), "//GetPort"), 
				SessionServiceConst.PORT, modelObjectType, whereXPath, responseDoc);
		
		XMPPTransaction.endBlock("subscribe");
	

		XMPPTransaction.startBlock("fillConnectionStateInfoInResponseDoc");
	
		if(selectFormatType.equals(SessionServiceConst.SELECTFORMAT_BASICCONNECTIONINFO) || selectFormatType.equals(SessionServiceConst.SELECTFORMAT_EXTENDEDCONNECTIONINFO) ){
			fillConnectionStateInfoInResponseDoc(responseDoc,request,xmpp,db,log,serviceAgentFactory,resourceFactory,selectFormatType, SessionServiceConst.PORT, msIPAddress,authScheme,dstTemplateTitle);
		}
		
		XMPPTransaction.endBlock("fillConnectionStateInfoInResponseDoc");
		
		return responseDoc;
	}
    
	
}
