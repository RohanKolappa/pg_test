package com.ipvs.systemservice.handler;

import java.io.BufferedReader;
import java.io.File;
import java.io.InputStreamReader;
import java.net.URLEncoder;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.StringTokenizer;

import org.apache.commons.lang.StringUtils;
import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.client.ServiceAgentUtils;
import com.ipvs.common.MRConst;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceConst;
import com.ipvs.sessionservice.impl.SessionServiceConst;
import com.ipvs.sessionservice.resource.handler.SessionServiceResourceConst;
import com.ipvs.systemservice.impl.SystemServiceConst;
import com.ipvs.utils.Utils;


public class SystemServiceUtils {
    //>Fri Jul 17 17:53:47 PDT 2009
    private static SimpleDateFormat script_df = new SimpleDateFormat("EEE MMM d HH:mm:ss z yyyy");
    public static String formatScriptDate(Date date){
        return script_df.format(date);
    }
    public static int executeProcess( String[] cmd, StringBuffer buf) throws Exception {
        Runtime rt = Runtime.getRuntime();
        Process process = rt.exec(cmd);
        BufferedReader stdout = new BufferedReader(new InputStreamReader(process.getInputStream()));
        String line = stdout.readLine();
        while (line != null) {
            //System.out.println(line);
            buf.append(line);
            line = stdout.readLine();
        }
        int exitVal = process.waitFor();
        if(exitVal !=0){
            System.out.println("Opps.. Script exited with non zero value ....!!!");
            SystemServiceUtils.printArray(cmd);
            System.out.println("Script output: "+buf.toString());
        }
        //todo: remove after testing 
        //System.out.println("################ executeProcess Start ################################  "  );
        //printArray(cmd);
        //System.out.println("exit value: " + exitVal +" "  );
        //System.out.println("script log : " + buf.toString() +" "  );
        //System.out.println("################ executeProcess End   ################################  "  );
        
        return exitVal;
    }
    
    public static int executeProcessInThread(String[] cmd, StringBuffer buf) throws Exception {
        //MRLog.println("executeProcessInBackground start");
        ExecHelper execHelper = new ExecHelper(cmd, buf);
        new Thread(execHelper).start();
        Thread.sleep(200);//wait for any system level errors in script execution
        int exitVal = execHelper.exitVal;
        if (exitVal != 0) {
            MRLog.println("Opps.. Script exited with non zero value ....!!!");
            SystemServiceUtils.printArray(cmd);
            MRLog.println("Script output: " + buf.toString());
        }
        //MRLog.println("executeProcessInBackground end");
        return exitVal;
    }

    public static class ExecHelper implements Runnable {
        int exitVal = 0;
        StringBuffer output = new StringBuffer();
        String[] cmd;
        public ExecHelper(String[] cmd, StringBuffer buf) {
            this.output = buf;
            this.cmd = cmd;
        }

        public void run() {
            try {
                exitVal = executeProcess(cmd, output);
            } catch (Exception ex) {
                exitVal=1;
                output.append("Error during process execution. " + ex.getMessage());
                ex.printStackTrace();
            }
        }
    }
    
    public static String getDatabaseManagementStatus(){
        StringBuffer buf = new StringBuffer();
        String[] cmdArgs = { "/V2O/bin/vutils", "get_db_management_status"};
        int exitVal = 0;
        try {
            exitVal = SystemServiceUtils.executeProcess(cmdArgs, buf);
            return buf.toString();
        } catch (Exception e) {
            e.printStackTrace();
            return "<Parameters><Parameter name=\"Error\" type=\"String\">exitVal=" + exitVal + " "+  buf.toString()+ "</Parameter>";
        }
    }    
    
    public static Element parseServiceStatusParameters(String xml) throws Exception  {
         Document doc = MRXMLUtils.stringToDocument(xml);
         return MRXMLUtils.getElementXpath(doc, "//Status/Parameters");
    }   
    
    public static String getAttibuteValue(Element elObjectEntry, String xpath, String defaultValue) throws Exception {
        if(elObjectEntry !=null){
            Element el = MRXMLUtils.getElementXpath(elObjectEntry, xpath);
            //System.out.println("el == null " +(el==null) + " " + xpath);
            if (el != null) {
                return MRXMLUtils.getAttributeValue(el, "value", defaultValue);
            }
        }
        return defaultValue;
    }

    public static boolean yesToBoolean(String value, boolean bdefault){
        if(value==null){
            return bdefault;
        }
        if(value.equalsIgnoreCase("yes") || value.equalsIgnoreCase("true") || value.equals("1")){
            return true;
        }else if(value.equalsIgnoreCase("no") || value.equalsIgnoreCase("false") || value.equals("0")){
            return false;
        }else{
            return bdefault;
        }
    }
    public static String booleanToYes(String value, String bdefault){
        if(value==null){
            return bdefault;
        }
        if(value.equalsIgnoreCase("yes") || value.equalsIgnoreCase("true") || value.equals("1")){
            return "Yes";
        }else if(value.equalsIgnoreCase("no") || value.equalsIgnoreCase("false") || value.equals("0")){
            return "No";
        }else{
            return bdefault;
        }
    }    
    public static String parseGetServiceConfig(String xml) throws Exception {
        Document doc = MRXMLUtils.stringToDocument(xml);
        Element elObjectEntry = MRXMLUtils.getElementXpath(doc, "//ObjectEntry[@name='DatabaseManagement']");
        //System.out.println("elObjectEntry == null " +(elObjectEntry==null) + "xml=" +xml.replace('\n', ' ') );
        boolean daily_backup = yesToBoolean(getAttibuteValue(elObjectEntry, "Attribute[@name='daily_backup']", "").trim().toLowerCase(), false);
        String when = getAttibuteValue(elObjectEntry, "Attribute[@name='when']", "").trim();
        String recovery_mode = getAttibuteValue(elObjectEntry, "Attribute[@name='recovery_mode']", "").trim();
        //added support for ftp
        boolean enable_export = yesToBoolean(getAttibuteValue(elObjectEntry, "Attribute[@name='enable_export']", "").trim().toLowerCase(), false);
        String export_url = getAttibuteValue(elObjectEntry, "Attribute[@name='export_url']", "").trim();
        
        //modified as per miyer review ... using the document template and updating values with xpath.
        // (the IPVSDBService SetServiceConfigRequest 
        // will be same as IPVSDBService GetServiceConfigResponse.)
        String xmlFilePath="/com/ipvs/systemservice/xml/SetServiceConfigRequest.xml";
        Document document = MRXMLUtils.loadXMLResource(xmlFilePath, MRLog.getInstance(MRLog.CONSOLE));
        MRXMLUtils.setValueXpath(document, "//IPVSDBService/Backup/DailyBackup", daily_backup+"");
        MRXMLUtils.setValueXpath(document, "//IPVSDBService/Backup/BackupTime", when+"");
        MRXMLUtils.setValueXpath(document, "//IPVSDBService/Restore/RecoveryMode", recovery_mode+"");
        //added support for ftp
        MRXMLUtils.setValueXpath(document, "//IPVSDBService/Backup/EnableExport", enable_export+"");
        MRXMLUtils.setValueXpath(document, "//IPVSDBService/Backup/ExportURL", export_url+"");
        Element elIPVSDBService = MRXMLUtils.getElementXpath(document, "//IPVSDBService");
        //System.out.println(" MRXMLUtils.elementToString(elIPVSDBService) " + MRXMLUtils.elementToString(elIPVSDBService));
        return MRXMLUtils.elementToString(elIPVSDBService);
    } 
    public static void printArray(String[] array){
        if(array !=null){
            StringBuffer buf = new StringBuffer();
            for (int i = 0; i < array.length; i++) {
                if(i!=0 ){
                    buf.append(" ");
                }
                buf.append(array[i] ); 
            }
            System.out.println( buf.toString() );
        }
    }
        
	public static void sendFileTransferRequest(String fromAgentJID,String fromFilename,String toAgentJID,String toFilename,String url,String responseHandlerName,MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception {
    	
    	String fromAgentName = MediaRoomServiceConst.APPSERVERAGENT;
    	String fileTransferId = Utils.getUUID();
		String toAgentName = SystemServiceConst.FILETRANSFERAGENT;	    	
								
		Document requestDoc = MRXMLUtils.loadXML(SystemServiceConst.FILEXFERSETUPREQUESTFILE, log);
		MRXMLUtils.setValueXpath(requestDoc, "//fromAgentJID", fromAgentJID);
		MRXMLUtils.setValueXpath(requestDoc, "//fromFileName", fromFilename);
		MRXMLUtils.setValueXpath(requestDoc, "//FileXferID",  fileTransferId);
		MRXMLUtils.setValueXpath(requestDoc, "//toAgentJID", toAgentJID);
		MRXMLUtils.setValueXpath(requestDoc, "//toFileName", toFilename);
		MRXMLUtils.setValueXpath(requestDoc, "//url", url);
		String requestName =  MRXMLUtils.getAttributeValueXpath(requestDoc,MRConst.HEADER_XPATH,MRConst.REQUEST_NAME, "");
		MRRequest ftpRequest = MRRequest.createRequest(request, requestDoc, requestName, request.getServiceName());	  	   
		Element data = MRXMLUtils.getElementXpath(requestDoc, ftpRequest.getRequestDataXPath());
		String dataStr = MRXMLUtils.elementToString(data);

		String info = "DownloadDBRequest::sendStartTransfer " + 
		fromAgentName + " to " + toAgentName;
		String serviceName = MRConst.FTP_SERVICE;
	
		Document userRequestDoc = request.marshalRequestObject(log, responseHandlerName);
		String clientData = URLEncoder.encode(MRXMLUtils.documentToString(userRequestDoc), "UTF-8");
		ServiceAgentUtils.sendAgentRequestMessage(xmpp, log, null, fromAgentName, toAgentJID, toAgentName, requestName, dataStr,
				info, request.getNID(), request.getUserJID(), serviceName, clientData);
				
	}
    
    public static boolean upgradeDeviceServiceVersionCheck(String serviceVersion, String upgradeFeatureServiceVersion) throws Exception
    {    	
    	if (serviceVersion == null || serviceVersion.isEmpty() || serviceVersion.equals(SessionServiceResourceConst.NOT_AVAILABLE))
    		return false;
    	
    	StringTokenizer currentServiceVersion = new StringTokenizer(serviceVersion, ".");
		StringTokenizer upgradeServiceVersion = new StringTokenizer(upgradeFeatureServiceVersion, ".");
						
		while (currentServiceVersion.hasMoreTokens() && upgradeServiceVersion.hasMoreTokens())
		{
			int currentServiceVer = Integer.parseInt(currentServiceVersion.nextToken());
			int upgradeFeatureServiceVer = Integer.parseInt(upgradeServiceVersion.nextToken());
			
			if ( currentServiceVer < upgradeFeatureServiceVer )	
			{
				return false;
			}
			else if ( currentServiceVer > upgradeFeatureServiceVer )
			{
				return true;
			}
		}
		
		return true;
    }
    
    public static boolean validateNGSSoftwareVersion(String softwareVersion, MRLog log) throws Exception {
    	
    	if (softwareVersion == null || softwareVersion.isEmpty())
    		return false;
    	
    	StringTokenizer currentSoftwareVesion = new StringTokenizer(softwareVersion, ".");
    	StringTokenizer supportedSoftwareVesion = new StringTokenizer(SystemServiceConst.UPGRADE_SUPPORTED_SOFTWARE_VERSION, ".");
    	int count = 1;
    	while(supportedSoftwareVesion.hasMoreTokens() && currentSoftwareVesion.hasMoreTokens()) {
			String sVerStr = supportedSoftwareVesion.nextToken();
			String cVerStr = currentSoftwareVesion.nextToken();
			
			if(sVerStr.startsWith("r") && cVerStr.startsWith("r")) {
				sVerStr = StringUtils.substringAfter(sVerStr, "r");
				cVerStr = StringUtils.substringAfter(cVerStr, "r");
			} else if(sVerStr.startsWith("b") && cVerStr.startsWith("b")) {
				sVerStr = StringUtils.substringAfter(sVerStr, "b");
				cVerStr = StringUtils.substringAfter(cVerStr, "b");
			}  
			int supportedVersionNumber = 0;
			int currentVersionNumber =  0;
			try {
				supportedVersionNumber = Integer.parseInt(sVerStr);
				currentVersionNumber =  Integer.parseInt(cVerStr);
			} catch (Exception e) {
				return false;
			}
			
			// check for major version number with first two digits.
			if(currentVersionNumber > supportedVersionNumber && count <= 2) 
				return true;
			
			if(currentVersionNumber < supportedVersionNumber) {
				return false;
			}
			count += 1;
		}
    	
    	return true;
    }
    
    public static Document getSoftwareImageStatus(MRLog log) {
        String[] cmdArr = { SystemServiceConst.GET_SOFTWARE_IMAGE_STATUS_CLI_CMD };
        Document cmdResultDoc = ServiceAgentUtils.executeCLICommand(cmdArr, log);

        return cmdResultDoc;
    }
    
    public static String getDeviceSoftwareVersionOnMS(String deviceType, MRLog log) throws Exception {
    	String deviceSoftwareVersion = "";
    	
    	if(SystemServiceConst.NSGD200_DEVICE.equals(deviceType)) {
    		String versionFilePath = "/home/onair/modules/mediaroom/public/ngs_IMAGES/server.version";
    		File fileObject = null;
    		try {
    			fileObject = new File(versionFilePath);
    			if (fileObject.exists()) {
    				String[] cmdArr = { "cat", versionFilePath };
    				log.addInfo("Reading device software version from " + versionFilePath);
    				StringBuffer buf = new StringBuffer();
    		        int cmdResult = SystemServiceUtils.executeProcess(cmdArr, buf);
    		        if(cmdResult != 0) {
    		        	log.addInfo("Failed reading device software version from " + versionFilePath);
    		        }else {
						deviceSoftwareVersion = buf.toString();
					}
    			}
    		} catch (Exception e) {
    			log.addInfo("Failed reading device software version from " + versionFilePath);
    		}
    	}
    	
    	return deviceSoftwareVersion;
    }
    
    public static Document getDownloadRequestDocument(String requestName, MRLog log, String folderPath,String toAgentJID,String downloadURL, String type) throws Exception {
		Document requestDocTemplate = MRXMLUtils.loadXMLResource(SessionServiceConst.DEFAULT_REQUEST_TEMPLATE, log);
		Document requestDoc = (Document) requestDocTemplate.clone();
		MRXMLUtils.setAttributeValueXPath(requestDoc.getRootElement(), MRConst.HEADER_XPATH, MRConst.REQUEST_NAME, requestName);
		MRXMLUtils.setAttributeValueXPath(requestDoc.getRootElement(), MRConst.HEADER_XPATH, MRConst.SERVICE_NAME, MRConst.SESSION_RESOURCE_SERVICE);
		Element DataEL = MRXMLUtils.getElementXpath(requestDoc, "//Data");
		Element requestDataEl = MRXMLUtils.addElement(DataEL, MRXMLUtils.newElement(requestName + "Data"));
		Element requestEl = MRXMLUtils.newElement(requestName.replace("Request", ""));
		MRXMLUtils.addElement(requestEl, "folderPath", folderPath);
		MRXMLUtils.addElement(requestEl, "url", downloadURL);
		MRXMLUtils.addElement(requestEl, "type", type);
		MRXMLUtils.addElement(requestEl, "toAgentJID", toAgentJID);
		MRXMLUtils.addElement(requestDataEl, requestEl);
		return requestDoc;
	}
    
}
