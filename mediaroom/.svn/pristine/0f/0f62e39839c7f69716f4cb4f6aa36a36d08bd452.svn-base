package com.ipvs.mediaroomservice.handler;

import java.util.Iterator;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.assetadminservice.handler.AMConst;
import com.ipvs.client.ServiceClient;
import com.ipvs.common.MRAuthorization;
import com.ipvs.common.MRConst;
import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRRequestHandlerI;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.mediaroomservice.datamodel.MediaRoomAuthorization;
import com.ipvs.mediaroomservice.datamodel.MediaRoomObject;
import com.ipvs.mediaroomservice.impl.MediaRoomPEM;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceConst;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceUtils;
import com.ipvs.policyadminservice.impl.GetDestinationLib;
import com.ipvs.policyadminservice.utils.PolicyAdminConstants;
import com.ipvs.policyadminservice.utils.PolicyManagementExceptionCodes;


public class GetDestinationListRequestHandler implements MRRequestHandlerI {
    public MRAuthorization authorizeRequest(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory)
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

    
    public Document getResponse(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory)
        throws MRException, Exception {
        Document requestDoc = request.getMessage().getDocument();
        String title = "";
        Element titleElement = MRXMLUtils.getElementXpath(requestDoc, "//title");

        if (titleElement != null) {
            title = MRXMLUtils.getValue(titleElement);
        }
        String pemXPath = "";
     
        try {
        	
        	String resourceXPath = GetDestinationLib.getDestinationXpath("", PolicyAdminConstants.VIEW_ROLE_ACTION, request, xmpp, log, db, resourceFactory, serviceAgentFactory);

            if ((resourceXPath != null) && (resourceXPath.length() == 0) && !"".equals(title.trim())) {
                pemXPath = "//MediaStreamIOPortResource/Info[title=" + title.trim() + "]";
            } else if ((resourceXPath != null) && (resourceXPath.length() == 0) && "".equals(title.trim())) {
                pemXPath = "//MediaStreamIOPortResource";
            } else if (!"".equals(title.trim())) {  
                pemXPath = "//MediaStreamIOPortResource/Info[title=" + title.trim() + "' and " + resourceXPath + "]";
            } else {
                pemXPath = "//MediaStreamIOPortResource/Info[" + resourceXPath + "]";
            }

            log.addInfo("GetDestinationList PEMWhereXPath="+pemXPath);

            // Filter out the StreamDst ports
            String ioPortWhereXPath = pemXPath + " and //MediaStreamIOPortResource/Info[Type != '" + MediaRoomServiceConst.STREAMSRC + "']";
            
            Document responseDoc = MediaRoomServiceUtils.handleGetRequest(ioPortWhereXPath, AMConst.MEDIA_STREAM_IO_PORT_RESOURCE_OBJECTTYPE, ServiceClient.ASSETADMINSERVICE, request, xmpp, log, db,
                    serviceAgentFactory, resourceFactory);     
            
            Element responseXML = MRXMLUtils.getElementXpath(responseDoc, "//ResponseXML");
            int objectCount = Integer.parseInt(
            		MRXMLUtils.getValueXpath(responseDoc, "//ObjectCount", "0"));
            
            // Adding DstPorts: This code to be removed once DstPort folded into IOPort
            
            pemXPath = pemXPath.replace("IOPort", "DstPort");
            Document responseSrcListDoc = MediaRoomServiceUtils.handleGetRequest(pemXPath, AMConst.MEDIA_STREAM_DST_PORT_RESOURCE_OBJECTTYPE, ServiceClient.ASSETADMINSERVICE, request, xmpp, log, db,
                    serviceAgentFactory, resourceFactory);

               Element responseSrcListXML = MRXMLUtils.getElementXpath(responseSrcListDoc, "//ResponseXML");
            int srcPortCount = 0;
            Iterator<?> j = responseSrcListXML.elements().iterator();
            while (j.hasNext()) {
                Element portElement = (Element) j.next();
            	responseXML.add((Element)portElement.clone());
            	srcPortCount++;
            }
            objectCount = objectCount + srcPortCount;
            MRXMLUtils.getElementXpath(responseDoc, "//ObjectCount").setText(
            		String.valueOf(objectCount));           
            
            return responseDoc;
        } catch (MRException e) {
            e.printStackTrace();
            pemXPath = "";
            throw new MRException(e.getCode(), e.getData());
        } catch (Exception exp) {
            exp.printStackTrace();
            throw new MRException(PolicyManagementExceptionCodes.PEM_GET_FAILED, exp.toString());
        }
    }
}
