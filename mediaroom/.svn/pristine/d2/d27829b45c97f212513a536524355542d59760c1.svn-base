package com.ipvs.systemservice.filetransfer;




import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.common.MRLog;
import com.ipvs.common.service.MRXMLUtils;


public class FileTransferTable {

    Document fileTransferInstanceListDoc;
    String userJID;
    MRLog log;
    
    private static final String FILETRANSFER_INSTANCE = "Instance";
    public static final String FILEXFERTYPE_INPUT = "Input";
    public static final String FILEXFERTYPE_OUTPUT = "Output";

    public FileTransferTable(String userJID, MRLog log) {
    	this.userJID = userJID;
    	this.log = log;
    	try {
    	this.fileTransferInstanceListDoc = MRXMLUtils.stringToDocument("<FileTransferInstanceList userJID='" + this.userJID + "'/>");
    	}
    	catch(Exception e) {
    		e.printStackTrace();
    	}
    }
    public void getStatus(Element rootElement) throws Exception {
	    rootElement.add((Element)fileTransferInstanceListDoc.getRootElement().clone());
	}
    
    private String generateFileTransferInstanceNID(String fileXferID) throws Exception {
    	return fileXferID + "." + FILETRANSFER_INSTANCE;
    }
    
    public void addFileTransferInstance(String type, Document FileXferDoc)
    throws Exception {
    	String clientJID=null;
		clientJID = MRXMLUtils.getValueXpath(FileXferDoc, "//toAgentJID", "");
    	addFileTransferInstance(type, FileXferDoc, clientJID);
    }
    
    public void addFileTransferInstance(String type, Document FileXferDoc, String clientJID)
    throws Exception {
    	Document doc;
    	String fileTransferNID=null;
    	String fromFileName=null;
    	String toFileName=null;
    	String fileXferID=null;
    	String serverJID=null;
		
		fileXferID = MRXMLUtils.getValueXpath(FileXferDoc, 
				"//FileXferID", "");
		serverJID = MRXMLUtils.getValueXpath(FileXferDoc, "//fromAgentJID", "");

		fromFileName = MRXMLUtils.getValueXpath(FileXferDoc, "//fromFileName", "");
		toFileName = MRXMLUtils.getValueXpath(FileXferDoc, "//toFileName", "");

    	fileTransferNID = generateFileTransferInstanceNID(fileXferID);
		
    	doc = MRXMLUtils.loadXMLResource("/com/ipvs/systemservice/filetransfer/FileTransferInstance.xml", log);
		try {
			MRXMLUtils.setValueXpath(doc, "//type", type);
			MRXMLUtils.setValueXpath(doc, "//fileXferID", fileXferID);
			MRXMLUtils.setValueXpath(doc, "//fromFileName", fromFileName);
			MRXMLUtils.setValueXpath(doc, "//toFileName", toFileName);
			MRXMLUtils.setValueXpath(doc, "//clientJID", clientJID);
			MRXMLUtils.setValueXpath(doc, "//serverJID", serverJID);
		}
		catch (Exception e) {
			e.printStackTrace();
		}
        Element resourceInstance = (Element) MRXMLUtils.getElementXpath(doc, 
        		"//FileTransfer").detach();
        MRXMLUtils.addAttribute(resourceInstance, "NID", fileTransferNID);
        MRXMLUtils.getElementXpath(resourceInstance, ".//request").add(
        		(Element)FileXferDoc.getRootElement().clone());
        
        // remove the old and replace with the new instance element if already exists.
        String xpath = "//FileTransfer[data/fileXferID='" + fileXferID + "']";
        Element oldResourceInstance = MRXMLUtils.getElementXpath(
        			this.fileTransferInstanceListDoc, xpath);
        this.fileTransferInstanceListDoc.getRootElement().remove(oldResourceInstance);
        
        this.fileTransferInstanceListDoc.getRootElement().add(resourceInstance);
        
    }

    public boolean updateFileTransferInstance(Document FileXferDoc)
    throws Exception {
    	String fileXferID=null;
        Element resourceInstance =null;
    	boolean bUpdated=false;
		String xpath = null;
		String progress;
		
		fileXferID = MRXMLUtils.getValueXpath(FileXferDoc, 
				"//FileXferID", "");
		xpath = "//FileTransfer[data/fileXferID='" + fileXferID + "']";
        resourceInstance = MRXMLUtils.getElementXpath(
        			this.fileTransferInstanceListDoc, xpath);
        if (resourceInstance != null) {        	
    		progress = MRXMLUtils.getValueXpath(FileXferDoc, "//percentageComplete", "");
    		MRXMLUtils.setValueXpath(resourceInstance, "//percentageComplete", progress);
    		bUpdated=true;    		 
        }
        
		return bUpdated;
    }
    
    public Element getFileTransferInstance(String fileXferID) throws Exception {
		String xpath = "//FileTransfer[data/fileXferID='" + fileXferID + "']";
        Element resourceInstance = (Element)MRXMLUtils.getElementXpath(
        			this.fileTransferInstanceListDoc, xpath).clone();
        return resourceInstance;
    }
    
    public Element getFileTransferInstance(Document FileXferDoc)
    throws Exception {
    	String fileXferID=null;
    	String xpath=null;
        Element resourceInstance =null;
		
		fileXferID = MRXMLUtils.getValueXpath(FileXferDoc, 
				"//FileXferID", "");
		xpath = "//FileTransfer[data/fileXferID='" + fileXferID + "']";
        resourceInstance = MRXMLUtils.getElementXpath(
        			this.fileTransferInstanceListDoc, xpath);
		return resourceInstance;
    }

    public boolean deleteFileTransferInstance(String fileXferID) throws Exception {
		String xpath = "//FileTransfer[data/fileXferID='" + fileXferID + "']";
        Element resourceInstance =null;
        boolean bResult=false;
        	
        try {
            resourceInstance = MRXMLUtils.getElementXpath(
        			this.fileTransferInstanceListDoc, xpath);
        }
		catch (Exception e) {
			e.printStackTrace();
		}
        
        if (resourceInstance != null) {        	
			resourceInstance.detach();
        	fileTransferInstanceListDoc.getRootElement().remove(resourceInstance);
        	bResult=true;        	
        }
        else {
        	log.addInfo ("Delete File Transfer Instance Could not find " + fileXferID);
        }
        
        return bResult;
        
    }       
}
