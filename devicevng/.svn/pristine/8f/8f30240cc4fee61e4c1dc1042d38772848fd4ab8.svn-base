package com.barco.device.vng.DeviceAdmin;

import java.io.File;
import java.io.FileInputStream;
import java.io.InputStream;
import java.util.Arrays;
import java.util.Properties;

import org.apache.log4j.Logger;
import org.dom4j.Document;

import com.barco.device.base.ResponseHandlerI;
import com.barco.device.common.DeviceFrameworkI;
import com.barco.device.common.Message;
import com.barco.device.vng.DeviceAdmin.HAService.ReplicationStatus;
import com.barco.device.vng.utils.CommandLineUtils;
import com.barco.utils.FileUtils;
import com.barco.utils.Utils;
import com.barco.utils.XMLUtils;

class HAService{

	private  Properties haConfProperties = null;
	public enum ReplicationStatus {NotStarted, InProgress, Done};
	
	private ReplicationStatus replicationStatus = null;
	
	public HAService(){
		this.setReplicationStatus(ReplicationStatus.NotStarted);
		try {
			this.setHaConfProperties(HAUtils.loadProperties(HAUtils.HA_CONF_FILE_PATH));    
			//log.info("HAUtils:HAService HA Active");
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

	public boolean isReplicationEnabled(){
		String replicationStr =  haConfProperties.getProperty(HAUtils.HA_CONF_PROPERTY_REPLICATE);
		return Utils.getBooleanValue(replicationStr, false);
	}

	public boolean isSecondary(){
		String secondaryStr =  haConfProperties.getProperty(HAUtils.HA_CONF_PROPERTY_SECONDARY);
		return Utils.getBooleanValue(secondaryStr, false);
	}
	public String getDBDir(){
		return haConfProperties.getProperty(HAUtils.HA_CONF_PROPERTY_DBDIR);    
	}
	
	public boolean dbRecreatePending() {
		String[] cmd = {"/etc/scripts/ipvsxmldbctl.sh", "check_createdbScheduled"};
		try {
			StringBuffer sb = new StringBuffer();
			int retVal = CommandLineUtils.executeSystemCommand(cmd, sb);
			return (retVal == 0);
		}
		catch(Exception e) {
			e.printStackTrace();
			return false;
		}
	}

	ReplicationStatus getReplicationStatus() {
		return replicationStatus;
	}

	void setReplicationStatus(ReplicationStatus replicationStatus) {
		this.replicationStatus = replicationStatus;
	}

}

public class HAUtils {

	private static HAService haService = null;
	
	//HA Constants
	private static final long DBWAIT = 200;
    public static final String HA_CONF_FILE_PATH = "/usr/local/exist/deviceapp.ha.conf";
    public static final String HA_CONF_PROPERTY_REPLICATE = "REPLICATE";
    public static final String HA_CONF_PROPERTY_SECONDARY = "SECONDARY";
    public static final String HA_CONF_PROPERTY_DBDIR = "DBDIR";
    public static final String DOWNLOAD_DIR_PATH = "/data/download/";
    public static final String DOWNLOAD_DB_REQUEST_XML_PATH = "/com/ipvs/systemservice/xml/DownloadDBRequest.xml";
    public static final String UPGARDE_REQUEST_XML_PATH = "/com/ipvs/systemservice/xml/UpgradeRequest.xml";
    public static final String DOWNLOAD_DIR_EL_NAME = "DownloadDirPath"; 
    public static final String DOWNLOAD_TGZ_FILE_NAME = "download_db.tgz";
    public static final String RECORDING_THUMBNAIL_CACHE = "/data/tncache/recordings";
	
	public static void updateHAProperties(){
		haService = new HAService();		
	}
	
	public static boolean isHAServiceEnabled() {

		if (haService == null || haService.getHaConfProperties() == null) {
			return false;
		}
		return true;
	}

	public static boolean isServerInStandby() {
		if (!isHAServiceEnabled()) {
			return false;
		}
		return haService.isReplicationEnabled();
	}
	
	public static ReplicationStatus getReplicationStatus() {
		if (!isHAServiceEnabled()) {
			return ReplicationStatus.NotStarted;
		}
		return haService.getReplicationStatus();
	}

	public static void setReplicationStatus(ReplicationStatus replicationStatus) {
		if (!isHAServiceEnabled()) 
			return;
		haService.setReplicationStatus(replicationStatus);
	}
	
	// send the download request to primary
	public static void sendDownloadDBRequest(DeviceFrameworkI deviceFramework, ResponseHandlerI responseHandler) throws Exception {

		Logger log = deviceFramework.getLogger();
		log.info("HAUtils:sendDownloadDBRequest stop replication before sending download request");
		stopReplication(log);

		log.info("HAUtils:sendDownloadDBRequest wait before Send download request to primary="+ DBWAIT);
		Thread.sleep(DBWAIT);

		log.info("HAUtils:sendDownloadDBRequest Send download request to primary");
		String archiveFilename = HAUtils.DOWNLOAD_DIR_PATH + "/" + HAUtils.DOWNLOAD_TGZ_FILE_NAME;
		if (FileUtils.isFileExist(HAUtils.DOWNLOAD_DIR_PATH, HAUtils.DOWNLOAD_TGZ_FILE_NAME))
			FileUtils.removeFileOrDir(archiveFilename);
		
		FileUtils.createDirIfNotExist(HAUtils.DOWNLOAD_DIR_PATH);
		Document downloadDBRequestDoc = XMLUtils.loadXMLResource(HAUtils.DOWNLOAD_DB_REQUEST_XML_PATH);
		XMLUtils.setValueXpath(downloadDBRequestDoc, "//"	+ HAUtils.DOWNLOAD_DIR_EL_NAME,	HAUtils.DOWNLOAD_DIR_PATH + "/"	+ HAUtils.DOWNLOAD_TGZ_FILE_NAME);
		XMLUtils.setValueXpath(downloadDBRequestDoc, "//"	+ "ClientData", "");
	
		Message downloadDbRequestMessage =  Message.createRequest(downloadDBRequestDoc, deviceFramework.getAppserverJID() , deviceFramework.getMyJID());
		deviceFramework.sendRequest(downloadDbRequestMessage, downloadDBRequestDoc, responseHandler);
	}
		
	// stop the replication process
	private static void stopReplication(Logger log) throws Exception {
		String[] systemCmd = { "sh", "/etc/scripts/ipvsxmldbctl.sh", "stopreplication" };
		int commandResult = CommandLineUtils.executeSystemCommand(systemCmd);
		handleCommandResult(commandResult, "StopReplicationFailed", log);
		log.info("HAUtils:stopReplication Stop Replication Process");
	}
	
	// start the replication process
	public static void startReplication(Logger log) throws Exception {
		String[] systemCmd = { "sh", "/etc/scripts/ipvsxmldbctl.sh",
				"startreplication", HAUtils.DOWNLOAD_DIR_PATH,
				HAUtils.DOWNLOAD_TGZ_FILE_NAME };
		int commandResult = CommandLineUtils.executeSystemCommand(systemCmd);
		handleCommandResult(commandResult, "StartReplicationFailed", log);
		log.info("HAUtils:startReplication Start Replication Process");
	}

	private static void handleCommandResult(int commandResult, String errorMessage, Logger log) throws Exception {
		if (commandResult != 0) {
			log.info("HAUtils:" + errorMessage);
			throw new Exception(errorMessage);
		}
	}
	
	public static Properties loadProperties(String propFile) throws Exception {
		return loadPropertiesFile(new Properties(), propFile);
	}
	
	public static Properties loadPropertiesFile(Properties props,
			String propFile) throws Exception {
		InputStream in = null;

		try {
			in = new FileInputStream(new File(propFile));
			props.load(in);
			props.setProperty("props.inited", "true");

			return props;
		} catch (Exception ex) {
			// ex.printStackTrace();
			throw new Exception("Properties not found or invalid." + propFile);
		} finally {
			if (in != null) {
				try {
					in.close();
				} catch (Exception ex) {
				}
			}

			in = null;
		}
	}
	
	public static void invalidateFileThumbnailCache(Logger log) {
		
		String[] systemCmd = new String[3];
		systemCmd[0] = "rm";
		systemCmd[1] = "-rf";
		systemCmd[2] = HAUtils.RECORDING_THUMBNAIL_CACHE;

		log.info("Invalidate File thumbnail cache command: "+ Arrays.toString(systemCmd));
		int cmdResult;
		try {
			cmdResult = CommandLineUtils.executeSystemCommand(systemCmd);
			if (cmdResult != 0) {
				log.info("Failed to invalidate thumbnail cache");
				return;
			}
			log.info("Successfully invalidated thumbnail cache");
			File tnDir = new File(HAUtils.RECORDING_THUMBNAIL_CACHE);
			if(!tnDir.exists())
				tnDir.mkdirs();
		} catch (Exception e) {
			log.info("Failed to invalidate thumbnail cache");
			e.printStackTrace();
		}
	}
}
