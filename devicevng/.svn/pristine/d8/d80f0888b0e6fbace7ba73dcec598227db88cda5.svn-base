package com.barco.device.vng.Media;

import java.util.Iterator;
import java.util.List;

import org.apache.log4j.Logger;
import org.dom4j.Document;
import org.dom4j.Element;

import com.barco.device.common.DeviceDBI;
import com.barco.utils.XMLUtils;
import com.barco.utils.mr.XMPPTransaction;

public class PortTable {
    Document portInstanceListDoc;
    Document mediaStreamResourceInstanceListDoc;
    Logger log;
    DeviceDBI deviceDB;
    
    public static final String MEDIASTREAM_RESOURCE = "MediaStreamResource";
    public static final String MediaStreamResourceInstanceListXML = "/com/barco/device/vng/Media/MediaStreamResource-InstanceList.xml";
    public static final String STREAM_INSTANCE = "Instance";


    public PortTable(Logger log, DeviceDBI deviceDB) {
    	this.log = log;
    	this.deviceDB = deviceDB;
    	try {
	    	this.portInstanceListDoc = XMLUtils.stringToDocument("<PortInstanceList userJID='" + this.deviceDB.getDeviceResourceNID() + "'/>");
			mediaStreamResourceInstanceListDoc = XMLUtils.loadXMLResource(MediaStreamResourceInstanceListXML);
    	}
    	catch(Exception e) {
    		e.printStackTrace();
    	}
    }
    
    private String generateStreamPortInstanceNID(String streamNID) throws Exception {
    	return streamNID + "." + STREAM_INSTANCE;
    }


    
    public void addPortInstance(String roomNID, String streamNID, String sourceNID, Element inputElement, Element outputElement)
    throws Exception {
    	XMPPTransaction.startBlock("SourceTable.addStreamPortInstance");

    //perf-logs this.log.addInfo(this.userJID + ": SourceTable: updateStreamInstance - Start", MRLog.OK, MRLog.NOTIFY);
    	
    	
    	String streamPortInstanceNID = generateStreamPortInstanceNID(streamNID);
    	
    

    String inputNID = XMLUtils.getAttributeValue(inputElement,"NID");
    String outputNID = XMLUtils.getAttributeValue(outputElement,"NID");

    Document doc = (Document)mediaStreamResourceInstanceListDoc.clone();
    //MRXMLUtils.stringToDocument(MRXMLDBUtils.getObjectData(request, db, log, streamInstanceNID));
    XMLUtils.setValueXpath(doc, "//RoomNID", roomNID);
    XMLUtils.setValueXpath(doc, "//StreamNID", streamNID);
    XMLUtils.setValueXpath(doc, "//ResourceNID", sourceNID);
    XMLUtils.setValueXpath(doc, "//DeviceStreamingServiceInputInstanceNID", inputNID);
    XMLUtils.setValueXpath(doc, "//DeviceStreamingServiceOutputInstanceNID", outputNID);

    // MRXMLUtils.setValueXpath(streamPortInstanceDoc, "//InputNID", inputNID);
    // MRXMLUtils.setValueXpath(streamPortInstanceDoc, "//OutputNID", outputNID);
    Element instance = (Element) XMLUtils.getElementXpath(doc, "//Instance").detach();
    XMLUtils.addAttribute(instance, "NID", streamPortInstanceNID);

    this.portInstanceListDoc.getRootElement().add(instance);

    // System.out.println(MRXMLUtils.documentToString(portInstanceListDoc));

    //perf-logs this.log.addInfo(this.userJID + ": SourceTable: setObjectData - end", MRLog.OK, MRLog.NOTIFY);
    // this.log.info("AddStreamPortInstance:" + this.deviceDB.getDeviceResourceNID() + "  StreamNID=" + streamNID);

    XMPPTransaction.endBlock("SourceTable.addStreamPortInstance");

    //perf-logs this.log.addInfo(this.userJID + ": SourceTable: updateStreamInstance - end", MRLog.OK, MRLog.NOTIFY);
}

public void updatePortInstance(Element msElement, Element instanceElement)
    throws Exception {
    // For each streamNID, find the PortInstance
    // Set the sourceNID and the input
    Element inputElement = instanceElement.element("Input");
    String inputNID = XMLUtils.getAttributeValue(inputElement,"NID");
    String sourceNID = XMLUtils.getAttributeValue(inputElement,"sourceNID");
    List<?> list = XMLUtils.selectNodesXpath(msElement, "//Stream");
    Iterator<?> i = list.iterator();

    while (i.hasNext()) {
        String streamNID =XMLUtils.getAttributeValue( ((Element) i.next()),"NID");
        Element instance = XMLUtils.getElementXpath(portInstanceListDoc, "//Instance[StreamNID='" + streamNID + "']");
        XMLUtils.setValueXpathElement(instance, "//ResourceNID",sourceNID);        
        XMLUtils.setValueXpathElement(instance, "//DeviceStreamingServiceInputInstanceNID",inputNID);
        // System.out.println(MRXMLUtils.documentToString(portInstanceListDoc));
    }
}

public void deletePortInstance(String streamNID) throws Exception {
    Element instance = XMLUtils.getElementXpath(portInstanceListDoc, "//Instance[StreamNID='" + streamNID + "']");

    if (instance != null) {
        portInstanceListDoc.getRootElement().remove(instance);
    }

    // System.out.println(MRXMLUtils.documentToString(portInstanceListDoc));
}

}
