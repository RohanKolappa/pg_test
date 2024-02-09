package com.barco.nimbus.database;

import java.util.ArrayList;
import java.util.concurrent.locks.ReadWriteLock;
import java.util.concurrent.locks.ReentrantReadWriteLock;

import org.apache.log4j.Logger;
import org.dom4j.Document;
import org.dom4j.Element;
import org.dom4j.io.DOMReader;
import org.osgi.service.component.annotations.Activate;
import org.osgi.service.component.annotations.Component;
import org.osgi.service.component.annotations.Deactivate;
import org.xmldb.api.base.ResourceIterator;
import org.xmldb.api.base.ResourceSet;
import org.xmldb.api.modules.XMLResource;

import com.barco.nimbus.api.DBI;
import com.barco.nimbus.api.DBListenerI;
import com.barco.nimbus.api.common.SCException;
import com.barco.nimbus.api.common.SCExceptionCodes;
import com.barco.nimbus.database.exist.ExistClient;
import com.barco.nimbus.utils.common.XMLUtils;

@Component
public class DB implements DBI {

	// connection parameters.
	private static final String serverIP = "localhost";
	private static final int dbServerPort = 10085;
	private static final String user = "admin";
	private static final String password = "test";
	@SuppressWarnings("unused")
	private static final String schemaPath = "schemaPath";

	private static Logger logger = Logger.getLogger(DB.class);

	private ExistClient ec;
	private int dbState = DBI.DISCONNECTED;
	private final static String NIDLabel = "NID";
	private DBListenerI listener;
	private ReadWriteLock readWriteLock = new ReentrantReadWriteLock(true);

	private String getWhereXpathFromNID(String NID) throws SCException {
		if (NID == null || NID.equals(""))
			throw new SCException(SCExceptionCodes.DB_NOT_INITIALIZED,
					"DB:getWhereXpathFromNID No NID='" + NID + "'", null, null);
		
		//If the nid is collection nid then add parentNID xpath 
		String[] array = NID.split("\\.");
		if (array.length == 2) {
			return "/*[@parentNID='"+NID+"']";
		}
		
		return "/*[@NID='" + NID + "']";
	}

	private String getCollectionNameFromNID(String NID) throws SCException {
		if (NID == null || NID.equals(""))
			throw new SCException(SCExceptionCodes.DB_NOT_INITIALIZED,
					"DB:getCollectionNameFromNID No NID='" + NID + "'", null,
					null);
		String[] array = NID.split("\\.");
		if (array.length < 2)
			throw new SCException(SCExceptionCodes.DB_NOT_INITIALIZED,
					"DB:getCollectionNameFromNID Invalid NID='" + NID + "'",
					null, null);
		return "ipvs." + array[0].trim() + "." + array[1].trim();
	}

	private String getIdFromNID(String NID) throws SCException {
		if (NID == null || NID.equals(""))
			throw new SCException(SCExceptionCodes.DB_NOT_INITIALIZED,
					"DB:getIdFromNID No NID='" + NID + "'", null, null);
		String[] array = NID.split("\\.");
		if (array.length < 3)
			throw new SCException(SCExceptionCodes.DB_NOT_INITIALIZED,
					"DB:getIdFromNID Invalid NID='" + NID + "'", null, null);
		return array[2].trim();
	}

	/*
	 * This shows the relationship between NID,collectionName and id
	 * collectionName = ipvs.default.mediastreamsrcportresourcelist NID =
	 * default
	 * .mediastreamsrcportresourcelist.mssprl_834a2a86-26cf-40f0-8d6b-6e1612
	 * b46a3f id = mssprl_834a2a86-26cf-40f0-8d6b-6e1612b46a3f
	 */
	public String createNIDFromCollectionName(String collectionName)
			throws SCException {
		if (collectionName == null || collectionName.equals(""))
			throw new SCException(SCExceptionCodes.DB_NOT_INITIALIZED,
					"DB:createNIDFromCollectionNameNo collectionName='"
							+ collectionName + "'", null, null);
		String[] array = collectionName.split("\\.");
		if (array.length < 3)
			throw new SCException(SCExceptionCodes.DB_NOT_INITIALIZED,
					"DB:createNIDFromCollectionName Invalid collectionName='"
							+ collectionName + "'", null, null);
		return array[1].trim() + "." + array[2].trim() + "."
				+ String.valueOf(System.currentTimeMillis());
	}

	@Activate
	public void start() throws Exception {
		logger.info("DB:start");

		try {
			login();
			logger.info(":start DB logged in");
		} catch (Exception e) {
			logger.error(":start DB login failed.", e);
		}

	}

	@Deactivate
	public void stop() throws Exception {
		this.logout();
	}

	@Override
	public int getState() {
		return dbState;
	}

	private void login() throws SCException, Exception {

		this.ec = new ExistClient();
		this.ec.login(serverIP, dbServerPort, user, password);
		dbState = DBI.CONNECTED;
	}

	private void logout() throws SCException, Exception {
		if (this.ec == null)
			return;
		try {
			logger.info(":logout");
			ec.logout();
			if (this.listener != null)
				this.listener.handleConnected();
		} catch (Exception e) {
			logger.warn(":logout; DB failed to logout", e);
		}
		ec = null;
		dbState = DBI.DISCONNECTED;
	}

	@Override
	public boolean objectExists(String collectionName, String NID)
			throws SCException {
		try {
			readWriteLock.readLock().lock();
			String id = this.getIdFromNID(NID);
			XMLResource res = this.ec.readById(collectionName, id);
			return (res != null);
		} catch (Exception e) {
			logger.error(":objectExists; error occured while reading object.",
					e);
			throw new SCException(SCExceptionCodes.DB_NOT_INITIALIZED,
					"DB:objectExists exception=" + e.toString(), null, null);
		}
		finally{
			readWriteLock.readLock().unlock();
		}
	}

	@Override
	public String addDocument(String collectionName, String NID, Document doc)
			throws SCException {
		String id =null;
		try {
			readWriteLock.writeLock().lock();
			if ((collectionName == null) || collectionName.isEmpty())
				collectionName = getCollectionNameFromNID(NID);
			id = this.getIdFromNID(NID);
			doc.getRootElement().addAttribute(NIDLabel, NID);
			this.ec.addbyId(collectionName, id, doc);
			return NID;
		} catch (Exception e) {
			logger.error(":addDocument; error occured while adding document. CollectionName: "+collectionName+";NID: "+ NID+ ";ID: " + id + ";Document: "+doc.asXML(),
					e);
			throw new SCException(SCExceptionCodes.DB_NOT_INITIALIZED,
					"DB:addDocument exception=" + e.toString(), e, NID);
		}
		finally{
			readWriteLock.writeLock().unlock();
		}
	}

	@Override
	public ArrayList<String> addElement(String collectionName, String NID,
			String parentElementXpath, String whereXpath, Element dataElement)
			throws SCException {
		String xquery = null;
		try {
			readWriteLock.writeLock().lock();

			if ((parentElementXpath == null) || parentElementXpath.isEmpty()) {
				Document doc = XMLUtils.elementToDocument(dataElement);
				NID = this.addDocument(collectionName, NID, doc);
				ArrayList<String> list = new ArrayList<String>();
				list.add(NID);
				return list;
			}
			if ((collectionName == null) || collectionName.isEmpty())
				collectionName = getCollectionNameFromNID(NID);
			if ((whereXpath == null) || whereXpath.isEmpty())
				whereXpath = getWhereXpathFromNID(NID);
			
			xquery = "for $doc in collection('" + collectionName + "')"
					+ whereXpath + "\n let $update := " + "update insert "
					+ dataElement.asXML() + " into $doc" + parentElementXpath
					+ "\n  return data($doc/@NID)";
			ResourceSet result = this.ec.readByXQuery(xquery);
			return this.resultToArray(result);
		} catch (Exception e) {
			logger.error(":addElement; error occured while adding element. CollectionName: " + collectionName + ";NID: " + NID + ";ParentEleXpath: "+parentElementXpath+";WhereXpath: " + whereXpath
					+";Query: " + xquery +";Element: " + dataElement.asXML(), e);
			throw new SCException(SCExceptionCodes.DB_NOT_INITIALIZED,
					"DB:addElement exception=" + e.toString(), e, xquery);
		}
		finally
		{
			readWriteLock.writeLock().unlock();
		}
	}

	@Override
	public ArrayList<String> addAttribute(String collectionName, String NID,
			String elementXpath, String whereXpath, String attributeName,
			String attributeValue) throws SCException {
		String xquery = null;
		try {
			readWriteLock.writeLock().lock();
			if ((collectionName == null) || collectionName.isEmpty())
				collectionName = getCollectionNameFromNID(NID);
			if ((whereXpath == null) || whereXpath.isEmpty())
				whereXpath = getWhereXpathFromNID(NID);
			xquery = "for $doc in collection('" + collectionName + "')"
					+ whereXpath + "\n let $update := "
					+ "update insert attribute " + attributeName + " {'"
					+ attributeValue + "'}  into $doc" + elementXpath
					+ "\n  return data($doc/@NID)";
			ResourceSet result = this.ec.readByXQuery(xquery);
			return this.resultToArray(result);
		} catch (Exception e) {
			logger.error(
					":addAttribute; error occured while adding attribute. CollectionName: " + collectionName +";ElementXpath: " + elementXpath + ";WhereXpath: " + whereXpath + ";AttributeName: " + attributeName +";AttributeValue: " + attributeValue +";Query: " + xquery, e);
			throw new SCException(SCExceptionCodes.DB_NOT_INITIALIZED,
					"DB:addAttribute exception=" + e.toString(), e, xquery);
		}
		finally
		{
			readWriteLock.writeLock().unlock();
		}
	}

	@Override
	public String replaceDocument(String collectionName, String NID,
			String whereXpath, Document doc) throws SCException {
		String xquery = null;
		try {
			readWriteLock.writeLock().lock();
			if ((NID == null) || NID.isEmpty()) { // try to get the NID from the
													// doc
				NID = doc.getRootElement().attributeValue("NID");
			}
			if (((collectionName == null) || collectionName.isEmpty()))
				collectionName = getCollectionNameFromNID(NID);
			
			String id = this.getIdFromNID(NID);
			this.ec.addbyId(collectionName, id, doc);
			return NID;
		} catch (Exception e) {
			logger.error(
					":replaceDocument; error occured while replacing document. CollectionName: " + collectionName+";NID: " + NID + ";WhereXpath: " + whereXpath + ";Query: " + xquery + ";Doc: " + doc.asXML(),
					e);
			throw new SCException(SCExceptionCodes.DB_NOT_INITIALIZED,
					"DB:replaceDocument 1 exception=" + e.toString(), e, NID);
		}
		finally
		{
			readWriteLock.writeLock().unlock();
		}
	}

	@Override
	public ArrayList<String> replaceElement(String collectionName, String NID,
			String elementXpath, String whereXpath, Element dataElement)
			throws SCException {
		String xquery = null;
		try {
			readWriteLock.writeLock().lock();
			ArrayList<String> list = new ArrayList<String>();
			if ((elementXpath == null) || elementXpath.isEmpty()) {
				logger.debug(":replaceElement collectionName: " + collectionName + ";NID: " + NID+";ElementXpath: " + elementXpath+";WhereXPath: "+ whereXpath);
				Document doc = XMLUtils.elementToDocument(dataElement);
				this.replaceDocument(collectionName, NID, whereXpath, doc);
				list.add(NID);
				return list;
			}
			if ((collectionName == null) || collectionName.isEmpty())
				collectionName = getCollectionNameFromNID(NID);
			if (((whereXpath == null) || whereXpath.isEmpty()))
				whereXpath = getWhereXpathFromNID(NID);
			
			// String xquery = "for $doc in " + whereXpath +
			// "\n let $update := " +
			xquery = "for $doc in collection('" + collectionName + "')"
					+ whereXpath + "\n let $update := " + "update replace "
					+ "$doc" + elementXpath + " with " + dataElement.asXML()
					+ "\n  return data($doc/@NID)";
			ResourceSet result = this.ec.readByXQuery(xquery);
			list = this.resultToArray(result);
			return list;
		} catch (Exception e) {
			logger.error(":replaceElement; error occured while replacing element collectionName: " + collectionName + ";NID: " + NID+";ElementXpath: " + elementXpath+";whereXPath: "+ whereXpath+";Query: " +xquery + ";Element: " + dataElement.asXML(),e);
			throw new SCException(SCExceptionCodes.DB_NOT_INITIALIZED,
					"DB:replaceElement exception=" + e.toString(), e, xquery);
		}
		finally
		{
			readWriteLock.writeLock().unlock();
		}
	}

	@Override
	public ArrayList<String> replaceAttribute(String collectionName,
			String NID, String attributeXpath, String whereXpath, String data)
			throws SCException {
		String xquery = null;
		try {
			readWriteLock.writeLock().lock();
			if ((collectionName == null) || collectionName.isEmpty())
				collectionName = getCollectionNameFromNID(NID);
			if ((whereXpath == null) || whereXpath.isEmpty())
				whereXpath = getWhereXpathFromNID(NID);
			xquery = "for $doc in collection('" + collectionName + "')"
					+ whereXpath + "\n let $update := " + "update value "
					+ "$doc" + attributeXpath + " with '" + data + "'"
					+ "\n  return data($doc/@NID)";
			ResourceSet result = this.ec.readByXQuery(xquery);
			return this.resultToArray(result);
		} catch (Exception e) {
			logger.error(":replaceAttribute; error occured while replacing attribute. CollectionName: " + collectionName + ";NID: " + NID+";AttributeXpath: " + attributeXpath+";WhereXPath: "+ whereXpath+";Query: " + xquery +";DataElement: " + data,e);
			throw new SCException(SCExceptionCodes.DB_NOT_INITIALIZED,
					"DB:replaceAttribute exception=" + e.toString(), e, xquery);
		}
		finally{
			readWriteLock.writeLock().unlock();
		}
	}

	@Override
	public ArrayList<String> deleteDocument(String collectionName, String NID,
			String whereXpath) throws SCException {
		ArrayList<String> result = new ArrayList<String>();
		try {
			readWriteLock.writeLock().lock();
			if ((collectionName == null) || collectionName.isEmpty())
				collectionName = getCollectionNameFromNID(NID);
			if ((NID != null) && !NID.isEmpty()) {
				logger.debug(":deleteDocument deleting Document. CollectionName: " + collectionName +";NID: " + NID +";WhereXpath: " + whereXpath);
				String id = this.getIdFromNID(NID);
				this.ec.deleteById(collectionName, id);
				result.add(NID);
				return result;
			}
			result = this.deleteDocumentsByCollectionName(collectionName, whereXpath);
			return result; 
		} catch (Exception e) {
			logger.error(":deleteDocument; error occured while deleting document(s). CollectionName: " + collectionName + ";NID: " + NID+";WhereXPath: "+ whereXpath,e);
			throw new SCException(SCExceptionCodes.DB_NOT_INITIALIZED,
					"DB:deleteDocument exception=" + e.toString(), e, NID);
		}
		finally
		{
			readWriteLock.writeLock().unlock();
		}
	}

	private ArrayList<String> deleteNode(String collectionName, String NID,
			String nodeXpath, String whereXpath) throws SCException, Exception {
		if ((collectionName == null) || collectionName.isEmpty())
			collectionName = getCollectionNameFromNID(NID);
		if ((whereXpath == null) || whereXpath.isEmpty())
			if ((NID != null) && (!NID.isEmpty()))
				whereXpath = getWhereXpathFromNID(NID);
		String xquery = "for $doc in collection('" + collectionName + "')"
				+ whereXpath + "\n " + "for $node in $doc" + nodeXpath
				+ "\n  let $update := " + "update delete $node"
				+ "\n  return data($doc/@NID)";
		ResourceSet result = this.ec.readByXQuery(xquery);
		return this.resultToArray(result);
	}

	private ArrayList<String> deleteDocumentsByCollectionName(
			String collectionName, String whereXpath) throws Exception {
		ArrayList<String> result = new ArrayList<String>();
		String xquery = "for $file in collection('" + collectionName + "')/*"
				+ whereXpath + "\n " + " return data($file/@NID)";
		ResourceSet resultSet = this.ec.readByXQuery(xquery);
		if (resultSet != null && resultSet.getSize() > 0) {
			ArrayList<String> nids = this.resultToArray(resultSet);
			for (String nid : nids) {
				if ((nid != null) && !nid.isEmpty()) {
					String id = this.getIdFromNID(nid);
					this.ec.deleteById(collectionName, id);
					result.add(nid);
			}
		}
		
	}
		return result;
	}
	@Override
	public ArrayList<String> deleteElement(String collectionName, String NID,
			String elementXpath, String whereXpath) throws SCException {
		try {
			readWriteLock.writeLock().lock();
			if ((elementXpath == null) || elementXpath.isEmpty()) {
				return this.deleteDocument(collectionName, NID, whereXpath);
			}
			if ((collectionName == null) || collectionName.isEmpty())
				collectionName = getCollectionNameFromNID(NID);
			
			return this.deleteNode(collectionName, NID, elementXpath,whereXpath);
		} catch (Exception e) {
			logger.error(":deleteElement; error occured while deleting element. CollectionName: " + collectionName + ";NID: " + NID+";ElementXpath: " + elementXpath+";WhereXPath: "+ whereXpath,e);
			throw new SCException(SCExceptionCodes.DB_NOT_INITIALIZED,
					"DB:deleteElement exception=" + e.toString(), e, NID);
		}
		finally{
			readWriteLock.writeLock().unlock();
		}
	}

	@Override
	public ArrayList<String> deleteAttribute(String collectionName, String NID,
			String attributeXpath, String whereXpath) throws SCException {
		try {
			readWriteLock.writeLock().lock();
			if ((collectionName == null) || collectionName.isEmpty())
				collectionName = getCollectionNameFromNID(NID);
			return this.deleteNode(collectionName, NID, attributeXpath,
					whereXpath);
		} catch (Exception e) {
			logger.error(":deletingAttribute; error occured while deleting attribute. CollectionName: " + collectionName + ";NID: " + NID+";AttributeXpath: " + attributeXpath+";WhereXPath: "+ whereXpath,e);
			throw new SCException(SCExceptionCodes.DB_NOT_INITIALIZED,
					"DB:deleteAttribute exception=" + e.toString(), e, NID);
		}
		finally
		{
			readWriteLock.writeLock().unlock();
		}
	}

	@Override
	public Document readSingleDocument(String collectionName, String NID,
			String whereXpath) throws SCException {
		try {
			readWriteLock.readLock().lock();
			if ((collectionName == null) || collectionName.isEmpty())
				collectionName = getCollectionNameFromNID(NID);
			String id = this.getIdFromNID(NID);
			XMLResource res = this.ec.readById(collectionName, id);
			Document result = null;
			if (res != null)
				result = this.resourceToDoc(res);
			return result;
		} catch (Exception e) {
			logger.error(":readSingleDocument; error occured while replacing single document. CollectionName: " + collectionName + ";NID: " + NID+";WhereXPath: "+ whereXpath,e);
			throw new SCException(SCExceptionCodes.DB_NOT_INITIALIZED,
					"DB:readSingleDocument exception=" + e.toString(), e, NID);
		}
		finally {
			readWriteLock.readLock().unlock();
		}
	}

	@Override
	public Element readSingleElement(String collectionName, String NID,
			String elementXpath, String whereXpath) throws SCException {
		try {
			readWriteLock.readLock().lock();
			if ((collectionName == null) || collectionName.isEmpty())
				collectionName = getCollectionNameFromNID(NID);
			if ((whereXpath == null) || whereXpath.isEmpty())
				whereXpath = getWhereXpathFromNID(NID);
			if (elementXpath == null)
				elementXpath = "";
			String xquery = "for $doc in collection('" + collectionName + "')"
					+ whereXpath + "\n " + "return $doc" + elementXpath;
			ResourceSet resultSet = this.ec.readByXQuery(xquery);
			ResourceIterator i = resultSet.getIterator();
			
			Element result = null;
			while (i.hasMoreResources()) {
				XMLResource res = (XMLResource) i.nextResource();
				result = (Element) this.resourceToDoc(res).getRootElement()
						.detach();
			}
			return result;
		} catch (Exception e) {
			logger.error(
					":readSingleElement; error occured while reading single element. CollectionName: " + collectionName + ";NID: " + NID+";ElementXpath: " + elementXpath+";WhereXpath: " + whereXpath,
					e);
			throw new SCException(SCExceptionCodes.DB_NOT_INITIALIZED,
					"DB:readSingleElement exception=" + e.toString(), e, NID);
		}
		finally{
			readWriteLock.readLock().unlock();
		}
	}

	@Override
	public ArrayList<Element> readElementList(String collectionName,
			String NID, String elementXpath, String whereXpath,
			String sortByXpath, String sortOrder, int startFrom,
			int countToFetch) throws SCException {
		String xquery = null;
		try {
			readWriteLock.readLock().lock();
			if ((collectionName == null) || collectionName.isEmpty())
				collectionName = getCollectionNameFromNID(NID);
			if ((whereXpath == null) || whereXpath.isEmpty()) {
				if ((NID != null) && !NID.isEmpty())
					whereXpath = getWhereXpathFromNID(NID);
			} else
				whereXpath = "/*" + whereXpath;
			if (elementXpath == null)
				elementXpath = "";

			startFrom = startFrom + 1; // Should start from 1 and not 0
			xquery = "let $list1 := for $doc in collection('" + collectionName
					+ "')" + whereXpath + "\n ";
			if ((sortByXpath != null) && !sortByXpath.isEmpty())
				xquery = xquery + "order by " + sortByXpath + " "
						+ sortOrder + "\n ";

			if (elementXpath.startsWith("<")) // this is a return clause
				xquery = xquery + "return " + elementXpath + "\n";
			else
				xquery = xquery + "return $doc" + elementXpath + "\n";

			if (countToFetch == -1)
				xquery = xquery + "return $list1";
			else
				xquery = xquery + "return subsequence($list1," + startFrom
						+ "," + countToFetch + ")";
			ResourceSet result = this.ec.readByXQuery(xquery);
			ArrayList<Element> list = new ArrayList<Element>();
			ResourceIterator i = result.getIterator();
			while (i.hasMoreResources()) {
				XMLResource res = (XMLResource) i.nextResource();
				Document dom4jdoc = this.resourceToDoc(res);
				list.add((Element) dom4jdoc.getRootElement().detach());
			}
			return list;
		} catch (Exception e) {
			logger.error(":readElementList; error occured while reading element list. CollectionName: " + collectionName + ";NID: " + NID+";WhereXPath: "+ whereXpath + ";SortByXpath: " + sortByXpath +";SortOrder: " +sortOrder + ";StartFrom: " + startFrom +";CountToFetch: " + countToFetch +";Query: " + xquery,e);
			throw new SCException(SCExceptionCodes.DB_NOT_INITIALIZED,
					"DB:readList exception =" + e.toString(), e, xquery);
		}
		finally{
			readWriteLock.readLock().unlock();
		}
	}

	@Override
	public String readQueryStringResult(String xquery) throws SCException {
		try {
			readWriteLock.readLock().lock();
			ResourceSet result = this.ec.readByXQuery(xquery);
			StringBuffer sb = new StringBuffer();
			ResourceIterator i = result.getIterator();
			while (i.hasMoreResources()) {
				XMLResource res = (XMLResource) i.nextResource();
				sb.append(res.getContent());
			}
			return sb.toString();
		} catch (Exception e) {
			logger.error(
					":readQueryStringResult; error occured while reading query string result. Query: "+xquery,
					e);
			throw new SCException(SCExceptionCodes.DB_NOT_INITIALIZED,
					"DB:readQuery exception=" + e.toString(), e, xquery);
		}
		finally{
			readWriteLock.readLock().unlock();
		}
	}

	@Override
	public ArrayList<Document> readQueryDocumentList(String xquery)
			throws SCException {
		try {
			readWriteLock.readLock().lock();
			ResourceSet result = this.ec.readByXQuery(xquery);
			ArrayList<Document> list = new ArrayList<Document>();
			ResourceIterator i = result.getIterator();
			while (i.hasMoreResources()) {
				XMLResource res = (XMLResource) i.nextResource();
				Document dom4jdoc = this.resourceToDoc(res);
				list.add(dom4jdoc);
			}
			return list;
		} catch (Exception e) {
			//logger.debug(":readQueryDocumentList; error occured while reading query document list. Query: " + xquery,e);
			throw new SCException(SCExceptionCodes.DB_NOT_INITIALIZED,
					"DB:readQuery exception=" + e.toString(), e, xquery);
		}
		finally{
			readWriteLock.readLock().unlock();
		}
	}

	/*
	 * Converts the resource to dom4j Document
	 * 
	 * @returns : the dom4j Document
	 */
	private Document resourceToDoc(XMLResource resource) throws Exception {
		org.w3c.dom.Document w3doc = (org.w3c.dom.Document) resource
				.getContentAsDOM();
		DOMReader xmlReader = new DOMReader();
		Document dom4jdoc = xmlReader.read(w3doc);
		return dom4jdoc;
	}


	private ArrayList<String> resultToArray(ResourceSet result) throws Exception {
		ResourceIterator i = result.getIterator();
		ArrayList<String> list = new ArrayList<String>();
		while (i.hasMoreResources()) {
			list.add((String) i.nextResource().getContent());
		}
		return list;
	}

	@Override
	public void importDB(String importDBName, String importDBParams)
			throws SCException {
		throw new SCException(SCExceptionCodes.DB_NOT_INITIALIZED,
				"DB:importDB not implemented", null, null);
		// take write lock while implementing this functionality
	}

	@Override
	public void exportDB(String exportDBName, String exportDBParams)
			throws SCException {
		throw new SCException(SCExceptionCodes.DB_NOT_INITIALIZED,
				"DB:exportDB not implemented", null, null);
		// take read lock while implementing this functionality
	}

	@Override
	public void deleteDB(String type, String deleteDBParams) throws SCException {
		throw new SCException(SCExceptionCodes.DB_NOT_INITIALIZED,
				"DB:deleteDB not implemented", null, null);
		// take write lock while implementing this functionality
	}

	@Override
	public void registerListener(DBListenerI listener) {
		this.listener = listener;
	}
}
