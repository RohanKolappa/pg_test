package com.ipvsserv.common.config;

import java.util.Hashtable;
import java.util.Iterator;

import org.dom4j.Element;

/**
 * @author pravin reddy
 * Email: pravin@ipvideosys.com
 * IP Video Systems , CA, USA
 * http://www.ipvideosys.com 
 * @since 1.0.0
 */
public class DomainDBSchema {
    private Hashtable<String, String> domainsConst = new Hashtable<String, String>();
    private Hashtable<String, String> collPrefix = new Hashtable<String, String>();
    private Hashtable<String, String> indexPrefix = new Hashtable<String, String>();
    private Hashtable<String, String> indexXpath = new Hashtable<String, String>();
    
    public DomainDBSchema() {}
    public String getDomainName(String collectionName){
         return getDomainName(collectionName, null);
    }
    public String getDomainName(String collectionName, String defualt){
        String collectionNamePrefix = IpvsServGlobals.getDomainDBSchema().getCollectionNamePrefix("data");
        String domainName = null;
        if(collectionName!=null && collectionName.startsWith(collectionNamePrefix)) {
            int pos = collectionName.indexOf(collectionNamePrefix) + collectionNamePrefix.length();
            domainName = collectionName.substring(pos);
        }  
        collectionNamePrefix = IpvsServGlobals.getDomainDBSchema().getCollectionNamePrefix("schema");
        if(collectionName!=null && collectionName.startsWith(collectionNamePrefix)) {
            int pos = collectionName.indexOf(collectionNamePrefix) + collectionNamePrefix.length();
            domainName = collectionName.substring(pos);
        }            
        if(domainName ==null || domainName.equals(""))
            return defualt;
         return domainName.trim();   
    }
    public void init() {
        Element domainsEl = IpvsServGlobals.getElement("domainSchema");
        Iterator<?> iterParams = domainsEl.elementIterator("const");
        while (iterParams.hasNext()) {
            Element elParam = (Element) iterParams.next();
            String paramName = elParam.attributeValue("name");
            String paramValue = elParam.getTextTrim();
            if (paramName != null && paramValue != null) {
                domainsConst.put(paramName.trim(), paramValue.trim());
            }
        }
        
        iterParams = domainsEl.elementIterator("collection");
        while (iterParams.hasNext()) {
            Element elParam = (Element) iterParams.next();
            String paramName = elParam.attributeValue("name");
            String prefixConst = elParam.attributeValue("prefixConst");
            if (prefixConst != null && prefixConst != null) {
                String paramValue = domainsConst.get(prefixConst);
                if (paramName != null && paramValue != null) {
                    collPrefix.put(paramName.trim(), paramValue.trim());
                }
            }
        }
        
        iterParams = domainsEl.elementIterator("index");
        while (iterParams.hasNext()) {
            Element elParam = (Element) iterParams.next();
            String paramName = elParam.attributeValue("name");
            String prefixConst = elParam.attributeValue("prefixConst");
            String xPath = elParam.attributeValue("xPath");
            if (prefixConst != null && prefixConst != null) {
                String paramValue = domainsConst.get(prefixConst);
                if (paramName != null && paramValue != null) {
                    indexPrefix.put(paramName.trim(), paramValue.trim());
                    indexXpath.put(paramName.trim(), xPath.trim());
                }
            }
        }
        
    }
    
    public String getDomainsPropterty(String name) {
        return domainsConst.get(name);
    }
    
    public String getCollectionName(String name, String domain) {
        return collPrefix.get(name) + domain;
    }
    
    public String getIndexName(String name, String domain) {
        return indexPrefix.get(name) + domain;
    }
    
    public String getIndexXpath(String name) {
        return indexXpath.get(name);
    }
    
    public String getCollectionNamePrefix(String name) {
        return collPrefix.get(name) ;
    }
    
    public String getIndexNamePrefix(String name) {
        return indexPrefix.get(name);
    }
    
     
}
/*   
 	<domainSchema>
	    <const name='DB_COL_PREFIX'>IPVSc_</const>
	    <const name='DB_DOC_PREFIX'>IPVSd_</const>
	    <const name='DB_INDEX_NODE_PREFIX'>IPVSi_nids_</const>
	    <const name='COL_INDEX_NODE_PREFIX'>IPVSis_colnids_</const>	
	    <const name='DB_SUB_DOC_PREFIX'>IPVScs_</const>
	    <const name='DB_SUB_COL_PREFIX'>IPVSds_</const>
		<collection name="data" prefixConst='DB_COL_PREFIX'/>
		<collection name="schema" prefixConst='DB_SUB_COL_PREFIX'/>
		<index name="nodeIndex" prefixConst='DB_COL_PREFIX' xPath='//@NID'/>
		<index name="nodeColIndex" prefixConst='DB_COL_PREFIX' xPath='//@parentNID'/>
 			 	
 	</domainSchema>
 
 
 */
