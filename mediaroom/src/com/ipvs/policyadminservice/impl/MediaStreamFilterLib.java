package com.ipvs.policyadminservice.impl;

import java.util.ArrayList;
import java.util.Hashtable;
import java.util.List;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.db.MRXMLDBUtils;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.policyadminservice.utils.PolicyAdminConstants;
import com.ipvs.utils.FileUtils;

public class MediaStreamFilterLib {
	public static final String MEDIA_STREAM_FILTER_LIST_QUERY = "/com/ipvs/policyadminservice/xquery/MediaStreamFilterListQuery.xml";
	public static final String MEDIA_STREAM_FILTER="MediaStreamFilter";
	public static final String RELAY_XPATH ="RelayXpath";
	public static final String PROFILE_XPATH ="ProfileXpath";

	public static final String RELAY_RULE_KEY_LIST ="RelayRuleKeyList";
	public static final String PROFILE_RULE_KEY_LIST ="ProfileRuleKeyList";
	public static final String MEDIA_STREAM_FILTER_ALL_FILTER = "(User and Resource)";

	private static Document mediaStreamFilterDoc = null;
	private static MediaStreamFilterLib mediaStreamFilterLibObject = null;
	public static final String RESOLVED_RULE_KEY_XPATH ="ResolvedRuleKeyXPath";
	
	
	private MediaStreamFilterLib(){}

	public static MediaStreamFilterLib getMediaStreamFilterLibObject() {
		if(mediaStreamFilterLibObject == null) {
			mediaStreamFilterLibObject = new MediaStreamFilterLib();
		}
		return mediaStreamFilterLibObject;
	}

	/*
	Query output Format
    <PolicyRule>
         <Name/>
	     <SourceXpath/>
	     <DestinationXpath/>
	     <RelayXpath/>
	     <ProfileXpath/>
	     <RuleKeyList>
	        <RelayRuleKeyList/>
	        <ProfileRuleKeyList/>
	     </RuleKeyList>
	   </PolicyRule>
	 */

	public synchronized Document getMediaStreamFilterDoc(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, 
			ServiceAgentFactoryI serviceAgentFactory) throws MRException,Exception {

		if(mediaStreamFilterDoc == null) {			
			Hashtable<String, String> table = new Hashtable<String, String>();			
			String query = 	FileUtils.readFileResource(PEMDBUtils.XQUERYLIB, request.getClass()) +
			FileUtils.readFileResource(MediaStreamFilterLib.MEDIA_STREAM_FILTER_LIST_QUERY, request.getClass());			
			String policyXML = MRXMLDBUtils.readQuery(MRXMLDBUtils.TIME_OUT, request, log, db, query, table, null);

			log.addInfo("Read Policy From DB. PolicyName :="+MEDIA_STREAM_FILTER);
			if(policyXML == null) 
				return null;
			mediaStreamFilterDoc = MRXMLUtils.stringToDocument(policyXML);
		}

		return mediaStreamFilterDoc;
	}

	public synchronized void removeCache() {
		mediaStreamFilterDoc = null;
	}

	/**
	 * Return matching stream filters for given source and destination
	 * @param userRequestDoc
	 * @param relayType
	 * @param request
	 * @param xmpp
	 * @param log
	 * @param db
	 * @param resourceFactory
	 * @param serviceAgentFactory
	 * @return
	 * @throws MRException
	 * @throws Exception
	 */

	public  Document getMatchingMediaStreamFilter(Document userRequestDoc,String relayType,MRRequest request, XMPPI xmpp, MRLog log, MRDB db,
			MRResourceFactoryI resourceFactory,	ServiceAgentFactoryI serviceAgentFactory) throws MRException,Exception {
		Document matchedStreamFilterList = MRXMLUtils.getDocument();
		matchedStreamFilterList.setRootElement(MRXMLUtils.addElement(PolicyManager.POLICY_RULE_LIST));

		Document mediaStreamFilterDoc = getMediaStreamFilterDoc(request, xmpp, log, db, resourceFactory, serviceAgentFactory);
		List<?> policiesList = MRXMLUtils.getListXpath(mediaStreamFilterDoc, "//"+PolicyManager.POLICY_RULE);
		if(policiesList == null || policiesList.isEmpty()){
			return null;
		}
		Element policyRuleEL = null;
		String sourceXpath = null;
		String destinationXpath = null;
		String relayXpath = null;
		String resolvedRuleKeyXpath = null;

		for(Object object : policiesList) {

			policyRuleEL = (Element)((Element) object).clone();
			sourceXpath = MRXMLUtils.getValueXpath(policyRuleEL, "./"+ MediaRoomPolicyLib.SOURCE_XPATH, "");		
			destinationXpath = MRXMLUtils.getValueXpath(policyRuleEL, "./"+ MediaRoomPolicyLib.DESTINATION_XPATH, "");	
			relayXpath = MRXMLUtils.getValueXpath(policyRuleEL, "./"+ RELAY_XPATH, "");	
			resolvedRuleKeyXpath = MRXMLUtils.getValueXpath(policyRuleEL, "./"+ RESOLVED_RULE_KEY_XPATH,"");
			boolean skipFilter = false;
			switch (PolicyAdminConstants.RelayTypeList.valueOf(relayType)) {
			
			case SourceRelay:
				// Destination should be ALL or any thing. Source should match with given source
				// if source xpath is ALL or no relay then skip the filter. 
				if(relayXpath.isEmpty() || sourceXpath.contains(MEDIA_STREAM_FILTER_ALL_FILTER)) {
					skipFilter=true;
				}

				break;

			case DestinationRelay:
				// Source should be ALL or any thing. Destination should match with given destination
				// if destination xpath is ALL or no relay then skip the filter.
				if( relayXpath.isEmpty() || destinationXpath.contains(MEDIA_STREAM_FILTER_ALL_FILTER)) {
					skipFilter=true;					
				}
				break;

			case NoRelay:
				// Source & Destination should match with given source and destination
				// Direct rule should match with direct connection rule. No relay, Source and Destination should match to given sources and destination
				if(!relayXpath.isEmpty() || sourceXpath.contains(MEDIA_STREAM_FILTER_ALL_FILTER) || destinationXpath.contains(MEDIA_STREAM_FILTER_ALL_FILTER)) {
					skipFilter=true;					
				}

				break;

			case ALLCheck:		        	
				// for ALL check check the source and destination should be ALL.		      
				if(!sourceXpath.equals(MEDIA_STREAM_FILTER_ALL_FILTER) || !destinationXpath.equals(MEDIA_STREAM_FILTER_ALL_FILTER)) {
					skipFilter=true;					
				}
				break;             
			default:  
				break;
			}
			//if operator, source ,destination and role action matches then add policy to matching policy document 
			if(!skipFilter &&  MRXMLUtils.getElementXpath(userRequestDoc,"//"+PolicyAdminConstants.SOURCE + "[" + sourceXpath +"]")!= null &&  
					MRXMLUtils.getElementXpath(userRequestDoc,"//"+PolicyAdminConstants.DESTINATION + "[" + destinationXpath +"]")!=null && (resolvedRuleKeyXpath.isEmpty() 
							|| MRXMLUtils.applyXPath(userRequestDoc, resolvedRuleKeyXpath))){
								MRXMLUtils.addElement(matchedStreamFilterList.getRootElement(), (Element)policyRuleEL.clone());
			}
		}

		return matchedStreamFilterList;
	}


	public Document getRelayAndProfileXpath(String relayType,Document userRequestDoc,
			MRRequest request, XMPPI xmpp, MRLog log, MRDB db,MRResourceFactoryI resourceFactory,ServiceAgentFactoryI serviceAgentFactory) throws MRException,Exception {
		String relayXpath = "";
		String profileXpath = "";
		Element policyRuleEL= null;
		String relayRulekeyXpath = "";
		String profileRulekeyXpath = "";
		RelayProfileXPath relayProfileXpath = null;
		List<RelayProfileXPath> relayProfileXpathList = new ArrayList<RelayProfileXPath>();
		Document matchingPoliciesDoc = getMatchingMediaStreamFilter(userRequestDoc, relayType, request, xmpp, log, db, resourceFactory, serviceAgentFactory);		
		List<?> policyRuleList = MRXMLUtils.getListXpath(matchingPoliciesDoc, "//"+PolicyManager.POLICY_RULE);
		if(policyRuleList.isEmpty()) {
			return generateRelayAndProfileResponseXML(relayProfileXpathList, relayType);
		}
		String relayRuleKeyElXpath =  "./"+PolicyManager.RULE_KEY_LIST +"/"+RELAY_RULE_KEY_LIST +"/"+ PolicyManager.RULE_KEY;
		String profileRuleKeyElXpath =  "./"+PolicyManager.RULE_KEY_LIST +"/"+PROFILE_RULE_KEY_LIST +"/"+ PolicyManager.RULE_KEY;
		for(Object object : policyRuleList) {
			policyRuleEL = (Element) object;
			relayXpath = MRXMLUtils.getValueXpath(policyRuleEL, "./"+RELAY_XPATH, null);
			profileXpath = MRXMLUtils.getValueXpath(policyRuleEL, "./"+PROFILE_XPATH, null);
			relayXpath = PolicyManager.parseTargetXPath(relayXpath);
			if(relayXpath != null) {					
				List<?> relayRuleKeyList = MRXMLUtils.getListXpath(policyRuleEL,relayRuleKeyElXpath);	
				relayRulekeyXpath = PolicyManager.generateRuleKeyXpath(userRequestDoc, policyRuleEL,relayRuleKeyList);		
				if(relayRulekeyXpath!=null && !relayRulekeyXpath.equals(PolicyAdminConstants.NULL)) {
					if(!relayXpath.isEmpty()) {
						relayXpath =  "("+relayXpath+")" + " and "+ "("+ relayRulekeyXpath +")";
					}
					else {
						relayXpath = "("+ relayRulekeyXpath +")";
					}
				} else if(relayRulekeyXpath!=null && relayRulekeyXpath.equals(PolicyAdminConstants.NULL)) {
					continue;
				}
			}
			
			
			List<?> profileRuleKeyList = MRXMLUtils.getListXpath(policyRuleEL,profileRuleKeyElXpath);	
			profileRulekeyXpath = PolicyManager.generateRuleKeyXpath(userRequestDoc, policyRuleEL,profileRuleKeyList);		
			if(profileRulekeyXpath!=null  && !profileRulekeyXpath.equals(PolicyAdminConstants.NULL)) {
				if(!profileXpath.isEmpty()) {
					profileXpath =  "("+profileXpath+")" + " and "+ "("+ profileRulekeyXpath +")";
				}
				else {
					profileXpath = "("+ profileRulekeyXpath +")";
				}
			} else if(profileRulekeyXpath!=null && profileRulekeyXpath.equals(PolicyAdminConstants.NULL)){
				continue;
			}
			relayProfileXpath = new RelayProfileXPath();
			relayProfileXpath.setProfilexPath(profileXpath);
			relayProfileXpath.setResourceFilterXPath(relayXpath);
			relayProfileXpathList.add(relayProfileXpath);		
		}
		
		return generateRelayAndProfileResponseXML(relayProfileXpathList, relayType);	
	}

	public static Document generateRelayAndProfileResponseXML(List<RelayProfileXPath> filterXpathList, String relayType)
	    throws Exception {
	
		StringBuffer xpathList = new StringBuffer("<XPathList>");
	
	    for (int i = 0; (filterXpathList != null) && (i < filterXpathList.size()); i++) {
	        if (filterXpathList.get(i) != null) {
	            StringBuffer xpath = new StringBuffer("<XPath>");	            
	            if (filterXpathList.get(i).getUserFilterXPath() != null) {                    
	            	xpath.append("<UserXPath>");
	                xpath.append(filterXpathList.get(i).getUserFilterXPath());
	                xpath.append("</UserXPath>");
	            }              
	            if (filterXpathList.get(i).getResourceFilterXPath() != null) {                   
	            	xpath.append("<MediaStreamResourceXPath>");
	                xpath.append(filterXpathList.get(i).getResourceFilterXPath());
	                xpath.append("</MediaStreamResourceXPath>");
	            }               
	            if (filterXpathList.get(i).getProfilexPath() != null) {                   
	            	xpath.append("<ProfileXPath>");
	                xpath.append(filterXpathList.get(i).getProfilexPath());
	                xpath.append("</ProfileXPath>");
	            }            
	            xpath.append("</XPath>");
	            xpathList.append(xpath);
	        }
	    }
	
	    xpathList.append("<RelayType>" + relayType + "</RelayType>");
	    xpathList.append("</XPathList>");
	    return MRXMLUtils.stringToDocument(xpathList.toString());
	}

}
