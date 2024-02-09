package com.ipvs.assetadminservice.handler;
 

import java.util.Iterator;
import java.util.List;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.client.ServiceClient;
import com.ipvs.common.MRAuthorization;
import com.ipvs.common.MRConst;
import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.db.DBExceptionCodes;
import com.ipvs.common.db.MRDBHelper;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRRequestHandlerI;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.mediaroomservice.datamodel.MediaRoomAuthorization;
import com.ipvs.mediaroomservice.datamodel.MediaRoomObject;
import com.ipvs.mediaroomservice.impl.MediaRoomPEM;


public class DBTestRequestHandler implements MRRequestHandlerI {
    private static String serviceName = ServiceClient.ASSETADMINSERVICE;

    public MRAuthorization authorizeRequest(MRRequest request, XMPPI xmpp, MRLog log, MRDB schema,
        MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory)
        throws Exception {
        log.addTraceBegin(request);

        int authType = MediaRoomObject.ROOM_TYPEID;
        int role = MediaRoomPEM.VISITOR_ROLE;
        String token = "testCommonToken";
        String authority = "testAuthority";
        String roomPolicyNID = MRConst.NULL;
        String streamPolicyNID = MRConst.NULL;
        MediaRoomAuthorization auth = new MediaRoomAuthorization(MRConst.NULL, MRConst.NULL, authType, role, roomPolicyNID,
                streamPolicyNID, token, authority);
        log.addTraceEnd(request);

        return auth;
    }

    public Document getResponse(MRRequest request, XMPPI xmpp, MRLog log, MRDB schema, MRResourceFactoryI resourceFactory,
        ServiceAgentFactoryI serviceAgentFactory) throws MRException, Exception {
        log.addTraceBegin(request);

        Document reqDoc = request.getMessage().getDocument();

        String dbObjectName = MRXMLUtils.getValueXpath(reqDoc, "//Data/DBTestRequestData/ObjectType", "");

        @SuppressWarnings("unused")
        boolean bFailed = false;
        //@SuppressWarnings("unused")
        //StringBuffer buf = new StringBuffer();

        //add response resDoc
        Document resDoc = MRXMLUtils.getDocument();

        Element rootRes = MRXMLUtils.addElement(resDoc, "Handler");
        MRXMLUtils.addElement(rootRes, "ObjectType", dbObjectName);

        String pemXPath = null;

        MRDBHelper pmUtil = new MRDBHelper();
        pmUtil.doInit(serviceName, dbObjectName, resDoc, request, xmpp, log, schema, resourceFactory, serviceAgentFactory);

        //call actions in loop 
        Element parentEL = MRXMLUtils.getElementXpath(reqDoc, "//Data/DBTestRequestData");
        List<?> listEL = MRXMLUtils.getList(parentEL);
        //MRConfig pmConfig = MRConfig.getInstance(serviceName, dbObjectName, log);

        for (Iterator<?> i = listEL.iterator(); i.hasNext();) {
            Element actionEL = (Element) i.next();
            String action = actionEL.getName();

            if (action.equalsIgnoreCase("ObjectType")) {
                //skip
            } else if (action.equalsIgnoreCase("GetRequest")) {
                try {
                	pmUtil.doGet(actionEL, "GetResponse", dbObjectName, pemXPath);
                } catch (MRException ex) {
                    bFailed = true;
                    ex.printStackTrace();
                    throw new MRException(ex.getCode(), ex.getData());
                } catch (Exception ex) {
                    bFailed = true;
                    ex.printStackTrace();
                    throw new MRException(DBExceptionCodes.DB_GET_FAILED, ex.getMessage());
                }
            } else if (action.equalsIgnoreCase("ReplaceRequest")) {
                try {
                    pmUtil.doUpdate(actionEL, "ReplaceResponse", dbObjectName, null);
                } catch (MRException ex) {
                    bFailed = true;
                    ex.printStackTrace();
                    throw new MRException(ex.getCode(), ex.getData());
                } catch (Exception ex) {
                    bFailed = true;
                    ex.printStackTrace();
                    throw new MRException(DBExceptionCodes.DB_UPDATE_FAILED, ex.getMessage());
                }
            } else if (action.equalsIgnoreCase("AddRequest")) {
                try {
                    pmUtil.doAdd(actionEL, "AddResponse", dbObjectName, null);
                } catch (MRException ex) {
                    bFailed = true;
                    ex.printStackTrace();
                    throw new MRException(ex.getCode(), ex.getData());
                } catch (Exception ex) {
                    bFailed = true;
                    ex.printStackTrace();
                    throw new MRException(DBExceptionCodes.DB_ADD_FAILED, ex.getMessage());
                }
            } else if (action.equalsIgnoreCase("DeleteRequest")) {
                try {
                      pmUtil.doDelete(actionEL, "DeleteResponse", dbObjectName, null);
                } catch (MRException ex) {
                    bFailed = true;
                    ex.printStackTrace();
                    throw new MRException(ex.getCode(), ex.getData());
                } catch (Exception ex) {
                    bFailed = true;
                    ex.printStackTrace();
                    throw new MRException(DBExceptionCodes.DB_DELETE_FAILED, ex.getMessage());
                }
            }
        }

        pmUtil.doUnInit();
        pmUtil = null;

        log.addTraceEnd(request);

        resDoc.getRootElement().setName(request.getResponseDataName());

        return resDoc;
    }
 
}
