 package com.ipvs.xmpp.client.smack;




import java.io.File;

import org.jivesoftware.smack.XMPPException;
import org.jivesoftware.smack.packet.Packet;
import org.jivesoftware.smackx.packet.StreamInitiation;

import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.XMPPTransaction;
import com.ipvs.xmpp.client.smack.ft.FileTransferManager;
import com.ipvs.xmpp.client.smack.ft.FileTransferRequest;
import com.ipvs.xmpp.client.smack.ft.IncomingFileTransfer;


public class IPVSGetFile extends XMPPRequestHandler {
    private String myjid;
    private String fromJID;
    private String fileName;
    private String destFileName;
    private String fileXferID;
    private FileTransferManager fxfer;
    private IncomingFileTransfer transfer;
    public void GetFile( String fromJID, String fileName, String destFileName, String fileXferID) {
    	this.fromJID = fromJID;
    	this.fileName = fileName;
    	this.destFileName = destFileName;
    	this.fileXferID=fileXferID;
    }

    public void process(Packet packet) {
    	StreamInitiation si = (StreamInitiation)packet;
		FileTransferRequest request = new FileTransferRequest(fxfer, si);
    	//MRLog.println("IPVSGetFile:process "+ fromJID + " :" + fileName + " -> " + myjid + " :" + destFileName);
		transfer = request.accept();
		try{
            createFolders(destFileName);
        } catch (Exception e) {
        }		
		try {
			transfer.recieveFile(new File(destFileName));
		} catch (XMPPException e) {
		    //MRLog.println("IPVSGetFile:process error "+ fromJID + " :" + fileName + " -> " + myjid + " :" + destFileName);
			e.printStackTrace();
            checkStatus();
		}
    }
    private  void createFolders(String filepath){
        File file = new File(filepath);
        try {
            file.getParentFile().mkdirs();
        } catch (Exception e) {
        }
    }    
	public void init(XMPPI xmpp,FileTransferManager fxfer,XMPPTransaction t, String myjid) {
	    this.myjid = myjid;
        this.xmpp = xmpp;
        this.t = t;
        this.state = 0;
        this.fxfer = fxfer;
    }

	public void checkStatus() {
		if (transfer != null) {
		    MRLog.println("IPVSGetFile:checkStatus" + transfer.getStatus() +
				"File     :" + transfer.getFileName()+  " " +
				"Size     :" + transfer.getFileSize()+  " " +
				"Written  :" + transfer.getAmountWritten()+ " " + 
				"Progress :" + transfer.getProgress()*100 + "% " + 
				"Error    :" + transfer.getError() + " " + 
				"Status   :" + transfer.getStatus() + " " + 
				" Done" + transfer.isDone());
		}
		else {
		    MRLog.println("IPVSGetFile:checkStatus Error");
		}
	}
    public void closeFile() {
        MRLog.println("IPVSGetFile:closeFile" + transfer.getStatus() +
				transfer.getProgress() + " Done" + transfer.isDone());
    	if (!transfer.isDone()) {
    	    MRLog.println("IPVSGetFile:closeFile Cancelling" + transfer.getStatus() +
    				"Size     :" + transfer.getFileSize()+  " " +
    				"Written  :" + transfer.getAmountWritten()+ " " + 
    				"Progress :" + transfer.getProgress()*100 + "% " + 
    				"Error    :" + transfer.getError() + " " + 
    				"Status   :" + transfer.getStatus() + " " + 
    				" Done" + transfer.isDone());
    		transfer.cancel();
    	}
    	transfer=null;
    }
    public String toString(){
        return fileXferID;
    }
}
