 package com.ipvs.xmpp.client.smack;

import java.io.File;
import java.io.OutputStream;

import org.dom4j.Document;
import org.jivesoftware.smack.packet.Packet;
 
import com.ipvs.assetadminservice.impl.FileSMUtils;
import com.ipvs.client.ServiceAgentUtils;
import com.ipvs.common.MRConst;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.XMPPTransaction;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.systemservice.impl.FileTransferAgent;
import com.ipvs.systemservice.impl.SystemServiceConst;
import com.ipvs.xmpp.client.smack.ft.FileTransfer;
import com.ipvs.xmpp.client.smack.ft.FileTransferManager;
import com.ipvs.xmpp.client.smack.ft.OutgoingFileTransfer;
import com.ipvs.xmpp.client.smack.ft.FileTransfer.Status;
import com.ipvs.xmpp.client.smack.ft.OutgoingFileTransfer.NegotiationProgress;


public class IPVSSendFile extends XMPPRequestHandler implements  NegotiationProgress{
    OutgoingFileTransfer oxfer;
    String myjid;
    String fileName=null;
    String fileXferID=null;
    String clientJID=null;
    MRLog log;
    String oldStatus;
    String newStatus;
    public void sendFile( FileTransferManager fxfer, String toJID, String fileName, String fileXferID) {
    	File pFile = new File(fileName);
    	log.addInfo("Sending File " + fileName + " To " + toJID);
    	this.clientJID = toJID;
    	this.fileName = fileName;
    	this.fileXferID = fileXferID;
    	oxfer = fxfer.createOutgoingFileTransfer(toJID);
        //MRLog.println("IPVSSendFile:sendFile:start " + fileName + ":" + fileXferID);
    	
  		oxfer.sendFile(fileName, pFile.length(),"outgoing transfer "+ fileXferID , this);
    }
    public boolean isDone() {
    	if (oxfer != null) return oxfer.isDone();
    	return false;
    }
    public void closeFile() {
        //MRLog.println("IPVSSendFile:closeFile" );       
    	if (oxfer == null) return;
    	log.addInfo("IPVSSendFile:closeFile" + oxfer.getStatus() + oxfer.getProgress() + " Done" + oxfer.isDone());
    	//MRLog.println("IPVSSendFile:closeFile" + oxfer.getStatus() + oxfer.getProgress() + " Done" + oxfer.isDone());    	
    	if (!oxfer.isDone()) {
        	log.addInfo("IPVSSendFile:closeFile Cancelling:" + oxfer.getStatus() +
    				"Size     :" + oxfer.getFileSize()+  " " +
    				"Written  :" + oxfer.getAmountWritten()+ " " + 
    				"Progress :" + oxfer.getProgress()*100 + "% " + 
    				"Error    :" + oxfer.getError() + " " + 
    				"Status   :" + oxfer.getStatus() + " " + 
    				"Done     :" + oxfer.isDone());
        	/*MRLog.println("IPVSSendFile:closeFile Cancelling:" + oxfer.getStatus() +
                    "Size     :" + oxfer.getFileSize()+  " " +
                    "Written  :" + oxfer.getAmountWritten()+ " " + 
                    "Progress :" + oxfer.getProgress()*100 + "% " + 
                    "Error    :" + oxfer.getError() + " " + 
                    "Status   :" + oxfer.getStatus() + " " + 
                    "Done     :" + oxfer.isDone());  */      	
    		oxfer.cancel();
    	}
    	oxfer=null;
    }
    public void checkStatus() {
    	if (oxfer==null) return;
    	log.addInfo("IPVSSendFile:checkStatus" + oxfer.getStatus() +
				"File     :" + oxfer.getFileName()+  " " +
				"Size     :" + oxfer.getFileSize()+  " " +
				"Written  :" + oxfer.getAmountWritten()+ " " + 
				"Progress :" + oxfer.getProgress()*100 + "% " + 
				"Error    :" + oxfer.getError() + " " + 
				"Status   :" + oxfer.getStatus() + " " + 
				" Done" + oxfer.isDone());
    }

    public void process(Packet packet) {
    }
    public void init(XMPPI xmpp, XMPPTransaction t, String myjid) {
	    this.myjid = xmpp.getXMPPUserJid();
        this.xmpp = xmpp;
        this.t = t;
        this.state = 0;
        this.log = MRLog.getInstance(MRLog.CONSOLE);
    }
	@Override
	public void errorEstablishingStream(Exception arg0) {
		log.addInfo("IPVSSendFile:errorEstablishingStream");
		MRLog.println("IPVSSendFile:errorEstablishingStream");
		try {
		    checkStatus(); 
		    arg0.printStackTrace();
        } catch (Exception e) {
        }
        try {
            Document FileTransferEventDoc = MRXMLUtils.loadXMLResource(SystemServiceConst.FILEXFER_EVENT_INSTANCE_XML, null);
            
            MRXMLUtils.setValueXpath(FileTransferEventDoc, "//FileXferID", fileXferID);
            MRXMLUtils.setValueXpath(FileTransferEventDoc, "//fromFileName", fileName);
            MRXMLUtils.setValueXpath(FileTransferEventDoc, "//toAgentJID", clientJID);
            MRXMLUtils.setValueXpath(FileTransferEventDoc, "//eventCode", FileTransferAgent.FILEXFERERROR);
            MRXMLUtils.setValueXpath(FileTransferEventDoc, "//percentageComplete", Double.toString(oxfer.getProgress()*100));
            FileSMUtils.addErrorToResponse(FileTransferEventDoc.getRootElement(), SystemServiceConst.FILETRANSFER_SEND_FILE_ERROR, "<error><scriptError>Error Establishing Stream fileXferID="+fileXferID +" fromFileName="+fileName+"</scriptError></error>")  ;
            ServiceAgentUtils.sendAgentEventMessage(xmpp, log, null, myjid, MRConst.FTP_SERVICE, FileTransferAgent.FILEXFEREVENT, FileTransferEventDoc.getRootElement(), 
                    "FileTransferAgent: Sending FileTransferRequest from " + SystemServiceConst.FILETRANSFERAGENT + " to " + SystemServiceConst.FILETRANSFERAGENT);
            ServiceAgentUtils.sendAgentEventMessage(xmpp, log, null, clientJID, MRConst.FTP_SERVICE, FileTransferAgent.FILEXFEREVENT, FileTransferEventDoc.getRootElement(), 
                    "FileTransferAgent: Sending FileTransferRequest from " + SystemServiceConst.FILETRANSFERAGENT + " to " + SystemServiceConst.FILETRANSFERAGENT);
            
        } catch (Exception e) {
            //MRLog.println("IPVSSendFile:errorEstablishingStream; error");
            e.printStackTrace();
        }		
	}

	@Override
	public void outputStreamEstablished(OutputStream arg0) {
		log.addInfo("outputStreamEstablished");
        //MRLog.println("IPVSSendFile:outputStreamEstablished");
	}

	@Override
	public void statusUpdated(Status oldStatus, Status newStatus) {
       	String eventCode="Unknown";
        //MRLog.println("IPVSSendFile:statusUpdated:"+oldStatus+":"+newStatus);
        this.oldStatus=oldStatus.toString();
        this.newStatus=newStatus.toString();
		if (newStatus.compareTo(FileTransfer.Status.complete)==0) {
			log.addInfo("Setting transaction to complete" + oxfer.getStatus() + oxfer.getProgress() + " " + oxfer.isDone());
			t.setResponse(null);
			//MRLog.println("IPVSSendFile:sendFile:end " + fileName + ":" + fileXferID);
			eventCode = FileTransferAgent.FILEXFERCOMPLETE;
		}
		else if ((newStatus.compareTo(FileTransfer.Status.error)==0) ||
				 (newStatus.compareTo(FileTransfer.Status.cancelled)==0) ||
				 (newStatus.compareTo(FileTransfer.Status.refused)==0)) {
			t.setException(new Exception("Send File Error"));
			eventCode = FileTransferAgent.FILEXFERERROR;
		}
		else if (newStatus.compareTo(FileTransfer.Status.in_progress)==0) {
				eventCode = FileTransferAgent.FILEXFERPROGRESS;
		}
		else if ((newStatus.compareTo(FileTransfer.Status.negotiating_stream)==0) ||
				(newStatus.compareTo(FileTransfer.Status.negotiating_transfer)==0)){
			eventCode = FileTransferAgent.FILEXFERNEGOTIATING;
		}
		else if (newStatus.compareTo(FileTransfer.Status.negotiated)==0) {
			eventCode = FileTransferAgent.FILEXFERNEGOTIATED;
        }
        else if (newStatus.compareTo(FileTransfer.Status.cancelled)==0) {
            eventCode = FileTransferAgent.FILEXFERERROR;
        }
        else if (newStatus.compareTo(FileTransfer.Status.refused)==0) {
            eventCode = FileTransferAgent.FILEXFERERROR;
		}
       	try {
           	Document FileTransferEventDoc = MRXMLUtils.loadXMLResource(SystemServiceConst.FILEXFER_EVENT_INSTANCE_XML, null);
           	
           	MRXMLUtils.setValueXpath(FileTransferEventDoc, "//FileXferID", fileXferID);
           	MRXMLUtils.setValueXpath(FileTransferEventDoc, "//fromFileName", fileName);
           	MRXMLUtils.setValueXpath(FileTransferEventDoc, "//toAgentJID", clientJID);
           	MRXMLUtils.setValueXpath(FileTransferEventDoc, "//eventCode", eventCode);
           	MRXMLUtils.setValueXpath(FileTransferEventDoc, "//percentageComplete", Double.toString(oxfer.getProgress()*100));
           	if(eventCode== FileTransferAgent.FILEXFERERROR){
           	 FileSMUtils.addErrorToResponse(FileTransferEventDoc.getRootElement(), SystemServiceConst.FILETRANSFER_SEND_FILE_ERROR, "<error><scriptError>fileXferID="+fileXferID +" fromFileName="+fileName+"</scriptError></error>")  ;
           	}

         	ServiceAgentUtils.sendAgentEventMessage(xmpp, log, null, myjid,MRConst.FTP_SERVICE, FileTransferAgent.FILEXFEREVENT, FileTransferEventDoc.getRootElement(), 
					"FileTransferAgent: Sending FileTransferRequest from " + SystemServiceConst.FILETRANSFERAGENT + " to " + SystemServiceConst.FILETRANSFERAGENT);
			ServiceAgentUtils.sendAgentEventMessage(xmpp, log, null, clientJID, MRConst.FTP_SERVICE, FileTransferAgent.FILEXFEREVENT, FileTransferEventDoc.getRootElement(), 
					"FileTransferAgent: Sending FileTransferRequest from " + SystemServiceConst.FILETRANSFERAGENT + " to " + SystemServiceConst.FILETRANSFERAGENT);
			
		} catch (Exception e) {
	        //MRLog.println("IPVSSendFile:statusUpdated: Error:"+oldStatus+":"+newStatus +" ");
			e.printStackTrace();
		}
	}
	public String toString(){
	    return fileXferID;
	}
}
