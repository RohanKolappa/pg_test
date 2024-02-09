package com.ipvs.assetadminservice.handler;

import java.net.URLDecoder;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.assetadminservice.impl.AssetAdminServiceUtils;
import com.ipvs.assetadminservice.impl.BaseManagementRequestHandler;
import com.ipvs.client.ServiceClient;
import com.ipvs.common.MRConst;
import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.db.MRDBHelper;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.AgentResponseHandlerI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRRequestHandlerI;
import com.ipvs.common.service.MRResponseHandlerI;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceExceptionCodes;
import com.ipvs.utils.Utils;


public class AssetManagementRequestHandler  extends BaseManagementRequestHandler implements MRRequestHandlerI, AgentResponseHandlerI {
    private static String serviceName = ServiceClient.ASSETADMINSERVICE;

    public Document getResponse(MRRequest request, XMPPI xmpp, MRLog log, MRDB schema, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory)
        throws MRException, Exception {
        log.addTraceBegin(request);

       return  GetResponseBasedOnAction(request, xmpp, log, schema, resourceFactory, serviceAgentFactory, serviceName);
    }

    public void handleAgentResponse(MRResponseHandlerI responseHandler, MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory)
        throws Exception {
        Document responseDoc = request.getResult();

        String clientData = MRXMLUtils.getValueXpath(responseDoc, "//"+MRConst.CLIENT_DATA, "");

        //get the MRRequest.xml document        
        Document requestXML = MRXMLUtils.stringToDocument((URLDecoder.decode(clientData, "UTF-8")));

        MRRequest getOffsetAtTimeStampRequest = MRRequest.unMarshalRequestObject(requestXML, null, "", log);

        String getOffsetAtTimeCodeRequestDocClientData = MRXMLUtils.getValueXpath(requestXML, "//"+MRConst.CLIENT_DATA, "");

        Document assetRequestDoc = MRXMLUtils.stringToDocument(URLDecoder.decode(getOffsetAtTimeCodeRequestDocClientData, "UTF-8"));
        MRRequest assetRequest = MRRequest.createRequest(getOffsetAtTimeStampRequest, assetRequestDoc, MRXMLUtils.getAttributeValueXpath(assetRequestDoc, "//Header",MRConst.REQUEST_NAME, ""), serviceName, MRXMLUtils.getValueXpath(assetRequestDoc, "//"+MRConst.CLIENT_DATA, ""));

        //Get the client Data of asset data to check user request is coming from media room or not.
        String assetClientData = MRXMLUtils.getValueXpath(assetRequestDoc, "//"+MRConst.CLIENT_DATA, "");
        Document userRequestDoc = null;

        if (AssetAdminServiceUtils.isMediaRoomRequest(assetClientData)) {
            userRequestDoc = MRXMLUtils.stringToDocument(URLDecoder.decode(assetClientData, "UTF-8"));
        } else {
            userRequestDoc = assetRequestDoc;
        }

        String requestName = MRXMLUtils.getAttributeValueXpath(userRequestDoc, MRConst.HEADER_XPATH, MRConst.REQUEST_NAME, "");
        String serviceName = MRXMLUtils.getAttributeValueXpath(userRequestDoc, MRConst.HEADER_XPATH, MRConst.SERVICE_NAME, "");
        String userClientData = MRXMLUtils.getValueXpath(userRequestDoc, "//"+MRConst.CLIENT_DATA, "");

        MRRequest userRequest = MRRequest.createRequest(getOffsetAtTimeStampRequest, userRequestDoc, requestName, serviceName, userClientData);

        //get the user request from get current time stamp request client data      
        int state = Integer.parseInt(MRXMLUtils.getAttributeValueXpath(responseDoc, MRConst.HEADER_XPATH,MRConst.STATE, "0"));
        String logData = MRXMLUtils.getValueXpath(responseDoc, "//log", "");
        userRequest.setState(state);

        if (state == MRRequest.STATE_EXCEPTION) {
            String codeStr = MRXMLUtils.getValueXpath(responseDoc, "//Code", String.valueOf(MediaRoomServiceExceptionCodes.AGENTREQUESTFAILED));
            int code = Utils.getIntValue(codeStr, MediaRoomServiceExceptionCodes.AGENTREQUESTFAILED);
            String description = MRXMLUtils.getValueXpath(responseDoc, "//Description", "AgentRequestFailed");
            MRException mrException = new MRException(code, description);
            userRequest.setException(mrException);
        } else {
            Document resDoc = MRXMLUtils.getDocument();
            Element rootRes = MRXMLUtils.addElement(resDoc, "Handler");
            String dbObjectName = MRXMLUtils.getValueXpath(assetRequestDoc, "//ObjectType", "");
            MRXMLUtils.addElement(rootRes, "ObjectType", dbObjectName);

            // AMUtil
            MRDBHelper amUtil = new MRDBHelper();
            amUtil.doInit(ServiceClient.ASSETADMINSERVICE, dbObjectName, resDoc, assetRequest, xmpp, log, db, resourceFactory, serviceAgentFactory);

            Element actionEL = MRXMLUtils.getElementXpath(assetRequestDoc, "//AddRequest");
            Element responseEL = null;

            MRXMLUtils.setValueXpath(assetRequestDoc, "//Offset", MRXMLUtils.getValueXpath(responseDoc, "//StreamOffset", ""));

            if (actionEL == null) {
                actionEL = MRXMLUtils.getElementXpath(assetRequestDoc, "//ReplaceRequest");
                amUtil.doUpdate(actionEL, "ReplaceResponse", dbObjectName, null);
                responseEL = MRXMLUtils.getElementXpath(resDoc, "//ReplaceResponse");
            } else {
                amUtil.doAdd(actionEL, "AddResponse", dbObjectName, null);
                responseEL = MRXMLUtils.getElementXpath(resDoc, "//AddResponse");
            }

            Document userResponseDoc = userRequest.createResponseData((Element) responseEL.detach());
            userRequest.setResult(userResponseDoc);
        }

        responseHandler.handleResponse(userRequest, logData);
    }
}
