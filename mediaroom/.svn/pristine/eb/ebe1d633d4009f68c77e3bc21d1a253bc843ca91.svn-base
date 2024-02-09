package com.ipvs.nimbus;


import java.util.ArrayList;

import org.dom4j.Document;
import org.dom4j.Element;

/*
 * Interface to be implemented by the DB Client
 */
public interface MRDBI {
    
	public static final int CONNECTED = 0;
	public static final int DISCONNECTED = 1;
	
	public static String ASCENDING = "ascending";
	public static String DESCENDING = "descending";


	/*
	 * @returns the state of the db connection
	 */
	public int getState();
	
	/*
	 * Login to the db and initialize the db connection
	 */
	public void login(String serverIP, int serverPort, String user, String password, String schemePath) throws MRSCException,Exception;
	
	/*
	 * Logout of the the db and teardown the db connection
	 */
	public void logout() throws MRSCException,Exception;

	/*
	 * @return true if the object identified by the NID exists
	 */
    public boolean objectExists(String collectionName, String NID) throws MRSCException;

    /*
     * Adds a the document to the database with the given NID
     * @return the id of the newly added object
     */
    public String addDocument(String collectionName, String NID, Document doc) throws MRSCException;

    /*
     * Adds an element identified by the parentElementXpath, within an existing document identified by the NID OR whereXpath
     * @return the array of ids of documents updated
     */
    public ArrayList<String> addElement(String collectionName, String NID, String parentElementXpath, String whereXpath, Element dataElement) throws MRSCException;

    /*
     * Adds an attribute to the element identified by the elementXpath, within an existing document identified by the NID OR whereXpath
     * @return the array of ids of documents updated
     */
    public ArrayList<String> addAttribute(String collectionName, String NID, String elementXpath, String whereXpath, String attributeName, String attributeValue) throws MRSCException;

    /*
     * Replaces a the document in the database with the given NID OR whereXpath
     * @return the id of the replacing object
     */
    public String replaceDocument(String collectionName, String NID, String whereXpath, Document doc) throws MRSCException;

    /*
     * Replaces an element identified by the elementXpath, within an existing document identified by the NID OR whereXpath
     * @return the array of ids of documents updated
     */
    public ArrayList<String> replaceElement(String collectionName, String NID, String elementXpath, String whereXpath, Element dataElement) throws MRSCException;
    
    /*
     * Replaces an attribute identified by the attributeXpath, within an existing document identified by the NID OR whereXpath
     * @return the array of ids of documents updated
     */
    public ArrayList<String> replaceAttribute(String collectionName, String NID, String attributeXpath, String whereXpath, String data) throws MRSCException ;

    
    /*
     * Deletes a the document in the database with the given NID OR whereXpath
     */
    public String deleteDocument(String collectionName, String NID, String whereXpath) throws MRSCException;

    /*
     * Deletes an element identified by the elementXpath, within an existing document identified by the NID OR whereXpath
     * @return the array of ids of documents updated
     */
    public ArrayList<String> deleteElement(String collectionName, String NID, String elementXpath, String whereXpath) throws MRSCException;

    /*
     * Deletes an attribute identified by the attributeXpath, within an existing document identified by the NID OR whereXpath
     * @return the array of ids of documents updated
     */
    public ArrayList<String> deleteAttribute(String collectionName, String NID, String attributeXpath, String whereXpath) throws MRSCException ;


    /*
     * Reads a the document in the database with the given NID OR whereXpath
     */
    public Document readSingleDocument(String collectionName, String NID, String whereXpath) throws MRSCException;

    /*
     * Reads an element identified by the elementXpath, within an existing document identified by the NID OR whereXpath
     */
    public Element readSingleElement(String collectionName, String NID, String elementXpath, String whereXpath) throws MRSCException;

    /*
     * Reads a list of countToFetch elements identified by the elementXpath, within an existing document identified by the NID OR whereXpath as per the sort and start criteria
     */
    public ArrayList<Element> readElementList(String collectionName, String NID, String elementXpath, String whereXpath, String sortByXpath, String sortOrder, int startFrom, int countToFetch) throws MRSCException;

    /*
     * @returns a string result as per the query
     */
    public String readQueryStringResult(String xquery) throws MRSCException;

    /*
     * @returns a list of documents as per the query
     */
    public ArrayList<Document> readQueryDocumentList(String xquery) throws MRSCException;

    /*
     * imports the db files into the active database
     */
    public void importDB(String importDBName, String path) throws MRSCException;

    /*
     * exports the active database to do files 
     */
    public void exportDB(String exportDBName, String path) throws MRSCException;
    
    /*
     * deletes a saved db
     */
    public void deleteDB(String type, String deleteDBParams) throws MRSCException;
    
	/*
	
	// List of methods required for backwards compatiblity with the IPVS codebase

    public Element getMRSchema(String schemaPath) throws SCException;

    public void initDB(String ipvsRootNID, String serviceDomain, String schemaPath, boolean reset) throws SCException;

    public boolean objectExists(String listName, String collectionName, String NID, String childXpath, String xQueryPrefix) throws SCException;

    public Document add(String listName, String collectionName, String NID, String childXpath, String xQueryPrefix, String[] arrWhereXpath, Element dataElement) throws SCException;

    public Document replace(String listName, String collectionName, String NID, String childXpath, String xQueryPrefix, String[] arrWhereXpath, Element dataElement) throws SCException;
    
    public Document replaceAttribute(String listName, String collectionName, String NID, String childXpath, String xQueryPrefix, String[] arrWhereXpath, String data) throws SCException ;

    public Document delete(String listName, String collectionName, String NID, String childXpath, String xQueryPrefix, String[] arrWhereXpath) throws SCException;

    public Element readSingle(String listName, String collectionName, String NID, String childXpath, String xQueryPrefix, String[] arrWhereXpath, String[] sortByXpath, String sortOrder, String selectXML, Element appendToParentElement) throws SCException;

    public Element readList(String listName, String collectionName, String NID, String childXpath, String xQueryPrefix, String[] arrWhereXpath, String[] sortByXpath, String sortOrder, String selectXML, Element appendToParentElement, int startFrom, int countToFetch) throws SCException;

    public String readQuery(String query, Map<String, String> pemXPathList, Element appendResultToParentElement) throws SCException;
	*/

}
