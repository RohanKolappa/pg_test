package com.barco.nimbus.api;

import java.util.ArrayList;

import org.dom4j.Document;
import org.dom4j.Element;

import com.barco.nimbus.api.common.SCException;

/**
 * Interface to be implemented by the DB Client
 */
public interface DBI {

	public static final int CONNECTED = 0;
	public static final int DISCONNECTED = 1;

	public static String ASCENDING = "ascending";
	public static String DESCENDING = "descending";

	public void registerListener(DBListenerI listener);

	/**
	 * @returns the NID to be used for a new document based on the collection it
	 *          needs to go into This shows the relationship between
	 *          NID,collectionName and id collectionName =
	 *          ipvs.default.mediastreamsrcportresourcelist NID =
	 *          default.mediastreamsrcportresourcelist
	 *          .mssprl_834a2a86-26cf-40f0-8d6b-6e1612b46a3f id =
	 *          mssprl_834a2a86-26cf-40f0-8d6b-6e1612b46a3f This enables the
	 *          search based on NID within the scope of the collection
	 *          identified by the NID
	 */
	public String createNIDFromCollectionName(String collectionName)
			throws SCException;

	/**
	 * @returns the state of the db connection
	 */
	public int getState();

	/**
	 * @return true if the object identified by the NID exists
	 */
	public boolean objectExists(String collectionName, String NID)
			throws SCException;

	/**
	 * Adds a the document to the database with the given NID
	 * 
	 * @return the id of the newly added object
	 */
	public String addDocument(String collectionName, String NID, Document doc)
			throws SCException;

	/**
	 * Adds an element identified by the parentElementXpath, within an existing
	 * document identified by the NID OR whereXpath
	 * 
	 * @return the array of NIDs of documents updated
	 */
	public ArrayList<String> addElement(String collectionName, String NID,
			String parentElementXpath, String whereXpath, Element dataElement)
			throws SCException;

	/**
	 * Adds an attribute to the element identified by the elementXpath, within
	 * an existing document identified by the NID OR whereXpath
	 * 
	 * @return the array of NIDs of documents updated
	 */
	public ArrayList<String> addAttribute(String collectionName, String NID,
			String elementXpath, String whereXpath, String attributeName,
			String attributeValue) throws SCException;

	/**
	 * Replaces a the document in the database with the given NID OR whereXpath
	 * 
	 * @return the id of the replacing object
	 */
	public String replaceDocument(String collectionName, String NID,
			String whereXpath, Document doc) throws SCException;

	/**
	 * Replaces an element identified by the elementXpath, within an existing
	 * document identified by the NID OR whereXpath
	 * 
	 * @return the array of NIDs of documents updated
	 */
	public ArrayList<String> replaceElement(String collectionName, String NID,
			String elementXpath, String whereXpath, Element dataElement)
			throws SCException;

	/**
	 * Replaces an attribute identified by the attributeXpath, within an
	 * existing document identified by the NID OR whereXpath
	 * 
	 * @return the array of NIDs of documents updated
	 */
	public ArrayList<String> replaceAttribute(String collectionName,
			String NID, String attributeXpath, String whereXpath, String data)
			throws SCException;

	/**
	 * Deletes a the document in the database with the given NID OR whereXpath
	 */
	public ArrayList<String> deleteDocument(String collectionName, String NID,
			String whereXpath) throws SCException;

	/**
	 * Deletes an element identified by the elementXpath, within an existing
	 * document identified by the NID OR whereXpath
	 * 
	 * @return the array of NIDs of documents updated
	 */
	public ArrayList<String> deleteElement(String collectionName, String NID,
			String elementXpath, String whereXpath) throws SCException;

	/**
	 * Deletes an attribute identified by the attributeXpath, within an existing
	 * document identified by the NID OR whereXpath
	 * 
	 * @return the array of NIDs of documents updated
	 */
	public ArrayList<String> deleteAttribute(String collectionName, String NID,
			String attributeXpath, String whereXpath) throws SCException;

	/**
	 * Reads a the document in the database with the given NID OR whereXpath
	 */
	public Document readSingleDocument(String collectionName, String NID,
			String whereXpath) throws SCException;

	/**
	 * Reads an element identified by the elementXpath, within an existing
	 * document identified by the NID OR whereXpath
	 */
	public Element readSingleElement(String collectionName, String NID,
			String elementXpath, String whereXpath) throws SCException;

	/**
	 * Reads a list of countToFetch elements identified by the elementXpath,
	 * within an existing document identified by the NID OR whereXpath as per
	 * the sort and start criteria
	 */
	public ArrayList<Element> readElementList(String collectionName,
			String NID, String elementXpath, String whereXpath,
			String sortByXpath, String sortOrder, int startFrom,
			int countToFetch) throws SCException;

	/**
	 * @returns a string result as per the query
	 */
	public String readQueryStringResult(String xquery) throws SCException;

	/**
	 * @returns a list of documents as per the query
	 */
	public ArrayList<Document> readQueryDocumentList(String xquery)
			throws SCException;

	/**
	 * imports the db files into the active database
	 */
	public void importDB(String importDBName, String path) throws SCException;

	/**
	 * exports the active database to do files
	 */
	public void exportDB(String exportDBName, String path) throws SCException;

	/**
	 * deletes a saved db
	 */
	public void deleteDB(String type, String deleteDBParams) throws SCException;

	/*
	 * 
	 * // List of methods required for backwards compatiblity with the IPVS
	 * codebase
	 * 
	 * public Element getMRSchema(String schemaPath) throws SCException;
	 * 
	 * public void initDB(String ipvsRootNID, String serviceDomain, String
	 * schemaPath, boolean reset) throws SCException;
	 * 
	 * public boolean objectExists(String listName, String objectType, String
	 * NID, String childXpath, String xQueryPrefix) throws SCException;
	 * 
	 * public Document add(String listName, String objectType, String NID,
	 * String childXpath, String xQueryPrefix, String[] arrWhereXpath, Element
	 * dataElement) throws SCException;
	 * 
	 * public Document replace(String listName, String objectType, String NID,
	 * String childXpath, String xQueryPrefix, String[] arrWhereXpath, Element
	 * dataElement) throws SCException;
	 * 
	 * public Document replaceAttribute(String listName, String objectType,
	 * String NID, String childXpath, String xQueryPrefix, String[]
	 * arrWhereXpath, String data) throws SCException ;
	 * 
	 * public Document delete(String listName, String objectType, String NID,
	 * String childXpath, String xQueryPrefix, String[] arrWhereXpath) throws
	 * SCException;
	 * 
	 * public Element readSingle(String listName, String objectType, String NID,
	 * String childXpath, String xQueryPrefix, String[] arrWhereXpath, String[]
	 * sortByXpath, String sortOrder, String selectXML, Element
	 * appendToParentElement) throws SCException;
	 * 
	 * public Element readList(String listName, String objectType, String NID,
	 * String childXpath, String xQueryPrefix, String[] arrWhereXpath, String[]
	 * sortByXpath, String sortOrder, String selectXML, Element
	 * appendToParentElement, int startFrom, int countToFetch) throws
	 * SCException;
	 * 
	 * public String readQuery(String query, Map<String, String> pemXPathList,
	 * Element appendResultToParentElement) throws SCException;
	 */

}
