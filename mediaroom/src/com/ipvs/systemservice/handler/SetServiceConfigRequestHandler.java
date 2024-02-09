package com.ipvs.systemservice.handler;

import java.util.Vector;

import org.dom4j.Document;

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
 
public class SetServiceConfigRequestHandler implements MRRequestHandlerI {

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
    private void addToList(Vector<String> vec, String argument, String value){
    	/*
	 		vec.add("-daily_backup");
	        if(!daily_backup.equals("")){
	            vec.add(daily_backup);
	        }else{
	            vec.add("");
	        }    	  
    	 */
    	 vec.add("-"+argument);
         if(value !=null && !value.trim().equals("")){
             vec.add(value);
         }else{
             //vec.add("\"\"");
             vec.add("");
         }
    }
    private String[] getCLI_set_db_management(String recovery_mode,  String daily_backup, String when,  String enable_export, String export_url) {
        Vector<String> vec = new Vector<String>();
        vec.add("/V2O/bin/vutils");
        vec.add("set_db_management");
        
        //the cli command allows to set individual parameters at a time..
        addToList(vec, "daily_backup", daily_backup);
        addToList(vec, "when", when);
        addToList(vec, "recovery_mode", recovery_mode);
        addToList(vec, "enable_export", enable_export);
        addToList(vec, "export_url", export_url);
      
        String[] cmdArgs = new  String[vec.size()];
        vec.copyInto(cmdArgs);
        return cmdArgs ;
    }
    
    public Document getResponse(MRRequest request, XMPPI xmpp, MRLog log, MRDB db,
            MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory)
            throws Exception {
        
            //parse request
            Document reqDoc = request.getMessage().getDocument();
            String recovery_mode = MRXMLUtils.getValueXpath(reqDoc, "//SetServiceConfigRequestData//RecoveryMode", "").trim();
            String daily_backup = SystemServiceUtils.booleanToYes( MRXMLUtils.getValueXpath(reqDoc, "//SetServiceConfigRequestData//DailyBackup", "").trim(), "" );
            String when = MRXMLUtils.getValueXpath(reqDoc, "//SetServiceConfigRequestData//BackupTime", "").trim();
            
            //added ftp support    
            String enable_export=  SystemServiceUtils.booleanToYes( MRXMLUtils.getValueXpath(reqDoc, "//SetServiceConfigRequestData//EnableExport", "").trim(),"");
            String export_url  = MRXMLUtils.getValueXpath(reqDoc, "//SetServiceConfigRequestData//ExportURL", "").trim();
            if(recovery_mode.equals("") && daily_backup.equals("") && when.equals("") && enable_export.equals("") && export_url.equals("")  ){
                //atleast one parameter is required
                throw new MRException(SystemServiceConst.SET_SERVICE_CONFIG_ERROR, MRXMLUtils.generateErrorXML("scriptError",  " Atleast one parameter is required- RecoveryMode, DailyBackup, BackupTime, EnableExport, ExportURL."));
            }
            //Other validations are done in script. no need to add validations here.
            //first : do set the settings 
            StringBuffer buf = new StringBuffer();
            int exitVal = 0;  
            String[] cmdArgs = getCLI_set_db_management(recovery_mode, daily_backup, when, enable_export, export_url);
            exitVal = SystemServiceUtils.executeProcess(cmdArgs, buf);
            if(exitVal !=0){
                //script should not return non zero value.
                throw new MRException(SystemServiceConst.SET_SERVICE_CONFIG_ERROR, MRXMLUtils.generateErrorXML("scriptError",  " '" + buf.toString() + "'."));
            }
            if(buf.toString().toLowerCase().indexOf("error") != -1 ){
                //there was some error in script.
                throw new MRException(SystemServiceConst.SET_SERVICE_CONFIG_ERROR, MRXMLUtils.generateErrorXML("scriptError",  " '" + buf.toString() + "'."));
            }
            
            //next: do get the settings 
            buf = new StringBuffer();
            String[] cmdArgsGet = { "/V2O/bin/vutils", "get_db_management" };
            exitVal = SystemServiceUtils.executeProcess(cmdArgsGet, buf);
            if(exitVal !=0){
                //script should not return non zero value.
                throw new MRException(SystemServiceConst.SET_SERVICE_CONFIG_ERROR, MRXMLUtils.generateErrorXML("scriptError",  " '" + buf.toString() + "'."));
            }
            if(buf.toString().toLowerCase().indexOf("error") != -1 ){
                //there was some error in script.
                throw new MRException(SystemServiceConst.SET_SERVICE_CONFIG_ERROR, MRXMLUtils.generateErrorXML("scriptError",  " '" + buf.toString() + "'."));
            }
            
            //create and send response
            StringBuffer bufRes = new StringBuffer();
            String rdname = request.getResponseDataName();
            bufRes.append("<" + rdname + ">");
            try {
                bufRes.append(SystemServiceUtils.parseGetServiceConfig(buf.toString()));
            } catch (Exception ex) {
                //the script returned non parsable/ invalid xml.
                throw new MRException(SystemServiceConst.GET_SERVICE_CONFIG_ERROR, MRXMLUtils.generateErrorXML("scriptError", " Error in parse '" + buf.toString() + "'." + ex.getMessage()));
            }
            bufRes.append("</" + rdname + ">");
            //System.out.println("SetServiceConfigRequestHandler getResponse "+bufRes.toString());
           
            return MRXMLUtils.stringToDocument(bufRes.toString());      
        }
    
}

 