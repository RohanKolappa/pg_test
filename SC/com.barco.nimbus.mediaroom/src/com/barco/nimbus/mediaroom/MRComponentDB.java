package com.barco.nimbus.mediaroom;

import java.util.ArrayList;

import org.dom4j.Document;
import org.dom4j.Element;

import com.barco.nimbus.api.DBI;
import com.barco.nimbus.api.common.SCException;
import com.ipvs.nimbus.MRDBI;
import com.ipvs.nimbus.MRSCException;
import com.ipvs.nimbus.MRSCExceptionCodes;

public class MRComponentDB implements MRDBI {
	
	DBI db;
	
	MRComponentDB(DBI db){
		this.db = db;
	}


	@Override
	public int getState() {
		// TODO Auto-generated method stub
		return 0;
	}

	@Override
	public void login(String serverIP, int serverPort, String user,
			String password, String schemePath) throws MRSCException, Exception {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void logout() throws MRSCException, Exception {
		// TODO Auto-generated method stub
		
	}

	@Override
	public boolean objectExists(String collectionName, String NID)
			throws MRSCException {
		// TODO Auto-generated method stub
		return false;
	}

	@Override
	public String addDocument(String collectionName, String NID, Document doc)
			throws MRSCException {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public ArrayList<String> addElement(String collectionName, String NID,
			String parentElementXpath, String whereXpath, Element dataElement)
			throws MRSCException {
		try {
			return this.db.addElement(collectionName, NID, parentElementXpath, whereXpath, dataElement);
		}
		catch(SCException se) {
			throw new MRSCException(MRSCExceptionCodes.DB_NOT_INITIALIZED, "MRComponentDB:addElement exception=" + se.toString(), se.getOriginException(), se.getOriginData());
		}
		catch(Exception e) {
			throw new MRSCException(MRSCExceptionCodes.DB_NOT_INITIALIZED, "MRComponentDB:addElement exception=" + e.toString(), e, NID);
		}

	}

	@Override
	public ArrayList<String> addAttribute(String collectionName, String NID,
			String elementXpath, String whereXpath, String attributeName,
			String attributeValue) throws MRSCException {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public String replaceDocument(String collectionName, String NID,
			String whereXpath, Document doc) throws MRSCException {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public ArrayList<String> replaceElement(String collectionName, String NID,
			String elementXpath, String whereXpath, Element dataElement)
			throws MRSCException {
		try {
			return this.db.replaceElement(collectionName, NID, elementXpath, whereXpath, dataElement);
		}
		catch(SCException se) {
			throw new MRSCException(MRSCExceptionCodes.DB_NOT_INITIALIZED, "MRComponentDB:replaceElement exception=" + se.toString(), se.getOriginException(), se.getOriginData());
		}
		catch(Exception e) {
			throw new MRSCException(MRSCExceptionCodes.DB_NOT_INITIALIZED, "MRComponentDB:replaceElement exception=" + e.toString(), e, NID);
		}
	}

	@Override
	public ArrayList<String> replaceAttribute(String collectionName, String NID,
			String attributeXpath, String whereXpath, String data)
			throws MRSCException {
		try {
			return this.db.replaceAttribute(collectionName, NID, attributeXpath, whereXpath, data);
		}
		catch(SCException se) {
			throw new MRSCException(MRSCExceptionCodes.DB_NOT_INITIALIZED, "MRComponentDB:replaceAttribute exception=" + se.toString(), se.getOriginException(), se.getOriginData());
		}
		catch(Exception e) {
			throw new MRSCException(MRSCExceptionCodes.DB_NOT_INITIALIZED, "MRComponentDB:replaceAttribute exception=" + e.toString(), e, NID);
		}
	}

	@Override
	public String deleteDocument(String collectionName, String NID,
			String whereXpath) throws MRSCException {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public ArrayList<String> deleteElement(String collectionName, String NID,
			String elementXpath, String whereXpath) throws MRSCException {
		try {
			return this.db.deleteElement(collectionName, NID, elementXpath, whereXpath);		
		}
		catch(SCException se) {
			throw new MRSCException(MRSCExceptionCodes.DB_NOT_INITIALIZED, "MRComponentDB:deleteElement exception=" + se.toString(), se.getOriginException(), se.getOriginData());
		}

		catch(Exception e) {
			throw new MRSCException(MRSCExceptionCodes.DB_NOT_INITIALIZED, "MRComponentDB:readSingleDocument exception=" + e.toString(), e, NID);
		}
	}

	@Override
	public ArrayList<String> deleteAttribute(String collectionName, String NID,
			String attributeXpath, String whereXpath) throws MRSCException {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public Document readSingleDocument(String collectionName, String NID,
			String whereXpath) throws MRSCException {
		try {
			return this.db.readSingleDocument(collectionName, NID, whereXpath);
		}
		catch(SCException se) {
			throw new MRSCException(MRSCExceptionCodes.DB_NOT_INITIALIZED, "MRComponentDB:readSingleDocument exception=" + se.toString(), se.getOriginException(), se.getOriginData());
		}
		catch(Exception e) {
			throw new MRSCException(MRSCExceptionCodes.DB_NOT_INITIALIZED, "MRComponentDB:readSingleDocument exception=" + e.toString(), e, NID);
		}		
	}

	@Override
	public Element readSingleElement(String collectionName, String NID,
			String elementXpath, String whereXpath) throws MRSCException {
		try {
			return this.db.readSingleElement(collectionName, NID, elementXpath, whereXpath);
		}
		catch(SCException se) {
			throw new MRSCException(MRSCExceptionCodes.DB_NOT_INITIALIZED, "MRComponentDB:readSingleElement exception=" + se.toString(), se.getOriginException(), se.getOriginData());
		}
		catch(Exception e) {
			throw new MRSCException(MRSCExceptionCodes.DB_NOT_INITIALIZED, "MRComponentDB:readSingleElement exception=" + e.toString(), e, NID);
		}
	}

	@Override
	public ArrayList<Element> readElementList(String collectionName, String NID,
			String elementXpath, String whereXpath, String sortByXpath,
			String sortOrder, int startFrom, int countToFetch)
			throws MRSCException {
		try {
			return this.db.readElementList(collectionName, NID, elementXpath, whereXpath, sortByXpath, sortOrder, startFrom, countToFetch);
		}
		catch(SCException se) {
			throw new MRSCException(MRSCExceptionCodes.DB_NOT_INITIALIZED, "MRComponentDB:readElementList exception=" + se.toString(), se.getOriginException(), se.getOriginData());
		}
		catch(Exception e) {
			throw new MRSCException(MRSCExceptionCodes.DB_NOT_INITIALIZED, "MRComponentDB:readElementList exception=" + e.toString(), e, NID);
		}
	}

	@Override
	public String readQueryStringResult(String xquery) throws MRSCException {
		try {
			return this.db.readQueryStringResult(xquery);
		}
		catch(SCException se) {
			throw new MRSCException(MRSCExceptionCodes.DB_NOT_INITIALIZED, "MRComponentDB:readQueryStringResult exception=" + se.toString(), se.getOriginException(), se.getOriginData());
		}
		catch(Exception e) {
			throw new MRSCException(MRSCExceptionCodes.DB_NOT_INITIALIZED, "MRComponentDB:readQueryStringResult exception=" + e.toString(), e, xquery);
		}
	}

	@Override
	public ArrayList<Document> readQueryDocumentList(String xquery)
			throws MRSCException {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public void importDB(String importDBName, String path) throws MRSCException {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void exportDB(String exportDBName, String path) throws MRSCException {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void deleteDB(String type, String deleteDBParams)
			throws MRSCException {
		// TODO Auto-generated method stub
		
	}

}
