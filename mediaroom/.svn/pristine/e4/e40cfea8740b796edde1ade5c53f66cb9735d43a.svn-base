package com.ipvs.mediaroomservice.handler;

import org.dom4j.Document;
import org.dom4j.Element;

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
import com.ipvs.mediaroomservice.impl.MediaRoomServiceExceptionCodes;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceUtils;
import com.ipvs.policyadminservice.utils.PolicyAdminConstants;
import com.ipvs.policyadminservice.utils.PolicyManagementExceptionCodes;


public class GetMediaRoomListRequestHandler implements MRRequestHandlerI {
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
        throws Exception {
        Element data = MediaRoomServiceUtils.cloneDataElement(request);

        Element whereAssistXML = MRXMLUtils.stringToElement("<MediaStreamSrcPortResource><Info><title>string</title><Groups><Tag  /></Groups></Info></MediaStreamSrcPortResource>");

        try {
            String whereXPath = null;
            String xPath = null;
            String resourceXPath = null;
            String streamXPath = null;
            String sourceXPath = null;
            String mediaSourceXPath = null;      
            streamXPath = MRXMLUtils.getValueXpath(data, "streamXPath", "");
            sourceXPath = MRXMLUtils.getValueXpath(data, "sourceXPath", "");
            mediaSourceXPath = MRXMLUtils.getValueXpath(data, "mediaSourceXPath", "");
            
            //TBD: Once we fixed media room object problem
            request.setSuperUser(true);
            if (!request.isSuperUser()) {
                Element operatorGroups = MediaRoomServiceUtils.getOperator(request, xmpp, log, db, resourceFactory, serviceAgentFactory);
                Document doc = MRXMLUtils.stringToDocument("<data><GetMediaRoomXPathRequestData>" + "<OperatorList/><Action>View</Action><ObjectType/>" + "</GetMediaRoomXPathRequestData></data>");
                MRXMLUtils.addElementXpath(doc, "//OperatorList",operatorGroups);

                Document policyResponse = MediaRoomServiceUtils.getPolicyAdminResponse(request, xmpp, log, db, resourceFactory, serviceAgentFactory, "policyadminservice", "GetMediaRoomXPathRequest",
                        doc);
                resourceXPath = MRXMLUtils.getValueXpath(policyResponse, "//XPath", "").trim();

                if (resourceXPath == null) {
                    throw new MRException(MediaRoomServiceExceptionCodes.XPATH_ELEMENT_MISSING, "xPath Element Missing in Policy Response");
                }
            } else {
                resourceXPath = PolicyAdminConstants.ALL_FILTER;
            }
            //System.out.println("resourceXPath----->"+resourceXPath);
            if (!"".equals(resourceXPath) && resourceXPath.equals(PolicyAdminConstants.ALL_FILTER)) {
                xPath = getXPath(streamXPath, sourceXPath, mediaSourceXPath);               
                if (xPath.length() != 0) {
                    whereXPath = "//MediaRoom[" + xPath + "]";
                } else {
                    whereXPath = "//MediaRoom";
                }
            } else if ((resourceXPath != null) && (resourceXPath.trim().length() != 0)) {
                xPath = getXPath(streamXPath, sourceXPath, mediaSourceXPath);

                if (xPath.length() != 0) {
                    //whereXPath = "//MediaRoom[ " + xPath.toString() + "  and " + resourceXPath + " ]";
                	whereXPath = "//MediaRoom[ " + xPath + "  and " + resourceXPath + " ]";
                } else {
                    //whereXPath = "//MediaRoom[" + resourceXPath.toString() + "]";
                	whereXPath = "//MediaRoom[" + resourceXPath + "]";
                }
            } else {
                throw new MRException(PolicyManagementExceptionCodes.ACCES_DENY_TO_VIEW_MEDIAROOMS, "Access Deny to View Media Rooms");
            }

            /*
            Document response = MediaRoomServiceUtils.getAssetAdminResponse(request, xmpp, log, db, resourceFactory, serviceAgentFactory, ServiceClient.ASSETADMINSERVICE,
                    "MediaRoom_BASE_ALL_GET_WHERE.xml", whereAssistXML, whereXPath,"-1");
            */
          
            Document result = MediaRoomServiceUtils.getMediaRoomList(request, xmpp, log, db, resourceFactory, serviceAgentFactory, whereAssistXML, whereXPath, true);
            Document response = request.createResponseData((Element)result.getRootElement().detach());
            return response;
        } catch (MRException e) {
            e.printStackTrace();
            throw new MRException(e.getCode(), e.getData());
        } catch (Exception exp) {
            exp.printStackTrace();
            throw new MRException(PolicyManagementExceptionCodes.PEM_GET_FAILED, exp.toString());
        }
    }

    private String getXPath(String streamXPath, String sourceXPath, String mediaSourceXPath) {
        StringBuffer xPath = new StringBuffer();
        xPath.append("");

        boolean isStarted = false;

        if ((sourceXPath != null) && !"".equals(sourceXPath)) {
            isStarted = true;
            xPath.append(sourceXPath);
        }

        if ((streamXPath != null) && !"".equals(streamXPath)) {
            if (isStarted) {
                xPath.append(" and " + streamXPath);
            } else {
                xPath.append(streamXPath);
                isStarted = true;
            }
        }

        if ((mediaSourceXPath != null) && !"".equals(mediaSourceXPath)) {
            if (isStarted) {
                xPath.append(" and " + mediaSourceXPath);
            } else {
                xPath.append(mediaSourceXPath);
                isStarted = true;
            }
        }

        return xPath.toString();
    }
}
