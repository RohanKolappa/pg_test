package com.ipvs.mediaroomservice.stream;

import java.util.Iterator;
import java.util.List;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.client.ServiceClient;
import com.ipvs.common.MRLog;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceConst;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceUtils;

public class PortTable {
    Document portInstanceListDoc;
    String userJID;
    MRLog log;
    
    private static final String STREAM_INSTANCE = "Instance";
    private static final String INSTANCE_XML = "-InstanceList.xml";

    public PortTable(String userJID, MRLog log) {
    	this.userJID = userJID;
    	this.log = log;
    	try {
    	this.portInstanceListDoc = MRXMLUtils.stringToDocument("<PortInstanceList userJID='" + this.userJID + "'/>");
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
    com.ipvs.common.XMPPTransaction.startBlock("SourceTable.addStreamPortInstance");

    //perf-logs this.log.addInfo(this.userJID + ": SourceTable: updateStreamInstance - Start", MRLog.OK, MRLog.NOTIFY);
    String streamPortInstanceNID = generateStreamPortInstanceNID(streamNID);

    String inputNID = MRXMLUtils.getAttributeValue(inputElement,"NID");
    String outputNID = MRXMLUtils.getAttributeValue(outputElement,"NID");

    Document doc = 	MediaRoomServiceUtils.loadInstanceListXML(log, ServiceClient.ASSETADMINSERVICE, MediaRoomServiceConst.MEDIASTREAM_RESOURCE + INSTANCE_XML);
    //MRXMLUtils.stringToDocument(MRXMLDBUtils.getObjectData(request, db, log, streamInstanceNID));
    MRXMLUtils.setValueXpath(doc, "//RoomNID", roomNID);
    MRXMLUtils.setValueXpath(doc, "//StreamNID", streamNID);
    MRXMLUtils.setValueXpath(doc, "//ResourceNID", sourceNID);
    MRXMLUtils.setValueXpath(doc, "//DeviceStreamingServiceInputInstanceNID", inputNID);
    MRXMLUtils.setValueXpath(doc, "//DeviceStreamingServiceOutputInstanceNID", outputNID);

    // MRXMLUtils.setValueXpath(streamPortInstanceDoc, "//InputNID", inputNID);
    // MRXMLUtils.setValueXpath(streamPortInstanceDoc, "//OutputNID", outputNID);
    Element instance = (Element) MRXMLUtils.getElementXpath(doc, "//Instance").detach();
    MRXMLUtils.addAttribute(instance, "NID", streamPortInstanceNID);

    this.portInstanceListDoc.getRootElement().add(instance);

    // System.out.println(MRXMLUtils.documentToString(portInstanceListDoc));

    //perf-logs this.log.addInfo(this.userJID + ": SourceTable: setObjectData - end", MRLog.OK, MRLog.NOTIFY);
    this.log.addInfo("AddStreamPortInstance:" + userJID + "  StreamNID=" + streamNID, MRLog.NOTIFY, MRLog.NOTIFY);

    com.ipvs.common.XMPPTransaction.endBlock("SourceTable.addStreamPortInstance");

    //perf-logs this.log.addInfo(this.userJID + ": SourceTable: updateStreamInstance - end", MRLog.OK, MRLog.NOTIFY);
}

public void updatePortInstance(Element msElement, Element instanceElement)
    throws Exception {
    // For each streamNID, find the PortInstance
    // Set the sourceNID and the input
    Element inputElement = instanceElement.element("Input");
    String inputNID = MRXMLUtils.getAttributeValue(inputElement,"NID");
    String sourceNID = MRXMLUtils.getAttributeValue(inputElement,"sourceNID");
    List<?> list = MRXMLUtils.selectNodesXpath(msElement, "//Stream");
    Iterator<?> i = list.iterator();

    while (i.hasNext()) {
        String streamNID =MRXMLUtils.getAttributeValue( ((Element) i.next()),"NID");
        Element instance = MRXMLUtils.getElementXpath(portInstanceListDoc, "//Instance[StreamNID='" + streamNID + "']");
        MRXMLUtils.setValueXpath(instance, "//ResourceNID", sourceNID);
        MRXMLUtils.setValueXpath(instance, "//DeviceStreamingServiceInputInstanceNID", inputNID);

        // System.out.println(MRXMLUtils.documentToString(portInstanceListDoc));
    }
}

public void deletePortInstance(String streamNID) throws Exception {
    Element instance = MRXMLUtils.getElementXpath(portInstanceListDoc, "//Instance[StreamNID='" + streamNID + "']");

    if (instance != null) {
        portInstanceListDoc.getRootElement().remove(instance);
    }

    // System.out.println(MRXMLUtils.documentToString(portInstanceListDoc));
}

}
