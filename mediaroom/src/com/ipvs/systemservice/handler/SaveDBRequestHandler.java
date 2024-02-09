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
import com.ipvs.common.service.AuditEvent;
import com.ipvs.common.service.AuditEventLogger;
import com.ipvs.common.service.EventLoggerI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRRequestHandlerI;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.config.IPVSConst;
import com.ipvs.mediaroomservice.datamodel.MediaRoomAuthorization;
import com.ipvs.policyadminservice.utils.PolicyAdminConstants;
import com.ipvs.policyadminservice.utils.PolicyAdminServiceUtils;
import com.ipvs.systemservice.impl.SystemServiceConst;

public class SaveDBRequestHandler implements MRRequestHandlerI,EventLoggerI {

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
        StringBuffer buf = new StringBuffer();
        
        
        PolicyAdminServiceUtils.checkPolicyPermissions(PolicyAdminConstants.BACKUP_ROLE_ACTION, PolicyAdminConstants.SYSTEM_OBJECT, request, xmpp, log, db, resourceFactory, serviceAgentFactory);
        
        String[] cmdArgs = { IPVSConst.getInstance().getIpvsxmldbactl(), "create_snapshot", "manual" };
        int exitVal = SystemServiceUtils.executeProcess(cmdArgs, buf);
        // check response
        if (exitVal != 0) {
            //script should not return non zero value.
            throw new MRException(SystemServiceConst.SAVE_DB_ERROR, MRXMLUtils.generateErrorXML("scriptError", "exitVal=" + exitVal + " '" + buf.toString() + "'."));
        }
        if (buf.toString().toLowerCase().indexOf("error") != -1) {
            //there was some error in script.
            throw new MRException(SystemServiceConst.SAVE_DB_ERROR, MRXMLUtils.generateErrorXML("scriptError", "'" + buf.toString() + "'."));
        }

        StringBuffer bufRes = new StringBuffer();
        String rdname = request.getResponseDataName();
        bufRes.append("<" + rdname + ">");
        Element el = null;
        try {
            el = SystemServiceUtils.parseServiceStatusParameters(SystemServiceUtils.getDatabaseManagementStatus());
        } catch (Exception ex) {
            ex.printStackTrace();
            //the script returned non parsable xml.
            throw new MRException(SystemServiceConst.SAVE_DB_ERROR, MRXMLUtils.generateErrorXML("scriptError", " Error in parse '" + buf.toString() + "'." + ex.getMessage()));
        }
        if (el != null) {
            bufRes.append(MRXMLUtils.elementToString(el));
        } else {
            //the script did not return parameters.
            throw new MRException(SystemServiceConst.SAVE_DB_ERROR, MRXMLUtils.generateErrorXML("scriptError", " Internal Error. Parameters tag not found '" + buf.toString() + "'."));
        }
        bufRes.append("</" + rdname + ">");
        //System.out.println("SaveDBRequestHandler getResponse " + bufRes.toString());

        return MRXMLUtils.stringToDocument(bufRes.toString());
    }
    
    public AuditEvent getAuditEvent(MRRequest request, XMPPI xmpp, MRLog log,MRDB db, String status, String level, String errorDescription) throws Exception {
  	  return new AuditEvent.Builder(AuditEventLogger.SYSTEM_EVENT, AuditEventLogger.NAME_BACKUP_DATABASE, request.getUserJID(), level).target("Server")
  	  .descriptionText(""+ errorDescription).status(status).build();
    }

}
