package com.ipvsserv.common.mdc;

import com.ipvsserv.common.config.IpvsServGlobals;

/**
 * @author pravin reddy
 * Email: pravin@ipvideosys.com
 * IP Video Systems , CA, USA
 * http://www.ipvideosys.com 
 * @since 1.0.0
 */
public class NIDSchemeMDC {
    public static final int COLLECTION = 2;
    public static final int DOCUMENT = 3;
    public static final int DOMAIN = 1;
    public static final int UNKNOWN = -1;
    
    private String domain = null;
    private String collectionId = "";
    private String docUri = "";
    private int type = NIDSchemeMDC.UNKNOWN;
    
    // TODO: define types
    
    public NIDSchemeMDC() {}
	public String getNID() {
		if(type==COLLECTION)
			return getCollectionNodeID();
		if(type==DOCUMENT)
			return getDocNodeID();
		return null;
		
	}
    public String getDocNodeID() {
        if(type==UNKNOWN){
            return null;
        }
        return getDomain() + "." + collectionId + "." + docUri;
    }
    
    public String getCollectionNodeID() {
        if(type==UNKNOWN){
            return null;
        }
        return getDomain() + "." + collectionId;
    }
    
    public String getDomain() {
       if (domain == null)
            domain = IpvsServGlobals.getDomainDefault();
        return domain;
    }
    
    public void setDomain(String domain) {
        type = NIDSchemeMDC.UNKNOWN;
        this.domain = domain;
    } 
    
    public String getCollectionId() {
        return collectionId;
    }
    
    public String getCollectionName() {
        if(IpvsServGlobals.isUseListCol()){
            if(type==UNKNOWN){
                return null;
            }
            return IpvsServGlobals.getListColPrefix()+getCollectionNodeID();
        }else{
            return IpvsServGlobals.getDomainDBSchema().getCollectionName("data", getDomain());
        }
    }
    public String getCollectionNameSCol() {
        //hack for import /export file metadata support
        return IpvsServGlobals.getDomainDBSchema().getCollectionName("data", getDomain());
    }
    public String getIndexName() {
        return IpvsServGlobals.getDomainDBSchema().getIndexName("nodeIndex", getDomain());
    }

    public String getIndexNameCol() {
        return IpvsServGlobals.getDomainDBSchema().getIndexName("nodeColIndex", getDomain());
    }

    public String getDocIdSub() {
        return IpvsServGlobals.getDomainDBSchema().getDomainsPropterty(IpvsServGlobals.DB_SUB_DOC_PREFIX) + getDomain();
    }

    public String getCollectionNameSub() {
        return IpvsServGlobals.getDomainDBSchema().getCollectionName("schema", getDomain());
    }    
    
    public String getDocUri() {
        return docUri;
    }
    
    public int getType() {
        return type;
    }
    
    public void parse(String nodeID) {
        if (nodeID == null || nodeID.equals("")) {
            return;
        }
        nodeID=nodeID.trim();
        String[] array = nodeID.split("\\.");
        int lenth = array.length;
        switch (lenth) {
            case 0:
                // should not come here
                break;
            case 1:
                // should not come here
                domain = array[0].trim();
                type = NIDSchemeMDC.DOMAIN;
                break;
            case 2:
                // schema node come here
                domain = array[0].trim();
                collectionId = array[1].trim();
                type = NIDSchemeMDC.COLLECTION;
                break;
            case 3:
                // doc node come here
                domain = array[0].trim();
                collectionId = array[1].trim();
                docUri = array[2].trim();
                type = NIDSchemeMDC.DOCUMENT;
                break;
            default:
                type = NIDSchemeMDC.UNKNOWN;
                break;
        }
    }
    
}
