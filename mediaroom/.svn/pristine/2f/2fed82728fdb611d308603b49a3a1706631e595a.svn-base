package com.ipvs.systemservice.handler;

import java.io.File;
import java.util.Date;

import org.dom4j.Document;

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

public class RestoreDBRequestHandler implements MRRequestHandlerI,EventLoggerI {
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
    	
    	PolicyAdminServiceUtils.checkPolicyPermissions(PolicyAdminConstants.RESTORE_ROLE_ACTION, PolicyAdminConstants.SYSTEM_OBJECT, request, xmpp, log, db, resourceFactory, serviceAgentFactory);
    	
        Document reqDoc = request.getMessage().getDocument();
        String restoreFrom = MRXMLUtils.getValueXpath(reqDoc, "//RestoreDBRequestData/RestoreFrom", "").trim();
        if (restoreFrom.equals("")) {
            //since the file is not there, can not restore from
            throw new MRException(SystemServiceConst.RESTORE_DB_ERROR, MRXMLUtils.generateErrorXML("scriptError", " restoreFrom  '" + restoreFrom + "' parameter is needed."));
        }
        //System.out.println("RestoreDB restoreFrom " + restoreFrom);
        StringBuffer buf = new StringBuffer();
        int exitVal = 0;

        //schedule the db creation on next reboot
        //Factory/LastBackup/LastImported
        if (restoreFrom.equals("Factory")) {
            String[] cmdArgs = { IPVSConst.getInstance().getIpvsxmldbactl(), "reset" , restoreFrom.toLowerCase()};
            exitVal = SystemServiceUtils.executeProcess(cmdArgs, buf);
        } else if (restoreFrom.equals("LastBackup")) {
            //TODO: is back up exists ? else throw error. call getDatabaseManagementStatus and parse.
            String[] cmdArgs = { IPVSConst.getInstance().getIpvsxmldbactl(), "schedule_restoreSnapshot" ,  restoreFrom.toLowerCase()};
            exitVal = SystemServiceUtils.executeProcess(cmdArgs, buf);
        } else if (restoreFrom.equals("LastImported")) {
            //todo: integration test 
            File file = new File("/data/saved_databases/importdb.tgz");
            if (!file.exists()) {
                //since the file is not there, can not restore from
                throw new MRException(SystemServiceConst.RESTORE_DB_ERROR, MRXMLUtils.generateErrorXML("scriptError", "'/data/saved_databases/importdb.tgz' does not exist."));
            }
            String[] cmdArgs = { IPVSConst.getInstance().getIpvsxmldbactl(), "schedule_importdb", "importdb" , restoreFrom.toLowerCase()};
            exitVal = SystemServiceUtils.executeProcess(cmdArgs, buf);
        }

        //check for errors in schedule
        if (exitVal != 0) {
            //script should not return non zero value.
            throw new MRException(SystemServiceConst.RESTORE_DB_ERROR, MRXMLUtils.generateErrorXML("scriptError", " '" + buf.toString() + "'."));
        }
        if (buf.toString().toLowerCase().indexOf("error") != -1) {
            //there was some error in script.
            throw new MRException(SystemServiceConst.RESTORE_DB_ERROR, MRXMLUtils.generateErrorXML("scriptError", " '" + buf.toString() + "'."));
        }

        //reboot in thread
        SednaImportDB importDB = new SednaImportDB(restoreFrom);
        importDB.start();

        //create and send response
        StringBuffer bufRes = new StringBuffer();
        String rdname = request.getResponseDataName();
        bufRes.append("<" + rdname + ">");
        bufRes.append("<Parameters>");
        bufRes.append(" <Parameter name=\"Restore From\" type=\"String\">" + SystemServiceUtils.formatScriptDate(new Date()) + "</Parameter>");
        bufRes.append("</Parameters>");
        bufRes.append("</" + rdname + ">");
        //System.out.println("RestoreDBRequestHandler getResponse " + bufRes.toString());

        return MRXMLUtils.stringToDocument(bufRes.toString());
    }

    private static class SednaImportDB extends Thread {
        String archiveName = "";

        SednaImportDB(String archiveName) {
            this.archiveName = archiveName;
        }

        public void run() {
            try {
                StringBuffer buf = new StringBuffer();
                //String[] importScheduleArgs = {  "/etc/scripts/restore_database.sh", archiveName };
                String[] importScheduleArgs = { "/V2O/bin/vutils", "reboot_system" };
                int exitVal = SystemServiceUtils.executeProcess(importScheduleArgs, buf);
                if (exitVal != 0) {
                    //script should not return non zero value.
                    throw new MRException(SystemServiceConst.RESTORE_DB_ERROR, MRXMLUtils.generateErrorXML("scriptError", " '" + buf.toString() + "'."));
                }
                if (buf.toString().toLowerCase().indexOf("error") != -1) {
                    //there was some error in script.
                    throw new MRException(SystemServiceConst.RESTORE_DB_ERROR, MRXMLUtils.generateErrorXML("scriptError", " '" + buf.toString() + "'."));
                }
            } catch (Exception ex) {
                ex.printStackTrace();
            }
        }
    }

public AuditEvent getAuditEvent(MRRequest request, XMPPI xmpp, MRLog log,MRDB db, String status, String level, String errorDescription) throws Exception {
		String restoreFrom = MRXMLUtils.getValueXpath(request.getMessage().getDocument(), "//RestoreDBRequestData/RestoreFrom", "").trim();
		return new AuditEvent.Builder(AuditEventLogger.SYSTEM_EVENT, AuditEventLogger.NAME_RESTORE_DATABASE, request.getUserJID(), level)
		.target(restoreFrom).descriptionText(""+ errorDescription).status(status).build();
	}
}

/*

<CLIStatus>
  <Command>
    <Name>DatabaseManagementStatus</Name>
    <CLICommand>get_db_management_status</CLICommand>
    <Status>Success</Status>
  </Command>
  <Status>
    <Parameters>
      <Parameter name="Last Backup" type="String">Fri Jul 17 17:53:47 PDT 2009</Parameter>
      <Parameter name="Last Restore" type="String">Fri Jul 17 17:53:47 PDT 2009</Parameter>
      <Parameter name="Restored From" type="String"></Parameter>
    </Parameters>
  </Status>
</CLIStatus>
 
*/
