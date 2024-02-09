package com.ipvs.nimbus;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Vector;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.common.MRException;
import com.ipvs.common.MRExceptionCodes;
import com.ipvs.common.MRLog;
import com.ipvs.common.MRRequestI;
import com.ipvs.common.db.DBUtilsBase;
import com.ipvs.common.db.IpvsDBI;
import com.ipvs.common.db.IpvsDBUtils;
import com.ipvs.common.db.NIDSchemeMDC;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRXMLUtils;

public class MRAppDB implements IpvsDBI{
	
	MRDBI mrComponentDB;
	MRDB mrdb;
	Document schemadoc;
	MRPubSubI mrPubSub;
	
	MRAppDB(MRDBI mrComponentDB, MRDB mrdb, MRPubSubI pubSub) {
		this.mrComponentDB= mrComponentDB;
		this.mrdb = mrdb;
		this.schemadoc = null;
		this.mrPubSub = pubSub;
	}

	@Override
	public Element getMRSchema(long DBCallTimeOut, MRDB db, MRLog log,
			String schemaPath) throws MRException {
		try {
			Element element = this.schemadoc.getRootElement();
			IpvsDBUtils.parseNID_BULK_SCHEMA(element, db.getServiceDomain());
			return (Element) element.clone();
		}
		catch(Exception e) {
			e.printStackTrace();
			throw new MRException(MRExceptionCodes.DB_NOT_INITIALIZED, "MRDBApp:getMRSchema exception=" + e.toString());			
		}
	}

	@Override
	public void initDB(long DBCallTimeOut, MRDB db, MRLog log,
			String ipvsRootNID, String serviceDomain, String schemaPath,
			boolean reset) throws MRException {
		try {
			schemadoc = MRXMLUtils.loadXMLResource(schemaPath, log);
		}
		catch(Exception e) {
			e.printStackTrace();
			throw new MRException(MRExceptionCodes.DB_NOT_INITIALIZED, "MRDBApp:initDB exception=" + e.toString());			
		}
	}
	
	String getCollectionName(String objectType, String NID) {
		if(objectType == null)
			return null;
		String collectionName = mrdb.getServiceDomainRootNID(objectType);
		if(collectionName != null)
			return collectionName;
		if( (NID == null) || NID.isEmpty()) // Need deeper lookup since DB will not have an NID to find listname
			collectionName = "ipvs." + mrdb.getServiceDomainRootNID(null, objectType);
		return collectionName;
		
	}

	@Override
	public boolean objectExists(long DBCallDBCallTimeOut, MRRequestI request,
			MRDB db, MRLog log, String listName, String objectType, String NID,
			String childXpath, String xQueryPrefix) throws MRException {
		// TODO Auto-generated method stub
		return false;
	}

	private String getNID(String objectType, Element dataElement, String childXpath,String NID) throws Exception {
	    boolean isDocumentRootNode = true;
	    if (childXpath != null && !childXpath.trim().equals("")) {
	        isDocumentRootNode = false;
	    }
	    MRRequest request = null;
	    MRLog log = null;
	    String listName = null;
	    NIDSchemeMDC schema = IpvsDBUtils.resolveNID(request, log, this.mrdb, listName, objectType, NID, dataElement, childXpath, true, isDocumentRootNode);
	    if (isDocumentRootNode) {
	        //update the xml with all NIDs
	        //does not creates new NID, if the element already has NID attribute. 
	        MRXMLUtils.addAttribute(dataElement, IpvsDBUtils.NODEID_2, schema.getDocNodeID());
	        MRXMLUtils.addAttribute(dataElement, IpvsDBUtils.PARENT_NODEID, schema.getCollectionNodeID());
	    }
	    return schema.getNID();
	}

	@Override
	public Document add(long DBCallTimeOut, MRRequestI request, MRLog log, MRDB db, String listName, String objectType, String NID, String childXpath, String xQueryPrefix,
			String[] arrWhereXpath, Element dataElement) throws MRException {
		String elementXpath = childXpath;
		try {
			String jid = IpvsDBUtils.getUserJID(request, db); //subscriber JID  
			String whereXpath = DBUtilsBase.convertArrayToXpath(arrWhereXpath);
			//if ((NID == null) || NID.isEmpty() || NID.split("\\.").length == 2)
			NID = this.getNID(objectType, dataElement, childXpath,NID);
			String collectionName = getCollectionName(objectType, NID);
			ArrayList<String> iList = this.mrComponentDB.addElement(collectionName, NID, elementXpath, whereXpath, dataElement);
			if (!iList.isEmpty()) {
				for (String nid : iList) {
				NID = nid;
				whereXpath = null;
				mrPubSub.addDocumentSubNode(NID, jid);
				String message = MRPubSub.getNotificationMessage("add", log, db, listName, objectType, NID, childXpath, dataElement, jid);
				mrPubSub.addNotificationToQueue(message);
				}
				return formatDBUpdateResponse(this.mrComponentDB.readSingleDocument(collectionName, NID, whereXpath));
			}
			return formatDBUpdateResponse(null);
		} catch (Exception e) {
			e.printStackTrace();
			throw new MRException(MRExceptionCodes.DB_NOT_INITIALIZED, "MRDBApp:add exception=" + e.toString());
		}
	}
	
	@Override
	public Document replace(long DBCallTimeOut, MRRequestI request, MRLog log, MRDB db, String listName, String objectType, String NID, String childXpath, String xQueryPrefix,
			String[] arrWhereXpath, Element dataElement) throws MRException {
		String elementXpath = childXpath;
		try {
			String jid = IpvsDBUtils.getUserJID(request, db); //subscriber JID   					
			String whereXpath = DBUtilsBase.convertArrayToXpath(arrWhereXpath);
			//if ((NID == null) || NID.isEmpty() || NID.split("\\.").length == 2)
				NID = this.getNID(objectType, dataElement, childXpath,NID);
			String collectionName = getCollectionName(objectType, NID);
			ArrayList<String> iList = this.mrComponentDB.replaceElement(collectionName, NID, elementXpath, whereXpath, dataElement);
			if (!iList.isEmpty()) {
				for (String nid : iList) {
					NID = nid;
					whereXpath = null;				
					mrPubSub.addDocumentSubNode(NID, jid);
					String message = MRPubSub.getNotificationMessage("replace", log, db, listName, objectType, NID, childXpath, dataElement, jid);
					mrPubSub.addNotificationToQueue(message);
				}
				return formatDBUpdateResponse(this.mrComponentDB.readSingleDocument(collectionName, NID, whereXpath));
			}
			return formatDBUpdateResponse(null);
		} catch (Exception e) {
			throw new MRException(MRExceptionCodes.DB_NOT_INITIALIZED, "MRDBApp:replace exception=" + e.toString());
		}
	}

	private Document formatDBUpdateResponse(Document dbResp) throws Exception {
		if (dbResp != null)
			return MRXMLUtils.stringToDocument("<DBResp>" + MRXMLUtils.documentToStringOmitDeclaration(dbResp) + "</DBResp>");
		return MRXMLUtils.stringToDocument("<DBResp/>");
	}
	
	@Override
	public Document replaceAttribute(long DBCallTimeOut, MRRequestI request,
			MRLog log, MRDB db, String listName, String objectType, String NID,
			String childXpath, String xQueryPrefix, String[] arrWhereXpath,
			String data) throws MRException {
		String jid = IpvsDBUtils.getUserJID(request, db); //subscriber JID   
		String whereXpath = DBUtilsBase.convertArrayToXpath(arrWhereXpath);
		if(whereXpath!=null && !whereXpath.isEmpty() && whereXpath.indexOf("/*")==-1){
		whereXpath = "/*"+whereXpath;
		}
		String collectionName = getCollectionName(objectType, NID);
		try {
			ArrayList<String> iList = this.mrComponentDB.replaceAttribute(collectionName, NID, childXpath, whereXpath, data);
			if (!iList.isEmpty()) {
				for (String nid : iList) {
					NID = nid;
					whereXpath = null;				
					mrPubSub.addDocumentSubNode(NID, jid);
					String message = MRPubSub.getNotificationMessage("replace", log, db, listName, objectType, NID, childXpath, null, jid);
					mrPubSub.addNotificationToQueue(message);
				}
				return formatDBUpdateResponse(this.mrComponentDB.readSingleDocument(collectionName, NID, whereXpath));
			}
			return formatDBUpdateResponse(null);
		} catch (Exception e) {
			throw new MRException(MRExceptionCodes.DB_NOT_INITIALIZED, "MRDBApp:replaceAttribute exception=" + e.toString());
		}
	}

	@Override
	public Document delete(long DBCallTimeOut, MRRequestI request, MRLog log, MRDB db, String listName, String objectType, String NID, String childXpath, String xQueryPrefix,
			String[] arrWhereXpath) throws MRException {
		try {
			String jid = IpvsDBUtils.getUserJID(request, db); //subscriber JID   
			String elementXpath = childXpath;
			String whereXpath = DBUtilsBase.convertArrayToXpath(arrWhereXpath);
			if (NID != null && NID.split("\\.").length == 2) {
				NID = "";
			}
			String collectionName = getCollectionName(objectType, NID);
			ArrayList<String> nidList = this.mrComponentDB.deleteElement(collectionName, NID, elementXpath, whereXpath);
			if (!nidList.isEmpty()){
				for (String nid : nidList) {
				NID = nid;
				mrPubSub.addDocumentSubNode(NID, jid);
				String message = MRPubSub.getNotificationMessage("delete", log, db, listName, objectType, NID, childXpath, null, jid);
				mrPubSub.addNotificationToQueue(message);
				}
				return formatDBDeleteResponse(nidList, collectionName,objectType);
			}
			return formatDBDeleteResponse(null, null,null);
		} catch (Exception e) {
			e.printStackTrace();
			throw new MRException(MRExceptionCodes.DB_NOT_INITIALIZED, "MRDBApp:readList exception=" + e.toString());
		}
	}

	private Document formatDBDeleteResponse(List<String> nidList, String collectionName, String objectType) throws Exception {
		if(nidList!=null && !nidList.isEmpty() && collectionName!=null){
			if(objectType == null)
				objectType ="DeletedItem"; //Default value if objectType is null
		StringBuffer sb = new StringBuffer();
		sb.append("<DBResp>");
		for(String nid : nidList ){
			sb.append("<"+objectType+ " NID=\"" + nid + "\" parentNID=\"" + collectionName + "\"/>");
		}
		sb.append("document deleted</DBResp>");
		return MRXMLUtils.stringToDocument (sb.toString());
		}else 
			return MRXMLUtils.stringToDocument("<DBResp/>");
	}

	@Override
	public Element readSingle(long DBCallTimeOut, MRRequestI request,
			MRLog log, MRDB db, String listName, String objectType, String NID,
			String childXpath, String xQueryPrefix, String[] arrWhereXpath,
			String[] sortByXpath, String sortOrder, String selectXML,
			Element appendToParentElement) throws MRException {
		try {
			String jid = IpvsDBUtils.getUserJID(request, db);
			String elementXpath = childXpath;
			String whereXpath = DBUtilsBase.convertArrayToXpath(arrWhereXpath);
			if(whereXpath!=null && !whereXpath.isEmpty() && whereXpath.indexOf("/*")==-1){
				whereXpath = "/*"+whereXpath;
			}
			
			Element result = this.mrComponentDB.readSingleElement(getCollectionName(objectType, NID), NID, elementXpath, whereXpath);
			mrPubSub.subscribeNode(NID, jid, "", "");
			return result;
					
		}
		catch(Exception e) {
			e.printStackTrace();
			throw new MRException(MRExceptionCodes.DB_NOT_INITIALIZED, "MRDBApp:readSingle exception=" + e.toString());			
		}
	}

	private Element formatReadListResponse(Element data, Element appendToParentElement, String objectType, int startFrom, int size) throws Exception {
		if(appendToParentElement != null) {
			Element parent = appendToParentElement.getParent();
			if(parent != null) { // Need to update  the ObjectCount etc
		        Element getObjectListResponseData = MRXMLUtils.stringToElement("<GetListObjectResponseData><ObjectCount>" + String.valueOf(size) + "</ObjectCount><ObjectLastWhereEndCount>" + String.valueOf(startFrom + size) + "</ObjectLastWhereEndCount></GetListObjectResponseData>");
		        parent.add(getObjectListResponseData);
			}
			return appendToParentElement;
		}
		
        Document responsedoc = MRXMLUtils.stringToDocument(
        		"<GetResponse>" +
        			"<ResponseID>myTest1</ResponseID>" +
        			// "<ResponseXML/>" +
        			"<GetListObjectResponseData>" + 
        				"<ObjectCount>" + String.valueOf(size) + "</ObjectCount>" +
        				"<ObjectLastWhereEndCount>" + String.valueOf(startFrom + size) + "</ObjectLastWhereEndCount>" +
    				"</GetListObjectResponseData>" + 
        		"</GetResponse>");
        responsedoc.getRootElement().add(data);
        return responsedoc.getRootElement();
	}
	
    private static String prefixContext(String childContext) {
        if (childContext == null)
            return "";
        childContext = childContext.trim();
        if (childContext.length() > 0) {
            if (childContext.equals(".")) {
                return "";
            } else if (!childContext.trim().startsWith("/")) {
                return "/" + childContext.trim();
            }
        }
        return childContext;
    }
    private static String getSelectClause(String varPrefix, String selectXml, String defaultSelectClause) throws Exception {
        if (selectXml == null || selectXml.equals(""))
            return defaultSelectClause;
        Document doc = MRXMLUtils.stringToDocument(selectXml);
        if (doc.getRootElement().getName().equals("xpath")) {
            String xpath = doc.getRootElement().getText();
            return " " + varPrefix + prefixContext(xpath) + " ";
        }
        List<?> list = MRXMLUtils.getListXpath(doc, "//xpath");
        Vector<Element> deleteList = new Vector<Element>();
        for (Iterator<?> iterator = list.iterator(); iterator.hasNext();) {
            try {
                Element el = (Element) iterator.next();
                String xpath = el.getText();
                Element parent = el.getParent();
                parent.addText("  " + varPrefix + prefixContext(xpath) + "       ");
                deleteList.add(el);
            } catch (Exception ex) {
                ex.printStackTrace();
            }
        }
        for (int i = 0; i < deleteList.size(); i++) {
            Element el = deleteList.get(i);
            el.detach();
        }
        deleteList.clear();
        deleteList = null;
        StringBuffer buf = new StringBuffer();
        buf.append(doc.getRootElement().asXML());
        return buf.toString();
    }
	
	
	@Override
	public Element readList(long DBCallTimeOut, MRRequestI request, MRLog log,
			MRDB db, String listName, String objectType, String NID,
			String childXpath, String xQueryPrefix, String[] arrWhereXpath,
			String[] sortByXpath, String sortOrder, String selectXML,
			Element appendToParentElement, int startFrom, int countToFetch)
			throws MRException {
		try {
			String jid = IpvsDBUtils.getUserJID(request, db); //subscriber JID 
			String elementXpath = childXpath;
	        if ( (selectXML != null && !selectXML.isEmpty())) {
	        	selectXML = selectXML.replace("$VAR", "$doc"); // ipvsdbservice feature
	            String selectClause = getSelectClause("$doc", selectXML, " return $doc ");
	            elementXpath = selectClause;
	        }
			String whereXpath = DBUtilsBase.convertArrayToXpath(arrWhereXpath);
			String sortXpath = DBUtilsBase.convertArrayToSortByXpath(sortByXpath);
			sortXpath = sortXpath.replace("$VAR", "$doc"); // ipvsdbservice feature
			
			List<Element> resultList = this.mrComponentDB.readElementList(getCollectionName(objectType, NID), NID, elementXpath, whereXpath, sortXpath, sortOrder, startFrom, countToFetch);
			Element listElement = MRXMLUtils.stringToElement("<ResponseXML/>");
			if(appendToParentElement != null)
				listElement = appendToParentElement;
			for(Element el : resultList)
				listElement.add(el);
			
			Element result = this.formatReadListResponse(listElement, appendToParentElement, objectType, startFrom, resultList.size());
			mrPubSub.subscribeNode(NID, jid, "", "");
			return result;
		}
		catch(Exception e) {
			e.printStackTrace();
			throw new MRException(MRExceptionCodes.DB_NOT_INITIALIZED, "MRDBApp:readList exception1 =" + e.toString());			
		}
	}

    private String tokenize(String sdp, String regex, String replacement) throws Exception {    	
		Matcher m = Pattern.compile(regex).matcher(sdp);
		if(!m.find())
			return sdp;
    	StringBuffer sb = new StringBuffer();
		m.appendReplacement(sb, replacement);
    	while(m.find()) {
    		m.appendReplacement(sb, replacement);
    	}
    	m.appendTail(sb);
		return sb.toString();
    }

	public String sednaTranslate(String query) {
		
		// This is simple since the collectionName is already present
		String patternString = "(index-scan\\(\\'IPVSis_colnids_default\\',)(\\s+)\\'(\\w+).(\\w+)',(\\s+)'EQ'\\)\\/..";
	    query = query.replaceAll(patternString, "collection('ipvs.$3.$4')/*");		
	    
 	   	// query = "for $d in index-scan('IPVSi_nids_default','default.tagobjectfilterlist.zzzz_1234465330250873f5e38', 'EQ')/.. return $d"; 

		// patternString = "(index-scan\\(\\'IPVSis_nids_default\\',)(\\s+)\\'(\\w+).(\\w+).(\\w+)',(\\s+)'EQ'\\)\\/..";
	    // query = query.replaceAll(patternString, "collection('ipvs.$3.$4')/*");		

	    return query;
	}

	@Override
	public String readQuery(long DBCallTimeOut, MRRequestI request, MRLog log,
			MRDB db, String query, Map<String, String> pemXPathList,
			Element appendResultToParentElement) throws MRException {
		String xquery = null;
		try {
			query = this.sednaTranslate(query);
			xquery = DBUtilsBase.updateNIDXPathList(query, pemXPathList, log, db);
			xquery = DBUtilsBase.convertXPaths( xquery);
			// System.out.println("MRAppDB:xquery query =" + xquery);
			/*  log.addInfo("########################### QUERY START ###########################");
		       log.addInfo(xquery);
		       log.addInfo("########################### QUERY END ###########################");*/
			return this.mrComponentDB.readQueryStringResult(xquery);
		}
		catch(Exception e) {
			System.out.println("MRAppDB:readQuery exception xquery=" + xquery);
			e.printStackTrace();
			throw new MRException(MRExceptionCodes.DB_NOT_INITIALIZED, "MRDBApp:readQuery exception=" + e.toString());
		}
	}

	@Override
	public boolean subscribe(long DBCallTimeOut, MRRequestI request, MRDB db,
			MRLog log, String listName, String objectType,
			String subscriptionParams) throws MRException {
		Element getSubParamsEL;
		try {
			getSubParamsEL = MRXMLUtils.stringToElement(subscriptionParams);
			String selectFormat = MRXMLUtils.getAttributeValue(getSubParamsEL, "selectFormat", "").trim();
			String jid = IpvsDBUtils.getUserJID(request, db); //subscriber JID
            String subscriptionId = MRXMLUtils.getValue(getSubParamsEL, "whereSubscriptionID", "");
            String whereNID = MRXMLUtils.getAttributeValue(getSubParamsEL, "whereNID", "").trim();
			return mrPubSub.subscribeNode(whereNID, jid, subscriptionId, selectFormat);
		} catch (Exception e) {
			throw new MRException(MRExceptionCodes.NODENOTFOUND, "MRDBApp:unsubscribe exception=" + e.toString());
		}
	}

	@Override
	public boolean updateSubscriptions(long DBCallTimeOut, MRRequestI request,
			MRDB db, MRLog log, String listName, String objectType,
			String subscriptionParams) throws MRException {
		Element subParamsEL;
		try {
			subParamsEL = MRXMLUtils.stringToElement(subscriptionParams);
			String jid = IpvsDBUtils.getUserJID(request, db); //subscriber JID
			String subscriptionId = MRXMLUtils.getValue(subParamsEL, "whereSubscriptionID", "");
			String whereNID = MRXMLUtils.getAttributeValue(subParamsEL, "whereNID", "").trim();
			List<String> nidList = null;
			if(!"null".equals(whereNID)&&!whereNID.isEmpty()) {
				String[] nidListArr = whereNID.split(",");
				nidList = Arrays.asList(nidListArr);
			}
			String topic = MRXMLUtils.getValue(subParamsEL, "topic", "");
			return mrPubSub.updateSubscriptions(jid, nidList, subscriptionId, topic);
		} catch (Exception e) {
			e.printStackTrace();
			throw new MRException(MRExceptionCodes.NODENOTFOUND, "MRDBApp:updateSubscriptions exception=" + e.toString());
		}
	}

	@Override
	public boolean unsubscribe(long DBCallTimeOut, MRRequestI request, MRDB db,
			MRLog log, String listName, String objectType,
			String unsubscribeParams) throws MRException {
		Element getSubParamsEL;
		try {
			getSubParamsEL = MRXMLUtils.stringToElement(unsubscribeParams);
			String jid = IpvsDBUtils.getUserJID(request, db); //subscriber JID
            String subscriptionId = MRXMLUtils.getValue(getSubParamsEL, "whereSubscriptionID", "");
            String whereNID = MRXMLUtils.getAttributeValue(getSubParamsEL, "whereNID", "").trim();
			return mrPubSub.unsubscribeNode(whereNID, jid, subscriptionId);
		} catch (Exception e) {
			throw new MRException(MRExceptionCodes.NODENOTFOUND, "MRDBApp:unsubscribe exception=" + e.toString());
		}
	}

	@Override
	public Document importDB(long DBCallTimeOut, MRRequestI request, MRDB db,
			MRLog log, String importDBName, String importDBParams)
			throws MRException {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public Document exportDB(long DBCallTimeOut, MRRequestI request, MRDB db,
			MRLog log, String exportDBName, String exportDBParams)
			throws MRException {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public Document deleteDB(long DBCallTimeOut, MRRequestI request, MRDB db,
			MRLog log, String type, String deleteDBParams) throws MRException {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public Document getSubscriptions(long DBCallTimeOut, MRRequestI request,
			MRLog log, MRDB db, String listName, String objectType, String JID,
			String selectFormat) throws MRException {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public Document getSubscribers(long DBCallTimeOut, MRRequestI request,
			MRLog log, MRDB db, String listName, String objectType, String NID,
			String getSubscribersParams) throws MRException {
		Element getSubParamsEL;
		try {
			getSubParamsEL = MRXMLUtils.stringToElement(getSubscribersParams);
			String selectFormat = MRXMLUtils.getAttributeValue(getSubParamsEL, "selectFormat", "").trim();
			return mrPubSub.getSubscribers(NID, selectFormat);
		} catch (Exception e) {
			throw new MRException(MRExceptionCodes.NODENOTFOUND, "MRDBApp:getSubscribers exception=" + e.toString());
		}
	}

}
