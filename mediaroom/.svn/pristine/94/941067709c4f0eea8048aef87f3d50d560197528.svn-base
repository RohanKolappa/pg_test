package com.ipvs.common.service;

import java.util.Enumeration;
import java.util.Hashtable;

import org.apache.commons.lang.StringUtils;
import org.dom4j.Document;

import com.ipvs.common.MRException;
import com.ipvs.common.MRExceptionCodes;
import com.ipvs.common.MRLog;
import com.ipvs.mediaroomservice.impl.AppServerAgent;

public class UserManager {
	private Hashtable<String, String> userTable;
	private final int CACHE_SIZE = 100;
	private UserManager() {
		userTable = new Hashtable<String, String>();
	}
	
	public static UserManager getInstance() {
		return new UserManager();
	}
	
	public synchronized Boolean checkJID(String userJID, MRLog log, MRDB db, String resource) throws Exception {
		
		// Look up JID in cache
		String lastAccessed = userTable.get(userJID);
		if (lastAccessed == null) {
     		// Look up in DB if JID is not found in cache.
	    	dbLookupJID(userJID, log, db, resource);
		}
		
		// Add or update JID
		addJID(userJID);
		
		return true;
	}
	
	//Look up JID in DB
	private void dbLookupJID(String jid, MRLog log, MRDB db, String resource) throws Exception {
//		System.out.println("===================== reading from database ================================");
		Document resourceDoc = MRClientUtils.readAssetResourceInternal(jid, log, db, resource);
		if(resourceDoc == null) {
			throw new MRException(MRExceptionCodes.RESOURCE_AUTHERIZATION_FAILED,
					"Failed to retrieve Resource information. ");
		}

		String owner = MRXMLUtils.getValueXpath(resourceDoc, 
										AppServerAgent.ASSETRESOURCE_OWNER_XPATH, "").trim();
		if(StringUtils.isEmpty(owner)) {
			throw new MRException(MRExceptionCodes.RESOURCE_AUTHERIZATION_FAILED,
					"Resource contact owner null or empty. ");
		}
		if(!owner.equals(jid)) {
			throw new MRException(MRExceptionCodes.RESOURCE_AUTHERIZATION_FAILED,
					String.format("Resource owner mismatch: %s (actual), %s (supplied).", owner, jid ));
		}
		
		String state = MRXMLUtils.getValueXpath(resourceDoc, 
										AppServerAgent.ASSETRESOURCE_OWNERSTATE_XPATH, "").trim();
		if(!(state.equals(MRClientUtils.ASSET_STATE_READY) || 
				state.equals(MRClientUtils.ASSET_STATE_NOT_READY))) {
			throw new MRException(MRExceptionCodes.RESOURCE_AUTHERIZATION_FAILED,
					"Invalid resource state for authorization: " + state);
		}
		
	}
	
	private void addJID(String jid) {
		long lastAccessed = System.currentTimeMillis();
		userTable.put(jid, String.valueOf(lastAccessed));
		if(userTable.size() < CACHE_SIZE)
			return;
		
		expireJID(jid);
	}
	
	//ExpireJID method removes longest JID
	private void expireJID(String jid) {
		Long smallestValue = new Long(userTable.get(jid));
		String expireJID = jid;
		for(Enumeration<String> e = userTable.keys(); e.hasMoreElements();) {
			String key = e.nextElement();
			Long value = new Long(userTable.get(key));
			if(smallestValue > value) {
				expireJID = key;
				smallestValue = value;
			}
		}
		removeJID(expireJID);
	}
	
	public synchronized void removeJID(String jid) {
		
		//Removes JID
		userTable.remove(jid);
	}

}
