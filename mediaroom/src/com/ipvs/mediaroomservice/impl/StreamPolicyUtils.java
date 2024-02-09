package com.ipvs.mediaroomservice.impl;

import java.util.Hashtable;
import java.util.List;
import java.util.StringTokenizer;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.assetadminservice.handler.AMConst;
import com.ipvs.assetadminservice.impl.AssetAdminServiceUtils;
import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.MRRequestCache;
import com.ipvs.common.XMPPI;
import com.ipvs.common.db.MRXMLDBUtils;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRRequestHandlerI;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.mediaroomservice.handler.GetRelayListRequestHandler;
import com.ipvs.mediaroomservice.streamingserver.SSAPI;
import com.ipvs.mediaroomservice.streamingserver.StreamingServerImpl;
import com.ipvs.policyadminservice.impl.GetMatchingPolicyLib;
import com.ipvs.policyadminservice.impl.PolicyManager;
import com.ipvs.policyadminservice.utils.PolicyAdminConstants;
import com.ipvs.systemservice.impl.HAUtils;
import com.ipvs.utils.Utils;

public class StreamPolicyUtils {

    private static Hashtable<String, Document> theRelayTable = new Hashtable<String, Document>();
    private static final int MAXCACHESIZE = 100;
 
	public static Element getOperatorGroups(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, 
			MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception {	
		String userJID = request.getUserJID();
	    String operatorGroupsKey = userJID + "." + "OperatorGroups";
	    MRRequestCache cache = request.getCache();
	    if(!cache.containsKey(operatorGroupsKey)) {
			Element operatorGroups = MediaRoomServiceUtils.getOperator(request, xmpp, log, db, resourceFactory, serviceAgentFactory);
		    cache.put(operatorGroupsKey, operatorGroups);
		    // System.out.println("StreamPolicyUtils:getOperatorGroups added key=" + operatorGroupsKey + " request#=" + request.hashCode() + " cache#=" + cache.hashCode());
	    }
	    return (Element)cache.get(operatorGroupsKey);
	}

	private static Document getDeviceDoc(MRRequest request, XMPPI xmpp, MRLog log,
			MRDB db, String assetResourceNID) throws Exception {
	    String deviceDocKey = assetResourceNID + "." + "DeviceDoc";
	    MRRequestCache cache = request.getCache();
	    if(!cache.containsKey(deviceDocKey)) {
			Document deviceDoc = AssetAdminServiceUtils.getDeviceIPAddressInformation(assetResourceNID, request, db, log, xmpp);
			if(deviceDoc == null)
				throw new MRException (MediaRoomServiceExceptionCodes.DEVICE_XML_NOT_FOUND, "No device xml found");
				//ServiceAgentUtils.getDeviceDoc(assetResourceNID, request.getUserJID(), db, log);
		    cache.put(deviceDocKey, deviceDoc);
		    // System.out.println("StreamPolicyUtils:getDeviceDoc added key=" + deviceDocKey + " request#=" + request.hashCode() + " cache#=" + cache.hashCode());
	    }
	    return (Document)cache.get(deviceDocKey);
	}

    public static boolean isAuthPolicyEnabled(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory, 
    		String dstResourceNID,String srcResourceNID) throws Exception {
    	
        Document policyResponse = GetMatchingPolicyLib.getMatchingPolicy(dstResourceNID, srcResourceNID, PolicyAdminConstants.VIEW_ROLE_ACTION, request, xmpp, log, db, resourceFactory, serviceAgentFactory);

        String xAuthPolicy = MRXMLUtils.getValueXpath(policyResponse, "//RuleKey", "").trim();

        if ("".equals(xAuthPolicy) || "false".equals(xAuthPolicy)) { // Some value other than false/empt
        	return false;
        }
        
        return true;
    } 
    /*<NetworkAddressScheme>
    <UseDNSName>false</UseDNSName>
    <UseLANIP>false</UseLANIP>
    <EthernetPortEntryID/>
    <UseNATZoneIP>true</UseNATZoneIP>
    <NATZoneEntryName>GLOBAL</NATZoneEntryName>
   </NetworkAddressScheme>*/
    
    private static String getIPAddressBasedOnURLScheme(Element networkURLRule, Document deviceDoc) throws Exception {
    	
	 	
	 	boolean useDNSName = false;
	 	boolean useLANIP = false;
	 	boolean useNATZoneIP = false;
	 	String ethernetPortEntryID = "1";
	 	String natZoneEntryName = null;
	 	Element ethernetPortConfigEL = null;
	 	String ipaddress = "";
	 	
		boolean isXMPPServerWithHA = isXMMPServerWithHAWithVirtualIPSetup(deviceDoc);		
	 	if(networkURLRule == null) {
	 		
	 		//List<?>ethernetPortConfigList = MRXMLUtils.getListXpath(deviceDoc, "//EthernetPortStatus/.//Parameters/Parameter[@name = 'Primary' and . = 'True']/../Parameter[@name='IPAddress']");
	 		List<?>ethernetPortConfigList = MRXMLUtils.getListXpath(deviceDoc, "//EthernetPortStatus/Parameters/Parameter[@name = 'Primary' and . = 'True']/../Parameter[@name='IPAddress']");
	 		/*if(ethernetPortConfigList == null || ethernetPortConfigList.size()<=0 ) {
	 			ethernetPortConfigList = MRXMLUtils.getListXpath(deviceDoc, "//EthernetPortConfig[.//IsPrimary='" + "true" + "'" + "and .//EnableInterface='" + "true" +  "']//IPAddress");
	 		}*/	
	 		if(ethernetPortConfigList == null || ethernetPortConfigList.size()<=0 ) {	 			
	 			ethernetPortConfigList = MRXMLUtils.getListXpath(deviceDoc,".//Parameters[Parameter[@name = 'Primary' and lower-case(.) = 'true'] and Parameter[@name = 'Status' and lower-case(.) = 'true']]/./Parameter[@name='IPAddress']");
	 			
	 		} 
	 		if(ethernetPortConfigList == null || ethernetPortConfigList.size()<=0 ){
 				throw new MRException (MediaRoomServiceExceptionCodes.ERROR_GETTING_PUBLISHED_IP, "No Primary Enabled interface found");
 			}
	 		else if (!isXMPPServerWithHA && ethernetPortConfigList.size() > 1) {
	 		
				throw new MRException (MediaRoomServiceExceptionCodes.ERROR_GETTING_PUBLISHED_IP, "Multiple Primary Enabled interfaces found");
	 		} else {
				ethernetPortConfigEL = getEthernetPortElement(deviceDoc,ethernetPortConfigList,isXMPPServerWithHA); 
			
				ipaddress = ethernetPortConfigEL.getText().trim();
   				if(ipaddress == null || "".equals(ipaddress.trim())) {
   					throw new MRException (MediaRoomServiceExceptionCodes.ERROR_GETTING_PUBLISHED_IP, "No Primary Enabled interface found");
   				}	
   				
   				if(!Utils.validateIPAddress(ipaddress)){
   				    throw new MRException (MediaRoomServiceExceptionCodes.INVALID_IP_ADDRESS, "Invalid IPAdress :="+ipaddress);
   		   		}
   				
   				return ipaddress;
   			}
 	 	}
 	 	Element urlScheme = MRXMLUtils.getElementXpath(MRXMLUtils.elementToDocument(networkURLRule), "//NetworkAddressScheme");
	 	//System.out.println(MRXMLUtils.elementToString(urlScheme));
	 	
	 	if(MRXMLUtils.getValueXpath(MRXMLUtils.elementToDocument(urlScheme), "//UseDNSName","").equalsIgnoreCase("true")) {
	 		useDNSName = true;
	 	} else  if (MRXMLUtils.getValueXpath(MRXMLUtils.elementToDocument(urlScheme), "//UseLANIP","").equalsIgnoreCase("true")) {
	 		useLANIP = true;
	 		ethernetPortEntryID = MRXMLUtils.getValueXpath(MRXMLUtils.elementToDocument(urlScheme), "//EthernetPortEntryID","");
	 	} else  if (MRXMLUtils.getValueXpath(MRXMLUtils.elementToDocument(urlScheme), "//UseNATZoneIP","").equalsIgnoreCase("true")) {
	 		useNATZoneIP = true;
	 		natZoneEntryName = MRXMLUtils.getValueXpath(MRXMLUtils.elementToDocument(urlScheme), "//NATZoneEntryName","");
	 	}
	 	
	 	if(useDNSName) {
	 		String hostName = MRXMLUtils.getValueXpath(deviceDoc, "//DNSClientConfig/HostName", ipaddress);
			String dnsDomain = MRXMLUtils.getValueXpath(deviceDoc, "//DNSClientConfig/DNSDomain", "");
			ipaddress = hostName + "." + dnsDomain;
			if(hostName.isEmpty() || dnsDomain.isEmpty()) {
				throw new MRException (MediaRoomServiceExceptionCodes.ERROR_GETTING_PUBLISHED_IP, "hostName = " + hostName + ",dnsDomain = " + dnsDomain);
			}
			
	 	} else if(useLANIP) {
	 		ethernetPortConfigEL = getEthernetPortElementBasedOnLAN(deviceDoc, isXMPPServerWithHA, ethernetPortEntryID);
	 				
	 				//MRXMLUtils.getElementXpath(deviceDoc, "//EthernetPortStatus/Parameters/Parameter[@name = 'LAN' and . = '" + ethernetPortEntryID + "']/../Parameter[@name='IPAddress']");
	 		/*if(ethernetPortConfigEL==null) {
	 			ethernetPortConfigEL = MRXMLUtils.getElementXpath(deviceDoc, "//EthernetPortConfig[.//PortID='" + ethernetPortEntryID + "']//IPAddress");
	 		}*/
	 		if(ethernetPortConfigEL!=null) {
   				//System.out.println("In get IP Address based on url scheme " + MRXMLUtils.elementToString(ethernetPortConfigEL));
   				ipaddress = ethernetPortConfigEL.getText();
   				if(ipaddress == null || "".equals(ipaddress.trim())) {
   					throw new MRException (MediaRoomServiceExceptionCodes.ERROR_GETTING_PUBLISHED_IP, "PortID = " + ethernetPortEntryID);
   				}
   				return ipaddress;
   			}
   			else {
   				throw new MRException (MediaRoomServiceExceptionCodes.ERROR_GETTING_PUBLISHED_IP, "PortID = " + ethernetPortEntryID);
   			}
	 	} else if(useNATZoneIP) {
	 		
	 		//TBD Channge this to NATZone
			Element natTraversalEntryEL =  MRXMLUtils.getElementXpath(deviceDoc, "//NATZoneTableConfig/NATZoneEntry[Name='"+ natZoneEntryName + "']"); 
			if(natTraversalEntryEL!=null) {
				ipaddress = MRXMLUtils.getValueXpath(MRXMLUtils.elementToDocument(natTraversalEntryEL),"//IPAddress", ipaddress);
				if(ipaddress == null || "".equals(ipaddress.trim())) {
   					throw new MRException (MediaRoomServiceExceptionCodes.ERROR_GETTING_PUBLISHED_IP, "NATZoneName = " + natZoneEntryName);
   				}
				return ipaddress;
			}
			else {
				throw new MRException (MediaRoomServiceExceptionCodes.ERROR_GETTING_PUBLISHED_IP, "NATZoneName = " + natZoneEntryName);
			}
	 	}
	 
    	return ipaddress;
    }
    
    
   private static Element getEthernetPortElementBasedOnLAN(Document deviceDoc, boolean isXMPPServerWithHA,String ethernetPortEntryID) throws Exception {    	
    
	    Element element = null;
	    
    	//if xmpp ha enabled find ipaddress based on virtual ip
    	if(isXMPPServerWithHA) {
    		element = MRXMLUtils.getElementXpath(deviceDoc, "//EthernetPortStatus/Parameters/Parameter[@name = 'LAN' and . = '" + ethernetPortEntryID + "']/../Parameter[@name='Virtual' and .='True']/../Parameter[@name='IPAddress']");   	
    	}
    	
    	if(!isXMPPServerWithHA || element == null) {
    		return MRXMLUtils.getElementXpath(deviceDoc, "//EthernetPortStatus/Parameters/Parameter[@name = 'LAN' and . = '" + ethernetPortEntryID + "']/../Parameter[@name='IPAddress']");
    	}
    	
    	return element;
	}
    
    
    
    private static Element getEthernetPortElement(Document deviceDoc, List<?> ethernetPortConfigList, boolean isXMPPServerWithHA) throws Exception {
    	
    	Element element = null;
    	
    	//if xmpp ha enabled find ipaddress based on virtual ip
    	if(isXMPPServerWithHA) {
    		element = MRXMLUtils.getElementXpath(deviceDoc, "//EthernetPortStatus/Parameters/Parameter[@name = 'Primary' and . = 'True']/../Parameter[@name='Virtual' and .='True']/../Parameter[@name='IPAddress']");
    	}
    	

    	if(!isXMPPServerWithHA || element==null) {
    		return (Element) ethernetPortConfigList.get(0);
    	}
		
    	return element;
	}

	private static boolean isXMMPServerWithHAWithVirtualIPSetup(Document deviceDoc) throws Exception {
    	
    	boolean xmppserver = Utils.getBooleanValue(MRXMLUtils.getValueXpath(deviceDoc, "//XMPPAgentServiceConfig/Enable","false"), false);
    	if(!xmppserver)
    		return false;
    	
    	return HAUtils.isHAServiceEnabled();
    }
	 
    
 	/*<OutboundNATTable>
    <UseNATTable>false</UseNATTable>
    <NATTable>
     <NATEntry>
      <Network/>
      <TranslatedNetwork/>
      <Netmask/>
     </NATEntry>
    </NATTable>
   </OutboundNATTable>*/
 	
 

	public static String getResolvedIP(Element networkURLRule, String ipaddress) throws Exception {
 		 
 		 if(networkURLRule == null|| ipaddress == null || "".equalsIgnoreCase("ipaddress")) {
 			 return ipaddress;
 		 }
 		 Element outboundNATTableElement = MRXMLUtils.getElementXpath(MRXMLUtils.elementToDocument(networkURLRule), "//OutboundNATTable");
 		 Document outboundNATTableDoc = MRXMLUtils.elementToDocument(outboundNATTableElement);
 		 String network = MRXMLUtils.getValueXpath(outboundNATTableDoc, "//Network", "");
 		 String translatedNetwork = MRXMLUtils.getValueXpath(outboundNATTableDoc, "//TranslatedNetwork", "");
 		 String netmask = MRXMLUtils.getValueXpath(outboundNATTableDoc, "//Netmask", "");
 		 if(network == null || "".equalsIgnoreCase(network) || translatedNetwork == null || "".equalsIgnoreCase(translatedNetwork) || netmask == null || "".equalsIgnoreCase(netmask) ) {
 			 return ipaddress;
 		 }
 		 else {
 			//System.out.println("Before getTranslatedIP " + ipaddress);
 			if(isNetworkMatch(ipaddress, network, netmask)) { 
 				ipaddress = getTranslatedIP(ipaddress, translatedNetwork, netmask);
 			} else {
 				throw new MRException (MediaRoomServiceExceptionCodes.ERROR_GETTING_RESOLVED_IP, "Network Mismatch , Network = " + network + " Netmask = " + netmask );
 			}
 		 }
 		 
 		 return ipaddress;
 	 }
 	
 	 
 	private static boolean isNetworkMatch(String ipaddress, String network, String netmask) {
    	long ipaddressToNum = convertIPAddressFromDottedDecimalToInt(ipaddress);
    	long netmaskNum = convertIPAddressFromDottedDecimalToInt(netmask);
    	String ipaddressNetwork = intToIp(ipaddressToNum & netmaskNum);
 		if(ipaddressNetwork.equalsIgnoreCase(network)) {
 			return true;
 		}
 		else {
 			return false;
 		}
 	}
 	
 	private static String  getTranslatedIP(String ipaddress, String network, String netmask) {
 		
    	long tempIPAddress  = convertIPAddressFromDottedDecimalToInt("255.255.255.255");
    	long netMaskNum = convertIPAddressFromDottedDecimalToInt(netmask);
    	long hostBitsNum = tempIPAddress ^ netMaskNum;
    	long ipaddressNum = convertIPAddressFromDottedDecimalToInt(ipaddress);
    	long hostNum = ipaddressNum & hostBitsNum;
    	long networkNum = convertIPAddressFromDottedDecimalToInt(network);
    	long translatedIPAddressNum = hostNum | networkNum;
    	String translatedIPAddress = intToIp(translatedIPAddressNum);
    	return translatedIPAddress;
    	
 	}

 	
 	private static long convertIPAddressFromDottedDecimalToInt(String ipaddress){
		 StringTokenizer st = new StringTokenizer(ipaddress,".");
		 long ipaddressparts[] = new long [4];
		 int i= 0;
		 int shiftbits = 24;
		 
		 while(st.hasMoreTokens()) {
			 long number = Long.valueOf(st.nextToken());
			 //System.out.println(Long.toBinaryString(number));
			 ipaddressparts[i] = number<<shiftbits;
			 //System.out.println(Long.toBinaryString(ipaddressparts[i]));
			 shiftbits = shiftbits - 8;
			 i++;
		 }
		 
		 long ipaddressIntNotation = ipaddressparts[0] | ipaddressparts[1] | ipaddressparts[2] | ipaddressparts[3];
		 //System.out.println(ipaddressIntNotation);
		 return ipaddressIntNotation;
	 }

 	private static String intToIp(long i) {
       return ((i >> 24 ) & 0xFF) + "." +
              ((i >> 16 ) & 0xFF) + "." +
              ((i >>  8 ) & 0xFF) + "." +
              ( i        & 0xFF);
   }
 	
 	
    public static String getResolvedIPAddressForURL(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, 
 			Element fromPortResource, Element toPortResource,Document deviceDoc) throws Exception {
    	
    	Element networkAddressRule = PolicyManager.getNetworkAddressRule(request, xmpp, log, db, fromPortResource, toPortResource);
    	String publishedIP = "";
    	try {
			publishedIP =	StreamPolicyUtils.getIPAddressBasedOnURLScheme(networkAddressRule, deviceDoc);
		}catch(MRException me) {
			me.printStackTrace();
    		throw new MRException (MediaRoomServiceExceptionCodes.ERROR_GETTING_PUBLISHED_IP, "Error getting published IP for source " +  me.getData());
			    		
		}
		String resolvedIP = "";
		try {
			resolvedIP = StreamPolicyUtils.getResolvedIP(networkAddressRule, publishedIP);
		}catch(MRException me) {
			me.printStackTrace();
			throw new MRException (MediaRoomServiceExceptionCodes.ERROR_GETTING_RESOLVED_IP, "Error getting resolved IP for source "  + me.getData());
		}
    	return resolvedIP;
    }
 	 

 	/*
 	 * @returns the rule which has the NetworkAddressRule and the published/resolved IP
 	 */
 	private static Element getNATRule(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, 
 			Element fromPortResource, Element toPortResource, String overrideIP, String ruleName, String streamType) throws Exception {
 		Element ruleResult = MRXMLUtils.stringToElement("<" + ruleName + "/>");
		Element networkAddressRule = PolicyManager.getNetworkAddressRule(request, xmpp, log, db, fromPortResource, toPortResource);
		String assetResourceNID = MRXMLUtils.getValueXpath(toPortResource, ".//AssetResourceNID", "");
		String title = MRXMLUtils.getValueXpath(toPortResource, ".//Title", "");
		MRXMLUtils.addAttribute(ruleResult,"title", title);
		if(networkAddressRule != null)
			ruleResult.add(networkAddressRule);
        if( (overrideIP != null) && (!"".equals(overrideIP)) )  {
            MRXMLUtils.addAttribute(ruleResult,"publishedIP", overrideIP);
            MRXMLUtils.addAttribute(ruleResult,"resolvedIP", overrideIP);
    		return ruleResult;
        }
		if("".equals(assetResourceNID)) { // The toPortResource is the assetResource
			assetResourceNID = MRXMLUtils.getAttributeValue(toPortResource,"NID");
		}
		Document deviceDoc = StreamPolicyUtils.getDeviceDoc(request, xmpp, log, db, assetResourceNID);
		String publishedIP = "";
		try {
			publishedIP =	StreamPolicyUtils.getIPAddressBasedOnURLScheme(networkAddressRule, deviceDoc);
		}catch(MRException me) {
			me.printStackTrace();
    		if(ruleName.equalsIgnoreCase("sourcerule") && !isValidSourceIP(publishedIP, streamType)) {
				throw new MRException (MediaRoomServiceExceptionCodes.ERROR_GETTING_PUBLISHED_IP, "Error getting published IP for source " +  me.getData());
			} else if (ruleName.equalsIgnoreCase("destrule") && !isValidDestIP(publishedIP, streamType)) {
				throw new MRException (MediaRoomServiceExceptionCodes.ERROR_GETTING_PUBLISHED_IP, "Error getting published IP for destination " +   me.getData());
			}
    		
		}
		String resolvedIP = "";
		try {
			resolvedIP = StreamPolicyUtils.getResolvedIP(networkAddressRule, publishedIP);
		}catch(MRException me) {
			me.printStackTrace();
			if(ruleName.equalsIgnoreCase("sourcerule") && !isValidSourceIP(resolvedIP, streamType)) {
				throw new MRException (MediaRoomServiceExceptionCodes.ERROR_GETTING_RESOLVED_IP, "Error getting resolved IP for source "  + me.getData());
			} else if (ruleName.equalsIgnoreCase("destrule") && !isValidDestIP(resolvedIP, streamType)) {
				throw new MRException (MediaRoomServiceExceptionCodes.ERROR_GETTING_RESOLVED_IP, "Error getting resolved IP for destination " + me.getData());
			}
		}
		MRXMLUtils.addAttribute(ruleResult,"publishedIP", publishedIP);
		MRXMLUtils.addAttribute(ruleResult,"resolvedIP", resolvedIP);
		return ruleResult;
 	}

 	/*
 	 * @returns the NATResult doc which has the published/resolved IP for source and dest
 	 */
    public static Document getNATResult(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, 
    		Element sourcePortResource, Element destPortResource, String destIP, String streamType) throws Exception {
		Document resultDoc = MRXMLUtils.stringToDocument("<NATResult/>");
		if(destPortResource == null){			
			String assetResourceNID = MRXMLUtils.getValueXpath(sourcePortResource, ".//AssetResourceNID", "");
			Element ruleResult = MRXMLUtils.stringToElement("<sourcerule/>");
			Document deviceDoc = StreamPolicyUtils.getDeviceDoc(request, xmpp, log, db, assetResourceNID);
			String publishedIP =	StreamPolicyUtils.getIPAddressBasedOnURLScheme(null, deviceDoc);
			MRXMLUtils.addAttribute(ruleResult,"publishedIP", publishedIP);
			MRXMLUtils.addAttribute(ruleResult,"resolvedIP", publishedIP);
			resultDoc.getRootElement().add(ruleResult);
		}
		else
		{
	    	Element sourceRuleResult = StreamPolicyUtils.getNATRule(request, xmpp, log, db, 
	    			destPortResource, sourcePortResource, null, "sourcerule", streamType);
	    	resultDoc.getRootElement().add(sourceRuleResult);
	    	Element destRuleResult = StreamPolicyUtils.getNATRule(request, xmpp, log, db, 
	    			sourcePortResource, destPortResource, destIP, "destrule", streamType);
	    	resultDoc.getRootElement().add(destRuleResult);
	        log.addInfo("StreamPolicyUtils:getNATResult: " + MRXMLUtils.documentToString(resultDoc), MRLog.OK, MRLog.NOTIFY);
		}
        return resultDoc;
    }

    public static boolean isValidSourceIP(String ipaddress, String streamType) {
    	try {
    		SSAPI ssAPI = StreamingServerImpl.getSSAPIFromStreamType(streamType);
    		return ssAPI.isValidSourceIP(ipaddress);
    	} catch (Exception e) {
    		e.printStackTrace();
    		return false;
    	}
    }
    
    public static boolean isValidDestIP(String ipaddress, String streamType) {
    	try {
    		SSAPI ssAPI = StreamingServerImpl.getSSAPIFromStreamType(streamType);
    		return ssAPI.isValidDestIP(ipaddress);
    	} catch (Exception e) {
    		e.printStackTrace();
    		return false;
    	}
    }


    public static synchronized Document getProfileRelayInfo(
    		MRRequest request,XMPPI xmpp, MRLog log, MRDB db, 
			MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory,
			String sourceNID, String destNID, String profileNID, String profileXML)
            throws Exception {
    	String key = sourceNID + "." + destNID + "." + request.getUserJID();
    	if(!StreamPolicyUtils.theRelayTable.containsKey(key)) {
    		// System.out.println("MediaRoomServiceUtils:getProfileRelayInfoInternal key=" + key);
            Document requestDoc = MRXMLUtils.loadXML("/com/ipvs/mediaroomservice/xml/GetRelayListRequest.xml", log);
            Element requestRootEL = requestDoc.getRootElement();
            MRXMLUtils.setValueXpath(requestRootEL, "//sourceNID", sourceNID);
            MRXMLUtils.setValueXpath(requestRootEL, "//destNID", destNID);
    		MRRequest policyRequest = MRRequest.createRequest(request.getUserJID());
    		policyRequest.setCache(request.getCache());
            policyRequest.setSuperUser(true);

            Document mroot =  policyRequest.getMessage().getDocument();
            mroot.clearContent();
            mroot.add(requestRootEL.detach());

            MRRequestHandlerI handler = new GetRelayListRequestHandler();
			log.addInfo(policyRequest.hashCode() + " StreamPolicyUtils:getProfileRelayInfo Begin", MRLog.OK, MRLog.NOTIFY);	

            Document doc = handler.getResponse(policyRequest, xmpp, log, db, resourceFactory, serviceAgentFactory);
			log.addInfo(policyRequest.hashCode() + " StreamPolicyUtils:getProfileRelayInfo End", MRLog.OK, MRLog.NOTIFY);	
            
            // Check the size of the table and trim if required
            if(StreamPolicyUtils.theRelayTable.size() > StreamPolicyUtils.MAXCACHESIZE)
            	StreamPolicyUtils.handlePolicyChanged();
            
        	StreamPolicyUtils.theRelayTable.put(key, doc);
    	}
    	else {
            log.addInfo("StreamPolicyUtils:getProfileRelayInfo found cached result key=" + key, MRLog.OK, MRLog.NOTIFY);    		
    	}
		return (Document)StreamPolicyUtils.theRelayTable.get(key).clone();
    }


    public static Document getRelayProfileResult(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, 
    		MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory,
    		String sourceNID, String destPortNID, String profileNID, String profileXML,String requestedProfileNID,String requestedProfileTitle) throws Exception {
    	
		Document relayProfileResultDoc = StreamPolicyUtils.getProfileRelayInfo(request, xmpp, log, db, resourceFactory, serviceAgentFactory, 
				sourceNID, destPortNID, profileNID, profileXML);
		
		//Check online devices first. If all the devices are Offline then pick the first one.
		String xPath ="//RelayProfile[//MediaStreamRelayPortResource[State/StateFlag='Ready']]";
		List<?> relayList = MRXMLUtils.getListXpath(relayProfileResultDoc, xPath);
		if(relayList.isEmpty()){
			xPath ="//RelayProfile";
			relayList = MRXMLUtils.getListXpath(relayProfileResultDoc, xPath);
		} 
		
		if(relayList.isEmpty())
	        throw new MRException(MediaRoomServiceExceptionCodes.PROFILENOTFOUND, "Profile Not Found");
		
		
		
		Element relayProfile = (Element)relayList.get(0);		
		Element profileElement = MRXMLUtils.getElementXpath(relayProfile, ".//Profile");		
		Element streamProfile = MRXMLUtils.getFirstChild(profileElement);
		
		//if user requested profile then check requested profile exists in the result or not.
		String relayProfileXpath = "";
		String requestedProfileXpath = "";
		if(!requestedProfileNID.isEmpty()) {
			requestedProfileXpath = "Profile/*[@NID='"+requestedProfileNID+"']";
		} else if(!requestedProfileTitle.isEmpty()) {
			requestedProfileXpath = "Profile/*[Title='"+requestedProfileTitle+"']";		
		}
		
		if(!requestedProfileXpath.isEmpty()) {
			relayProfileXpath = "//RelayProfile["+requestedProfileXpath+"]";
			relayProfile = MRXMLUtils.getElementXpath(relayProfileResultDoc, relayProfileXpath);
			if(relayProfile == null) {
				throw new MRException(MediaRoomServiceExceptionCodes.CAN_NOT_USE_REQUESTED_PROFILE, "Can not use requested profile. Access Denied");
			}			
			streamProfile = MRXMLUtils.getElementXpath(relayProfile, ".//"+ requestedProfileXpath);
		} 	
		
		
		Element relayElement = MRXMLUtils.getElementXpath(relayProfile, ".//Relay");
		Element relayResource = null;
		// make sure get relay profile result return relay between source and destination. 
		// If it is direct connection no need to check the below condition
		if (relayElement.nodeCount() > 0) {
			// check for streamType and throw  exception
			Document sourceDoc = MRXMLDBUtils.readElement(request, log, db, sourceNID);
			String streamType = MRXMLUtils.getValueXpath(sourceDoc, "//StreamType", "");
			if (streamType.equals("RTP-RAW") || streamType.equals("RTP-PCM") || streamType.equals("RFB")) {
				throw new MRException(MediaRoomServiceExceptionCodes.RELAY_NOT_SUPPORTED_FOR_STREAM_TYPE, "Relay not supported for streamType " + streamType);
			}
			relayResource = MRXMLUtils.getElementXpath(relayElement, "//MediaStreamRelayPortResource[State/StateFlag='Ready']");
			if (relayResource == null) {
				relayResource = MRXMLUtils.getFirstChild(relayElement);
				throw new MRException(MediaRoomServiceExceptionCodes.DEVICE_UNAVAILABLE, MRXMLUtils.getValueXpath(relayResource, "//Title", "") + " Device Unavailable");
			}
		}
		String relayNID = "";
		if(relayResource != null) {
			relayNID = MRXMLUtils.getAttributeValue(relayResource,"NID");
			//adding to request cache to avoid multiple reads			
			request.getCache().put(relayNID, MRXMLUtils.elementToDocument(relayResource));
		}
		if(streamProfile == null) {
	        throw new MRException(MediaRoomServiceExceptionCodes.PROFILENOTFOUND, "Profile Not Found");
		}
		profileNID = MRXMLUtils.getAttributeValue(streamProfile,"NID");
		if(profileNID == null)
			profileNID = "";
		profileXML = MRXMLUtils.elementToString(streamProfile);

		Document resultDoc = MRXMLUtils.stringToDocument("<RelayProfile/>");
		MRXMLUtils.addAttribute(resultDoc.getRootElement(),"relayNID", relayNID);
		MRXMLUtils.addAttribute(resultDoc.getRootElement(),"profileNID", profileNID);
		MRXMLUtils.addElement(resultDoc.getRootElement(), (Element)streamProfile.detach());
		return resultDoc;

    }
	

    public static synchronized void handlePolicyChanged() {
    	MRLog.getInstance(MRLog.CONSOLE_DAEMON).addInfo("Invalidate the global Cache");
    	StreamPolicyUtils.theRelayTable.clear();   	
    }
    
    public static void updateRecorderConfigInProfile(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, String sourceNID, Element profileXML) throws Exception {
    	Document sourceDoc = MRXMLDBUtils.readElement(request, log, db, sourceNID);
        if(sourceDoc.getRootElement().getName().equals(AMConst.MEDIA_STREAM_FILE_RESOURCE)) {
    		Element sourceDocRecorderConfigEL = MRXMLUtils.getElementXpath(sourceDoc, ".//V2DRecorderConfig");
    		Element profileXMLRecorderConfigEL = MRXMLUtils.getElementXpath(profileXML, ".//V2DRecorderConfig");
    		MRXMLUtils.replaceContent(profileXMLRecorderConfigEL, sourceDocRecorderConfigEL);
        }
    	
    }

}
