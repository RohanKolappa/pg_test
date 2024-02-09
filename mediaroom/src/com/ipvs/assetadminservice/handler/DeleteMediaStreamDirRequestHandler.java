package com.ipvs.assetadminservice.handler;

import org.dom4j.Document;

import com.ipvs.common.MRAuthorization;
import com.ipvs.common.MRConst;
import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.db.MRXMLDBUtils;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRRequestHandlerI;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.mediaroomservice.datamodel.MediaRoomAuthorization;
import com.ipvs.utils.Utils;

public class DeleteMediaStreamDirRequestHandler implements MRRequestHandlerI {
    public MRAuthorization authorizeRequest(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception {
        log.addTraceBegin(request);
        int authType = 1;
        int role = 2;
        String token = "testCommonToken";
        String authority = "testAuthority";
        String roomPolicyNID = MRConst.NULL;
        String streamPolicyNID = MRConst.NULL;

        MediaRoomAuthorization auth = new MediaRoomAuthorization(MRConst.NULL, MRConst.NULL, authType, role, roomPolicyNID, streamPolicyNID, token, authority);
        log.addTraceEnd(request);
        return auth;

    }
    private static long TIMEOUT_SEC =900;
    public Document getResponse(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception {
        Document reqDoc = request.getMessage().getDocument();
        String MediaStreamDirResourceNID = MRXMLUtils.getValueXpath(reqDoc, "//mediaStreamDirResourceNID", "").trim();
        if (MediaStreamDirResourceNID.equals("")) {
            //since the file is not there, can not restore from
            throw new MRException(AMExceptionCodes.DELETE_DB_ERROR, MRXMLUtils.generateErrorXML("scriptError", " MediaStreamDirResourceNID  '" + MediaStreamDirResourceNID + "' parameter is needed."));
        }
        String zipFolderPath = MRXMLUtils.getValueXpath(reqDoc, "//zipFolderPath", "").trim();
        String scriptFilePath = MRXMLUtils.getValueXpath(reqDoc, "//scriptFilePath", "").trim();
        long timeout = Utils.getLongValue(MRXMLUtils.getValueXpath(reqDoc, "//timeout_sec", "").trim(), TIMEOUT_SEC) * 1000;
         
        StringBuffer buf =new StringBuffer();
        buf.append("<deleteMedia>");
        buf.append("<params><mediaStreamDirResourceNID>"+MediaStreamDirResourceNID+"</mediaStreamDirResourceNID></params>");
        if(!zipFolderPath.equals("")){
            buf.append("<zipFolderPath>"+zipFolderPath+"</zipFolderPath>");
        }
        if(!scriptFilePath.equals("")){
            buf.append("<scriptFilePath>"+scriptFilePath+"</scriptFilePath>");
        }
        buf.append("</deleteMedia>");
        MRLog.println("Start DBDeleteMediaDir " + buf.toString()  );
        Document resp = MRXMLDBUtils.deleteDB(timeout, request, db, log, "DeleteMedia", buf.toString() );
       
        //create and send response
        StringBuffer bufRes = new StringBuffer();
        String rdname = request.getResponseDataName();
        bufRes.append("<" + rdname + ">");
        if(resp !=null && resp.getRootElement() != null ){
            bufRes.append(resp.getRootElement().asXML());
        }
        bufRes.append("</" + rdname + ">");
        System.out.println("DeleteMediaStreamDirRequestHandler getResponse " + bufRes.toString());

        return MRXMLUtils.stringToDocument(bufRes.toString());
    }
    

}

 
