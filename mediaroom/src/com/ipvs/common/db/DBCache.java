package com.ipvs.common.db;

import java.util.Hashtable;
import java.util.Vector;

import org.dom4j.Element;

public class DBCache extends Hashtable<String, DBCache.CacheElement> {
 	private static final long serialVersionUID = 1L;
	private static Hashtable<String, DBCache> dbCacheList = null;
	private static DBCache dbCache = null;
	
	private boolean bclearOldDataRunning = false;
	private int maxCount;
	//private String hashname = null;
	private long nextttime ;
	private int skip_check ;
	private int clearRecOlderThanSec;
	private long lasttime ;
	public boolean ENABLE_DBCache=true;
	private Object sync = new Object();
 
	////////////////////////////////////////////// DBCache object //////////////////////////////////////////////
	private DBCache() {
		this("default");
	}
	private DBCache(String hashname) {
		//this.hashname = hashname;
		clearRecOlderThanSec = 100 * 1000;
		maxCount = 50;
		skip_check = 5 * 1000;
		nextttime = System.currentTimeMillis();
		lasttime = System.currentTimeMillis();
	}
 	//////////////////////////////////////////////////////////////////////////////////////////
	public Element addDBCache(String key, Element el) {
		try{
			if(!ENABLE_DBCache)
				return null;
			if (!bclearOldDataRunning)
				clearOldData();
			if (key != null && el != null) {
				if (super.size() > maxCount) {
					return null;
				}
				Element elClone = el; //already cloned
				super.put(key, new DBCache.CacheElement(key, elClone));
				return elClone;
			}
		} catch (Exception e) {
			e.printStackTrace();
		} 
		return null;
	}
	public Element getDBCache(String key) {
		try{
			if(!ENABLE_DBCache)
				return null;
			if (!bclearOldDataRunning)
				clearOldData();
			if (super.containsKey(key)) {
				CacheElement cacheElement = super.get(key);
				if ( cacheElement != null && !cacheElement.isExpired()) {
					return (Element) cacheElement.EL.clone();
				} else {
					//delete since expired
					super.remove(key);
				}
			}
		} catch (Exception e) {
			e.printStackTrace();
		} 		
		return null;
	}
	public Element removeDBCache(String key) {
		try{
			if(!ENABLE_DBCache)
				return null;
			if (super.containsKey(key)) {
				CacheElement cacheElement = super.remove(key);
				if ( cacheElement != null && !cacheElement.isExpired()) {
					//return if not expired
					return (Element) cacheElement.EL.clone();
				}
			}
		} catch (Exception e) {
			e.printStackTrace();
		} 		
		return null;
	}
	public  void clearOldData() {
		if(!ENABLE_DBCache)
			return; 
		
		if (bclearOldDataRunning)
			return;
		synchronized(sync) {
			bclearOldDataRunning = true;
			try {
			
				//do not check any thing for skip_check 5 Sec from last check 
				//nothing to clear
				long currenttime = System.currentTimeMillis();
				if (skip_check > (currenttime - lasttime) ) { 
					return;
				}
				if (clearRecOlderThanSec > (currenttime - nextttime) ) { 
					return;
				}
				lasttime = System.currentTimeMillis();
				nextttime =  System.currentTimeMillis();
				
				Vector<String> vec = new Vector<String>();
				DBCache.CacheElement cacheElement = null;
				Object [] objArray = super.keySet().toArray();
				if(objArray != null && objArray.length >0) {
					for (int i = 0; i < objArray.length; i++) {
						try {
							String key = (String)objArray[i];
							if(key !=null && super.containsKey(key)){
								cacheElement = super.get(key);
								if( cacheElement != null ){
									if (cacheElement.isExpired()) {
										try{
											super.remove(cacheElement.key);
										}catch(Exception ex){
											ex.printStackTrace();
										}
									} else if (cacheElement.createtime < nextttime) {
										nextttime = cacheElement.createtime;
									}
								}
							}
						} catch (Exception ex) {
							ex.printStackTrace();
						}
					}
				 }
				for (int index = 0; index < vec.size(); index++) {
					String key = vec.elementAt(index);
					removeDBCache(key);
				}
			} catch (Exception e) {
				e.printStackTrace();
			} finally {
				bclearOldDataRunning = false;
			}
		}
	}	
	// //////////////////////////////////////////// DBCache object
	class CacheElement {
		private Element EL = null;
		private Long createtime = null;
		private String key = null;
		CacheElement(String key, Element EL) {
			this.EL = EL;
			this.createtime = System.currentTimeMillis();
			this.key = key;
		}
		boolean isExpired() {
			try {
				long currenttime = System.currentTimeMillis();
				long diff = currenttime - createtime;
				if (clearRecOlderThanSec < diff) {
					return true;
				}
			} catch (Exception e) {
				e.printStackTrace();
			}
			return false;
		}
	}
	// //////////////////////////////////////////// DBCache getInstance
	public static DBCache getInstance() {
		if (dbCache == null) {
			dbCache = new DBCache();
		}
		return dbCache;
	}

	public static DBCache getInstance(String key) {
		if (dbCacheList == null) {
			dbCacheList = new Hashtable<String, DBCache>();
		}
		if (!dbCacheList.containsKey(key))
			dbCacheList.put(key, new DBCache(key));
		return dbCacheList.get(key);
	}
 
	
}
 