package com.ipvs.systemservice.impl;

import java.net.InetAddress;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.Properties;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.appserver.AppServerAgentUtils;
import com.ipvs.assetadminservice.handler.AMConst;
import com.ipvs.assetadminservice.impl.AssetAdminServiceUtils;
import com.ipvs.client.ServiceAgentUtils;
import com.ipvs.client.ServiceClient;
import com.ipvs.common.MRConst;
import com.ipvs.common.MRException;
import com.ipvs.common.MRExceptionCodes;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.db.DBServiceUtils;
import com.ipvs.common.db.MRConfig;
import com.ipvs.common.db.MRXMLDBUtils;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.config.IPVSConst;
import com.ipvs.device.impl.DeviceConst;
import com.ipvs.mediaroomservice.impl.AppServerAgent;
import com.ipvs.mediaroomservice.impl.ConfigStreamAgent;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceUtils;
import com.ipvs.systemservice.handler.SystemServiceUtils;
import com.ipvs.utils.FileUtils;
import com.ipvs.utils.Utils;



class HAService{

	private  Properties haConfProperties = null;

	public HAService(MRLog log){
		try {
			this.setHaConfProperties(Utils.loadProperties(DeviceConst.HA_CONF_FILE_PATH));    		
			log.addInfo("HAUtils:HAService HA Active");
		}catch(Exception exp){
			this.setHaConfProperties(null);
		}
	}

	public  Properties getHaConfProperties() {
		return haConfProperties;
	}

	public  void setHaConfProperties(Properties haConfProperties) {
		this.haConfProperties = haConfProperties;
	}

	/*public String getServer(){
		return haConfProperties.getProperty(DeviceConst.HA_CONF_PROPERTY_SERVER);    
	}*/

	public boolean isReplicationEnabled(){
		String replicationStr =  haConfProperties.getProperty(DeviceConst.HA_CONF_PROPERTY_REPLICATE);
		return Utils.getBooleanValue(replicationStr, false);
	}

	public boolean isSecondary(){
		String secondaryStr =  haConfProperties.getProperty(DeviceConst.HA_CONF_PROPERTY_SECONDARY);
		return Utils.getBooleanValue(secondaryStr, false);
	}
	public String getDBDir(){
		return haConfProperties.getProperty(DeviceConst.HA_CONF_PROPERTY_DBDIR);    
	}
	
	public boolean dbRecreatePending() {
		String[] cmd = {"/etc/scripts/ipvsxmldbctl.sh", "check_createdbScheduled"};
		try {
			StringBuffer sb = new StringBuffer();
			int retVal = SystemServiceUtils.executeProcess(cmd, sb);
			return (retVal == 0);
		}
		catch(Exception e) {
			e.printStackTrace();
			return false;
		}
	}

}

public class HAUtils {
		
	private static final String REPLICATION_STATUS_ROOT_DIR = "/var/status"; 
	private static final String REPLICATION_STATUS_FILE_NAME = "replication"; 	
	
	private static HAService haService = null;
/*	private static final String TAR_FILE_NAME = "tar_file.sh";
	private static final String UNTAR_FILE_NAME = "untar_file.sh";	
*/	private static final long DBWAIT = 60000;
	// private static final long DBWAIT = 0;

	public static void updateHAProperties(MRLog log){
		haService = new HAService(log);		
	}

	public static boolean isHAServiceEnabled(){

		if(haService == null ||  haService.getHaConfProperties() == null) {
			return false;
		}
		return true;
	}
	
	public static boolean isServerInStandby(){
		if(!isHAServiceEnabled()){
			return false;
		}
		return haService.isReplicationEnabled();
	}
	
	private static void logHA(String str)
	{
		try
		{
			String prevLog = "";
			String ReplicationStatusLogFile = REPLICATION_STATUS_ROOT_DIR + "/" + REPLICATION_STATUS_FILE_NAME;
			if (FileUtils.isFileExist(REPLICATION_STATUS_ROOT_DIR, REPLICATION_STATUS_FILE_NAME))
			{
				prevLog = FileUtils.readFile(ReplicationStatusLogFile);										
			}
			SimpleDateFormat dateFormat = new SimpleDateFormat( "dd HH:mm:ss");
			String timeStr = dateFormat.format(new Date());
			FileUtils.writeFile(ReplicationStatusLogFile, prevLog + timeStr + " " + str + "\n");
		}
		catch (Exception e)
		{
			e.printStackTrace();
		}
	}

	//Handle HA Service if enabled. Start the replication process based on configuration
	public static void handleHA(MRLog log, MRDB db, XMPPI xmpp, ServiceAgentFactoryI serviceAgentFactory, String userJID, String deviceType,Document setServiceDomainResponseDoc, String  downloadDbRequestToken) throws Exception {
		//Check HA configuration file is exists or not. If not then ha service is not running just return.
		
		if(!isHAServiceEnabled()) {
		    if(isStandaloneMSForMyDomain(userJID, log, db, xmpp)){
		        updateServiceResourceWithStandaloneState(userJID, log, db, xmpp);
		    }		    
		    resetServiceResourceWithHAState(userJID, log, db, xmpp, deviceType, setServiceDomainResponseDoc);			
		    return;
		}
		
		updateServiceResourceWithHAState(userJID, log, db, xmpp);
		//Check do we need to start replication or not.
		if(!haService.isReplicationEnabled()){
			log.addInfo("HAUtils:handleHA Replication disabled");
			return;
		}
		log.addInfo("HAUtils:handleHA Replication enabled");

		// If primary, Check if db reset/upload is set in which case we do not want to start replication
		if(haService.dbRecreatePending()) {
			if(haService.isSecondary()){
				log.addInfo("HAUtils:handleHA handleHA Error Secondary has dbCreatePending");
				return;
			}
			else {
				log.addInfo("HAUtils:handleHA Replication masked since dbCreatePending");
				return;
			}
		}
		
		sendDownloadDBRequest(log,xmpp,serviceAgentFactory,userJID, downloadDbRequestToken);
	}
	


	// star the replication process
	private static void startReplication(MRLog log, XMPPI xmpp, String userJID) throws Exception{		
		String[] systemCmd = {"sh" , "/etc/scripts/ipvsxmldbctl.sh", "startreplication",DeviceConst.DOWNLOAD_DIR_PATH,DeviceConst.DOWNLOAD_TGZ_FILE_NAME};		
		int commandResult = MediaRoomServiceUtils.executeSystemCommand(systemCmd);
		handleCommandResult(commandResult, "StartReplicationFailed",log);
		log.addInfo("HAUtils:startReplication Start Replication Process");				
		logHA("StartedReplication");
	}
	
	// stop the replication process
	private static void stopReplication(MRLog log) throws Exception{		
		String[] systemCmd = {"sh" , "/etc/scripts/ipvsxmldbctl.sh", "stopreplication"};		
		int commandResult = MediaRoomServiceUtils.executeSystemCommand(systemCmd);	
		handleCommandResult(commandResult, "StopReplicationFailed",log);
		log.addInfo("HAUtils:stopReplication Stop Replication Process");				
		logHA("StoppedReplication");
	}
	
	private static void handleCommandResult(int commandResult,String errorMessage,MRLog log) throws MRException,Exception {
		if(commandResult != 0 ) {
			log.addInfo("HAUtils:"+ errorMessage);
			logHA(errorMessage);
			throw new MRException(SystemServiceConst.EXECUTE_COMMAND_FAILED, errorMessage);
		}
	}

	
	//send the download request to primary
	private static void sendDownloadDBRequest(MRLog log,XMPPI xmpp,ServiceAgentFactoryI serviceAgentFactory,String userJID, String downloadDbRequestToken) throws Exception {
		
		log.addInfo("HAUtils:sendDownloadDBRequest stop replication before sending download request");
		stopReplication(log);

		log.addInfo("HAUtils:sendDownloadDBRequest wait before Send download request to primary=" + DBWAIT);
		Thread.sleep(DBWAIT);
		
		log.addInfo("HAUtils:sendDownloadDBRequest Send download request to primary");
		String archiveFilename = DeviceConst.DOWNLOAD_DIR_PATH+"/"+DeviceConst.DOWNLOAD_TGZ_FILE_NAME;
		if(FileUtils.isFileExist(DeviceConst.DOWNLOAD_DIR_PATH, DeviceConst.DOWNLOAD_TGZ_FILE_NAME))
			FileUtils.removeFileOrDir(archiveFilename);
		FileUtils.createDirIfNotExist(DeviceConst.DOWNLOAD_DIR_PATH);
		Document downloadDBRequestDoc = MRXMLUtils.loadXMLResource(DeviceConst.DOWNLOAD_DB_REQUEST_XML_PATH, log);
		MRXMLUtils.setValueXpath(downloadDBRequestDoc, "//"+DeviceConst.DOWNLOAD_DIR_EL_NAME, DeviceConst.DOWNLOAD_DIR_PATH+"/"+DeviceConst.DOWNLOAD_TGZ_FILE_NAME);
		String requestName =MRXMLUtils.getAttributeValueXpath(downloadDBRequestDoc,MRConst.HEADER_XPATH,MRConst.REQUEST_NAME, "");
		MRXMLUtils.setValueXpath(downloadDBRequestDoc, "//"+MRConst.CLIENT_DATA, downloadDbRequestToken);
		String fromAgentClassName = ConfigStreamAgent.class.getName();
		MediaRoomServiceUtils.sendMessageToAppServerAgent(xmpp, userJID, downloadDBRequestDoc, log,requestName,fromAgentClassName, AppServerAgentUtils.getAppServerAgentJID(userJID,serviceAgentFactory));		
		logHA("Downloading");			
	}
	
	// Handles the download request on appserver
	//created tgz file and send the file transfer request to sender
	public static String handleDownloadDbRequest(MRRequest request, XMPPI xmpp,
			MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory,String responseHandlerName) throws MRException,Exception {
		Document requestDoc = request.getMessage().getDocument();
						
		String downloadDir = MRXMLUtils.getValueXpath(requestDoc, "//"+DeviceConst.DOWNLOAD_DIR_EL_NAME, "");

		String dbDir = haService.getDBDir();

		//append the data dir to db directory
		String liveDbDir = dbDir+ DeviceConst.DATA_DIR_NAME;
		String uploadDbDir = dbDir+DeviceConst.UPLOAD_DIR_NAME;

		//remove the download tgz file if exists
		FileUtils.removeFileOrDir(uploadDbDir+"/"+DeviceConst.DOWNLOAD_TGZ_FILE_NAME);

		//create replication file in downoad db
		createreplicationfile(uploadDbDir,liveDbDir,dbDir,log);

		String uploadFileDir = uploadDbDir +"/"+DeviceConst.DOWNLOAD_TGZ_FILE_NAME;
		
		return uploadFileDir;
	}
	

/*	private static void createUploadDirIfNotExist(String uploadDbDir){

		File uploadDirFile = new File(uploadDbDir);
		if(!uploadDirFile.exists()){
			uploadDirFile.mkdirs();
		}
	}*/

	private static void createreplicationfile(String uploadDbDir,String liveDbDir,String dbDir,MRLog log) throws MRException,Exception {

		/*//check upload db dir exists or not.
		createUploadDirIfNotExist(uploadDbDir);
			
		
		 * create sh file in db directory (/data/existdbserver/) which will create tar the file     	 
		 * cd /data/existdbserver/data/
		 * tar -cvvf /data/existserver/upload/download_db.tgz *
		 

		//create sh file 
		String archiveFilename = uploadDbDir + "/" + DeviceConst.DOWNLOAD_TGZ_FILE_NAME;
		String commands = "cd " + dbDir +"\n" + 
						  "tar -cvf " + archiveFilename +" "+DeviceConst.DATA_DIR_NAME + 
						  " --exclude=data/sanity " +
						  " --exclude=data/*.lck ";
		FileUtils.writeFile(dbDir+TAR_FILE_NAME, commands);
		
		//run the sh file
		String[] systemCmd1 = {"sh",dbDir+TAR_FILE_NAME};
		MediaRoomServiceUtils.executeSystemCommand(systemCmd1);

		//remove the temporary sh file
		FileUtils.removeFileOrDir(dbDir+TAR_FILE_NAME);*/
		
		String[] systemCmd = {"sh" , "/etc/scripts/ipvsxmldbctl.sh", "createreplicationfile", uploadDbDir, DeviceConst.DOWNLOAD_TGZ_FILE_NAME};		
		int commandResult = MediaRoomServiceUtils.executeSystemCommand(systemCmd);
		handleCommandResult(commandResult, "CreateReplicationFile Failed",log);
		log.addInfo("HAUtils:CreateReplicationFile");
	}
/*
	private static void startFileTransfer(String uploadDbDir,String downloadDir,String responseHandlerName,MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception {
	    String fromAgentJID = AppServerAgentUtils.getAppServerAgentJID(request.getUserJID(),serviceAgentFactory);
		String fromFilename = uploadDbDir;
		String toAgentJID = request.getUserJID();
		String toFilename = downloadDir;

		SystemServiceUtils.sendFileTransferRequest(fromAgentJID, fromFilename, toAgentJID, toFilename,"", responseHandlerName, request, xmpp, log, db, resourceFactory, serviceAgentFactory);
				
	}*/
	
	//handle the download response from primary. copy the downlaoded files to live db dig and start replication process
	public static void handleDownloadDBResponse(Document responseDoc,MRLog log, XMPPI xmpp, String userJID, ServiceAgentFactoryI serviceAgentFactory) throws Exception {	

		log.addInfo("HAUtils:handleDownloadDBResponse Handle download Response and start replication process");

		try {
			int state = Utils.getIntValue(MRXMLUtils.getAttributeValueXpath(responseDoc,MRConst.HEADER_XPATH,MRConst.STATE, ""), MRRequest.STATE_EXCEPTION);			
			if(state == MRRequest.STATE_EXCEPTION) {
				String exceptionCode= MRXMLUtils.getValueXpath(responseDoc, "//Exception/Code", "");
				String exceptionDescription= MRXMLUtils.getValueXpath(responseDoc, "//Exception/Description", "");
				log.addInfo("HAUtils:handleDownloadDBResponse HandleDownloadDBResponse Failed: Code:="+exceptionCode  +" Description:="+exceptionDescription);
				System.out.println("responseDoc :="+responseDoc.asXML());
				logHA("DownloadFailed");
				throw new MRException(Utils.getIntValue(exceptionCode, MRExceptionCodes.SYSTEMERROR), exceptionDescription);
			}
			
			logHA("DownloadFinished");
			//String dbDir = haService.getDBDir();
			//copyDownloadedFilesToLiveDB(log,dbDir);
			startReplication(log, xmpp, userJID);
		} catch(MRException e) { // Downloaded a bad db, Resend the download request
			log.addInfo("HAUtils:startReplication Failed, resending download request");	
			String requestToken = MRXMLUtils.getValueXpath(responseDoc, "//"+MRConst.CLIENT_DATA, "");
			HAUtils.sendDownloadDBRequest(log, xmpp, serviceAgentFactory, userJID, requestToken);
			throw e;
		}

		//log.addInfo("HAUtils:handleDownloadDBResponse Wait before returning=" + DBWAIT);
		//Thread.sleep(DBWAIT);
		log.addInfo("HAUtils:sendDownloadDBRequest returning");


	}	
	
	/*//copy the download files from download dir to live db dir
	private static  void copyDownloadedFilesToLiveDB(MRLog log,String dbDir) throws Exception {

		String liveDbDir = dbDir+DeviceConst.DATA_DIR_NAME;    	
		String temporarySaveDbDir = dbDir + DeviceConst.DATA_SAVE_DIR_NAME;
		String downloadDir = DeviceConst.DOWNLOAD_DIR_PATH;

		//untar the tgz file to db dir
        //untarFiles(dbDir,downloadDir+DeviceConst.DOWNLOAD_TGZ_FILE_NAME);
		
		//delete any left over copy of /data from download dir
		FileUtils.removeFileOrDir(downloadDir+DeviceConst.DATA_DIR_NAME);
		
		//untar in the same directory i.e., download dir
		untarFiles(downloadDir,downloadDir+DeviceConst.DOWNLOAD_TGZ_FILE_NAME);
		
		//move live db dir to temporary dir
		moveDir(liveDbDir, temporarySaveDbDir);
		
		//move the untarred downloaded data to live directory 
		moveDir(downloadDir + DeviceConst.DATA_DIR_NAME, dbDir );
        
		//remove the down loaded tgz file.
        //System.out.println("HAUtils:copyDownloadedFilesToLiveDB not deleting downloaded file:" + downloadDir+DeviceConst.DOWNLOAD_TGZ_FILE_NAME);
        //FileUtils.removeFileOrDir(downloadDir+DeviceConst.DOWNLOAD_TGZ_FILE_NAME);

		//remove the temporary saved db files        
        FileUtils.removeFileOrDir(temporarySaveDbDir);

		log.addInfo("HAUtils:copyDownloadedFilesToLiveDB Copied files from download dir to live db dir");
		
	}
	
	private static void untarFiles(String destinationDir,String sourceDir) throws Exception {
		
		 * create sh file which will untar the files
		 * cd  /data/download/
		 * tar -xvf database_db.tgz
		        	
		//create sh file 
		String commands = "cd " + destinationDir +"\n" + "tar -xvf " + sourceDir;
		FileUtils.writeFile(destinationDir+UNTAR_FILE_NAME, commands);
		
		//run sh file
		String[] systemCmd2 = {"sh",destinationDir+UNTAR_FILE_NAME};
		MediaRoomServiceUtils.executeSystemCommand(systemCmd2);
		
		//remove the temporary sh file
	    FileUtils.removeFileOrDir(destinationDir+UNTAR_FILE_NAME);		 
	}
	
	
	private static void moveDir(String sourceDir,String destinationDir) throws Exception{
	   	
	   	 *  mv the files in live db to save dir
	   	 *  mv /data/existdbserver/data/ /data/existdbserver/data_save
	   	 *  

		String[] systemCmd1 = { "mv" ,sourceDir , destinationDir};
		MediaRoomServiceUtils.executeSystemCommand(systemCmd1);  	
	}	*/
	
/*	//update the server ip if ha service enabled
	public static String  updateLoginRequest(String loginRequest) throws Exception {    
		if(!isHAServiceEnabled()){
			return loginRequest;
		}

		String server = haService.getServer();	    
		try {
			Document loginRequestDoc = MRXMLUtils.stringToDocument(loginRequest);
			MRXMLUtils.setValueXpath(loginRequestDoc, "//server", server);    	
			loginRequest = loginRequestDoc.asXML();
		}catch(Exception exp){
			throw exp;
		}
		return loginRequest;
	}*/

	//If the replication is false then app server is running.
	//When primary is in stand by mode app server agent is still running on primary. It should run on secondary.
	public static boolean enableAppserverAgentOrNot(boolean xmppEnabled)
	{
		if(!HAUtils.isHAServiceEnabled()) {
			return xmppEnabled; 
		}

		if(!haService.isReplicationEnabled()) {
			return true;
		}

		return false;
	}
	
	public static final String ON_OFF_HA_SERVER = "OnOffHAServer";

	// handles ha events sent my system. If the system is primary then will find the secondary owner jid 
	// send request to secondary to on/off ha server. 
	public static void handleHAEvents(String userJID, MRLog log, MRDB db,
			XMPPI xmpp, ServiceAgentFactoryI serviceAgentFactory,
			MRResourceFactoryI resourceFactory,Document deviceDoc,String action) throws MRException,Exception {
		
		if(haService.isSecondary() || haService.isReplicationEnabled()){
			return;
		}
		
		String secondaryIPAddress = MRXMLUtils.getValueXpath(deviceDoc, "//HAServiceConfig/HAServer/PeerIP","" );
	    String secondayName = MRXMLUtils.getValueXpath(deviceDoc, "//HAServiceConfig/HAServer/PeerName","" );
		
		String whereXpath  = "//EthernetPortStatus/Parameters/Parameter[(@name='IPAddress' and .='"+secondaryIPAddress+"') or (@name='Name' and .='"+secondayName+"') ]";
		String childXpath = "//AssetResourceNID";
		
	    MRRequest request = MRRequest.createRequest(userJID);
	    
    	//get the secondary agent JID.
	    String selectXML = "<Device>" +"{$VAR//AssetResourceNID}" + "</Device>";

    	Element responseEL = AssetAdminServiceUtils.getChildBasedOnSelectXML(AMConst.Device,selectXML, whereXpath, request, db, log, xmpp);    
    	
    	Element assetResourceNIDEL = MRXMLUtils.getElementXpath(responseEL, "./Device/AssetResourceNID");
     	if(assetResourceNIDEL ==null){
    		return;
    	}
    	String assetResourceNID = MRXMLUtils.getValue(assetResourceNIDEL);
    	childXpath = "//OwnerUserJID";
		responseEL = AssetAdminServiceUtils.getChild(assetResourceNID, childXpath, request, log, db, xmpp, resourceFactory, serviceAgentFactory);
		String secondaryJID = MRXMLUtils.getValue(MRXMLUtils.getElementXpath(responseEL, "./OwnerUserJID"));
		if(secondaryJID == null || secondaryJID.isEmpty()){
			log.addInfo("HAUtils:handleHAEvents Secondary Device in not logged intot the system.Can not send message to Secondary");
			return;	
		}
		
		//send OnOffHAserver request to secondary
		String agentName = ConfigStreamAgent.class.getName();
	    ServiceAgentUtils.sendAgentRequestMessage(xmpp, log, null, ConfigStreamAgent.class.getName(), secondaryJID, agentName, HAUtils.ON_OFF_HA_SERVER,
	    		"<data>" + 
	            "<action>" +action + "</action>" +
	            "</data>",
             "ConfigStreamAgent:=" + request.getUserJID() +" > OnOffHAServer Message to " +
                ServiceAgentUtils.getAgentShortName(agentName), request.getNID(),request.getUserJID(),MRConst.CONFIG_ADMIN_SERVICE,"");
	    
	}

	// secondary handle the request and perform the action on/off ha server
	public static void handleOnOffHAServer(Document requestDoc, MRLog log)  throws MRException,Exception {
		if(!haService.isSecondary() || !haService.isReplicationEnabled()){
			return;
		}
		String action = MRXMLUtils.getValueXpath(requestDoc, "//action", "");
		log.addInfo("HAUtils:handleOnOffHAServer Handle OnOffHAServer action:="+action);
		String[] systemCmd = {"hactl",action};
		MediaRoomServiceUtils.executeSystemCommand(systemCmd);
	}
	
	private static boolean isStandaloneMSForMyDomain(String userJID, MRLog log, MRDB db, XMPPI xmpp) throws Exception {
	    try {
    	    String xmppServer = getDeviceXmppServer(userJID, log, db, xmpp);
    	    if(isXMPPServerEnable(log) && isLocalServerIP(xmppServer, log)){
                return true;
            }	    
	    } catch (Exception e) {
            e.printStackTrace();
            log.addInfo("HAUtils:isStandaloneMSForMyDomain " + MRLog.stackToString(e));
        }
        log.addInfo("HAUtils:isStandaloneMSForMyDomain HA_STATE: isStandaloneMSForMyDomain false " );
	    return false;
	}
	
    private static void updateServiceResourceWithStandaloneState(String userJID, MRLog log, MRDB db,
            XMPPI xmpp) {
        if(IPVSConst.getInstance().isComponentLogin()){//(!xmpp.getLoginType().equals(IPVSSession.SMACK_CONNECTION)){
            //this is component login
            //log.addInfo("DO_NOTHING: HA_STATE: updateServiceResourceWithStandaloneState isComponentLogin " + userJID);
            return;
        }
        try {
            String assetResNID_active = getAssetResoureNIDForDeviceLogin(userJID, log, db, xmpp);
            MRConfig mrConfig = MRConfig.getInstance(AMConst.SERVICE_RESOURCE, log);
            Element serviceResRootEL = mrConfig.getXmlRepresentation(log).getDoc().getRootElement();    
            Element assetEL = MRXMLUtils.getElement(serviceResRootEL, "AssetResourceNID").createCopy();
            MRXMLUtils.setValue( assetEL , assetResNID_active );
            udpateServiceResourceInDB(log, db, xmpp, userJID, "//AssetResourceNID", assetEL);
            log.addInfo("HAUtils:updateServiceResourceWithStandaloneState HA_STATE: updateServiceResourceWithStandaloneState " + userJID +" assetResNID_active=" + assetResNID_active);
        } catch (Exception e) {
            e.printStackTrace();
            log.addInfo("HAUtils:updateServiceResourceWithStandaloneState" + MRLog.stackToString(e));
        }
    }
    
    private static void resetServiceResourceWithHAState(String userJID, MRLog log, MRDB db,
            XMPPI xmpp, String deviceType, Document setServiceDomainResponseDoc) throws Exception {
    	log.addInfo("HAUtils:resetServiceResourceWithHAState HA disabled on MS with userJID= " + userJID);    	
 	
    	if (!AMConst.MS_DEVICE_OBJECTYPE.equals(deviceType) && !AMConst.MS_25_DEVICE_OBJECTYPE.equals(deviceType)) {
    		return;
    	}
    	
        MRRequest request = MRRequest.createRequest(userJID);
        MRConfig config = MRConfig.getInstance(request.getServiceName(), AMConst.SERVICE_RESOURCE, log);
        String rootNID = db.getServiceDomainRootNID(config.getParentNIDKey(log));    
        Element serviceResourceEl = MRXMLDBUtils.readElement(1, null, null, rootNID, 1, null, null, request, db, log);
        if(serviceResourceEl==null) {
        	return;
        }
        Document serviceResourceDBDoc = MRXMLUtils.elementToDocument(serviceResourceEl);
        
        String assetResourceNID = MRXMLUtils.getAttributeValueXpath(setServiceDomainResponseDoc, "//" + AMConst.AssetResource, AMConst.NID, "").trim();        
        Element haEL = MRXMLUtils.getElementXpath(serviceResourceDBDoc, "//HA/*[@MSAssetResourceNID='" + assetResourceNID + "']");
        if (haEL != null) {
        	log.addInfo("HAUtils:resetServiceResourceWithHAState Reset HA on MS with userJID= " + userJID);
        	MRXMLUtils.setAttributeValue(haEL, "MSAssetResourceNID", "");               
        	MRXMLUtils.setAttributeValue(haEL, "state", "");
        	udpateServiceResourceInDB(log, db, xmpp, userJID, "//" + haEL.getName(), haEL);        	
        }        	       	            
    }

    private static void updateServiceResourceWithHAState(String userJID, MRLog log, MRDB db, XMPPI xmpp) throws Exception {
        if(IPVSConst.getInstance().isComponentLogin()){//(!xmpp.getLoginType().equals(IPVSSession.SMACK_CONNECTION)){
            //this is component login
            //log.addInfo("DO_NOTHING: HA_STATE updateServiceResourceWithHAState isComponentLogin " + userJID);
            return;
        }
        String elementname = "Primary";
        String state = "active";
        String assetResNID_active ="";
        try {
            if (haService.isReplicationEnabled()) {
                state = "standby";
            }  
            if (haService.isSecondary()) {
                elementname = "Secondary";
            } 
            @SuppressWarnings("unused")
			Document result = null;
            MRConfig mrConfig = MRConfig.getInstance(AMConst.SERVICE_RESOURCE, log);
            Element serviceResRootEL = mrConfig.getXmlRepresentation(log).getDoc().getRootElement();            
            //set the <Primary MSAssetResourceNID='' state=''/>
            assetResNID_active = getAssetResoureNIDForDeviceLogin(userJID, log, db, xmpp);
            Element haEL = MRXMLUtils.getElementXpath(serviceResRootEL, "//"+elementname).createCopy();
            MRXMLUtils.setAttributeValue(haEL, "MSAssetResourceNID", assetResNID_active );               
            MRXMLUtils.setAttributeValue(haEL, "state", state );            
            result = udpateServiceResourceInDB(log, db, xmpp, userJID, "//"+elementname, haEL);
            //log.addInfo(" HA_STATE: "+elementname+":result:=" + result + " " + haEL.asXML() );
            //is this correct 
            if(state.equals("active")) {
                Element assetEL = MRXMLUtils.getElement(serviceResRootEL, "AssetResourceNID").createCopy();
                MRXMLUtils.setValue( assetEL , assetResNID_active );
                result = udpateServiceResourceInDB(log, db, xmpp, userJID, "//AssetResourceNID", assetEL);
           		//log.addInfo(" HA_STATE: "+elementname+":result:=" + result + " " + assetEL.asXML() );
            }       
        } catch (Exception e) {
           e.fillInStackTrace();
           log.addInfo(MRLog.stackToString(e));
        }
        log.addInfo("HAUtils:updateServiceResourceWithHAState HA_STATE: updateServiceResourceWithHAState " + userJID +" elementname " + elementname +" " + state +" " + assetResNID_active);
    }
    
    //may be move these functions to relevant places
    private static String getAssetResoureNIDForDeviceLogin(String userJID, MRLog log, MRDB db, XMPPI xmpp) throws Exception{
        MRRequest surequest = MRRequest.createRequest(userJID);
        surequest.setSuperUser(true);
        String resourceName = getDeviceResource(userJID, log, db, xmpp) ;
        return DBServiceUtils.getAssetResourceNID(resourceName, surequest, log, db);
    }
    
    private static String getDeviceResource(String userJID, MRLog log, MRDB db, XMPPI xmpp) throws Exception{
        String xmlFilePath ="/etc/config/LoginRequest.xml";
        Document doc = MRXMLUtils.loadXMLFile(xmlFilePath, log);
        return MRXMLUtils.getValueXpath(doc, "//resource", "").trim();
    }
    
    private static String getDeviceXmppServer(String userJID, MRLog log, MRDB db, XMPPI xmpp) throws Exception{
        String xmlFilePath ="/etc/config/LoginRequest.xml";
        Document doc = MRXMLUtils.loadXMLFile(xmlFilePath, log);
        return MRXMLUtils.getValueXpath(doc, "//server", "").trim();
    }
    
    private static boolean isXMPPServerEnable(MRLog log ) throws Exception{
        Document configDoc = MRXMLUtils.loadXMLFile("/etc/config/Device.xml", log);
        return Utils.booleanValue(MRXMLUtils.getValueXpath(configDoc, "//XMPPServerEnable", "false"), false);
    }
    
    private static boolean isLocalServerIP(String xmppServer, MRLog log) throws Exception {
        if (xmppServer.equals("127.0.0.1") || xmppServer.equals("localhost"))
            return true;
        String ipAddress_login = resolve(xmppServer);
        StringBuffer buf = new StringBuffer();
        String[] cmd = { "/V2O/bin/get_network_ports_status" };
        try {
            SystemServiceUtils.executeProcess(cmd, buf);
        } catch (Exception ex) {
            log.addInfo("HAUtils:isLocalServerIP get_network_ports_status Error " + buf.toString(), MRLog.INFO, MRLog.NOTIFY);
            ex.printStackTrace();
            return false;
        }
        Document doc = MRXMLUtils.stringToDocument(buf.toString());
        String xpath = "//ParametersList/Parameters[Parameter[@name='Primary' and .='True']]/Parameter[@name='IPAddress']";
        Element EL = MRXMLUtils.getElementXpath(doc, xpath);
        if(EL !=null){
            String primaryIP= MRXMLUtils.getValue2(EL, "").trim();
            if(primaryIP.equals(ipAddress_login)){
                return true;
            }
        }
        return false;
    }
    
    private static String resolve(String xmppServer)  throws Exception {
        InetAddress inetAddress = InetAddress.getByName(xmppServer);
        //System.out.println(" " + xmppServer + " resolved to : "+ inetAddress.getHostAddress() + " Host name :" + inetAddress.getHostName());
        return inetAddress.getHostAddress();
    }
    
    public static Element readMSDevice_db(MRRequest request, MRDB db, MRLog log) throws Exception {
        Element el = null;
        //get Object MRConfig
        String objectType = AMConst.MS_DEVICE_OBJECTYPE;
        MRConfig mrConfig = MRConfig.getInstance(objectType, log);
        String listName = mrConfig.getParentNIDKey(log);
        String perentNID = db.getServiceDomainRootNID(mrConfig.getParentNIDKey(log));
        String childXPath = null;
        String[] arrWhereXpath = { "//ServiceElementsConfig/HAServiceConfig/HAServer" };
        String[] sortByXpath = null;
        String selectXML = "<Device>{$VAR//AssetResourceNID, $VAR//HAServer}</Device>";
        String sortOrder = null;
        //do db read
        el = MRXMLDBUtils.readList(MRXMLDBUtils.TIME_OUT, request, log, db, listName, objectType, perentNID, childXPath, arrWhereXpath, sortByXpath, sortOrder, selectXML, null, 1, -1);
        if (el != null) {
            Document doc = MRXMLUtils.getDocument();
            MRXMLUtils.addElement(doc, el);
        }
        return el;
    }
    
    public static void updateServiceResourceWithHA(Document requestDoc, String userJID, MRDB db, MRLog log)  throws MRException,Exception {
        try{
            Element haEL = MRXMLUtils.getElementXpath(requestDoc, "//HA"); 
            if (haEL != null) {
                haEL.detach();
            }
            MRConfig mrconfig = MRConfig.getInstance(AMConst.SERVICE_RESOURCE, log);
            Element haEL_object = MRXMLUtils.getElementXpath(mrconfig.getXmlRepresentation(log).getDoc(),"//HA");
            Element msAssetElement = MRXMLUtils.getElementXpath(requestDoc, "//ServiceResource");
            MRXMLUtils.addElement(msAssetElement, haEL_object.createCopy());
        }catch (Exception ex) {
            log.addInfo("HAUtils:updateServiceResourceWithHA " + MRLog.stackToString(ex));
            throw ex;
        }
        return;
    }    
    
    private static Document  udpateServiceResourceInDB(MRLog log, MRDB db, XMPPI xmpp, String userJID , String childXpath, Element el)  throws MRException,Exception {
        String[] arrWhereXpath = {"//ServiceResource/Info[Type='" + AMConst.APP_SERVICE_TYPE+ "']"};
        MRRequest surequest = MRRequest.createRequest(userJID);
        surequest.setSuperUser(true);
        MRConfig mrConfig = MRConfig.getInstance(AMConst.SERVICE_RESOURCE, log);
        String listName = mrConfig.getParentNIDKey(log);
        String perentNID = db.getServiceDomainRootNID(mrConfig.getParentNIDKey(log));
        Element serviceResRootEL = mrConfig.getXmlRepresentation(log).getDoc().getRootElement();
        MRXMLUtils.elementToDocument(serviceResRootEL);
        return MRXMLDBUtils.replace(MRXMLDBUtils.TIME_OUT, surequest, log, db, listName, AMConst.SERVICE_RESOURCE, perentNID,
                childXpath , arrWhereXpath, el.createCopy()); 
    }
    
      
	public static void main(String[] args) throws Exception {
		//HAService service = new HAService(null);
		//System.out.println(service.dbRecreatePending());
	    try {
	        String xmppServer = "ms.ipvs.example.com";//"10.1.5.179";//"ms.ipvs.example.com";//"127.0.0.1";
	        String ipAddress = resolve(xmppServer);
	        System.out.println(" " + xmppServer + " resolved to : "+ ipAddress);
        } catch (Exception e) {
            e.printStackTrace();
        }
        System.exit(0);
	}
	public static void sendPrimaryConfigUpdateEvent(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory, String deviceJID, String userJID, String assetResourceNID, Element serviceResourceElement, String responseHandlerName, Element dataEl, String xpath) throws Exception{
		if(!assetResourceNID.isEmpty()){
		  
		    String activeNID= getMsAssetResourceNID("active",serviceResourceElement,log);
		    if(!activeNID.isEmpty()) {
			if(deviceJID != null) {		   
			    String xmlFilePath = "/com/ipvs/deviceclient/agent/deviceadmin/internal/xml/ActiveDeviceConfigUpdateRequest.xml";        
			    Element activeDeviceConfigRequestEl = MRXMLUtils.getElementXpath(MRXMLUtils.loadXMLResource(xmlFilePath, log),"//ActiveDeviceConfigUpdate");
			    MRXMLUtils.addAttribute(activeDeviceConfigRequestEl, "xpath", xpath);
			    MRXMLUtils.addChildElement(activeDeviceConfigRequestEl, dataEl);
			    AppServerAgent.sendAgentRequestForUserRequest(request, xmpp, log, db, resourceFactory, serviceAgentFactory, AppServerAgent.class.getName(),
				    deviceJID, "ActiveDeviceConfigUpdateRequest", MRConst.DEVICE_ADMIN_SERVICE, activeDeviceConfigRequestEl.asXML(), responseHandlerName);	        		
			}


		    }
		    else
			log.addInfo("AssetAdminServiceUtils.sendPrimaryConfigUpdateEvent:Not sending config updates. Active device is not ready yet");
		}
		
	    }
	    	public static String getMsAssetResourceNID(String state, Element serviceResourceElement,MRLog log) throws Exception{
		    if(serviceResourceElement != null){		
			Element haInfoEL = MRXMLUtils.getElementXpath(serviceResourceElement, ".//HA");		
			String activeResourceNID = MRXMLUtils.getAttributeValueXpath(haInfoEL, "./*[@state='"+state+"']", "MSAssetResourceNID", "");		
				return activeResourceNID;		
		    }
		    else
			return "";
		}
	    	public static String getDeviceJIDFromAssetResourceNID(MRRequest request, MRLog log, MRDB db,  String assetResourceNID) throws Exception{
	    	    Element resourceEL = MRXMLDBUtils.readElement(request, log, db, assetResourceNID).getRootElement();
	    	    if(resourceEL != null) {    		
	    		return MRXMLUtils.getValueXpath(resourceEL, ".//"+AMConst.OWNERUSERJID, "");
	    	    }
	    	    return null;

	    	}
	    	public static Element getServiceResourceElement(MRDB db, MRRequest request, MRLog log) throws Exception{
	    	    	MRConfig config = MRConfig.getInstance(ServiceClient.ASSETADMINSERVICE, AMConst.SERVICE_RESOURCE, log);
			String rootNID = db.getServiceDomainRootNID(config.getParentNIDKey(log));
			String whereXPath[] = {"//ServiceResource/Info[Type='" + AMConst.APP_SERVICE_TYPE+ "']"};
			return MRXMLDBUtils.readElement(1, null, whereXPath, rootNID, 1, null, null, request, db, log);	
	    	}
		
}
