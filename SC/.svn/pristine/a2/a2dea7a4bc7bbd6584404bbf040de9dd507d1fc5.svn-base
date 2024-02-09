package com.barco.nimbus.database.test;

import java.util.ArrayList;

import org.apache.log4j.Logger;
import org.dom4j.Document;
import org.dom4j.Element;

import com.barco.nimbus.api.DBI;
import com.barco.nimbus.api.common.SCException;
import com.barco.nimbus.utils.common.XMLUtils;

/*
 * Assumes that the following information is present in the db
 * collection : /db/testcol
 * resource : id="test.xml" doc="<User><Name type="last">Iyer</Name></User>"
 */
public class DBTest {

	DBI db;

	String serverIP = "10.51.51.22";
	int serverPort = 10085;
	String user = "admin";
	String password = "test";
	String schemaPath = "schemaPath";
	Document doc = null;
	String NID = null;

	String now = String.valueOf(System.currentTimeMillis());

	String collectionName = "ipvs.default.testcol";
	String objectName = "User";
	String s;
	String xquery = "for $doc in collection('" + collectionName
			+ "') return $doc//Name";

	public DBTest(DBI db) {
		this.db = db;
		try {
			NID = db.createNIDFromCollectionName(collectionName);
		} catch (Exception e) {
			e.printStackTrace();
		}
	}

	public void test_objectExists() throws SCException, Exception {
		Logger.getLogger(DBTest.class).info("DBTest:test_objectExists");
		boolean b = this.db.objectExists(collectionName, NID);
		if (!b)
			throw new Exception();
		b = this.db.objectExists(collectionName, NID + "1");
		if (b)
			throw new Exception();
	}

	public void test_addDocument() throws SCException, Exception {
		Logger.getLogger(DBTest.class).info("DBTest:test_addDocument");
		doc = this.db.readSingleDocument(collectionName, NID, null);
		if (doc != null)
			throw new Exception();
		String name = "Name_" + NID;
		doc = XMLUtils.stringToDocument("<User><Name>" + name
				+ "</Name></User>");
		NID = this.db.addDocument(collectionName, NID, doc);
		doc = this.db.readSingleDocument(collectionName, NID, null);
		if (doc == null)
			throw new Exception();
	}

	/*
	 * Adds an element identified by the elementXpath, within an existing
	 * document identified by the id OR whereXpath
	 */
	public void test_addElement() throws SCException, Exception {
		Logger.getLogger(DBTest.class).info("DBTest:test_addElement");
		doc = this.db.readSingleDocument(collectionName, NID, null);
		Element last = XMLUtils.getElementXpath(doc, "//Last");
		if (last != null)
			throw new Exception();
		Element element = XMLUtils.stringToElement("<Last>AddedElement</Last>");
		ArrayList<String> list = this.db.addElement(collectionName, NID,
				"//Name", null, element);
		Logger.getLogger(DBTest.class).info(
				"DBTest:test_addElement list=" + list.toString());

		String s = list.get(0);
		doc = this.db.readSingleDocument(collectionName, s, null);
		last = XMLUtils.getElementXpath(doc, "//Last");
		if (last == null)
			throw new Exception();
	}

	/*
	 * Adds an attribute to the element identified by the elementXpath, within
	 * an existing document identified by the id OR whereXpath
	 */
	public String test_addAttribute() throws SCException, Exception {
		Logger.getLogger(DBTest.class).info("DBTest:test_addAttribute");
		doc = this.db.readSingleDocument(collectionName, NID, null);
		Element nameElement = (Element) XMLUtils.getElementXpath(doc, "//Name")
				.clone();

		String attributeName = "Name_" + now;
		String attributeValue = nameElement.attributeValue(attributeName);
		if (attributeValue != null)
			throw new Exception();
		String addedAttributeValue = "Value_" + now;
		ArrayList<String> list = this.db.addAttribute(collectionName, NID,
				"//Name", null, attributeName, addedAttributeValue);
		Logger.getLogger(DBTest.class).info(
				"DBTest:test_addAttribute list=" + list.toString());

		doc = this.db.readSingleDocument(collectionName, NID, null);
		nameElement = (Element) XMLUtils.getElementXpath(doc, "//Name").clone();
		attributeValue = nameElement.attributeValue(attributeName);
		if (!attributeValue.equals(addedAttributeValue))
			throw new Exception();
		return attributeName;
	}

	/*
	 * Replaces a the document in the database with the given NID OR whereXpath
	 */
	public void test_replaceDocument() throws SCException, Exception {
		Logger.getLogger(DBTest.class).info("DBTest:test_replaceDocument");
		doc = this.db.readSingleDocument(collectionName, NID, null);
		String name = XMLUtils.getValueXpath(doc, "//Name", "");
		String newName = name + String.valueOf(System.currentTimeMillis());
		doc = XMLUtils.stringToDocument("<User><Name>" + newName
				+ "</Name></User>");
		NID = this.db.replaceDocument(collectionName, NID, null, doc);
		doc = this.db.readSingleDocument(collectionName, NID, null);
		name = XMLUtils.getValueXpath(doc, "//Name", "");
		if (!newName.equals(name))
			throw new Exception();
	}

	/*
	 * Replaces an element identified by the elementXpath, within an existing
	 * document identified by the id OR whereXpath
	 */
	public void test_replaceElement() throws SCException, Exception {
		Logger.getLogger(DBTest.class).info("DBTest:test_replaceElement 1");
		doc = this.db.readSingleDocument(collectionName, NID, null);
		Element nameElement = (Element) XMLUtils.getElementXpath(doc, "//Name")
				.clone();
		String name = XMLUtils.getValueXpath(doc, "//Name", "");
		String nameTime = name + String.valueOf(System.currentTimeMillis());
		Element dataElement = XMLUtils.stringToElement("<NameTime>" + nameTime
				+ "</NameTime>");
		ArrayList<String> list = this.db.replaceElement(collectionName, NID,
				"//Name", null, dataElement);
		Logger.getLogger(DBTest.class).info(
				"DBTest:test_replaceElement list=" + list.toString());

		doc = this.db.readSingleDocument(collectionName, NID, null);
		String replacedName = XMLUtils.getValueXpath(doc, "//NameTime", "");
		if (!nameTime.equals(replacedName))
			throw new Exception();
		list = this.db.replaceElement(collectionName, NID, "//NameTime", null,
				nameElement);
		doc = this.db.readSingleDocument(collectionName, NID, null);
		replacedName = XMLUtils.getValueXpath(doc, "//Name", "");
		if (!name.equals(replacedName))
			throw new Exception();
	}

	/*
	 * Replaces an attribute identified by the elementXpath, within an existing
	 * document identified by the id OR whereXpath
	 */
	public void test_replaceAttribute(String attributeName) throws SCException,
			Exception {
		Logger.getLogger(DBTest.class).info(
				"DBTest:test_replaceAttribute =" + attributeName);
		doc = this.db.readSingleDocument(collectionName, NID, null);
		Element nameElement = (Element) XMLUtils.getElementXpath(doc, "//Name")
				.clone();
		String attributeValue = String.valueOf(System.currentTimeMillis());
		ArrayList<String> list = this.db.replaceAttribute(collectionName, NID,
				"//Name/@" + attributeName, null, attributeValue);
		Logger.getLogger(DBTest.class).info(
				"DBTest:test_replaceAttribute list=" + list.toString());

		doc = this.db.readSingleDocument(collectionName, NID, null);
		nameElement = (Element) XMLUtils.getElementXpath(doc, "//Name").clone();
		String replacedAttributeValue = nameElement
				.attributeValue(attributeName);
		if (!attributeValue.equals(replacedAttributeValue))
			throw new Exception();
	}

	/*
	 * Deletes a the document in the database with the given id OR whereXpath
	 */
	public void test_deleteDocument() throws SCException, Exception {
		Logger.getLogger(DBTest.class).info("DBTest:test_deleteDocument");
		doc = this.db.readSingleDocument(collectionName, NID, null);
		if (doc == null)
			throw new Exception();
		ArrayList<String> eleElements = this.db.deleteDocument(collectionName, NID, null);
		Logger.getLogger(DBTest.class).info(
				"DBTest:test_deleteDocument, Content of deleted Documents=" + eleElements);
		for (String s : eleElements)
		{
			doc = this.db.readSingleDocument(collectionName, s, null);
			if (doc != null)
				throw new Exception();
		}
	}

	/*
	 * Deletes an element identified by the elementXpath, within an existing
	 * document identified by the id OR whereXpath
	 */
	public void test_deleteElement() throws SCException, Exception {
		Logger.getLogger(DBTest.class).info("DBTest:test_deleteElement");
		doc = this.db.readSingleDocument(collectionName, NID, null);
		Element nameElement = (Element) XMLUtils.getElementXpath(doc, "//Name")
				.clone();
		ArrayList<String> list = this.db.deleteElement(collectionName, NID,
				"//Name", null);
		Logger.getLogger(DBTest.class).info(
				"DBTest:test_deleteElement list=" + list.toString());

		doc = this.db.readSingleDocument(collectionName, NID, null);
		Element deletedElement = (Element) XMLUtils.getElementXpath(doc,
				"//Name");
		if (deletedElement != null)
			throw new Exception();
		this.db.addElement(collectionName, NID, "//User", null, nameElement);
	}

	/*
	 * Deletes an attribute identified by the elementXpath, within an existing
	 * document identified by the id OR whereXpath
	 */
	public void test_deleteAttribute() throws SCException, Exception {
		Logger.getLogger(DBTest.class).info("DBTest:test_deleteElement");
		doc = this.db.readSingleDocument(collectionName, NID, null);

		String attributeName = "Name_" + now;
		Element nameElement = (Element) XMLUtils.getElementXpath(doc, "//Name")
				.clone();
		String attributeValue = nameElement.attributeValue(attributeName);
		if (attributeValue == null)
			throw new Exception();

		ArrayList<String> list = this.db.deleteAttribute(collectionName, NID,
				"//Name/@" + attributeName, null);
		Logger.getLogger(DBTest.class).info(
				"DBTest:test_deleteAttribute list=" + list.toString());

		doc = this.db.readSingleDocument(collectionName, NID, null);
		nameElement = (Element) XMLUtils.getElementXpath(doc, "//Name").clone();
		attributeValue = nameElement.attributeValue(attributeName);
		if (attributeValue != null)
			throw new Exception();
	}

	/*
	 * Reads a the document in the database with the given id OR whereXpath
	 */
	public void test_readSingleDocument() throws SCException, Exception {
		Logger.getLogger(DBTest.class).info("DBTest:test_readSingleDocument");
		doc = this.db.readSingleDocument(collectionName, NID, null);
		if (doc == null)
			throw new Exception();
	}

	/*
	 * Reads an element identified by the elementXpath, within an existing
	 * document identified by the id OR whereXpath
	 */
	public void test_readSingleElement() throws SCException, Exception {
		Logger.getLogger(DBTest.class).info("DBTest:test_readSingleElement");
		Element element = this.db.readSingleElement(collectionName, NID,
				"//Name", null);
		if (element == null)
			throw new Exception();
	}

	/*
	 * Reads a list of countToFetch elements identified by the elementXpath,
	 * within an existing document identified by the id OR whereXpath as per the
	 * sort and start criteria
	 */
	public void test_readElementList() throws SCException, Exception {
		Logger.getLogger(DBTest.class).info("DBTest:test_readElementList");
		String sortByXpath = "//Name";
		String sortOrder = DBI.ASCENDING;
		int startFrom = 0;
		int countToFetch = 100;
		String whereXpath = "//User";
		ArrayList<Element> list = this.db.readElementList(collectionName, null,
				"//Name", whereXpath, sortByXpath, sortOrder, startFrom,
				countToFetch);
		Element element = list.get(0);
		if (element == null)
			throw new Exception();
		for (Element e : list) {
			Logger.getLogger(DBTest.class).info(
					"DBTest:test_readElementList e=" + e.asXML());
		}
	}

	/*
	 * @returns a string result as per the query
	 */
	public void test_readQueryStringResult() throws SCException, Exception {
		Logger.getLogger(DBTest.class)
				.info("DBTest:test_readQueryStringResult");
		s = this.db.readQueryStringResult(xquery);
		doc = XMLUtils.stringToDocument("<result>" + s + "</result>");
		Element element = XMLUtils.getElementXpath(doc, "//Name");
		if (element == null)
			throw new Exception();
	}

	/*
	 * @returns a list of documents as per the query
	 */
	public void test_readQueryDocumentList() throws SCException, Exception {
		Logger.getLogger(DBTest.class)
				.info("DBTest:test_readQueryDocumentList");
		String xquery = "for $doc in collection('" + collectionName
				+ "') return $doc";
		ArrayList<Document> list = this.db.readQueryDocumentList(xquery);
		Document doc = list.get(0);
		Element element = XMLUtils.getElementXpath(doc, "//Name");
		if (element == null)
			throw new Exception();
	}

	public void test(DBI db) throws Exception {

		test_addDocument();
		test_objectExists();
		test_addElement();
		test_addAttribute();
		test_replaceDocument();
		test_replaceElement();

		String attributeName = test_addAttribute();
		test_replaceAttribute(attributeName);

		test_readSingleDocument();
		test_readSingleElement();
		test_readElementList();
		test_readQueryStringResult();
		test_readQueryDocumentList();

		test_deleteAttribute();
		test_deleteElement();
		test_deleteDocument();

	}

}
