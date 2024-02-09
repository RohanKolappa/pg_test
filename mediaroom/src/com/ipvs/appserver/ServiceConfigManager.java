package com.ipvs.appserver;

import java.util.List;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.assetadminservice.handler.AMConst;
import com.ipvs.client.ServiceClient;
import com.ipvs.common.AsynchDBRequestResponseListenerI;
import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPF;
import com.ipvs.common.db.MRConfig;
import com.ipvs.common.db.MRXMLAsynchDBUtils;
import com.ipvs.common.db.MRXMLDBUtils;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.mediaroomservice.impl.StreamPolicyUtils;
import com.ipvs.mediaroomservice.streamingserver.MCastIPAddressReservation;
import com.ipvs.utils.Utils;

/**
 * This class maintains the service configuration information of System Controller. 
 * Currently this class includes multicast ipaddress pool.Allocating and releasing multicast ipaddress management will handle by this class
 * This class object is registered as observer on RoomDB. If Room or stream deleted we will handle the update and release the multicast ipaddress used in room/stream.
 *
 * In this class, Eventually we will add more information related to service configuration.
 * 
 * @author NVAT
 *
 */
public class ServiceConfigManager {


	MRLog log = null;
	XMPPF xf = null;
	String userJID = "";
	String serviceConfigNID = "";

	Element serviceConfigElement = null;
	MRDB db=null;

	ServiceConfigNetworkResources serviceConfigNetworkResource = null;

	public ServiceConfigManager (MRLog log,XMPPF xf, MRDB db,String userJID) {
		this.log = log;
		this.xf=xf;
		this.userJID=userJID;	
		this.db=db;
	}

	public void init() {
		try {

			//ReadServiceConfig
			MRRequest surequest = MRRequest.createRequest(userJID);   
			MRConfig config = MRConfig.getInstance(ServiceClient.ASSETADMINSERVICE, AMConst.SERVICE_CONFIG, log);
			String rootNID = db.getServiceDomainRootNID(config.getParentNIDKey(log));
			String whereXPath[] = {};
			try {
				serviceConfigElement = MRXMLDBUtils.readElement(1, null, whereXPath, rootNID, 1, null, null, surequest, db, log);
			} catch (Exception e) {
				log.addInfo("ServiceConfig not exist",MRLog.INFO,MRLog.NOTIFY);
			}
			this.serviceConfigNID = MRXMLUtils.getAttributeValue(serviceConfigElement, AMConst.NID,"");		

			//add service configuration in database
			if(serviceConfigNID.isEmpty()) {
				Document objectXML = MRConfig.getObjectXMLDocument(ServiceClient.ASSETADMINSERVICE, AMConst.SERVICE_CONFIG, log);
				MRXMLUtils.getElementXpath(objectXML, "//UsedIPAddressList").clearContent();

				//set hls enable true based on if any profile has tru

				String[] xpathArr = { "//RTPMediaStreamProfile[.//HLSConfig/Enable='true']" };
				Document responseDoc = MRXMLUtils.getDocument();
				responseDoc.add(MRXMLUtils.newElement("ResponseDoc"));   	 
				MRXMLDBUtils.readList(MRXMLDBUtils.TIME_OUT,null,log,db, "", AMConst.RTP_MEDIA_STREAM_PROFILE_OBJECTTYPE, "","",xpathArr, null,"", "", 
						responseDoc.getRootElement(), 0,-1);

				String hlsEnable = "false";
				List<?> hlsEnabledProfiles = MRXMLUtils.getListXpath(responseDoc, "//RTPMediaStreamProfile");
				if(hlsEnabledProfiles!=null && !hlsEnabledProfiles.isEmpty()) {
					hlsEnable = "true";
				}

				MRXMLUtils.setAttributeValueXPath(objectXML, "//WriterOptions", "enableHLS", hlsEnable);
				serviceConfigElement = objectXML.getRootElement();
				MRXMLDBUtils.add(MRXMLDBUtils.TIME_OUT, MRRequest.createRequest(userJID), log, db, "", AMConst.SERVICE_CONFIG, "", "", null,serviceConfigElement);
				this.serviceConfigNID = MRXMLUtils.getAttributeValue(serviceConfigElement, AMConst.NID,"");					
			}

			//initialize network resources 
			serviceConfigNetworkResource = new ServiceConfigNetworkResources();

			//TBD: max media clients

		} catch (MRException e) {
			e.printStackTrace();
		} catch (Exception e) {
			e.printStackTrace();
		}	
	}

	public String getServiceConfigNID() {
		return serviceConfigNID;
	}


	public Element getServiceConfigElement() {
		return serviceConfigElement;
	}

	/**
	 * Update multicast ipaddress pool information in service config. Creating new object of serviceconfignetworkresource will take care of resetting the pool.
	 * @param multicastIPAddressPoolEL
	 * @throws Exception
	 */
	public void resetServiceConfigNetworkResources(Element multicastIPAddressPoolEL) throws Exception {
		Element localNetworkEl = MRXMLUtils.getElementXpath(serviceConfigElement, ".//MulticastIPAddressPool");
		localNetworkEl.detach();
		MRXMLUtils.addElement(serviceConfigElement, multicastIPAddressPoolEL);

		//reset network resource object
		this.serviceConfigNetworkResource = null;
		this.serviceConfigNetworkResource = new ServiceConfigNetworkResources();		
	}

	public ServiceConfigNetworkResources getServiceConfigNetworkResources(){
		return this.serviceConfigNetworkResource;
	}

	public void updateServiceConfigWriterOptions(Element writerOptionsEL) throws Exception {
		Element writerOptionsLocal = MRXMLUtils.getElementXpath(serviceConfigElement, ".//WriterOptions");
		writerOptionsLocal.detach();
		log.addInfo("ServiceConfigManager: Update HLS Writer options "+ writerOptionsEL.asXML(),MRLog.INFO,MRLog.NOTIFY);
		MRXMLUtils.addElement(serviceConfigElement, (Element)writerOptionsEL.clone());
		StreamPolicyUtils.handlePolicyChanged();		
	}

	public boolean isHLSEnabled() throws Exception {
		Element writerOptionsLocal = MRXMLUtils.getElementXpath(serviceConfigElement, ".//WriterOptions");
		return Utils.getBooleanValue(MRXMLUtils.getAttributeValue(writerOptionsLocal, "enableHLS","false"), false);

	}

	public class ServiceConfigNetworkResources  implements AsynchDBRequestResponseListenerI{				

		MCastIPAddressReservation ipaddressReservation=null;
		boolean multicastPoolEnabled = false;
		/**
		 * Initialize the multicast pool if pool is enabled in configuration
		 */
		private ServiceConfigNetworkResources() {

			try {
				//check multicast pool is enabled
				Element multicastPoolEl = MRXMLUtils.getElementXpath(serviceConfigElement, "./MulticastIPAddressPool");
				multicastPoolEnabled = Utils.getBooleanValue(MRXMLUtils.getAttributeValue(multicastPoolEl, "enable", "false"),false);
				if(!multicastPoolEnabled){
					log.addInfo("ServiceConfigManager: Multicast Pool not configured",MRLog.INFO,MRLog.NOTIFY);
					return;
				}

				//initialize multicast cache
				String rangeStart = MRXMLUtils.getAttributeValue(multicastPoolEl, "start","226.1.1.1");
				String rangeEnd = MRXMLUtils.getAttributeValue(multicastPoolEl, "end","226.1.1.255");			
				ipaddressReservation = new MCastIPAddressReservation();
				ipaddressReservation.init(rangeStart, rangeEnd, "1");
				log.addInfo("ServiceConfigManager: Multicast Pool  configured. RangeStart:= "+rangeStart + " RangeEnd:="+rangeEnd,MRLog.INFO,MRLog.NOTIFY);

				//update used pool information in cache
				//Stream GC will take care of releasing ipaddress when GC's the streams if destination is not connected.
				//SC factory default no used pool information available in database.GC thread is taking care of deleting streams
				List<?> usedPoolList = MRXMLUtils.getListXpath(serviceConfigElement, ".//UsedIPAddress");

				if(usedPoolList==null || usedPoolList.isEmpty()) {
					return;	
				}

				boolean updateDatabase= false;				
				for(Object object:usedPoolList) {
					Element element =(Element) object;
					List<String> outputKeyValues= MRXMLUtils.getValuesXpath(element, "./OutputKey");

					if(outputKeyValues==null || outputKeyValues.isEmpty()){
						element.detach();
						updateDatabase = true;
					}
					String inputKey = MRXMLUtils.getAttributeValue(element, "inputKey");
					String ipAddress = MRXMLUtils.getAttributeValue(element, "ipAddress");					
					for(String outputKey:outputKeyValues){
						ipaddressReservation.updateReservationForReliability(inputKey, outputKey, ipAddress);		
						log.addInfo("ServiceConfigManager: Add used ipaddress information multicastIP:="+ipAddress +" inputKey:="+inputKey+" outputKey:="+outputKey,MRLog.INFO,MRLog.NOTIFY);

					}
				}
				if(updateDatabase==true){
					MRXMLDBUtils.updateElement(MRRequest.createRequest(userJID), log, db, serviceConfigNID, serviceConfigElement);
				}

			} catch (Exception e) {
				e.printStackTrace();
			}
		}

		private boolean isMulticastPoolEnabled() {
			return multicastPoolEnabled;
		}


		/**
		 * Returns Multicast ipaddress if given profile specify to do multicast and pool is available based on configuration.
		 * After allocating ipaddress update database with used pool information. Updating database is done in asynchronous way 
		 * @param streamNID
		 * @param sourceNID
		 * @param relayNID
		 * @param profileXML
		 * @return Multicast ipaddress 
		 * @throws MRException if no available ipaddress
		 * @throws Exception
		 */

		public String reserveMulticastIPAddress(String streamNID, String sourceNID, String relayNID,Document profileXML) throws MRException,Exception{

			if(!isMulticastPoolEnabled() || !Utils.getBoolean(MRXMLUtils.getValueXpath(profileXML, "//IsMulticast", "false"), false)) {				
				log.addInfo("ServiceConfigManager:reserveMulticastIPAddress Multicast is not enabled for streamNID:="+streamNID +" sourceNID:="
						+sourceNID+" relayNID:="+relayNID,MRLog.INFO,MRLog.NOTIFY);				
				return "";
			}

			String inputKey = sourceNID;

			//if the stream going through relay add input key as source + relay information. 
			if(!relayNID.isEmpty()){
				inputKey +=":"+relayNID;
			}

			String outputKey = streamNID;
			String multicastIpAddress = "";
			synchronized (ipaddressReservation) {
				multicastIpAddress = this.ipaddressReservation.reserve(outputKey, inputKey);
			}

			log.addInfo("ServiceConfigManager:reserveMulticastIPAddress Reserve Multicast IPAddress :="+multicastIpAddress +" for streamNID:="+streamNID +" sourceNID:="
					+sourceNID+" relayNID:="+relayNID, MRLog.INFO,MRLog.NOTIFY);

			//update used ipaddress in Database
			String childXPath = "//UsedIPAddress[@ipAddress='"+multicastIpAddress+"' and @inputKey='"+inputKey+"']";
			Element updateEl = MRXMLUtils.addElement("OutputKey", outputKey); 
			Element usedIPAddressEL = MRXMLUtils.getElementXpath(serviceConfigElement, childXPath);
			if(usedIPAddressEL == null) {
				updateEl = MRXMLUtils.newElement("UsedIPAddress");
				MRXMLUtils.addAttribute(updateEl, "ipAddress", multicastIpAddress);
				MRXMLUtils.addAttribute(updateEl, "inputKey", inputKey);	
				MRXMLUtils.addElement(updateEl, "OutputKey", outputKey);
				childXPath ="//UsedIPAddressList";				
			} 

			//update in local cache
			MRXMLUtils.addElement(MRXMLUtils.getElementXpath(serviceConfigElement,"."+ childXPath), updateEl);

			MRXMLAsynchDBUtils.getInstance().addAsynch(MRXMLDBUtils.TIME_OUT,  MRRequest.createRequest(userJID), log, db, 
					"", AMConst.SERVICE_CONFIG, serviceConfigNID, childXPath, new String[]{}, updateEl, null, this, xf.sf(userJID));

			return multicastIpAddress;
		}

		/**
		 * Release IPAddress if there is no references associate to input.
		 * @param streamNID
		 * @param sourceNID
		 * @param relayNID
		 * @param multicastIpAddress
		 * @return void
		 * @throws MRException
		 * @throws Exception
		 */
		public void releaseMulticastIPAddress(String streamNID, String sourceNID, String relayNID,String multicastIpAddress) throws MRException,Exception{

			if(!isMulticastPoolEnabled() || multicastIpAddress.isEmpty()) {				
				log.addInfo("ServiceConfigManager:releaseMulticastIPAddress Multicast is not enabled for streamNID:="+streamNID +" sourceNID:="
						+sourceNID+" relayNID:="+relayNID,MRLog.INFO,MRLog.NOTIFY);				
				return;
			}	

			String outputKey = streamNID;
			synchronized (ipaddressReservation) {
				this.ipaddressReservation.release(outputKey);
			}
			String inputKey = sourceNID;

			//if the stream going through relay add input key as source + relay information. 
			if(!relayNID.isEmpty()){
				inputKey +=":"+relayNID;
			}
			String childXpath ="//UsedIPAddress[@ipAddress='"+multicastIpAddress+"' and @inputKey='"+inputKey+"']";
			Element usedIPAddress = MRXMLUtils.getElementXpath(serviceConfigElement, "."+childXpath);			
			//remove entry from database
			Element outputKeyEl =MRXMLUtils.getElementXpath(usedIPAddress, ".//OutputKey[.='"+outputKey+"']");				

			if(outputKeyEl!=null){			    
				outputKeyEl.detach();				
			}
			if(MRXMLUtils.getChildren(usedIPAddress).size() == 0) {
				//remove used element			
				usedIPAddress.detach();
			} else {			
				childXpath = "//UsedIPAddress[@inputKey='"+inputKey+"']/OutputKey[.='"+outputKey+"']";		 

			}		 	

			//delete entry from database
			MRXMLAsynchDBUtils.getInstance().deleteAsynch(MRXMLDBUtils.TIME_OUT,  MRRequest.createRequest(userJID), log, db, 
					"", AMConst.SERVICE_CONFIG, serviceConfigNID, childXpath, new String[]{},  null, this, xf.sf(userJID));


			log.addInfo("ServiceConfigManager:releaseMulticastIPAddress Release Multicast IPAddress :="+multicastIpAddress +" for streamNID:="+streamNID +" sourceNID:="
					+sourceNID+" relayNID:="+relayNID, MRLog.INFO,MRLog.NOTIFY);

		}

		@Override
		public void handleAsynchDBRequestResponse(String dbAction,
				Element responseElement, Object callBackDataObject, int status,
				String logData, ServiceAgentFactoryI serviceAgentFactory)
						throws Exception {
			if(status==MRRequest.STATE_EXCEPTION){
				log.addInfo("ServiceConfigManager:AsyndbREquestFailed. LogData:="+logData,MRLog.INFO,MRLog.NOTIFY);
			}
		}
	}	

}




