package com.ipvs.policyadminservice.impl;

import java.util.Hashtable;
import java.util.Iterator;
import java.util.List;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.db.MRXMLDBUtils;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.utils.FileUtils;

public class NetworkAddressFilterLib {
    private static Document theNetworkAddressFilterDoc = null;
	public static final String NETWORKADDRESSLISTQUERY = "/com/ipvs/policyadminservice/xquery/NetworkAddressRuleListQuery.xml";
	public static final String NETWORKADDRESSRULELISTREQUEST = "/com/ipvs/policyadminservice/impl/NetworkAddressRuleListRequest.xml";
	private static NetworkAddressFilterLib networkAddressFilterLibObject = null;
	
	private NetworkAddressFilterLib() {}

	public static NetworkAddressFilterLib getNetworkAddressFilterLibObject() {
		if(networkAddressFilterLibObject == null) {
			networkAddressFilterLibObject = new NetworkAddressFilterLib();
		}
		return networkAddressFilterLibObject;
	}
	
	public synchronized Document getNetworkAddressRuleListFilter(MRRequest request, XMPPI xmpp, MRLog log, MRDB db) throws Exception {
		if(theNetworkAddressFilterDoc == null){
	    	long dbtimeout = MRXMLDBUtils.TIME_OUT;
	    	Hashtable<String, String> table = new Hashtable<String, String>();
	    	String query = 	FileUtils.readFileResource(PEMDBUtils.XQUERYLIB, request.getClass()) +
	    					FileUtils.readFileResource(NETWORKADDRESSLISTQUERY, request.getClass());
	    	String filterString = MRXMLDBUtils.readQuery(dbtimeout, request, log, db, query, table, null);
	    	theNetworkAddressFilterDoc = MRXMLUtils.stringToDocument(filterString);
		}
    	return theNetworkAddressFilterDoc;
	}
	
	
	public synchronized void removeCache() {
		theNetworkAddressFilterDoc = null;
	}
	
	
	public Element getNetworkAddressRule(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, 
    		Element fromPortResource, Element toPortResource) throws Exception {
    	
    	
    	Document filterdoc = getNetworkAddressRuleListFilter(request, xmpp, log, db);    	
    	// System.out.println("StreamPolicyUtils:getNetworkAddressRule filterdoc=:" + MRXMLUtils.documentToString(filterdoc));
    	Document networkurlpublishresourcerequest = MRXMLUtils.loadXMLResource(NETWORKADDRESSRULELISTREQUEST, log);
		MRXMLUtils.getElementXpath(networkurlpublishresourcerequest, "//From/Resource").add((Element)fromPortResource.clone());
		MRXMLUtils.getElementXpath(networkurlpublishresourcerequest, "//To/Resource").add((Element)toPortResource.clone());
    	// System.out.println("StreamPolicyUtils:getNetworkAddressRule request=:" + MRXMLUtils.documentToString(networkurlpublishresourcerequest));
    	List<?> rulelist = MRXMLUtils.getListXpath(filterdoc, "//rule");
    	for(Iterator<?> i = rulelist.iterator(); i.hasNext();) {
    		Element rule = (Element)i.next();
    		//System.out.println(StMRXMLUtils.documentToString(networkurlpublishresourcerequest));
    		String rulexpath = rule.elementText("rulexpath");
    		//System.out.println("Rule XPATH : " + rulexpath);
    		boolean isMatch = false;
    		if(rulexpath!=null && !"".equals(rulexpath)) {
    			isMatch = MRXMLUtils.applyXPath(networkurlpublishresourcerequest, rulexpath);
    		}	
        	if(isMatch) {
        		//System.out.println(MRXMLUtils.elementToString(MRXMLUtils.getElementXpath(MRXMLUtils.elementToDocument(rule), "//URLPublishScheme")));
        		return (Element)(MRXMLUtils.getElementXpath(rule, ".//NetworkAddressRule").clone());
        	}	
    	}
    	return null;
    } 
}
