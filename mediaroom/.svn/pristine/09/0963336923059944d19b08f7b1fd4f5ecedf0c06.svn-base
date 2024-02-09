package com.ipvs.policyadminservice.impl;

import java.util.ArrayList;
import java.util.Hashtable;
import java.util.Iterator;
import java.util.List;

import org.dom4j.Element;

import com.ipvs.assetadminservice.handler.AMConst;
import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.db.IpvsDBUtils;
import com.ipvs.common.db.MRConfig;
import com.ipvs.common.db.MRXMLDBUtils;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.config.IPVSConst;
import com.ipvs.policyadminservice.utils.PolicyAdminConstants;
import com.ipvs.utils.FileUtils;


public class PEMDBUtils {
	private  Hashtable<String, String> parentNIDWatchList = null;
    private  Hashtable<String, String> nodeIDWatchList = null;
    private static PEMDBUtils pemObject = null;
    private Hashtable<String, Element> policyObjectTable = null;
    private Hashtable<String, Boolean> policyCacheTable = null;
    private List<String> objectTypeList = null;
    private boolean isInitialized = false;
    //private XMPPI xmpp = null;
    
	public static String XQUERYLIB = "/com/ipvs/policyadminservice/xquery/XQueryLib.xml";
	private static String URLSCHEMEQUERY = "/com/ipvs/policyadminservice/xquery/URLSchemeQuery.xml";
	private static final String NATFILTERLISTQUERY = "/com/ipvs/policyadminservice/xquery/NATFilterListQuery.xml";
	
    public PEMDBUtils() {
    	
    }
    public PEMDBUtils(XMPPI xmpp) {
        intiallizeObjectList();

        if (policyCacheTable == null) {
            policyCacheTable = new Hashtable<String, Boolean>();
        }

        Iterator<String> objListIt = objectTypeList.iterator();
        String objectType = null;
        Boolean dirtyFlag = Boolean.valueOf(true);

        while (objListIt.hasNext()) {
            objectType = objListIt.next();
            setPolicyDirtyFlag(objectType, dirtyFlag);
        }
        //this.xmpp = xmpp;
    }

    public static PEMDBUtils getInstatnce(XMPPI xmpp) {
        if (pemObject == null) {
            pemObject = new PEMDBUtils(xmpp);
        }

        return pemObject;
    }

    @SuppressWarnings("unchecked")
    public Element getMatchingNodes(String parentElementNID, String objectType, String[] arrXpath, MRRequest request, MRDB db, MRLog log)
        throws Exception {
        Element response = null;
        boolean dirtyFlag = getPolicyDirtyFlag(objectType);
        List<?> objectList = null;

    	if(!isInitialized) {
            intializeWatchList(request, db, log, db.getXMPPI());            
    	}
        if (dirtyFlag) {
            log.addInfo("Dirty Node objectType=" + objectType);
            log.addInfo("Sync the Cache");
            objectList = getData(parentElementNID, objectType, request, db, log);
        }

        Element objectEL = getPolicyObject(objectType);

        if (objectEL != null) {
            objectList = new ArrayList<Object>(objectEL.elements(objectType));

            if (objectList == null) {
                objectList = getData(parentElementNID, objectType, request, db, log);
            }
        }

        if ((objectList != null) && (objectList.size() != 0)) {
            if ((arrXpath != null) && (arrXpath.length != 0)) {
                response = getMatchingNodesBasedOnXPath(objectList, arrXpath, objectType, request, db, log);            
            } else {            	
                response = objectEL;
            }
        }       
       
        if (response == null) {
            return null;
        }
      
        return response;
    }

    public List<?> refreshCache(String objectType, MRRequest request, MRDB db, MRLog log, MRConfig config)
        throws MRException, Exception {
        String parentNID = db.getServiceDomainRootNID(config.getParentNIDKey(log));
        Element objectEL = getPolicyObject(objectType);
        List<?> objList = null;

        if (objectEL == null) {
            return null;
        }

        objList = getData(parentNID, objectType, request, db, log);

        return objList;
    }

    private void intiallizeObjectList() {
        if (objectTypeList == null) {
            objectTypeList = new ArrayList<String>();
        }
        objectTypeList.add(PolicyAdminConstants.TAG_OBJECT_FILTER_OBJECTYPE);
        objectTypeList.add(PolicyAdminConstants.OBJECT_FILTER_OBJECTYPE);
        objectTypeList.add(PolicyAdminConstants.ASSET_ADMIN_POLICY_OBJECTTYPE);
        objectTypeList.add(PolicyAdminConstants.POLICY_ADMIN_POLICY_OBJECTTYPE);
        objectTypeList.add(PolicyAdminConstants.USER_ADMIN_POLICY_OBJECTTYPE);
        objectTypeList.add(PolicyAdminConstants.MEDIA_STORE_ADMIN_POLICY_OBJECTTYPE);
        objectTypeList.add(PolicyAdminConstants.MEDIA_STREAM_FILTER_OBJECTTYPE);
        objectTypeList.add(PolicyAdminConstants.MEDIA_ROOM_POLICY_OBJECTTYPE);
        objectTypeList.add(AMConst.MEDIA_STREAM_PROFILE_OBJECTTYPE);
        objectTypeList.add(PolicyAdminConstants.NETWORK_ADDRESS_RULE);
        
    }

    public List<String> getPolicyObjectList() {
        return objectTypeList;
    }

    public Element getDataFromDB(String parentElementNID, String objectType, MRRequest request, MRDB db, MRLog log)
        throws MRException, Exception {
        Element objectEL = null;
        objectEL = MRXMLUtils.newElement(objectType + "List");
        MRXMLDBUtils.findChildElements(parentElementNID, objectEL, null, 1, null, null, request, db, log);

        return objectEL;
    }

    @SuppressWarnings("unchecked")
    public List<?> getData(String parentElementNID, String objectType, MRRequest request, MRDB db, MRLog log)
        throws MRException, Exception {
        Element objectEL = null;
        //objectEL = MRXMLUtils.newElement(objectType + "List");
        objectEL = null;

        List<?> objectList = null;

        objectEL = getDataFromDB(parentElementNID, objectType, request, db, log);

        objectList = new ArrayList<Object>(objectEL.elements(objectType));

        setPolicyObject(objectType, objectEL);
        setPolicyDirtyFlag(objectType, false);

        return objectList;
    }

    @SuppressWarnings("unchecked")
    private Element getMatchingNodesBasedOnXPath(List<?> objectList, String[] whereXPath, String objectType, MRRequest request, MRDB db, MRLog log)
        throws MRException, Exception {
        List<Element> matchingObjectList = new ArrayList<Element>();

        for (Iterator<?> objectIT = objectList.iterator(); objectIT.hasNext();) {
            Element object = (Element) objectIT.next();

            boolean bValidateXpath = IpvsDBUtils.validateXpath(object, whereXPath, request, db, log);  
            //System.out.println("object-->"+MRXMLUtils.elementToString(object));
            if (bValidateXpath) {               	
                matchingObjectList.add((Element) object.clone());
            }
        }

        if ((matchingObjectList != null) && (matchingObjectList.size() != 0)) {
            Element objEL = MRXMLUtils.newElement(objectType + "List");
            objEL.content().addAll(matchingObjectList);

            return objEL;
        }

        return null;
    }
    
    
    public void intializeWatchList(MRRequest request, MRDB db, MRLog log, XMPPI xmpp) throws Exception {
        /*if (getParentNIDWatchList() == null) {
        	setParentNIDWatchList(new Hashtable<String, String>());
        }*/
    	
    	if (parentNIDWatchList == null) {
        	parentNIDWatchList = new Hashtable<String, String>();
        }

        //Execute the below query if it is not latest pubsub
        if (!IPVSConst.getInstance().isEnableNewPubSub()) {
            if (nodeIDWatchList == null) {
            	nodeIDWatchList = new Hashtable<String, String>();
            }
        }

        Iterator<String> objListIt = getPolicyObjectList().iterator();
        String objectType = null;
        String parentNID = null;
        List<?> objNodeList = null;

        while (objListIt.hasNext()) {
            objectType = objListIt.next();
            parentNID = db.getServiceDomainRootNID(getMRConfig(objectType,log).getParentNIDKey(log));
            //getParentNIDWatchList().put(parentNID, objectType);
            parentNIDWatchList.put(parentNID, objectType);

            try {
                //xmpp.PubSub_Subscribe(parentNID);
                MRXMLDBUtils.subscribe(request, db, log, parentNID, "", "");
            } catch (Exception e) {
                e.printStackTrace();
            }

            objNodeList = getData(parentNID, objectType, request, db, log);

            //Execute the below query if it is not latest pubsub
            if (!IPVSConst.getInstance().isEnableNewPubSub()) {
                if ((objNodeList == null) || (objNodeList.size() == 0)) {
                    continue;
                }

                Iterator<?> objNodeListIT = objNodeList.iterator();
                Element objEL = null;

                while (objNodeListIT.hasNext()) {
                    objEL = (Element) objNodeListIT.next();
                    nodeIDWatchList.put(MRXMLUtils.getAttributeValue(objEL, "NID", ""), objEL.getName());
                    
                }
            }
        }
        isInitialized = true;        
    }

    /**
     * @return the policyObjectTable
     */
    /*public Hashtable<String, Element> getPolicyObjectTable() {
        if (policyObjectTable == null) {
            policyObjectTable = new Hashtable<String, Element>();
        }

        return policyObjectTable;
    }*/

    /*private void updatePolicyObject(String objectType, Element objectEL) {
        if (policyObjectTable == null) {
            policyObjectTable = new Hashtable<String, Element>();
        }
    
        policyObjectTable.remove(objectType);
        policyObjectTable.put(objectType, objectEL);
    }*/
    private void setPolicyObject(String objectType, Element objectEL) {
        if (policyObjectTable == null) {
            policyObjectTable = new Hashtable<String, Element>();
        }

        policyObjectTable.put(objectType, objectEL);
    }

    private Element getPolicyObject(String objectType) {
        if (policyObjectTable == null) {
            return null;
        }

        Element objectEL = policyObjectTable.get(objectType);

        if (objectEL == null) {
            return null;
        }

        return objectEL;
    }

    public synchronized boolean getPolicyDirtyFlag(String objectType) {
        /////////new pub sub unitl sub/////
    	if(IPVSConst.getInstance().isEnableNewPubSub()){
            if(IPVSConst.getInstance().isDisablePolicyCatche()){
            	return true;
            }        	
        }
        if (policyCacheTable == null) {
            return true;
        }

        Boolean dirtyflag = policyCacheTable.get(objectType);

        if (dirtyflag == null) {
            return true;
        }

        return dirtyflag.booleanValue();
    }

    public synchronized void setPolicyDirtyFlag(String objectType, Boolean dirtyFlag) {
        if (policyCacheTable == null) {
            return;
        }

        policyCacheTable.put(objectType, dirtyFlag);
    }
    
    private MRConfig getMRConfig(String objectType, MRLog log) throws Exception, MRException {
    	
    	String serviceName = MRConfig.getObjectServiceName(objectType);
    	return MRConfig.getInstance(serviceName, objectType, log);
    }
	public Hashtable<String, String> getParentNIDWatchList() {
		if(parentNIDWatchList!=null) {
			Hashtable<String,String> tempParentNIDWatchList = new Hashtable<String, String>();
			tempParentNIDWatchList.putAll(parentNIDWatchList);
			return tempParentNIDWatchList;
		}
		else {
			return null;
		}
		
	}
	
	public Hashtable<String, String> getNodeIDWatchList() {
		if(nodeIDWatchList!=null) {
			Hashtable<String,String> tempNodeIDWatchList = new Hashtable<String, String>();
			tempNodeIDWatchList.putAll(nodeIDWatchList);
			return tempNodeIDWatchList;
		}
		else {
			return null;
		}
		
	}
	/*public void setParentNIDWatchList(Hashtable<String, String> parentNIDWatchList) {
		this.parentNIDWatchList = parentNIDWatchList;
	}
	public void setNodeIDWatchList(Hashtable<String, String> nodeIDWatchList) {
		this.nodeIDWatchList = nodeIDWatchList;
	}*/
    
	public static String getURLSchemeFilter(MRRequest request, XMPPI xmpp, MRLog log, MRDB db) throws Exception {
    	long dbtimeout = MRXMLDBUtils.TIME_OUT;
    	Hashtable<String, String> table = new Hashtable<String, String>();
    	// Document doc = MRXMLUtils.loadXMLResource(GetNATXPathRequestHandler.NATXQUERY, log);
 	   	// String query = MRXMLUtils.getValueXpath(doc, "//NATXQuery", "");
 	   	// query = "for $d in index-scan('IPVSis_colnids_default', 'default.mediastreamfilterlist', 'EQ')/.. return $d";
 	   	// query = "for $d in index-scan('IPVSi_nids_default','default.tagobjectfilterlist.zzzz_1234465330250873f5e38', 'EQ')/.. return $d"; 
    	String query = 	FileUtils.readFileResource(PEMDBUtils.XQUERYLIB, request.getClass()) +
    					FileUtils.readFileResource(PEMDBUtils.URLSCHEMEQUERY, request.getClass());
    	return MRXMLDBUtils.readQuery(dbtimeout, request, log, db, query, table, null);
	}
	
	public static String getNATFilterList(MRRequest request, XMPPI xmpp, MRLog log, MRDB db) throws Exception {
    	long dbtimeout = MRXMLDBUtils.TIME_OUT;
    	Hashtable<String, String> table = new Hashtable<String, String>();
    	String query = 	FileUtils.readFileResource(PEMDBUtils.XQUERYLIB, request.getClass()) +
    					FileUtils.readFileResource(PEMDBUtils.NATFILTERLISTQUERY, request.getClass());
    	return MRXMLDBUtils.readQuery(dbtimeout, request, log, db, query, table, null);
	}
}
