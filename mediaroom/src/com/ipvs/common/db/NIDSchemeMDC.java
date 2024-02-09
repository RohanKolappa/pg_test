package com.ipvs.common.db;

public class NIDSchemeMDC {
	public static final int COLLECTION = 1;
	public static final int DOCUMENT= 2;
	public static final int UNKNOWN= -1;

	private String domain = "";
	private String collectionId = "";
	private String docUri = "";
	private int type = UNKNOWN;
	
	// TODO: define types

	public NIDSchemeMDC() {
	}
	public String getNID() {
		if(type==COLLECTION)
			return getCollectionNodeID();
		if(type==DOCUMENT)
			return getDocNodeID();
		return null;
		
	}
	public String getDocNodeID() {
		return domain + "." + collectionId + "." + docUri;
	}
	public String getCollectionNodeID() {
		return domain + "." + collectionId;
	}
	public String getDomain() {
		return domain;
	}

	public String getCollectionId() {
		return collectionId;
	}

	public String getDocUri() {
		return docUri;
	}

	public int getType() {
		return type;
	}
	public String getNodePrefix_add(){
	    if(type==COLLECTION)
            return getCollectionNodeID();
	    return null;
	}	
	public void reset() {
	    domain = "";
	    collectionId = "";
	    docUri = "";
	    type = UNKNOWN;
	}
	public void parse(String nodeID) throws Exception {
	    reset();
		if (nodeID == null || nodeID.equals(""))
			throw new Exception("Unknown NID ='"+nodeID+"'");
		String[] array = nodeID.split("\\.");
		int length = array.length;
		switch (length) {
		case 2:
			// schema node come here
			domain = array[0].trim();
			collectionId = array[1].trim();
			type =COLLECTION;
			break;
		case 3:
			// doc node come here
			domain = array[0].trim();
			collectionId = array[1].trim();
			docUri = array[2].trim();
			type = DOCUMENT;
			break;
		default:
		    throw new Exception("Unknown NID ='"+nodeID+"'");		
		}
	}
	
}
