package com.barco.nimbus.database.exist;

import javax.xml.transform.OutputKeys;

import org.apache.log4j.Logger;
import org.dom4j.Document;
import org.exist.xmldb.DatabaseImpl;
import org.xmldb.api.DatabaseManager;
import org.xmldb.api.base.Collection;
import org.xmldb.api.base.Database;
import org.xmldb.api.base.ResourceSet;
import org.xmldb.api.base.XMLDBException;
import org.xmldb.api.modules.CollectionManagementService;
import org.xmldb.api.modules.XMLResource;
import org.xmldb.api.modules.XPathQueryService;

public class ExistClient {
	
	public final static String URI = "xmldb:exist://SERVER/exist/xmlrpc/db";
	private Collection root = null;
	private boolean DEBUG = false;
	
	/*
	 * @return the collection corresponding to the name
	 * This allows the method to be refined in case we did not want to hold onto the root collection
	 */
	private Collection getCollection(String collectionName) throws Exception{
		return root.getChildCollection(collectionName);
	}
	
	/*
	 * Login to the existDB server and get the rootCollection
	 */
	public void login(String serverIP, int serverPort, String user, String password) throws Exception {
		this.registerDriver();
		String rootURI = URI.replace("SERVER", serverIP + ":" + String.valueOf(serverPort));
		root = DatabaseManager.getCollection(rootURI, user, password);
		DatabaseManager.setProperty("uri", rootURI);
		DatabaseManager.setProperty("user", user);
		DatabaseManager.setProperty("password", password);
		root.setProperty(OutputKeys.INDENT, "no");
	}
	
	/*
	 * Logout of the existDB server and release the rootCollection
	 */
	public void logout() {
		try {
			root.close();
		} catch (Exception e) {
			Logger.getLogger(getClass()).error("Error while logout." + e);
		}
	}
	
	
	/*
	 * Registers the ExistDB driver with the xmldb DatabaseManager
	 */
	private void registerDriver() throws Exception {
		Database database = new DatabaseImpl();
		database.setProperty("create-database", "true");
		DatabaseManager.registerDatabase(database);
		Logger.getLogger(ExistClient.class).debug("ExistClient:registerDriver");
	}
	
	/*
	 * Reads the document by id
	 * @returns Document corresponding to the id
	 */
	public XMLResource readById(String collectionName, String id) throws Exception {
		Collection col = this.getCollection(collectionName);
		XMLResource res = (XMLResource)col.getResource(id);
		if(DEBUG) {
			if( (res != null) && (res.getContent() != null))
				Logger.getLogger(ExistClient.class).debug("ExistClient:readById=" + res.getContent());
			else
				Logger.getLogger(ExistClient.class).debug("ExistClient:readById not found");
		}
		return res;
	}
	
	/*
	 * Adds the document by id
	 * @doc : dom4j Document
	 */
	public String addbyId(String collectionName, String id, Document doc) throws Exception {
		Collection col = this.getOrCreateCollection(collectionName);
		XMLResource res = (XMLResource) col.createResource(id, "XMLResource");
		if (id == null)
			id = res.getId();
		res.setContent(doc.asXML());
		col.storeResource(res);
		if(DEBUG)
			Logger.getLogger(ExistClient.class).debug("ExistClient:add id=" + id + " doc=" + doc.asXML());
		return id;
	}

	private Collection getOrCreateCollection(String collectionUri) throws XMLDBException {
		String rootURI = DatabaseManager.getProperty("uri");
		String user = DatabaseManager.getProperty("user");
		String password = DatabaseManager.getProperty("password");
		collectionUri= "/"+collectionUri;
		Collection col = DatabaseManager.getCollection(rootURI + collectionUri, user, password);	
		if (col == null) {			
			// collection does not exist, so create
			String parentPath = collectionUri.substring(0, collectionUri.lastIndexOf("/"));				
			Collection parent = DatabaseManager.getCollection(rootURI + parentPath, user, password);					
			CollectionManagementService mgt = (CollectionManagementService) parent.getService("CollectionManagementService", "1.0");
			col = mgt.createCollection(collectionUri.substring(collectionUri.lastIndexOf("/"),collectionUri.length()));
			col.close();
			parent.close();				
		}
		if (root == null)
			root = DatabaseManager.getCollection(rootURI, user, password);
		return col;
	}

	/*
	 * Deletes the document identified by the id
	 */
	public XMLResource deleteById(String collectionName, String id) throws Exception {
		Collection col = this.getCollection(collectionName);
		XMLResource res = (XMLResource)col.getResource(id);
		if(DEBUG)
			Logger.getLogger(ExistClient.class).debug("ExistClient:delete=" + res.getContent());
		if (res != null)
			col.removeResource(res);
		return res;
	}


	/*
	 * Executes the xquery
	 * 
	 * @returns : the result of the XQuery
	 */
	public ResourceSet readByXQuery(String xquery) throws Exception  {
		XPathQueryService xpqs = (XPathQueryService)root.getService("XPathQueryService", "1.0");
		xpqs.setProperty("indent", "yes");
		ResourceSet result = xpqs.query(xquery);
		if(DEBUG)
			Logger.getLogger(ExistClient.class).debug("ExistClient:readByXQuery2 :" + result.toString());
		return result;
	}
	


}
