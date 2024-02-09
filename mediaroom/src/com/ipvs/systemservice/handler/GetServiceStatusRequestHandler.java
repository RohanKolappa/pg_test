package com.ipvs.systemservice.handler;

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
import com.ipvs.systemservice.impl.SystemServiceConst;

public class GetServiceStatusRequestHandler implements MRRequestHandlerI {

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

    public Document getResponse(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception {
        //System.out.println("GetServiceStatus getResponse ");
        StringBuffer buf = new StringBuffer();

        int exitVal = 0;
        String[] cmdArgs = {"/V2O/bin/vutils", "get_db_management_status" };
        exitVal = SystemServiceUtils.executeProcess(cmdArgs, buf);
        if (exitVal != 0) {
            //script should not return non zero value.
            throw new MRException(SystemServiceConst.GET_SERVICE_STATUS_ERROR, MRXMLUtils.generateErrorXML("scriptError", " '" + buf.toString() + "'."));
        }
        if (buf.toString().toLowerCase().indexOf("error") != -1) {
            //there was some error in script.
            throw new MRException(SystemServiceConst.GET_SERVICE_STATUS_ERROR, MRXMLUtils.generateErrorXML("scriptError", " '" + buf.toString() + "'."));
        }

        StringBuffer bufRes = new StringBuffer();
        String rdname = request.getResponseDataName();
        bufRes.append("<" + rdname + ">");
        Element el = null;
        try {
            el = SystemServiceUtils.parseServiceStatusParameters(buf.toString());
        } catch (Exception ex) {
            //the script returned non parsable xml.
            throw new MRException(SystemServiceConst.GET_SERVICE_STATUS_ERROR, MRXMLUtils.generateErrorXML("scriptError", " Error in parse '" + buf.toString() + "'." + ex.getMessage()));
        }
        if (el != null) {
            bufRes.append(MRXMLUtils.elementToString(el));
        } else {
            //the script did not return parameters.
            throw new MRException(SystemServiceConst.GET_SERVICE_STATUS_ERROR, MRXMLUtils.generateErrorXML("scriptError", " Internal Error. Parameters tag not found '" + buf.toString() + "'."));
        }
        bufRes.append("</" + rdname + ">");
        //System.out.println("GetServiceStatusRequestHandler getResponse "+bufRes.toString());

        return MRXMLUtils.stringToDocument(bufRes.toString());
    }
}
